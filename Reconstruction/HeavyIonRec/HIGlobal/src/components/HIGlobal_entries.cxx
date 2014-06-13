#include "HIGlobal/ReadSiClusters.h"
#include "HIGlobal/MakePixelTracklets.h"
#include "HIGlobal/HICentrality.h"
#include "HIGlobal/PrintHICentralityData.h"
#include "HIGlobal/HIGlobalEt.h"
#include "HIGlobal/PPGlobalEt.h"
#include "HIGlobal/HIGlobalNSiCluster.h"
#include "HIGlobal/HIPixelTrackletsMaker.h"
#include "HIGlobal/HIPixelTrackletsTruthMaker.h"
#include "HIGlobal/HITRT.h"
#include "HIGlobal/PrintHITRTInfo.h"
#include "HIGlobal/HIFlow.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_ALGORITHM_FACTORY( ReadSiClusters )
DECLARE_ALGORITHM_FACTORY( MakePixelTracklets )
DECLARE_ALGORITHM_FACTORY( HICentrality )
DECLARE_ALGORITHM_FACTORY( PrintHICentralityData )
DECLARE_ALGORITHM_FACTORY( HIGlobalEt )
DECLARE_ALGORITHM_FACTORY( PPGlobalEt )
DECLARE_ALGORITHM_FACTORY( HIGlobalNSiCluster )
DECLARE_ALGORITHM_FACTORY( HIPixelTrackletsTruthMaker )
DECLARE_ALGORITHM_FACTORY( HIPixelTrackletsMaker )
DECLARE_ALGORITHM_FACTORY( HITRT )
DECLARE_ALGORITHM_FACTORY( PrintHITRTInfo )
DECLARE_ALGORITHM_FACTORY( HIFlow )
DECLARE_FACTORY_ENTRIES( HIGlobal ) {
    DECLARE_ALGORITHM( ReadSiClusters );
    DECLARE_ALGORITHM( MakePixelTracklets );
    DECLARE_ALGORITHM( HICentrality );
    DECLARE_ALGORITHM( PrintHICentralityData );
    DECLARE_ALGORITHM( HIGlobalEt );
    DECLARE_ALGORITHM( PPGlobalEt );
    DECLARE_ALGORITHM( HIGlobalNSiCluster );
    DECLARE_ALGORITHM( HIPixelTrackletsTruthMaker );
    DECLARE_ALGORITHM( HIPixelTrackletsMaker );
    DECLARE_ALGORITHM( HITRT );
    DECLARE_ALGORITHM( PrintHITRTInfo );
    DECLARE_ALGORITHM( HIFlow );
}

