/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


#include <GaudiKernel/DeclareFactoryEntries.h>

#include <TriggerAnalysisAlgorithms/TrigEventSelectionAlg.h>
#include <TriggerAnalysisAlgorithms/TrigPrescalesAlg.h>


DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, TrigEventSelectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, TrigPrescalesAlg)

DECLARE_FACTORY_ENTRIES(TriggerAnalysisAlgorithms) {
  DECLARE_NAMESPACE_ALGORITHM (CP, TrigEventSelectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, TrigPrescalesAlg)
}
