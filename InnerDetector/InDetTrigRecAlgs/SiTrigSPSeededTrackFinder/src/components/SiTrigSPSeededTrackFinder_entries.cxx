#include "GaudiKernel/DeclareFactoryEntries.h"
#include "SiTrigSPSeededTrackFinder/SiTrigSPSeededTrackFinder.h"

#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "InDetRecToolInterfaces/ISiZvertexMaker.h" 
#include "InDetRecToolInterfaces/ISiTrackMaker.h" 
#include "IRegionSelector/IRegSelSvc.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( SiTrigSPSeededTrackFinder )

DECLARE_FACTORY_ENTRIES( SiTrigSPSeededTrackFinder )
{
  DECLARE_ALGORITHM( SiTrigSPSeededTrackFinder )
}

