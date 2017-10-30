#include "DerivationFrameworkMCTruth/TruthDressingTool.h"
#include "DerivationFrameworkMCTruth/TruthIsolationTool.h"
#include "DerivationFrameworkMCTruth/MenuTruthThinning.h"
#include "DerivationFrameworkMCTruth/GenericTruthThinning.h"
#include "DerivationFrameworkMCTruth/TruthCollectionMaker.h"
#include "DerivationFrameworkMCTruth/TruthCollectionMakerTau.h"
#include "DerivationFrameworkMCTruth/TruthClassificationDecorator.h"
#include "DerivationFrameworkMCTruth/CompactHardTruth.h"
#include "DerivationFrameworkMCTruth/HardTruthThinning.h"

#include "DerivationFrameworkMCTruth/HadronOriginDecorator.h"
#include "DerivationFrameworkMCTruth/HadronOriginClassifier.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( TruthDressingTool )
DECLARE_TOOL_FACTORY( TruthIsolationTool )
DECLARE_TOOL_FACTORY( MenuTruthThinning )
DECLARE_TOOL_FACTORY( GenericTruthThinning )
DECLARE_TOOL_FACTORY( TruthCollectionMaker )
DECLARE_TOOL_FACTORY( TruthCollectionMakerTau )
DECLARE_TOOL_FACTORY( TruthClassificationDecorator )
DECLARE_ALGORITHM_FACTORY( CompactHardTruth )
DECLARE_TOOL_FACTORY( HardTruthThinning )

DECLARE_TOOL_FACTORY( HadronOriginDecorator )
DECLARE_TOOL_FACTORY(  HadronOriginClassifier )

