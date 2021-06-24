/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AnaAlgorithm/AnaReentrantAlgorithmConfig.h>

#include <AnaAlgorithm/AnaReentrantAlgorithm.h>
#include <AnaAlgorithm/AlgorithmWorkerData.h>
#include <AnaAlgorithm/MessageCheck.h>
#include <AsgTools/AsgTool.h>

//
// method implementations
//

namespace EL
{
  AnaReentrantAlgorithmConfig ::
  AnaReentrantAlgorithmConfig ()
  {
  }



  AnaReentrantAlgorithmConfig ::
  AnaReentrantAlgorithmConfig (const std::string& val_typeAndName)
    : AsgComponentConfig (val_typeAndName)
  {
  }



  AnaReentrantAlgorithmConfig ::
  AnaReentrantAlgorithmConfig (const AsgComponentConfig& val_config)
    : AsgComponentConfig (val_config)
  {
  }



  ::StatusCode AnaReentrantAlgorithmConfig ::
  makeAlgorithm (std::unique_ptr<AnaReentrantAlgorithm>& algorithm,
                 const AlgorithmWorkerData& workerData) const
  {
    using namespace msgAlgorithmConfig;

    ANA_CHECK (makeComponentExpert (algorithm, "new %1% (\"%2%\", nullptr)", true, ""));
    algorithm->setFilterWorker (workerData.m_filterWorker);
    algorithm->setWk (workerData.m_wk);
    if (workerData.m_evtStore)
      algorithm->setEvtStore (workerData.m_evtStore);
    ANA_CHECK (algorithm->sysInitialize());
    return StatusCode::SUCCESS;
  }
}
