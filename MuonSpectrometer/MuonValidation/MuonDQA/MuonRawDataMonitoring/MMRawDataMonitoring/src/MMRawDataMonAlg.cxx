/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : MMRawDataMonAlg
// Authors:   M. Biglietti, E. Rossi (Roma Tre)
// 
//
// DESCRIPTION:
// Subject: MM-->Offline Muon Data Quality
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonSegment/MuonSegment.h"

#include "MMRawDataMonitoring/MMRawDataMonAlg.h"
#include "TrkEventPrimitives/FitQuality.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "MuonPrepRawData/MMPrepData.h"

//root includes
#include <TMath.h>
#include <math.h>
#include <sstream>
#include <TH2F.h>
#include <cmath>

namespace {

  int PCB;
  static constexpr double const toDeg = 180/M_PI;

  //1e=1.6X10-4 fC                                                                                          
  static constexpr double conversion_charge=1.6E-04;

  static const std::array<std::string,2> MM_Side = {"CSide", "ASide"};
  static const std::array<std::string,2> MM_Sector = {"MMS", "MML"};
  static const std::array<std::string,2> EtaSector = {"1","2"};


  struct MMOverviewHistogramStruct {

    std::vector<int> statEta_strip;
    std::vector<int> charge_all;
    std::vector<int> time_all;
    std::vector<int> strip_number;
    std::vector<int> numberofstrips_percluster;
    std::vector<float> mu_TPC_angle;
    std::vector<float> mu_TPC_chi2;
    std::vector<float> R_mon;
    std::vector<float> z_mon;
    std::vector<float> x_mon;
    std::vector<float> y_mon;
    std::vector<int> stationPhi_ASide_eta1_ontrack;
    std::vector<int> stationPhi_ASide_eta2_ontrack;
    std::vector<int> stationPhi_CSide_eta1_ontrack;
    std::vector<int> stationPhi_CSide_eta2_ontrack;
    std::vector<int> sector_ASide_eta2_ontrack;
    std::vector<int> sector_CSide_eta1_ontrack;
    std::vector<int> sector_ASide_eta1_ontrack;
    std::vector<int> sector_CSide_eta2_ontrack;
    std::vector<int> sector_lb_ASide_eta1_ontrack;
    std::vector<int> sector_lb_ASide_eta2_ontrack;
    std::vector<int> sector_lb_CSide_eta1_ontrack;
    std::vector<int> sector_lb_CSide_eta2_ontrack;
    std::vector<int> stationPhi_ASide_eta1;
    std::vector<int> stationPhi_ASide_eta2;
    std::vector<int> stationPhi_CSide_eta1;
    std::vector<int> stationPhi_CSide_eta2;
    std::vector<int> sector_CSide_eta2;
    std::vector<int> sector_ASide_eta2;
    std::vector<int> sector_CSide_eta1;
    std::vector<int> sector_ASide_eta1;
    std::vector<int> sector_lb_ASide_eta1;
    std::vector<int> sector_lb_ASide_eta2;
    std::vector<int> sector_lb_CSide_eta1;
    std::vector<int> sector_lb_CSide_eta2;

  };

  struct MMSummaryHistogramStruct {
    
    std::vector<int> strip_number;
    std::vector<int> sector_strip;
    std::vector<int> charge;
    std::vector<float> mu_TPC_angle;
    std::vector<float> x_ontrack;
    std::vector<float> y_ontrack;
  };

}

/////////////////////////////////////////////////////////////////////////////
// *********************************************************************
// Public Methods
// ********************************************************************* 

MMRawDataMonAlg::MMRawDataMonAlg( const std::string& name, ISvcLocator* pSvcLocator )
  :AthMonitorAlgorithm(name,pSvcLocator),
   m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool"),
   m_MMContainerKey("MM_Measurements")
{
  //Declare the property 
  declareProperty("MMPrepDataContainerName",m_MMContainerKey);
}

