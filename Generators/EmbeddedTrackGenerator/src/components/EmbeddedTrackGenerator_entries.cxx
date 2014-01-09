#include "GaudiKernel/DeclareFactoryEntries.h"
#include "EmbeddedTrackGenerator/EmbeddedTrackGenerator.h"
#include "EmbeddedTrackGenerator/VertexedTrackGenerator.h"

DECLARE_ALGORITHM_FACTORY( EmbeddedTrackGenerator )
DECLARE_ALGORITHM_FACTORY( VertexedTrackGenerator )
  
DECLARE_FACTORY_ENTRIES(EmbeddedTrackGenerator) {
  DECLARE_ALGORITHM( EmbeddedTrackGenerator )
  DECLARE_ALGORITHM( VertexedTrackGenerator )
}
