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