/*---------------------------------------------------------*/
StatusCode MMRawDataMonAlg::initialize()
/*---------------------------------------------------------*/
{   
  // init message stream
  ATH_MSG_DEBUG("initialize MMRawDataMonAlg" );
  ATH_MSG_DEBUG("******************" );
  ATH_MSG_DEBUG("doMMESD: " << m_doMMESD );
  ATH_MSG_DEBUG("******************" );
  
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_DetectorManagerKey.initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());

  ATH_MSG_INFO(" Found the MuonIdHelperSvc " );
  ATH_CHECK(m_muonKey.initialize());
  ATH_CHECK(m_MMContainerKey.initialize() );
  
  ATH_MSG_DEBUG(" end of initialize " );
  ATH_MSG_INFO("MMRawDataMonAlg initialization DONE " );

  return StatusCode::SUCCESS;
} 

StatusCode MMRawDataMonAlg::fillHistograms(const EventContext& ctx) const
{  
  int lumiblock = -1;

  lumiblock = GetEventInfo(ctx)->lumiBlock();

  ATH_MSG_INFO("MMRawDataMonAlg::MM RawData Monitoring Histograms being filled" );

  SG::ReadHandle<Muon::MMPrepDataContainer> mm_container(m_MMContainerKey,ctx);

  ATH_MSG_INFO("****** mmContainer->size() : " << mm_container->size());

  if (m_doMMESD) {
    MMOverviewHistogramStruct overviewPlots;
    MMSummaryHistogramStruct summaryPlots[2][2][8][2][2][4];

  //loop in MMPrepDataContainer                                                                             
         
  for(const Muon::MMPrepDataCollection* coll : *mm_container) {

    for (const Muon::MMPrepData* prd : *coll){

      ATH_CHECK(fillMMOverviewVects(prd, overviewPlots));
      ATH_CHECK(fillMMSummaryVects(prd, summaryPlots));
      ATH_CHECK(fillMMHistograms(prd));
    }
  }

  if (m_do_mm_overview) fillMMOverviewHistograms(overviewPlots,lumiblock);

  ATH_CHECK( fillMMSummaryHistograms(summaryPlots) );

  const xAOD::TrackParticleContainer* meTPContainer = nullptr;
  ATH_CHECK(evtStore()->retrieve(meTPContainer,"ExtrapolatedMuonTrackParticles" ));
  clusterFromTrack(meTPContainer,lumiblock);
  
  }

 return StatusCode::SUCCESS;

}

