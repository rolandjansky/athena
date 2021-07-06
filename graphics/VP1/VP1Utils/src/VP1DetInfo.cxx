/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1DetInfo                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Utils/VP1DetInfo.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1AthenaPtrs.h"

#include "GeoModelKernel/GeoPVConstLink.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "LArReadoutGeometry/EMBDetectorManager.h"
#include "LArReadoutGeometry/HECDetectorManager.h"
#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "LArReadoutGeometry/LArDetectorManager.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "CaloTTDetDescr/CaloTTDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "Identifier/Identifier.h"

//____________________________________________________________________
class VP1DetInfo::Imp {
public:
  template <class T>
  static const T * cachedRetrieve(const T*& cachedPtr, const char* key, const bool& configallows );

  static bool m_initialised;
  static const char m_badInitFlag;//Address of this means bad initialisation.

  static const InDetDD::PixelDetectorManager * m_pixelDetMgr;
  static const InDetDD::SCT_DetectorManager * m_sctDetMgr;
  static const InDetDD::TRT_DetectorManager * m_trtDetMgr;
  //  static const InDetDD::InDetServMatManager * m_inDetServMatDetMgr;
  static const MuonGM::MuonDetectorManager * m_muonDetMgr;
  static const TileDetDescrManager * m_tileDetMgr;
  static const EMBDetectorManager * m_embDetMgr;
  static const HECDetectorManager * m_hecDetMgr;
  static const EMECDetectorManager * m_emecDetMgr;
  static const LArDetectorManager * m_larDetMgr;
  static const FCALDetectorManager * m_fcalDetMgr;
  static const CaloTTDescrManager * m_calottDetMgr;
  static const CaloIdManager * m_caloIdMgr;
  //  static const LUCID_DetectorManager * m_lucidDetMgr;
  //  static const BeamPipeDetectorManager * m_beamPipeDetMgr;
  //  static const CavernInfraDetectorManager * m_cavernInfraDetMgr;

  static const AtlasDetectorID * m_atlasIDHelper;
  static const CscIdHelper * m_cscIDHelper;
  static const RpcIdHelper * m_rpcIDHelper;
  static const TgcIdHelper * m_tgcIDHelper;
  static const sTgcIdHelper * m_stgcIDHelper;
  static const MdtIdHelper * m_mdtIDHelper;
  static const MmIdHelper * m_mmIDHelper;
  static const PixelID * m_pixelIDHelper;
  static const SCT_ID * m_sctIDHelper;
  static const TRT_ID * m_trtIDHelper;
};

bool VP1DetInfo::Imp::m_initialised = false;
const char VP1DetInfo::Imp::m_badInitFlag = ' ';

const InDetDD::PixelDetectorManager * VP1DetInfo::Imp::m_pixelDetMgr = 0;
const InDetDD::SCT_DetectorManager * VP1DetInfo::Imp::m_sctDetMgr = 0;
const InDetDD::TRT_DetectorManager * VP1DetInfo::Imp::m_trtDetMgr = 0;
//const InDetDD::InDetServMatManager * VP1DetInfo::Imp::m_inDetServMatDetMgr = 0;
const MuonGM::MuonDetectorManager * VP1DetInfo::Imp::m_muonDetMgr = 0;
const TileDetDescrManager * VP1DetInfo::Imp::m_tileDetMgr = 0;
const EMBDetectorManager * VP1DetInfo::Imp::m_embDetMgr = 0;
const HECDetectorManager * VP1DetInfo::Imp::m_hecDetMgr = 0;
const EMECDetectorManager * VP1DetInfo::Imp::m_emecDetMgr = 0;
const LArDetectorManager * VP1DetInfo::Imp::m_larDetMgr = 0;
const FCALDetectorManager * VP1DetInfo::Imp::m_fcalDetMgr = 0;
const CaloTTDescrManager * VP1DetInfo::Imp::m_calottDetMgr = 0;
const CaloIdManager * VP1DetInfo::Imp::m_caloIdMgr = 0;
//const LUCID_DetectorManager * VP1DetInfo::Imp::m_lucidDetMgr = 0;
//const BeamPipeDetectorManager * VP1DetInfo::Imp::m_beamPipeDetMgr = 0;
//const CavernInfraDetectorManager * VP1DetInfo::Imp::m_cavernInfraDetMgr = 0;

const AtlasDetectorID * VP1DetInfo::Imp::m_atlasIDHelper = 0;
const CscIdHelper * VP1DetInfo::Imp::m_cscIDHelper = 0;
const RpcIdHelper * VP1DetInfo::Imp::m_rpcIDHelper = 0;
const TgcIdHelper * VP1DetInfo::Imp::m_tgcIDHelper = 0;
const sTgcIdHelper * VP1DetInfo::Imp::m_stgcIDHelper = 0;
const MdtIdHelper * VP1DetInfo::Imp::m_mdtIDHelper = 0;
const MmIdHelper * VP1DetInfo::Imp::m_mmIDHelper = 0;
const PixelID * VP1DetInfo::Imp::m_pixelIDHelper = 0;
const SCT_ID * VP1DetInfo::Imp::m_sctIDHelper = 0;
const TRT_ID * VP1DetInfo::Imp::m_trtIDHelper = 0;

