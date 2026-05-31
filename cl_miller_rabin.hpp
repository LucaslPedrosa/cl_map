
#pragma once

#include <bit>
#include <concepts>
#include <type_traits>

using u64 = unsigned long long;
using u32 = unsigned int;
using u128 = __uint128_t;


template <class T>
concept UInt = std::unsigned_integral<T> && !std::same_as<T, bool> && sizeof(T) <= sizeof(u64);


template <UInt T>
T mult_mod(T a, T b, T mod)
{
  if constexpr (sizeof(T) <= 4)
  {
    return static_cast<T>((static_cast<u64>(a) * b) % mod);
  }
  else
  {
    return static_cast<T>((static_cast<u128>(a) * b) % mod);
  }

}

// Assumes the base is already reduced
template <UInt T>
inline T pow_mod_exp(T base, T exp, T mod)
{
  T result = 1;

  while (exp > 0)
  {
    if (exp & 1)
    {
      result = mult_mod(result, base, mod);
    }

    exp >>= 1;
  }
  return result;
}

template <UInt T>
bool miller_test(T base, T prime_candidate, int t_zeros, T exp)
{
  if (base >= prime_candidate)
    base %= prime_candidate;
  if (!base)
    return true;

  T component = prime_candidate - 1;
  T rabin = pow_mod_exp(base, exp, prime_candidate);

  [[unlikely]]
  if (rabin == 1 || rabin == component)
    return true;

  for (int r = 1; r < t_zeros; r++)
  {
    rabin = mult_mod(rabin, rabin, prime_candidate);
    if (rabin == component)
      return true;
    if (rabin == 1)
      return false;
  }
  return false;
}


template <UInt T>
bool is_prime(T prime_candidate)
{
  [[unlikely]]
  if (prime_candidate <= 1)
    return false;
  [[unlikely]]
  if (prime_candidate == 2 || prime_candidate == 3 || prime_candidate == 5)
    return true;
  if (prime_candidate % 2 == 0 || prime_candidate % 3 == 0 || prime_candidate % 5 == 0)
    return false;

  // (Jim Sinclair)
  static constexpr T bases[] = {T{2},      T{325},     T{9375},      T{28178},
                                T{450775}, T{9780504}, T{1795265022}};
  static constexpr T small_bases[] = {T{2}, T{7}, T{61}};

  // U32
  int t_zeros = std::countr_zero(prime_candidate - 1);
  T exp = (prime_candidate - 1) >> t_zeros;

  [[likely]]
  if (prime_candidate < 4'759'123'141ULL)
  {
    for (T base : small_bases)
    {
      if (!miller_test(base, prime_candidate, t_zeros, exp))
      {
        return false;
      }
    }
    return true;
  }


  // U64
  for (T base : bases)
  {
    if (!miller_test(base, prime_candidate, t_zeros, exp))
    {
      return false;
    }
  }


  return true;
}

template <std::signed_integral T>
bool is_prime(T n)
{
  if (n < 0)
    return false;

  using U = std::make_unsigned_t<T>;
  return is_prime(static_cast<U>(n));
}

extern template bool is_prime<u32>(u32);
extern template bool is_prime<u64>(u64);
extern template bool is_prime<int>(int);
extern template bool is_prime<long long>(long long);

#if 0
template bool is_prime<u32>(u32);
template bool is_prime<u64>(u64);
template bool is_prime<int>(int);
template bool is_prime<long long>(long long);
#endif
