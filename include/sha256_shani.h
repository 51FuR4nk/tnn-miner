#pragma once
/*
 * SHA-256 using SHA-NI intrinsics with software prefetching.
 * Optimized for Goldmont (in-order) CPUs with SHA extension.
 *
 * Prefetch-ahead distance is tuned for DRAM latency hiding on
 * in-order cores with small L2 and no L3 (e.g., J3455).
 */

#if defined(__x86_64__)

#include <immintrin.h>
#include <string.h>
#include <stdint.h>

/* SHA-256 round constants, 4 at a time for _mm_add_epi32 */
alignas(16) static const uint32_t K256_SHANI[][4] = {
  {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5},
  {0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5},
  {0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3},
  {0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174},
  {0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc},
  {0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da},
  {0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7},
  {0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967},
  {0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13},
  {0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85},
  {0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3},
  {0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070},
  {0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5},
  {0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3},
  {0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208},
  {0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2},
};

/* Byte-swap mask for big-endian message words */
static const __m128i SHANI_BSWAP_MASK =
    (__m128i)(__v2di){0x0405060700010203LL, 0x0c0d0e0f08090a0bLL};

/*
 * Process one 64-byte SHA-256 block using SHA-NI.
 * STATE0 holds {A,B,E,F}, STATE1 holds {C,D,G,H} in shuffled order.
 */
