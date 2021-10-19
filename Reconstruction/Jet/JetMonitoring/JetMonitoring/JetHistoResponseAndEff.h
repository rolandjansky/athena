// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETHISTORESPNSEEFFICIENCY_H
#define JETHISTORESPNSEEFFICIENCY_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "JetMonitoring/IJetHistoFiller.h"

////////////////////////////////////////////////////
/// \class Jethistoleadingjetsrelations
///
/// A IJetHistoFiller implementation in charge of efficiency and pT response
/// between the jets being monitored and truth jets (or any reference jets)
///
class JetHistoResponseAndEff : public AthAlgTool, virtual public IJetHistoFiller {
public:
  JetHistoResponseAndEff( const std::string& type,  const std::string & name ,const IInterface* parent);
  virtual ~JetHistoResponseAndEff(){}

  virtual StatusCode initialize() ;
  virtual StatusCode processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx) const;  
  
private:

  Gaudi::Property<std::string> m_group {this,"Group", "undefined"};
  Gaudi::Property<float> m_energyScale {this, "EnergyScale", 1.}; 
  SG::ReadHandleKey<xAOD::JetContainer> m_refContainerKey;

  
};
#endif
