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
 inline std::vector<T> FlattenInput2D_1D( std::vector<std::vector<T>> features, int size){
    std::vector<float> Flatten1D(size);
    for(std::size_t r=0;r<features.size();++r){
      int n_cols = features[r].size();
      for(int c=0;c<n_cols;++c){
        float a = float(features[r][c]);
        Flatten1D[r*n_cols+c] = a;
      }
    }
   return Flatten1D;
  }

}
#endif
