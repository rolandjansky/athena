//#include "BTagging/BJetBuilder.h"
//#include "BTagging/BJetBuilderFast.h"
#include "BTagging/BTagTool.h"
#include "BTagging/BTagLabeling.h"
#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/BTagSecVertexing.h"
#include "BTagging/BTagJetPtScaling.h"
//#include "BTagging/BTagRemoving.h"
#include "BTagging/JetBTaggerTool.h"
#include "BTagging/StandAloneJetBTaggerAlg.h"

using namespace Analysis ;

//DECLARE_COMPONENT( BJetBuilder )
//DECLARE_COMPONENT( BJetBuilderFast )
DECLARE_COMPONENT( StandAloneJetBTaggerAlg )
DECLARE_COMPONENT( BTagLabeling )
DECLARE_COMPONENT( BTagTrackAssociation )
DECLARE_COMPONENT( BTagSecVertexing )
DECLARE_COMPONENT( BTagJetPtScaling )
//DECLARE_COMPONENT( BTagRemoving )
DECLARE_COMPONENT( BTagTool )
DECLARE_COMPONENT( JetBTaggerTool )

/** factory entries need to have the name of the package */

