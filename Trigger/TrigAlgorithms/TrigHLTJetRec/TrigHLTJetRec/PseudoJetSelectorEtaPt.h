/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_PSEUDOJETSELECTORETAPT_H
#define TRIGHLTJETREC_PSEUDOJETSELECTORETAPT_H

#include <cmath>
#include "TrigHLTJetRec/IPseudoJetSelector.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "fastjet/PseudoJet.hh"

class StatusCode;
using jet::PseudoJetVector;

class PseudoJetSelectorEtaPt: virtual public IPseudoJetSelector, 
  public AthAlgTool {

public:

  PseudoJetSelectorEtaPt(const std::string&,
                         const std::string&,
                         const IInterface*);

  ~PseudoJetSelectorEtaPt(){};
  
  StatusCode initialize();
  StatusCode select(PseudoJetVector&) const override;
 private:

  class PseudoJetRemover{
  public:

  PseudoJetRemover():m_etaMax(0), m_ptMin(0){}
  PseudoJetRemover(double etaMax, double ptMin):
    m_etaMax(etaMax), m_ptMin(ptMin){}

    bool operator()(const fastjet::PseudoJet& pj) const{
      if (std::abs(pj.eta()) > m_etaMax){return true;}
      if (pj.pt() < m_ptMin){return true;}
      return false;}

    private:
      double m_etaMax;
      double m_ptMin;
    };

  double m_etaMax;
  double m_ptMin;
  PseudoJetRemover m_remover;
};

#endif
