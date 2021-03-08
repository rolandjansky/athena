/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "ClusterRoadDefiner.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::ClusterRoadDefiner::ClusterRoadDefiner(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface*  parent):
  AthAlgTool(type, name, parent)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::ClusterRoadDefiner::initialize()
{
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_regionSelector.retrieve());
  ATH_MSG_DEBUG("Retrieved the RegionSelector tool ");

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::ClusterRoadDefiner::defineRoad(const LVL1::RecMuonRoI*                   p_roi,
						        std::vector<TrigL2MuonSA::MuonRoad>&      clusterRoad,
                                                        TrigL2MuonSA::RpcLayerClusters&           rpcLayerClusters,
                                                        ToolHandle<ClusterPatFinder>*             clusterPatFinder,
                                                        std::vector<TrigL2MuonSA::RpcFitResult>&  clusterFitResults,
                                                        double                                    roiEtaMinLow,
                                                        double                                    roiEtaMaxLow,
                                                        double                                    roiEtaMinHigh,
                                                        double                                    roiEtaMaxHigh) const
{
  const double ZERO_LIMIT = 1e-5;
  const int N_STATION = 3; //0:inner, 1:middle, 2:outer

  const int N_LAYER = 5; // 0: inner, 1: middle, 2: outer 4: BME 5: BMG
  const int N_SECTOR = 2; // 0: normal, 1:overlap

  if (m_use_rpc) {
    std::vector< std::vector< double > > aw, bw;
    std::vector< double > clearRoad;
    clearRoad.clear();
    aw.assign(N_STATION, clearRoad);
    bw.assign(N_STATION, clearRoad);
    ATH_MSG_DEBUG("start searching eta pattern");
    if ( (*clusterPatFinder)->findPatternEta(aw, bw, rpcLayerClusters) ) {
      for(int iClus = 0; iClus < (int)aw[1].size(); iClus++){
        TrigL2MuonSA::RpcFitResult clusFitResult;
        clusFitResult.Clear();
        clusFitResult.isSuccess = true;
        clusFitResult.offset_inner = (bw[0]).at(iClus);
        clusFitResult.offset_middle = (bw[1]).at(iClus);
        clusFitResult.offset_outer = (bw[2]).at(iClus);
        clusFitResult.slope_inner = 1.0/(aw[0]).at(iClus);
        clusFitResult.slope_middle = 1.0/(aw[1]).at(iClus);
        clusFitResult.slope_outer = 1.0/(aw[2]).at(iClus);
        ATH_MSG_DEBUG("==========================================================");
        ATH_MSG_DEBUG("inner clusterRoad slope/offset = " << clusFitResult.slope_inner << "/" << clusFitResult.offset_inner);
        ATH_MSG_DEBUG("middle clusterRoad slope/offset = " << clusFitResult.slope_middle << "/" << clusFitResult.offset_middle);
        ATH_MSG_DEBUG("outer clusterRoad slope/offset = " << clusFitResult.slope_outer << "/" << clusFitResult.offset_outer);
        for(int i=0;i<N_STATION;i++){
          if(std::abs(1.0/aw[i].at(iClus)) <= ZERO_LIMIT) clusFitResult.isSuccess = false;
        }
        clusterFitResults.push_back(clusFitResult);
      }
      ATH_MSG_DEBUG("==========================================================");
    } else {
      TrigL2MuonSA::RpcFitResult clusFitResult;
      clusFitResult.Clear();
      clusFitResult.isSuccess = false;
      clusterFitResults.push_back(clusFitResult);
    }

    ATH_MSG_DEBUG("start searching phi pattern");
    std::vector<double> phi_middle, phi_outer;
    phi_middle.clear(); phi_outer.clear();
    if ( (*clusterPatFinder)->findPatternPhi(phi_middle, phi_outer, rpcLayerClusters) ){
      double phi_middle_tot = 0;
      double phi_outer_tot = 0;
      size_t npatternPhi = phi_middle.size();
      if(npatternPhi > 0){
        for(double& phi : phi_middle){
          phi_middle_tot = phi_middle_tot + phi;
        }
        for(double& phi : phi_outer){
          phi_outer_tot = phi_outer_tot + phi;
        }
        double phi_middle_center = phi_middle_tot/npatternPhi;
        double phi_outer_center = phi_outer_tot/npatternPhi;
        ATH_MSG_DEBUG("center of phi middle/outer = " << phi_middle_center << "/" << phi_outer_center);
        for(int iClus_fit = 0; iClus_fit < (int)clusterFitResults.size(); iClus_fit++){
          clusterFitResults.at(iClus_fit).phi = phi_middle_center;
          clusterFitResults.at(iClus_fit).phi_middle = phi_middle_center;
          clusterFitResults.at(iClus_fit).phi_outer = phi_outer_center;
        }
      } else {
        for(int iClus_fit = 0; iClus_fit < (int)clusterFitResults.size(); iClus_fit++){
          clusterFitResults.at(iClus_fit).phi = p_roi->phi();
          clusterFitResults.at(iClus_fit).phi_middle = p_roi->phi();
          clusterFitResults.at(iClus_fit).phi_outer = p_roi->phi();
        }
      }
    } else {
      for(int iClus_fit = 0; iClus_fit < (int)clusterFitResults.size(); iClus_fit++){
        clusterFitResults.at(iClus_fit).phi = p_roi->phi();
      }
    }
  }
  if(clusterFitResults.empty()){
    TrigL2MuonSA::RpcFitResult clusFitResult;
    clusFitResult.Clear();
    clusFitResult.isSuccess = false;
    clusFitResult.phi = p_roi->phi();
    clusterFitResults.push_back(clusFitResult);
  }

  ATH_MSG_DEBUG("stored cluster eta/phi pattern to clusterRoad");
  for(int iClus_fit = 0; iClus_fit < (int)clusterFitResults.size(); iClus_fit++){
    TrigL2MuonSA::MuonRoad muonRoad;
    muonRoad.Clear();
    // RPC data is not available -> use RoI

    muonRoad.isEndcap   = false;
    muonRoad.phiMiddle  = clusterFitResults.at(iClus_fit).phi;
    muonRoad.phiRoI     = p_roi->phi();
    muonRoad.side       = (p_roi->phi()<0.)? 0 : 1;
    muonRoad.LargeSmall = ((p_roi->sectorID() + 1)/2 )%2;

    int PhysicsSector = ((p_roi->sectorID() + 1)/4 )%8 + 1;

    int special = 0;
    if (muonRoad.LargeSmall == 0 && (PhysicsSector == 6 || PhysicsSector == 8 ))
      special = 1;
    else if (muonRoad.LargeSmall == 1 && (PhysicsSector == 6 || PhysicsSector == 7 ))
      special = 1;
    muonRoad.Special = special;

    for (int i_station=0; i_station<6; i_station++) {
      for (int i_layer=0; i_layer<8; i_layer++) {
        if(!clusterFitResults.at(iClus_fit).isSuccess) {
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
    IdContext context = m_idHelperSvc->mdtIdHelper().module_context();

    double etaMin =  p_roi->eta()-.02;
    double etaMax =  p_roi->eta()+.02;
    double phiMin = muonRoad.phiMiddle-.01;
    double phiMax = muonRoad.phiMiddle+.01;
    if(phiMax > M_PI) phiMax -= M_PI*2.;
    if(phiMin < M_PI*-1) phiMin += M_PI*2.;

    TrigRoiDescriptor* roi = new TrigRoiDescriptor( p_roi->eta(), etaMin, etaMax, p_roi->phi(), phiMin, phiMax );

    const IRoiDescriptor* iroi = (IRoiDescriptor*) roi;

    if (iroi) m_regionSelector->HashIDList(*iroi, mdtHashList);
    else {
      TrigRoiDescriptor fullscan_roi( true );
      m_regionSelector->HashIDList(fullscan_roi, mdtHashList);
    }

    if(roi) delete roi;

    for( const IdentifierHash& hash : mdtHashList){

      Identifier id;
      const int convert = m_idHelperSvc->mdtIdHelper().get_id(hash, id, &context);

      if(convert!=0) ATH_MSG_ERROR("problem converting hash list to id");

      muonRoad.stationList.push_back(id);
      const int stationPhi = m_idHelperSvc->mdtIdHelper().stationPhi(id);
      std::string name = m_idHelperSvc->mdtIdHelper().stationNameString(m_idHelperSvc->mdtIdHelper().stationName(id));

      if ( name[1]=='M' && name[2]=='E' ) continue;//exclude BME
      else if ( name[1]=='M' && name[2]=='G' ) continue;//exclude BMG

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

    if (clusterFitResults.at(iClus_fit).isSuccess) {
      for (int i_sector=0; i_sector<N_SECTOR; i_sector++) {
        muonRoad.aw[0][i_sector] = clusterFitResults.at(iClus_fit).slope_inner;
        muonRoad.bw[0][i_sector] = clusterFitResults.at(iClus_fit).offset_inner;
        muonRoad.aw[1][i_sector] =  clusterFitResults.at(iClus_fit).slope_middle;
        muonRoad.bw[1][i_sector] = clusterFitResults.at(iClus_fit).offset_middle;
        muonRoad.aw[2][i_sector] =  clusterFitResults.at(iClus_fit).slope_outer;
        muonRoad.bw[2][i_sector] = clusterFitResults.at(iClus_fit).offset_outer;
        muonRoad.aw[3][i_sector] = clusterFitResults.at(iClus_fit).slope_inner; // Endcap Inner
        muonRoad.bw[3][i_sector] = clusterFitResults.at(iClus_fit).offset_inner;
        muonRoad.aw[9][i_sector] = clusterFitResults.at(iClus_fit).slope_middle;//BME
        muonRoad.bw[9][i_sector] = clusterFitResults.at(iClus_fit).offset_middle;
        muonRoad.aw[10][i_sector] = clusterFitResults.at(iClus_fit).slope_middle;//BMG
        muonRoad.bw[10][i_sector] = clusterFitResults.at(iClus_fit).offset_middle;
      }
    } else {
      double roiEtaLow = (roiEtaMinLow + roiEtaMaxLow) * 0.5;
      double roiEtaHigh = (roiEtaMinHigh + roiEtaMaxHigh) * 0.5;
      double thetaLow  = std::atan(std::exp(-std::abs(roiEtaLow)))*2.;
      double thetaHigh  = std::atan(std::exp(-std::abs(roiEtaHigh)))*2.;
      double awLow     = (std::abs(roiEtaLow) > ZERO_LIMIT)? std::tan(thetaLow)*(std::abs(roiEtaLow)/roiEtaLow): 0.;
      double awHigh     = (std::abs(roiEtaHigh) > ZERO_LIMIT)? std::tan(thetaHigh)*(std::abs(roiEtaHigh)/roiEtaHigh): 0.;

      for (int i_station=0; i_station<N_LAYER; i_station++) {
        for (int i_sector=0; i_sector<N_SECTOR; i_sector++) {
          muonRoad.aw[i_station][i_sector] = awLow;
          muonRoad.bw[i_station][i_sector] = 0;
          if (i_station==2) muonRoad.aw[i_station][i_sector] = awHigh;
          else if (i_station==3) muonRoad.aw[i_station][i_sector] = awLow; //EI
          else if (i_station==4) muonRoad.aw[9][i_sector] = awLow; //BME
          else if (i_station==5) muonRoad.aw[10][i_sector] = awLow; //BMG
        }
      }
    }
    clusterRoad.push_back(muonRoad);
  }
  ATH_MSG_DEBUG("finished ClusterRoadDefiner algorithm... leave");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::ClusterRoadDefiner::defineRoad(const xAOD::MuonRoI*                      p_roi,
						        std::vector<TrigL2MuonSA::MuonRoad>&      clusterRoad,
                                                        TrigL2MuonSA::RpcLayerClusters&           rpcLayerClusters,
                                                        ToolHandle<ClusterPatFinder>*             clusterPatFinder,
                                                        std::vector<TrigL2MuonSA::RpcFitResult>&  clusterFitResults,
                                                        double                                    roiEtaMinLow,
                                                        double                                    roiEtaMaxLow,
                                                        double                                    roiEtaMinHigh,
                                                        double                                    roiEtaMaxHigh) const
{
  const double ZERO_LIMIT = 1e-5;
  const int N_STATION = 3; //0:inner, 1:middle, 2:outer

  const int N_LAYER = 5; // 0: inner, 1: middle, 2: outer 4: BME 5: BMG
  const int N_SECTOR = 2; // 0: normal, 1:overlap

  if (m_use_rpc) {
    std::vector< std::vector< double > > aw, bw;
    std::vector< double > clearRoad;
    clearRoad.clear();
    aw.assign(N_STATION, clearRoad);
    bw.assign(N_STATION, clearRoad);
    ATH_MSG_DEBUG("start searching eta pattern");
    if ( (*clusterPatFinder)->findPatternEta(aw, bw, rpcLayerClusters) ) {
      for(int iClus = 0; iClus < (int)aw[1].size(); iClus++){
        TrigL2MuonSA::RpcFitResult clusFitResult;
        clusFitResult.Clear();
        clusFitResult.isSuccess = true;
        clusFitResult.offset_inner = (bw[0]).at(iClus);
        clusFitResult.offset_middle = (bw[1]).at(iClus);
        clusFitResult.offset_outer = (bw[2]).at(iClus);
        clusFitResult.slope_inner = 1.0/(aw[0]).at(iClus);
        clusFitResult.slope_middle = 1.0/(aw[1]).at(iClus);
        clusFitResult.slope_outer = 1.0/(aw[2]).at(iClus);
        ATH_MSG_DEBUG("==========================================================");
        ATH_MSG_DEBUG("inner clusterRoad slope/offset = " << clusFitResult.slope_inner << "/" << clusFitResult.offset_inner);
        ATH_MSG_DEBUG("middle clusterRoad slope/offset = " << clusFitResult.slope_middle << "/" << clusFitResult.offset_middle);
        ATH_MSG_DEBUG("outer clusterRoad slope/offset = " << clusFitResult.slope_outer << "/" << clusFitResult.offset_outer);
        for(int i=0;i<N_STATION;i++){
          if(std::abs(1.0/aw[i].at(iClus)) <= ZERO_LIMIT) clusFitResult.isSuccess = false;
        }
        clusterFitResults.push_back(clusFitResult);
      }
      ATH_MSG_DEBUG("==========================================================");
    } else {
      TrigL2MuonSA::RpcFitResult clusFitResult;
      clusFitResult.Clear();
      clusFitResult.isSuccess = false;
      clusterFitResults.push_back(clusFitResult);
    }

    ATH_MSG_DEBUG("start searching phi pattern");
    std::vector<double> phi_middle, phi_outer;
    phi_middle.clear(); phi_outer.clear();
    if ( (*clusterPatFinder)->findPatternPhi(phi_middle, phi_outer, rpcLayerClusters) ){
      double phi_middle_tot = 0;
      double phi_outer_tot = 0;
      size_t npatternPhi = phi_middle.size();
      if(npatternPhi > 0){
        for(double& phi : phi_middle){
          phi_middle_tot = phi_middle_tot + phi;
        }
        for(double& phi : phi_outer){
          phi_outer_tot = phi_outer_tot + phi;
        }
        double phi_middle_center = phi_middle_tot/npatternPhi;
        double phi_outer_center = phi_outer_tot/npatternPhi;
        ATH_MSG_DEBUG("center of phi middle/outer = " << phi_middle_center << "/" << phi_outer_center);
        for(int iClus_fit = 0; iClus_fit < (int)clusterFitResults.size(); iClus_fit++){
          clusterFitResults.at(iClus_fit).phi = phi_middle_center;
          clusterFitResults.at(iClus_fit).phi_middle = phi_middle_center;
          clusterFitResults.at(iClus_fit).phi_outer = phi_outer_center;
        }
      } else {
        for(int iClus_fit = 0; iClus_fit < (int)clusterFitResults.size(); iClus_fit++){
          clusterFitResults.at(iClus_fit).phi = p_roi->phi();
          clusterFitResults.at(iClus_fit).phi_middle = p_roi->phi();
          clusterFitResults.at(iClus_fit).phi_outer = p_roi->phi();
        }
      }
    } else {
      for(int iClus_fit = 0; iClus_fit < (int)clusterFitResults.size(); iClus_fit++){
        clusterFitResults.at(iClus_fit).phi = p_roi->phi();
      }
    }
  }
  if(clusterFitResults.empty()){
    TrigL2MuonSA::RpcFitResult clusFitResult;
    clusFitResult.Clear();
    clusFitResult.isSuccess = false;
    clusFitResult.phi = p_roi->phi();
    clusterFitResults.push_back(clusFitResult);
  }

  ATH_MSG_DEBUG("stored cluster eta/phi pattern to clusterRoad");
  for(int iClus_fit = 0; iClus_fit < (int)clusterFitResults.size(); iClus_fit++){
    TrigL2MuonSA::MuonRoad muonRoad;
    muonRoad.Clear();
    // RPC data is not available -> use RoI

    muonRoad.isEndcap   = false;
    muonRoad.phiMiddle  = clusterFitResults.at(iClus_fit).phi;
    muonRoad.phiRoI     = p_roi->phi();
    muonRoad.side       = (p_roi->phi()<0.)? 0 : 1;
    const int SectorID = ( ( p_roi->getSectorAddress() >> 1 ) & 0x1F ); // temporal until sector ID is available in xAOD::MuonRoI
    muonRoad.LargeSmall = ((SectorID + 1)/2 )%2;

    int PhysicsSector = ((SectorID + 1)/4 )%8 + 1;

    int special = 0;
    if (muonRoad.LargeSmall == 0 && (PhysicsSector == 6 || PhysicsSector == 8 ))
      special = 1;
    else if (muonRoad.LargeSmall == 1 && (PhysicsSector == 6 || PhysicsSector == 7 ))
      special = 1;
    muonRoad.Special = special;

    for (int i_station=0; i_station<6; i_station++) {
      for (int i_layer=0; i_layer<8; i_layer++) {
        if(!clusterFitResults.at(iClus_fit).isSuccess) {
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
    IdContext context = m_idHelperSvc->mdtIdHelper().module_context();

    double etaMin =  p_roi->eta()-.02;
    double etaMax =  p_roi->eta()+.02;
    double phiMin = muonRoad.phiMiddle-.01;
    double phiMax = muonRoad.phiMiddle+.01;
    if(phiMax > M_PI) phiMax -= M_PI*2.;
    if(phiMin < M_PI*-1) phiMin += M_PI*2.;

    TrigRoiDescriptor* roi = new TrigRoiDescriptor( p_roi->eta(), etaMin, etaMax, p_roi->phi(), phiMin, phiMax );

    const IRoiDescriptor* iroi = (IRoiDescriptor*) roi;

    if (iroi) m_regionSelector->HashIDList(*iroi, mdtHashList);
    else {
      TrigRoiDescriptor fullscan_roi( true );
      m_regionSelector->HashIDList(fullscan_roi, mdtHashList);
    }

    if(roi) delete roi;

    for( const IdentifierHash& hash : mdtHashList){

      Identifier id;
      const int convert = m_idHelperSvc->mdtIdHelper().get_id(hash, id, &context);

      if(convert!=0) ATH_MSG_ERROR("problem converting hash list to id");

      muonRoad.stationList.push_back(id);
      const int stationPhi = m_idHelperSvc->mdtIdHelper().stationPhi(id);
      std::string name = m_idHelperSvc->mdtIdHelper().stationNameString(m_idHelperSvc->mdtIdHelper().stationName(id));

      if ( name[1]=='M' && name[2]=='E' ) continue;//exclude BME
      else if ( name[1]=='M' && name[2]=='G' ) continue;//exclude BMG

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

    if (clusterFitResults.at(iClus_fit).isSuccess) {
      for (int i_sector=0; i_sector<N_SECTOR; i_sector++) {
        muonRoad.aw[0][i_sector] = clusterFitResults.at(iClus_fit).slope_inner;
        muonRoad.bw[0][i_sector] = clusterFitResults.at(iClus_fit).offset_inner;
        muonRoad.aw[1][i_sector] =  clusterFitResults.at(iClus_fit).slope_middle;
        muonRoad.bw[1][i_sector] = clusterFitResults.at(iClus_fit).offset_middle;
        muonRoad.aw[2][i_sector] =  clusterFitResults.at(iClus_fit).slope_outer;
        muonRoad.bw[2][i_sector] = clusterFitResults.at(iClus_fit).offset_outer;
        muonRoad.aw[3][i_sector] = clusterFitResults.at(iClus_fit).slope_inner; // Endcap Inner
        muonRoad.bw[3][i_sector] = clusterFitResults.at(iClus_fit).offset_inner;
        muonRoad.aw[9][i_sector] = clusterFitResults.at(iClus_fit).slope_middle;//BME
        muonRoad.bw[9][i_sector] = clusterFitResults.at(iClus_fit).offset_middle;
        muonRoad.aw[10][i_sector] = clusterFitResults.at(iClus_fit).slope_middle;//BMG
        muonRoad.bw[10][i_sector] = clusterFitResults.at(iClus_fit).offset_middle;
      }
    } else {
      double roiEtaLow = (roiEtaMinLow + roiEtaMaxLow) * 0.5;
      double roiEtaHigh = (roiEtaMinHigh + roiEtaMaxHigh) * 0.5;
      double thetaLow  = std::atan(std::exp(-std::abs(roiEtaLow)))*2.;
      double thetaHigh  = std::atan(std::exp(-std::abs(roiEtaHigh)))*2.;
      double awLow     = (std::abs(roiEtaLow) > ZERO_LIMIT)? std::tan(thetaLow)*(std::abs(roiEtaLow)/roiEtaLow): 0.;
      double awHigh     = (std::abs(roiEtaHigh) > ZERO_LIMIT)? std::tan(thetaHigh)*(std::abs(roiEtaHigh)/roiEtaHigh): 0.;

      for (int i_station=0; i_station<N_LAYER; i_station++) {
        for (int i_sector=0; i_sector<N_SECTOR; i_sector++) {
          muonRoad.aw[i_station][i_sector] = awLow;
          muonRoad.bw[i_station][i_sector] = 0;
          if (i_station==2) muonRoad.aw[i_station][i_sector] = awHigh;
          else if (i_station==3) muonRoad.aw[i_station][i_sector] = awLow; //EI
          else if (i_station==4) muonRoad.aw[9][i_sector] = awLow; //BME
          else if (i_station==5) muonRoad.aw[10][i_sector] = awLow; //BMG
        }
      }
    }
    clusterRoad.push_back(muonRoad);
  }
  ATH_MSG_DEBUG("finished ClusterRoadDefiner algorithm... leave");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
