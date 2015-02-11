//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBPARTICLEIDCHERENKOVMUTAG
#define TBPARTICLEIDCHERENKOVMUTAG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

class TBPartIDCherenkovMuTag : public AthAlgorithm
{
 public:
  TBPartIDCherenkovMuTag(const std::string& name, ISvcLocator* pSvcLocator);
  ~TBPartIDCherenkovMuTag();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  std::string m_SGkeyscint,m_muonTagName,m_CherenkovName,m_particleKey;
  int m_muonADCcut, m_cherenkovADCcut;
  double m_useCherenkovBelow;

  bool m_printSummary;
  unsigned m_NElectrons,m_NPions,m_NMuons,m_Nunknown;
};

#endif
