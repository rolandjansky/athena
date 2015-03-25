/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * todo fullscan
 * faster data prep
 */

#include "TrigTRTHTHCounter.h"
#include "GaudiKernel/IssueSeverity.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"


#include "InDetIdentifier/TRT_ID.h"
#include "Identifier/IdentifierHash.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h" 
#include "TrigInDetToolInterfaces/ITrigTRT_DriftCircleProvider.h"

//edm
#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "TMath.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include<cmath>
#include<fstream>

 

const double PI = TMath::Pi();
const double TWOPI = 2.0*PI;

//
double hth_delta_phi(const float& phi1, const float& phi2)
{
  float PHI=fabs(phi1-phi2);
  return (PHI<=PI)? PHI : TWOPI-PHI;
}

bool hth_eta_match(const float& caleta, const float& trteta, const float& etaWindow){
  bool r=false;
  if (fabs(caleta)<etaWindow || caleta*trteta>0.){
    r = true;
  }
  return r;
}

float dist2COR(float R, float phi1, float phi2)
{
  float PHI=fabs(phi1-phi2);
  return fabs(R*sin(PHI));
}

struct TRT_hit {
  float phi;
  float R;
  bool isHT;
};

TRT_hit make_hit(float phi, float R, bool isHT){
  TRT_hit my_hit={phi,R,isHT};
  return my_hit;
}


//---------------------------------------------------------------------------------

TrigTRTHTHCounter::TrigTRTHTHCounter(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator),
    m_detStore("DetectorStore", name),
    m_storeGate("StoreGateSvc", name),
    m_trtHelper(0),
    m_rawDataTool("TrigTRT_DriftCircleProviderTool"),
    m_phiHalfWidth(0.1),
    m_etaHalfWidth(0.1),
    m_doFullScan(false),
    m_maxCaloEta(2.),
    m_roadWidth(4.),
    m_nBinCoarse(10.),
    m_nBinFine(10.),
    m_wedgeMinEta(0.)
{
  
  declareProperty( "TRT_DC_ContainerName",   m_trtDCContainerName = "Trig_OfflineDriftCircles" );
  declareProperty( "TrtDataProviderTool",    m_rawDataTool, "TrigTRT_DriftCircleProviderTool");
  declareProperty("EtaHalfWidth",            m_etaHalfWidth);
  declareProperty("PhiHalfWidth",            m_phiHalfWidth);
  declareProperty("doFullScan",              m_doFullScan);
  declareProperty("RoadWidth",              m_roadWidth);
  declareProperty("nBinCoarse",              m_nBinCoarse);
  declareProperty("nBinFine",              m_nBinFine);
  declareProperty("WedgeMinEta",              m_wedgeMinEta);

}

//---------------------------------------------------------------------------------

