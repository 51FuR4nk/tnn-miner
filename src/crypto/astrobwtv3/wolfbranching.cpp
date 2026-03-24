#include "astrobwtv3.h"
#include <inttypes.h>
#include <stdio.h>

// The base for the following code was contributed by @Wolf9466 on Discord

// Last instruction is a special case, and duplicated.
alignas(32) uint32_t CodeLUT[257] =
{
	0x090F020A, 0x060B0500, 0x09080609, 0x0A0D030B, 0x04070A01, 0x09030607, 0x060D0401, 0x000A0904,
	0x040F0F06, 0x030E070C, 0x04020D02, 0x0B0F050A, 0x0C020C04, 0x0B03070F, 0x07060206, 0x0C060501,
	0x0E020B04, 0x03020F04, 0x0E0D0B0F, 0x010F0600, 0x0503080C, 0x0B030005, 0x0608020B, 0x0D0B0905,
	0x00070E0F, 0x090D0A01, 0x02090008, 0x0F050E0F, 0x0600000F, 0x02030700, 0x050E0F06, 0x040C0602,
	0x0C080D0C, 0x0A0E0802, 0x01060601, 0x00040B03, 0x090B0C0B, 0x0A070702, 0x070D090A, 0x0C030705,
	0x0A030903, 0x0F010D0E, 0x0B0D0C0A, 0x05000501, 0x09090D0A, 0x0F0F0509, 0x09000F0E, 0x0F050F06,
	0x0A04040F, 0x0900080E, 0x080D000B, 0x030E0E0F, 0x0A070409, 0x00090E0E, 0x08030404, 0x080E0E0B,
	0x0C02040B, 0x0A0F0D08, 0x080C0500, 0x0B020A04, 0x0304020D, 0x0F060D0F, 0x05040C00, 0x0F090100,
	0x03080E02, 0x0F0D0C02, 0x0C080E0B, 0x0B090C0F, 0x05040E03, 0x00020807, 0x0302070E, 0x0F040206,
	0x08090306, 0x09080F01, 0x020D0805, 0x0209050E, 0x0A0C0F07, 0x0D000609, 0x0A080201, 0x0E0C0002,
	0x0A060005, 0x0E060A09, 0x03040407, 0x06080D08, 0x010B0600, 0x07030A06, 0x0E0A0E04, 0x000D0E00,
	0x0C0B0204, 0x0002040C, 0x080F0B07, 0x09050E08, 0x09040905, 0x0C020500, 0x0B0A0506, 0x0B040F0F,
	0x0C0C090B, 0x0B060907, 0x0E06070E, 0x0E010807, 0x0A060809, 0x07090704, 0x0D01000D, 0x0B08030A,
	0x08090F00, 0x060D0A0C, 0x080E0B02, 0x070C0F0B, 0x0304050C, 0x020A030C, 0x000C0C07, 0x02080207,
	0x0D040F01, 0x0F0B0904, 0x0B080A04, 0x0A0F050D, 0x05030906, 0x060D0605, 0x0700060F, 0x080C0403,
	0x0C020308, 0x07000902, 0x0E0A0F0C, 0x05040D0D, 0x0C0C0304, 0x080C0007, 0x0D0B0F08, 0x06020503,
	0x0A0C0C0F, 0x04090907, 0x070A0B0E, 0x010B0902, 0x05080F0C, 0x030F0C06, 0x040E0B05, 0x070C0008,
	0x0701030F, 0x0F07080A, 0x03030001, 0x0F0D0C0D, 0x0B0C030F, 0x0B010900, 0x050F080C, 0x050D0706,
	0x0A06040A, 0x080E0C0E, 0x05060509, 0x04060E02, 0x050F0601, 0x03080100, 0x06060605, 0x00060206,
	0x0704060C, 0x0B0D0404, 0x0F040309, 0x01030903, 0x07070D0B, 0x07060A0B, 0x090D000B, 0x01030A03,
	0x07080B0D, 0x03030F0A, 0x02080C01, 0x06010E0B, 0x02090104, 0x0E030600, 0x0D000C04, 0x04040207,
	0x0A050A0B, 0x0B060E05, 0x01080102, 0x0D010908, 0x0E01060B, 0x04060200, 0x040A0909, 0x0D01020F,
	0x0302030F, 0x090C0C05, 0x0500040B, 0x0C000708, 0x070E0301, 0x04060C0F, 0x030B0F0E, 0x00010102,
	0x06020F03, 0x040E0F07, 0x0C0E0107, 0x0304000D, 0x0E090E0E, 0x0F0E0301, 0x0F07050C, 0x000D0A07,
	0x00060002, 0x05060A0B, 0x050A0605, 0x090C030E, 0x0D08060B, 0x0E0A0202, 0x0707080B, 0x04000203,
	0x07090808, 0x0D0C0E04, 0x03040A0F, 0x03050B0A, 0x0F0C0A03, 0x090E0600, 0x0E080809, 0x0F0D0909,
	0x0000070D, 0x0F080901, 0x0C0A0F04, 0x0E00010A, 0x0A0C0303, 0x00060D01, 0x03010704, 0x03050602,
	0x0A040105, 0x0F000B0E, 0x08040201, 0x0E0D0508, 0x0B060806, 0x0F030408, 0x07060302, 0x0D030A01,
	0x0C0B0D06, 0x0407080D, 0x08010203, 0x04060105, 0x00070009, 0x0D0A0C09, 0x02050A0A, 0x0D070308,
	0x02020E0F, 0x0B090D09, 0x05020703, 0x0C020D04, 0x03000501, 0x0F060C0D, 0x00000D01, 0x0F0B0205,
	0x04000506, 0x0E09030B, 0x00000103, 0x0F0C090B, 0x040C080F, 0x010F0C07, 0x000B0700, 0x0F0C0F04,
	0x0401090F, 0x080E0E0A, 0x050A090E, 0x0009080C, 0x080E0C06, 0x0D0C030D, 0x090D0C0D, 0x090D0C0D,
  0x00000000
};

