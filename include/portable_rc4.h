#ifndef PORTABLE_RC4_H
#define PORTABLE_RC4_H

#include <cstddef>
#include <cstdint>
#include <utility>

struct RC4_KEY {
  std::uint32_t x;
  std::uint32_t y;
  std::uint8_t data[256];
};

inline void RC4_set_key(RC4_KEY *key, int len, const unsigned char *raw_key) {
  key->x = 0;
  key->y = 0;

  for (std::uint32_t i = 0; i < 256; ++i) {
    key->data[i] = static_cast<std::uint8_t>(i);
  }

  std::uint32_t j = 0;
  for (std::uint32_t i = 0; i < 256; ++i) {
    j = (j + key->data[i] + raw_key[i % len]) & 0xffU;
    std::swap(key->data[i], key->data[j]);
  }
}

inline void RC4(RC4_KEY *key, std::size_t len, const unsigned char *in, unsigned char *out) {
  std::uint32_t x = key->x;
  std::uint32_t y = key->y;

  for (std::size_t n = 0; n < len; ++n) {
    x = (x + 1U) & 0xffU;
    y = (y + key->data[x]) & 0xffU;
    std::swap(key->data[x], key->data[y]);
    const std::uint8_t idx = static_cast<std::uint8_t>((key->data[x] + key->data[y]) & 0xffU);
    out[n] = in[n] ^ key->data[idx];
  }

  key->x = x;
  key->y = y;
}

#endif
