#include "BTagging/BTagTool.h"
#include "BTagging/BTagSecVertexing.h"
#include "BTagging/BTagLightSecVertexing.h"
#include "BTagging/BTagJetPtScaling.h"
#include "BTagging/JetBTaggingAlg.h"
#include "BTagging/JetSecVertexingAlg.h"
#include "BTagging/JetSecVtxFindingAlg.h"
#include "BTagging/BTagTrackAugmenterAlg.h"

using namespace Analysis ;

DECLARE_COMPONENT( BTagSecVertexing )
DECLARE_COMPONENT( BTagLightSecVertexing )
DECLARE_COMPONENT( JetSecVertexingAlg )
DECLARE_COMPONENT( JetSecVtxFindingAlg )
DECLARE_COMPONENT( BTagJetPtScaling )
DECLARE_COMPONENT( BTagTool )
DECLARE_COMPONENT( JetBTaggingAlg )
DECLARE_COMPONENT( BTagTrackAugmenterAlg )
/** factory entries need to have the name of the package */