StatusCode MMRawDataMonAlg::fillMMOverviewVects( const Muon::MMPrepData* prd, MMOverviewHistogramStruct& vects ) const {

  Identifier Id = prd->identify();
  const std::vector<Identifier>& stripIds = prd->rdoList();
  unsigned int nStrips = stripIds.size();  // number of strips in this cluster (cluster size)               
  const std::vector<uint16_t>& stripNumbers=prd->stripNumbers();
 
  std::string stName   = m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(Id));
  int gas_gap          = m_idHelperSvc->mmIdHelper().gasGap(Id);
  int stationNumber    = m_idHelperSvc->mmIdHelper().stationName(Id);
  int stationEta       = m_idHelperSvc->mmIdHelper().stationEta(Id);
  int stationPhi       = m_idHelperSvc->mmIdHelper().stationPhi(Id);
  int multiplet        = m_idHelperSvc->mmIdHelper().multilayer(Id);
  int channel          = m_idHelperSvc->mmIdHelper().channel(Id);
   
  // Returns the charge (number of electrons) converted in fC
  int charge=prd->charge()*conversion_charge;
  // Returns the time (in ns)
  int drift_time=prd->time();
  // Returns the microTPC angle (radians converted in degrees)
  float mu_TPC_angle=prd->angle()*toDeg;
  // Returns the microTPC chisq Prob.
  float mu_TPC_chi2=prd->chisqProb();

  Amg::Vector3D pos    = prd->globalPosition();
  TVector3 v (pos.x(),pos.y(),pos.z());
  
  float R=std::hypot(pos.x(),pos.y());

  PCB=get_PCB_from_channel(channel);

  //MM gaps are back to back, so the direction of the drift (time) is different for the even and odd gaps -> flip for the even gaps

  if (gas_gap % 2 == 0) { mu_TPC_angle = -mu_TPC_angle; } 
  
  vects.charge_all.push_back(charge);
  vects.time_all.push_back(drift_time);
  vects.numberofstrips_percluster.push_back(nStrips);
  vects.mu_TPC_angle.push_back(mu_TPC_angle);
  vects.mu_TPC_chi2.push_back(mu_TPC_chi2);
  vects.x_mon.push_back(pos.x());
  vects.y_mon.push_back(pos.y());
  vects.z_mon.push_back(pos.z());
  vects.R_mon.push_back(R);

    
  //MMS and MML phi sectors
  int phisec=0;
  if (stationNumber%2 == 0) phisec=1;
    
  //16 phi sectors, 8 stationPhi times 2 stName, MMS and MML
  int sectorPhi=get_sectorPhi_from_stationPhi_stName(stationPhi,stName);  
  
  //Occupancy plots with PCB granularity further divided for each eta sector: -2, -1, 1, 2
  
  //Filling Vectors for stationEta=-2
  if (stationEta==-2){
    vects.sector_CSide_eta2.push_back(get_bin_for_occ_CSide_pcb_eta2_hist(stationEta,multiplet,gas_gap,PCB));
    vects.stationPhi_CSide_eta2.push_back(sectorPhi);
    vects.sector_lb_CSide_eta2.push_back(get_bin_for_occ_lb_CSide_pcb_eta2_hist(stationEta,multiplet,gas_gap,PCB,phisec));
  }
  //Filling Vectors for stationEta=-1
  else if (stationEta==-1){
    vects.sector_CSide_eta1.push_back(get_bin_for_occ_CSide_pcb_eta1_hist(stationEta,multiplet,gas_gap,PCB));
    vects.stationPhi_CSide_eta1.push_back(sectorPhi);
    vects.sector_lb_CSide_eta1.push_back(get_bin_for_occ_lb_CSide_pcb_eta1_hist(stationEta,multiplet,gas_gap,PCB,phisec));
  }
  //Filling Vectors for stationEta=1
  else if (stationEta==1){
    vects.sector_ASide_eta1.push_back(get_bin_for_occ_ASide_pcb_eta1_hist(stationEta,multiplet,gas_gap,PCB));
    vects.stationPhi_ASide_eta1.push_back(sectorPhi);
    vects.sector_lb_ASide_eta1.push_back(get_bin_for_occ_lb_ASide_pcb_eta1_hist(stationEta,multiplet,gas_gap,PCB,phisec));
  }
  //Filling Vectors for stationEta=2
  else {
    vects.sector_ASide_eta2.push_back(get_bin_for_occ_ASide_pcb_eta2_hist(stationEta,multiplet,gas_gap,PCB));
    vects.stationPhi_ASide_eta2.push_back(sectorPhi);
    vects.sector_lb_ASide_eta2.push_back(get_bin_for_occ_lb_ASide_pcb_eta2_hist(stationEta,multiplet,gas_gap,PCB,phisec));
  }

  //loop on each strip                                                                                       
  
  int sIdx = 0; // index-counter for the vector of Id's                                
  for (const Identifier& id : stripIds){    
    
    std::string stName_strip   = m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(id));
    int stationEta_strip       = m_idHelperSvc->mmIdHelper().stationEta(id);

    vects.statEta_strip.push_back(stationEta_strip);
    vects.strip_number.push_back(stripNumbers[sIdx]);
  }
  return StatusCode::SUCCESS;
}


