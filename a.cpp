#include "cl_hash.hpp"
#include <iostream>

int main(int argc, char *argv[])
{


  std::string s;
  std::cin >> s;
  std::cout << clhash::wyhash_(s) << '\n';

  return 0;
}
