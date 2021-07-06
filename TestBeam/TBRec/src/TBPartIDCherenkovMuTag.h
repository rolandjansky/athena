//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBPARTICLEIDCHERENKOVMUTAG_H
#define TBREC_TBPARTICLEIDCHERENKOVMUTAG_H

#include "AthenaBaseComps/AthAlgorithm.h"

class TBPartIDCherenkovMuTag : public AthAlgorithm
{
 public:
  TBPartIDCherenkovMuTag(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TBPartIDCherenkovMuTag();
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
 private:
  std::string m_SGkeyscint,m_muonTagName,m_CherenkovName,m_particleKey;
  int m_muonADCcut, m_cherenkovADCcut;
  double m_useCherenkovBelow;

  bool m_printSummary;
  unsigned m_NElectrons,m_NPions,m_NMuons,m_Nunknown;
};

#endif
