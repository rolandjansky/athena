/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileCalibType.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/TileBchDecoder.h"
#include "TileCalibBlobObjs/TileBchPrbs.h"
#include "TileCalibBlobObjs/TileBchStatus.h"
#include "TileCalibBlobObjs/TileBchWords.h"

#include "TileCalibBlobObjs/TileCalibDrawerBase.h"
#include "TileCalibBlobObjs/TileCalibDrawerCmt.h"
#include "TileCalibBlobObjs/TileCalibDrawerOfc.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileCalibBlobObjs/TileCalibDrawerBch.h"
#include "TileCalibBlobObjs/TileCalibDrawerInt.h"

//==== create a dummy entry for the set
#include <set>
class TileCalibBlobObjsDict_Dummy {
 std::set<TileBchPrbs::Prb>                           m_set_TileBchPrbs__Prb_dummy;
 std::set<TileBchPrbs::Prb>::iterator                 m_set_TileBchPrbs__Prb_dummy_iter;
 std::pair<std::set<TileBchPrbs::Prb>::iterator,bool> m_set_TileBchPrbs__Prb_dummy_pair;
};
