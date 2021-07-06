/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RNDTEST_H
#define RNDTEST_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include <GaudiKernel/ITHistSvc.h>
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include <string>
#include <vector>
#include "TH1.h"

namespace AtlasCLHEP_RandomGenerators {

  class AtlasCLHEP_RandomGenerators_test : public AthAlgorithm {

  public:
    /** Standard Athena-Algorithm Constructor */
    AtlasCLHEP_RandomGenerators_test(const std::string& name, ISvcLocator* pSvcLocator);
    ~AtlasCLHEP_RandomGenerators_test();
    
    StatusCode          initialize();
    StatusCode          execute();
    StatusCode          finalize();

  private:
    /**  central crono service*/
    IChronoStatSvc*                     m_chrono;

    CLHEP::HepRandomEngine*             m_ranlux64;
    CLHEP::HepRandomEngine*             m_ranecu;
    CLHEP::HepRandomEngine*             m_mtwist;
    ITHistSvc*                          m_histSvc;
    
    ServiceHandle<IAtRndmGenSvc>        m_rndmSvc1;
    CLHEP::HepRandomEngine*             m_randomEngine1;
    std::string                         m_randomEngineName1;   
    ServiceHandle<IAtRndmGenSvc>        m_rndmSvc2;
    CLHEP::HepRandomEngine*             m_randomEngine2;
    std::string                         m_randomEngineName2; 
    
    TH1* m_hflat=nullptr;
      
    TH1* m_hgauss1=nullptr;  
    TH1* m_hgauss2=nullptr;  
    TH1* m_hgauss3=nullptr;  

    TH1* m_hexp1=nullptr;  
    TH1* m_hexp2=nullptr;  

    TH1* m_hbin1=nullptr;  
    TH1* m_hbin2=nullptr;  

    int m_ntest=10000000;
  };

} // end of namespace bracket
#endif


















