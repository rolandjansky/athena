/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/RpcRoadDefiner.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------                 // --------------------------------------------------------------------------------                  
static const InterfaceID IID_RpcRoadDefiner("IID_RpcRoadDefiner", 1, 0);

const InterfaceID& TrigL2MuonSA::RpcRoadDefiner::interfaceID() { return IID_RpcRoadDefiner; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcRoadDefiner::RpcRoadDefiner(const std::string& type,
                                             const std::string& name,
                                             const IInterface*  parent):
  AthAlgTool(type, name, parent),
  m_roadData(0),
  m_rWidth_RPC_Failed(0), m_use_rpc(true),
  m_regionSelector(0), m_mdtIdHelper(0)
{
  declareInterface<TrigL2MuonSA::RpcRoadDefiner>(this);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcRoadDefiner::~RpcRoadDefiner(void)
{
}


// --------------------------------------------------------------------------------                  
// --------------------------------------------------------------------------------                  

StatusCode TrigL2MuonSA::RpcRoadDefiner::initialize()
{
  ATH_MSG_DEBUG("Initializing RpcRoadDefiner - package version " << PACKAGE_VERSION) ;

  StatusCode sc;
  sc = AthAlgTool::initialize();
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
    return sc;
  }

  //                                                                                                 
  return StatusCode::SUCCESS;
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
						    ToolHandle<RpcPatFinder>*    rpcPatFinder,
						    TrigL2MuonSA::RpcFitResult&  rpcFitResult,
                                                    double                       roiEtaMinLow,
                                                    double                       roiEtaMaxLow,
                                                    double                       roiEtaMinHigh,
                                                    double                       roiEtaMaxHigh)
{

  if (!m_roadData) m_roadData = new BarrelRoadData();
  
  const double ZERO_LIMIT = 1e-5;
  
  const int N_LAYER = 5; // 0: inner, 1: middle, 2: outer 4: BME 5: BMG
  const int N_SECTOR = 2; // 0: normal, 1:overlap

  if (m_use_rpc) {
    double aw[3]={0.,0.,0.};
    double bw[3]={0.,0.,0.};
    unsigned int rpc_pattern;
    if ( (*rpcPatFinder)->findPatternEta(aw, bw, rpc_pattern) ) {
      rpcFitResult.isSuccess = true;
      rpcFitResult.offset_inner = bw[0];
      rpcFitResult.offset_middle = bw[1];
      rpcFitResult.offset_outer = bw[2];
      rpcFitResult.slope_inner = 1.0/aw[0];
      rpcFitResult.slope_middle = 1.0/aw[1];
      rpcFitResult.slope_outer = 1.0/aw[2];

      for(int i=0;i<3;i++){
	if(fabs(1.0/aw[i]) <= ZERO_LIMIT) rpcFitResult.isSuccess = false;
      }
    } else {
      rpcFitResult.isSuccess = false;
    }
    
    double phi_middle;
    double phi_outer;
    unsigned int phi_pattern;
    if ( (*rpcPatFinder)->findPatternPhi(phi_middle, phi_outer, phi_pattern)) {
      rpcFitResult.phi = phi_middle;
      rpcFitResult.phi_middle = phi_middle;
      rpcFitResult.phi_outer = phi_outer;
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
  
  for (int i_station=0; i_station<6; i_station++) {
    for (int i_layer=0; i_layer<8; i_layer++) {
      if(!rpcFitResult.isSuccess) {
	if (i_station==0)      muonRoad.rWidth[i_station][i_layer] = 500;//for inner
	else if (i_station==1) muonRoad.rWidth[i_station][i_layer] = 650;//for middle
	else if (i_station==2) muonRoad.rWidth[i_station][i_layer] = 800;//for outer
	else if (i_station==3) muonRoad.rWidth[i_station][i_layer] = 500;//EndcapInner
	else if (i_station==4) muonRoad.rWidth[9][i_layer] = 650;//BME
	else if (i_station==5) muonRoad.rWidth[10][i_layer] = 650;//BMG
	else muonRoad.rWidth[i_station][i_layer] = m_rWidth_RPC_Failed;
      }
      else {
	if (i_station==0)      muonRoad.rWidth[i_station][i_layer] = 400;//for inner
	else if (i_station==1) muonRoad.rWidth[i_station][i_layer] = 200;//for middle
	else if (i_station==2) muonRoad.rWidth[i_station][i_layer] = 400;//for outer
	else if (i_station==3) muonRoad.rWidth[i_station][i_layer] = 400;//EndcapInner
	else if (i_station==4) muonRoad.rWidth[9][i_layer] = m_rWidth_RPC_Failed;//BME
	else if (i_station==5) muonRoad.rWidth[10][i_layer] = m_rWidth_RPC_Failed;//BMG
	else muonRoad.rWidth[i_station][i_layer] = m_rWidth_RPC_Failed;
      }
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

  if (iroi) m_regionSelector->DetHashIDList(MDT, *iroi, mdtHashList);
  else m_regionSelector->DetHashIDList(MDT, mdtHashList);

  if(roi) delete roi;
  
  for(int i_hash=0; i_hash < (int)mdtHashList.size(); i_hash++){
    
    Identifier id;
    int convert = m_mdtIdHelper->get_id(mdtHashList[i_hash], id, &context);
    //	int convert = m_mdtIdHelper->get_id(mdtHashList[i_hash], id);

    if(convert!=0) ATH_MSG_ERROR("problem converting hash list to id");
    
    muonRoad.stationList.push_back(id);
    int stationPhi = m_mdtIdHelper->stationPhi(id);
    std::string name = m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(id));
    
    if ( name[1]=='M' && name[2]=='E' ) continue;//exclude BME
    if ( name[1]=='M' && name[2]=='G' ) continue;//exclude BMG
    
    int LargeSmall = 0;
    if(name[2]=='S' || name[2]=='F' || name[2]=='G' ) LargeSmall = 1;
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
    for (int i_sector=0; i_sector<N_SECTOR; i_sector++) {
      muonRoad.aw[0][i_sector] = rpcFitResult.slope_inner;
      muonRoad.bw[0][i_sector] = rpcFitResult.offset_inner;
      muonRoad.aw[1][i_sector] =  rpcFitResult.slope_middle;
      muonRoad.bw[1][i_sector] = rpcFitResult.offset_middle;
      muonRoad.aw[2][i_sector] =  rpcFitResult.slope_outer;
      muonRoad.bw[2][i_sector] = rpcFitResult.offset_outer;
      muonRoad.aw[3][i_sector] = rpcFitResult.slope_inner; // Endcap Inner
      muonRoad.bw[3][i_sector] = rpcFitResult.offset_inner;
      muonRoad.aw[9][i_sector] = rpcFitResult.slope_middle;//BME
      muonRoad.bw[9][i_sector] = rpcFitResult.offset_middle;
      muonRoad.aw[10][i_sector] = rpcFitResult.slope_middle;//BMG
      muonRoad.bw[10][i_sector] = rpcFitResult.offset_middle;
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
  if (i_station==3) muonRoad.aw[i_station][i_sector] = awLow; //EI
  if (i_station==4) muonRoad.aw[9][i_sector] = awLow; //BME
  if (i_station==5) muonRoad.aw[10][i_sector] = awLow; //BMG
      }
    }
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------                  
// --------------------------------------------------------------------------------                  

StatusCode TrigL2MuonSA::RpcRoadDefiner::finalize()
{
  ATH_MSG_DEBUG("Finalizing RpcRoadDefiner - package version " << PACKAGE_VERSION);

  if (m_roadData) delete m_roadData;

  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

// --------------------------------------------------------------------------------                  
// --------------------------------------------------------------------------------                  
