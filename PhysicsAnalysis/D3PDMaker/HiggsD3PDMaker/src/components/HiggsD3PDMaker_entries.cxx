// $Id: HiggsD3PDMaker_entries.cxx 485019 2012-02-24 16:16:04Z krasznaa $
/**
 * @file HiggsD3PDMaker_entries.cxx
 * @author Kirill.Prokofiev@cern.ch
 * @date Feb. 2012
 * @brief List Gaudi components.
 */

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s)
#include "../QuadrupletFillerTool.h"
#include "../QuadrupletVertexAssociationTool.h"
#include "../QuadrupletCompositeParticleAssociationTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, QuadrupletFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, QuadrupletVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, QuadrupletCompositeParticleAssociationTool)

DECLARE_FACTORY_ENTRIES(HiggsD3PDMaker) 
{
  DECLARE_NAMESPACE_TOOL   (D3PD, QuadrupletFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, QuadrupletVertexAssociationTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, QuadrupletCompositeParticleAssociationTool)
}
