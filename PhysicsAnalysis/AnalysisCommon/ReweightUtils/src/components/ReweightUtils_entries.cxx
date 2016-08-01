#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../../Root/McEventWeight.cxx"
#include "../../Root/PDFWeight.cxx"
#include "../../Root/WeightsAgregator.cxx"
#include "../../Root/WeightToolBase.cxx"
#include "ReweightUtils/SumOfWeightsComputations.h"


DECLARE_TOOL_FACTORY( McEventWeight )
DECLARE_TOOL_FACTORY( PDFWeight )
DECLARE_TOOL_FACTORY( WeightsAgregator )
DECLARE_ALGORITHM_FACTORY( SumOfWeightsTool)

DECLARE_FACTORY_ENTRIES( ReweightUtils ) 
{ 
  DECLARE_TOOL( McEventWeight ); 
  DECLARE_TOOL( PDFWeight ); 
  DECLARE_TOOL( WeightsAgregator ); 
  DECLARE_ALGORITHM( SumOfWeightsTool);
} 
