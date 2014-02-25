#include "Pythia8B_i/Pythia8B_i.h"
#include "Pythia8B_i/CheckCloningFactor.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

/**
 * author: James Catmore (James.Catmore@cern.ch)
 */

DECLARE_ALGORITHM_FACTORY( Pythia8B_i )
DECLARE_ALGORITHM_FACTORY( CheckCloningFactor )
DECLARE_FACTORY_ENTRIES( Pythia8B_i ){
  DECLARE_ALGORITHM( Pythia8B_i )
  DECLARE_ALGORITHM( CheckCloningFactor )
}
