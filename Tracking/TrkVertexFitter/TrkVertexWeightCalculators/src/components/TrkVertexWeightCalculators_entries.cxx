
 #include "GaudiKernel/DeclareFactoryEntries.h"

 #include "TrkVertexWeightCalculators/SumPtVertexWeightCalculator.h"
 #include "TrkVertexWeightCalculators/VxProbVertexWeightCalculator.h"
 #include "TrkVertexWeightCalculators/NNVertexWeightCalculator.h"
 #include "TrkVertexWeightCalculators/TrueVertexDistanceWeightCalculator.h"

 
 using namespace Trk;
 
 DECLARE_TOOL_FACTORY(SumPtVertexWeightCalculator )
 DECLARE_TOOL_FACTORY(VxProbVertexWeightCalculator )
 DECLARE_TOOL_FACTORY(NNVertexWeightCalculator )
 DECLARE_TOOL_FACTORY(TrueVertexDistanceWeightCalculator)

 DECLARE_FACTORY_ENTRIES( TrkVertexWeightCalculators )
 {
   DECLARE_TOOL(SumPtVertexWeightCalculator)
   DECLARE_TOOL(VxProbVertexWeightCalculator)
   DECLARE_TOOL(NNVertexWeightCalculator)
   DECLARE_TOOL(TrueVertexDistanceWeightCalculator)

 }
 

 
