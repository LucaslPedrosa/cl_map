#include <bit>
#include <concepts>

using u64 = unsigned long long;
using u128 = __uint128_t;

template <std::unsigned_integral T>
T mult_mod(T a, T b, T mod)
{
  return static_cast<T>((static_cast<u128>(a) * b % mod));
}


template <std::unsigned_integral T>
inline T pow_mod_exp(T base, T exp, T mod)
{
  T result = 1 % mod;
  base %= mod;
  while (exp > 0)
  {
    if (exp & 1)
    {
      result = mult_mod(result, base, mod);
    }

    base = mult_mod(base, base, mod);
    exp >>= 1;

  }
  return result;
}

template <std::unsigned_integral T>
bool miller_(T base, T prime_candidate)
{

  T component = prime_candidate - 1;
  T t_zeros = std::countr_zero(component);
  T exp = component >> t_zeros;
  T rabin = pow_mod_exp(base, exp, prime_candidate);

  if (rabin == 1 || rabin == component)
    return 1;

  for (int i = 0; i < t_zeros; i++)
  {
    rabin = mult_mod(rabin, rabin, prime_candidate);
    if (rabin == 1)
      return 0;
    if (rabin == component)
      return 1;
  }
  return 1;
}


template <typename T>
bool is_prime(T prime_candidate)
{
  if (prime_candidate <= 1)
    return 0;
  if (prime_candidate == 2)
    return 1;
  if (!(prime_candidate & 1))
    return 0;
  if (!millet_(2, prime_candidate))
  {
    return 0;
  }
  if (!millet_(3, prime_candidate))
  {
    return 0;
  }
  if (!millet_(5, prime_candidate))
  {
    return 0;
  }
  if (!millet_(7, prime_candidate))
  {
    return 0;
  }
  if (!millet_(11, prime_candidate))
  {
    return 0;
  }
  if (!millet_(13, prime_candidate))
  {
    return 0;
  }
  if (!millet_(17, prime_candidate))
  {
    return 0;
  }
  if (!millet_(19, prime_candidate))
  {
    return 0;
  }
  if (!millet_(23, prime_candidate))
  {
    return 0;
  }
  if (!millet_(29, prime_candidate))
  {
    return 0;
  }
  if (!millet_(31, prime_candidate))
  {
    return 0;
  }
  if (!millet_(37, prime_candidate))
  {
    return 0;
  }


  return 1;
}


int main()
{
  bool v = miller_(2u, 17u);
}