__attribute__((target("sha,sse4.1,ssse3")))
static inline void sha256_shani_block(__m128i &STATE0, __m128i &STATE1,
                                      const uint8_t *block) {
  __m128i ABEF_SAVE = STATE0;
  __m128i CDGH_SAVE = STATE1;
  __m128i MSG0, MSG1, MSG2, MSG3, TMP;

  /* Load and byte-swap message words */
  MSG0 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)(block + 0)),  SHANI_BSWAP_MASK);
  MSG1 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)(block + 16)), SHANI_BSWAP_MASK);
  MSG2 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)(block + 32)), SHANI_BSWAP_MASK);
  MSG3 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)(block + 48)), SHANI_BSWAP_MASK);

  /* Rounds 0-3 */
  TMP = _mm_add_epi32(MSG0, _mm_load_si128((const __m128i *)K256_SHANI[0]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

  /* Rounds 4-7 */
  TMP = _mm_add_epi32(MSG1, _mm_load_si128((const __m128i *)K256_SHANI[1]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

  /* Rounds 8-11 */
  TMP = _mm_add_epi32(MSG2, _mm_load_si128((const __m128i *)K256_SHANI[2]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

  /* Rounds 12-15 */
  TMP = _mm_add_epi32(MSG3, _mm_load_si128((const __m128i *)K256_SHANI[3]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
  MSG0 = _mm_add_epi32(MSG0, TMP);
  MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
  MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

  /* Rounds 16-19 */
  TMP = _mm_add_epi32(MSG0, _mm_load_si128((const __m128i *)K256_SHANI[4]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
  MSG1 = _mm_add_epi32(MSG1, TMP);
  MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
  MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

  /* Rounds 20-23 */
  TMP = _mm_add_epi32(MSG1, _mm_load_si128((const __m128i *)K256_SHANI[5]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
  MSG2 = _mm_add_epi32(MSG2, TMP);
  MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
  MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

  /* Rounds 24-27 */
  TMP = _mm_add_epi32(MSG2, _mm_load_si128((const __m128i *)K256_SHANI[6]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
  MSG3 = _mm_add_epi32(MSG3, TMP);
  MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
  MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

  /* Rounds 28-31 */
  TMP = _mm_add_epi32(MSG3, _mm_load_si128((const __m128i *)K256_SHANI[7]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
  MSG0 = _mm_add_epi32(MSG0, TMP);
  MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
  MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

  /* Rounds 32-35 */
  TMP = _mm_add_epi32(MSG0, _mm_load_si128((const __m128i *)K256_SHANI[8]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
  MSG1 = _mm_add_epi32(MSG1, TMP);
  MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
  MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

  /* Rounds 36-39 */
  TMP = _mm_add_epi32(MSG1, _mm_load_si128((const __m128i *)K256_SHANI[9]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
  MSG2 = _mm_add_epi32(MSG2, TMP);
  MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
  MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

  /* Rounds 40-43 */
  TMP = _mm_add_epi32(MSG2, _mm_load_si128((const __m128i *)K256_SHANI[10]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
  MSG3 = _mm_add_epi32(MSG3, TMP);
  MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
  MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

  /* Rounds 44-47 */
  TMP = _mm_add_epi32(MSG3, _mm_load_si128((const __m128i *)K256_SHANI[11]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
  MSG0 = _mm_add_epi32(MSG0, TMP);
  MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
  MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

  /* Rounds 48-51 */
  TMP = _mm_add_epi32(MSG0, _mm_load_si128((const __m128i *)K256_SHANI[12]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
  MSG1 = _mm_add_epi32(MSG1, TMP);
  MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);

  /* Rounds 52-55 */
  TMP = _mm_add_epi32(MSG1, _mm_load_si128((const __m128i *)K256_SHANI[13]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
  MSG2 = _mm_add_epi32(MSG2, TMP);
  MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);

  /* Rounds 56-59 */
  TMP = _mm_add_epi32(MSG2, _mm_load_si128((const __m128i *)K256_SHANI[14]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);
  TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
  MSG3 = _mm_add_epi32(MSG3, TMP);
  MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);

  /* Rounds 60-63 */
  TMP = _mm_add_epi32(MSG3, _mm_load_si128((const __m128i *)K256_SHANI[15]));
  STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, TMP);
  TMP = _mm_shuffle_epi32(TMP, 0x0E);
  STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, TMP);

  /* Accumulate */
  STATE0 = _mm_add_epi32(STATE0, ABEF_SAVE);
  STATE1 = _mm_add_epi32(STATE1, CDGH_SAVE);
}

/*
 * Full SHA-256 hash with software prefetching.
 * Prefetch distance: 1024 bytes (16 cache lines) ahead of current block.
 * This hides DRAM latency on in-order cores without L3.
 */
__attribute__((target("sha,sse4.1,ssse3")))
static inline void sha256_shani(const uint8_t *data, size_t len,
                                uint8_t digest[32]) {
  /* Initial hash values H0..H7, packed for SHA-NI state layout */
  /* STATE0 = {H0(A), H1(B), H4(E), H5(F)} after shuffle */
  /* STATE1 = {H2(C), H3(D), H6(G), H7(H)} after shuffle */
  __m128i STATE0, STATE1, TMP;

  TMP    = _mm_set_epi32(0x6a09e667, 0xbb67ae85, 0x510e527f, 0x9b05688c);
  STATE1 = _mm_set_epi32(0x3c6ef372, 0xa54ff53a, 0x1f83d9ab, 0x5be0cd19);

  /* SHA-NI state layout: CDGH in STATE0, ABEF in STATE1 initially,
     then we shuffle to get the right interleaving */
  TMP    = _mm_shuffle_epi32(TMP, 0xB1);    /* CDAB */
  STATE1 = _mm_shuffle_epi32(STATE1, 0x1B);  /* EFGH */
  STATE0 = _mm_alignr_epi8(TMP, STATE1, 8); /* ABEF */
  STATE1 = _mm_blend_epi16(STATE1, TMP, 0xF0); /* CDGH */

  /* Process complete 64-byte blocks */
  size_t nblocks = len >> 6;
  const uint8_t *p = data;

  /* Prefetch first 1KB to warm L2 before we start */
  for (size_t i = 0; i < 16 && i * 64 < len; ++i)
    _mm_prefetch((const char *)(p + i * 64), _MM_HINT_T0);

  for (size_t i = 0; i < nblocks; ++i, p += 64) {
    /* Prefetch 1024 bytes ahead (16 blocks) */
    _mm_prefetch((const char *)(p + 1024), _MM_HINT_T0);
    _mm_prefetch((const char *)(p + 1088), _MM_HINT_T0);

    sha256_shani_block(STATE0, STATE1, p);
  }

  /* Handle final block(s) with padding */
  uint8_t lastblocks[128];
  size_t tail = len & 63;
  memcpy(lastblocks, p, tail);
  lastblocks[tail] = 0x80;
  memset(lastblocks + tail + 1, 0, sizeof(lastblocks) - tail - 1);

  size_t padblocks;
  if (tail >= 56) {
    /* Need two padding blocks */
    padblocks = 2;
  } else {
    padblocks = 1;
  }
  /* Store length in bits as big-endian 64-bit at end of last block */
  uint64_t bitlen = (uint64_t)len << 3;
  uint8_t *lenp = lastblocks + padblocks * 64 - 8;
  lenp[0] = (uint8_t)(bitlen >> 56);
  lenp[1] = (uint8_t)(bitlen >> 48);
  lenp[2] = (uint8_t)(bitlen >> 40);
  lenp[3] = (uint8_t)(bitlen >> 32);
  lenp[4] = (uint8_t)(bitlen >> 24);
  lenp[5] = (uint8_t)(bitlen >> 16);
  lenp[6] = (uint8_t)(bitlen >> 8);
  lenp[7] = (uint8_t)(bitlen);

  for (size_t i = 0; i < padblocks; ++i)
    sha256_shani_block(STATE0, STATE1, lastblocks + i * 64);

  /* Extract digest: un-shuffle state back to H0..H7 order */
  TMP    = _mm_shuffle_epi32(STATE0, 0x1B); /* FEBA */
  STATE1 = _mm_shuffle_epi32(STATE1, 0xB1); /* DCHG */
  STATE0 = _mm_blend_epi16(TMP, STATE1, 0xF0); /* DCBA = H0 H1 H2 H3 */
  STATE1 = _mm_alignr_epi8(STATE1, TMP, 8);    /* HGFE = H4 H5 H6 H7 */

  /* Byte-swap to big-endian and store */
  STATE0 = _mm_shuffle_epi8(STATE0, SHANI_BSWAP_MASK);
  STATE1 = _mm_shuffle_epi8(STATE1, SHANI_BSWAP_MASK);
  _mm_storeu_si128((__m128i *)(digest + 0),  STATE0);
  _mm_storeu_si128((__m128i *)(digest + 16), STATE1);
}

#endif /* __x86_64__ */