alignas(32) uint16_t *CodeLUT_16 = (uint16_t*)malloc_huge_pages(257*4);

void initWolfLUT() {
  for (int i = 0; i < 256; i++) {
    uint16_t t = 0;
    uint32_t opcode = CodeLUT[i];
    for (int j = 3; j >= 0; j--) {
      uint8_t insn = (opcode >> (j << 3)) & 0xFF;
      t |= (insn << (j << 2));
    }
    CodeLUT_16[i] = t;
  }

  // printf("%02X\n", CodeLUT_16[0]);
}

#if defined(__x86_64)

__attribute__((target("avx,avx2")))
void wolfBranch_avx2(__m256i &in, uint8_t pos2val, uint32_t opcode, workerData &worker)
{
  const __m256i vec_3_local = _mm256_set1_epi8(3);

  for (int i = 3; i >= 0; --i)
  {
    uint8_t insn = (opcode >> (i << 2)) & 0xF;
    switch (insn)
    {
    case 0:
      in = _mm256_add_epi8(in, in);
      break;
    case 1:
      in = _mm256_sub_epi8(in, _mm256_xor_si256(in, _mm256_set1_epi8(97)));
      break;
    case 2:
      in = _mm256_mul_epi8(in, in);
      break;
    case 3:
      in = _mm256_xor_si256(in, _mm256_set1_epi8(pos2val));
      break;
    case 4:
      in = _mm256_xor_si256(in, _mm256_set1_epi64x(-1LL));
      break;
    case 5:
      in = _mm256_and_si256(in, _mm256_set1_epi8(pos2val));
      break;
    case 6:
      in = _mm256_sllv_epi8(in, _mm256_and_si256(in, vec_3_local));
      break;
    case 7:
      in = _mm256_srlv_epi8(in, _mm256_and_si256(in, vec_3_local));
      break;
    case 8:
      in = _mm256_reverse_epi8(in);
      break;
    case 9:
      in = _mm256_xor_si256(in, popcnt256_epi8(in));
      break;
    case 10:
      in = _mm256_rolv_epi8(in, in);
      break;
    case 11:
      in = _mm256_rol_epi8(in, 1);
      break;
    case 12:
      in = _mm256_xor_si256(in, _mm256_rol_epi8(in, 2));
      break;
    case 13:
      in = _mm256_rol_epi8(in, 3);
      break;
    case 14:
      in = _mm256_xor_si256(in, _mm256_rol_epi8(in, 4));
      break;
    case 15:
      in = _mm256_rol_epi8(in, 5);
      break;
    }      
  }
}

