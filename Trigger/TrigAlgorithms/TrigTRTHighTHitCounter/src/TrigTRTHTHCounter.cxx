/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * todo fullscan
 * faster data prep
 */

#include "TrigTRTHTHCounter.h"
#include "GaudiKernel/IssueSeverity.h"
//#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"


#include "InDetIdentifier/TRT_ID.h"
#include "Identifier/IdentifierHash.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h" 
#include "TrigInDetToolInterfaces/ITrigTRT_DriftCircleProvider.h"

//edm
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigRNNOutput.h"

#include "TMath.h"
#include "GeoPrimitives/GeoPrimitives.h"
 
const unsigned int TrigTRTHTHCounter::c_phibins=5;

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
    m_maxCaloEta(2.)
{
  
  declareProperty( "TRT_DC_ContainerName",   m_trtDCContainerName = "Trig_OfflineDriftCircles" );
  declareProperty( "TrtDataProviderTool",    m_rawDataTool, "TrigTRT_DriftCircleProviderTool");
  declareProperty("EtaHalfWidth",            m_etaHalfWidth);
  declareProperty("PhiHalfWidth",            m_phiHalfWidth);
  declareProperty("doFullScan",              m_doFullScan);

  for (size_t i=0; i<c_phibins; i++){
    m_phiWindows.push_back(m_phiHalfWidth*float(i+1)/float(c_phibins));
  }

}

//---------------------------------------------------------------------------------

