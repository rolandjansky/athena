/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_PRINTCONTAINER_H
#define FPTRACKER_PRINTCONTAINER_H
#include <iostream>
namespace FPTracker{
template<class Container>
  void printCont(const Container& arr, int perline, int nele){
    int  linecount = 0;
    for(int i = 0; i<nele; ++i){
      std::cout<<" "<<arr[i];
      if(++linecount == perline){
	linecount = 0;
	std::cout<<'\n';
      }
    }
    std::cout<<'\n';
 }
}
#endif
