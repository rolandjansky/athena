#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DetDescrCondExample/ReadAlignTrans.h"
#include "DetDescrCondExample/ReadTopTrans.h"
#include "DetDescrCondExample/ReadWriteCoolStrFile.h"
#include "DetDescrCondExample/CoolHistExample.h"

DECLARE_ALGORITHM_FACTORY( ReadAlignTrans )
DECLARE_ALGORITHM_FACTORY( ReadTopTrans )
DECLARE_ALGORITHM_FACTORY( ReadWriteCoolStrFile )
DECLARE_ALGORITHM_FACTORY( CoolHistExample )

DECLARE_FACTORY_ENTRIES( DetDescrCondExample )
{
    DECLARE_ALGORITHM( ReadAlignTrans )
    DECLARE_ALGORITHM( ReadTopTrans )
    DECLARE_ALGORITHM( ReadWriteCoolStrFile )
    DECLARE_ALGORITHM( CoolHistExample )
}
