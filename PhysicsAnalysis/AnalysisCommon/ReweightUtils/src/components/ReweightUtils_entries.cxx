#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ReweightUtils/McEventWeight.h"
#include "ReweightUtils/PDFWeight.h"
#include "ReweightUtils/WeightsAgregator.h"
#include "ReweightUtils/WeightToolBase.h"
#include "../SumOfWeightsAlg.h"


DECLARE_TOOL_FACTORY( McEventWeight )
DECLARE_TOOL_FACTORY( PDFWeight )
DECLARE_TOOL_FACTORY( WeightsAgregator )
DECLARE_ALGORITHM_FACTORY( SumOfWeightsAlg )


#include "ReweightUtils/ParticleScaleFactorTool.h"
DECLARE_TOOL_FACTORY( ParticleScaleFactorTool )

DECLARE_FACTORY_ENTRIES( ReweightUtils ) 
{ 
  DECLARE_TOOL( ParticleScaleFactorTool );
  DECLARE_TOOL( McEventWeight ); 
  DECLARE_TOOL( PDFWeight ); 
  DECLARE_TOOL( WeightsAgregator ); 
  DECLARE_ALGORITHM( SumOfWeightsAlg );
} 
