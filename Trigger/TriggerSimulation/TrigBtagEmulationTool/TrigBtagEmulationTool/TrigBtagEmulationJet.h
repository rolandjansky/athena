/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBTAGEMULATION_JET_H
#define TRIGBTAGEMULATION_JET_H

#include <iostream>
#include <memory>
#include <map>
#include "xAODJet/JetContainer.h"
#include "xAODTrigger/JetRoIContainer.h"

namespace Trig {

  class TrigBtagEmulationJet {
  public:
    TrigBtagEmulationJet();
    TrigBtagEmulationJet(const xAOD::Jet*);
    TrigBtagEmulationJet(const xAOD::JetRoI*,bool isJJ = false);
    TrigBtagEmulationJet(const TrigBtagEmulationJet&);
    ~TrigBtagEmulationJet();

    double pt() const;
    double eta() const;
    double phi() const;

    double weights(std::string) const;
    void weights(std::string,double);
    std::map< std::string,double > weights() const;

    xAOD::Jet* jet() const;

  protected:
    std::unique_ptr< xAOD::Jet > m_jet;
    std::unique_ptr< xAOD::JetRoI > m_jetRoI;

    double m_pt;
    double m_eta;    
    double m_phi;

    std::map< std::string , double > m_weights;
  };

  // *****

  bool sortEmulationJetByPt( Trig::TrigBtagEmulationJet* , Trig::TrigBtagEmulationJet* );

} //namespace

#endif
