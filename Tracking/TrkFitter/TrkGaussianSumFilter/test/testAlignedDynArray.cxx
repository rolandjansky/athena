/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "TrkGaussianSumFilter/AlignedDynArray.h"
#include <iostream>
#include <cstdint>
int
main()
{

  //
  //32 bit int has size 4 bytes
  using testType = int32_t;
  constexpr size_t alignement = 32; //32 byte alignment
  constexpr size_t n = 100;

   //begin should be aligned at 32 boundary
   //end (32+100*4)%32 ==> 16


  std::cout << "\n ==> Test ctor with default init of elements " << '\n';
  GSFUtils::AlignedDynArray<testType, alignement> test1(n);
  std::cout << "Size = " << test1.size() << '\n';
  std::cout << "Address of [0]%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(test1.buffer()) % alignement
            << '\n';
  std::cout << "Address of begin%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(test1.begin()) % alignement
            << '\n';
  std::cout << "Address of end%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(test1.end()) % alignement
            << '\n';
  std::cout << "Value of begin = " << *(test1.begin()) << '\n';
  std::cout << "Value of end -1 = " << *(test1.end() - 1) << '\n';

  std::cout << "\n ==> Test ctor with value init of elements" << '\n';
  GSFUtils::AlignedDynArray<testType, alignement> test2(n, 4);
  std::cout << "Size = " << test2.size() << '\n';
  std::cout << "Address of [0]%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(test2.buffer()) % alignement
            << '\n';
  std::cout << "Address of begin%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(test2.begin()) % alignement
            << '\n';
  std::cout << "Address of end%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(test2.end()) % alignement
            << '\n';
  std::cout << "Value of begin = " << *(test2.begin()) << '\n';
  std::cout << "Value of end -1 = " << *(test2.end() - 1) << '\n';

  std::cout << "\n ==> Test move copy ctor" << '\n';
  GSFUtils::AlignedDynArray<testType, alignement> test3(std::move(test2));
  std::cout << "Size after copy ctor = " << test3.size() <<'\n';
  std::cout << "Address of [0]%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(test3.buffer()) % alignement
            << '\n';
  std::cout << "Address of begin%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(test3.begin()) % alignement
            << '\n';
  std::cout << "Address of end%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(test3.end()) % alignement
            << '\n';
  std::cout << "Value of begin = " << *(test3.begin()) << '\n';
  std::cout << "Value of end -1 = " << *(test3.end() - 1) << '\n';

  std::cout << "\n ==> Test move assignment" << '\n';
  GSFUtils::AlignedDynArray<testType, alignement> test4(n / 2);
  std::cout << "Size before assignment = " << test4.size() << '\n';
  test4 = std::move(test3);
  std::cout << "Size after assignment " << test4.size() << '\n';
  std::cout << "Address of [0]%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(test4.buffer()) % alignement
            << '\n';
  std::cout << "Address of begin%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(test4.begin()) % alignement
            << '\n';
  std::cout << "Address of end%alignment = "
            << reinterpret_cast<std::ptrdiff_t>(test4.end()) % alignement
            << '\n';
  std::cout << "Value of begin = " << *(test4.begin()) << '\n';
  std::cout << "Value of end -1 = " << *(test4.end() - 1) << '\n';



  return 0;
}

