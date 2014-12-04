#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MDT_PrepDataToxAOD.h"
#include "../RPC_PrepDataToxAOD.h"
#include "../TGC_PrepDataToxAOD.h"
#include "../CSC_PrepDataToxAOD.h"
#include "../RPC_RDOToxAOD.h"


using namespace Muon;

DECLARE_ALGORITHM_FACTORY( MDT_PrepDataToxAOD )
DECLARE_ALGORITHM_FACTORY( RPC_PrepDataToxAOD )
DECLARE_ALGORITHM_FACTORY( TGC_PrepDataToxAOD )
DECLARE_ALGORITHM_FACTORY( CSC_PrepDataToxAOD )
DECLARE_ALGORITHM_FACTORY( RPC_RDOToxAOD )

DECLARE_FACTORY_ENTRIES( MuonPrepRawDataToxAOD ) 
{
  DECLARE_ALGORITHM( MDT_PrepDataToxAOD )
  DECLARE_ALGORITHM( RPC_PrepDataToxAOD )
  DECLARE_ALGORITHM( TGC_PrepDataToxAOD )
  DECLARE_ALGORITHM( CSC_PrepDataToxAOD )
  DECLARE_ALGORITHM( RPC_RDOToxAOD )
}
