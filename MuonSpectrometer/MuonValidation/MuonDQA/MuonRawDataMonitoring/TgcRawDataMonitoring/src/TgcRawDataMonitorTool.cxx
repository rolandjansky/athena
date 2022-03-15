/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "TgcRawDataMonitorTool.h"
#include <boost/format.hpp>
#include "TVector2.h"
#include "MuonReadoutGeometry/TgcDetectorElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "TrkSurfaces/TrapezoidBounds.h"

TgcRawDataMonitorTool::TgcRawDataMonitorTool( const std::string& type,
					      const std::string& name,
					      const IInterface* parent)
  : base_class(type,name,parent)
{
}

TgcRawDataMonitorTool::~TgcRawDataMonitorTool(){}
StatusCode TgcRawDataMonitorTool::initialize() {
  ATH_MSG_DEBUG("TgcRawDataMonitorTool::initialize(): Start");
  ATH_CHECK(m_DetectorManagerKey.initialize());
  ATH_MSG_DEBUG("TgcRawDataMonitorTool::initialize(): End");
  return StatusCode::SUCCESS;
}
std::set<std::string> TgcRawDataMonitorTool::getPassedChambers(const Amg::Vector3D& extVec) const {
  ATH_MSG_DEBUG("TgcRawDataMonitorTool::getPassedChambers(): Start");  
  double ext_x = extVec[0];
  double ext_y = extVec[1];
  double ext_z = extVec[2];
  std::set<std::string> passed_chambers;
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey,Gaudi::Hive::currentContext()};
  const MuonGM::MuonDetectorManager* muonMgr = DetectorManagerHandle.cptr();
  if(muonMgr==nullptr){
    ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object.");
    return passed_chambers;
  }     
  for(int iStationName = 41 ; iStationName <= 48 ; iStationName++){
    if ( (iStationName==41 || iStationName==42) && // M1
	 ( std::abs(ext_z)<m_M1Mod1Pl1L1_offsetZ-10. || std::abs(ext_z)>m_M1Mod2Pl2L3_offsetZ+10. ) )continue;
    if ( (iStationName==43 || iStationName==44) && // M2
	 ( std::abs(ext_z)<m_M2Mod1Pl1L1_offsetZ-10. || std::abs(ext_z)>m_M2Mod2Pl2L2_offsetZ+10. ) )continue;
    if ((iStationName==45 || iStationName==46) && // M3
	( std::abs(ext_z)<m_M3Mod1Pl1L1_offsetZ-10. || std::abs(ext_z)>m_M3Mod2Pl2L2_offsetZ+10. ) )continue;
    if ( iStationName==48 && // EI
	 ( std::abs(ext_z)<m_EIMod1Pl1L1_offsetZ-10. || std::abs(ext_z)>m_EIMod2Pl1L2_offsetZ+10. ) )continue;
    if ( iStationName==47 && // FI
	 ( std::abs(ext_z)<m_FIMod1Pl1L1_offsetZ-10. || std::abs(ext_z)>m_FIMod2Pl1L2_offsetZ+10. ) )continue;
    bool endcap = (iStationName%2==0);
    int maxEta = 1;
    if(endcap && iStationName==42) maxEta = 4;
    else if(endcap && (iStationName==44 || iStationName==46)) maxEta = 5;
    for(int iStationEta = -maxEta ; iStationEta <= maxEta ; iStationEta++){
      if(iStationEta==0)continue;
      if(ext_z > 0 && iStationEta < 0)continue;
      if(ext_z < 0 && iStationEta > 0)continue;
      for(int iStationPhi = muonMgr->tgcIdHelper()->stationPhiMin(endcap)  ;
	  iStationPhi <= muonMgr->tgcIdHelper()->stationPhiMax(endcap) ; iStationPhi++){
	if(iStationName==48 && iStationPhi>21)continue;
	TGC::TgcChamber tgcChamber; tgcChamber.initChamber(iStationEta, iStationPhi, iStationName);
	auto id = muonMgr->tgcIdHelper()->elementID(iStationName, iStationEta, iStationPhi);
	IdentifierHash hash_id;
	muonMgr->tgcIdHelper()->get_detectorElement_hash(id,hash_id);
	auto detEle = muonMgr->getTgcDetectorElement(hash_id);
	const Trk::TrapezoidBounds* tb = (const Trk::TrapezoidBounds*)&detEle->bounds();
	Amg::Vector3D trkLocVec3D = detEle->transform().inverse() * Amg::Vector3D(ext_x,ext_y,ext_z);
	if(std::abs(trkLocVec3D.x())>1000.)continue;
	if(std::abs(trkLocVec3D.y())>1500.)continue;
	Amg::Vector2D trkvec(trkLocVec3D.x(),trkLocVec3D.y());
	if( tb->minDistance(trkvec) < m_fiduciFrame ) passed_chambers.insert(tgcChamber.cham_name());
      }
    }
  }

  ATH_MSG_DEBUG("TgcRawDataMonitorTool::getPassedChambers(): End");
  ATH_MSG_DEBUG("Number of passed TGC chambers: " << passed_chambers.size());
  return passed_chambers;
}

bool TgcRawDataMonitorTool::getMapIndex(const TGC::TgcHit& tgcHit,
					int& etamap_index, int& phimap_index, int& phimap_global_index ) const {

  ATH_MSG_DEBUG("TgcRawDataMonitorTool::getMapIndex(const TGC::TgcHit& tgcHit,: Start");  
  return getMapIndex(tgcHit,tgcHit.gasGap(),etamap_index,phimap_index,phimap_global_index);
  ATH_MSG_DEBUG("TgcRawDataMonitorTool::getMapIndex(const TGC::TgcHit& tgcHit,: End");  
}
bool TgcRawDataMonitorTool::getMapIndex(const TGC::TgcChamber& tgcCham, int iLay,
					int& etamap_index, int& phimap_index, int& phimap_global_index ) const {
  ATH_MSG_DEBUG("TgcRawDataMonitorTool::getMapIndex(const TGC::TgcChamber& tgcCham,: Start");  
  if (tgcCham.iM() > 1 && iLay > 2){
    ATH_MSG_DEBUG("tgcCham.iM() > 1 && iLay > 2");  
    return false;
  }
  int nLay = (tgcCham.iM()==1)?(3):(2);
  if (tgcCham.iM() == 4) { // EI/FI
    etamap_index = tgcCham.iEta() * nLay + iLay; // 1..4
    if(tgcCham.iEta() == 0){ //FI
      phimap_index = tgcCham.iPhi(); // 1..24
    }else{ // EI 
      phimap_index = tgcCham.iPhi() + int(tgcCham.iSec() / 2) + tgcCham.iSec(); // 1..24
    }
    phimap_global_index = tgcCham.iEta() * 24*nLay + (phimap_index - 1) * nLay + iLay; // 1..96
  }else{ // BigWheel
    etamap_index = tgcCham.iEta() * nLay + iLay; // 1..15 for M1, 1..12 for M2M3
    phimap_index = (tgcCham.iSec() - 1) * 4 + tgcCham.iPhi() + 1; // 1..48
    phimap_global_index = tgcCham.iEta() * 48*nLay + (phimap_index - 1) * nLay + iLay; // 1..720 for M1, 1..576 for M2M3
  }
  ATH_MSG_DEBUG("TgcRawDataMonitorTool::getMapIndex(const TGC::TgcChamber& tgcCham,: End");
  return true;
}
