/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  This is an algorithm to monitor the HT (scalar sum of jet pT) of jet containers

*/

#ifndef JETHISTOHTFILLER_H
#define JETHISTOHTFILLER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "JetMonitoring/IJetHistoFiller.h"


class JetHistoHTFiller : public AthAlgTool, virtual public IJetHistoFiller {
public:
  JetHistoHTFiller( const std::string& type,  const std::string & name ,const IInterface* parent);

  virtual StatusCode initialize() ;
  virtual StatusCode finalize() ;
  virtual StatusCode processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx) const;  
  
private:

  Gaudi::Property<std::string> m_group {this,"Group" ,      "undefined"};
  Gaudi::Property<float> m_minPt       {this,"MinPt" , 30., "Minimum pT cut on jets"};
  Gaudi::Property<float> m_maxEta      {this,"MaxEta", 3.2, "Maximum absolute eta cut on jets"};
  bool m_failureOnMissingContainer;
};
#endif