#if defined(__x86_64)
__attribute__((target("sse2,ssse3,sse4.1")))
static inline __m128i genMask_sse41(int bytes)
{
  const __m128i sequence = _mm_setr_epi8(
      0, 1, 2, 3, 4, 5, 6, 7,
      8, 9, 10, 11, 12, 13, 14, 15);
  bytes = (bytes < 0) ? 0 : (bytes > 16) ? 16 : bytes;
  return _mm_cmpgt_epi8(_mm_set1_epi8(static_cast<char>(bytes)), sequence);
}

__attribute__((target("sse2,ssse3,sse4.1")))
static inline __m128i sllv_epi8_sse41(__m128i a, __m128i count)
{
  const __m128i mask_hi = _mm_set1_epi32(0xFF00FF00);
  const __m128i multiplier_lut = _mm_setr_epi8(
      0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, static_cast<char>(0x80),
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

  __m128i count_sat = _mm_min_epu8(count, _mm_set1_epi8(8));
  __m128i multiplier = _mm_shuffle_epi8(multiplier_lut, count_sat);
  __m128i x_lo = _mm_mullo_epi16(a, multiplier);
  __m128i multiplier_hi = _mm_srli_epi16(multiplier, 8);
  __m128i a_hi = _mm_and_si128(a, mask_hi);
  __m128i x_hi = _mm_mullo_epi16(a_hi, multiplier_hi);

  return _mm_blendv_epi8(x_lo, x_hi, mask_hi);
}

__attribute__((target("sse2,ssse3,sse4.1")))
static inline __m128i srlv_epi8_sse41(__m128i a, __m128i count)
{
  const __m128i mask_hi = _mm_set1_epi32(0xFF00FF00);
  const __m128i multiplier_lut = _mm_setr_epi8(
      static_cast<char>(0x80), 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

  __m128i count_sat = _mm_min_epu8(count, _mm_set1_epi8(8));
  __m128i multiplier = _mm_shuffle_epi8(multiplier_lut, count_sat);
  __m128i a_lo = _mm_andnot_si128(mask_hi, a);
  __m128i multiplier_lo = _mm_andnot_si128(mask_hi, multiplier);
  __m128i x_lo = _mm_mullo_epi16(a_lo, multiplier_lo);
  x_lo = _mm_srli_epi16(x_lo, 7);

  __m128i multiplier_hi = _mm_and_si128(mask_hi, multiplier);
  __m128i x_hi = _mm_mulhi_epu16(a, multiplier_hi);
  x_hi = _mm_slli_epi16(x_hi, 1);

  return _mm_blendv_epi8(x_lo, x_hi, mask_hi);
}

__attribute__((target("sse2,ssse3,sse4.1")))
static inline __m128i rolv_epi8_sse41(__m128i x, __m128i y)
{
  __m128i y_mod = _mm_and_si128(y, _mm_set1_epi8(7));
  __m128i left_shift = sllv_epi8_sse41(x, y_mod);
  __m128i right_shift_counts = _mm_sub_epi8(_mm_set1_epi8(8), y_mod);
  __m128i right_shift = srlv_epi8_sse41(x, right_shift_counts);
  return _mm_or_si128(left_shift, right_shift);
}

__attribute__((target("sse2,ssse3,sse4.1")))
static inline __m128i rol_epi8_sse41(__m128i x, int r)
{
  const __m128i mask1 = _mm_set1_epi16(0x00FF);
  const __m128i mask2 = _mm_set1_epi16(0xFF00);
  __m128i a = _mm_and_si128(x, mask1);
  __m128i b = _mm_and_si128(x, mask2);

  __m128i shiftedA = _mm_slli_epi16(a, r);
  __m128i wrappedA = _mm_srli_epi16(a, 8 - r);
  __m128i rotatedA = _mm_and_si128(_mm_or_si128(shiftedA, wrappedA), mask1);

  __m128i shiftedB = _mm_slli_epi16(b, r);
  __m128i wrappedB = _mm_srli_epi16(b, 8 - r);
  __m128i rotatedB = _mm_and_si128(_mm_or_si128(shiftedB, wrappedB), mask2);

  return _mm_or_si128(rotatedA, rotatedB);
}

__attribute__((target("sse2,ssse3,sse4.1")))
static inline __m128i reverse_epi8_sse41(__m128i input)
{
  const __m128i lookup = _mm_setr_epi8(
      0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE,
      0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF);
  const __m128i low_mask = _mm_set1_epi8(0x0F);

  __m128i low = _mm_and_si128(input, low_mask);
  __m128i high = _mm_and_si128(_mm_srli_epi16(input, 4), low_mask);
  __m128i rev_low = _mm_shuffle_epi8(lookup, low);
  __m128i rev_high = _mm_shuffle_epi8(lookup, high);

  return _mm_or_si128(_mm_slli_epi16(rev_low, 4), rev_high);
}

__attribute__((target("sse2,ssse3,sse4.1")))
static inline __m128i popcnt_epi8_sse41(__m128i input)
{
  return parallelPopcnt16bytes(input);
}

__attribute__((target("sse2,ssse3,sse4.1")))
static inline void wolfBranch_sse41(__m128i &in, uint8_t pos2val, uint32_t opcode, workerData &worker)
{
  (void) worker;
  const __m128i vec_3_local = _mm_set1_epi8(3);

  for (int i = 3; i >= 0; --i)
  {
    uint8_t insn = (opcode >> (i << 2)) & 0xF;
    switch (insn)
    {
    case 0:
      in = _mm_add_epi8(in, in);
      break;
    case 1:
      in = _mm_sub_epi8(in, _mm_xor_si128(in, _mm_set1_epi8(97)));
      break;
    case 2:
      in = mullo_epi8(in, in);
      break;
    case 3:
      in = _mm_xor_si128(in, _mm_set1_epi8(static_cast<char>(pos2val)));
      break;
    case 4:
      in = _mm_xor_si128(in, _mm_set1_epi8(static_cast<char>(0xFF)));
      break;
    case 5:
      in = _mm_and_si128(in, _mm_set1_epi8(static_cast<char>(pos2val)));
      break;
    case 6:
      in = sllv_epi8_sse41(in, _mm_and_si128(in, vec_3_local));
      break;
    case 7:
      in = srlv_epi8_sse41(in, _mm_and_si128(in, vec_3_local));
      break;
    case 8:
      in = reverse_epi8_sse41(in);
      break;
    case 9:
      in = _mm_xor_si128(in, popcnt_epi8_sse41(in));
      break;
    case 10:
      in = rolv_epi8_sse41(in, in);
      break;
    case 11:
      in = rol_epi8_sse41(in, 1);
      break;
    case 12:
      in = _mm_xor_si128(in, rol_epi8_sse41(in, 2));
      break;
    case 13:
      in = rol_epi8_sse41(in, 3);
      break;
    case 14:
      in = _mm_xor_si128(in, rol_epi8_sse41(in, 4));
      break;
    case 15:
      in = rol_epi8_sse41(in, 5);
      break;
    }
  }
}
#endif

#endif

uint8_t wolfBranch(uint8_t val, uint8_t pos2val, uint32_t opcode)
{
  for (int i = 3; i >= 0; --i)
  {
    uint8_t insn = (opcode >> (i << 3)) & 0xFF;
    switch (insn)
    {
    case 0:
      val += val;
      break;
    case 1:
      val -= (val ^ 97);
      break;
    case 2:
      val *= val;
      break;
    case 3:
      val ^= pos2val;
      break;
    case 4:
      val = ~val;
      break;
    case 5:
      val &= pos2val;
      break;
    case 6:
      val <<= (val & 3);
      break;
    case 7:
      val >>= (val & 3);
      break;
    case 8:
      val = (((val & 0xAA) >> 1) | ((val & 0x55) << 1));
      val = (((val & 0xCC) >> 2) | ((val & 0x33) << 2));
      val = (((val & 0xF0) >> 4) | ((val & 0x0F) << 4));
      break;
    case 9:
      val ^= (uint8_t)__builtin_popcount(val);
      break;
    case 10:
      val = rl8(val, val);
      break;
    case 11:
      val = rl8(val, 1);
      break;
    case 12:
      val ^= rl8(val, 2);
      break;
    case 13:
      val = rl8(val, 3);
      break;
    case 14:
      val ^= rl8(val, 4);
      break;
    case 15:
      val = rl8(val, 5);
      break;
    }
  }

  return (val);
}

#if defined(__x86_64)
__attribute__((target("sse2,ssse3,sse4.1")))
void wolfPermute_sse41(uint8_t *in, uint8_t *out, uint16_t op, uint8_t pos1, uint8_t pos2, workerData &worker)
{
  uint32_t Opcode = CodeLUT_16[op];
  int bytes_remaining = pos2 - pos1;
  int offset = 0;

  while (bytes_remaining >= 16) {
    __m128i data = _mm_loadu_si128((__m128i*)&in[pos1 + offset]);
    __m128i old = data;

    wolfBranch_sse41(data, in[pos2], Opcode, worker);
    data = _mm_blendv_epi8(old, data, genMask_sse41(16));

    _mm_storeu_si128((__m128i*)&out[pos1 + offset], data);
    bytes_remaining -= 16;
    offset += 16;
  }

  for (int i = pos1 + offset; i < pos2; ++i) {
    out[i] = wolfBranch(in[i], in[pos2], CodeLUT[op]);
  }
}

__attribute__((target("avx512f,avx512bw,avx512vl")))
void wolfPermute_avx512(uint8_t *in, uint8_t *out, uint16_t op, uint8_t pos1, uint8_t pos2, workerData &worker)
{
  uint32_t Opcode = CodeLUT_16[op];
  
  __m256i data = _mm256_loadu_si256((__m256i*)&in[pos1]);
  wolfBranch_avx2(data, in[pos2], Opcode, worker);
  
  int bytes_to_update = pos2 - pos1;
  __mmask32 mask = _cvtu32_mask32((1U << bytes_to_update) - 1);
  
  _mm256_mask_storeu_epi8((void*)&out[pos1], mask, data);
}

__attribute__((target("avx,avx2")))
void wolfPermute_avx2(uint8_t *in, uint8_t *out, uint16_t op, uint8_t pos1, uint8_t pos2, workerData &worker)
{
	uint32_t Opcode = CodeLUT_16[op];

  __m256i data = _mm256_loadu_si256((__m256i*)&in[pos1]);
  __m256i old = data;

  wolfBranch_avx2(data, in[pos2], Opcode, worker);
  data = _mm256_blendv_epi8(old, data, genMask_avx2(pos2-pos1));

  _mm256_storeu_si256((__m256i*)&out[pos1], data);
}
#endif

void wolfPermute(uint8_t *in, uint8_t *out, uint16_t op, uint8_t pos1, uint8_t pos2, workerData &worker)
{
  uint32_t Opcode = CodeLUT[op];
  for (int i = pos1; i < pos2; ++i)
  {
    out[i] = wolfBranch(in[i], in[pos2], Opcode);
  }
}
