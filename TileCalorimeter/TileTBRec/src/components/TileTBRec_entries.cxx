
#include "TileTBRec/TileTBNtuple.h"
#include "TileTBRec/TileTBAANtuple.h"
#include "TileTBRec/TileTBStat.h"
#include "TileTBRec/TileTBDump.h"
#include "TileTBRec/TileTBOldNtupleRead.h"
#include "TileTBRec/TileTBOldNtupleWrite.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TileTBNtuple )
DECLARE_ALGORITHM_FACTORY( TileTBAANtuple )
DECLARE_ALGORITHM_FACTORY( TileTBStat )
DECLARE_ALGORITHM_FACTORY( TileTBDump )
DECLARE_ALGORITHM_FACTORY( TileTBOldNtupleRead )
DECLARE_ALGORITHM_FACTORY( TileTBOldNtupleWrite )	  	    

DECLARE_FACTORY_ENTRIES(TileTBRec) {
  DECLARE_ALGORITHM( TileTBNtuple )
  DECLARE_ALGORITHM( TileTBAANtuple )
  DECLARE_ALGORITHM( TileTBStat )
  DECLARE_ALGORITHM( TileTBDump )
  DECLARE_ALGORITHM( TileTBOldNtupleRead )
  DECLARE_ALGORITHM( TileTBOldNtupleWrite )	  	    
}
