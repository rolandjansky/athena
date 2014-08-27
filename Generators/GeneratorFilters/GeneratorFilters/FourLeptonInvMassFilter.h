/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
    @class GeneratorFilters/FourLeptonInvMassFilter.h
    
    Filter on 4lepton-mass in the mass windows (by default >60GeV)
    
    - Apply Pt and Eta cuts on leptons.  Default is Pt > 5 GeV and |eta| < 5
    
    based on FourLeptonMassFilter by 
    @author Theodota Lagouri Theodota Lagouri <theodota.lagouri@cern.ch>
    @author Konstantinos Nikolopoulos <konstantinos.nikolopoulos@cern.ch>
    
    @author Antonio Salvucci <antonio.salvucci@cern.ch>
*/

#ifndef GENERATORFILTERFOUREPTONINVMASSFILTER  
#define GENERATORFILTERFOUREPTONINVMASSFILTER        

#include "GeneratorModules/GenFilter.h"

class FourLeptonInvMassFilter:public GenFilter {
public:
  FourLeptonInvMassFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FourLeptonInvMassFilter();
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();
  
private:
  double m_minPt;
  double m_maxEta;
  double m_minMass;
  double m_maxMass;
};

#endif
