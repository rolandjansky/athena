/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <chrono>
#include <iostream>

using namespace std::chrono;

struct Timer{

  void timeit(){
 
    auto t0 = high_resolution_clock::now();
    int ndo = 1000000;
    int i{0};
    for (int i = 0; i != ndo; ++i){
      i += 1;
      i -= 1;
    }
    auto t1 = high_resolution_clock::now();
    std::cout << i << '\n';
    std::cout << duration_cast<milliseconds>(t1-t0).count()
	      <<"ms\n";
  }
};

int main(){
  Timer timer;
  timer.timeit();
}
