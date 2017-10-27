#include "TrkVertexFitters/SequentialVertexFitter.h"
#include "TrkVertexFitters/AdaptiveMultiVertexFitter.h"
#include "TrkVertexFitters/AdaptiveVertexFitter.h"
#include "TrkVertexFitters/SequentialVertexSmoother.h"
#include "TrkVertexFitters/DummyVertexSmoother.h"

using namespace Trk;

DECLARE_COMPONENT( SequentialVertexFitter )
DECLARE_COMPONENT( AdaptiveMultiVertexFitter )
DECLARE_COMPONENT( SequentialVertexSmoother )
DECLARE_COMPONENT( AdaptiveVertexFitter )
DECLARE_COMPONENT( DummyVertexSmoother )

