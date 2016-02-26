/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_ToT_Tools/TRT_ToT_dEdx.h"
#include "TRT_ToT_Tools/TRT_ToT_Corrections.h"


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "InDetIdentifier/TRT_ID.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/IChronoStatSvc.h"

#include "TF1.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "StoreGate/DataHandle.h"

// constructor
TRT_ToT_dEdx::TRT_ToT_dEdx(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t,n,p)
{
  declareInterface<ITRT_ToT_dEdx>(this);
}


// destructor
TRT_ToT_dEdx::~TRT_ToT_dEdx() {}


// initialize
StatusCode TRT_ToT_dEdx::initialize() {

  MsgStream log(msgSvc(), name());
  
  // retrieve TRT-ID helper
  StoreGateSvc* detStore = 0;
  StatusCode sc = service( "DetectorStore", detStore );
  if (sc.isFailure()){
    log << MSG::ERROR << "Could not get DetectorStore" << endreq;
    return sc;
  }
  
  sc = detStore->retrieve(m_trtId, "TRT_ID");
  if (sc.isFailure()){
    log << MSG::ERROR << "Could not get TRT_ID helper !" << endreq;
    return StatusCode::FAILURE;
  }

  sc = detStore->retrieve(m_trtman, "TRT");
  if (sc.isFailure()){
    log << MSG::ERROR << "Could not get TRT detector manager !" << endreq;

    return StatusCode::FAILURE;
  }

  m_timingProfile=0;
  sc = service("ChronoStatSvc", m_timingProfile);
  if ( sc.isFailure() || 0 == m_timingProfile) {
    log<< MSG::DEBUG <<"Can not find ChronoStatSvc name="<<m_timingProfile << endreq;
  }
 
  const DataHandle<CondAttrListVec> aptr;
  std::string folderName = {"/TRT/Calib/ToT/ToTVectors"};
  if (StatusCode::SUCCESS == detStore->regFcn(&TRT_ToT_dEdx::update,this,aptr,folderName)){
   ATH_MSG_DEBUG ("Registered callback for ToT");
  }else{
		ATH_MSG_ERROR ("Callback registration failed for /TRT/Calib/ToT/ToTVectors ");
  }

  const DataHandle<CondAttrListCollection> affectedRegionH;
  if (detStore->regFcn(&TRT_ToT_dEdx::update2,this,affectedRegionH,"/TRT/Calib/ToT/ToTValue").isSuccess()){
	ATH_MSG_DEBUG ( "Registered callback for  /TRT/Calib/ToT/ToTValue " );
  }else{
	ATH_MSG_WARNING ( "Cannot register callback for /TRT/Calib/ToT/ToTValue " );
  }

  
  log <<  MSG::INFO  << name() <<" initialize() successful" << endreq;    
  return StatusCode::SUCCESS;
}

StatusCode TRT_ToT_dEdx::finalize() {
  MsgStream log(msgSvc(), name());
  log <<  MSG::VERBOSE  << "... in finalize() ..." << endreq ;
  if(m_timingProfile)m_timingProfile->chronoPrint("TRT_ToT_dEdx"); //MJ
  return StatusCode::SUCCESS;
}


