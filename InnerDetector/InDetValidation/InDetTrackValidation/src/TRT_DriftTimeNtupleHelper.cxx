/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TRT_DriftTimeNtupleHelper.cxx
//   Source file for class TRT_DriftTimeNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// John.Alison@cern.ch Sebastian.Fleischmann@cern.ch
////////////////////////////////////////////////////////////////////


//Gaudi
//#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "TTree.h"

// Trk
#include "InDetTrackValidation/TRT_DriftTimeNtupleHelper.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkMeasurementBase/MeasurementBase.h"

//#include "TrkEventPrimitives/LocalPosition.h"
//#include "TrkEventPrimitives/LocalParameters.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
//#include "TrkEventPrimitives/CovarianceMatrix.h"
//#include "TrkParameters/MeasuredTrackParameters.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

//#include "TrkParameters/MeasuredAtaStraightLine.h"


#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

#include "InDetIdentifier/TRT_ID.h"

// For the t0 and r(t) relations
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "TRT_ConditionsData/RtRelation.h"

//To get the chip level info
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"

 #include "EventPrimitives/EventPrimitivesHelpers.h"

// constructor
InDet::TRT_DriftTimeNtupleHelper::TRT_DriftTimeNtupleHelper(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
AthAlgTool(t,n,p),
m_trtID(0),
m_trtCondDbSvc("TRT_CalDbSvc",n),
m_TRTStrawNeighbourSvc("TRT_StrawNeighbourSvc", n),
m_Trt_R(0),
m_Trt_R_Err(0),
m_Trt_Rtrack(0),
m_Trt_Rtrack_Err(0),
m_Trt_DTtrack(0),
m_Trt_t(0),
m_Trt_tot(0),
m_Trt_bitpattern(0),
m_Trt_HL(0),
m_Trt_strawlayer(0),
m_Trt_layer(0),
m_Trt_straw(0),
m_Trt_phi(0),
m_Trt_bec(0),
m_Trt_chip(0),
m_Trt_pad(0) {
  declareInterface<IValidationNtupleHelperTool>(this);
  // Declare the properties
  // declareProperty("BookNewNtuple", m_bookNewNtuple = true);
  declareProperty("CalDBSvc", m_trtCondDbSvc);  
  declareProperty("TRT_StrawNeighbourSvc",m_TRTStrawNeighbourSvc);
}

// destructor
InDet::TRT_DriftTimeNtupleHelper::~TRT_DriftTimeNtupleHelper() {}




// initialize
StatusCode InDet::TRT_DriftTimeNtupleHelper::initialize() {

    if (detStore()->retrieve(m_trtID, "TRT_ID").isFailure()) {
      ATH_MSG_FATAL("Could not get TRT ID helper");
      return StatusCode::FAILURE;
    }
    
    if(StatusCode::SUCCESS!=m_trtCondDbSvc.retrieve() ) {
      ATH_MSG_FATAL("Could not get TRTCalDbSvc !");
      return StatusCode::FAILURE;
    } 

    if(StatusCode::SUCCESS!=m_TRTStrawNeighbourSvc.retrieve()){
      ATH_MSG_FATAL("Could not get StrawNeighbourSvc!");
      return StatusCode::FAILURE;
    } 

    m_Trt_R         = new std::vector<float>();
    m_Trt_R_Err         = new std::vector<float>();
    m_Trt_Rtrack    = new std::vector<float>();
    m_Trt_Rtrack_Err    = new std::vector<float>();
    m_Trt_DTtrack    = new std::vector<float>();
    m_Trt_t         = new std::vector<float>();
    m_Trt_tot       = new std::vector<float>();
    m_Trt_bitpattern= new std::vector<unsigned int>();
    m_Trt_HL        = new std::vector<int>();
    m_Trt_layer     = new std::vector<int>();
    m_Trt_straw     = new std::vector<int>();
    m_Trt_phi       = new std::vector<int>();
    m_Trt_strawlayer= new std::vector<int>();
    m_Trt_bec       = new std::vector<int>();
    m_Trt_chip       = new std::vector<int>();
    m_Trt_pad       = new std::vector<int>();

    return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// addNtupleItems
///////////////////////////////////////
StatusCode InDet::TRT_DriftTimeNtupleHelper::addNtupleItems (TTree* tree,
							     const int& detectorType ) const {
  // -------------------------------------------
  // add items to the ntuple
  if (detectorType==Trk::TrackState::TRT) {
    // add items
    tree->Branch("TRT_R",           &m_Trt_R);
    tree->Branch("TRT_R_Err",           &m_Trt_R_Err);
    tree->Branch("TRT_R_track",     &m_Trt_Rtrack);
    tree->Branch("TRT_R_track_Err",     &m_Trt_Rtrack_Err);
    tree->Branch("TRT_DT_track",    &m_Trt_DTtrack);
    tree->Branch("TRT_t",           &m_Trt_t);
    tree->Branch("TRT_tot",         &m_Trt_tot);
    tree->Branch("TRT_bitpattern",  &m_Trt_bitpattern);
    tree->Branch("TRT_HL",          &m_Trt_HL);
    tree->Branch("TRT_strawlayer",  &m_Trt_strawlayer);
    tree->Branch("TRT_layer",       &m_Trt_layer);
    tree->Branch("TRT_straw",       &m_Trt_straw);
    tree->Branch("TRT_phi",         &m_Trt_phi);
    tree->Branch("TRT_bec",         &m_Trt_bec);
    tree->Branch("TRT_chip",        &m_Trt_chip);
    tree->Branch("TRT_pad",         &m_Trt_pad);
  }
  ATH_MSG_VERBOSE( "added items to ntuple" );
  return StatusCode::SUCCESS;
}


///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode InDet::TRT_DriftTimeNtupleHelper::finalize() {
    // delete all vectors
    delete m_Trt_R;
    m_Trt_R = 0;
    delete m_Trt_R_Err;
    m_Trt_R_Err = 0;
    delete m_Trt_Rtrack;
    m_Trt_Rtrack = 0;
    delete m_Trt_Rtrack_Err;
    m_Trt_Rtrack_Err = 0;
    delete m_Trt_DTtrack;
    m_Trt_DTtrack = 0;
    delete m_Trt_t;
    m_Trt_t = 0;
    delete m_Trt_tot;
    m_Trt_tot = 0;
    delete m_Trt_bitpattern;
    m_Trt_bitpattern = 0;
    delete m_Trt_HL;
    m_Trt_HL = 0;
    delete m_Trt_layer;
    m_Trt_layer = 0;
    delete m_Trt_straw;
    m_Trt_straw = 0;
    delete m_Trt_phi;
    m_Trt_phi = 0;
    delete m_Trt_strawlayer;
    m_Trt_strawlayer = 0;
    delete m_Trt_bec;
    m_Trt_bec = 0;
    delete m_Trt_chip;
    m_Trt_chip = 0;
    delete m_Trt_pad;
    m_Trt_pad = 0;

    ATH_MSG_INFO( "finalize() successful in " << name() );
    return StatusCode::SUCCESS;
}

/// fill residual data
StatusCode InDet::TRT_DriftTimeNtupleHelper::fillMeasurementData (const Trk::MeasurementBase* measurement,
								  const Trk::TrackParameters* trkParameters,
								  const int&,
								  const bool& ) const {

    const InDet::TRT_DriftCircleOnTrack* trtcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*> (measurement);
    
    if (trtcircle) {

        ATH_MSG_DEBUG("in fillMeasurementData, have drift radius = "<< trtcircle->localParameters()[Trk::driftRadius]);
        
	// Get driftradius and drift time
        m_Trt_R->push_back( trtcircle->localParameters()[Trk::driftRadius] );
	//m_Trt_R_Err->push_back( trtcircle->localErrorMatrix().error(Trk::driftRadius) );

	m_Trt_R_Err->push_back( Amg::error(trtcircle->localCovariance(),Trk::driftRadius) );
	//m_Trt_Status->push_back( trtcircle->status() );
        m_Trt_HL->push_back( trtcircle->highLevel() );

	float tot = trtcircle->timeOverThreshold();
	m_Trt_tot->push_back( tot );
	unsigned int bitpattern = trtcircle->prepRawData()->getWord();
	m_Trt_bitpattern->push_back( bitpattern );

        // Get prepRawData for drifttime
        const InDet::TRT_DriftCircle *RawDriftCircle=dynamic_cast<const InDet::TRT_DriftCircle*>(trtcircle->prepRawData());
        // Get Straw identifier
	Identifier DCId = trtcircle->identify();
		

	if (RawDriftCircle) {
            bool isvalid;
            float dt = RawDriftCircle->driftTime(isvalid);
	    float t0 =  m_trtCondDbSvc->getT0(DCId);
	    if (!isvalid) {
                ATH_MSG_DEBUG( "The retrieved driftTime is not valid" );
		m_Trt_t->push_back( -999. );
            } else {
	      m_Trt_t->push_back( dt - t0 );
            }
            
            
        } else {
	  ATH_MSG_WARNING( "prepRawData (TRT_DriftCircle) for TRT_DriftCircleOnTrack == Null" );
	  m_Trt_t->push_back( -999. );
	}
        
	// Get info in which straw the hit was
	m_Trt_straw->push_back( m_trtID->straw(DCId) );
        m_Trt_strawlayer->push_back( m_trtID->straw_layer(DCId) );
        m_Trt_layer->push_back( m_trtID->layer_or_wheel(DCId) );
        m_Trt_phi->push_back( m_trtID->phi_module(DCId) );
        m_Trt_bec->push_back( m_trtID->barrel_ec(DCId) );

	int m_chip;
	m_TRTStrawNeighbourSvc->getChip(DCId, m_chip);
	m_Trt_chip->push_back(m_chip);

	int m_pad;
	m_TRTStrawNeighbourSvc->getPad(DCId, m_pad);
	m_Trt_pad->push_back(m_pad);

        // Fill local R track prediction and DT drift time prediction
        if (trkParameters) {
	  m_Trt_Rtrack->push_back( trkParameters->parameters()[Trk::locR] );
	  
	  //Trk::MeasuredTrackParameters const* measParam = dynamic_cast<Trk::MeasuredTrackParameters const*>(trkParameters);
	  //if(measParam){
	  //m_Trt_Rtrack_Err->push_back( measParam->localErrorMatrix().error(Trk::locR) );
	  const AmgSymMatrix(5) * merr = trkParameters->covariance();
	  if(merr){
	    m_Trt_Rtrack_Err->push_back( Amg::error(*merr,Trk::locR) );  
	  }else{
	    ATH_MSG_INFO( "No measured Parameters");
	    m_Trt_Rtrack_Err->push_back( -999 );
	  }
	  
	  const TRTCond::RtRelation *rtr = m_trtCondDbSvc->getRtRelation(DCId);
	  if(rtr) 
	    m_Trt_DTtrack->push_back( rtr->drifttime( fabs(trkParameters->parameters()[Trk::locR]) ) );
	  else
	    m_Trt_DTtrack->push_back( -999.0 );

	} else {
	  
	  ATH_MSG_DEBUG("no trkParameters associated to hit ");
	  m_Trt_Rtrack->push_back( -999.0 );
	  m_Trt_Rtrack_Err->push_back( -999.0 );
	  m_Trt_DTtrack->push_back( -999.0 );
        }
	
	
    } else {
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

StatusCode InDet::TRT_DriftTimeNtupleHelper::resetVariables (
        const int& detectorType ) const {
    if (detectorType==Trk::TrackState::TRT) {
        m_Trt_R->clear();
        m_Trt_R_Err->clear();
	//m_Trt_Status->clear();
        m_Trt_Rtrack->clear();
	m_Trt_Rtrack_Err->clear();
	m_Trt_DTtrack->clear();
        m_Trt_t->clear();
        m_Trt_tot->clear();
        m_Trt_bitpattern->clear();
	m_Trt_HL->clear();
        m_Trt_layer->clear();
        m_Trt_straw->clear();
        m_Trt_phi->clear();
        m_Trt_bec->clear();
	m_Trt_chip->clear();
	m_Trt_pad->clear();
        m_Trt_strawlayer->clear();
    }
    return StatusCode::SUCCESS;
}

StatusCode InDet::TRT_DriftTimeNtupleHelper::fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int&) const {
    // we do nothing with holes
    return StatusCode::SUCCESS;
}
