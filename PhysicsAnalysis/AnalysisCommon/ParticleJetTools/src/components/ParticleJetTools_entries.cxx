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

/* DECLARE_COMPONENT( Analysis::JetTrackTruthMatching ) */
DECLARE_COMPONENT( Analysis::JetQuarkLabel )
DECLARE_COMPONENT( Analysis::JetConeLabeling )
DECLARE_COMPONENT( Analysis::JetPartonTruthLabel )
/* DECLARE_COMPONENT( Analysis::JetQGPartonLabel ) */
DECLARE_COMPONENT( Analysis::ParticleToJetAssociator )
/* DECLARE_COMPONENT( Analysis::FindLeptonTruth ) */
/// @todo Convert to namespace, tool, etc?
DECLARE_COMPONENT( CopyFlavorLabelTruthParticles )
DECLARE_COMPONENT( CopyBosonTopLabelTruthParticles )
DECLARE_COMPONENT( CopyTruthPartons )
DECLARE_COMPONENT( CopyTruthJetParticles )
DECLARE_COMPONENT( ParticleJetDeltaRLabelTool )
DECLARE_COMPONENT( JetParticleShrinkingConeAssociation )
DECLARE_COMPONENT( JetParticleCenterOfMassAssociation )
DECLARE_COMPONENT( JetParticleAssociationAlg )

