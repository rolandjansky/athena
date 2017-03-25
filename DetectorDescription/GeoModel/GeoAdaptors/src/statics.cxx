/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoAdaptors/GeoLArHit.h"
#include "GeoAdaptors/GeoSiHit.h"
#include "GeoAdaptors/GeoMuonHits.h"
#include "GeoAdaptors/GeoTRTUncompressedHit.h"

const InDetDD::SCT_DetectorManager  *GeoSiHit::s_sct = 0;
const InDetDD::PixelDetectorManager *GeoSiHit::s_pix = 0;
const PixelID                       *GeoSiHit::s_pID = 0;
const SCT_ID                        *GeoSiHit::s_sID = 0;


const MuonGM::MuonDetectorManager* GeoMDTHit::s_man = 0;
const MdtIdHelper* GeoMDTHit::s_mdtID = 0;

const MuonGM::MuonDetectorManager* GeoTGCHit::s_man = 0;
const TgcIdHelper* GeoTGCHit::s_tgcID = 0;

const MuonGM::MuonDetectorManager* GeoRPCHit::s_man = 0;
const RpcIdHelper* GeoRPCHit::s_rpcID = 0;

const MuonGM::MuonDetectorManager* GeoCSCHit::s_man = 0;
const CscIdHelper* GeoCSCHit::s_cscID = 0;

const MuonGM::MuonDetectorManager* GeosTGCHit::s_man = 0;
const sTgcIdHelper* GeosTGCHit::s_stgcID = 0;

const MuonGM::MuonDetectorManager* GeoMMHit::s_man = 0;
const MmIdHelper* GeoMMHit::s_mmID = 0;

const CaloDetDescrManager*  GeoLArHit::s_man = 0;

const InDetDD::TRT_DetectorManager *GeoTRTUncompressedHit::s_mgr = 0;

