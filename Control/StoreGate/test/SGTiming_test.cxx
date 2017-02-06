/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include <string>
#include <vector>
#include <chrono>
#include "TestTools/initGaudi.h"
using namespace Athena_test;
#include "TestTools/SGassert.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include "../src/SGHiveMgrSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SGtests.h"

#include "SGTiming_test_objs.h"
#include "SGTiming_test_def.inc"

using namespace std;

int main() {

  cout << "**** SGTimingTest BEGINS ****" << endl;

  ISvcLocator* pSvcLoc;
  initGaudi("SGTiming_test.txt", pSvcLoc);

  StoreGateSvc* pSG(0);
  assert( pSvcLoc->service("StoreGateSvc", pSG, true).isSuccess() );

  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

  std::vector<std::chrono::duration<double>> vd_rec, vd_ret, vd_clr;
  
  for (size_t i=0; i<NITER; ++i) {
    
#   include "SGTiming_test_ptr.inc"
    
    start = std::chrono::high_resolution_clock::now();
#   include "SGTiming_test_rec.inc"
    end = std::chrono::high_resolution_clock::now();
    vd_rec.push_back(end-start);

    start = std::chrono::high_resolution_clock::now();
#   include "SGTiming_test_ret.inc"
    end = std::chrono::high_resolution_clock::now();
    vd_ret.push_back(end-start);
        
#   include "SGTiming_test_chk.inc"

    start = std::chrono::high_resolution_clock::now();
    pSG->clearStore();
    end = std::chrono::high_resolution_clock::now();
    vd_clr.push_back(end-start);

    cout << "rec: " << setw(8) 
         << chrono::duration_cast<chrono::microseconds>(vd_rec[i]).count() 
         << "  ret: " << setw(8) 
         << chrono::duration_cast<chrono::microseconds>(vd_ret[i]).count() 
         << "  clr: " << setw(8)
         << chrono::duration_cast<chrono::microseconds>(vd_clr[i]).count() 
         << endl;

    
  }

  unsigned int a_ret{0}, a_rec{0}, a_clr{0};
  for (size_t i=1; i<NITER; ++i) {
    a_rec += chrono::duration_cast<chrono::microseconds>(vd_rec[i]).count();
    a_ret += chrono::duration_cast<chrono::microseconds>(vd_ret[i]).count();
    a_clr += chrono::duration_cast<chrono::microseconds>(vd_clr[i]).count();
  }
    
  cout << "===== averages  TYPES: " << NTYPES << "  KEYS: " << NKEYS << "  iter: " 
       << NITER << " ===============\n";

  cout << NTYPES << "/" << NKEYS << " ";
  cout << "rec: " << setw(6) << setprecision(2) << fixed
       << float(a_rec) / ((NITER-1) * NTYPES * NKEYS)
       << "  ret: " << setw(6)  
       << float(a_ret) / ((NITER-1) * NTYPES * NKEYS)
       << "  clr: " << setw(6)  
       << float(a_clr) / ((NITER-1) * NTYPES * NKEYS)
       << endl;
 
  cout << "**** SGTimingTest ENDS ****" << endl;

  return 0;

}
