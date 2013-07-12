/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PmbCxxUtils/CustomBenchmark.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cmath>

//Example: Assume that we are bench-marking some simulation code, and
//we would like to know time spent as a function of pdg (just the
//absolute pdg value, and any |pdg|>99 we wrap to 99).

int main(int,char**)
{
  srand(0);
  const unsigned maxpdgrecord=99;
  PMonUtils::CustomBenchmark bench(maxpdgrecord+1);//Create bench object, allowing to track 100 unique ids (0..99).

  double dummy(17.0);
  for (unsigned i=0;i<100;++i) {
    int pdg = -150+(rand()%30)*10;
    int id = std::min<unsigned>(maxpdgrecord,abs(pdg));//map pdg to a number in 0..99
    bench.begin(id);
    //Fake work begin
    for (unsigned j=0;j<3000*(abs(pdg)+1);++j)
      dummy = sin(dummy);
    //Fake work end
    bench.end();
  }

  //Exactly the same loop once again, to show how to use the Guard mini helper:
  for (unsigned i=0;i<100;++i) {
    int pdg = -150+(rand()%30)*10;
    int id = std::min<unsigned>(maxpdgrecord,abs(pdg));//map pdg to a number in 0..99
    {
      PMonUtils::CustomBenchmarkGuard cbg(&bench,id);
      //Fake work begin
      for (unsigned j=0;j<3000*(abs(pdg)+1);++j)
	dummy = sin(dummy);//Even if an exception is thrown here, the destructor of the cbg object will invoke bench.end() for us.
      //Fake work end
    }
  }

  //Print out the results:

  for (unsigned pdg = 0; pdg<=maxpdgrecord; ++pdg) {
    uint64_t count; double time_ms;
    bench.getData(pdg, count, time_ms);
    if (count)//Only report on those encountered at least once
      std::cout<< "Work done on particle with |pdg|="<<pdg<<(pdg==maxpdgrecord?"+":"")
               << " was done "<<count<<" times and took a total of "<<time_ms
               <<" milliseconds [on average "<<time_ms/count<<" ms each time]"<<std::endl;
  }
}
