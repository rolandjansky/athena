/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILETPCNV_TILETPCNVDICT_H
#define TILETPCNV_TILETPCNVDICT_H

#include "TileTPCnv/TileHit_p1.h"
#include "TileTPCnv/TileHitVector_p1.h"
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

#include "TileTPCnv/TileBeamElem_p1.h"
#include "TileTPCnv/TileBeamElemContainer_p1.h"
#include "TileTPCnv/TileBeamElemContainerCnv_p1.h"

#include "TileTPCnv/TileRawChannel_p1.h"
#include "TileTPCnv/TileRawChannelContainer_p1.h"
#include "TileTPCnv/TileRawChannelContainerCnv_p1.h"

#include "TileTPCnv/TileMuonReceiverObj_p1.h"
#include "TileTPCnv/TileMuonReceiverContainer_p1.h"
#include "TileTPCnv/TileMuonReceiverContainerCnv_p1.h"

struct dummy {
  T_TPCnv<TileHitContainer, TileHitContainer_p1> m_tilehitcnv;
  T_TPCnv<TileTTL1Container, TileTTL1Container_p1> m_tilettl1cnv;
  T_TPCnv<TileL2Container, TileL2Container_p1> m_tilel2cnv1;
  T_TPCnv<TileL2Container, TileL2Container_p2> m_tilel2cnv2;
  T_TPCnv<TileMuContainer, TileMuContainer_p1> m_tilemucnv;
  T_TPCnv<TileCosmicMuonContainer, TileCosmicMuonContainer_p1> m_tilecosmicmuoncnv1;
  T_TPCnv<TileCosmicMuonContainer, TileCosmicMuonContainer_p2> m_tilecosmicmuoncnv2;
  T_TPCnv<TileDigitsContainer, TileDigitsContainer_p1> m_tiledigitscnv;
  T_TPCnv<TileBeamElemContainer, TileBeamElemContainer_p1> m_tilebeamelemcnv;
  T_TPCnv<TileRawChannelContainer, TileRawChannelContainer_p1> m_tilerawchannelcnv;
  T_TPCnv<TileMuonReceiverContainer, TileMuonReceiverContainer_p1> m_tilemuonreceivercnv;
};

#endif
