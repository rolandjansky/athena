#include "../JetAlgorithm.h"
#include "JetRec/JetToolRunner.h"
#include "JetRec/JetRecTool.h"
#include "JetRec/JetDumper.h"
#include "JetRec/PseudoJetGetter.h"
#include "JetRec/MuonSegmentPseudoJetGetter.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetRec/JetFinder.h"
#include "JetRec/JetByVertexFinder.h"
#include "JetRec/JetSplitter.h"
#include "JetRec/JetTrimmer.h"
#include "JetRec/JetPruner.h"
#include "JetRec/JetReclusterer.h"
#include "JetRec/FastJetInterfaceTool.h"

#include "JetRec/JetSorter.h"
#include "JetRec/JetPseudojetRetriever.h"
#include "JetRec/JetConstituentsRetriever.h"
#include "JetRec/JetPseudojetCopier.h"
#include "JetRec/JetFilterTool.h"
#include "JetRec/JetConstitRemover.h"

DECLARE_COMPONENT( JetToolRunner )
DECLARE_COMPONENT( JetRecTool )
DECLARE_COMPONENT( JetDumper )
DECLARE_COMPONENT( PseudoJetGetter )
DECLARE_COMPONENT( MuonSegmentPseudoJetGetter )
DECLARE_COMPONENT( JetFromPseudojet )
DECLARE_COMPONENT( JetFinder )
DECLARE_COMPONENT( JetByVertexFinder )
DECLARE_COMPONENT( JetSorter )
DECLARE_COMPONENT( JetFilterTool )
DECLARE_COMPONENT( JetSplitter )
DECLARE_COMPONENT( JetTrimmer )
DECLARE_COMPONENT( JetPruner )
DECLARE_COMPONENT( JetReclusterer )
DECLARE_COMPONENT( FastJetInterfaceTool )
DECLARE_COMPONENT( JetPseudojetRetriever )
DECLARE_COMPONENT( JetConstituentsRetriever )
DECLARE_COMPONENT( JetPseudojetCopier )
DECLARE_COMPONENT( JetConstitRemover )

DECLARE_COMPONENT( JetAlgorithm )

