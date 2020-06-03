/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TileTPCnv/TileHitContainer_p1.h"
#include "TileTPCnv/TileHitContainerCnv_p1.h"

#include "TileTPCnv/TileTTL1_p1.h"
#include "TileTPCnv/TileTTL1Container_p1.h"
#include "TileTPCnv/TileTTL1ContainerCnv_p1.h"

#include "TileTPCnv/TileL2_p1.h"
#include "TileTPCnv/TileL2Container_p1.h"
#include "TileTPCnv/TileL2ContainerCnv_p1.h"

#include "TileTPCnv/TileL2_p2.h"
#include "TileTPCnv/TileL2Container_p2.h"
#include "TileTPCnv/TileL2ContainerCnv_p2.h"

#include "TileTPCnv/TileMu_p1.h"
#include "TileTPCnv/TileMuContainer_p1.h"
#include "TileTPCnv/TileMuContainerCnv_p1.h"

#include "TileTPCnv/TileCosmicMuon_p1.h"
#include "TileTPCnv/TileCosmicMuonContainer_p1.h"
#include "TileTPCnv/TileCosmicMuonContainerCnv_p1.h"

#include "TileTPCnv/TileCosmicMuon_p2.h"
#include "TileTPCnv/TileCosmicMuonContainer_p2.h"
#include "TileTPCnv/TileCosmicMuonContainerCnv_p2.h"

#include "TileTPCnv/TileDigits_p1.h"
#include "TileTPCnv/TileDigitsContainer_p1.h"
#include "TileTPCnv/TileDigitsContainerCnv_p1.h"

#include "TileTPCnv/TileDigits_p2.h"
#include "TileTPCnv/TileDigitsContainer_p2.h"
#include "TileTPCnv/TileDigitsContainerCnv_p2.h"

#include "TileTPCnv/TileDigits_p3.h"
#include "TileTPCnv/TileDigitsContainer_p3.h"
#include "TileTPCnv/TileDigitsContainerCnv_p3.h"

#include "TileTPCnv/TileBeamElem_p1.h"
#include "TileTPCnv/TileBeamElemContainer_p1.h"
#include "TileTPCnv/TileBeamElemContainerCnv_p1.h"

#include "TileTPCnv/TileRawChannel_p1.h"
#include "TileTPCnv/TileRawChannelContainer_p1.h"
#include "TileTPCnv/TileRawChannelContainerCnv_p1.h"

#include "TileTPCnv/TileMuonReceiverObj_p1.h"
#include "TileTPCnv/TileMuonReceiverContainer_p1.h"
#include "TileTPCnv/TileMuonReceiverContainerCnv_p1.h"

DECLARE_NAMED_TPCNV_FACTORY(TileBeamElemContainerCnv_p1,
                            TileBeamElemContainerCnv_p1,
                            TileBeamElemContainer,
                            TileBeamElemContainer_p1,
                            Athena::TPCnvVers::Current)
                      
DECLARE_NAMED_TPCNV_FACTORY(TileCosmicMuonContainerCnv_p1,
                            TileCosmicMuonContainerCnv_p1,
                            TileContainer<TileCosmicMuon>,
                            TileCosmicMuonContainer_p1,
                            Athena::TPCnvVers::Old)
                      
DECLARE_NAMED_TPCNV_FACTORY(TileCosmicMuonContainerCnv_p2,
                            TileCosmicMuonContainerCnv_p2,
                            TileContainer<TileCosmicMuon>,
                            TileCosmicMuonContainer_p2,
                            Athena::TPCnvVers::Current)
                      
DECLARE_NAMED_TPCNV_FACTORY(TileL2ContainerCnv_p1,
                            TileL2ContainerCnv_p1,
                            TileContainer<TileL2>,
                            TileL2Container_p1,
                            Athena::TPCnvVers::Old)
                      
DECLARE_NAMED_TPCNV_FACTORY(TileL2ContainerCnv_p2,
                            TileL2ContainerCnv_p2,
                            TileContainer<TileL2>,
                            TileL2Container_p2,
                            Athena::TPCnvVers::Current)
                      
DECLARE_NAMED_TPCNV_FACTORY(TileMuContainerCnv_p1,
                            TileMuContainerCnv_p1,
                            TileContainer<TileMu>,
                            TileMuContainer_p1,
                            Athena::TPCnvVers::Current)
                      
DECLARE_NAMED_TPCNV_FACTORY(TileTTL1ContainerCnv_p1,
                            TileTTL1ContainerCnv_p1,
                            TileContainer<TileTTL1>,
                            TileTTL1Container_p1,
                            Athena::TPCnvVers::Current)
                      
DECLARE_NAMED_TPCNV_FACTORY(TileDigitsContainerCnv_p1,
                            TileDigitsContainerCnv_p1,
                            TileDigitsContainer,
                            TileDigitsContainer_p1,
                            Athena::TPCnvVers::Old)
                      
DECLARE_NAMED_TPCNV_FACTORY(TileDigitsContainerCnv_p2,
                            TileDigitsContainerCnv_p2,
                            TileDigitsContainer,
                            TileDigitsContainer_p2,
                            Athena::TPCnvVers::Old)

DECLARE_NAMED_TPCNV_FACTORY(TileDigitsContainerCnv_p3,
                            TileDigitsContainerCnv_p3,
                            TileDigitsContainer,
                            TileDigitsContainer_p3,
                            Athena::TPCnvVers::Current)

DECLARE_NAMED_TPCNV_FACTORY(TileHitContainerCnv_p1,
                            TileHitContainerCnv_p1,
                            TileHitContainer,
                            TileHitContainer_p1,
                            Athena::TPCnvVers::Current)
                      
DECLARE_NAMED_TPCNV_FACTORY(TileRawChannelContainerCnv_p1,
                            TileRawChannelContainerCnv_p1,
                            TileRawChannelContainer,
                            TileRawChannelContainer_p1,
                            Athena::TPCnvVers::Current)

DECLARE_NAMED_TPCNV_FACTORY(TileMuonReceiverContainerCnv_p1,
			    TileMuonReceiverContainerCnv_p1,
			    TileContainer<TileMuonReceiverObj>,
			    TileMuonReceiverContainer_p1,
			    Athena::TPCnvVers::Current)
