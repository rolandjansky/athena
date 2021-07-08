// Dear emacs, this is -*- c++ -*-
// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#ifndef FlattenInput_H
#define FlattenInput_H

#include <string>
#include <iostream> 
#include <fstream>
#include <arpa/inet.h>
#include <vector>
#include <iterator>


namespace AthONNX {
 template<typename T>

 inline std::vector<T> FlattenInput_multiD_1D( std::vector<std::vector<T>> features){
    // 1. Compute the total size required.
    int total_size = 0;
    for (auto& feature : features) total_size += feature.size();
    
    // 2. Create a vector to hold the data.
    std::vector<T> Flatten1D;
    Flatten1D.reserve(total_size);

    // 3. Fill it
    for (auto& feature : features)
      for (auto& elem : feature)
        Flatten1D.push_back(elem);
 
   return Flatten1D;
  }

}
#endif
