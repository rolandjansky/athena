#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../TRT_RDOAnalysis.h"
#include "../PixelRDOAnalysis.h"
#include "../SCT_RDOAnalysis.h"
#include "../BCM_RDOAnalysis.h"
#include "../LArRDOAnalysis.h"
#include "../TileRDOAnalysis.h"
#include "../RPC_RDOAnalysis.h"
#include "../TGC_RDOAnalysis.h"
#include "../CSC_RDOAnalysis.h"
#include "../MDT_RDOAnalysis.h"
#include "../TRT_FastRDOAnalysis.h"
#include "../PixelFastRDOAnalysis.h"
#include "../SCT_FastRDOAnalysis.h"

DECLARE_ALGORITHM_FACTORY( TRT_RDOAnalysis )
DECLARE_ALGORITHM_FACTORY( PixelRDOAnalysis )
DECLARE_ALGORITHM_FACTORY( SCT_RDOAnalysis )
DECLARE_ALGORITHM_FACTORY( BCM_RDOAnalysis )
DECLARE_ALGORITHM_FACTORY( LArRDOAnalysis )
DECLARE_ALGORITHM_FACTORY( TileRDOAnalysis )
DECLARE_ALGORITHM_FACTORY( RPC_RDOAnalysis )
DECLARE_ALGORITHM_FACTORY( TGC_RDOAnalysis )
DECLARE_ALGORITHM_FACTORY( CSC_RDOAnalysis )
DECLARE_ALGORITHM_FACTORY( MDT_RDOAnalysis )
DECLARE_ALGORITHM_FACTORY( TRT_FastRDOAnalysis )
DECLARE_ALGORITHM_FACTORY( PixelFastRDOAnalysis )
DECLARE_ALGORITHM_FACTORY( SCT_FastRDOAnalysis )

DECLARE_FACTORY_ENTRIES( RDOAnalysis ) {
  DECLARE_ALGORITHM( TRT_RDOAnalysis )
  DECLARE_ALGORITHM( PixelRDOAnalysis )
  DECLARE_ALGORITHM( SCT_RDOAnalysis )
  DECLARE_ALGORITHM( BCM_RDOAnalysis )
  DECLARE_ALGORITHM( LArRDOAnalysis )
  DECLARE_ALGORITHM( TileRDOAnalysis )
  DECLARE_ALGORITHM( RPC_RDOAnalysis )
  DECLARE_ALGORITHM( TGC_RDOAnalysis )
  DECLARE_ALGORITHM( CSC_RDOAnalysis )
  DECLARE_ALGORITHM( MDT_RDOAnalysis )
  DECLARE_ALGORITHM( TRT_FastRDOAnalysis )
  DECLARE_ALGORITHM( PixelFastRDOAnalysis )
  DECLARE_ALGORITHM( SCT_FastRDOAnalysis )
}
