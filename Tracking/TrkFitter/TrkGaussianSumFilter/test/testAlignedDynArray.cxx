/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "TrkGaussianSumFilter/AlignedDynArray.h"
#include <iostream>
int main()
{

  constexpr size_t alignement = 32;
  constexpr size_t n = 100;

  std::cout << "\n ==> Test ctor with default init of elements " << std::endl;
  GSFUtils::AlignedDynArray<double, alignement> test1(n);
  std::cout << "Size = " << test1.size()<< std::endl;
  std::cout << "Address of [0]%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(&test1[0]) % alignement << std::endl;
  std::cout << "Value of 0th = " << test1[0] << std::endl;
  std::cout << "Value of last = " << test1[99] << std::endl;

  std::cout << "\n ==> Test ctor with value init of elements" << std::endl;
  GSFUtils::AlignedDynArray<double, alignement> test2(n, 0.1);
  std::cout << "Size = " << test2.size()<< std::endl;
  std::cout << "Address of [0]%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(&test2[0]) % alignement << std::endl;
  std::cout << "Value of 0th = " << test2[0] << std::endl;
  std::cout << "Value of last = " << test2[99] << std::endl;

  std::cout << "\n ==> Test move copy ctor" << std::endl;
  GSFUtils::AlignedDynArray<double, alignement> test3(std::move(test2));
  std::cout << "Size = " << test3.size()<< std::endl;
  std::cout << "Size of moved from " << test2.size()<< std::endl;
  std::cout << "Address of [0]%alignment = "
    << reinterpret_cast<std::ptrdiff_t>(&test3[0]) % alignement << std::endl;
  std::cout << "Value of 0th = " << test3[0] << std::endl;
  std::cout << "Value of last = " << test3[99] << std::endl;

  std::cout << "\n ==> Test move assignment" << std::endl;
  GSFUtils::AlignedDynArray<double, alignement> test4(n/2);
  std::cout << "Size before assignment = " << test4.size()<< std::endl;
  test4=std::move(test3);
  std::cout << "Size = " << test4.size()<< std::endl;
  std::cout << "Size of moved from " << test3.size()<< std::endl;
  std::cout << "Address of [0]%alignment = "
    << reinterpret_cast<std::ptrdiff_t>(&test4[0]) % alignement << std::endl;
  std::cout << "Value of 0th = " << test4[0] << std::endl;
  std::cout << "Value of last = " << test4[99] << std::endl;

  return 0;
}

