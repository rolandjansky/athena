#include "GaudiKernel/DeclareFactoryEntries.h"
#include "RecExAlgs/SimpleRawObjMaker.h"
#include "RecExAlgs/SimpleCookedObjMaker.h"


DECLARE_ALGORITHM_FACTORY( SimpleRawObjMaker )
DECLARE_ALGORITHM_FACTORY( SimpleCookedObjMaker )

DECLARE_FACTORY_ENTRIES( RecExAlgs ) {
  DECLARE_ALGORITHM( SimpleRawObjMaker );
  DECLARE_ALGORITHM( SimpleCookedObjMaker );
}   
