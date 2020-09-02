/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include <string>
#include <vector>
#include <chrono>
#include "TestTools/initGaudi.h"
#include "TestTools/SGassert.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include "../src/SGHiveMgrSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SGtests.h"

#include "SGTiming_test_objs.h"
#include "SGTiming_test_def.inc"

int main() {
  std::cout << "**** SGTimingTest BEGINS ****" << std::endl;

  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("StoreGate/SGTiming_test.txt", pSvcLoc)) {
    return 1;
  }

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
    pSG->clearStore().ignore();
    end = std::chrono::high_resolution_clock::now();
    vd_clr.push_back(end-start);

    std::cout << "rec: " << std::setw(8) 
              << std::chrono::duration_cast<std::chrono::microseconds>(vd_rec[i]).count() 
              << "  ret: " << std::setw(8) 
              << std::chrono::duration_cast<std::chrono::microseconds>(vd_ret[i]).count() 
              << "  clr: " << std::setw(8)
              << std::chrono::duration_cast<std::chrono::microseconds>(vd_clr[i]).count() 
              << std::endl;

    
  }

  unsigned int a_ret{0}, a_rec{0}, a_clr{0};
  for (size_t i=1; i<NITER; ++i) {
    a_rec += std::chrono::duration_cast<std::chrono::microseconds>(vd_rec[i]).count();
    a_ret += std::chrono::duration_cast<std::chrono::microseconds>(vd_ret[i]).count();
    a_clr += std::chrono::duration_cast<std::chrono::microseconds>(vd_clr[i]).count();
  }
  
  std::cout << "===== averages  TYPES: " << NTYPES << "  KEYS: " << NKEYS << "  iter: " 
            << NITER << " ===============\n";

  std::cout << NTYPES << "/" << NKEYS << " ";
  std::cout << "rec: " << std::setw(6) << std::setprecision(2) << std::fixed
            << float(a_rec) / ((NITER-1) * NTYPES * NKEYS)
            << "  ret: " << std::setw(6)  
            << float(a_ret) / ((NITER-1) * NTYPES * NKEYS)
            << "  clr: " << std::setw(6)  
            << float(a_clr) / ((NITER-1) * NTYPES * NKEYS)
            << std::endl;
 
  std::cout << "**** SGTimingTest ENDS ****" << std::endl;

  return 0;

}
