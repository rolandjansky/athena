#include "GaudiKernel/DeclareFactoryEntries.h"
#include "BeamHaloGenerator/BeamHaloGeneratorAlg.h"

DECLARE_ALGORITHM_FACTORY(BeamHaloGeneratorAlg)
  
DECLARE_FACTORY_ENTRIES(BeamHaloGenerator) {
  DECLARE_ALGORITHM(BeamHaloGeneratorAlg)
}
