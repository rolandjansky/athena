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
#include "DerivationFrameworkMCTruth/TruthJetDecorationTool.h"
#include "DerivationFrameworkMCTruth/TruthD2Decorator.h"
#include "DerivationFrameworkMCTruth/TruthHSDecorator.h"
#include "../GenFilterTool.h"
#include "../HardScatterCollectionMaker.h"
#include "../TruthNavigationDecorator.h"
#include "../TruthBornLeptonCollectionMaker.h"
#include "../TruthLinkRepointTool.h"
#include "../TruthEDDecorator.h"

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
DECLARE_TOOL_FACTORY( TruthJetDecorationTool )
DECLARE_TOOL_FACTORY( GenFilterTool )
DECLARE_TOOL_FACTORY( HardScatterCollectionMaker )
DECLARE_TOOL_FACTORY( TruthNavigationDecorator )
DECLARE_TOOL_FACTORY( TruthBornLeptonCollectionMaker )
DECLARE_TOOL_FACTORY( TruthD2Decorator )
DECLARE_TOOL_FACTORY( TruthHSDecorator )
DECLARE_TOOL_FACTORY( TruthLinkRepointTool )
DECLARE_TOOL_FACTORY( TruthEDDecorator )

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
   DECLARE_TOOL( TruthJetDecorationTool )
   DECLARE_TOOL( GenFilterTool )
   DECLARE_TOOL( HardScatterCollectionMaker )
   DECLARE_TOOL( TruthNavigationDecorator )
   DECLARE_TOOL( TruthBornLeptonCollectionMaker )
   DECLARE_TOOL( TruthD2Decorator )
   DECLARE_TOOL( TruthHSDecorator )
   DECLARE_TOOL( TruthLinkRepointTool )
   DECLARE_TOOL( TruthEDDecorator )
}