void MMRawDataMonAlg::fillMMOverviewHistograms( const MMOverviewHistogramStruct& vects, int lb ) const {

  auto charge_all = Monitored::Collection("charge_all", vects.charge_all);
  auto numberofstrips_percluster = Monitored::Collection("numberofstrips_percluster", vects.numberofstrips_percluster);
  fill("mmMonitor",charge_all,numberofstrips_percluster);

  auto time_all = Monitored::Collection("time_all", vects.time_all);
  auto mu_TPC_angle=Monitored::Collection("mu_TPC_angle", vects.mu_TPC_angle);
  auto mu_TPC_chi2=Monitored::Collection("mu_TPC_chi2", vects.mu_TPC_chi2);
  auto strip_number = Monitored::Collection("strip_number", vects.strip_number);
  auto statEta_strip = Monitored::Collection("statEta_strip", vects.statEta_strip);

  fill("mmMonitor",time_all,mu_TPC_angle,mu_TPC_chi2,strip_number,statEta_strip);

  auto x_mon = Monitored::Collection("x_mon", vects.x_mon);
  auto y_mon = Monitored::Collection("y_mon", vects.y_mon);
  auto z_mon = Monitored::Collection("z_mon", vects.z_mon);
  auto R_mon = Monitored::Collection("R_mon", vects.R_mon);

  fill("mmMonitor",x_mon,y_mon,z_mon,R_mon);
    
  auto lb_mon = Monitored::Scalar<int>("lb_mon", lb);
    
  auto sector_lb_CSide_eta2 = Monitored::Collection("sector_lb_CSide_eta2",vects.sector_lb_CSide_eta2);
  auto sector_lb_CSide_eta1 = Monitored::Collection("sector_lb_CSide_eta1",vects.sector_lb_CSide_eta1);
  auto sector_lb_ASide_eta2 = Monitored::Collection("sector_lb_ASide_eta2",vects.sector_lb_ASide_eta2);
  auto sector_lb_ASide_eta1 = Monitored::Collection("sector_lb_ASide_eta1",vects.sector_lb_ASide_eta1);
    
  fill("mmMonitor",lb_mon,sector_lb_CSide_eta2,sector_lb_CSide_eta1,sector_lb_ASide_eta1,sector_lb_ASide_eta2);
    
  auto sector_CSide_eta2 = Monitored::Collection("sector_CSide_eta2",vects.sector_CSide_eta2);
  auto sector_CSide_eta1 = Monitored::Collection("sector_CSide_eta1",vects.sector_CSide_eta1);
  auto sector_ASide_eta1 = Monitored::Collection("sector_ASide_eta1",vects.sector_ASide_eta1);
  auto sector_ASide_eta2 = Monitored::Collection("sector_ASide_eta2",vects.sector_ASide_eta2);
  auto stationPhi_CSide_eta1 = Monitored::Collection("stationPhi_CSide_eta1",vects.stationPhi_CSide_eta1);
  auto stationPhi_CSide_eta2 = Monitored::Collection("stationPhi_CSide_eta2",vects.stationPhi_CSide_eta2);
  auto stationPhi_ASide_eta1 = Monitored::Collection("stationPhi_ASide_eta1",vects.stationPhi_ASide_eta1);
  auto stationPhi_ASide_eta2 = Monitored::Collection("stationPhi_ASide_eta2",vects.stationPhi_ASide_eta2);
  
  fill("mmMonitor",sector_CSide_eta1,sector_CSide_eta2,sector_ASide_eta1,sector_ASide_eta2,stationPhi_CSide_eta1,stationPhi_CSide_eta2,stationPhi_ASide_eta1,stationPhi_ASide_eta2);
    
    

}

StatusCode MMRawDataMonAlg::fillMMSummaryVects( const Muon::MMPrepData* prd, MMSummaryHistogramStruct (&vects)[2][2][8][2][2][4]) const{

  Identifier Id = prd->identify();
  const std::vector<Identifier>& stripIds = prd->rdoList();

  std::string stName   = m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(Id));
  int thisStationNumber    = m_idHelperSvc->mmIdHelper().stationName(Id);
  int thisStationEta       = m_idHelperSvc->mmIdHelper().stationEta(Id);
  int thisStationPhi       = m_idHelperSvc->mmIdHelper().stationPhi(Id);
  int thisMultiplet        = m_idHelperSvc->mmIdHelper().multilayer(Id);
  int thisGasgap          = m_idHelperSvc->mmIdHelper().gasGap(Id);
  int thisCharge=prd->charge()*conversion_charge;

  float thisMu_TPC_angle=prd->angle()*toDeg;
    
  if ( thisGasgap % 2 == 0 ) { thisMu_TPC_angle = - thisMu_TPC_angle; }
    
  //MMS and MML phi sectors
  int phisec=0;
  if (thisStationNumber%2 == 0) phisec=1;
  
  //CSide and ASide
  int iside=0;
  if(thisStationEta>0) iside=1;
  
  //2 eta sectors depending on Eta=+-1 (0) and +-2 (1)
  int sectorEta=get_sectorEta_from_stationEta(thisStationEta);

  auto& Vectors = vects[iside][phisec][thisStationPhi-1][sectorEta][thisMultiplet-1][thisGasgap-1];
  
  Vectors.mu_TPC_angle.push_back(thisMu_TPC_angle);
  Vectors.charge.push_back(thisCharge);
  
  //loop on strips
  int sIdx = 0;
  const std::vector<uint16_t>& stripNumbers=prd->stripNumbers();
  
  for ( const Identifier& id : stripIds){
    
    int stationEta       = m_idHelperSvc->mmIdHelper().stationEta(id);
    int gas_gap          = m_idHelperSvc->mmIdHelper().gasGap(Id);
    int multiplet        = m_idHelperSvc->mmIdHelper().multilayer(Id); 

    //    Filling Vectors for both sides, considering each strip  
    Vectors.strip_number.push_back(stripNumbers[sIdx]);
    Vectors.sector_strip.push_back(get_bin_for_occ_ASide_hist(stationEta,multiplet,gas_gap));
    
  }

  return StatusCode::SUCCESS;
}


