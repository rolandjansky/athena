/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1TrkAuxAlgs/VP1TrkInitializer.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Utils/VP1ToolAccessHelper.h"
#include "VP1Utils/VP1AvailableToolsHelper.h"

#include "GaudiKernel/IToolSvc.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include <QtCoreVersion>

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
  msg(MSG::INFO) << " in initialize() " << endmsg;

  //ToolSvc
  StatusCode status = service("ToolSvc",m_toolSvc);
  if (status.isFailure()||!m_toolSvc) {
    msg(MSG::ERROR) << " Unable to get ToolSvc!" << endmsg;
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
  msg(MSG::DEBUG) <<" in execute() " << endmsg;
  return StatusCode::SUCCESS;
}

//____________________________________________________________________
StatusCode VP1TrkInitializer::finalize()
{
  msg(MSG::INFO) <<" in finalize() " << endmsg;
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

#if QTCORE_VERSION >= 0x050E00
  foreach (QString key,VP1QtUtils::environmentVariableValue(env).split(';',Qt::SkipEmptyParts))
#else
  foreach (QString key,VP1QtUtils::environmentVariableValue(env).split(';',QString::SkipEmptyParts))
#endif
  {
    if (existingTools.contains(key))
      continue;
    msg(MSG::DEBUG) << "Attempting creation of fittertool with tooltype/key " << key.toStdString() << endmsg;
    T* tool = toolAccessHelper.getToolPointer<T>(key,false/*silent*/,true/*create if not exists*/);
    if (!tool) {
      msg(MSG::WARNING) << "Could not create tool with type/key " << key.toStdString() << endmsg;
    }
  }
}
