// Getter tool(s):
#include "../GenEventGetterTool.h"
#include "../GenParticleGetterTool.h"
#include "../GenVertexGetterTool.h"
// Associator tool(s):
#include "../GenEventGenParticleAssociationTool.h"
#include "../GenParticleEventAssociationTool.h"
#include "../GenParticleParticleAssociationTool.h"
#include "../GenParticleTruthParticleAssociationTool.h"
#include "../GenParticleVertexAssociationTool.h"
#include "../GenVertexParticleAssociationTool.h"
#include "../GenVertexEventAssociationTool.h"
#include "../TruthParticleEtIsolationAssociationTool.h"
#include "../TruthParticleChildAssociationTool.h"
#include "../TruthParticleParentAssociationTool.h"
#include "../TruthParticleGenParticleAssociationTool.h"
#include "../TruthParticleProdVertexAssociationTool.h"
#include "../PileUpInfoAssociatorTool.h"
#include "../TruthTauDecayAssociationTool.h"
#include "../TruthLeptonParentAssociationTool.h"
#include "../TruthLeptonNearbyAssociationTool.h"
// Filler tool(s):
#include "../GenEventFillerTool.h"
#include "../GenEventPileUpFillerTool.h"
#include "../TruthParticleBremFillerTool.h"
#include "../GenParticleFillerTool.h"
#include "../GenParticlePerigeeFillerTool.h"
#include "../GenVertexFillerTool.h"
#include "../TruthEtIsolationFillerTool.h"
#include "../TruthParticleFillerTool.h"
#include "../TruthParticleBarcodesFillerTool.h"
#include "../PileUpInfoFillerTool.h"
#include "../TruthParticleFakerTool.h"
#include "../HforFillerTool.h"
#include "../JetFullTruthTag.h"
#include "../TruthParticleClassificationFillerTool.h"
// Filter tool(s):
#include "../GenEventGetterFilterTool.h"
#include "../TruthJetFilterTool.h"

// Getter tool(s):
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenEventGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenParticleGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenVertexGetterTool)
// Associator tool(s):
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenEventGenParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenParticleEventAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenParticleParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenParticleTruthParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenParticleVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenVertexParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenVertexEventAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthParticleEtIsolationAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthParticleChildAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthParticleParentAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthParticleGenParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthParticleProdVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PileUpInfoAssociatorTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthTauDecayAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthLeptonParentAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthLeptonNearbyAssociationTool)
// Filler tool(s):
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenEventFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenEventPileUpFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthParticleBremFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenParticleFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenParticlePerigeeFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenVertexFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthEtIsolationFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthParticleFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthParticleBarcodesFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PileUpInfoFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthParticleFakerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, JetFullTruthTag)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, HforFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthParticleClassificationFillerTool)
// Filter tool(s):
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenEventGetterFilterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthJetFilterTool)

