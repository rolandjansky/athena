/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTRTHTHCounterMT.h"
#include "AthenaMonitoringKernel/Monitored.h"

//Function to calculate distance for road algorithm
float dist2COR_MT(float R, float phi1, float phi2){
  float PHI= std::abs(phi1-phi2);
  return std::abs(R*sin(PHI));
}

//TRT hit struct used for convenience
struct TRT_hit_MT {
  float phi;
  float R;
  bool isHT;
};

//Constructor of above struct
TRT_hit_MT make_hit_MT(float phi, float R, bool isHT){
  TRT_hit_MT my_hit_MT={phi,R,isHT};
  return my_hit_MT;
}


//---------------------------------------------------------------------------------


TrigTRTHTHCounterMT::TrigTRTHTHCounterMT(const std::string & name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
      m_trtHelper(0),
      m_maxCaloEta(1.7)
{}

StatusCode TrigTRTHTHCounterMT::initialize()
{
  ATH_MSG_DEBUG ( "Initialising this TrigTRTHTHCounter: " << name());

  // Get a TRT identifier helper
  if( detStore()->retrieve(m_trtHelper, "TRT_ID").isFailure()) {
    ATH_MSG_ERROR ( "Failed to retrieve " << m_trtHelper); // fatal?
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO ( "Retrieved service " << m_trtHelper);

  if (!m_doFullScan){
    ATH_MSG_INFO ( "PhiHalfWidth: " << m_phiHalfWidth << " EtaHalfWidth: "<< m_etaHalfWidth);
  } else {
    ATH_MSG_INFO ( "FullScan mode");
  }

  ATH_MSG_INFO ( " TrigTRTHTHCounter initialized successfully");

  ATH_CHECK( m_roiCollectionKey.initialize() );
  ATH_CHECK( m_trtDCContainerKey.initialize() );
  ATH_CHECK( m_trigRNNOutputKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode TrigTRTHTHCounterMT::execute() {
 using namespace xAOD;   

 ATH_MSG_DEBUG( "Executing " <<name());
 auto ctx = getContext();
 
 auto trigRNNOutputColl = SG::makeHandle (m_trigRNNOutputKey, ctx);  
 ATH_CHECK(trigRNNOutputColl.record (std::make_unique<xAOD::TrigRNNOutputContainer>(),
                                      std::make_unique<xAOD::TrigRNNOutputAuxContainer>()));
  
 ATH_MSG_DEBUG( "Made WriteHandle " << m_trigRNNOutputKey );  

 auto roiCollection = SG::makeHandle(m_roiCollectionKey, ctx);
 ATH_MSG_DEBUG( "Made handle " << m_roiCollectionKey  );

 if (roiCollection->size()==0) {
    ATH_MSG_DEBUG(" RoI collection size = 0");
    return StatusCode::SUCCESS;
 }

 const TrigRoiDescriptor* roiDescriptor = *(roiCollection->begin());

 ATH_MSG_DEBUG(" RoI ID = "   << (roiDescriptor)->roiId()
		<< ": Eta = "      << (roiDescriptor)->eta()
		<< ", Phi = "      << (roiDescriptor)->phi());

    
 m_listOfTrtIds.clear();

 float hitInit[6]={0,-999,0,-999,-999,-999};
 m_trththits.clear();
 for (int i=0; i<6; i++) {
    m_trththits.push_back(hitInit[i]);
 }

 //Vectors to hold the count of total and HT TRT hits in the coarse bins 
 std::vector<int> count_httrt_c(m_nBinCoarse);
 std::vector<int> count_tottrt_c(m_nBinCoarse);
  
 //Vectors to hold the count of total and HT TRT hits in the fine bins
 std::vector<int> count_httrt(3*m_nBinFine);
 std::vector<int> count_tottrt(3*m_nBinFine);

 //Vector to hold TRT hits that are within RoI
 std::vector<TRT_hit_MT> hit;

 //Sanity check of the ROI size
 double deltaEta= std::abs(roiDescriptor->etaPlus()-roiDescriptor->etaMinus());
 double deltaPhi=CxxUtils::deltaPhi(roiDescriptor->phiPlus(),roiDescriptor->phiMinus());
 float phiTolerance = 0.001;
 float etaTolerance = 0.001;

 if((m_etaHalfWidth - deltaEta/2.) > etaTolerance)
  ATH_MSG_WARNING ( "ROI eta range too small : " << deltaEta);

 if((m_phiHalfWidth - deltaPhi/2.) > phiTolerance)
  ATH_MSG_WARNING ( "ROI phi range too small : " << deltaPhi);

 float coarseWedgeHalfWidth = m_phiHalfWidth/m_nBinCoarse;
 float fineWedgeHalfWidth = coarseWedgeHalfWidth/m_nBinFine;

 //Code will only proceed if the RoI eta is not too large; used to limit rate from endcap
 if ( std::abs(roiDescriptor->eta())<=m_maxCaloEta ){

  SG::ReadHandle<InDet::TRT_DriftCircleContainer> trtDCC(m_trtDCContainerKey, ctx);
  ATH_MSG_DEBUG( "Made handle " << m_trtDCContainerKey );

  if (trtDCC->size() == 0){
      return StatusCode::SUCCESS; // Exit early if there are no tracks
  }

  InDet::TRT_DriftCircleContainer::const_iterator trtdriftContainerItr  = trtDCC->begin();
  InDet::TRT_DriftCircleContainer::const_iterator trtdriftContainerItrE = trtDCC->end();

  for (; trtdriftContainerItr != trtdriftContainerItrE; ++trtdriftContainerItr) {
    
    InDet::TRT_DriftCircleCollection::const_iterator trtItr = (*trtdriftContainerItr)->begin();
    InDet::TRT_DriftCircleCollection::const_iterator trtEnd = (*trtdriftContainerItr)->end();
      
    for(; trtItr!=trtEnd; trtItr++){
	
      // find out which detector element the hit belongs to
      const InDetDD::TRT_BaseElement *det = (*trtItr)->detectorElement();
      Identifier ID = (*trtItr)->identify();
      const Amg::Vector3D& strawcenter = det->strawCenter(m_trtHelper->straw(ID));

      bool hth = false;
      float hphi = strawcenter.phi();
      float heta = strawcenter.eta();
      float R = strawcenter.perp(); 

      if ((*trtItr)->highLevel()) hth = true;

      //hit needs to be stored
      hit.push_back(make_hit_MT(hphi,R,hth));

      //First, define coarse wedges in phi, and count the TRT hits in these wedges
      int countbin=0;	
      if(CxxUtils::deltaPhi(hphi, static_cast<float>(roiDescriptor->phi())) < 0.1){
        float startValue = roiDescriptor->phi() - m_phiHalfWidth + coarseWedgeHalfWidth;
	float endValue = roiDescriptor->phi() + m_phiHalfWidth;
	float increment = 2*coarseWedgeHalfWidth;
        for(float roibincenter = startValue; roibincenter < endValue; roibincenter += increment){
        if (CxxUtils::deltaPhi(hphi,roibincenter)<=coarseWedgeHalfWidth) {
	  if(hth) count_httrt_c.at(countbin) += 1.;
	  count_tottrt_c.at(countbin) += 1.;
	  break; //the hit has been assigned to one of the coarse wedges, so no need to continue the for loop							
	}
	countbin++;
       }
     }
     ATH_MSG_VERBOSE ( "timeOverThreshold=" << (*trtItr)->timeOverThreshold()
			<< "  highLevel=" << (*trtItr)->highLevel()
			<< " rawDriftTime=" << (*trtItr)->rawDriftTime()
			<< " barrel_ec=" << m_trtHelper->barrel_ec(ID)
			<< " phi_module=" << m_trtHelper->phi_module(ID)
			<< " layer_or_wheel=" << m_trtHelper->layer_or_wheel(ID)
			<< " straw_layer=" << m_trtHelper->straw_layer(ID)
			<< " straw=" << m_trtHelper->straw(ID)
			<< " scR=" << det->strawCenter(m_trtHelper->straw(ID)).perp()
			<< " scPhi=" << hphi
			<< " scEta=" << heta);		    
      } // end of loop over TRT drift circle collection
    } //end of loop over TRT drift circle container
  } //end of check to see if RoI eta is not too large

  //Now figure out which of the coarse bins in phi has the max number of HT TRT hits
  int maxHits = 0; //used to keep track of the max number of HT TRT hits in a coarse bin
  int dist = 0; //used to keep track of which coarse bin has the max number of HT TRT hits

  for (size_t iw = 0; iw < count_httrt_c.size(); iw++){
    if(maxHits <= count_httrt_c[iw]){ 
      maxHits = count_httrt_c[iw]; 
      dist = iw;  
    }
  }

  //Value of dist can range between 0 and (m_nBinCoarse-1)
  float center_pos_phi=roiDescriptor->phi()+(2*dist+1-m_nBinCoarse)*coarseWedgeHalfWidth;

  //Now, define fine wedges in phi, centered around the best coarse wedge, and count the TRT hits in these fine wedges
  for(size_t v=0;v<hit.size();v++){
    int countbin=0;	
    if(CxxUtils::deltaPhi(hit[v].phi, center_pos_phi) < 0.01){
      float startValue = center_pos_phi - 3*coarseWedgeHalfWidth + fineWedgeHalfWidth;
      float endValue = center_pos_phi + 3*coarseWedgeHalfWidth;
      float increment = 2*fineWedgeHalfWidth;
      for(float roibincenter = startValue; roibincenter < endValue; roibincenter += increment){	
        if (CxxUtils::deltaPhi(hit[v].phi,roibincenter)<=fineWedgeHalfWidth) {
          if(hit[v].isHT) count_httrt.at(countbin) += 1.;
          count_tottrt.at(countbin) += 1.;
          break; //the hit has been assigned to one of the fine wedges, so no need to continue the for loop							
	}
	countbin++;
      }
    }    
  }
  
  //Now figure out which of the fine bins in phi has the max number of HT TRT hits
  maxHits = 0; //used to keep track of the max number of HT TRT hits in a fine bin
  dist = 0; //used to keep track of which fine bin has the max number of HT TRT hits
  
  for (size_t iw = 0; iw < count_httrt.size(); iw++){
    if(maxHits <= count_httrt[iw]){ 
      maxHits = count_httrt[iw]; 
      dist = iw; 
    }
  }

  //Value of dist can range between 0 and (3*m_nBinFine-1)
  center_pos_phi+=(2*dist+1-3*m_nBinFine)*fineWedgeHalfWidth;

  //Count the number of total and HT TRT hits for the road algorithm
  int trthit=0, trthit_ht=0;
  for(size_t v=0;v<hit.size();v++){
    if (dist2COR_MT(hit[v].R,hit[v].phi,center_pos_phi)<=m_roadWidth){
      if(hit[v].isHT) trthit_ht+=1; 
      trthit+=1;						
    }
  }

  if (trthit!=0&&(std::abs(roiDescriptor->eta())<=m_roadMaxEta)){
    m_trththits[0] = trthit_ht;
    m_trththits[1] = (float)trthit_ht/trthit;
  }

  //Count the number of total and HT TRT hits for the wedge algorithm
  trthit = 0;
  trthit_ht = 0;

  for (int k=0;k<(2*m_wedgeNBin+1);k++){
    int binNumber = dist+k-m_wedgeNBin;
    //Even though we are supposed to use 2*m_wedgeNBin+1 fine bins, 
    //need to make sure that binNumber is sensible
    if(binNumber >= 0 && binNumber < (int)count_httrt.size()){
      trthit += count_tottrt.at(binNumber);
      trthit_ht += count_httrt.at(binNumber);
    }
  }

  if (trthit!=0&&(std::abs(roiDescriptor->eta())>=m_wedgeMinEta)){
    m_trththits[2] = trthit_ht;
    m_trththits[3] = (float)trthit_ht/trthit;
  }

  m_trththits[4]=roiDescriptor->eta();
  m_trththits[5]=roiDescriptor->phi();

  ATH_MSG_VERBOSE ( "trthits with road algorithm : " << m_trththits[0]);
  ATH_MSG_VERBOSE ( "fHT with road algorithm : " << m_trththits[1]);
  ATH_MSG_VERBOSE ( "trthits with wedge algorithm : " << m_trththits[2]);
  ATH_MSG_VERBOSE ( "fHT with wedge algorithm : " << m_trththits[3]);
  ATH_MSG_VERBOSE ( "ROI eta : " << m_trththits[4]);
  
  //Writing to xAOD
  xAOD::TrigRNNOutput* rnnOutput = new xAOD::TrigRNNOutput();
  rnnOutput->setRnnDecision(m_trththits);
  trigRNNOutputColl->push_back(rnnOutput);
  
  ATH_MSG_DEBUG("REGTEST:  returning an xAOD::TrigRNNOutputContainer with size "<< trigRNNOutputColl->size() << ".");

  return StatusCode::SUCCESS;
}

