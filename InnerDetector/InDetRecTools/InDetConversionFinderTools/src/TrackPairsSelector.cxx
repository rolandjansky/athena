/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                TrackPairsSelector.cxx  -  Description
		------------------
		begin   : 01-01-2008
		authors : Tatjana Lenz, Thomas Koffas
		email   : tatjana.lenz@cern.ch, Thomas.Koffas@cern.ch
		changes : M.Elsing
***************************************************************************/
#include "InDetConversionFinderTools/TrackPairsSelector.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"
#include "TrkVertexSeedFinderUtils/ITrkDistanceFinder.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "xAODTracking/TrackParticle.h"


namespace InDet {

  // -------------------------------------------------------------
  static const InterfaceID IID_ITrackPairsSelector("InDet::TrackPairsSelector", 1, 0);
  
  // -------------------------------------------------------------
  TrackPairsSelector::TrackPairsSelector(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_helpertool("InDet::ConversionFinderUtils"),
    m_distanceTool("Trk::SeedNewtonDistanceFinder/InDetConversionTrkDistanceFinder"),
    m_maxR(500.),
    m_MinTrkAngle(0.),
    m_distance(9999.),
    m_deltaCotTheta(9999.),
    m_deltaInit(9999.)
  {
    m_etaCut.push_back(0.8);
    m_etaCut.push_back(1.2);
    m_etaCut.push_back(1.2);
    
    m_initCut.push_back(10000.);
    m_initCut.push_back(10000.);
    m_initCut.push_back(10000.);
    
    m_maxDist.push_back(8.);
    m_maxDist.push_back(80.);
    m_maxDist.push_back(45.);

    declareInterface<TrackPairsSelector>(this);
    declareProperty("ConversionFinderHelperTool", m_helpertool);
    declareProperty("DistanceTool"              , m_distanceTool);
    declareProperty("MaxFirstHitRadius"         , m_maxR);
    declareProperty("MaxDistBetweenTracks"      , m_maxDist);
    declareProperty("MaxEta"                    , m_etaCut   );
    declareProperty("MaxInitDistance"           , m_initCut  );
    declareProperty("MinTrackAngle"             , m_MinTrkAngle);
  }

  // -------------------------------------------------------------
  TrackPairsSelector::~TrackPairsSelector() {}

  // -------------------------------------------------------------
  const InterfaceID& TrackPairsSelector::interfaceID() {
    return IID_ITrackPairsSelector;
  }

  // -------------------------------------------------------------
  StatusCode TrackPairsSelector::initialize() {

    /* Get the track selector tool from ToolSvc */
    if ( m_helpertool.retrieve().isFailure() ) {
      msg(MSG::ERROR) << "Failed to retrieve tool " << m_helpertool << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_helpertool << endreq;
    }

    /* Get the distance tool from ToolsSvc */
    if(m_distanceTool.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Could not get " << m_distanceTool << endreq; return StatusCode::FAILURE;
    }else{
      msg(MSG::INFO) << "Got the distance tool " << m_distanceTool << endreq;
    }
    msg(MSG::INFO) << "Initialization successful" << endreq;
    return StatusCode::SUCCESS;
  }
  
  // -------------------------------------------------------------
  StatusCode TrackPairsSelector::finalize() {
    return StatusCode::SUCCESS;
  }

  // -------------------------------------------------------------
  bool TrackPairsSelector::selectTrackParticlePair(const xAOD::TrackParticle* trkPpos, const xAOD::TrackParticle* trkPneg) {

    bool pass = true;
    //Getting the track perigee parameters
    const Trk::TrackParameters* perPos = &(trkPpos->perigeeParameters());
    const Trk::TrackParameters* perNeg = &(trkPneg->perigeeParameters());
    if (!(m_helpertool->momFraction(perPos, perNeg))) pass = false;
    
    //Track summary information
    
    
    uint8_t nclusPos(0);
    uint8_t dummy(0);
    if(trkPpos->summaryValue(dummy,xAOD::numberOfSCTHits)){
      nclusPos += dummy;
    } 
    if(trkPpos->summaryValue(dummy,xAOD::numberOfPixelHits)){
      nclusPos += dummy;
    } 
  
    uint8_t nclusNeg(0);
    if(trkPneg->summaryValue(dummy,xAOD::numberOfSCTHits)){
      nclusNeg += dummy;
    } 
    if(trkPneg->summaryValue(dummy,xAOD::numberOfPixelHits)){
      nclusNeg += dummy;
    } 
  
    
    int sCase = 100;
    if(nclusNeg>0 && nclusPos>0)                                   sCase = 0;
    if((nclusNeg>0 && nclusPos==0) || (nclusNeg==0 && nclusPos>0)) sCase = 1;
    if(nclusNeg==0 && nclusPos==0)                                 sCase = 2;
    
    //Position of first hit in track particle
    Trk::CurvilinearParameters parPos; 
    Trk::CurvilinearParameters parNeg; 

    int index(-1);
    for(unsigned int i(0); i< trkPpos->numberOfParameters() ; ++i ){
      if( xAOD::FirstMeasurement == trkPpos->parameterPosition(i) ){
        index = i;
        break;
      }
    }
    if(index!=-1){
      parPos = trkPpos->curvilinearParameters(index);
    } else {
      ATH_MSG_WARNING("Track Particle does not contain first Measurement track parameters");
      return false;
    }
    
    index = -1;
    for(unsigned int i(0); i< trkPneg->numberOfParameters() ; ++i ){
      if( xAOD::FirstMeasurement == trkPneg->parameterPosition(i) ){
        index = i;
        break;
      }
    }
    if(index!=-1){
      parNeg = trkPneg->curvilinearParameters(index);
    } else {
      ATH_MSG_WARNING("Track Particle does not contain first Measurement track parameters");
      return false;
    }

    
    
//  Need to work out a way to do this elegently
//    if(!parPos || !parNeg) {pass = false; return pass;}
    double firstRpos = parPos.position().perp();
    double firstRneg = parNeg.position().perp();

    //Cut on Deta
    double detaCut = 0.0;
    if(sCase == 0) {
      detaCut = m_etaCut[0];
    } else if(sCase == 1) {
      detaCut = m_etaCut[1];
    } else if(sCase == 2) {
      if(fabs(perPos->eta())<0.6 && fabs(perNeg->eta())<0.6) detaCut = 10000.; // No eta cut for barrel TRT tracks
      else                                                   detaCut = m_etaCut[2];
    }

    m_deltaCotTheta = fabs(1./tan(perPos->parameters()[Trk::theta]) - 1./tan(perNeg->parameters()[Trk::theta]));
    if (m_deltaCotTheta > detaCut) return false;
    
    //Cut on distance between the initial hit position of the two tracks.
    double dinit = 1000.;
    if(sCase == 0) {
      dinit = m_initCut[0];
    } else if(sCase == 1) {
      dinit = m_initCut[1];
    } else if(sCase == 2) {
      dinit = m_initCut[2];
    }
    
    m_deltaInit = fabs(firstRpos - firstRneg);
    if (m_deltaInit > dinit) return false;
    
    //Cut on distance of minimum approach between the two tracks.
    double maxDist = 1000.;
    if(sCase == 0) {
      maxDist = m_maxDist[0];
    } else if(sCase == 1) {
      maxDist = m_maxDist[1];
    } else if(sCase == 2) {
      maxDist = m_maxDist[2];
    }

    bool gotDistance = false; m_distance = 1000000.;
    gotDistance = m_distanceTool->CalculateMinimumDistance(trkPneg->perigeeParameters(),trkPpos->perigeeParameters() );
    if (gotDistance) m_distance = m_distanceTool->GetDistance();
    if (!gotDistance || (m_distance>maxDist)) return false;
    
    //3D angle cut in the case of V0s, not used in the case of conversions
    double d_beta = (perPos->momentum().dot(perNeg->momentum()))/(perPos->momentum().mag()*perNeg->momentum().mag());
    if(d_beta <m_MinTrkAngle) pass = false;
    
    return pass;
  }
  
  // -------------------------------------------------------------
  bool TrackPairsSelector::selectTrackPair(const Trk::Track* trkpos, const Trk::Track* trkneg) {

    bool pass = true;
    ///Getting the track perigee parameters
    const Trk::Perigee* perPos = trkpos->perigeeParameters();
    const Trk::Perigee* perNeg = trkneg->perigeeParameters();
    if (!(m_helpertool->momFraction(perPos, perNeg))) pass = false;
    
    ///Position of initial hit of the two tracks
    double init_pos = 0.; double init_neg = 0.;
    const DataVector<const Trk::MeasurementBase>* mb_pos = trkpos->measurementsOnTrack();
    const DataVector<const Trk::MeasurementBase>* mb_neg = trkneg->measurementsOnTrack();
    if(!mb_pos || !mb_neg) {pass = false; return pass;}
    DataVector<const Trk::MeasurementBase>::const_iterator itp_pos=mb_pos->begin();
    init_pos = (*itp_pos)->globalPosition().perp();
    DataVector<const Trk::MeasurementBase>::const_iterator itp_neg=mb_neg->begin();
    init_neg = (*itp_neg)->globalPosition().perp();
    int sCase = 100;
    if (init_neg<=m_maxR && init_pos<=m_maxR)  sCase = 0;
    if ((init_neg<=m_maxR && init_pos>m_maxR) || (init_neg>m_maxR && init_pos<=m_maxR))	sCase = 1;
    if (init_neg>m_maxR && init_pos>m_maxR)	sCase = 2;
    
    //Cut on Deta
    double detaCut = 0.0;
    if(sCase == 0) {
      detaCut = m_etaCut[0];
    } else if(sCase == 1) {
      detaCut = m_etaCut[1];
    } else if(sCase == 2) {
      detaCut = m_etaCut[2];
    }

    if(fabs(1./tan(perPos->parameters()[Trk::theta]) - 1./tan(perNeg->parameters()[Trk::theta])) > detaCut) pass = false;

    //Cut on distance between the initial hit position of the two tracks.
    double dinit = 1000.;
    if(sCase == 0) {
      dinit = m_initCut[0];
    } else if(sCase == 1) {
      dinit = m_initCut[1];
    } else if(sCase == 2) {
      dinit = m_initCut[2];
    }
    if(fabs(init_pos - init_neg) > dinit) pass = false;

    //Cut on distance of minimum approach between the two tracks.
    double maxDist = 1000.;
    if(sCase == 0) {
      maxDist = m_maxDist[0];
    } else if(sCase == 1) {
      maxDist = m_maxDist[1];
    } else if(sCase == 2) {
      maxDist = m_maxDist[2];
    }
    bool gotDistance = false; double newDistance = 1000000.;
    gotDistance = m_distanceTool->CalculateMinimumDistance(*trkpos,*trkneg);
    if (gotDistance) newDistance = m_distanceTool->GetDistance();
    if (!gotDistance || (newDistance>maxDist)) pass = false;
    
    //3D angle cut in the case of V0s, not used in the case of conversions
    double  d_beta = (perPos->momentum().dot(perNeg->momentum()))/(perPos->momentum().mag()*perNeg->momentum().mag());
    if(d_beta <m_MinTrkAngle) pass = false;
    
    return pass;
  }

  // -------------------------------------------------------------
  std::map<std::string, float> TrackPairsSelector::getLastValues()
  {
    return {{"minimumDistanceTrk", m_distance},
            {"deltaCotThetaTrk", m_deltaCotTheta},
            {"deltaInitRadius", m_deltaInit} };
  }

} // namespace InDet




