

#include <cstdint>
#include <iostream>

uint64_t ab_modN(int a, int b, int n)
{
  if (!b)
    return 1;

  if (b & 1)
  {
    return (a * ab_modN(a, b - 1, n)) % n;
  }
  else
  {
    return ab_modN((a * a) % n, b / 2, n);
  }
}

static inline unsigned long long mul_mod(unsigned long long a, unsigned long long b,
                                         unsigned long long m)
{
  unsigned long long r = 0;
  while (b)
  {
    if (b & 1)
    {
      unsigned long long r2 = r + a;
      if (r2 < r)
        r2 -= m;
      r = r2 % m;
    }
    b >>= 1;
    if (b)
    {
      unsigned long long a2 = a + a;
      if (a2 < a)
        a2 -= m;
      a = a2 % m;
    }
  }
  return r;
}
static inline unsigned long long pow_mod(unsigned long long a, unsigned long long b,
                                         unsigned long long m)
{
  unsigned long long r = 1;
  while (b)
  {
    if (b & 1)
      r = mul_mod(r, a, m);
    b >>= 1;
    if (b)
      a = mul_mod(a, a, m);
  }
  return r;
}

unsigned sprp(unsigned long long prime_candidate, unsigned long long base)
{
  unsigned long long d = prime_candidate - 1;
  unsigned char s = 0;
  while (!(d & 0xff))
  {
    d >>= 8;
    s += 8;
  }
  if (!(d & 0xf))
  {
    d >>= 4;
    s += 4;
  }
  if (!(d & 0x3))
  {
    d >>= 2;
    s += 2;
  }
  if (!(d & 0x1))
  {
    d >>= 1;
    s += 1;
  }
  unsigned long long b = pow_mod(base, d, prime_candidate);
  if ((b == 1) || (b == (prime_candidate - 1)))
    return 1;
  unsigned char r;
  for (r = 1; r < s; r++)
  {
    b = mul_mod(b, b, prime_candidate);
    if (b <= 1)
      return 0;
    if (b == (prime_candidate - 1))
      return 1;
  }
  return 0;
}
unsigned is_prime(unsigned long long n)
{
  if (n < 2 || !(n & 1))
    return 0;
  if (n < 4)
    return 1;
  if (!sprp(n, 2))
    return 0;
  if (n < 2047)
    return 1;
  if (!sprp(n, 3))
    return 0;
  if (!sprp(n, 5))
    return 0;
  if (!sprp(n, 7))
    return 0;
  if (!sprp(n, 11))
    return 0;
  if (!sprp(n, 13))
    return 0;
  if (!sprp(n, 17))
    return 0;
  if (!sprp(n, 19))
    return 0;
  if (!sprp(n, 23))
    return 0;
  if (!sprp(n, 29))
    return 0;
  if (!sprp(n, 31))
    return 0;
  if (!sprp(n, 37))
    return 0;
  return 1;
}

int main()
{
  std::cout << is_prime(2) << '\n';
}
