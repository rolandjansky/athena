/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTransportModelTool.h"

#include "ForwardTransportModel.h"

ForwardTransportModelTool::ForwardTransportModelTool(const std::string& type, const std::string& name, const IInterface *parent)
  : FastSimulationBase(type,name,parent)
{
  declareInterface<IFastSimulation>(this);
  declareProperty("ForwardTransportSvcName", m_FwdTrSvcName="ForwardTransportSvc", "");
}

G4VFastSimulationModel* ForwardTransportModelTool::makeFastSimModel()
{
  ATH_MSG_DEBUG( "Initializing Fast Sim Model" );

  int verboseLevel(0);
  if(msgLvl(MSG::VERBOSE)) verboseLevel = 10;
  else if(msgLvl(MSG::DEBUG)) verboseLevel = 5;
  // Create a fresh Fast Sim Model
  return new ForwardTransportModel(name(), verboseLevel, m_FwdTrSvcName);

}
