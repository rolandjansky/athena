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

//DECLARE_ALGORITHM_FACTORY( BJetBuilder )
//DECLARE_ALGORITHM_FACTORY( BJetBuilderFast )
DECLARE_ALGORITHM_FACTORY( StandAloneJetBTaggerAlg )
DECLARE_TOOL_FACTORY( BTagLabeling )
DECLARE_TOOL_FACTORY( BTagTrackAssociation )
DECLARE_TOOL_FACTORY( BTagSecVertexing )
DECLARE_TOOL_FACTORY( BTagJetPtScaling )
//DECLARE_TOOL_FACTORY( BTagRemoving )
DECLARE_TOOL_FACTORY( BTagTool )
DECLARE_TOOL_FACTORY( JetBTaggerTool )

/** factory entries need to have the name of the package */

