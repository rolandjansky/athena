/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


This is tool is a demonstrator and is used in testing/developing the jet monitoring system.
It's not meant to be used in useful monitoring jobs.

*/

#ifndef JETMJETPTHISTOTOOL_H
#define JETMJETPTHISTOTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "JetMonitoring/IJetHistoFiller.h"



class JetHistoPtTool : public AthAlgTool, virtual public IJetHistoFiller {
public:
  JetHistoPtTool( const std::string& type,  const std::string & name ,const IInterface* parent);
  virtual ~JetHistoPtTool(){}

  virtual StatusCode processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ) const ;  

  
private:


  Gaudi::Property<std::string> m_group {this,"Group", "undefined"};
  
};
#endif