TrigTRTHTHCounter::~TrigTRTHTHCounter() {
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTRTHTHCounter::hltInitialize() {
  ATH_MSG_DEBUG ( "Initialising this TrigTRTHTHCounter: " << name());
  

  // Get storegate svc
  if(m_detStore.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Failed to connect to " << m_detStore.typeAndName());
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO ( "Retrieved service " << m_detStore.typeAndName());

  if(m_storeGate.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Failed to connect to " << m_storeGate.typeAndName());
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO ( "Retrieved service " << m_storeGate.typeAndName());

  // Get a TRT identifier helper
  if( m_detStore->retrieve(m_trtHelper, "TRT_ID").isFailure()) {
    ATH_MSG_ERROR ( "Failed to retrieve " << m_trtHelper); // fatal?
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO ( "Retrieved service " << m_trtHelper);
  

  // Get TrigTRT_DriftCircleProviderTool
  if( m_rawDataTool.retrieve().isFailure() ){
    ATH_MSG_FATAL ( "Failed to retrieve " << m_rawDataTool);
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO ( "Retrieved service " << m_rawDataTool);
  

  ATH_MSG_INFO ( " TrigTRTHTHCounter initialized successfully"); 

  //

  m_trththits.assign(2,0.);



  return HLT::OK;  
}

//---------------------------------------------------------------------------------------------------------------------------------------------
HLT::ErrorCode TrigTRTHTHCounter::hltBeginRun() {
  ATH_MSG_DEBUG ( "beginning run in this " << name());

  ATH_MSG_INFO ( "Pixel_TrgClusterization::hltBeginRun() ");
  if (!m_doFullScan){
    ATH_MSG_INFO ( "PhiHalfWidth: " << m_phiHalfWidth << " EtaHalfWidth: "<< m_etaHalfWidth);
  } else {
    ATH_MSG_INFO ( "FullScan mode");
  }

  return HLT::OK;
}
//---------------------------------------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigTRTHTHCounter::hltExecute(const HLT::TriggerElement* inputTE,
					     HLT::TriggerElement* outputTE) {


  ATH_MSG_DEBUG ( "Executing TrigTRTHTHCounter " << name());

  //
  m_listOfTrtIds.clear();

  for (size_t iw=0; iw<2; iw++){
    m_trththits[iw] = 0.;
  }

//count_httrt and count_tottrt are vectors of size 20, since we have 20 bins of 0.01 around the roi phi. At each bin (vector position) we look at the number of high threshold as well as the total number of TRT hits, which get stored in count_httrt and count_tottrt respectively. These values will be eventually transferred into the trththits arrays. 
  std::vector<int> count_httrt_c(m_nBinCoarse);
  std::vector<int> count_tottrt_c(m_nBinCoarse);
  
 
  std::vector<int> count_httrt(m_nBinFine);
  std::vector<int> count_tottrt(m_nBinFine);


  std::vector<TRT_hit> hit;
  int pos=0, result=0;
  unsigned int dist=0;

  const TrigRoiDescriptor* roi = 0;
  HLT::ErrorCode stat = getFeature( inputTE, roi, "initialRoI");
  if (stat!=HLT::OK || roi==0){
    return HLT::NAV_ERROR;
  }

// Adding xAOD information 
  ATH_MSG_VERBOSE ( "Attempting to get xAOD::RNNOutput");

  xAOD::TrigRNNOutput *rnnOutput = new xAOD::TrigRNNOutput();
  ATH_MSG_VERBOSE ( "Successfully got xAOD::RNNOutput ");
  rnnOutput->makePrivateStore();

  ATH_MSG_VERBOSE ( "Got makePrivateStore " << name());


  if ( fabs(roi->eta())<=m_maxCaloEta ){



//     out->setCluster(*(el_cluster.getStorableObjectPointer()), (unsigned int) el_cluster.index());

    //

    /*
    if (fabs(roi->phiPlus()-roi->phiMinus()-2.*m_phiHalfWidth)>0.01 ||
	fabs(roi->etaPlus()-roi->etaMinus()-2.*m_etaHalfWidth)>0.01){
      msg() << MSG::ERROR << "RoI size not consistent with configuration" << *roi <<  endreq;
      return HLT::NAV_ERROR;
    }
    */

    float phi=roi->phi();
    float phimin=roi->phi()-m_phiHalfWidth;
    float phimax=roi->phi()+m_phiHalfWidth;
    if (phi<-PI) phi+=2.*PI;
    if (phi>PI) phi-=2.*PI;

    TrigRoiDescriptor tmproi(roi->eta(), roi->eta()-m_etaHalfWidth, roi->eta()+m_etaHalfWidth,
			     phi, phimin, phimax);

 
			     //    StatusCode sc_fc = m_rawDataTool->fillCollections(*roi);
    StatusCode sc_fc = m_rawDataTool->fillCollections(tmproi);

    if (sc_fc.isRecoverable()){
    }
    else if (sc_fc.isFailure()){
      ATH_MSG_WARNING ( "Failed to prepare TRTDriftCircle collection");
      //return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::CORRUPTED_ROD);
    }
  



    const InDet::TRT_DriftCircleContainer* driftCircleContainer = nullptr;
    StatusCode sc_sg = evtStore()->retrieve( driftCircleContainer, m_trtDCContainerName) ;
    if( sc_sg.isFailure() ){
      ATH_MSG_ERROR ( " Failed to retrieve trt data from SG. "); 
      return HLT::TOOL_FAILURE;
    }

    else {
      ATH_MSG_VERBOSE ( " Successfully retrieved trt data from SG. "); 
    }    


  
    InDet::TRT_DriftCircleContainer::const_iterator trtdriftContainerItr  = driftCircleContainer->begin();
    InDet::TRT_DriftCircleContainer::const_iterator trtdriftContainerItrE = driftCircleContainer->end();



    for (; trtdriftContainerItr != trtdriftContainerItrE; ++trtdriftContainerItr) {
    
      InDet::TRT_DriftCircleCollection::const_iterator trtItr = (*trtdriftContainerItr)->begin();
      InDet::TRT_DriftCircleCollection::const_iterator trtEnd = (*trtdriftContainerItr)->end();
    
      for(; trtItr!=trtEnd; trtItr++){
      
        // find out which detector element the hit belongs to
        const InDetDD::TRT_BaseElement *det = (*trtItr)->detectorElement();
        // InDetDD::TRT_BaseElement::Type type = det->type();
      
        Identifier ID = (*trtItr)->identify();
        // IdentifierHash iH = m_trtHelper->straw_layer_hash(m_trtHelper->layer_id(ID));

        const Amg::Vector3D& strawcenter = det->strawCenter(m_trtHelper->straw(ID));

        bool hth = false;
	float hphi = strawcenter.phi();
	float heta = - TMath::Log(strawcenter.theta()/2.);
	float R = strawcenter.perp();
	

        if ((*trtItr)->highLevel()){
          hth = true;
        }

        if(hth_eta_match(roi->eta(), heta, m_etaHalfWidth))
          hit.push_back(make_hit(hphi,R,hth));

        int countbin=0;	
        if(hth_delta_phi(hphi, roi->phi()) < 0.1)
	  {
          for(float roibincenter=roi->phi()-m_phiHalfWidth+m_phiHalfWidth/m_nBinCoarse; roibincenter < roi->phi()+m_phiHalfWidth; roibincenter+=2*m_phiHalfWidth/m_nBinCoarse)
            {	
            if (hth_delta_phi(hphi,roibincenter)<=m_phiHalfWidth/m_nBinCoarse && hth_eta_match(roi->eta(), heta, m_etaHalfWidth))
	      {
                if(hth){
                  count_httrt_c.at(countbin) = count_httrt_c.at(countbin)+1.;				
                } 
                count_tottrt_c.at(countbin) = count_tottrt_c.at(countbin)+1.;
                break;							
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
      }
    }
  }
  for (size_t iw=0; iw< count_httrt_c.size(); iw++)
    {
    if (iw == 0) 
      {	
      result = count_httrt_c[iw];
      dist = pos;
      } 
    if(iw != 0 and result <= count_httrt_c[iw]) { result = count_httrt_c[iw]; dist = pos; }
      pos = pos + 1; 
    }  

  float center_pos_phi=roi->phi()+(2*(int)dist+1-m_nBinCoarse)*m_phiHalfWidth/m_nBinCoarse;


  for(size_t v=0;v<hit.size();v++){
      
    int countbin=0;	
    if(hth_delta_phi(hit[v].phi, center_pos_phi) < 0.01)
      {
      for(float roibincenter=center_pos_phi-m_phiHalfWidth/m_nBinCoarse+((m_phiHalfWidth/m_nBinCoarse)/m_nBinFine); roibincenter < (center_pos_phi+m_phiHalfWidth/m_nBinCoarse); roibincenter+=2*(m_phiHalfWidth/m_nBinCoarse)/m_nBinFine)
        {	
        if (hth_delta_phi(hit[v].phi,roibincenter)<=(m_phiHalfWidth/m_nBinCoarse)/m_nBinFine)
          {
          if(hit[v].isHT){
            count_httrt.at(countbin) = count_httrt.at(countbin)+1.;
          } 
          count_tottrt.at(countbin) = count_tottrt.at(countbin)+1.;
          break;							
          }
          countbin++;
        }
      }    
    }


  pos=result=dist=0;

  for (size_t iw=0; iw< count_httrt.size(); iw++){
    if (iw == 0) {
      result = count_httrt[iw];
      dist = pos;
    } 
    if(iw != 0 and result <= count_httrt[iw]) { result = count_httrt[iw]; dist = pos; }
    pos = pos + 1; 
    }

  center_pos_phi+=(2*(int)dist+1-m_nBinFine)*(m_phiHalfWidth/m_nBinCoarse)/m_nBinFine;


  int trthit=0, trthit_ht=0;
  for(size_t v=0;v<hit.size();v++){
    if (dist2COR(hit[v].R,hit[v].phi,center_pos_phi)<=m_roadWidth){
      if(hit[v].isHT){
        trthit_ht+=1;
      } 
      trthit+=1;						
    }
  }

  if (trthit!=0)
  m_trththits[0] = trthit + (double)trthit_ht/trthit;

  trthit=trthit_ht=0;

  if (dist>0&&dist<count_httrt.size()-1){
    for (int k=0;k<3;k++){
      trthit+= count_tottrt[dist+k-1];
      trthit_ht+=count_httrt[dist+k-1];
    }
  }


  if (dist==0){
    for (int k=1;k<3;k++){
      trthit+= count_tottrt[dist+k-1];
      trthit_ht+=count_httrt[dist+k-1];
    }
  }

  if (dist==count_httrt.size()-1){
    for (int k=0;k<2;k++){
      trthit+= count_tottrt[dist+k-1];
      trthit_ht+=count_httrt[dist+k-1];
    }
  }

  if (trthit!=0&&(fabs(roi->eta())>=m_wedgeMinEta))
    m_trththits[1] = trthit + (double)trthit_ht/trthit;



  ATH_MSG_VERBOSE ( "trththits with road algorithm : " << m_trththits[0]);
  ATH_MSG_VERBOSE ( "trththits with wedge algorithm : " << m_trththits[1]);
  
  //Writing to xAOD
  rnnOutput->auxdata< std::vector<float>  >("trththits") = m_trththits;// decoration for now.
  if( msg().level() <= MSG::DEBUG){
    static SG::AuxElement::Accessor< std::vector<float> >orig("trththits");
    if( !orig.isAvailable(*rnnOutput)  ){
      ATH_MSG_WARNING ( "Problem with decorator.");
    }
  }  


  std::string key="";
  std::string label="TrigTRTHTCounts";

  //Write and attach for xAOD
  HLT::ErrorCode hitStatus = recordAndAttachFeature<xAOD::TrigRNNOutput>(outputTE, rnnOutput, key, label) ;


   if (hitStatus != HLT::OK)
   {
    ATH_MSG_ERROR ( "Writing to xAODs failed");
    return HLT::NAV_ERROR;
  }
  return HLT::OK;
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTRTHTHCounter::hltFinalize() {
  ATH_MSG_DEBUG ( " finalizing TrigTRTHTHCounter : "<< name()); 
  return HLT::OK;  
}


