#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../../Root/McEventWeight.cxx"
#include "../../Root/WeightsAgregator.cxx"
#include "../../Root/WeightToolBase.cxx"

DECLARE_TOOL_FACTORY( McEventWeight )
DECLARE_TOOL_FACTORY( WeightsAgregator )

DECLARE_FACTORY_ENTRIES( ReweightUtils ) 
{ 
  DECLARE_TOOL( McEventWeight ); 
  DECLARE_TOOL( WeightsAgregator ); 
} 
