/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief  Algorithm to analyse the detector timing info, to be run after the l1 correlation algorithm
 * @author Nils Ruthmann, Till Eifert
 *
 * $Id: $
 */

#include "TrigGenericAlgs/DetectorTimingAlgo.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Property.h"

#include <math.h>
#include <sstream>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/RecMuonRoI.h"

#include "xAODTrigger/TrigCompositeContainer.h"

using namespace std;


const short N_ROBID_BITS(24);  // 3 bytes for ROB ID

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
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc",name),
    m_trigMuonRoITool( "TrigMuonRoITool" ),
    m_recRPCRoiSvc("LVL1RPC::RPCRecRoiSvc",""),
    m_rpcDataPreparator("TrigL2MuonSA::RpcDataPreparator/RpcDataPreparator")
    
  
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
  declareMonitoredVariable("EF_TT_mean",TT_mean,-50);
  declareMonitoredVariable("EF_TT_sigma",TT_sigma,-1);
  declareMonitoredVariable("EF_TT_nEM",TT_nEM,0);
  declareMonitoredVariable("EF_TT_nHad",TT_nHad,0);
  declareMonitoredVariable("EF_TT_total",TT_total,0);

  declareMonitoredVariable("EF_ObjAlg_accept_symmetric",ObjAlg_accept_symmetric,0);

  declareMonitoredStdContainer("EF_TT_timings",TT_timings, AutoClear);

 

  declareMonitoredStdContainer("EF_Obj_timings",combinedObjects_timings, AutoClear);
  declareMonitoredVariable("EF_Obj_mean",combinedObjects_mean,-50);
  declareMonitoredVariable("EF_Obj_sigma",combinedObjects_sigma,-1);

  declareMonitoredStdContainer("EF_mu_timings",mu_timings, AutoClear);
  declareMonitoredVariable("EF_mu_mean",mu_mean,-50);
  declareMonitoredVariable("EF_mu_sigma",mu_sigma,-1);
}


