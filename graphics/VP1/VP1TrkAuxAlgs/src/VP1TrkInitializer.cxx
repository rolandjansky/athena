/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1TrkAuxAlgs/VP1TrkInitializer.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Utils/VP1ToolAccessHelper.h"
#include "VP1Utils/VP1AvailableToolsHelper.h"

#include "GaudiKernel/IToolSvc.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"

//____________________________________________________________________
VP1TrkInitializer::VP1TrkInitializer(const std::string& name, ISvcLocator* svcLocator):
  AthAlgorithm(name, svcLocator),
  m_toolSvc(0)
{
  // Tool initialization
  declareProperty("ForceExtrapolatorTools",m_forceExtrapolatorTools=false);
  declareProperty("ForceFitterTools",m_forceFitterTools=false);
}

//____________________________________________________________________
VP1TrkInitializer::~VP1TrkInitializer()
{
}

//____________________________________________________________________
StatusCode VP1TrkInitializer::initialize()
{
  msg(MSG::INFO) << " in initialize() " << endreq;

  //ToolSvc
  StatusCode status = service("ToolSvc",m_toolSvc);
  if (status.isFailure()||!m_toolSvc) {
    msg(MSG::ERROR) << " Unable to get ToolSvc!" << endreq;
    return status;
  }

  // Initialize tools (if needed)
  if(m_forceExtrapolatorTools) {
    QStringList tooltypes;
    tooltypes << "Trk::Extrapolator";
    initTools<Trk::IExtrapolator>(tooltypes,QString("VP1_JOBCFG_EXTRA_VP1_EXTRAPOLATORS"));
  }

  if(m_forceFitterTools) {
    QStringList tooltypes;
    tooltypes << "Trk::KalmanFitter"
	      << "Trk::GlobalChi2Fitter"
	      << "Trk::GaussianSumFilter"
	      << "Trk::DistributedKalmanFilter"
	      << "Trk::DeterministicAnnealingFilter";
    initTools<Trk::ITrackFitter>(tooltypes,QString("VP1_JOBCFG_EXTRA_VP1_FITTERS"));
  }

  return StatusCode::SUCCESS;
}

//____________________________________________________________________
StatusCode VP1TrkInitializer::execute()
{
  msg(MSG::DEBUG) <<" in execute() " << endreq;
  return StatusCode::SUCCESS;
}

//____________________________________________________________________
StatusCode VP1TrkInitializer::finalize()
{
  msg(MSG::INFO) <<" in finalize() " << endreq;
  return StatusCode::SUCCESS;
}

// Private method
template <typename T> 
void VP1TrkInitializer::initTools(QStringList& toolTypes, QString env)
{
  VP1ToolAccessHelper toolAccessHelper(m_toolSvc);
  VP1AvailableToolsHelper availTools(m_toolSvc);
  availTools.addMonitoredTypes(toolTypes);
  QStringList existingTools = availTools.availableTools();

  foreach (QString key,VP1QtUtils::environmentVariableValue(env).split(';',QString::SkipEmptyParts)) {
    if (existingTools.contains(key))
      continue;
    msg(MSG::DEBUG) << "Attempting creation of fittertool with tooltype/key " << key.toStdString() << endreq;
    T* tool = toolAccessHelper.getToolPointer<T>(key,false/*silent*/,true/*create if not exists*/);
    if (!tool) {
      msg(MSG::WARNING) << "Could not create tool with type/key " << key.toStdString() << endreq;
    }
  }
}
