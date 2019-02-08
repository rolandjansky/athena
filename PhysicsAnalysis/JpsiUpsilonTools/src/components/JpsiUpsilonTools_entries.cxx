#include "GaudiKernel/DeclareFactoryEntries.h"
#include "JpsiUpsilonTools/JpsiFinder.h"
#include "JpsiUpsilonTools/JpsiPlus1Track.h"
#include "JpsiUpsilonTools/JpsiPlus2Tracks.h"
#include "JpsiUpsilonTools/JpsiFinder_ee.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "JpsiUpsilonTools/JpsiAlg.h"
#include "JpsiUpsilonTools/JpsiExample.h"

using namespace Analysis;

DECLARE_TOOL_FACTORY( JpsiFinder )
DECLARE_TOOL_FACTORY( JpsiPlus1Track )
DECLARE_TOOL_FACTORY( JpsiPlus2Tracks )
DECLARE_TOOL_FACTORY( JpsiFinder_ee )
DECLARE_TOOL_FACTORY( PrimaryVertexRefitter )
DECLARE_ALGORITHM_FACTORY( JpsiAlg )
DECLARE_ALGORITHM_FACTORY( JpsiExample )

DECLARE_FACTORY_ENTRIES( JpsiUpsilonTools ) {
    DECLARE_ALGORITHM( JpsiAlg )
    DECLARE_ALGORITHM( JpsiExample )
    DECLARE_TOOL( Analysis::JpsiFinder )
    DECLARE_TOOL( Analysis::JpsiPlus1Track )
    DECLARE_TOOL( Analysis::JpsiPlus2Tracks )
    DECLARE_TOOL( Analysis::PrimaryVertexRefitter )
}