//____________________________________________________________________
template <class T>
const T * VP1DetInfo::Imp::cachedRetrieve(const T*& cachedPtr, const char* preferredKey, const bool& configallows ) {
  const T * bad = static_cast<const T*>(static_cast<const void*>(&m_badInitFlag));
  if (cachedPtr)
    return ( cachedPtr==bad? 0 : cachedPtr );
  QString key(preferredKey);
  if (!configallows) {
    VP1Msg::messageDebug("VP1DetInfo WARNING: Will not attempt to get (type="+QString(typeid(T).name())+", key="+key+") due to missing/disabled features in job!");
    cachedPtr = bad;
    return 0;
  }
  if (!VP1SGContentsHelper(VP1AthenaPtrs::detectorStore()).contains<T>(key)) {
    //Try to gracefully guess at a different key:
    QStringList keys = VP1SGContentsHelper(VP1AthenaPtrs::detectorStore()).getKeys<T>();
    if (keys.empty()) {
      VP1Msg::messageDebug("VP1DetInfo WARNING: Could not find (type="+QString(typeid(T).name())+") in detector store (expected key="+key+")");
      cachedPtr = bad;
      return 0;
    }
    if (keys.count()>1) {
      VP1Msg::messageDebug("VP1DetInfo WARNING: Could not find (type="+QString(typeid(T).name())+", key="+key+") in detector store, and could not uniquely guess at alternate key.");
      cachedPtr = bad;
      return 0;
    }
    VP1Msg::messageDebug("VP1DetInfo WARNING: Could not find (type="+QString(typeid(T).name())+", key="+key+") in detector store. Trying with key="+keys.first()+")");
    key = keys.first();
  }
  if (!VP1SGAccessHelper(VP1AthenaPtrs::detectorStore()).retrieve(cachedPtr,key)||!cachedPtr) {
    VP1Msg::messageDebug("VP1DetInfo WARNING: Could not retrieve (type="+QString(typeid(T).name())+", key="+key+") from detector store!");
    cachedPtr = bad;
    return 0;
  }
  VP1Msg::messageVerbose("VP1DetInfo Succesfully retrieved (type="+QString(typeid(T).name())+", key="+key+") from detector store!");
  return cachedPtr;

}

const InDetDD::PixelDetectorManager * VP1DetInfo::pixelDetMgr() { return Imp::cachedRetrieve(Imp::m_pixelDetMgr,"Pixel",VP1JobConfigInfo::hasPixelGeometry()); }
const InDetDD::SCT_DetectorManager * VP1DetInfo::sctDetMgr() { return Imp::cachedRetrieve(Imp::m_sctDetMgr,"SCT",VP1JobConfigInfo::hasSCTGeometry()); }
const InDetDD::TRT_DetectorManager * VP1DetInfo::trtDetMgr() { return Imp::cachedRetrieve(Imp::m_trtDetMgr,"TRT",VP1JobConfigInfo::hasTRTGeometry()); }
//const InDetDD::InDetServMatManager * VP1DetInfo::inDetServMatDetMgr() { return Imp::cachedRetrieve(Imp::m_inDetServMatDetMgr,"InDetServMat",VP1JobConfigInfo::hasInDetServiceMaterialGeometry()); }
const MuonGM::MuonDetectorManager * VP1DetInfo::muonDetMgr() { return Imp::cachedRetrieve(Imp::m_muonDetMgr,"Muon",VP1JobConfigInfo::hasMuonGeometry()); }
const TileDetDescrManager * VP1DetInfo::tileDetMgr() { return Imp::cachedRetrieve(Imp::m_tileDetMgr,"Tile",VP1JobConfigInfo::hasTileGeometry()); }
const EMBDetectorManager * VP1DetInfo::embDetMgr() { return Imp::cachedRetrieve(Imp::m_embDetMgr,"LArEMB",VP1JobConfigInfo::hasLArGeometry()); }
const HECDetectorManager * VP1DetInfo::hecDetMgr() { return Imp::cachedRetrieve(Imp::m_hecDetMgr,"LArHEC",VP1JobConfigInfo::hasLArGeometry()); }
const EMECDetectorManager * VP1DetInfo::emecDetMgr() { return Imp::cachedRetrieve(Imp::m_emecDetMgr,"LArEMEC",VP1JobConfigInfo::hasLArGeometry()); }
const LArDetectorManager * VP1DetInfo::larDetMgr() { return Imp::cachedRetrieve(Imp::m_larDetMgr,"LArMgr",VP1JobConfigInfo::hasLArGeometry()); }
const FCALDetectorManager * VP1DetInfo::fcalDetMgr() { return Imp::cachedRetrieve(Imp::m_fcalDetMgr,"LArFCAL",VP1JobConfigInfo::hasLArGeometry()); }
const CaloTTDescrManager * VP1DetInfo::calottDetMgr() { return Imp::cachedRetrieve(Imp::m_calottDetMgr,"CaloTTMgr",VP1JobConfigInfo::hasLArGeometry()&&VP1JobConfigInfo::hasTileGeometry()); }
const CaloIdManager * VP1DetInfo::caloIDMgr() { return Imp::cachedRetrieve(Imp::m_caloIdMgr,"CaloIdManager",VP1JobConfigInfo::hasLArGeometry()&&VP1JobConfigInfo::hasTileGeometry()); }