StatusCode MMRawDataMonAlg::fillMMSummaryHistograms( const MMSummaryHistogramStruct (&vects)[2][2][8][2][2][4]) const{
  
    
  for (int iside=0;iside<2;iside++){
    std::string MM_sideGroup = "MM_sideGroup"+MM_Side[iside];
    
    for (int isector=0;isector<2;isector++){
      for( int statPhi=0; statPhi<8; statPhi++) {
	for( int statEta=0; statEta<2; statEta++) {
	  for( int multiplet=0; multiplet<2; multiplet++) {
	    for( int gas_gap=0; gas_gap<4; gas_gap++) {
	      
	      auto& Vectors = vects[iside][isector][statPhi][statEta][multiplet][gas_gap];
                          
	      auto sector_strip=Monitored::Collection("sector_strip_"+MM_Side[iside]+"_"+MM_Sector[isector]+"_phi"+std::to_string(statPhi+1),Vectors.sector_strip);
	      auto strip_number = Monitored::Collection("strip_number_"+MM_Side[iside]+"_"+MM_Sector[isector]+"_phi"+std::to_string(statPhi+1), Vectors.strip_number);
	      
	      fill(MM_sideGroup,strip_number,sector_strip);

	      auto charge_perLayer = Monitored::Collection("charge_"+MM_Side[iside]+"_sector_"+MM_Sector[isector]+"_phi"+std::to_string(statPhi+1)+"_stationEta"+EtaSector[statEta]+"_multiplet"+std::to_string(multiplet+1)+"_gas_gap"+std::to_string(gas_gap+1), Vectors.charge);
	      auto mu_TPC_angle_perLayer = Monitored::Collection("mu_TPC_angle_"+MM_Side[iside]+"_sector_"+MM_Sector[isector]+"_phi"+std::to_string(statPhi+1)+"_stationEta"+EtaSector[statEta]+"_multiplet"+std::to_string(multiplet+1)+"_gas_gap"+std::to_string(gas_gap+1),Vectors.mu_TPC_angle);

	      fill(MM_sideGroup,charge_perLayer,mu_TPC_angle_perLayer);
	      
	    }
	  }
	}
      }
    }
  }
 
  return StatusCode::SUCCESS;
    
}

StatusCode MMRawDataMonAlg::fillMMHistograms( const Muon::MMPrepData* ) const{

  return StatusCode::SUCCESS;
}

                                                                                                           
      
