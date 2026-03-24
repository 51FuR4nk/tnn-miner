#ifndef DEBUG_STAGE_H
#define DEBUG_STAGE_H

#include <atomic>
#include <cstdint>

extern thread_local const char *tnnDebugStage;
extern bool astroProfileEnabled;
extern std::atomic<std::uint64_t> astroProfileCalls;
extern std::atomic<std::uint64_t> astroProfilePreNs;
extern std::atomic<std::uint64_t> astroProfileComputeNs;
extern std::atomic<std::uint64_t> astroProfileSuffixNs;
extern std::atomic<std::uint64_t> astroProfileFinalNs;

inline void setDebugStage(const char *stage) {
  tnnDebugStage = stage;
}

inline void recordAstroProfile(std::uint64_t pre_ns,
                               std::uint64_t compute_ns,
                               std::uint64_t suffix_ns,
                               std::uint64_t final_ns) {
  if (!astroProfileEnabled) {
    return;
  }

  astroProfileCalls.fetch_add(1, std::memory_order_relaxed);
  astroProfilePreNs.fetch_add(pre_ns, std::memory_order_relaxed);
  astroProfileComputeNs.fetch_add(compute_ns, std::memory_order_relaxed);
  astroProfileSuffixNs.fetch_add(suffix_ns, std::memory_order_relaxed);
  astroProfileFinalNs.fetch_add(final_ns, std::memory_order_relaxed);
}

#endif
