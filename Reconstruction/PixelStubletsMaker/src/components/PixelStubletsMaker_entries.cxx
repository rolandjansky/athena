#include "GaudiKernel/DeclareFactoryEntries.h"
#include "PixelStubletsMaker/PixelStubletsMakerAlg.h"
 
DECLARE_ALGORITHM_FACTORY(PixelStubletsMakerAlg)
 
DECLARE_FACTORY_ENTRIES(PixelStubletsMaker) {
  DECLARE_ALGORITHM(PixelStubletsMakerAlg)
}
