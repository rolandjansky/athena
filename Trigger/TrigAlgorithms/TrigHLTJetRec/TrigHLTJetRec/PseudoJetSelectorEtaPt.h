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
  StatusCode select(const PseudoJetVector&,
                    PseudoJetVector&) const override;
 private:

  class PseudoJetSelector{
  public:

  PseudoJetSelector():m_etaMax(0), m_ptMin(0){}
  PseudoJetSelector(double etaMax, double ptMin):
    m_etaMax(etaMax), m_ptMin(ptMin){}

    bool operator()(const fastjet::PseudoJet& pj) const{
      if (std::abs(pj.eta()) > m_etaMax){return false;}
      if (pj.pt() < m_ptMin){return false;}
      return true;}

    private:
      double m_etaMax;
      double m_ptMin;
    };

  double m_etaMax;
  double m_ptMin;
  PseudoJetSelector m_selector;
};

#endif
