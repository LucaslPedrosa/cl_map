
#include "cl_rapidhash.hpp"
#include <iostream>

int main(int argc, char *argv[])
{


  std::string s;
  std::cin >> s;
  std::cout << clhash::rapidhash_(s) << '\n';

  return 0;
}
