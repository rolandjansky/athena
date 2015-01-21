/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/RpcRoadDefiner.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcRoadDefiner::RpcRoadDefiner(MsgStream* msg)
  : m_msg(), m_roadData(0),
    m_rWidth_RPC_Failed(0), m_use_rpc(0)
{
  if ( msg ) m_msg = msg; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcRoadDefiner::~RpcRoadDefiner(void)
{
  if (m_roadData) delete m_roadData;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcRoadDefiner::setRoadWidthForFailure(double rWidth_RPC_Failed)
{
  m_rWidth_RPC_Failed = rWidth_RPC_Failed;
  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcRoadDefiner::setRpcGeometry(bool use_rpc)
{
   m_use_rpc = use_rpc;
   return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcRoadDefiner::setMdtGeometry(IRegSelSvc* regionSelector, const MdtIdHelper* mdtIdHelper)
{
  m_regionSelector = regionSelector;
  m_mdtIdHelper = mdtIdHelper;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcRoadDefiner::defineRoad(const LVL1::RecMuonRoI*      p_roi,
						    TrigL2MuonSA::MuonRoad&      muonRoad,
						    TrigL2MuonSA::RpcHits&       /*rpcHits*/,
						    TrigL2MuonSA::RpcPatFinder*  rpcPatFinder,
						    TrigL2MuonSA::RpcFitResult&  rpcFitResult,
                                                    double                       roiEtaMinLow,
                                                    double                       roiEtaMaxLow,
                                                    double                       roiEtaMinHigh,
                                                    double                       roiEtaMaxHigh)
{

  if (!m_roadData) m_roadData = new BarrelRoadData();
  
  const double ZERO_LIMIT = 1e-5;
  
  const int N_LAYER = 3; // 0: inner, 1: middle, 2: outer
  const int N_SECTOR = 2; // 0: normal, 1:overlap

  if (m_use_rpc) {

    double ZoverR_middle;
    double ZoverR_outer;
    unsigned int ZoverR_pattern;
    
    if ( rpcPatFinder->findPatternEta(ZoverR_middle, ZoverR_outer, ZoverR_pattern) ) {
      rpcFitResult.isSuccess = true;
      rpcFitResult.ZoverR_middle = ZoverR_middle;
      rpcFitResult.ZoverR_outer = ZoverR_outer;
    } else {
      rpcFitResult.isSuccess = false;
    }
    
    double phi_middle;
    double phi_outer;
    unsigned int phi_pattern;
    if ( rpcPatFinder->findPatternPhi(phi_middle, phi_outer, phi_pattern)) {
      rpcFitResult.phi = phi_middle;
    } else {
      rpcFitResult.phi = p_roi->phi();
    }
  } 

  // RPC data is not available -> use RoI 
  
  muonRoad.isEndcap   = false;
  muonRoad.phiMiddle  = rpcFitResult.phi;
  muonRoad.phiRoI     = p_roi->phi();
  muonRoad.side       = (p_roi->phi()<0.)? 0 : 1;
  muonRoad.LargeSmall = ((p_roi->sectorID() + 1)/2 )%2;
  
  int PhysicsSector = ((p_roi->sectorID() + 1)/4 )%8 + 1;
  
  int special = 0;
  if (muonRoad.LargeSmall == 0 && (PhysicsSector == 6 || PhysicsSector == 8 ))
    special = 1;
  if (muonRoad.LargeSmall == 1 && (PhysicsSector == 6 || PhysicsSector == 7 ))
    special = 1;
  muonRoad.Special = special;
  
  for (int i_station=0; i_station<3; i_station++) {
    for (int i_layer=0; i_layer<8; i_layer++) {
      muonRoad.rWidth[i_station][i_layer] = m_rWidth_RPC_Failed;
      if (i_station==2) muonRoad.rWidth[i_station][i_layer] = 400;//for outer
    }
  }
  
  int sector_trigger = 99;
  int sector_overlap = 99;
  std::vector<Identifier> stationList;
  std::vector<IdentifierHash> mdtHashList;
  
  // get sector_trigger and sector_overlap by using the region selector
  IdContext context = m_mdtIdHelper->module_context();
  
  double etaMin =  p_roi->eta()-.02;
  double etaMax =  p_roi->eta()+.02;
  double phiMin = muonRoad.phiMiddle-.01;
  double phiMax = muonRoad.phiMiddle+.01;
  if(phiMax > CLHEP::pi) phiMax -= CLHEP::pi*2.;
  if(phiMin < CLHEP::pi*-1) phiMin += CLHEP::pi*2.;
  TrigRoiDescriptor* roi = new TrigRoiDescriptor( p_roi->eta(), etaMin, etaMax, p_roi->phi(), phiMin, phiMax ); 
  const IRoiDescriptor* iroi = (IRoiDescriptor*) roi;
  m_regionSelector->DetHashIDList(MDT, *iroi, mdtHashList);
  if(roi) delete roi;
  
  for(int i_hash=0; i_hash < (int)mdtHashList.size(); i_hash++){
    
    Identifier id;
    int convert = m_mdtIdHelper->get_id(mdtHashList[i_hash], id, &context);
    //	int convert = m_mdtIdHelper->get_id(mdtHashList[i_hash], id);
    if(convert!=0)
      msg() << MSG::ERROR << "problem converting hash list to id" << endreq;
    
    muonRoad.stationList.push_back(id);
    int stationPhi = m_mdtIdHelper->stationPhi(id);
    std::string name = m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(id));
    
    int LargeSmall = 0;
    if(name[2]=='S') LargeSmall = 1;
    int sector = (stationPhi-1)*2 + LargeSmall;
    if(sector_trigger == 99)
      sector_trigger = sector;
    else if(sector_trigger != sector)
      sector_overlap = sector;
  }
  
  int MDT_tr = (PhysicsSector - 1)*2 + muonRoad.LargeSmall;
  if (MDT_tr == sector_overlap) {
    sector_overlap = sector_trigger;
    sector_trigger = MDT_tr;
  }
  
  muonRoad.MDT_sector_trigger = sector_trigger;
  muonRoad.MDT_sector_overlap = sector_overlap;
  
  if (rpcFitResult.isSuccess) {

    double awMiddle = (fabs(rpcFitResult.ZoverR_middle)>ZERO_LIMIT)? 1./rpcFitResult.ZoverR_middle: 0.;
    double awOuter  = (fabs(rpcFitResult.ZoverR_outer)>ZERO_LIMIT)? 1./rpcFitResult.ZoverR_outer: 0.;
    
    for (int i_sector=0; i_sector<N_SECTOR; i_sector++) {
      muonRoad.aw[0][i_sector] = awMiddle;
      muonRoad.bw[0][i_sector] = 0;
    }
    for (int i_sector=0; i_sector<N_SECTOR; i_sector++) {
      muonRoad.aw[1][i_sector] = awMiddle;
      muonRoad.bw[1][i_sector] = 0;
    }
    for (int i_sector=0; i_sector<N_SECTOR; i_sector++) {
      muonRoad.aw[2][i_sector] = awOuter;
      muonRoad.bw[2][i_sector] = 0;
    }
    
  } else {
    double roiEtaLow = (roiEtaMinLow + roiEtaMaxLow) * 0.5;
    double roiEtaHigh = (roiEtaMinHigh + roiEtaMaxHigh) * 0.5;
    double thetaLow  = atan(exp(-fabs(roiEtaLow)))*2.;
    double thetaHigh  = atan(exp(-fabs(roiEtaHigh)))*2.;
    double awLow     = (fabs(roiEtaLow) > ZERO_LIMIT)? tan(thetaLow)*(fabs(roiEtaLow)/roiEtaLow): 0.;
    double awHigh     = (fabs(roiEtaHigh) > ZERO_LIMIT)? tan(thetaHigh)*(fabs(roiEtaHigh)/roiEtaHigh): 0.;
    
    for (int i_station=0; i_station<N_LAYER; i_station++) {
      for (int i_sector=0; i_sector<N_SECTOR; i_sector++) {
	muonRoad.aw[i_station][i_sector] = awLow;
	muonRoad.bw[i_station][i_sector] = 0;
	if (i_station==2) muonRoad.aw[i_station][i_sector] = awHigh;
      }
    }
  }

  return StatusCode::SUCCESS;
}