HLT::ErrorCode DetectorTimingAlgo::hltInitialize()
{
  ServiceHandle<IJobOptionsSvc> jobOptSvc("JobOptionsSvc", name());
  if (m_configSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve " << m_configSvc);
    return HLT::BAD_JOB_SETUP;
  }


  if (  m_rpcDataPreparator.retrieve().isFailure() ) {
    msg() << MSG::ERROR << "Could not retrieve " << m_rpcDataPreparator << endreq;
    return HLT::BAD_JOB_SETUP;
  }                    
  m_rpcDataPreparator->setRoIBasedDataAccess(false);

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


void DetectorTimingAlgo::updateHandler(Property&)
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


  summed_L1AClusterE=0;
  weighted_L1AClusterTime=0;

  max_L1AClusterTime_2=-30;
  max_L1AClusterTime_10=-30;

  closest_L1AClusterTime=0;
  accept=0;
  m_clustertime_10_sigma=0;
  
  L1ACluster_E.clear();
  L1ACluster_eta.clear();
  L1ACluster_phi.clear();
  L1ACluster_t.clear();

  L1ACluster_eta_funnypeak.clear();
  L1ACluster_phi_funnypeak.clear();
  L1ACluster_t_funnypeak.clear();

  //std::cout<<" ------ start event --------- " << std::endl;

  // Below all functionality about caloclusters and L1 ROI retrival is only commented out.
  // We might want to reconsider our choices later.
  // For now use TT only
  int beforeafterflag=0;
  int l1accept_type=0;
  int other_type=0;
  
  if(tes_in.size()>0){
    
    for (HLT::TEVec::const_iterator it = tes_in[0].begin(); it != tes_in[0].end(); ++it) {
      const xAOD::TrigCompositeContainer * passBitContainer ;   
      HLT::ErrorCode status = getFeature(  (*it) , passBitContainer);
      for (xAOD::TrigCompositeContainer::const_iterator it = passBitContainer->begin(); it != passBitContainer->end(); ++it ){
	(*it)->getDetail("beforeafterflag",beforeafterflag);
	(*it)->getDetail("l1a_type",l1accept_type);
	(*it)->getDetail("other_type",other_type);
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

    
    if ( (l1accept_type==2 ||  l1accept_type==4 || l1accept_type==6 || l1accept_type==7) || ( other_type==2 ||  other_type==4 || other_type==6 || other_type==7) ){

      // now get a list of the in and out of time muon ROIS:
      if( (l1accept_type==2 ||  l1accept_type==4 || l1accept_type==6 || l1accept_type==7) ){
	for  (std::vector< ROIB::MuCTPIRoI >::const_iterator it = m_trigMuonRoITool->begin_InTimeRoIs();
	      it != m_trigMuonRoITool->end_InTimeRoIs(); ++it) {

	  if ( ((*it).pt() <6  )) continue;

	  //// need to reject the long L1 signals.. reject rois which are the same as in the BCID before:
	  bool overlapsintime=false;
	  for  (std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator itoot = m_trigMuonRoITool->begin_OutOfTimeRoIs();
		itoot != m_trigMuonRoITool->end_OutOfTimeRoIs(); ++itoot) {
	    if ( (*itoot).second != -1 ) continue; // if its not one BCID before continue
	    if( ((*itoot).first).getSectorID() == (*it).getSectorID() &&
		((*itoot).first).getSectorAddress() == (*it).getSectorAddress() ){
	      overlapsintime=true;
	      break;
	    }
	  }  
	  if(overlapsintime) continue;
	  
	  unsigned int temp_sysID = getBitMaskValue(((*it)).getSectorAddress(), LVL1::SysIDMask );
	  unsigned int sysID = 0;                // Barrel
	  if( temp_sysID & 0x2 ) sysID = 1;      // Endcap
	  else if( temp_sysID & 0x1 ) sysID = 2; // Forward

     
	  
	  if( sysID == 0 ) {  // only if its in the RPC domain.. we are not interested in the other detectors
	    m_recRPCRoiSvc->reconstruct( ((*it)).roIWord() );
	    float eta = m_recRPCRoiSvc->eta();
	    float phi = m_recRPCRoiSvc->phi();

	    m_muonroi_intime_eta.push_back( eta );
	    m_muonroi_intime_phi.push_back( phi );
	  }
	}
      }/// in the current BCID we found a muon ROI

      
      if ( other_type==2 ||  other_type==4 || other_type==6 || other_type==7){
	
	for  (std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator it = m_trigMuonRoITool->begin_OutOfTimeRoIs();
	      it != m_trigMuonRoITool->end_OutOfTimeRoIs(); ++it) {
	  if (abs((*it).second) !=1) continue;
	  if ( ((*it).first).pt() < 6 ) continue;

	  //// need to reject the long L1 signals.. reject rois which are the same as in the central BCID:
	  if( (*it).second == +1 ) { // if this ROI is actually a late ROI.. for early ones we don't do anything
	    bool overlapsintime=false;
	    for  (std::vector< ROIB::MuCTPIRoI >::const_iterator itintime = m_trigMuonRoITool->begin_InTimeRoIs();
		  itintime != m_trigMuonRoITool->end_InTimeRoIs(); ++itintime) {
	      if( ((*it).first).getSectorID() == (*itintime).getSectorID() &&
		  ((*it).first).getSectorAddress() == (*itintime).getSectorAddress() ){
		overlapsintime=true;
		break;
	      }
	    }
	    if(overlapsintime) continue;
	  }
	  unsigned int temp_sysID = getBitMaskValue(((*it).first).getSectorAddress(), LVL1::SysIDMask );
	  unsigned int sysID = 0;                // Barrel
	  if( temp_sysID & 0x2 ) sysID = 1;      // Endcap
	  else if( temp_sysID & 0x1 ) sysID = 2; // Forward
	  
	  if( sysID == 0 ) {  // only if its in the RPC domain.. we are not interested in the other detectors
	    m_recRPCRoiSvc->reconstruct( ((*it).first).roIWord() );
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
      TrigL2MuonSA::RpcPatFinder dummypatfinder( &(msg()) );
      m_rpcDataPreparator->prepareData(&DummyRoiDescriptor ,
				       0  , /// dummy roiword
				       rpcHits ,
				       &dummypatfinder );


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
	  mu_timings.push_back(avgtime);
	  combinedObjects_timings.push_back(avgtime);
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
	  mu_timings.push_back(avgtime);
	  combinedObjects_timings.push_back(avgtime);
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
    // 	      L1ACluster_E.push_back((*it)->e());
    // 	      L1ACluster_t.push_back((*it)->time());
    // 	      L1ACluster_eta.push_back((*it)->eta());
    // 	      L1ACluster_phi.push_back((*it)->phi());
    // 	      summed_L1AClusterE+=(*it)->e();

    // 	      if( (*it)->e()>10000 && (*it)->time() > max_L1AClusterTime_10 )
    // 		max_L1AClusterTime_10 = (*it)->time();
    // 	      if( (*it)->e()>2000 && (*it)->time() > max_L1AClusterTime_2 )
    // 		max_L1AClusterTime_2 = (*it)->time();
	      
    // 	      // if( (*it)->time() > 6 &&  (*it)->time() <16 ){
    // 	      // 	L1ACluster_t_funnypeak.push_back((*it)->time());
    // 	      // 	L1ACluster_eta_funnypeak.push_back((*it)->eta());
    // 	      // 	L1ACluster_phi_funnypeak.push_back((*it)->phi());

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
    // 	    //msg() << MSG::INFO << " Cluster E, eta, phi, time: " << (*it)->e()<<", "<< (*it)->eta()<<", "<< (*it)->phi()<<", "<< (*it)->time() << endreq;   
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
	TT_timings.clear();
	int threshold=72;
	TT_nEM=0;
	TT_nHad=0;
	TT_total=0;
	TTAlg_accept=0;
	const xAOD::TriggerTowerContainer* container(0);
	if ( getFeature((*it),container) != HLT::OK ) {
	  msg() << MSG::WARNING << " Failed to get TT collection." << endreq;
	  return HLT::ERROR;
	}
	
	for (xAOD::TriggerTowerContainer::const_iterator eIt = m_TriggerTowers->begin(); eIt!=m_TriggerTowers->end(); ++eIt) {
	  eta=(*eIt)->eta();
	  readout=0;
	  readout = (*eIt)->adc().size();
	  if(readout==5){
	    y0=(*eIt)->adc()[0];
	    y1=(*eIt)->adc()[1];
	    y2=(*eIt)->adc()[2];
	    y3=(*eIt)->adc()[3];
	    y4=(*eIt)->adc()[4];
	  }
	  else if(readout==7){
	    y0=(*eIt)->adc()[1];
	    y1=(*eIt)->adc()[2];
	    y2=(*eIt)->adc()[3];
	    y3=(*eIt)->adc()[4];
	    y4=(*eIt)->adc()[5];
	  }
	  else if(readout==15){
	    y0=(*eIt)->adc()[3];
	    y1=(*eIt)->adc()[5];
	    y2=(*eIt)->adc()[7];
	    y3=(*eIt)->adc()[9];
	    y4=(*eIt)->adc()[11];
	  }
	  x0=-50.;
	  x1=-25.;
	  x2=0.;
	  x3=25.;
	  x4=50.;
	  if(y0>=1020 || y1>=1020 || y2>=1020 || y3>=1020 || y4>=1020){
	    continue;
	  }
	  if(y2>threshold && y2>=y1 && y2>=y3){
	    xv = time(x1,x2,x3,y1,y2,y3);
	    if((*eIt)->sampling()==1 && fabs(eta)>1.3 && fabs(eta)<1.6 && y2>750){
	      continue;
	    }
	  }
	  else if(y1>threshold && y1>y2 && y1>=y0){
	    xv = time(x0,x1,x2,y0,y1,y2);
	    if((*eIt)->sampling()==1 && fabs(eta)>1.3 && fabs(eta)<1.6 && y1>750){
	      continue;
	    }
	  }
	  else if(y3>threshold && y3>y2 && y3>=y4){
	    xv = time(x2,x3,x4,y2,y3,y4);
	    if((*eIt)->sampling()==1 && fabs(eta)>1.3 && fabs(eta)<1.6 && y3>750){
	      continue;
	    }
	  }
	  else{
	    continue;
	  }
	  if((*eIt)->sampling()==0){
	    TT_nEM++;
	    TT_total++;
	  }
	  else if((*eIt)->sampling()==1){
	    TT_nHad++;
	    TT_total++;
	  }
	  TT_timings.push_back(xv);
	  combinedObjects_timings.push_back(xv);

	  
				
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
  
  
  // for (unsigned int ic=0; ic< L1ACluster_E.size(); ic++){
  //   weighted_L1AClusterTime +=  L1ACluster_t[ic] * L1ACluster_E[ic]/summed_L1AClusterE;    
  // }


  //  if( weighted_L1AClusterTime > 19 || max_L1AClusterTime_10  > 21 || max_L1AClusterTime_10  < -29 ){
  //   accept=1; 
  // }

  //  //  if( weighted_L1AClusterTime < 19 || max_L1AClusterTime_10  < 19 ){
  //   //    std::cout<<" ------ this event has average matched cluster time < 19 ns --------- " << std::endl;
  //   // std::cout<<" avg. weighted time: " << weighted_L1AClusterTime << " max. time 10: "<<max_L1AClusterTime_10<<"  max. time 2: "<<max_L1AClusterTime_2  <<std::endl;
  //  //  }
  // //  std::cout<<" ------ end event --------- " << std::endl;

   int size = TT_timings.size();
   if(size==0){
     TT_median=-50;
     TT_mean=-50;
     TT_sigma=-1;
   }
   else{
    

     float n = 0;
     float tot = 0, tot2 =0, adouble;
     for(int i=0; i<size; i++){
       adouble=TT_timings[i];
       tot += adouble; tot2 += adouble*adouble;
       ++n;
     }
     float n1 = 1./n;
     TT_mean = tot*n1;
     TT_sigma = sqrt(fabs(tot2*n1 -TT_mean*TT_mean));
   }

 

   size = mu_timings.size();
   if(size==0){
     mu_mean=-50;
     mu_sigma=-1;
   }
   else{
     float n = 0;
     float tot = 0, tot2 =0, adouble;
     for(int i=0; i<size; i++){
       adouble=mu_timings[i];
       tot += adouble; tot2 += adouble*adouble;
       ++n;
     }
     float n1 = 1./n;
     mu_mean = tot*n1;
     mu_sigma = sqrt(fabs(tot2*n1 -mu_mean*mu_mean));
   }

   size = combinedObjects_timings.size();
   if(size==0){
     combinedObjects_mean=-50;
     combinedObjects_sigma=-1;
   }
   else{
     float n = 0;
     float tot = 0, tot2 =0, adouble;
     for(int i=0; i<size; i++){
       adouble=combinedObjects_timings[i];
       tot += adouble; tot2 += adouble*adouble;
       ++n;
     }
     
     float n1 = n>0 ? 1./n : 0;
     combinedObjects_mean = tot*n1;
     combinedObjects_sigma = sqrt(fabs(tot2*n1 -combinedObjects_mean*combinedObjects_mean));
   }
     
   
   //   TTAlg_accept=0;
   // if(TT_mean>22 && TT_nEM>1 && TT_sigma<4){
   //  TTAlg_accept=1;
     // std::vector<HLT::TriggerElement*> empty_seed;
     // HLT::TriggerElement* te = config()->getNavigation()->addNode(empty_seed, output);
     // te->setActiveState(true);
   //  }

   // TTAlg_accept=0;
   // if(combinedObjects_mean>22 && combinedObjects_sigma<4){
   //   TTAlg_accept=1;
   //   std::vector<HLT::TriggerElement*> empty_seed;
   //   HLT::TriggerElement* te = config()->getNavigation()->addNode(empty_seed, output);
   //   te->setActiveState(true);
   // }


   ObjAlg_accept_symmetric = 0;
   if(combinedObjects_mean>-45 && fabs(combinedObjects_mean)>22 && combinedObjects_sigma<4){
     ObjAlg_accept_symmetric=1;
     std::vector<HLT::TriggerElement*> empty_seed;
     HLT::TriggerElement* te = config()->getNavigation()->addNode(empty_seed, output);
     te->setActiveState(true);
   }

   // std::cout<<"---  Decision of timingalg: ---"<<std::endl;
   // std::cout<<"Dec: "<<ObjAlg_accept_symmetric <<" TTAlg accept: " << TTAlg_accept<<" nEM:" <<TT_nEM << " obj timing mean: " <<combinedObjects_mean <<" sigma: "<<combinedObjects_sigma <<std::endl;

   afterExecMonitors().ignore();

   return HLT::OK;
}