StatusCode TRT_ToT_dEdx::update(int& /*i*/ , std::list<std::string>& /*l*/)
{

StoreGateSvc* detStore = 0;
StatusCode sc = service( "DetectorStore", detStore );

std::vector<std::string>  dict_names = {"resolution","resolution_e","para_long_corrRZ_MC","para_short_corrRZ_MC","para_end_corrRZ_MC","para_long_corrRZL_MC","para_short_corrRZL_MC","para_end_corrRZL_MC"};
std::map<std::string,std::vector<float> > result_dict;

const DataHandle<CondAttrListVec> channel_values;

if (StatusCode::SUCCESS == detStore->retrieve(channel_values, "/TRT/Calib/ToT/ToTVectors" )){

CondAttrListVec::const_iterator first_channel = channel_values->begin();
CondAttrListVec::const_iterator last_channel  = channel_values->end();

unsigned int current_channel = 0;
std::vector<float> current_array_values = {};

  for (; first_channel != last_channel; ++first_channel) {
   
	if (current_channel != first_channel->first){
	
		result_dict[dict_names[current_channel]] = current_array_values;
		current_channel = first_channel->first;      
        current_array_values.clear();

	}
	current_array_values.push_back(first_channel->second["array_value"].data<float>());		
   
  }
  result_dict[dict_names[current_channel]] = current_array_values;
 
}else {
       ATH_MSG_ERROR ("Problem reading condDB object. -");
       return StatusCode::FAILURE;
}

    for (unsigned int ind=0; ind < 4; ++ind) {
	Dedxcorrection::resolution[ind]=result_dict["resolution"][ind];
    }

    for (unsigned int ind=0; ind < 4; ++ind) {
	Dedxcorrection::resolution_e[ind]=result_dict["resolution_e"][ind];
    }

    for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection::para_long_corrRZ_MC[ind]=result_dict["para_long_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 216; ++ind) {
	Dedxcorrection::para_short_corrRZ_MC[ind]=result_dict["para_short_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 630; ++ind) {
	Dedxcorrection::para_long_corrRZL_MC[ind]=result_dict["para_long_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 63; ++ind) {
	Dedxcorrection::para_short_corrRZL_MC[ind]=result_dict["para_short_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 252; ++ind) {
	Dedxcorrection::para_end_corrRZL_MC[ind]=result_dict["para_end_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 3240; ++ind) {
	Dedxcorrection::para_long_corrRZ[ind]=result_dict["para_long_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 216; ++ind) {
	Dedxcorrection::para_short_corrRZ[ind]=result_dict["para_short_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 630; ++ind) {
	Dedxcorrection::para_long_corrRZL_DATA[ind]=result_dict["para_long_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 63; ++ind) {
	Dedxcorrection::para_short_corrRZL_DATA[ind]=result_dict["para_short_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 252; ++ind) {
	Dedxcorrection::para_end_corrRZL_DATA[ind]=result_dict["para_end_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 336; ++ind) {
	Dedxcorrection::para_end_corrRZ[ind]=result_dict["para_end_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 336; ++ind) {
	Dedxcorrection::para_end_corrRZ_MC[ind]=result_dict["para_end_corrRZ_MC"][ind];
    }

    return StatusCode::SUCCESS;
}



StatusCode TRT_ToT_dEdx::update2(int& /*i*/, std::list<std::string>& /*l*/ ){

const CondAttrListCollection* attrListColl = 0;
StoreGateSvc* detStore = 0;
StatusCode sc = service( "DetectorStore", detStore );

if (StatusCode::SUCCESS == detStore->retrieve(attrListColl, "/TRT/Calib/ToT/ToTValue" )){
CondAttrListCollection::const_iterator first = attrListColl->begin();
CondAttrListCollection::const_iterator last  = attrListColl->end();


  for (; first != last; ++first) {
    
	const coral::AttributeList& attrList = (*first).second;
	Dedxcorrection::paraL_dEdx_p1 = attrList["paraL_dEdx_p1"].data<float>();
	Dedxcorrection::paraL_dEdx_p2 = attrList["paraL_dEdx_p2"].data<float>();
	Dedxcorrection::paraL_dEdx_p3 = attrList["paraL_dEdx_p3"].data<float>();
	Dedxcorrection::paraL_dEdx_p4 = attrList["paraL_dEdx_p4"].data<float>();
	Dedxcorrection::paraL_dEdx_p5 = attrList["paraL_dEdx_p5"].data<float>();

	Dedxcorrection::para_dEdx_p1 = attrList["para_dEdx_p1"].data<float>();
	Dedxcorrection::para_dEdx_p2 = attrList["para_dEdx_p2"].data<float>();
	Dedxcorrection::para_dEdx_p3 = attrList["para_dEdx_p3"].data<float>();
	Dedxcorrection::para_dEdx_p4 = attrList["para_dEdx_p4"].data<float>();
	Dedxcorrection::para_dEdx_p5 = attrList["para_dEdx_p5"].data<float>();
	  
	Dedxcorrection::norm_offset_data = attrList["norm_offset_data"].data<float>();
	Dedxcorrection::norm_slope_tot = attrList["norm_slope_tot"].data<float>();  
	Dedxcorrection::norm_slope_totl = attrList["norm_slope_totl"].data<float>(); 
	Dedxcorrection::norm_offset_tot = attrList["norm_offset_tot"].data<float>(); 
	Dedxcorrection::norm_offset_totl = attrList["norm_offset_totl"].data<float>();		
    Dedxcorrection::norm_nzero=attrList["norm_nzero"].data<int>();
    
  }
}else {
       ATH_MSG_ERROR ("Problem reading condDB object. -");
       return StatusCode::FAILURE;
      }
 
 return StatusCode::SUCCESS;
}


bool TRT_ToT_dEdx::isGood_Hit(const Trk::TrackStateOnSurface *itr, bool useHThits) const
{
  const Trk::MeasurementBase* trkM = itr->measurementOnTrack();
  if (!trkM)  return false;   
  const InDet::TRT_DriftCircleOnTrack *driftcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(trkM);  
  if (!driftcircle) return false;

  const Trk::TrackParameters* trkP = itr->trackParameters();
  if(trkP==0)return false; 
  double Trt_Rtrack = fabs(trkP->parameters()[Trk::locR]);
  double Trt_RHit = fabs(driftcircle->localParameters()[Trk::driftRadius]);
  if ( Trt_RHit==0)return false;                                     // tube hit
  if ( (Trt_Rtrack >= 1.9) || (Trt_Rtrack <= 0.01) )return false;    // drift radius close to wire or wall
  if(!useHThits){
    int TrtHl = driftcircle->highLevel();
    if (TrtHl==1) return false; 
  }
  return true;
}

double TRT_ToT_dEdx::dEdx(const Trk::Track* track, bool DivideByL, bool useHThits, bool corrected) const
{
  //m_timingProfile->chronoStart("TRT_ToT_dEdx::dEdx");
  bool data=true;
  double nVtx=-1.;
	
  // Event information 
  const xAOD::EventInfo* eventInfo = 0;
  ATH_CHECK( evtStore()->retrieve( eventInfo) );
  
 // check if data or MC 
  if(eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION ) ){
	data=false;
  }
  // 	Average interactions per crossing for the current BCID
  nVtx = eventInfo->averageInteractionsPerCrossing();
   
  std::vector<double> vecToT;
  int nhits =0;

  double ToTsum = 0;
  if (!track) {
    return 0;
  }
  const DataVector<const Trk::TrackStateOnSurface>* vtsos = track->trackStateOnSurfaces(); 
  if (!vtsos) {
    return 0;
  }
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itr  = vtsos->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itre = vtsos->end();  
  for ( ; itr!=itre ; ++itr) {  
    if ( isGood_Hit((*itr), useHThits)) {
      vecToT.push_back(correctToT_corrRZ((*itr),DivideByL, data, corrected));
      nhits++;

    }
  } 

  sort(vecToT.begin(), vecToT.end());
  int limit = (int)vecToT.size();
  int ntrunk = 1;
  if(DivideByL){
    nhits-=ntrunk;
    limit-=ntrunk;
  }
  for (int i = 0; i < limit;i++){
    ToTsum+=vecToT.at(i);
  } 
  ToTsum*=correctNormalization(DivideByL,data,nVtx);
  // m_timingProfile->chronoStop("TRT_ToT_dEdx::dEdx");

  if(nhits<1)return 0.0;

  return ToTsum/nhits;
}

double TRT_ToT_dEdx::usedHits(const Trk::Track* track, bool DivideByL, bool useHThits) const
{
  int nhits =0;
  if (!track) {
    return 0;
  }
  const DataVector<const Trk::TrackStateOnSurface>* vtsos = track->trackStateOnSurfaces(); 
  if (!vtsos) {
    return 0;
  }
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itr  = vtsos->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itre = vtsos->end();  
  for ( ; itr!=itre ; ++itr) {  
    if ( isGood_Hit((*itr), useHThits)) {
        nhits++;
    }
  } 
  int ntrunk = 1;
  if(DivideByL){
    nhits-=ntrunk;
  }
  return nhits;
}

double TRT_ToT_dEdx::correctNormalization(bool divideLength,bool scaledata, double nVtx) const{
    if(nVtx<=0)nVtx=Dedxcorrection::norm_nzero;
    double slope = Dedxcorrection::norm_slope_tot;
    double offset = Dedxcorrection::norm_offset_tot;
    if(divideLength){
      slope = Dedxcorrection::norm_slope_tot;
      offset = Dedxcorrection::norm_offset_tot;
    } 
    double shift = Dedxcorrection::norm_offset_data;
    if(!scaledata)shift = 0;
    return (slope*Dedxcorrection::norm_nzero+offset)/(slope*nVtx+offset+shift);
  }




double TRT_ToT_dEdx::getProb(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, int nUsedHits, bool dividebyL) const{
  double dEdx_pred = predictdEdx(pTrk, hypothesis, dividebyL); 
  if(dEdx_pred==0)return 0.0;
  if(hypothesis==Trk::electron){
    // correction for pTrk in [MeV]
    double factor = 1;
    double correct = 1+factor*(0.045*log10(pTrk)+0.885-1);
    dEdx_pred= dEdx_pred/correct;
  }

  double Resolution = Dedxcorrection::resolution[0]+Dedxcorrection::resolution[1]*(nUsedHits+0.5)+Dedxcorrection::resolution[2]*(nUsedHits+0.5)*(nUsedHits+0.5)+Dedxcorrection::resolution[3]*(nUsedHits+0.5)*(nUsedHits+0.5)*(nUsedHits+0.5);
  if(hypothesis==Trk::electron){
    Resolution = Dedxcorrection::resolution_e[0]+Dedxcorrection::resolution_e[1]*(nUsedHits+0.5)+Dedxcorrection::resolution_e[2]*(nUsedHits+0.5)*(nUsedHits+0.5)+Dedxcorrection::resolution_e[3]*(nUsedHits+0.5)*(nUsedHits+0.5)*(nUsedHits+0.5);
  }

  double prob =exp( -0.5 * ( ( ( dEdx_obs - dEdx_pred ) / (Resolution*dEdx_pred) ) * 
			     ( ( dEdx_obs - dEdx_pred ) / (Resolution*dEdx_pred) ) ))  ; 

  return prob;

}


double TRT_ToT_dEdx::getTest(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, Trk::ParticleHypothesis antihypothesis, int nUsedHits, bool dividebyL) const{
  double Pone = getProb(dEdx_obs,pTrk,hypothesis,nUsedHits, dividebyL);
  double Ptwo = getProb(dEdx_obs,pTrk,antihypothesis,nUsedHits, dividebyL);
  if( (Pone+Ptwo) != 0){
    return Pone/(Pone+Ptwo);
  }else
    return 0.5;

}

double TRT_ToT_dEdx::predictdEdx(const double pTrk, Trk::ParticleHypothesis hypothesis, bool dividebyL)const{
  
  double mass = m_particlemasses.mass[hypothesis];

  double betaGamma = pTrk/mass;

  /** @todo why is it possible that a 20 MeV particle reaches this point? (see Savannah bug 94644) */
  // low momentum particle can create floating point error 
  // do we need the check in the log parameter in addition? will create CPU increase
  // do we want to throw an assertion here?
  if(pTrk<100)return 0; 
  if(dividebyL){    
    if(Dedxcorrection::paraL_dEdx_p3+1./( std::pow( betaGamma, Dedxcorrection::paraL_dEdx_p5))<=0) return 0;
    return Dedxcorrection::paraL_dEdx_p1/std::pow( sqrt( (betaGamma*betaGamma)/(1.+(betaGamma*betaGamma)) ), Dedxcorrection::paraL_dEdx_p4)  * 
      (Dedxcorrection::paraL_dEdx_p2 - std::pow( sqrt( (betaGamma*betaGamma)/(1.+(betaGamma*betaGamma)) ), Dedxcorrection::paraL_dEdx_p4 ) - log(Dedxcorrection::paraL_dEdx_p3+1./( std::pow( betaGamma, Dedxcorrection::paraL_dEdx_p5) ) ) );
  }else {
    if(Dedxcorrection::para_dEdx_p3+1./( std::pow( betaGamma, Dedxcorrection::para_dEdx_p5) )<=0)return 0; 
    return Dedxcorrection::para_dEdx_p1/std::pow( sqrt( (betaGamma*betaGamma)/(1.+(betaGamma*betaGamma)) ), Dedxcorrection::para_dEdx_p4)  * 
      (Dedxcorrection::para_dEdx_p2 - std::pow( sqrt( (betaGamma*betaGamma)/(1.+(betaGamma*betaGamma)) ), Dedxcorrection::para_dEdx_p4 ) - log(Dedxcorrection::para_dEdx_p3+1./( std::pow( betaGamma, Dedxcorrection::para_dEdx_p5) ) ) );
  }
  //return 0;  
}

double TRT_ToT_dEdx::mass(const double pTrk, double dEdx )const{
  
  // only for testing purposes!!!!
  // note that dE/dx has to be corrected on track level first before value can be transferred to mass
  // this has to be tuned on user level
  /** @todo make failsafe */
  static const double bg_min = 0.001;
  static const double bg_max = 3;   // maximal allowed bg
  
  static const std::string blumRolandiFunction = "( [0]/sqrt( (x*x/([5]*[5]))/(1.+(x*x/([5]*[5]))) )^[3] ) * ([1] - sqrt( (x*x/([5]*[5]))/(1.+(x*x/([5]*[5]))) )^[3] - log([2]+1./((x/[5])^[4]) ) )";
  
  TF1 blumRolandi( "BR", blumRolandiFunction.c_str(), 0.7, 100000);

  blumRolandi.SetParameters(Dedxcorrection::para_dEdx_p1,Dedxcorrection::para_dEdx_p2,Dedxcorrection::para_dEdx_p3,Dedxcorrection::para_dEdx_p4,Dedxcorrection::para_dEdx_p5, 1. ); 
  //blumRolandi.SetParameters(&Dedxcorrection::para_dEdx_BB);
  double betaGamma = blumRolandi.GetX(dEdx, bg_min, bg_max); 
  
  return pTrk/betaGamma;
}


double TRT_ToT_dEdx::correctToT_corrRZ(const Trk::TrackStateOnSurface *itr,bool DivideByL, bool data, bool corrected) const{

  const Trk::MeasurementBase* trkM = itr->measurementOnTrack();
  const Trk::TrackParameters* trkP = itr->trackParameters();
  const InDet::TRT_DriftCircleOnTrack *driftcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(trkM);

  if (!driftcircle) return 0;
  if (driftcircle->prepRawData()==0) return 0;

  Identifier DCId = driftcircle->identify();
  unsigned int BitPattern = driftcircle->prepRawData()->getWord();
  double ToT = getToTlargerIsland(BitPattern);
  double L = 0;
  double Trt_HitPhi = trkP->parameters()[Trk::phi];
  double Trt_HitTheta = trkP->parameters()[Trk::theta];
  const InDetDD::TRT_BaseElement* element = m_trtman->getElement(DCId);
  double strawphi = element->center(DCId).phi();
  int HitPart =  m_trtId->barrel_ec(DCId);
  int StrawLayer = m_trtId->straw_layer(DCId);
  int Layer = m_trtId->layer_or_wheel(DCId);
  
  
  double HitRtrack = fabs(trkP->parameters()[Trk::locR]);
  
  
  if (fabs(HitPart)==1) { //Barrel
    L = 2*sqrt(4-HitRtrack*HitRtrack)*1./fabs(sin(Trt_HitTheta));
  }
  if (fabs(HitPart)==2) { //EndCap
    double phiHit =  Trt_HitPhi;
    double omega = strawphi;
    L = 2*sqrt(4-HitRtrack*HitRtrack)*1./sqrt(1-sin(Trt_HitTheta)*sin(Trt_HitTheta)*cos(phiHit-omega)*cos(phiHit-omega));
  }
  if(DivideByL)ToT = ToT/L;
  if(!corrected) return ToT;
  /* else correct */
	   
  double HitZ = driftcircle->globalPosition().z();
  double trackx =  driftcircle->globalPosition().x();
  double tracky =  driftcircle->globalPosition().y();
  double HitRpos = sqrt(trackx*trackx+tracky*tracky);
  
  /** @todo implement possiblity to set the scaling factor run-by-run from database, 
      should probably be done later on track- level */
  double ToTmip = 1;
  double valToT = 0;
  if(DivideByL){
    if (abs(HitPart)==1) // Barrel
      valToT = fitFuncBarrel_corrRZL(HitRtrack,HitZ, Layer, StrawLayer, data);
    else // End-cap
      valToT = fitFuncEndcap_corrRZL(HitRtrack,HitRpos,Layer, HitZ>0?1:(HitZ<0?-1:0) ,data);   
  }else{
    if (abs(HitPart)==1) // Barrel
      valToT = fitFuncBarrel_corrRZ(HitRtrack,HitZ, Layer, StrawLayer, data);
    else // End-cap
      valToT = fitFuncEndcap_corrRZ(HitRtrack,HitRpos,Layer, HitZ>0?1:(HitZ<0?-1:0) ,data);
  } 
  return ToTmip*ToT/valToT;

}

double TRT_ToT_dEdx::correctToT_corrRZL(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer, bool data) const{
  // quality criteria, if not fullfilled return 0 // make sure this is not counted!
  if (!driftcircle) return 0;
  double HitRtrack = fabs(trkP->parameters()[Trk::locR]);  
  double Trt_RHit = fabs(driftcircle->localParameters()[Trk::driftRadius]);
  if ( Trt_RHit==0)return 0;                                    // tube hit
  if ( (HitRtrack >= 1.9) || (HitRtrack <= 0.01) )return 0;    // drift radius close to wire or wall
  if ( fabs(HitRtrack-Trt_RHit)>0.2)return 0;                

  if (driftcircle->prepRawData()==0) return 0;
  unsigned int BitPattern = driftcircle->prepRawData()->getWord();
  double ToT = getToTlargerIsland(BitPattern);
  double L = 0;
  double Trt_HitTheta = trkP->parameters()[Trk::theta];
  Identifier DCId = driftcircle->identify();
  const InDetDD::TRT_BaseElement* element = m_trtman->getElement(DCId);
  if (fabs(HitPart)==1) { //Barrel
    L = 2*sqrt(4-HitRtrack*HitRtrack)*1./fabs(sin(Trt_HitTheta));
  }
  if (fabs(HitPart)==2) { //EndCap
    double phiHit =  trkP->parameters()[Trk::phi];
    double omega = element->center(DCId).phi();
    L = 2*sqrt(4-HitRtrack*HitRtrack)*1./sqrt(1-sin(Trt_HitTheta)*sin(Trt_HitTheta)*cos(phiHit-omega)*cos(phiHit-omega));
  }
  ToT = ToT/L;
  const Amg::Vector3D& gp = driftcircle->globalPosition();
  double HitR = sqrt( gp.x() * gp.x() + gp.y() * gp.y() );
  double HitZ = gp.z();
  double ToTmip = 1;
  double valToT = 0;

  if (abs(HitPart)==1) // Barrel
    valToT = fitFuncBarrel_corrRZL(HitRtrack,HitZ, Layer, StrawLayer, data);
  else // End-cap
    valToT = fitFuncEndcap_corrRZL(HitRtrack ,HitR,Layer, HitZ>0?1:(HitZ<0?-1:0) , data);
  return ToTmip*ToT/valToT;
}


double TRT_ToT_dEdx::correctToT_corrRZ(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer, bool data) const{
  // quality criteria, if not fullfilled return 0 // make sure this is not counted!
  if (!driftcircle) return 0;
  double HitRtrack = fabs(trkP->parameters()[Trk::locR]);  
  double Trt_RHit = fabs(driftcircle->localParameters()[Trk::driftRadius]);
  if ( Trt_RHit==0)return 0;                                    // tube hit
  if ( (HitRtrack >= 1.9) || (HitRtrack <= 0.01) )return 0;    // drift radius close to wire or wall
  if ( fabs(HitRtrack-Trt_RHit)>0.2)return 0;                

  if (driftcircle->prepRawData()==0) return 0;
  unsigned int BitPattern = driftcircle->prepRawData()->getWord();
  double ToT = getToTlargerIsland(BitPattern);
  //  Identifier DCId = driftcircle->identify();
  //  const InDetDD::TRT_BaseElement* element = m_trtman->getElement(DCId);
  const Amg::Vector3D& gp = driftcircle->globalPosition();
  double HitR = sqrt( gp.x() * gp.x() + gp.y() * gp.y() );
  double HitZ = gp.z();
  double ToTmip = 1;
  double valToT = 0;

  if (abs(HitPart)==1) // Barrel
    valToT = fitFuncBarrel_corrRZ(HitRtrack,HitZ, Layer, StrawLayer, data);
  else // End-cap
    valToT = fitFuncEndcap_corrRZ(HitRtrack ,HitR,Layer, HitZ>0?1:(HitZ<0?-1:0) , data);
  return ToTmip*ToT/valToT;
}


double TRT_ToT_dEdx::fitFuncBarrel_corrRZ(double driftRadius,double zPosition, int Layer, int StrawLayer, bool data) const{
 
  if(Layer==0 && StrawLayer<9)
    return fitFuncBarrelShort_corrRZ(driftRadius,zPosition, StrawLayer,data);
  else
    return fitFuncBarrelLong_corrRZ(driftRadius,zPosition,Layer, StrawLayer,data);
  //return 0;
}


double TRT_ToT_dEdx::fitFuncEndcap_corrRZ(double driftRadius,double radialPosition, int Layer, int sign, bool data) const {
  /**
   * T(r,R) = T0(r)+ a(r)*R
   */
  double T0 =  fitFuncPol_corrRZ(0,driftRadius,Layer,0,sign,2,data);
  double a  =  fitFuncPol_corrRZ(1,driftRadius,Layer,0,sign,2,data);
  return T0+a*radialPosition;
}


double TRT_ToT_dEdx::fitFuncBarrelLong_corrRZ(double driftRadius,double zPosition, int Layer, int StrawLayer, bool data)const{
  /**
   *                   |z|       /|z| - l  \
   * T(r,z) = T0(r) +   ---  exp|----------|
   *                    v(r)     \  s(r)   /
   */ 
  double z = fabs(zPosition);
  int sign=1;
  if(zPosition<0)sign=-1;
  double l = 704.6;
  // define set of parameters for negative and positive z positions
  double T0 =  fitFuncPol_corrRZ(0,driftRadius,Layer,StrawLayer,sign,0,data);
  double  v =  fitFuncPol_corrRZ(1,driftRadius,Layer,StrawLayer,sign,0,data);
  double  s =  fitFuncPol_corrRZ(2,driftRadius,Layer,StrawLayer,sign,0,data);
  return T0+z/v*exp((z-l)/s);
}


double TRT_ToT_dEdx::fitFuncBarrelShort_corrRZ(double driftRadius,double zPosition, int StrawLayer, bool data)const{
  /**
   *  T(r,z) = T0(r)+ b(r)*|z|
   */
  double z = fabs(zPosition);
  int sign=1;
  if(zPosition<0)sign=-1;
  double T0 = fitFuncPol_corrRZ(0,driftRadius,0,StrawLayer,sign,1,data);
  double b  = fitFuncPol_corrRZ(1,driftRadius,0,StrawLayer,sign,1,data);
  return T0+b*z; 

}


double TRT_ToT_dEdx::fitFuncPol_corrRZ(int parameter, double driftRadius, int Layer, int Strawlayer, int sign, int set,  bool data)const{

  double a = 0;
  double b = 0;
  double c = 0;
  double d = 0;
  double e = 0;
  double f = 0;
  double r = driftRadius;
  int offset = 0;
  if(data){
    if(set==0){ // long straws in barrel
      //int layerindex = Layer*30+Strawlayer;
     //int parId=0;
     //parId=0;
     //if(sign>0)parId=1620;  // FIXME: parId is not used
     a = Dedxcorrection::para_long_corrRZ[(6*parameter+0)*30*3+Layer*30+Strawlayer+offset];
     b = Dedxcorrection::para_long_corrRZ[(6*parameter+1)*30*3+Layer*30+Strawlayer+offset];
     c = Dedxcorrection::para_long_corrRZ[(6*parameter+2)*30*3+Layer*30+Strawlayer+offset];
     d = Dedxcorrection::para_long_corrRZ[(6*parameter+3)*30*3+Layer*30+Strawlayer+offset];
     e = Dedxcorrection::para_long_corrRZ[(6*parameter+4)*30*3+Layer*30+Strawlayer+offset];
     f = Dedxcorrection::para_long_corrRZ[(6*parameter+5)*30*3+Layer*30+Strawlayer+offset];
     
    }else if (set ==1) { // short straws in barrel
      if(sign > 0) offset+=108;
      a = Dedxcorrection::para_short_corrRZ[(6*parameter+0)*9+Layer+offset];
      b = Dedxcorrection::para_short_corrRZ[(6*parameter+1)*9+Layer+offset];
      c = Dedxcorrection::para_short_corrRZ[(6*parameter+2)*9+Layer+offset];
      d = Dedxcorrection::para_short_corrRZ[(6*parameter+3)*9+Layer+offset];
      e = Dedxcorrection::para_short_corrRZ[(6*parameter+4)*9+Layer+offset];
      f = Dedxcorrection::para_short_corrRZ[(6*parameter+5)*9+Layer+offset];
    }else{  // straws in endcap
      if(sign >0) Layer+=14;
      a = Dedxcorrection::para_end_corrRZ[(6*parameter+0)*28+Layer];
      b = Dedxcorrection::para_end_corrRZ[(6*parameter+1)*28+Layer];
      c = Dedxcorrection::para_end_corrRZ[(6*parameter+2)*28+Layer];
      d = Dedxcorrection::para_end_corrRZ[(6*parameter+3)*28+Layer];
      e = Dedxcorrection::para_end_corrRZ[(6*parameter+4)*28+Layer];
      f = Dedxcorrection::para_end_corrRZ[(6*parameter+5)*28+Layer];
    }
  }else{
    if(set==0){ // long straws in barrel
      if(sign > 0) offset=1620;
      a = Dedxcorrection::para_long_corrRZ_MC[(6*parameter+0)*30*3+Layer*30+Strawlayer+offset];
      b = Dedxcorrection::para_long_corrRZ_MC[(6*parameter+1)*30*3+Layer*30+Strawlayer+offset];
      c = Dedxcorrection::para_long_corrRZ_MC[(6*parameter+2)*30*3+Layer*30+Strawlayer+offset];
      d = Dedxcorrection::para_long_corrRZ_MC[(6*parameter+3)*30*3+Layer*30+Strawlayer+offset];
      e = Dedxcorrection::para_long_corrRZ_MC[(6*parameter+4)*30*3+Layer*30+Strawlayer+offset];
      f = Dedxcorrection::para_long_corrRZ_MC[(6*parameter+5)*30*3+Layer*30+Strawlayer+offset];
    }else if (set ==1) { // short straws in barrel
      if(sign > 0) offset+=108;
      a = Dedxcorrection::para_short_corrRZ_MC[(6*parameter+0)*9+Layer+offset];
      b = Dedxcorrection::para_short_corrRZ_MC[(6*parameter+1)*9+Layer+offset];
      c = Dedxcorrection::para_short_corrRZ_MC[(6*parameter+2)*9+Layer+offset];
      d = Dedxcorrection::para_short_corrRZ_MC[(6*parameter+3)*9+Layer+offset];
      e = Dedxcorrection::para_short_corrRZ_MC[(6*parameter+4)*9+Layer+offset];
      f = Dedxcorrection::para_short_corrRZ_MC[(6*parameter+5)*9+Layer+offset];
  }else{  // straws in endcap
      if(sign >0) Layer+=14;
      a = Dedxcorrection::para_end_corrRZ_MC[(6*parameter+0)*28+Layer];
      b = Dedxcorrection::para_end_corrRZ_MC[(6*parameter+1)*28+Layer];
      c = Dedxcorrection::para_end_corrRZ_MC[(6*parameter+2)*28+Layer];
      d = Dedxcorrection::para_end_corrRZ_MC[(6*parameter+3)*28+Layer];
      e = Dedxcorrection::para_end_corrRZ_MC[(6*parameter+4)*28+Layer];
      f = Dedxcorrection::para_end_corrRZ_MC[(6*parameter+5)*28+Layer];
    }    
  }
  return a+b*r+c*r*r+d*r*r*r+e*r*r*r*r+f*r*r*r*r*r;
}


double TRT_ToT_dEdx::fitFuncEndcap_corrRZL(double driftRadius,double radialPosition, int Layer, int sign,  bool data)const {
  /*
   T(r,R) = T0(r)+ a(r)*R
  */

  double r = fabs(driftRadius);
  double a,b,c,d,e,f,g,h,i;  
  if(sign >0) Layer+=14;
  if(data){
    a = Dedxcorrection::para_end_corrRZL_DATA[(0)*28+Layer];
    b = Dedxcorrection::para_end_corrRZL_DATA[(1)*28+Layer];
    c = Dedxcorrection::para_end_corrRZL_DATA[(2)*28+Layer];
    d = Dedxcorrection::para_end_corrRZL_DATA[(3)*28+Layer];
    e = Dedxcorrection::para_end_corrRZL_DATA[(4)*28+Layer];
    f = Dedxcorrection::para_end_corrRZL_DATA[(5)*28+Layer];  
    g = Dedxcorrection::para_end_corrRZL_DATA[(6)*28+Layer];  
    h = Dedxcorrection::para_end_corrRZL_DATA[(7)*28+Layer];  
    i = Dedxcorrection::para_end_corrRZL_DATA[(8)*28+Layer];  
  }else{
    a = Dedxcorrection::para_end_corrRZL_MC[(0)*28+Layer];
    b = Dedxcorrection::para_end_corrRZL_MC[(1)*28+Layer];
    c = Dedxcorrection::para_end_corrRZL_MC[(2)*28+Layer];
    d = Dedxcorrection::para_end_corrRZL_MC[(3)*28+Layer];
    e = Dedxcorrection::para_end_corrRZL_MC[(4)*28+Layer];
    f = Dedxcorrection::para_end_corrRZL_MC[(5)*28+Layer];  
    g = Dedxcorrection::para_end_corrRZL_MC[(6)*28+Layer];  
    h = Dedxcorrection::para_end_corrRZL_MC[(7)*28+Layer];  
    i = Dedxcorrection::para_end_corrRZL_MC[(8)*28+Layer]; 
  } 

  double T1    = b*r+c*r*r+d*r*r*r+e*r*r*r*r+f*r*r*r*r*r;
  double slope = g+h*r+i*r*r;
  double T0    = a;
  
  return T0+T1+slope*radialPosition;

}



double TRT_ToT_dEdx::fitFuncBarrel_corrRZL(double driftRadius,double zPosition, int Layer, int Strawlayer,  bool data)const {
   /*
     T(r,z) = T0(r)+ b(r)*z*z 
   */
 double a,b,c,d,e,f,g;  

  if(Layer==0 && Strawlayer<9){ // short straws
    if(data){
      a = Dedxcorrection::para_short_corrRZL_DATA[(0)*9+Strawlayer];
      b = Dedxcorrection::para_short_corrRZL_DATA[(1)*9+Strawlayer];
      c = Dedxcorrection::para_short_corrRZL_DATA[(2)*9+Strawlayer];
      d = Dedxcorrection::para_short_corrRZL_DATA[(3)*9+Strawlayer];
      e = Dedxcorrection::para_short_corrRZL_DATA[(4)*9+Strawlayer];
      f = Dedxcorrection::para_short_corrRZL_DATA[(5)*9+Strawlayer];
      g = Dedxcorrection::para_short_corrRZL_DATA[(6)*9+Strawlayer];
    }else{
      a = Dedxcorrection::para_short_corrRZL_MC[(0)*9+Strawlayer];
      b = Dedxcorrection::para_short_corrRZL_MC[(1)*9+Strawlayer];
      c = Dedxcorrection::para_short_corrRZL_MC[(2)*9+Strawlayer];
      d = Dedxcorrection::para_short_corrRZL_MC[(3)*9+Strawlayer];
      e = Dedxcorrection::para_short_corrRZL_MC[(4)*9+Strawlayer];
      f = Dedxcorrection::para_short_corrRZL_MC[(5)*9+Strawlayer];
      g = Dedxcorrection::para_short_corrRZL_MC[(6)*9+Strawlayer];
    }
    
  }else{
    if(data){
      a = Dedxcorrection::para_long_corrRZL_DATA[(0)*30*3+Layer*30+Strawlayer];
      b = Dedxcorrection::para_long_corrRZL_DATA[(1)*30*3+Layer*30+Strawlayer];
      c = Dedxcorrection::para_long_corrRZL_DATA[(2)*30*3+Layer*30+Strawlayer];
      d = Dedxcorrection::para_long_corrRZL_DATA[(3)*30*3+Layer*30+Strawlayer];
      e = Dedxcorrection::para_long_corrRZL_DATA[(4)*30*3+Layer*30+Strawlayer];
      f = Dedxcorrection::para_long_corrRZL_DATA[(5)*30*3+Layer*30+Strawlayer];
      g = Dedxcorrection::para_long_corrRZL_DATA[(6)*30*3+Layer*30+Strawlayer];
    }else{
      a = Dedxcorrection::para_long_corrRZL_MC[(0)*30*3+Layer*30+Strawlayer];
      b = Dedxcorrection::para_long_corrRZL_MC[(1)*30*3+Layer*30+Strawlayer];
      c = Dedxcorrection::para_long_corrRZL_MC[(2)*30*3+Layer*30+Strawlayer];
      d = Dedxcorrection::para_long_corrRZL_MC[(3)*30*3+Layer*30+Strawlayer];
      e = Dedxcorrection::para_long_corrRZL_MC[(4)*30*3+Layer*30+Strawlayer];
      f = Dedxcorrection::para_long_corrRZL_MC[(5)*30*3+Layer*30+Strawlayer];
      g = Dedxcorrection::para_long_corrRZL_MC[(6)*30*3+Layer*30+Strawlayer];
    }
  }
  double z = fabs(zPosition);
  double r = fabs(driftRadius);
  double T0neg=a;
  double T0pos=b;  
  double T1 = exp(-c*r*r)+d*r;
  double slope = e*r+f*r*r+g*r*r*r;  
  double result;
  result = T0neg+T1+slope*z;
  if(zPosition>0)result = T0pos+T1+slope*z;

  return result;

}



double TRT_ToT_dEdx::getToTlargerIsland(unsigned int BitPattern) const {

   unsigned long mask = 0x02000000;
   unsigned int best_length = 0;
   unsigned int current_length = 0;
   unsigned int k = 0;

   //shift bitmask to the right until end;
   while (true) {
     if (BitPattern & mask) {
       ++current_length;
     }
     else {
       // remember longest island
       if (current_length > best_length)
         best_length = current_length;
       current_length = 0;
     }
     if (!mask)
       break;
     assert(k < 24);
     mask >>= 1;
     if (k == 7 || k == 15)
       mask >>= 1;
     ++k;
   }
   assert(k == 24);
   return best_length*3.125; 

}



double TRT_ToT_dEdx::getToTonly1bits(unsigned int BitPattern) const {
  
  /********  Islands ********/
  unsigned  mask2 = 0x02000000;
  int k2;
  int i_island = 0;
  std::vector<int> StartIsland;
  std::vector<int> EndIsland;
  std::vector<int> LengthIsland;
  bool StartedIsland = false;
  bool EndedIsland = true;
  for(k2=0;k2<24;++k2) {
    if ( !(BitPattern & mask2) ) {
      if (StartedIsland) {
	EndIsland.push_back(k2-1);
	LengthIsland.push_back(EndIsland[i_island]-StartIsland[i_island]+1);
	i_island = i_island+1;
	EndedIsland = true;
	StartedIsland = false;
      }
      mask2>>=1;
      if (k2==7 || k2==15) mask2>>=1;
    } else {
      if (EndedIsland) {
	StartIsland.push_back(k2);
	StartedIsland = true;
	EndedIsland = false;
      }
      if (k2==23) {
	EndIsland.push_back(k2);
	LengthIsland.push_back(EndIsland[i_island]-StartIsland[i_island]+1);
      }
      mask2>>=1;
      if (k2==7 || k2==15) mask2>>=1;
    }
  }
  
  // ToT using only bits set to 1 (remove all 0)
  double ToT_only1bits = 0;
  for (unsigned int m=0 ; m<StartIsland.size() ; m++) {
    ToT_only1bits = ToT_only1bits + LengthIsland[m]*3.125;
  }
  
  return ToT_only1bits;
}

