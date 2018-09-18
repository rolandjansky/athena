#include "Pythia8_i/Pythia8_i.h"
#include "../Pythia8Custom/HllgamRepeatTimeShower.h"
#include "../Pythia8Custom/UserPhotonFlux.h"
#include "../Pythia8Custom/CheckForFinalPartons.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

/**
 * author: James Monk (jmonk@hep.ucl.ac.uk)
 */

DECLARE_ALGORITHM_FACTORY( Pythia8_i )
DECLARE_TOOL_FACTORY( HllgamRepeatTimeShower )
DECLARE_TOOL_FACTORY( UserPhotonFlux )
DECLARE_TOOL_FACTORY( CheckForFinalPartons )
DECLARE_FACTORY_ENTRIES( Pythia8_i ){
  DECLARE_ALGORITHM( Pythia8_i )
}
