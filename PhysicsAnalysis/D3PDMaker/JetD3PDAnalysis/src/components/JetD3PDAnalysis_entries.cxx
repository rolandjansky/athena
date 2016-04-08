// $Id$
/**
 * @file JetD3PDAnalysis/src/components/JetD3PDAnalysis_entries.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2011
 * @brief List Gaudi components.
 */


#include "../JetVertexFractionAlg.h"

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY      (D3PD, JetVertexFractionAlg)


DECLARE_FACTORY_ENTRIES(JetD3PDAnalysis) {
  DECLARE_NAMESPACE_ALGORITHM      (D3PD, JetVertexFractionAlg)
}
