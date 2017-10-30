//#include "ParticleJetTools/JetTrackTruthMatching.h"
#include "ParticleJetTools/JetQuarkLabel.h"
#include "ParticleJetTools/JetConeLabeling.h"
//#include "ParticleJetTools/JetQGPartonLabel.h"
#include "ParticleJetTools/ParticleToJetAssociator.h"
//#include "ParticleJetTools/FindLeptonTruth.h"
#include "ParticleJetTools/CopyFlavorLabelTruthParticles.h"
#include "ParticleJetTools/CopyBosonTopLabelTruthParticles.h"
#include "ParticleJetTools/CopyTruthPartons.h"
#include "ParticleJetTools/JetPartonTruthLabel.h"
#include "ParticleJetTools/CopyTruthJetParticles.h"
#include "ParticleJetTools/ParticleJetDeltaRLabelTool.h"
#include "ParticleJetTools/JetParticleAssociationAlg.h"
#include "ParticleJetTools/JetParticleShrinkingConeAssociation.h"
#include "ParticleJetTools/JetParticleCenterOfMassAssociation.h"

using namespace Analysis;

/* DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetTrackTruthMatching ) */
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetQuarkLabel )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetConeLabeling )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetPartonTruthLabel )
/* DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetQGPartonLabel ) */
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, ParticleToJetAssociator )
/* DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, FindLeptonTruth ) */
/// @todo Convert to namespace, tool, etc?
DECLARE_TOOL_FACTORY( CopyFlavorLabelTruthParticles )
DECLARE_TOOL_FACTORY( CopyBosonTopLabelTruthParticles )
DECLARE_TOOL_FACTORY( CopyTruthPartons )
DECLARE_TOOL_FACTORY( CopyTruthJetParticles )
DECLARE_TOOL_FACTORY( ParticleJetDeltaRLabelTool )
DECLARE_TOOL_FACTORY( JetParticleShrinkingConeAssociation )
DECLARE_TOOL_FACTORY( JetParticleCenterOfMassAssociation )
DECLARE_ALGORITHM_FACTORY( JetParticleAssociationAlg )

