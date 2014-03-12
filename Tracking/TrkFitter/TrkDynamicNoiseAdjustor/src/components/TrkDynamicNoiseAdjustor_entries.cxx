#include "TrkDynamicNoiseAdjustor/InDetDynamicNoiseAdjustment.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// using namespace Trk;

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, InDetDynamicNoiseAdjustment )

DECLARE_FACTORY_ENTRIES(TrkDynamicNoiseAdjustor)
{ // in this package name these methods are registered 
  DECLARE_NAMESPACE_TOOL(Trk, InDetDynamicNoiseAdjustment)
}

