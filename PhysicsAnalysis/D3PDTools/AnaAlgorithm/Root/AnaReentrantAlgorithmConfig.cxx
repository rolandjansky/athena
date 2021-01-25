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
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace EL
{
  void AnaReentrantAlgorithmConfig ::
  testInvariant () const
  {
  }



  AnaReentrantAlgorithmConfig ::
  AnaReentrantAlgorithmConfig ()
  {
    RCU_NEW_INVARIANT (this);
  }



  AnaReentrantAlgorithmConfig ::
  AnaReentrantAlgorithmConfig (const std::string& val_typeAndName)
    : AsgComponentConfig (val_typeAndName)
  {
    RCU_NEW_INVARIANT (this);
  }



  ::StatusCode AnaReentrantAlgorithmConfig ::
  makeAlgorithm (std::unique_ptr<AnaReentrantAlgorithm>& algorithm,
                 const AlgorithmWorkerData& workerData) const
  {
    RCU_READ_INVARIANT (this);
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
