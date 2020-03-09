#include "TrkVertexFitters/SequentialVertexFitter.h"
#include "TrkVertexFitters/AdaptiveMultiVertexFitter.h"
#include "TrkVertexFitters/AdaptiveVertexFitter.h"
#include "TrkVertexFitters/SequentialVertexSmoother.h"
#include "TrkVertexFitters/DummyVertexSmoother.h"
#include "../AdaptiveVertexFitterTestAlg.h"
#include "../AdaptiveMultiVertexFitterTestAlg.h"

using namespace Trk;

DECLARE_COMPONENT( SequentialVertexFitter )
DECLARE_COMPONENT( AdaptiveMultiVertexFitter )
DECLARE_COMPONENT( SequentialVertexSmoother )
DECLARE_COMPONENT( AdaptiveVertexFitter )
DECLARE_COMPONENT( DummyVertexSmoother )
DECLARE_COMPONENT( AdaptiveVertexFitterTestAlg )
DECLARE_COMPONENT( AdaptiveMultiVertexFitterTestAlg )

