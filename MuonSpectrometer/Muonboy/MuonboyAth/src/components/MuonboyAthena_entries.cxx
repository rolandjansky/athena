#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonboyAth/MboyDigiEmptyLoop.h"
#include "MuonboyAth/MboyRec.h"
#include "MuonboyAth/MboyMuonChamberT0s.h"
#include "MuonboyAth/MboyChecks.h"
#include "MuonboyAth/MboySegmentCheck.h"

DECLARE_ALGORITHM_FACTORY( MboyDigiEmptyLoop )
DECLARE_ALGORITHM_FACTORY( MboyRec )
DECLARE_ALGORITHM_FACTORY( MboyMuonChamberT0s )
DECLARE_ALGORITHM_FACTORY( MboyChecks  )
DECLARE_ALGORITHM_FACTORY( MboySegmentCheck  )

DECLARE_FACTORY_ENTRIES( MuonboyAth ) {
        DECLARE_ALGORITHM( MboyDigiEmptyLoop );
        DECLARE_ALGORITHM( MboyRec );
        DECLARE_ALGORITHM( MboyMuonChamberT0s );
        DECLARE_ALGORITHM( MboyChecks );
        DECLARE_ALGORITHM( MboySegmentCheck );
}                                                   
