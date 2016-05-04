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


using namespace std;



DetectorTimingAlgo::DetectorTimingAlgo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc",name)
    // m_dataPreparator("TrigL2MuonSA::MuFastDataPreparator"),
    //    m_backExtrapolatorTool("TrigMuonBackExtrapolator")
    // m_robDataProviderSvc("ROBDataProviderSvc", name),
    // m_trigMuonRoITool( "TrigMuonRoITool" ),
    // m_recRPCRoiSvc("LVL1RPC::RPCRecRoiSvc",""),
    // m_recTGCRoiSvc("LVL1TGC::TGCRecRoiSvc",""),
{
  m_caloCluster=0;
  //  m_muonColl=0;
  // declareProperty("noMuon",   m_nomuon = false, "no muon in the list of items to be correlated");
  // declareProperty("m_l1itemlist",   m_l1itemlist = {"L1_EM22VHI","L1_J400","L1_MU20"}, "list of l1 items to be used");
  //  declareProperty("DataPreparator",    m_dataPreparator,    "data preparator");
  // declareMonitoredVariable("EF_ootfrac_bc", m_ootfrac_bc, 0);
  //  declareMonitoredVariable("EF_ootfrac_bc_10", m_ootfrac_bc_10, 0);
  // declareMonitoredVariable("EF_ootfrac_bc_25", m_ootfrac_bc_25, 0);
  // declareMonitoredVariable("EF_ootfrac_bc_50", m_ootfrac_bc_50, 0);
  // declareMonitoredVariable("EF_ootfrac", m_ootfrac, 0);
  // declareMonitoredVariable("EF_itfrac", m_itfrac, 0);

  // declareMonitoredVariable("EF_L1AClusterTime_weighted", weighted_L1AClusterTime, 0);
  // declareMonitoredVariable("EF_L1AClusterTime_max_2", max_L1AClusterTime_2, -30);
  // declareMonitoredVariable("EF_L1AClusterTime_max_10", max_L1AClusterTime_10, -30);
  // declareMonitoredVariable("EF_L1AClusterTime_closest", closest_L1AClusterTime, -30);
  // declareMonitoredVariable("EF_L1AClusterTime_accept", accept, 0);

  // declareMonitoredStdContainer("EF_clustertime_10", m_clustertime_10, AutoClear);
  // declareMonitoredStdContainer("EF_rpctime_15", m_rpctime_15, AutoClear);

  // declareMonitoredVariable("EF_clustertime_10_sigma", m_clustertime_10_sigma, 0);

  // declareMonitoredStdContainer("EF_L1ACluster_eta", L1ACluster_eta, AutoClear);
  // declareMonitoredStdContainer("EF_L1ACluster_phi", L1ACluster_phi, AutoClear);
  // declareMonitoredStdContainer("EF_L1ACluster_t", L1ACluster_t, AutoClear);


 
  ////////TriggerTowers/////////
  //  declareMonitoredVariable("EF_TT_median",TT_median,-50);
  declareMonitoredVariable("EF_TT_mean",TT_mean,-50);
  declareMonitoredVariable("EF_TT_sigma",TT_sigma,-1);
  declareMonitoredVariable("EF_TT_nEM",TT_nEM,0);
  declareMonitoredVariable("EF_TT_nHad",TT_nHad,0);
  declareMonitoredVariable("EF_TT_total",TT_total,0);
  declareMonitoredVariable("EF_TTAlg_accept",TTAlg_accept,0);
  //  declareMonitoredStdContainer("EF_deltaR",deltaR,AutoClear);

  declareMonitoredStdContainer("EF_TT_timings",TT_timings, AutoClear);
  //  declareMonitoredStdContainer("EF_TT_timings_noFCAL",TT_timings_noFCAL, AutoClear);
  //  declareMonitoredStdContainer("EF_TT_timings_RoI",TT_timings_RoI, AutoClear);

  //  declareMonitoredVariable("EF_TT_median_noFCAL",TT_median_noFCAL,-50);
  // declareMonitoredVariable("EF_TT_mean_noFCAL",TT_mean_noFCAL,-50);
  // declareMonitoredVariable("EF_TT_sigma_noFCAL",TT_sigma_noFCAL,-1);
  // declareMonitoredVariable("EF_TT_median_RoI",TT_median_RoI,-50);
  // declareMonitoredVariable("EF_TT_mean_RoI",TT_mean_RoI,-50);
  // declareMonitoredVariable("EF_TT_sigma_RoI",TT_sigma_RoI,-1);
  /////////////////////////////
  
}


