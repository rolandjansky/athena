#include "GaudiKernel/LoadFactoryEntries.h"

LOAD_FACTORY_ENTRIES(LArRecUtils)

//needed for LArCellCorrection, which is the base class of LArG3Escale; 
//LArG3Escale is declared as a factory in LArCellRec
