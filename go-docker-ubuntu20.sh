#!/bin/bash

set -euo pipefail

tgt="${1:-amd64}"
PACKAGE_VERSION="${2:-0.0.0}"
EXTRA_CMAKE_ARGS="${3:-}"

valid_targets=("amd64" "arm64")
chk="\\<${tgt}\\>"
if [[ ! "${valid_targets[*]}" =~ ${chk} ]]; then
  echo "Invalid target specified: ${tgt}"
  exit 1
fi

DEFAULT_CMAKE_ARGS="-DWITH_HIP=OFF -DUSE_CPM_BOOST=ON"
if [[ -n "${EXTRA_CMAKE_ARGS}" ]]; then
  CMAKE_ARGS="${DEFAULT_CMAKE_ARGS} ${EXTRA_CMAKE_ARGS}"
else
  CMAKE_ARGS="${DEFAULT_CMAKE_ARGS}"
fi

OUT_DIR="${OUT_DIR:-./export-ubuntu20}"
TARGZ_FILE="${TARGZ_FILE:-tnn-miner-ubuntu20-${tgt}-${PACKAGE_VERSION}.tar.gz}"
RUN_TESTS="${RUN_TESTS:-ON}"
LLVM_VERSION="${LLVM_VERSION:-18}"
CPU_ARCHTARGET="${CPU_ARCHTARGET:-}"
ASTRO_SPSA_MODE="${ASTRO_SPSA_MODE:-auto}"

case "${ASTRO_SPSA_MODE}" in
  auto)
    if [[ -n "${CPU_ARCHTARGET}" && "${CPU_ARCHTARGET}" != "x86-64-v3" && "${tgt}" == "amd64" ]]; then
      CMAKE_ARGS="${CMAKE_ARGS} -DUSE_ASTRO_SPSA=OFF"
    fi
    ;;
  on)
    CMAKE_ARGS="${CMAKE_ARGS} -DUSE_ASTRO_SPSA=ON"
    ;;
  off)
    CMAKE_ARGS="${CMAKE_ARGS} -DUSE_ASTRO_SPSA=OFF"
    ;;
  *)
    echo "Invalid ASTRO_SPSA_MODE: ${ASTRO_SPSA_MODE} (expected: auto, on, off)"
    exit 1
    ;;
esac

if [[ "${ASTRO_SPSA_MODE}" != "auto" ]]; then
  echo "ASTRO_SPSA_MODE=${ASTRO_SPSA_MODE}"
fi

mkdir -p "${OUT_DIR}"

docker buildx build \
  --platform="linux/${tgt}" \
  --build-arg "LLVM_VERSION=${LLVM_VERSION}" \
  --build-arg "PACKAGE_VERSION=${PACKAGE_VERSION}" \
  --build-arg "TARGZ_FILE=${TARGZ_FILE}" \
  --build-arg "RUN_TESTS=${RUN_TESTS}" \
  --build-arg "CPU_ARCHTARGET=${CPU_ARCHTARGET}" \
  --build-arg "CMAKE_ARGS=${CMAKE_ARGS}" \
  -f docker/Dockerfile.ubuntu20 \
  . \
  --output "${OUT_DIR}"
