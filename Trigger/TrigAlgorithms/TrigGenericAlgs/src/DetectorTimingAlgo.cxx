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

    // m_robDataProviderSvc("ROBDataProviderSvc", name),
    // m_trigMuonRoITool( "TrigMuonRoITool" ),
    // m_recRPCRoiSvc("LVL1RPC::RPCRecRoiSvc",""),
    // m_recTGCRoiSvc("LVL1TGC::TGCRecRoiSvc",""),
{
  m_caloCluster=0;
  // declareProperty("noMuon",   m_nomuon = false, "no muon in the list of items to be correlated");
  // declareProperty("m_l1itemlist",   m_l1itemlist = {"L1_EM22VHI","L1_J400","L1_MU20"}, "list of l1 items to be used");
  
  // declareMonitoredVariable("EF_ootfrac_bc", m_ootfrac_bc, 0);
  declareMonitoredVariable("EF_ootfrac_bc_10", m_ootfrac_bc_10, 0);
  // declareMonitoredVariable("EF_ootfrac_bc_25", m_ootfrac_bc_25, 0);
  // declareMonitoredVariable("EF_ootfrac_bc_50", m_ootfrac_bc_50, 0);

  // declareMonitoredVariable("EF_ootfrac", m_ootfrac, 0);
  // declareMonitoredVariable("EF_itfrac", m_itfrac, 0);

  declareMonitoredVariable("EF_L1AClusterTime_weighted", weighted_L1AClusterTime, 0);

  declareMonitoredVariable("EF_L1AClusterTime_max_2", max_L1AClusterTime_2, -30);
  declareMonitoredVariable("EF_L1AClusterTime_max_10", max_L1AClusterTime_10, -30);

  declareMonitoredVariable("EF_L1AClusterTime_closest", closest_L1AClusterTime, -30);

  declareMonitoredVariable("EF_L1AClusterTime_accept", accept, 0);

  //  declareMonitoredStdContainer("EF_clustertime_5", m_clustertime_5, AutoClear);
  declareMonitoredStdContainer("EF_clustertime_10", m_clustertime_10, AutoClear);
  // declareMonitoredStdContainer("EF_clustertime_25", m_clustertime_25, AutoClear);
  // declareMonitoredStdContainer("EF_clustertime_50", m_clustertime_50, AutoClear);

  declareMonitoredStdContainer("EF_L1ACluster_eta", L1ACluster_eta, AutoClear);
  declareMonitoredStdContainer("EF_L1ACluster_phi", L1ACluster_phi, AutoClear);
  declareMonitoredStdContainer("EF_L1ACluster_t", L1ACluster_t, AutoClear);


  // declareMonitoredStdContainer("EF_L1ACluster_eta_funnypeak", L1ACluster_eta_funnypeak, AutoClear);
  // declareMonitoredStdContainer("EF_L1ACluster_phi_funnypeak", L1ACluster_phi_funnypeak, AutoClear);
  // declareMonitoredStdContainer("EF_L1ACluster_t_funnypeak", L1ACluster_t_funnypeak, AutoClear);


  
}


HLT::ErrorCode DetectorTimingAlgo::hltInitialize()
{
  ServiceHandle<IJobOptionsSvc> jobOptSvc("JobOptionsSvc", name());
  if (m_configSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve " << m_configSvc);
    return HLT::BAD_JOB_SETUP;
  }
  


   return HLT::OK;
}


void DetectorTimingAlgo::updateHandler(Property&)
{
  //  parseROBRequest();
}