HLT::ErrorCode DetectorTimingAlgo::hltInitialize()
{
  ServiceHandle<IJobOptionsSvc> jobOptSvc("JobOptionsSvc", name());
  if (m_configSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve " << m_configSvc);
    return HLT::BAD_JOB_SETUP;
  }
  // if ( m_backExtrapolatorTool.retrieve().isFailure() ) {
  //   msg() << MSG::ERROR << "Could not retrieve " << m_backExtrapolatorTool << endreq;
  //   return HLT::BAD_JOB_SETUP;
  // }

  // if (m_dataPreparator.retrieve().isFailure()) {
  //   msg() << "Cannot retrieve Tool DataPreparator" << endreq;
  //   return HLT::BAD_JOB_SETUP;
  // }
  
  // m_dataPreparator->setExtrapolatorTool(&m_backExtrapolatorTool);
  
  // // set road width in case TGC/RPC readout failure
  // m_dataPreparator->setRoadWidthForFailure(400, 200);
  
  // //StatusCode sc;
  
  // m_dataPreparator->setRpcGeometry(1);
  // m_dataPreparator->setMdtDataCollection(1);
  // m_dataPreparator->setRoIBasedDataAccess(1,1,1,1);
  
  // // set data or MC flag
  // StatusCode sc = m_dataPreparator->setMCFlag(0);
  // if (!sc.isSuccess()) {
  //   msg() << MSG::ERROR << "Failed to set MC flag to DataPreparator" << endreq;
  //   return HLT::ERROR;
  // }

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
  m_roiDescriptor = 0;

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
  
  if(tes_in.size()>0){

    // Retrieve ROIS

    m_roiDescriptor=0;
    m_roiDescriptorVector.clear();

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
    
    // m_roiDescriptor=0;
    // m_roiDescriptorVector.clear();
    // // retrieve muon descriptor if its there
    // // [4] Muon ROI :
    // if (tes_in.size()>4) {
    //   for (HLT::TEVec::const_iterator it = tes_in[4].begin(); it != tes_in[4].end(); ++it) {

    // 	HLT::ErrorCode status = getFeature(  (*it) , m_muonColl );

    // 	//	HLT::ErrorCode status = getFeatures(  (*it) , m_roiDescriptorVector );

    // 	// Feature Key: "MuonL2SAInfo"

    // 	if(status!=HLT::OK ||  !m_muonColl) {
    // 	}else{
	  
    // 	  for (xAOD::L2StandAloneMuonContainer::const_iterator it = m_muonColl->begin(); it != m_muonColl->end(); ++it ){
    // 	    std::cout<<"We got a muon! eta:"<<(*it)->eta() <<" phi:"<<(*it)->phi() << " pt:"<<(*it)->pt() <<std::endl;
    // 	    if ((*it)->pt() > 6){
    // 	      for(unsigned int ih=0; ih< ((*it)-> rpcHitTime()).size(); ih++ ){
    // 		m_rpctime_15.push_back(  ((*it)-> rpcHitTime())[ih]  );
    // 		std::cout<<"    - rpchit time:"<<  ((*it)-> rpcHitTime())[ih] <<std::endl;
    // 	      }
    // 	    }
    // 	  }
    // 	}
    //   }// the 4th input (Muons)
    
    // }// no 4th input TE
    // ---- muon retrieval
    
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
    for (HLT::TEVec::const_iterator it = tes_in[0].begin(); it != tes_in[0].end(); ++it) {
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
	  // if(fabs(eta)<3.1){
	  //   TT_timings_noFCAL.push_back(xv);
	  // }
	  // for(unsigned int ic=0; ic<m_l1roi_phi.size(); ic++ ){
	  //   float phi = (*eIt)->phi();
	  //   if(phi > 3.14159){
	  //     phi = phi - 2*3.14159;
	  //   }
	  //   float deta = fabs( eta - m_l1roi_eta[ic] );
	  //   float dphi = fabs( phi- m_l1roi_phi[ic] );
	  //   if (dphi > 3.14159){
	  //     dphi = fabs(dphi - 2*3.14159);
	  //   }
	  //   if( sqrt(deta*deta + dphi*dphi) < 0.2)
	  //     {
	  // 	TT_timings_RoI.push_back(xv);
	  // 	break;
	  //     }
	  //   else{
	  //     //deltaR.push_back(sqrt(deta*deta + dphi*dphi));
	  //   }
	  // }
				
	}
      }
    }// the 0st input (TT)
    
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
   //std::sort(TT_timings.begin(), TT_timings.end() );
   if(size==0){
     TT_median=-50;
     TT_mean=-50;
     TT_sigma=-1;
   }
   else{
     // if(size%2==0){
     //   TT_median = (TT_timings[size/2 -1]+TT_timings[size/2])/2;
     // }
     // else{
     //   TT_median = TT_timings[ ((size-1)/2) ];
     // }

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

   // size = TT_timings_noFCAL.size();
   // std::sort(TT_timings_noFCAL.begin(), TT_timings_noFCAL.end() );
   // if(size==0){
   //   TT_median_noFCAL=-50;
   //   TT_mean_noFCAL=-50;
   //   TT_sigma_noFCAL=-1;
   // }
   // else{
   //   if(size%2==0){
   //     TT_median_noFCAL = (TT_timings_noFCAL[size/2 -1]+TT_timings_noFCAL[size/2])/2;
   //   }
   //   else{
   //     TT_median_noFCAL = TT_timings_noFCAL[ ((size-1)/2) ];
   //   }

   //   float n = 0;
   //   float tot = 0, tot2 =0, adouble;
   //   for(int i=0; i<size; i++){
   //     adouble=TT_timings_noFCAL[i];
   //     tot += adouble; tot2 += adouble*adouble;
   //     ++n;
   //   }
   //   float n1 = 1./n;
   //   TT_mean_noFCAL = tot*n1;
   //   TT_sigma_noFCAL = sqrt(fabs(tot2*n1 -TT_mean*TT_mean));
   // }

   // size = TT_timings_RoI.size();
   // std::sort(TT_timings_RoI.begin(), TT_timings_RoI.end() );
   // if(size==0){
   //   TT_median_RoI=-50;
   //   TT_mean_RoI=-50;
   //   TT_sigma_RoI=-1;
   // }
   // else{
   //   if(size%2==0){
   //     TT_median_RoI = (TT_timings_RoI[size/2 -1]+TT_timings_RoI[size/2])/2;
   //   }
   //   else{
   //     TT_median_RoI = TT_timings_RoI[ ((size-1)/2) ];
   //   }

   //   float n = 0;
   //   float tot = 0, tot2 =0, adouble;
   //   for(int i=0; i<size; i++){
   //     adouble=TT_timings_RoI[i];
   //     tot += adouble; tot2 += adouble*adouble;
   //     ++n;
   //   }
   //   float n1 = 1./n;
   //   TT_mean_RoI = tot*n1;
   //   TT_sigma_RoI = sqrt(fabs(tot2*n1 -TT_mean*TT_mean));
   // }


   
   TTAlg_accept=0;
   if(TT_mean>22 && TT_nEM>1 && TT_sigma<4){
     TTAlg_accept=1;
     std::vector<HLT::TriggerElement*> empty_seed;
     HLT::TriggerElement* te = config()->getNavigation()->addNode(empty_seed, output);
     te->setActiveState(true);
   }
   afterExecMonitors().ignore();

   return HLT::OK;
}


