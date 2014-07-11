#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkVxEdmCnv/VxCandidateXAODVertex.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( VxCandidateXAODVertex )

DECLARE_FACTORY_ENTRIES( TrkVxEdmCnv )
{
  DECLARE_TOOL( VxCandidateXAODVertex )
}
