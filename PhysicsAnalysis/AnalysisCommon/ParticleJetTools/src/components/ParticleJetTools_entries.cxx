#include "GaudiKernel/DeclareFactoryEntries.h"
//#include "ParticleJetTools/JetTrackTruthMatching.h"
#include "ParticleJetTools/JetQuarkLabel.h"
#include "ParticleJetTools/JetConeLabeling.h"
//#include "ParticleJetTools/JetQGPartonLabel.h"
#include "ParticleJetTools/ParticleToJetAssociator.h"
#include "ParticleJetTools/FindLeptonTruth.h"
#include "ParticleJetTools/CopyFlavorLabelTruthParticles.h"
#include "ParticleJetTools/CopyBosonTopLabelTruthParticles.h"
#include "ParticleJetTools/CopyTruthPartons.h"
#include "ParticleJetTools/JetPartonTruthLabel.h"

using namespace Analysis;

/* DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetTrackTruthMatching ) */
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetQuarkLabel )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetConeLabeling )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetPartonTruthLabel )
/* DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetQGPartonLabel ) */
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, ParticleToJetAssociator )
/* DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, FindLeptonTruth ) */
/// @todo Convert to namespace, tool, etc?
DECLARE_ALGORITHM_FACTORY( CopyFlavorLabelTruthParticles )
DECLARE_ALGORITHM_FACTORY( CopyBosonTopLabelTruthParticles )
DECLARE_ALGORITHM_FACTORY( CopyTruthPartons )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( ParticleJetTools ) {
/*    DECLARE_NAMESPACE_TOOL( Analysis, JetTrackTruthMatching ) */
    DECLARE_NAMESPACE_TOOL( Analysis, JetQuarkLabel )
    DECLARE_NAMESPACE_TOOL( Analysis, JetConeLabeling )
    DECLARE_NAMESPACE_TOOL( Analysis, JetPartonTruthLabel )
/*    DECLARE_NAMESPACE_TOOL( Analysis, JetQGPartonLabel ) */
    DECLARE_NAMESPACE_TOOL( Analysis, ParticleToJetAssociator )
/*    DECLARE_NAMESPACE_TOOL( Analysis, FindLeptonTruth ) */
    /// @todo Convert to namespace, tool, etc?
    DECLARE_ALGORITHM( CopyFlavorLabelTruthParticles )
    DECLARE_ALGORITHM( CopyBosonTopLabelTruthParticles )
    DECLARE_ALGORITHM( CopyTruthPartons )
}