//const LUCID_DetectorManager * VP1DetInfo::lucidDetMgr() { return Imp::cachedRetrieve(Imp::m_lucidDetMgr,"LUCID",VP1JobConfigInfo::hasLUCIDGeometry()); }//Fixme: Get correct key!
//const BeamPipeDetectorManager * VP1DetInfo::beamPipeDetMgr() { return Imp::cachedRetrieve(Imp::m_beamPipeDetMgr,"BeamPipe",VP1JobConfigInfo::hasBeamPipeGeometry()); }
//const CavernInfraDetectorManager * VP1DetInfo::cavernInfraDetMgr() { return Imp::cachedRetrieve(Imp::m_cavernInfraDetMgr,"CavernInfra",VP1JobConfigInfo::hasCavernInfraGeometry()); }//Fixme: Get correct key!
const AtlasDetectorID * VP1DetInfo::atlasIDHelper() { return Imp::cachedRetrieve(Imp::m_atlasIDHelper,"AtlasID",true); }
const CscIdHelper * VP1DetInfo::cscIDHelper() { return Imp::cachedRetrieve(Imp::m_cscIDHelper,"CSCIDHELPER",VP1JobConfigInfo::hasMuonGeometry()); }
const RpcIdHelper * VP1DetInfo::rpcIDHelper() { return Imp::cachedRetrieve(Imp::m_rpcIDHelper,"RPCIDHELPER",VP1JobConfigInfo::hasMuonGeometry()); }
const TgcIdHelper * VP1DetInfo::tgcIDHelper() { return Imp::cachedRetrieve(Imp::m_tgcIDHelper,"TGCIDHELPER",VP1JobConfigInfo::hasMuonGeometry()); }
const sTgcIdHelper * VP1DetInfo::stgcIDHelper() { return Imp::cachedRetrieve(Imp::m_stgcIDHelper,"STGCIDHELPER",VP1JobConfigInfo::hasMuonGeometry() && VP1JobConfigInfo::hasMuonNSWGeometry()); }
const MmIdHelper * VP1DetInfo::mmIDHelper() { return Imp::cachedRetrieve(Imp::m_mmIDHelper,"MMIDHELPER",VP1JobConfigInfo::hasMuonGeometry() && VP1JobConfigInfo::hasMuonNSWGeometry()); }
const MdtIdHelper * VP1DetInfo::mdtIDHelper() { return Imp::cachedRetrieve(Imp::m_mdtIDHelper,"MDTIDHELPER",VP1JobConfigInfo::hasMuonGeometry() ); }
const PixelID * VP1DetInfo::pixelIDHelper() { return Imp::cachedRetrieve(Imp::m_pixelIDHelper,"PixelID",VP1JobConfigInfo::hasPixelGeometry()); }
const SCT_ID * VP1DetInfo::sctIDHelper() { return Imp::cachedRetrieve(Imp::m_sctIDHelper,"SCT_ID",VP1JobConfigInfo::hasSCTGeometry()); }
const TRT_ID * VP1DetInfo::trtIDHelper() { return Imp::cachedRetrieve(Imp::m_trtIDHelper,"TRT_ID",VP1JobConfigInfo::hasTRTGeometry()); }

//____________________________________________________________________
bool VP1DetInfo::isUnsafe( const Identifier& id ) {

  const AtlasDetectorID * idhelper = atlasIDHelper();
  if ( !idhelper || !id.is_valid() )
    return true;

  if (idhelper->is_indet(id)) {
    if (!VP1JobConfigInfo::hasTRTGeometry() && idhelper->is_trt(id))
      return true;
    if (!VP1JobConfigInfo::hasPixelGeometry() && idhelper->is_pixel(id))
      return true;
    if (!VP1JobConfigInfo::hasSCTGeometry() && idhelper->is_sct(id))
      return true;
  } else if (idhelper->is_muon(id)) {
    if (!VP1JobConfigInfo::hasMuonGeometry())
      return true;
  } else if (idhelper->is_calo(id)) {
    if (!VP1JobConfigInfo::hasLArGeometry() && idhelper->is_lar(id) )
      return true;
    if (!VP1JobConfigInfo::hasTileGeometry() && idhelper->is_tile(id) )
      return true;
  }
  return false;
}
