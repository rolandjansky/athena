//#include "BTagging/BJetBuilder.h"
//#include "BTagging/BJetBuilderFast.h"
#include "BTagging/BTagTool.h"
#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/BTagSecVertexing.h"
#include "BTagging/BTagJetPtScaling.h"
#include "BTagging/JetBTaggerTool.h"
#include "BTagging/JetBTaggerAlg.h"
#include "BTagging/StandAloneJetBTaggerAlg.h"

using namespace Analysis ;

DECLARE_COMPONENT( StandAloneJetBTaggerAlg )
DECLARE_COMPONENT( BTagTrackAssociation )
DECLARE_COMPONENT( BTagSecVertexing )
DECLARE_COMPONENT( BTagJetPtScaling )
DECLARE_COMPONENT( BTagTool )
DECLARE_COMPONENT( JetBTaggerTool )
DECLARE_COMPONENT( JetBTaggerAlg )

/** factory entries need to have the name of the package */

