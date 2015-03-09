#include "LArEventTest/CheckLArFebHeader.h"
#include "LArEventTest/DumpLArDigits.h"
#include "LArEventTest/DumpLArRawChannels.h"
#include "LArEventTest/FakeLArOFCs.h"
#include "LArEventTest/FakeLArTimeOffset.h"
#include "LArEventTest/FakeTBPhase.h"
#include "LArEventTest/LArDigitsToNtuple.h"
#include "LArEventTest/TriggerPatternCount.h"
#include "LArEventTest/CaloClusterNavTest.h"
#include "LArEventTest/FindDuplicatedLArDigits.h"


#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( CheckLArFebHeader )
DECLARE_ALGORITHM_FACTORY( DumpLArDigits )
DECLARE_ALGORITHM_FACTORY( DumpLArRawChannels )
DECLARE_ALGORITHM_FACTORY( FakeLArOFCs )
DECLARE_ALGORITHM_FACTORY( FakeLArTimeOffset )
DECLARE_ALGORITHM_FACTORY( FakeTBPhase )
DECLARE_ALGORITHM_FACTORY( LArDigitsToNtuple )
  DECLARE_ALGORITHM_FACTORY( CaloClusterNavTest )
  DECLARE_ALGORITHM_FACTORY(FindDuplicatedLArDigits)


                           
DECLARE_FACTORY_ENTRIES(LArEventTest) 
{
  DECLARE_ALGORITHM( CheckLArFebHeader )
  DECLARE_ALGORITHM( DumpLArDigits )
  DECLARE_ALGORITHM( DumpLArRawChannels )
  DECLARE_ALGORITHM( FakeLArOFCs )
  DECLARE_ALGORITHM( FakeLArTimeOffset )
  DECLARE_ALGORITHM( FakeTBPhase )
  DECLARE_ALGORITHM( LArDigitsToNtuple )
  DECLARE_ALGORITHM( CaloClusterNavTest )
    DECLARE_ALGORITHM( FindDuplicatedLArDigits )
}
