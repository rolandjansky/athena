// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMATCHEDFILLER_H
#define JETMATCHEDFILLER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "JetMonitoring/IJetHistoFiller.h"
#include "JetMonitoring/JetHistoVarTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"

class JetHistoMatchedFiller : public AthAlgTool, virtual public IJetHistoFiller {
public:
  JetHistoMatchedFiller( const std::string& type,  const std::string & name ,const IInterface* parent);

  virtual StatusCode initialize() ;
  virtual StatusCode processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx) const;  
  
private:

  Gaudi::Property<std::string> m_group {this,"Group", "undefined"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_matchedKey{this, "matched", "matched", "SG key for input matched decoration"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_ptdiffKey{this, "ptdiff", "ptdiff", "SG key for input matched ptdiff decoration"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_energydiffKey{this, "energydiff", "energydiff", "SG key for input matched energydiff decoration"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_massdiffKey{this, "massdiff", "massdiff", "SG key for input matched massdiff decoration"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_ptrespKey{this, "ptresp", "ptresp", "SG key for input matched ptresp decoration"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_energyrespKey{this, "energyresp", "energyresp", "SG key for input matched energyresp decoration"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_massrespKey{this, "massresp", "massresp", "SG key for input matched massresp decoration"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_ptrefKey{this, "ptref", "ptref", "SG key for input matched ptref decoration"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_etarefKey{this, "etaref", "etaref", "SG key for input matched etaref decoration"};
  
};
#endif
