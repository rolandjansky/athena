#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkMCTruth/TruthDressingTool.h"
#include "DerivationFrameworkMCTruth/TruthIsolationTool.h"
#include "DerivationFrameworkMCTruth/MenuTruthThinning.h"
#include "DerivationFrameworkMCTruth/GenericTruthThinning.h"
#include "DerivationFrameworkMCTruth/TruthCollectionMaker.h"
#include "DerivationFrameworkMCTruth/TruthCollectionMakerTau.h"
#include "DerivationFrameworkMCTruth/TruthDecayCollectionMaker.h"
#include "DerivationFrameworkMCTruth/TruthPVCollectionMaker.h"
#include "DerivationFrameworkMCTruth/TruthMetaDataWriter.h"
#include "DerivationFrameworkMCTruth/TruthClassificationDecorator.h"
#include "DerivationFrameworkMCTruth/CompactHardTruth.h"
#include "DerivationFrameworkMCTruth/HardTruthThinning.h"
#include "DerivationFrameworkMCTruth/HadronOriginDecorator.h"
#include "DerivationFrameworkMCTruth/HadronOriginClassifier.h"
#include "DerivationFrameworkMCTruth/TruthQGDecorationTool.h"
#include "../GenFilterTool.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( TruthDressingTool )
DECLARE_TOOL_FACTORY( TruthIsolationTool )
DECLARE_TOOL_FACTORY( MenuTruthThinning )
DECLARE_TOOL_FACTORY( GenericTruthThinning )
DECLARE_TOOL_FACTORY( TruthCollectionMaker )
DECLARE_TOOL_FACTORY( TruthCollectionMakerTau )
DECLARE_TOOL_FACTORY( TruthDecayCollectionMaker )
DECLARE_TOOL_FACTORY( TruthPVCollectionMaker )
DECLARE_TOOL_FACTORY( TruthMetaDataWriter )
DECLARE_TOOL_FACTORY( TruthClassificationDecorator )
DECLARE_ALGORITHM_FACTORY( CompactHardTruth )
DECLARE_TOOL_FACTORY( HardTruthThinning )
DECLARE_TOOL_FACTORY( HadronOriginDecorator )
DECLARE_TOOL_FACTORY( HadronOriginClassifier )
DECLARE_TOOL_FACTORY( TruthQGDecorationTool )
DECLARE_TOOL_FACTORY( GenFilterTool )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkMCTruth) {
   DECLARE_TOOL( TruthDressingTool )
   DECLARE_TOOL( TruthIsolationTool )
   DECLARE_TOOL( MenuTruthThinning )
   DECLARE_TOOL( GenericTruthThinning )
   DECLARE_TOOL( TruthCollectionMaker )
   DECLARE_TOOL( TruthCollectionMakerTau )
   DECLARE_TOOL( TruthDecayCollectionMaker )
   DECLARE_TOOL( TruthPVCollectionMaker )
   DECLARE_TOOL( TruthMetaDataWriter )
   DECLARE_TOOL( TruthClassificationDecorator )
   DECLARE_ALGORITHM( CompactHardTruth )
   DECLARE_TOOL( HardTruthThinning )
   DECLARE_TOOL( HadronOriginDecorator )
   DECLARE_TOOL( HadronOriginClassifier )
   DECLARE_TOOL( TruthQGDecorationTool )
   DECLARE_TOOL( GenFilterTool )
}

