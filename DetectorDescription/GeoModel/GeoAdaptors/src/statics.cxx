/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoAdaptors/GeoLArHit.h"
#include "GeoAdaptors/GeoSiHit.h"
#include "GeoAdaptors/GeoMuonHits.h"
#include "GeoAdaptors/GeoTRTUncompressedHit.h"

const InDetDD::SCT_DetectorManager  *GeoSiHit::_sct = 0;
const InDetDD::PixelDetectorManager *GeoSiHit::_pix = 0;
const HGTD_DetectorManager          *GeoSiHit::_hgtd = 0;
const PixelID                       *GeoSiHit::_pID = 0;
const SCT_ID                        *GeoSiHit::_sID = 0;
const HGTD_ID                       *GeoSiHit::_hID = 0;


const MuonGM::MuonDetectorManager* GeoMDTHit::_man = 0;
const MdtIdHelper* GeoMDTHit::_mdtID = 0;

const MuonGM::MuonDetectorManager* GeoTGCHit::_man = 0;
const TgcIdHelper* GeoTGCHit::_tgcID = 0;

const MuonGM::MuonDetectorManager* GeoRPCHit::_man = 0;
const RpcIdHelper* GeoRPCHit::_rpcID = 0;

const MuonGM::MuonDetectorManager* GeoCSCHit::_man = 0;
const CscIdHelper* GeoCSCHit::_cscID = 0;

const MuonGM::MuonDetectorManager* GeosTGCHit::_man = 0;
const sTgcIdHelper* GeosTGCHit::_stgcID = 0;

const MuonGM::MuonDetectorManager* GeoMMHit::_man = 0;
const MmIdHelper* GeoMMHit::_mmID = 0;

const CaloDetDescrManager*  GeoLArHit::_man = 0;

const InDetDD::TRT_DetectorManager *GeoTRTUncompressedHit::_mgr = 0;

