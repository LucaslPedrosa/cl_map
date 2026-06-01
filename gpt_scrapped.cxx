#ifndef WYHASH_MINIMAL_H
#define WYHASH_MINIMAL_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef WYHASH_CONDOM
#define WYHASH_CONDOM 1
#endif

#if defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#pragma intrinsic(_umul128)
#endif

#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
#define _likely_(x) __builtin_expect((x), 1)
#define _unlikely_(x) __builtin_expect((x), 0)
#else
#define _likely_(x) (x)
#define _unlikely_(x) (x)
#endif

static inline void _wymum(uint64_t *A, uint64_t *B) {
#if defined(__SIZEOF_INT128__)
  __uint128_t r = *A;
  r *= *B;

#if WYHASH_CONDOM > 1
  *A ^= (uint64_t)r;
  *B ^= (uint64_t)(r >> 64);
#else
  *A = (uint64_t)r;
  *B = (uint64_t)(r >> 64);
#endif

#elif defined(_MSC_VER) && defined(_M_X64)
#if WYHASH_CONDOM > 1
  uint64_t high;
  uint64_t low = _umul128(*A, *B, &high);
  *A ^= low;
  *B ^= high;
#else
  *A = _umul128(*A, *B, B);
#endif

#else
  uint64_t ha = *A >> 32;
  uint64_t hb = *B >> 32;
  uint64_t la = (uint32_t)*A;
  uint64_t lb = (uint32_t)*B;

  uint64_t rh = ha * hb;
  uint64_t rm0 = ha * lb;
  uint64_t rm1 = hb * la;
  uint64_t rl = la * lb;

  uint64_t t = rl + (rm0 << 32);
  uint64_t c = t < rl;

  uint64_t lo = t + (rm1 << 32);
  c += lo < t;

  uint64_t hi = rh + (rm0 >> 32) + (rm1 >> 32) + c;

#if WYHASH_CONDOM > 1
  *A ^= lo;
  *B ^= hi;
#else
  *A = lo;
  *B = hi;
#endif
#endif
}

static inline uint64_t _wymix(uint64_t A, uint64_t B) {
  _wymum(&A, &B);
  return A ^ B;
}

static inline uint64_t _wyr8(const uint8_t *p) {
  uint64_t v;
  memcpy(&v, p, 8);
  return v;
}

static inline uint64_t _wyr4(const uint8_t *p) {
  uint32_t v;
  memcpy(&v, p, 4);
  return v;
}

static inline uint64_t _wyr3(const uint8_t *p, size_t k) {
  return (((uint64_t)p[0]) << 16) | (((uint64_t)p[k >> 1]) << 8) | p[k - 1];
}

static const uint64_t _wyp[4] = {0x2d358dccaa6c78a5ull, 0x8bb84b93962eacc9ull,
                                 0x4b33a62ed433d4a3ull, 0x4d5a2da51de1aa47ull};

static inline uint64_t wyhash_with_secret(const void *key, size_t len,
                                          uint64_t seed,
                                          const uint64_t *secret) {
  const uint8_t *p = (const uint8_t *)key;

  seed ^= _wymix(seed ^ secret[0], secret[1]);

  uint64_t a;
  uint64_t b;

  if (_likely_(len <= 16)) {
    if (_likely_(len >= 4)) {
      a = (_wyr4(p) << 32) | _wyr4(p + ((len >> 3) << 2));
      b = (_wyr4(p + len - 4) << 32) | _wyr4(p + len - 4 - ((len >> 3) << 2));
    } else if (_likely_(len > 0)) {
      a = _wyr3(p, len);
      b = 0;
    } else {
      a = 0;
      b = 0;
    }
  } else {
    size_t i = len;

    if (_unlikely_(i >= 48)) {
      uint64_t see1 = seed;
      uint64_t see2 = seed;

      do {
        seed = _wymix(_wyr8(p) ^ secret[1], _wyr8(p + 8) ^ seed);
        see1 = _wymix(_wyr8(p + 16) ^ secret[2], _wyr8(p + 24) ^ see1);
        see2 = _wymix(_wyr8(p + 32) ^ secret[3], _wyr8(p + 40) ^ see2);

        p += 48;
        i -= 48;
      } while (_likely_(i >= 48));

      seed ^= see1 ^ see2;
    }

    while (_unlikely_(i > 16)) {
      seed = _wymix(_wyr8(p) ^ secret[1], _wyr8(p + 8) ^ seed);
      i -= 16;
      p += 16;
    }

    a = _wyr8(p + i - 16);
    b = _wyr8(p + i - 8);
  }

  a ^= secret[1];
  b ^= seed;

  _wymum(&a, &b);

  return _wymix(a ^ secret[0] ^ len, b ^ secret[1]);
}

static inline uint64_t wyhash(const void *key, size_t len, uint64_t seed) {
  return wyhash_with_secret(key, len, seed, _wyp);
}

#endif
