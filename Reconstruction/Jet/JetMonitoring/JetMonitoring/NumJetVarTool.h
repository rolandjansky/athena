/* this file is -*- C++ -*- 
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_NUMJETVARTOOL_H
#define JETMONITORING_NUMJETVARTOOL_H

#include "JetMonitoring/EventHistoVarTool.h"
#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

///////////////////////////////////////////////////////////
/// \class NumJetVarTool
///
/// This class is a simple tool to count the jet multiplicity
/// using the existing EventHistoVarTool and JetEventSelector
///

class NumJetVarTool : public AthAlgTool, virtual public IEventHistoVarTool {
public:
  NumJetVarTool(const std::string & type, const std::string & name, const IInterface* parent);
  virtual ~NumJetVarTool(){}

  virtual StatusCode initialize() ;  

  virtual float value(const xAOD::EventInfo &, const xAOD::JetContainer&) const;
  virtual std::string describe() const {return "NJets with pT cut";}
  
private:

  Gaudi::Property<float> m_pTcut = {this,"JetMinPtCut", 0.};
  bool m_failureOnMissingContainer;

};


#endif
