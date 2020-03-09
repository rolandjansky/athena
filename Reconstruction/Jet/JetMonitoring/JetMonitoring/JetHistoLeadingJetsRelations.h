// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETHISTOLEADINGJETSRELATIONS_H
#define JETHISTOLEADINGJETSRELATIONS_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "JetMonitoring/IJetHistoFiller.h"
#include "JetMonitoring/JetHistoVarTool.h"

////////////////////////////////////////////////////
/// \class Jethistoleadingjetsrelations
///
/// A IJetHistoFiller implementation in charge of calculating
/// angular variables between leading and subleading jets in an event.
/// If njet<2, no variable are calculated (and thus no histo entry filled).
///
class JetHistoLeadingJetsRelations : public AthAlgTool, virtual public IJetHistoFiller {
public:
  //LeadingJetsRelationsFiller();
  // : public AthAlgTool, public IJetHIstoFiller();
  JetHistoLeadingJetsRelations( const std::string& type,  const std::string & name ,const IInterface* parent);
  virtual ~JetHistoLeadingJetsRelations(){}

  virtual StatusCode initialize() ;
  virtual StatusCode processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx) const;  

  
private:


  Gaudi::Property<std::string> m_group {this,"Group", "undefined"};
  
};
#endif