void MMRawDataMonAlg::clusterFromTrack(const xAOD::TrackParticleContainer*  muonContainer, int lb) const{

  MMSummaryHistogramStruct summaryPlots[2][2][4];
  MMOverviewHistogramStruct overviewPlots;
  int nmu=0;

  for (const xAOD::TrackParticle* meTP  : *muonContainer){

    if (meTP) {
      nmu++;

      // retrieve the original track                                                       
      const Trk::Track* meTrack = meTP->track();
      if (meTrack) {
        // get the vector of measurements on track                                                           
       
	const DataVector<const Trk::MeasurementBase>* meas = meTrack->measurementsOnTrack();
	
	for(const Trk::MeasurementBase* it: *meas){
	
	  const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(it);
	  
	  if (rot) {
	      
	    Identifier rot_id = rot->identify();
	    if (m_idHelperSvc->isMM(rot_id)) {
	      const Muon::MMClusterOnTrack* cluster = dynamic_cast<const Muon::MMClusterOnTrack*>(rot);
	      
	      if (cluster) {
		
		std::string stName   = m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(rot_id));
		int stNumber    = m_idHelperSvc->mmIdHelper().stationName(rot_id);
		int stEta= m_idHelperSvc->mmIdHelper().stationEta(rot_id);
		int stPhi= m_idHelperSvc->mmIdHelper().stationPhi(rot_id);
		int multi = m_idHelperSvc->mmIdHelper().multilayer(rot_id);
		int gap=  m_idHelperSvc->mmIdHelper().gasGap(rot_id);
		int ch=  m_idHelperSvc->mmIdHelper().channel(rot_id);
                        
		//MMS and MML phi sectors                                                                    
		int phisec=0;
		if (stNumber%2 == 0) phisec=1;
		
		int sectorPhi=get_sectorPhi_from_stationPhi_stName(stPhi,stName);
		
		PCB=get_PCB_from_channel(ch);
		
		auto& vects=overviewPlots;
		
		//Occupancy plots with PCB granularity further divided for each eta sector: -2, -1, 1, 2  
		//Filling Vectors for stationEta=-1 - cluster on track
		if (stEta==-1){
		  vects.stationPhi_CSide_eta1_ontrack.push_back(sectorPhi);
		  vects.sector_CSide_eta1_ontrack.push_back(get_bin_for_occ_CSide_pcb_eta1_hist(stEta,multi,gap,PCB));
		  vects.sector_lb_CSide_eta1_ontrack.push_back(get_bin_for_occ_lb_CSide_pcb_eta1_hist(stEta,multi,gap,PCB,phisec));
		}
		//Filling Vectors for stationEta=-2 - cluster on track 
		else if (stEta==-2){
		  vects.stationPhi_CSide_eta2_ontrack.push_back(sectorPhi);
		  vects.sector_CSide_eta2_ontrack.push_back(get_bin_for_occ_CSide_pcb_eta2_hist(stEta,multi,gap,PCB));
		  vects.sector_lb_CSide_eta2_ontrack.push_back(get_bin_for_occ_lb_CSide_pcb_eta2_hist(stEta,multi,gap,PCB,phisec));
		}
		//Filling Vectors for stationEta=1 - cluster on track 
		else if (stEta==1){
		  vects.stationPhi_ASide_eta1_ontrack.push_back(sectorPhi);
		  vects.sector_ASide_eta1_ontrack.push_back(get_bin_for_occ_ASide_pcb_eta1_hist(stEta,multi,gap,PCB));
		  vects.sector_lb_ASide_eta1_ontrack.push_back(get_bin_for_occ_lb_ASide_pcb_eta1_hist(stEta,multi,gap,PCB,phisec));
		}
		//Filling Vectors for stationEta=2 - cluster on track 
		else {
		  vects.stationPhi_ASide_eta2_ontrack.push_back(sectorPhi);
		  vects.sector_ASide_eta2_ontrack.push_back(get_bin_for_occ_ASide_pcb_eta2_hist(stEta,multi,gap,PCB));
		  vects.sector_lb_ASide_eta2_ontrack.push_back(get_bin_for_occ_lb_ASide_pcb_eta2_hist(stEta,multi,gap,PCB,phisec));
		  
		  }
	                      
	      } //if cluster
	    } //isMM
	  } // if rot
	} // loop on meas
	
	for (const Trk::TrackStateOnSurface* trkState: *meTrack->trackStateOnSurfaces()) {
	  
	  if(!(trkState)) continue;
	  Identifier surfaceId = (trkState)->surface().associatedDetectorElementIdentifier();
	  if(!m_idHelperSvc->isMM(surfaceId)) continue;
	  
	  const Amg::Vector3D& pos    = (trkState)->trackParameters()->position();
	  int stEta= m_idHelperSvc->mmIdHelper().stationEta(surfaceId);
	  int multi = m_idHelperSvc->mmIdHelper().multilayer(surfaceId);
	  int gap=  m_idHelperSvc->mmIdHelper().gasGap(surfaceId);

	  //CSide and ASide                                                                                  
	  int iside=0;
	  if(stEta>0) iside=1;

	  auto& Vectors = summaryPlots[iside][multi-1][gap-1];
	  
	  //Filling x-y position vectors using the trackStateonSurface 
	    Vectors.x_ontrack.push_back(pos.x());
	    Vectors.y_ontrack.push_back(pos.y());
	
	  
	}
      } // if meTrack
    } // if muon
  } //loop on muonContainer

  auto& vects=overviewPlots;

  auto stationPhi_CSide_eta1_ontrack = Monitored::Collection("stationPhi_CSide_eta1_ontrack",vects.stationPhi_CSide_eta1_ontrack);
  auto stationPhi_CSide_eta2_ontrack = Monitored::Collection("stationPhi_CSide_eta2_ontrack",vects.stationPhi_CSide_eta2_ontrack);
  auto stationPhi_ASide_eta1_ontrack = Monitored::Collection("stationPhi_ASide_eta1_ontrack",vects.stationPhi_ASide_eta1_ontrack);
  auto stationPhi_ASide_eta2_ontrack = Monitored::Collection("stationPhi_ASide_eta2_ontrack",vects.stationPhi_ASide_eta2_ontrack);
  auto sector_ASide_eta1_ontrack = Monitored::Collection("sector_ASide_eta1_ontrack",vects.sector_ASide_eta1_ontrack);
  auto sector_ASide_eta2_ontrack = Monitored::Collection("sector_ASide_eta2_ontrack",vects.sector_ASide_eta2_ontrack);
  auto sector_CSide_eta2_ontrack = Monitored::Collection("sector_CSide_eta2_ontrack",vects.sector_CSide_eta2_ontrack);                                                                                         
  auto sector_CSide_eta1_ontrack = Monitored::Collection("sector_CSide_eta1_ontrack",vects.sector_CSide_eta1_ontrack);   

  auto lb_ontrack = Monitored::Scalar<int>("lb_ontrack", lb);

  auto sector_lb_CSide_eta2_ontrack = Monitored::Collection("sector_lb_CSide_eta2_ontrack",vects.sector_lb_CSide_eta2_ontrack);
  auto sector_lb_CSide_eta1_ontrack = Monitored::Collection("sector_lb_CSide_eta1_ontrack",vects.sector_lb_CSide_eta1_ontrack);
  auto sector_lb_ASide_eta2_ontrack = Monitored::Collection("sector_lb_ASide_eta2_ontrack",vects.sector_lb_ASide_eta2_ontrack);
  auto sector_lb_ASide_eta1_ontrack = Monitored::Collection("sector_lb_ASide_eta1_ontrack",vects.sector_lb_ASide_eta1_ontrack);

  fill("mmMonitor",stationPhi_CSide_eta1_ontrack,stationPhi_CSide_eta2_ontrack,stationPhi_ASide_eta1_ontrack,stationPhi_ASide_eta2_ontrack,sector_CSide_eta1_ontrack,sector_CSide_eta2_ontrack,sector_ASide_eta1_ontrack,sector_ASide_eta2_ontrack,sector_lb_CSide_eta2_ontrack,sector_lb_CSide_eta1_ontrack,sector_lb_ASide_eta2_ontrack,sector_lb_ASide_eta1_ontrack,lb_ontrack);
    
  for (int iside=0;iside<2;iside++){
   std::string MM_sideGroup = "MM_sideGroup"+MM_Side[iside];
   for( int multiplet=0; multiplet<2; multiplet++) {
     for( int gas_gap=0; gas_gap<4; gas_gap++) {
	      
       auto& Vectors = summaryPlots[iside][multiplet][gas_gap];

       auto x_ontrack = Monitored::Collection("x_"+MM_Side[iside]+"_multiplet"+std::to_string(multiplet+1)+"_gas_gap_"+std::to_string(gas_gap+1)+"_ontrack", Vectors.x_ontrack);
       auto y_ontrack = Monitored::Collection("y_"+MM_Side[iside]+"_multiplet"+std::to_string(multiplet+1)+"_gas_gap_"+std::to_string(gas_gap+1)+"_ontrack", Vectors.y_ontrack);
      
       fill(MM_sideGroup,x_ontrack,y_ontrack);
     }          
   }
  }
           
  
}
                                                                                                             
        
                                                                                                           
         
          
        

                                                                                                                      
                                                                                                             
