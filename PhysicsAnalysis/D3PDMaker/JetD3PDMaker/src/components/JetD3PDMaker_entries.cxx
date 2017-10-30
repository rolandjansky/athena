#include "../JetBTagWeightFillerTool.h"
#include "../JetFlavorTagComponentsFillerTool.h"
#include "../JetVtxFillerTool.h"
#include "../JetVertexFractionFillerTool.h"
#include "../JetEMFractionFillerTool.h"
#include "../JetSamplingsFillerTool.h"
#include "../JetSignalStateFillerTool.h"
#include "../JetAuthorFillerTool.h"
#include "../JetMomentFillerTool.h"
#include "../JetConstituentFillerTool.h"
#include "../JetTileCellFillerTool.h"
#include "../JetTileFilterAlg.h"
#include "../JetTrueTagFillerTool.h"
#include "../FlavorAssociationTool.h"
#include "../JetCaloUtilsFillerTool.h"
#include "../JetShapeFillerTool.h"
//#include "../JetFixupAlg.h"
#include "../JetROIWordFillerTool.h"
#include "../JetSamplingsFracFillerTool.h"
#include "../JetL1TriggerObjectAssociationTool.h"
#include "../JetL2TriggerObjectAssociationTool.h"
#include "../JetEFTriggerObjectAssociationTool.h"


DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetBTagWeightFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetFlavorTagComponentsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTrueTagFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetVtxFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetVertexFractionFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetEMFractionFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetSamplingsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetSignalStateFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetAuthorFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetMomentFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetConstituentFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, FlavorAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetCaloUtilsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetShapeFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTileCellFillerTool)
DECLARE_ALGORITHM_FACTORY (JetTileFilterAlg)
//DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, JetFixupAlg)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetROIWordFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetSamplingsFracFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetL1TriggerObjectAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetL2TriggerObjectAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetEFTriggerObjectAssociationTool)

