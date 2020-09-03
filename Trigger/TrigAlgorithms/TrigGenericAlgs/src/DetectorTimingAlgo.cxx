/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief  Algorithm to analyse the detector timing info, to be run after the l1 correlation algorithm
 * @author Nils Ruthmann, Till Eifert
 *
 * $Id: $
 */

#include "TrigGenericAlgs/DetectorTimingAlgo.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "Gaudi/Property.h"

#include <math.h>
#include <sstream>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/RecMuonRoI.h"

#include "xAODTrigger/TrigCompositeContainer.h"

using namespace std;


//const short N_ROBID_BITS(24);  // 3 bytes for ROB ID

unsigned int DetectorTimingAlgo::getBitMaskValue( const unsigned int uintValue,
						  const unsigned int mask ) {
  unsigned int result;
  unsigned int maskcopy;
  maskcopy = mask;
  result = uintValue & mask;
  if ( mask != 0 ) {
    while ( ( maskcopy & 0x00000001 ) == 0 ) {
      maskcopy = maskcopy >> 1;
      result = result >> 1;
    }
  }
  return result;
}

DetectorTimingAlgo::DetectorTimingAlgo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_rpcDataPreparator("TrigL2MuonSA::RpcDataPreparator/RpcDataPreparator"),
    m_trigMuonRoITool( "TrigMuonRoITool" ),
    m_dummypatfinder("TrigL2MuonSA::RpcPatFinder"),
    m_recRPCRoiSvc("LVL1RPC::RPCRecRoiSvc",""),
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc",name)
{
  m_caloCluster=0;

 
  // declareMonitoredStdContainer("EF_clustertime_10", m_clustertime_10, AutoClear);

  // declareMonitoredStdContainer("EF_rpctime_intime", m_rpctime_intimemuonl1, AutoClear);
  // declareMonitoredStdContainer("EF_rpctime_other", m_rpctime_othermuonl1, AutoClear);

  declareMonitoredStdContainer("EF_muonroi_intime_eta",  m_muonroi_intime_eta, AutoClear);
  declareMonitoredStdContainer("EF_muonroi_intime_phi",  m_muonroi_intime_phi, AutoClear);
  declareMonitoredStdContainer("EF_muonroi_outtime_eta",  m_muonroi_outtime_eta, AutoClear);
  declareMonitoredStdContainer("EF_muonroi_outtime_phi",  m_muonroi_outtime_phi, AutoClear);

  declareMonitoredStdContainer("EF_rpctime_matched_intimeroi",  m_rpctime_matched_intimeroi, AutoClear);
  declareMonitoredStdContainer("EF_rpctime_matched_outtimeroi",  m_rpctime_matched_outtimeroi, AutoClear);

  declareMonitoredStdContainer("EF_rpctime_matched_intimeroi_averaged",  m_rpctime_matched_intimeroi_averaged, AutoClear);
  declareMonitoredStdContainer("EF_rpctime_matched_outtimeroi_averaged",  m_rpctime_matched_outtimeroi_averaged, AutoClear);

  // declareMonitoredStdContainer("EF_rpctime_matched_intimeroi_averaged_NOL1BC",  m_rpctime_matched_intimeroi_averaged_NOL1BC, AutoClear);
  // declareMonitoredStdContainer("EF_rpctime_matched_outtimeroi_averaged_NOL1BC",  m_rpctime_matched_outtimeroi_averaged_NOL1BC, AutoClear);

  // declareMonitoredStdContainer("EF_rpceta_matched_intimeroi",  m_rpceta_matched_intimeroi, AutoClear);
  // declareMonitoredStdContainer("EF_rpcphi_matched_intimeroi",  m_rpcphi_matched_intimeroi, AutoClear);
  // declareMonitoredStdContainer("EF_rpceta_matched_outtimeroi",  m_rpceta_matched_outtimeroi, AutoClear);
  // declareMonitoredStdContainer("EF_rpcphi_matched_outtimeroi",  m_rpcphi_matched_outtimeroi, AutoClear);
   
  ////////TriggerTowers/////////
  declareMonitoredVariable("EF_TT_mean",m_TT_mean,-50);
  declareMonitoredVariable("EF_TT_sigma",m_TT_sigma,-1);
  declareMonitoredVariable("EF_TT_nEM",m_TT_nEM,0);
  declareMonitoredVariable("EF_TT_nHad",m_TT_nHad,0);
  declareMonitoredVariable("EF_TT_total",m_TT_total,0);

  declareMonitoredVariable("EF_ObjAlg_accept_symmetric",m_ObjAlg_accept_symmetric,0);

  declareMonitoredStdContainer("EF_TT_timings",m_TT_timings, AutoClear);

 

  declareMonitoredStdContainer("EF_Obj_timings",m_combinedObjects_timings, AutoClear);
  declareMonitoredVariable("EF_Obj_mean",m_combinedObjects_mean,-50);
  declareMonitoredVariable("EF_Obj_sigma",m_combinedObjects_sigma,-1);

  declareMonitoredStdContainer("EF_mu_timings",m_mu_timings, AutoClear);
  declareMonitoredVariable("EF_mu_mean",m_mu_mean,-50);
  declareMonitoredVariable("EF_mu_sigma",m_mu_sigma,-1);
}


HLT::ErrorCode DetectorTimingAlgo::hltInitialize()
{
  ServiceHandle<IJobOptionsSvc> jobOptSvc("JobOptionsSvc", name());
  if (m_configSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve " << m_configSvc);
    return HLT::BAD_JOB_SETUP;
  }


  if (  m_rpcDataPreparator.retrieve().isFailure() ) {
    msg() << MSG::ERROR << "Could not retrieve " << m_rpcDataPreparator << endmsg;
    return HLT::BAD_JOB_SETUP;
  }                    
  m_rpcDataPreparator->setRoIBasedDataAccess(false);

  if (  m_dummypatfinder.retrieve().isFailure() ) {
    msg() << MSG::ERROR << "Could not retrieve " << m_dummypatfinder << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  
  
  // Retrieve the MuonRoITool
  StatusCode sc = m_trigMuonRoITool.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Could not retrieve " << m_trigMuonRoITool );
    return HLT::ERROR;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_trigMuonRoITool);
  }
  sc = m_recRPCRoiSvc.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't connect to " << m_recRPCRoiSvc );
    return HLT::ERROR;
  } else {
   ATH_MSG_INFO( "Retrieved Service " << m_recRPCRoiSvc );
  }
  

   return HLT::OK;
}


void DetectorTimingAlgo::updateHandler(Gaudi::Details::PropertyBase&)
{
  //  parseROBRequest();
}

float time (float x1, float x2, float x3, int y1, int y2, int y3){
	float denom = (x1 - x2) * (x1 - x3) * (x2 - x3);
	float A     = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
	float B     = (x3*x3 * (y1 - y2) + x2*x2 * (y3 - y1) + x1*x1 * (y2 - y3)) / denom;
	float xv = (-B / (2*A))-4;
	return xv;
}

/*

*/
HLT::ErrorCode DetectorTimingAlgo::hltExecute(std::vector<HLT::TEVec>& tes_in, unsigned int output)
{
  m_caloCluster=0;
  //  m_muonColl=0;
  //  m_roiDescriptor = 0;

  beforeExecMonitors().ignore();

  m_l1roi_eta.clear();
  m_l1roi_phi.clear();
    


  //  int nclus_ootBC_10=0;
  //  int nclus_10=0;


  m_summed_L1AClusterE=0;
  m_weighted_L1AClusterTime=0;

  m_max_L1AClusterTime_2=-30;
  m_max_L1AClusterTime_10=-30;

  m_closest_L1AClusterTime=0;
  m_accept=0;
  m_clustertime_10_sigma=0;
  
  m_L1ACluster_E.clear();
  m_L1ACluster_eta.clear();
  m_L1ACluster_phi.clear();
  m_L1ACluster_t.clear();

  m_L1ACluster_eta_funnypeak.clear();
  m_L1ACluster_phi_funnypeak.clear();
  m_L1ACluster_t_funnypeak.clear();

  //std::cout<<" ------ start event --------- " << std::endl;

  // Below all functionality about caloclusters and L1 ROI retrival is only commented out.
  // We might want to reconsider our choices later.
  // For now use TT only
  int beforeafterflag=0;
  int l1accept_type=0;
  int other_type=0;
  
  if(tes_in.size()>0){
    
    for (HLT::TEVec::const_iterator it = tes_in[0].begin(); it != tes_in[0].end(); ++it) {
      const xAOD::TrigCompositeContainer * passBitContainer(0) ;   
      HLT::ErrorCode status = getFeature(  (*it) , passBitContainer);
      if (( status != HLT::OK ) || (!passBitContainer)) {
	ATH_MSG_WARNING(" Failed to get passBitContainer. Status = " << status << ", passBitContainer = " << passBitContainer);
      } else {
	for (xAOD::TrigCompositeContainer::const_iterator itc = passBitContainer->begin(); itc != passBitContainer->end(); ++itc ){
	  (*itc)->getDetail("beforeafterflag",beforeafterflag);
	  (*itc)->getDetail("l1a_type",l1accept_type);
	  (*itc)->getDetail("other_type",other_type);
	}
      }
    } // retrieve the encoded info from the L1Correlation algo

    //std::cout<< "I retrieved the following info: beforeafter:"<< beforeafterflag<< " l1at:"<< l1accept_type<<" othertype:"<< other_type<<std::endl;
    
    // Retrieve ROIS

    // m_roiDescriptor=0;
    // m_roiDescriptorVector.clear();

    // // [2] EM ROI : 
    // for (HLT::TEVec::const_iterator it = tes_in[2].begin(); it != tes_in[2].end(); ++it) {

    //   HLT::ErrorCode status = getFeatures(  (*it) , m_roiDescriptorVector );

    //   if(status!=HLT::OK) {
    //   }else{
    // 	for( unsigned int iroi=0; iroi<m_roiDescriptorVector.size(); iroi++){

    // 	  m_roiDescriptor=m_roiDescriptorVector[iroi];
    // 	  //std::cout<<"!!!!This TE has A EM roidescriptor!! ETA="<<m_roiDescriptor->eta()<< " Phi="<< m_roiDescriptor->phi() << std::endl;
    // 	  m_l1roi_eta.push_back(m_roiDescriptor->eta());
    // 	  m_l1roi_phi.push_back(m_roiDescriptor->phi());
    // 	}
    //   }
            
    // }

    // // [3] Jet ROI : 
    // m_roiDescriptor=0;
    // m_roiDescriptorVector.clear();
    // for (HLT::TEVec::const_iterator it = tes_in[3].begin(); it != tes_in[3].end(); ++it) {
    //   HLT::ErrorCode status = getFeatures(  (*it) , m_roiDescriptorVector );
    //   if(status!=HLT::OK) {
    //   }else{
    // 	for( unsigned int iroi=0; iroi<m_roiDescriptorVector.size(); iroi++){
    // 	  m_roiDescriptor=m_roiDescriptorVector[iroi];
    // 	  //std::cout<<"!!!!This TE has A Jet roidescriptor!! ETA="<<m_roiDescriptor->eta()<< " Phi="<< m_roiDescriptor->phi() << std::endl;
    // 	  m_l1roi_eta.push_back(m_roiDescriptor->eta());
    // 	  m_l1roi_phi.push_back(m_roiDescriptor->phi());
    // 	}
    //   }
      
    // }


    //get muon RoIs
    auto roiVectors = m_trigMuonRoITool->decodeMuCTPi();
    if(!roiVectors){
      ATH_MSG_VERBOSE("No RoIs found");
      return HLT::OK;
    }

    if ( (l1accept_type==2 ||  l1accept_type==4 || l1accept_type==6 || l1accept_type==7) || ( other_type==2 ||  other_type==4 || other_type==6 || other_type==7) ){

      // now get a list of the in and out of time muon ROIS:
      if( (l1accept_type==2 ||  l1accept_type==4 || l1accept_type==6 || l1accept_type==7) ){
	//loop over in time RoIs
	for(const auto& it : *(roiVectors->inTimeRois)){

	  if ( ((it).pt() <6  )) continue;

	  //// need to reject the long L1 signals.. reject rois which are the same as in the BCID before:
	  bool overlapsintime=false;
	  //loop over out of time rois
	  for(const auto& itoot : *(roiVectors->outOfTimeRois)){
	    if ( (itoot).second != -1 ) continue; // if its not one BCID before continue
	    if( ((itoot).first).getSectorID() == (it).getSectorID() &&
		((itoot).first).getSectorAddress() == (it).getSectorAddress() ){
	      overlapsintime=true;
	      break;
	    }
	  }  
	  if(overlapsintime) continue;
	  
	  unsigned int temp_sysID = getBitMaskValue(((it)).getSectorAddress(), LVL1::SysIDMask );
	  unsigned int sysID = 0;                // Barrel
	  if( temp_sysID & 0x2 ) sysID = 1;      // Endcap
	  else if( temp_sysID & 0x1 ) sysID = 2; // Forward

     
	  
	  if( sysID == 0 ) {  // only if its in the RPC domain.. we are not interested in the other detectors
	    m_recRPCRoiSvc->reconstruct( ((it)).roIWord() );
	    float eta = m_recRPCRoiSvc->eta();
	    float phi = m_recRPCRoiSvc->phi();

	    m_muonroi_intime_eta.push_back( eta );
	    m_muonroi_intime_phi.push_back( phi );
	  }
	}
      }/// in the current BCID we found a muon ROI

      
      if ( other_type==2 ||  other_type==4 || other_type==6 || other_type==7){

	//loop over out of time rois
	for(const auto& it : *(roiVectors->outOfTimeRois)){
	  if (abs((it).second) !=1) continue;
	  if ( ((it).first).pt() < 6 ) continue;

	  //// need to reject the long L1 signals.. reject rois which are the same as in the central BCID:
	  if( (it).second == +1 ) { // if this ROI is actually a late ROI.. for early ones we don't do anything
	    bool overlapsintime=false;
	    //loop over in time rois
	    for(const auto& itintime : *(roiVectors->inTimeRois)){
	      if( ((it).first).getSectorID() == (itintime).getSectorID() &&
		  ((it).first).getSectorAddress() == (itintime).getSectorAddress() ){
		overlapsintime=true;
		break;
	      }
	    }
	    if(overlapsintime) continue;
	  }
	  unsigned int temp_sysID = getBitMaskValue(((it).first).getSectorAddress(), LVL1::SysIDMask );
	  unsigned int sysID = 0;                // Barrel
	  if( temp_sysID & 0x2 ) sysID = 1;      // Endcap
	  else if( temp_sysID & 0x1 ) sysID = 2; // Forward
	  
	  if( sysID == 0 ) {  // only if its in the RPC domain.. we are not interested in the other detectors
	    m_recRPCRoiSvc->reconstruct( ((it).first).roIWord() );
	    float eta = m_recRPCRoiSvc->eta();
	    float phi = m_recRPCRoiSvc->phi();
	     
	    m_muonroi_outtime_eta.push_back( eta );
	    m_muonroi_outtime_phi.push_back( phi );
	  }
	  
	}// out of time muon rois
      }// we found a muon L1 in a neihgbouring BC

      
      /////// retrieve all the rpc hits
      
      TrigRoiDescriptor DummyRoiDescriptor;
      TrigL2MuonSA::RpcHits   rpcHits;
      m_dummypatfinder->clear();
      if ( (m_rpcDataPreparator->prepareData(&DummyRoiDescriptor ,
					     0  , /// dummy roiword
					     rpcHits ,
					     &m_dummypatfinder
					     )).isFailure() ) {
	ATH_MSG_WARNING(" RpcDataPreparator->prepareData(...) failed.");
      }

      // new solution: loop over all rois then over all hits
      // for (TrigL2MuonSA::RpcHits::const_iterator it = rpcHits.begin(); it!=rpcHits.end(); ++it) {
      // 	m_rpctime_all.push_back( (*it).time );
      // 	m_rpctime_eta.push_back( (*it).eta );
      // 	m_rpctime_phi.push_back( (*it).phi );
      // }
      
      for (unsigned int irois=0; irois<m_muonroi_intime_eta.size(); irois++ ){
	std::vector<float> tmp_rpctime_matched;
	
	for (TrigL2MuonSA::RpcHits::const_iterator it = rpcHits.begin(); it!=rpcHits.end(); ++it) {
	  float deta = fabs( (*it).eta- m_muonroi_intime_eta[irois]);
	  float dphi = fabs( (*it).phi- m_muonroi_intime_phi[irois] );
	  if (dphi > 3.14159){
	    dphi = fabs(dphi - 2*3.14159);
	  }
	  if( deta*deta + dphi*dphi < 0.0025){
	    m_rpctime_matched_intimeroi.push_back( (*it).time );
	    tmp_rpctime_matched.push_back( (*it).time );
	    // m_rpcphi_matched_intimeroi.push_back( (*it).phi );
	    // m_rpceta_matched_intimeroi.push_back( (*it).eta );
	  }
	  
	}// all rpc hits
	float avgtime=0.;
	for (unsigned int itimes=0; itimes< tmp_rpctime_matched.size(); itimes++){
	  avgtime+= tmp_rpctime_matched[itimes];
	}
	avgtime=avgtime/ tmp_rpctime_matched.size() ;

	if(beforeafterflag>-2){
	  m_rpctime_matched_intimeroi_averaged.push_back(avgtime);
	  m_mu_timings.push_back(avgtime);
	  m_combinedObjects_timings.push_back(avgtime);
	}
	// if(beforeafterflag<-1)
	//   m_rpctime_matched_intimeroi_averaged_NOL1BC.push_back(avgtime);
	
      } // intime rois

      for (unsigned int irois=0; irois<m_muonroi_outtime_eta.size(); irois++ ){
	std::vector<float> tmp_rpctime_matched;

	for (TrigL2MuonSA::RpcHits::const_iterator it = rpcHits.begin(); it!=rpcHits.end(); ++it) {
	  float deta = fabs( (*it).eta- m_muonroi_outtime_eta[irois]);
	  float dphi = fabs( (*it).phi- m_muonroi_outtime_phi[irois] );
	  if (dphi > 3.14159){
	    dphi = fabs(dphi - 2*3.14159);
	  }
	  if( deta*deta + dphi*dphi < 0.0025){
	    tmp_rpctime_matched.push_back( (*it).time );
	    m_rpctime_matched_outtimeroi.push_back( (*it).time );
	    m_rpcphi_matched_outtimeroi.push_back( (*it).phi );
	    m_rpceta_matched_outtimeroi.push_back( (*it).eta );
	  }
	  
	}// all rpc hits
	float avgtime=0.;
	for (unsigned int itimes=0; itimes< tmp_rpctime_matched.size(); itimes++){
	  avgtime+= tmp_rpctime_matched[itimes];
	}
	avgtime=avgtime/ tmp_rpctime_matched.size() ;
	if(beforeafterflag>-2){
	  m_rpctime_matched_outtimeroi_averaged.push_back(avgtime);
	  m_mu_timings.push_back(avgtime);
	  m_combinedObjects_timings.push_back(avgtime);
	}
	//	if(beforeafterflag<-1)
	// m_rpctime_matched_outtimeroi_averaged_NOL1BC.push_back(avgtime);

      }  // outime rois 

      /// If no muon ROI was found in the RPC coverage reject the event
      if( other_type==2 && m_rpctime_matched_outtimeroi_averaged.size()==0) return HLT::OK;
      if( l1accept_type==2 && m_rpctime_matched_intimeroi_averaged.size()==0) return HLT::OK; 
    } /// the first algorithm found muons
    
    ////////
    

    
  
    // // loop over caloclusters
    // for (HLT::TEVec::const_iterator it = tes_in[0].begin(); it != tes_in[0].end(); ++it) {

    //   HLT::ErrorCode status = getFeature(  (*it) , m_caloCluster );
      
    //   if(status!=HLT::OK || !m_caloCluster) {
    // 	//std::cout<<"This TE has no caloclusters"<< std::endl;
    //   } else {     
	
    // 	  for (xAOD::CaloClusterContainer::const_iterator it = m_caloCluster->begin(); it != m_caloCluster->end(); ++it ){

    // 	    // Loop over l1 rois and perform a matching
    // 	    bool L1RoiMatch=false;
    // 	    for(unsigned int ic=0; ic<m_l1roi_phi.size(); ic++ ){
    // 	      float deta = fabs( (*it)->eta()- m_l1roi_eta[ic] );
    // 	      float dphi = fabs( (*it)->phi()- m_l1roi_phi[ic] );
    // 	      if (dphi > 3.14159){
    // 		dphi = fabs(dphi - 2*3.14159);
    // 	      }
    // 	      if( (deta*deta + dphi*dphi) < 0.04)
    // 		{
    // 		  L1RoiMatch=true;
    // 		  break;
    // 		}
    // 	    }
	    
    // 	    if(L1RoiMatch ){
    // 	      //std::cout << "Matched cluster E, eta, phi, time: " << (*it)->e()<<", "<< (*it)->eta()<<", "<< (*it)->phi()<<", "<< (*it)->time() << std::endl;	      
    // 	      // Record some properties:
    // 	      m_L1ACluster_E.push_back((*it)->e());
    // 	      m_L1ACluster_t.push_back((*it)->time());
    // 	      m_L1ACluster_eta.push_back((*it)->eta());
    // 	      m_L1ACluster_phi.push_back((*it)->phi());
    // 	      m_summed_L1AClusterE+=(*it)->e();

    // 	      if( (*it)->e()>10000 && (*it)->time() > m_max_L1AClusterTime_10 )
    // 		m_max_L1AClusterTime_10 = (*it)->time();
    // 	      if( (*it)->e()>2000 && (*it)->time() > m_max_L1AClusterTime_2 )
    // 		m_max_L1AClusterTime_2 = (*it)->time();
	      
    // 	      // if( (*it)->time() > 6 &&  (*it)->time() <16 ){
    // 	      // 	m_L1ACluster_t_funnypeak.push_back((*it)->time());
    // 	      // 	m_L1ACluster_eta_funnypeak.push_back((*it)->eta());
    // 	      // 	m_L1ACluster_phi_funnypeak.push_back((*it)->phi());

    // 	      // }
    // 	    }
	    
	    
    // 	    // if ( (*it)->e()/1000. > 5){
    // 	    //   nclus_5++;
    // 	    //   m_clustertime_5.push_back((*it)->time());

    // 	    //   if( fabs( (*it)->time()) <5 ){
    // 	    // 	nclus_it_5++;
    // 	    //   }
    // 	    //   else{
    // 	    // 	nclus_oot_5++;
    // 	    // 	if( fabs( (*it)->time()) < 27 &&  fabs( (*it)->time()) > 18   )
    // 	    // 	  nclus_ootBC_5++;
    // 	    //   }

		
    // 	    // }
    // 	    if ( (*it)->e()/1000. > 10){
    // 	      nclus_10++;
    // 	      m_clustertime_10.push_back((*it)->time());
    // 	      if( fabs( (*it)->time()) < 27 &&  fabs( (*it)->time()) > 18   )
    // 		nclus_ootBC_10++;

    // 	    } 
    // 	    // if ( (*it)->e()/1000. > 25){
    // 	    //   nclus_25++;
    // 	    //   m_clustertime_25.push_back((*it)->time());
    // 	    //   if( fabs( (*it)->time()) < 27 &&  fabs( (*it)->time()) > 18   )
    // 	    // 	nclus_ootBC_25++;

    // 	    // }
    // 	    // if ( (*it)->e()/1000. > 50){
    // 	    //   nclus_50++;
    // 	    //   m_clustertime_50.push_back((*it)->time());
    // 	    //   if( fabs( (*it)->time()) < 27 &&  fabs( (*it)->time()) > 18   )
    // 	    // 	nclus_ootBC_50++;
    // 	    // } 

	    
    // 	    //std::cout << " Cluster E, eta, phi, time: " << (*it)->e()<<", "<< (*it)->eta()<<", "<< (*it)->phi()<<", "<< (*it)->time() << std::endl;
    // 	    //msg() << MSG::INFO << " Cluster E, eta, phi, time: " << (*it)->e()<<", "<< (*it)->eta()<<", "<< (*it)->phi()<<", "<< (*it)->time() << endmsg;   
    //       }
    //   }
      
    // } // the 0st input (CaloClusters)


    //    loop over TriggerTowers
    for (HLT::TEVec::const_iterator it = tes_in[1].begin(); it != tes_in[1].end(); ++it) {
      /////////////////////////Trigger Tower Code/////////////////////////////
      HLT::ErrorCode status = getFeature(  (*it) , m_TriggerTowers );
      
      if(status!=HLT::OK || !m_TriggerTowers) {
	//std::cout<<"This TE has no TriggerTowers"<< std::endl;
      } 
      else {     
	m_TT_timings.clear();
	int threshold=72;
	m_TT_nEM=0;
	m_TT_nHad=0;
	m_TT_total=0;
	m_TTAlg_accept=0;
	const xAOD::TriggerTowerContainer* container(0);
	if ( getFeature((*it),container) != HLT::OK ) {
	  msg() << MSG::WARNING << " Failed to get TT collection." << endmsg;
	  return HLT::ERROR;
	}
	
	for (xAOD::TriggerTowerContainer::const_iterator eIt = m_TriggerTowers->begin(); eIt!=m_TriggerTowers->end(); ++eIt) {
	  m_eta=(*eIt)->eta();
	  m_readout=0;
	  m_readout = (*eIt)->adc().size();
	  if(m_readout==5){
	    m_y0=(*eIt)->adc()[0];
	    m_y1=(*eIt)->adc()[1];
	    m_y2=(*eIt)->adc()[2];
	    m_y3=(*eIt)->adc()[3];
	    m_y4=(*eIt)->adc()[4];
	  }
	  else if(m_readout==7){
	    m_y0=(*eIt)->adc()[1];
	    m_y1=(*eIt)->adc()[2];
	    m_y2=(*eIt)->adc()[3];
	    m_y3=(*eIt)->adc()[4];
	    m_y4=(*eIt)->adc()[5];
	  }
	  else if(m_readout==15){
	    m_y0=(*eIt)->adc()[3];
	    m_y1=(*eIt)->adc()[5];
	    m_y2=(*eIt)->adc()[7];
	    m_y3=(*eIt)->adc()[9];
	    m_y4=(*eIt)->adc()[11];
	  }
	  m_x0=-50.;
	  m_x1=-25.;
	  m_x2=0.;
	  m_x3=25.;
	  m_x4=50.;
	  if(m_y0>=1020 || m_y1>=1020 || m_y2>=1020 || m_y3>=1020 || m_y4>=1020){
	    continue;
	  }
	  if(m_y2>threshold && m_y2>=m_y1 && m_y2>=m_y3){
	    m_xv = time(m_x1,m_x2,m_x3,m_y1,m_y2,m_y3);
	    if((*eIt)->sampling()==1 && fabs(m_eta)>1.3 && fabs(m_eta)<1.6 && m_y2>750){
	      continue;
	    }
	  }
	  else if(m_y1>threshold && m_y1>m_y2 && m_y1>=m_y0){
	    m_xv = time(m_x0,m_x1,m_x2,m_y0,m_y1,m_y2);
	    if((*eIt)->sampling()==1 && fabs(m_eta)>1.3 && fabs(m_eta)<1.6 && m_y1>750){
	      continue;
	    }
	  }
	  else if(m_y3>threshold && m_y3>m_y2 && m_y3>=m_y4){
	    m_xv = time(m_x2,m_x3,m_x4,m_y2,m_y3,m_y4);
	    if((*eIt)->sampling()==1 && fabs(m_eta)>1.3 && fabs(m_eta)<1.6 && m_y3>750){
	      continue;
	    }
	  }
	  else{
	    continue;
	  }
	  if((*eIt)->sampling()==0){
	    m_TT_nEM++;
	    m_TT_total++;
	  }
	  else if((*eIt)->sampling()==1){
	    m_TT_nHad++;
	    m_TT_total++;
	  }
	  m_TT_timings.push_back(m_xv);
	  m_combinedObjects_timings.push_back(m_xv);

	  
				
	}
      }
    }// the 1st input (TT)
    
  }else{
    //std::cout << " input TE size =0" <<std::endl;
  }

  // float timemean=0;
  // for (unsigned int ic=0; ic< m_clustertime_10.size(); ic++){
  //   timemean+=m_clustertime_10[ic];
  // }
  // timemean=timemean/ m_clustertime_10.size();

  // m_clustertime_10_sigma = 0; 
  // for (unsigned int ic=0; ic< m_clustertime_10.size(); ic++){
  //   m_clustertime_10_sigma+= (m_clustertime_10[ic] - timemean)*(m_clustertime_10[ic] - timemean);
  // }
  // m_clustertime_10_sigma=sqrt(m_clustertime_10_sigma/m_clustertime_10.size());
  
  // //  m_ootfrac_bc = (float)(nclus_ootBC_5) / nclus_5;
  // m_ootfrac_bc_10 = (float)(nclus_ootBC_10) / nclus_10;
  // //  m_ootfrac_bc_25 = (float)(nclus_ootBC_25) / nclus_25;
  // // m_ootfrac_bc_50 = (float)(nclus_ootBC_50) / nclus_50;

  // // m_ootfrac = (float)(nclus_oot_5) / nclus_5;
  // // m_itfrac = (float)(nclus_it_5)/ nclus_5;
  
  
  // for (unsigned int ic=0; ic< m_L1ACluster_E.size(); ic++){
  //   m_weighted_L1AClusterTime +=  m_L1ACluster_t[ic] * m_L1ACluster_E[ic]/m_summed_L1AClusterE;    
  // }


  //  if( m_weighted_L1AClusterTime > 19 || m_max_L1AClusterTime_10  > 21 || m_max_L1AClusterTime_10  < -29 ){
  //   m_accept=1; 
  // }

  //  //  if( m_weighted_L1AClusterTime < 19 || m_max_L1AClusterTime_10  < 19 ){
  //   //    std::cout<<" ------ this event has average matched cluster time < 19 ns --------- " << std::endl;
  //   // std::cout<<" avg. weighted time: " << m_weighted_L1AClusterTime << " max. time 10: "<<m_max_L1AClusterTime_10<<"  max. time 2: "<<m_max_L1AClusterTime_2  <<std::endl;
  //  //  }
  // //  std::cout<<" ------ end event --------- " << std::endl;

   int size = m_TT_timings.size();
   if(size==0){
     m_TT_median=-50;
     m_TT_mean=-50;
     m_TT_sigma=-1;
   }
   else{
    

     float n = 0;
     float tot = 0, tot2 =0, adouble;
     for(int i=0; i<size; i++){
       adouble=m_TT_timings[i];
       tot += adouble; tot2 += adouble*adouble;
       ++n;
     }
     float n1 = 1./n;
     m_TT_mean = tot*n1;
     m_TT_sigma = sqrt(fabs(tot2*n1 -m_TT_mean*m_TT_mean));
   }

 

   size = m_mu_timings.size();
   if(size==0){
     m_mu_mean=-50;
     m_mu_sigma=-1;
   }
   else{
     float n = 0;
     float tot = 0, tot2 =0, adouble;
     for(int i=0; i<size; i++){
       adouble=m_mu_timings[i];
       tot += adouble; tot2 += adouble*adouble;
       ++n;
     }
     float n1 = 1./n;
     m_mu_mean = tot*n1;
     m_mu_sigma = sqrt(fabs(tot2*n1 -m_mu_mean*m_mu_mean));
   }

   size = m_combinedObjects_timings.size();
   if(size==0){
     m_combinedObjects_mean=-50;
     m_combinedObjects_sigma=-1;
   }
   else{
     float n = 0;
     float tot = 0, tot2 =0, adouble;
     for(int i=0; i<size; i++){
       adouble=m_combinedObjects_timings[i];
       tot += adouble; tot2 += adouble*adouble;
       ++n;
     }
     
     float n1 = n>0 ? 1./n : 0;
     m_combinedObjects_mean = tot*n1;
     m_combinedObjects_sigma = sqrt(fabs(tot2*n1 -m_combinedObjects_mean*m_combinedObjects_mean));
   }
     
   
   //   TTAlg_accept=0;
   // if(TT_mean>22 && TT_nEM>1 && TT_sigma<4){
   //  TTAlg_accept=1;
     // std::vector<HLT::TriggerElement*> empty_seed;
     // HLT::TriggerElement* te = config()->getNavigation()->addNode(empty_seed, output);
     // te->setActiveState(true);
   //  }

   // TTAlg_accept=0;
   // if(m_combinedObjects_mean>22 && m_combinedObjects_sigma<4){
   //   TTAlg_accept=1;
   //   std::vector<HLT::TriggerElement*> empty_seed;
   //   HLT::TriggerElement* te = config()->getNavigation()->addNode(empty_seed, output);
   //   te->setActiveState(true);
   // }


   m_ObjAlg_accept_symmetric = 0;
   if(m_combinedObjects_mean>-45 && fabs(m_combinedObjects_mean)>22 && m_combinedObjects_sigma<4){
     m_ObjAlg_accept_symmetric=1;
     std::vector<HLT::TriggerElement*> empty_seed;
     HLT::TriggerElement* te = config()->getNavigation()->addNode(empty_seed, output);
     te->setActiveState(true);
   }

   // std::cout<<"---  Decision of timingalg: ---"<<std::endl;
   // std::cout<<"Dec: "<<m_ObjAlg_accept_symmetric <<" TTAlg accept: " << TTAlg_accept<<" nEM:" <<TT_nEM << " obj timing mean: " <<m_combinedObjects_mean <<" sigma: "<<m_combinedObjects_sigma <<std::endl;

   afterExecMonitors().ignore();

   return HLT::OK;
}