/*

*/
HLT::ErrorCode DetectorTimingAlgo::hltExecute(std::vector<HLT::TEVec>& tes_in, unsigned int output)
{
  m_caloCluster=0;
  m_roiDescriptor = 0;

  beforeExecMonitors().ignore();

  m_l1roi_eta.clear();
  m_l1roi_phi.clear();
    


  int nclus_ootBC_10=0;
  int nclus_10=0;

  //  int nclus_5=0;
  //  int nclus_ootBC_5=0;
  // int nclus_ootBC_25=0;
  // int nclus_ootBC_50=0;

  // int nclus_oot_5=0;
  // int nclus_it_5=0;
  

  // int nclus_25=0;
  // int nclus_50=0;

  summed_L1AClusterE=0;
  weighted_L1AClusterTime=0;

  max_L1AClusterTime_2=-30;
  max_L1AClusterTime_10=-30;

  closest_L1AClusterTime=0;
  accept=0;
  
  L1ACluster_E.clear();
  L1ACluster_eta.clear();
  L1ACluster_phi.clear();
  L1ACluster_t.clear();

  L1ACluster_eta_funnypeak.clear();
  L1ACluster_phi_funnypeak.clear();
  L1ACluster_t_funnypeak.clear();

  //  std::cout<<" ------ start event --------- " << std::endl;
  
  if(tes_in.size()>0){

    // Retrieve ROIS

    m_roiDescriptor=0;
    m_roiDescriptorVector.clear();

    for (HLT::TEVec::const_iterator it = tes_in[1].begin(); it != tes_in[1].end(); ++it) {

      HLT::ErrorCode status = getFeatures(  (*it) , m_roiDescriptorVector );

      if(status!=HLT::OK) {
      }else{
	for( unsigned int iroi=0; iroi<m_roiDescriptorVector.size(); iroi++){

	  m_roiDescriptor=m_roiDescriptorVector[iroi];
	  //std::cout<<"!!!!This TE has A EM roidescriptor!! ETA="<<m_roiDescriptor->eta()<< " Phi="<< m_roiDescriptor->phi() << std::endl;
	  m_l1roi_eta.push_back(m_roiDescriptor->eta());
	  m_l1roi_phi.push_back(m_roiDescriptor->phi());
	}
      }
            
    }

    m_roiDescriptor=0;
    m_roiDescriptorVector.clear();
    for (HLT::TEVec::const_iterator it = tes_in[2].begin(); it != tes_in[2].end(); ++it) {
      HLT::ErrorCode status = getFeatures(  (*it) , m_roiDescriptorVector );
      if(status!=HLT::OK) {
      }else{
	for( unsigned int iroi=0; iroi<m_roiDescriptorVector.size(); iroi++){
	  m_roiDescriptor=m_roiDescriptorVector[iroi];
	  //std::cout<<"!!!!This TE has A Jet roidescriptor!! ETA="<<m_roiDescriptor->eta()<< " Phi="<< m_roiDescriptor->phi() << std::endl;
	  m_l1roi_eta.push_back(m_roiDescriptor->eta());
	  m_l1roi_phi.push_back(m_roiDescriptor->phi());
	}
      }
      
    }
    
    // loop over caloclusters
    for (HLT::TEVec::const_iterator it = tes_in[0].begin(); it != tes_in[0].end(); ++it) {

      HLT::ErrorCode status = getFeature(  (*it) , m_caloCluster );
      
      if(status!=HLT::OK || !m_caloCluster) {
	//std::cout<<"This TE has no caloclusters"<< std::endl;
      } else {     
	
	  for (xAOD::CaloClusterContainer::const_iterator it = m_caloCluster->begin(); it != m_caloCluster->end(); ++it ){

	    // Loop over l1 rois and perform a matching
	    bool L1RoiMatch=false;
	    for(unsigned int ic=0; ic<m_l1roi_phi.size(); ic++ ){
	      if( ((*it)->eta()- m_l1roi_eta[ic])*((*it)->eta()- m_l1roi_eta[ic]) + ((*it)->phi()- m_l1roi_phi[ic])*((*it)->phi()- m_l1roi_phi[ic]) < 0.04){
		L1RoiMatch=true;
		break;
	      }
	    }
	    
	    if(L1RoiMatch ){
	      //std::cout << "Matched cluster E, eta, phi, time: " << (*it)->e()<<", "<< (*it)->eta()<<", "<< (*it)->phi()<<", "<< (*it)->time() << std::endl;	      
	      // Record some properties:
	      L1ACluster_E.push_back((*it)->e());
	      L1ACluster_t.push_back((*it)->time());
	      L1ACluster_eta.push_back((*it)->eta());
	      L1ACluster_phi.push_back((*it)->phi());
	      summed_L1AClusterE+=(*it)->e();

	      if( (*it)->e()>10000 && (*it)->time() > max_L1AClusterTime_10 )
		max_L1AClusterTime_10 = (*it)->time();
	      if( (*it)->e()>2000 && (*it)->time() > max_L1AClusterTime_2 )
		max_L1AClusterTime_2 = (*it)->time();
	      
	      // if( (*it)->time() > 6 &&  (*it)->time() <16 ){
	      // 	L1ACluster_t_funnypeak.push_back((*it)->time());
	      // 	L1ACluster_eta_funnypeak.push_back((*it)->eta());
	      // 	L1ACluster_phi_funnypeak.push_back((*it)->phi());

	      // }
	    }
	    
	    
	    // if ( (*it)->e()/1000. > 5){
	    //   nclus_5++;
	    //   m_clustertime_5.push_back((*it)->time());

	    //   if( fabs( (*it)->time()) <5 ){
	    // 	nclus_it_5++;
	    //   }
	    //   else{
	    // 	nclus_oot_5++;
	    // 	if( fabs( (*it)->time()) < 27 &&  fabs( (*it)->time()) > 18   )
	    // 	  nclus_ootBC_5++;
	    //   }

		
	    // }
	    if ( (*it)->e()/1000. > 10){
	      nclus_10++;
	      m_clustertime_10.push_back((*it)->time());
	      if( fabs( (*it)->time()) < 27 &&  fabs( (*it)->time()) > 18   )
		nclus_ootBC_10++;

	    } 
	    // if ( (*it)->e()/1000. > 25){
	    //   nclus_25++;
	    //   m_clustertime_25.push_back((*it)->time());
	    //   if( fabs( (*it)->time()) < 27 &&  fabs( (*it)->time()) > 18   )
	    // 	nclus_ootBC_25++;

	    // }
	    // if ( (*it)->e()/1000. > 50){
	    //   nclus_50++;
	    //   m_clustertime_50.push_back((*it)->time());
	    //   if( fabs( (*it)->time()) < 27 &&  fabs( (*it)->time()) > 18   )
	    // 	nclus_ootBC_50++;
	    // } 

	    
	    //std::cout << " Cluster E, eta, phi, time: " << (*it)->e()<<", "<< (*it)->eta()<<", "<< (*it)->phi()<<", "<< (*it)->time() << std::endl;
	    //msg() << MSG::INFO << " Cluster E, eta, phi, time: " << (*it)->e()<<", "<< (*it)->eta()<<", "<< (*it)->phi()<<", "<< (*it)->time() << endreq;   
          }
      }
      
    } // loop over TE
  }else{
    //std::cout << " input TE size =0" <<std::endl;
  }


  //  m_ootfrac_bc = (float)(nclus_ootBC_5) / nclus_5;
  m_ootfrac_bc_10 = (float)(nclus_ootBC_10) / nclus_10;
  //  m_ootfrac_bc_25 = (float)(nclus_ootBC_25) / nclus_25;
  // m_ootfrac_bc_50 = (float)(nclus_ootBC_50) / nclus_50;

  // m_ootfrac = (float)(nclus_oot_5) / nclus_5;
  // m_itfrac = (float)(nclus_it_5)/ nclus_5;
  

  
  for (unsigned int ic=0; ic< L1ACluster_E.size(); ic++){
    weighted_L1AClusterTime +=  L1ACluster_t[ic] * L1ACluster_E[ic]/summed_L1AClusterE;    
  }

  std::vector<HLT::TriggerElement*> empty_seed;
  HLT::TriggerElement* te = config()->getNavigation()->addNode(empty_seed, output);
 

  // if( weighted_L1AClusterTime > 19 || max_L1AClusterTime_10  > 19){
  //   accept=1;
  //   te->setActiveState(true);
  // }

   if( weighted_L1AClusterTime > 19 || max_L1AClusterTime_10  > 21 || max_L1AClusterTime_10  < -29 ){
    accept=1;
    te->setActiveState(true);
  }

   //  if( weighted_L1AClusterTime < 19 || max_L1AClusterTime_10  < 19 ){
    //    std::cout<<" ------ this event has average matched cluster time < 19 ns --------- " << std::endl;
    // std::cout<<" avg. weighted time: " << weighted_L1AClusterTime << " max. time 10: "<<max_L1AClusterTime_10<<"  max. time 2: "<<max_L1AClusterTime_2  <<std::endl;
   //  }
  //  std::cout<<" ------ end event --------- " << std::endl;

  afterExecMonitors().ignore();

  return HLT::OK;
}


