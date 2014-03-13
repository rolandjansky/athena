#include "MissingETGoodness/EtmissGoodnessSelectorAlg.h"
#include "MissingETGoodness/EtmissGoodnessManagerAlg.h"
#include "MissingETGoodness/EtmissGoodnessManagerTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MissingETGoodness/JetGoodiesFiller.h"
#include "MissingETGoodness/MetGoodiesFiller.h"
#include "MissingETGoodness/JetVarTool.h"
#include "MissingETGoodness/METCollisionFilter.h"
#include "MissingETGoodness/SkimDecisionFilter.h"
#include "MissingETGoodness/MuonGoodiesFiller.h"

DECLARE_ALGORITHM_FACTORY( EtmissGoodnessSelectorAlg )
DECLARE_ALGORITHM_FACTORY( EtmissGoodnessManagerAlg )
DECLARE_ALGORITHM_FACTORY( JetGoodiesFiller )
DECLARE_ALGORITHM_FACTORY( MetGoodiesFiller )
DECLARE_ALGORITHM_FACTORY( METCollisionFilter )
DECLARE_ALGORITHM_FACTORY( SkimDecisionFilter )
DECLARE_ALGORITHM_FACTORY( MuonGoodiesFiller )
DECLARE_TOOL_FACTORY( EtmissGoodnessManagerTool )
DECLARE_TOOL_FACTORY( JetVarTool )

DECLARE_FACTORY_ENTRIES( MissingETGoodness ) {
    DECLARE_TOOL( EtmissGoodnessManagerTool )
    DECLARE_TOOL( JetVarTool )
    DECLARE_ALGORITHM( EtmissGoodnessManagerAlg )
    DECLARE_ALGORITHM( EtmissGoodnessSelectorAlg )
    DECLARE_ALGORITHM( JetGoodiesFiller )
    DECLARE_ALGORITHM( MetGoodiesFiller )
    DECLARE_ALGORITHM( METCollisionFilter )
    DECLARE_ALGORITHM( SkimDecisionFilter )
    DECLARE_ALGORITHM( MuonGoodiesFiller )
}

