/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AnaAlgorithm/AnaAlgorithmConfig.h>

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AnaAlgorithm/AlgorithmWorkerData.h>
#include <AnaAlgorithm/MessageCheck.h>
#include <AsgTools/AsgTool.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace EL
{
  void AnaAlgorithmConfig :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  AnaAlgorithmConfig :: 
  AnaAlgorithmConfig ()
  {
    RCU_NEW_INVARIANT (this);
  }



  AnaAlgorithmConfig ::
  AnaAlgorithmConfig (const std::string& val_typeAndName)
    : AsgComponentConfig (val_typeAndName)
  {
    RCU_NEW_INVARIANT (this);
  }



  AnaAlgorithmConfig ::
  AnaAlgorithmConfig (const AsgComponentConfig& val_config)
    : AsgComponentConfig (val_config)
  {
    RCU_NEW_INVARIANT (this);
  }



  bool AnaAlgorithmConfig ::
  useXAODs () const noexcept
  {
    RCU_READ_INVARIANT (this);
    return m_useXAODs;
  }



  void AnaAlgorithmConfig ::
  setUseXAODs (bool val_useXAODs) noexcept
  {
    RCU_CHANGE_INVARIANT (this);
    m_useXAODs = val_useXAODs;
  }



  bool AnaAlgorithmConfig ::
  isPublicTool () const noexcept
  {
    RCU_READ_INVARIANT (this);
    return m_isPublicTool;
  }



  void AnaAlgorithmConfig ::
  setIsPublicTool (bool val_isPublicTool) noexcept
  {
    RCU_CHANGE_INVARIANT (this);
    m_isPublicTool = val_isPublicTool;
  }



  ::StatusCode AnaAlgorithmConfig ::
  makeAlgorithm (std::unique_ptr<AnaAlgorithm>& algorithm,
                 const AlgorithmWorkerData& workerData) const
  {
    RCU_READ_INVARIANT (this);
    using namespace msgAlgorithmConfig;

    if (m_isPublicTool == false)
    {
      ANA_CHECK (makeComponentExpert (algorithm, "new %1% (\"%2%\", nullptr)", true, ""));
      algorithm->setHistogramWorker (workerData.m_histogramWorker);
      algorithm->setTreeWorker (workerData.m_treeWorker);
      algorithm->setFilterWorker (workerData.m_filterWorker);
      algorithm->setWk (workerData.m_wk);
      if (workerData.m_evtStore)
        algorithm->setEvtStore (workerData.m_evtStore);
      ANA_CHECK (algorithm->sysInitialize());
    } else
    {
      std::unique_ptr<asg::AsgTool> tool;
      ANA_CHECK (makeComponentExpert (tool, "new %1% (\"%2%\")", true, ""));
      ANA_CHECK (tool->initialize());

      AnaAlgorithmConfig dummyAlg ("EL::AnaAlgorithm/DummyAlg." + name());
      ANA_CHECK (dummyAlg.makeAlgorithm (algorithm, workerData));
      algorithm->addCleanup (std::move (tool));
    }

    ANA_MSG_DEBUG ("Created component of type " << type());
    return StatusCode::SUCCESS;
  }
}