TrigTRTHTHCounter::~TrigTRTHTHCounter() {
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTRTHTHCounter::hltInitialize() {
  msg() << MSG::DEBUG << "Initialising this TrigTRTHTHCounter: " << name() << endreq;
  

  // Get storegate svc
  if(m_detStore.retrieve().isFailure()) {
    msg() << MSG::FATAL << "Failed to connect to " << m_detStore.typeAndName() << endreq;
    return StatusCode::FAILURE;
  } else
    msg() << MSG::INFO << "Retrieved service " << m_detStore.typeAndName() << endreq;

  if(m_storeGate.retrieve().isFailure()) {
    msg() << MSG::FATAL << "Failed to connect to " << m_storeGate.typeAndName() << endreq;
    return StatusCode::FAILURE;
  } else
    msg() << MSG::INFO << "Retrieved service " << m_storeGate.typeAndName() << endreq;

  // Get a TRT identifier helper
  if( m_detStore->retrieve(m_trtHelper, "TRT_ID").isFailure()) {
    msg() << MSG::ERROR << "Failed to retrieve " << m_trtHelper << endreq; // fatal?
    return StatusCode::FAILURE;
  } else
    msg() << MSG::INFO << "Retrieved service " << m_trtHelper << endreq;
  

  // Get TrigTRT_DriftCircleProviderTool
  if( m_rawDataTool.retrieve().isFailure() ){
    msg() << MSG::FATAL << "Failed to retrieve " << m_rawDataTool << endreq;
    return StatusCode::FAILURE;
  } else
    msg() << MSG::INFO << "Retrieved service " << m_rawDataTool << endreq;
  

  msg() << MSG::INFO << " TrigTRTHTHCounter initialized successfully" << endreq; 

  //
  for (size_t iw=0; iw<c_phibins; iw++){
    m_trthits.push_back(0.);
    m_trththits.push_back(0.);
  }


  return HLT::OK;  
}

//---------------------------------------------------------------------------------------------------------------------------------------------
HLT::ErrorCode TrigTRTHTHCounter::hltBeginRun() {
  msg() << MSG::DEBUG << "beginning run in this " << name() << endreq;

  msg() << MSG::INFO << "Pixel_TrgClusterization::hltBeginRun() ";
  if (!m_doFullScan){
    msg() << MSG::INFO << "PhiHalfWidth: " << m_phiHalfWidth << " EtaHalfWidth: "<< m_etaHalfWidth;
  } else {
    msg() << MSG::INFO << "FullScan mode";
  }
  msg() << MSG::INFO << endreq;

  return HLT::OK;
}
//---------------------------------------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigTRTHTHCounter::hltExecute(const HLT::TriggerElement* inputTE,
					     HLT::TriggerElement* outputTE) {

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Executing TrigTRTHTHCounter " << name() << endreq;

  //
  m_listOfTrtIds.clear();

  for (size_t iw=0; iw<c_phibins; iw++){
    m_trthits[iw] = 0.;
    m_trththits[iw] = 0.;
  }
//count_httrt and count_tottrt are vectors of size 20, since we have 20 bins of 0.01 around the roi phi. At each bin (vector position) we look at the number of high threshold as well as the total number of TRT hits, which get stored in count_httrt and count_tottrt respectively. These values will be eventually transferred into the trththits arrays. 
  std::vector<int> count_httrt(20);
  std::vector<int> count_tottrt(20);
  count_httrt.assign(20, 0.);
  count_tottrt.assign(20, 0.);
  int pos;
  int result;
  int dist;
  pos = result = dist = 0;

  const TrigRoiDescriptor* roi = 0;
  HLT::ErrorCode stat = getFeature( inputTE, roi, "initialRoI");
  if (stat!=HLT::OK || roi==0){
    return HLT::NAV_ERROR;
  }

//   ElementLink<TrigEMClusterContainer> el_cluster;
//   if ( (getFeatureLink<TrigEMClusterContainer, TrigEMCluster>(outputTE, el_cluster) != HLT::OK) || (!el_cluster.isValid()) ) {
//     msg() << MSG::ERROR << "cannot get an EMCluster" << endreq; 
//     return HLT::NAV_ERROR;
//   }


  //    
  TrigRNNOutput *out = new TrigRNNOutput;
//   msg() << MSG::INFO << "ROI Eta: " << roi->eta0() << endreq;
//   msg() << MSG::INFO << "ROI Phi: " << roi->phi0() << endreq;
 
//   float caleta = (*el_cluster)->eta();
//   float calphi = (*el_cluster)->phi();
  //  float calet  = (*el_cluster)->et();
//   if ( msgLvl() <= MSG::DEBUG ) (*el_cluster)->print(msg());

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
      msg() << MSG::WARNING << "Failed to prepare TRTDriftCircle collection" << endreq;
      //return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::CORRUPTED_ROD);
    }
  
    std::vector<double> trtphi;
    std::vector<double> httrtphi;
    std::vector<std::pair<double, double> > trthtphieta;

    for (size_t iw=0; iw<c_phibins; iw++){
      m_trththits[iw] = m_trthits[iw] =0.; 
    }


    const InDet::TRT_DriftCircleContainer* driftCircleContainer;
    StatusCode sc_sg = m_storeGate->retrieve( driftCircleContainer, m_trtDCContainerName);

    if( sc_sg.isFailure() ){
      msg() << MSG::ERROR << " Failed to retrieve trt data from SG. " << endreq; 
      return HLT::TOOL_FAILURE;
    }
    else {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Successfully retrieved trt data from SG. " << endreq; 
    }
  
    InDet::TRT_DriftCircleContainer::const_iterator trtdriftContainerItr  = driftCircleContainer->begin();
    InDet::TRT_DriftCircleContainer::const_iterator trtdriftContainerItrE = driftCircleContainer->end();
  
    for (; trtdriftContainerItr != trtdriftContainerItrE; ++trtdriftContainerItr) {
    
      InDet::TRT_DriftCircleCollection::const_iterator trtItr = (*trtdriftContainerItr)->begin();
      InDet::TRT_DriftCircleCollection::const_iterator trtEnd = (*trtdriftContainerItr)->end();
    
      for(; trtItr!=trtEnd; trtItr++){
      
	// find out which detector element the hit belongs to
	const InDetDD::TRT_BaseElement *det = (*trtItr)->detectorElement();
	//	  InDetDD::TRT_BaseElement::Type type = det->type();
      
	Identifier ID = (*trtItr)->identify();
	// 	  IdentifierHash iH = m_trtHelper->straw_layer_hash(m_trtHelper->layer_id(ID));

	const Amg::Vector3D& strawcenter = det->strawCenter(m_trtHelper->straw(ID));
	trtphi.push_back(strawcenter.phi());
      
	bool hth = false;
	float hphi = strawcenter.phi();
	float heta = - TMath::Log(strawcenter.theta()/2.);
	if ((*trtItr)->highLevel()){
	  hth = true;
	}
	int countbin=0;	
	if(hth_delta_phi(hphi, roi->phi()) < 0.1)
	   {

	for(float roibincenter=roi->phiMinus()+0.005; roibincenter < roi->phiPlus(); roibincenter+=0.01)
		{	
		if (hth_delta_phi(hphi,roibincenter)<=0.005 && hth_eta_match(roi->eta(), heta, m_etaHalfWidth))
	
		{
			if(hth){
				count_httrt.at(countbin) = count_httrt.at(countbin)+1.;
				} 
			count_tottrt.at(countbin) = count_tottrt.at(countbin)+1.;
			break;							
			}
		countbin++;
		}
	   }
	if (msgLvl()<= MSG::DEBUG) msg() <<  MSG::DEBUG
					 << "timeOverThreshold=" << (*trtItr)->timeOverThreshold()
					 << "  highLevel=" << (*trtItr)->highLevel()
					 << " rawDriftTime=" << (*trtItr)->rawDriftTime()
					 << " barrel_ec=" << m_trtHelper->barrel_ec(ID)
					 << " phi_module=" << m_trtHelper->phi_module(ID)
					 << " layer_or_wheel=" << m_trtHelper->layer_or_wheel(ID)
					 << " straw_layer=" << m_trtHelper->straw_layer(ID)
					 << " straw=" << m_trtHelper->straw(ID)
					 << " scR=" << det->strawCenter(m_trtHelper->straw(ID)).perp()
					 << " scPhi=" << hphi
					 << " scEta=" << heta
					 <<endreq;		    
      }
     }
}
      for (size_t iw=0; iw< count_httrt.size(); iw++)
	{
		if (iw == 0) 
		{	result = count_httrt[iw];
			dist = pos;
		} 
		if(iw != 0 and result <= count_httrt[iw]) { result = count_httrt[iw]; dist = pos; }
		pos = pos + 1; 
	}  
	for(size_t pbins = 0; pbins<c_phibins; pbins++){
			if(pbins==0) 
			{
				m_trththits[pbins] = count_httrt[dist];
				m_trthits[pbins] = count_tottrt[dist];
				if(dist+1<20)
				{
					m_trththits[pbins] += count_httrt[dist+1];
					m_trthits[pbins] += count_tottrt[dist+1];
				}
				if(dist-1>-1)
				{
					m_trththits[pbins] += count_httrt[dist-1];
					m_trthits[pbins] += count_tottrt[dist-1];
				}
			}
			else 
			{
				int int_pbins = (int) pbins;
				m_trththits[pbins] += m_trththits[pbins-1];
				m_trthits[pbins] += m_trthits[pbins-1];
				if(dist+(pbins+1)<20)
				{
					m_trththits[pbins] += count_httrt[dist+(pbins+1)];
					m_trthits[pbins] += count_tottrt[dist+(pbins+1)];
				}
				if(dist-(int_pbins+1)>-1)
				{
					m_trththits[pbins] += count_httrt[dist-(pbins+1)];
					m_trthits[pbins] += count_tottrt[dist-(pbins+1)];
				}
			}
		}
  
  
  for (size_t iw=0; iw<c_phibins; iw++){
    if (m_trthits[iw])
      m_trththits[iw] = m_trththits[iw] + (m_trththits[iw]/m_trthits[iw]);
   
    if (msgLvl()<= MSG::DEBUG) msg() <<  MSG::DEBUG << "trththits at "<< iw << ": " << m_trththits[iw] << endreq;
  }
  out->output(m_trththits);

  std::string key="";
  std::string label="TrigTRTHTCounts";
  if (recordAndAttachFeature<TrigRNNOutput>(outputTE, out, key,label) !=HLT::OK){
    return HLT::NAV_ERROR;

  }
  return HLT::OK;

}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTRTHTHCounter::hltFinalize() {
  msg() << MSG::DEBUG << " finalizing TrigTRTHTHCounter : "<< name() << endreq; 
  return HLT::OK;  
}

