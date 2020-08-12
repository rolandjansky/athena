/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "TrkMeasurementBase/MeasurementBase.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "xAODTracking/TrackParticle.h"


// Would be nice to use something like Amg::distance instead.
// But that rounds slightly differently.
// Do it like this so that results are identical with the pre-MT version.
namespace {
  inline double square(const double tosquare) {
    return std::pow(tosquare,2);
  }
  double dist(const std::pair<Amg::Vector3D,Amg::Vector3D>& pairofpos) {
    Amg::Vector3D diff(pairofpos.first-pairofpos.second);
    return std::sqrt(square(diff.x())+square(diff.y())+square(diff.z()));
  }
}


namespace InDet {

  // -------------------------------------------------------------
  static const InterfaceID IID_ITrackPairsSelector("InDet::TrackPairsSelector", 1, 0);

  // -------------------------------------------------------------
  TrackPairsSelector::TrackPairsSelector(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_maxR(500.)
    , m_MinTrkAngle(0.)
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
    declareProperty("MaxFirstHitRadius"         , m_maxR);
    declareProperty("MaxDistBetweenTracks"      , m_maxDist);
    declareProperty("MaxEta"                    , m_etaCut   );
    declareProperty("MaxInitDistance"           , m_initCut  );
    declareProperty("MinTrackAngle"             , m_MinTrkAngle);
  }

  // -------------------------------------------------------------
  TrackPairsSelector::~TrackPairsSelector() = default;

  // -------------------------------------------------------------
  const InterfaceID& TrackPairsSelector::interfaceID() {
    return IID_ITrackPairsSelector;
  }

  // -------------------------------------------------------------
  StatusCode TrackPairsSelector::initialize() {

    /* Get the track selector tool from ToolSvc */
    if ( m_helpertool.retrieve().isFailure() ) {
      msg(MSG::ERROR) << "Failed to retrieve tool " << m_helpertool << endmsg;
      return StatusCode::FAILURE;
    }
      msg(MSG::INFO) << "Retrieved tool " << m_helpertool << endmsg;


    /* Get the distance tool from ToolsSvc */
    if(m_distanceTool.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Could not get " << m_distanceTool << endmsg; return StatusCode::FAILURE;
    }
      msg(MSG::INFO) << "Got the distance tool " << m_distanceTool << endmsg;

    msg(MSG::INFO) << "Initialization successful" << endmsg;
    return StatusCode::SUCCESS;
  }

  // -------------------------------------------------------------
  StatusCode TrackPairsSelector::finalize() {
    return StatusCode::SUCCESS;
  }

  // -------------------------------------------------------------
  bool
  TrackPairsSelector::selectTrackParticlePair(
    const xAOD::TrackParticle* trkPpos,
    const xAOD::TrackParticle* trkPneg,
    TrackPairsSelector::Cache& cache) const
  {

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

    cache.m_deltaCotTheta = fabs(1. / tan(perPos->parameters()[Trk::theta]) -
                                 1. / tan(perNeg->parameters()[Trk::theta]));
    if (cache.m_deltaCotTheta > detaCut)
      return false;

    //Cut on distance between the initial hit position of the two tracks.
    double dinit = 1000.;
    if(sCase == 0) {
      dinit = m_initCut[0];
    } else if(sCase == 1) {
      dinit = m_initCut[1];
    } else if(sCase == 2) {
      dinit = m_initCut[2];
    }

    cache.m_deltaInit = fabs(firstRpos - firstRneg);
    if (cache.m_deltaInit > dinit) return false;

    //Cut on distance of minimum approach between the two tracks.
    double maxDist = 1000.;
    if(sCase == 0) {
      maxDist = m_maxDist[0];
    } else if(sCase == 1) {
      maxDist = m_maxDist[1];
    } else if(sCase == 2) {
      maxDist = m_maxDist[2];
    }

    cache.m_distance = 1000000.;
    std::optional<Trk::ITrkDistanceFinder::TwoPoints> result
      = m_distanceTool->CalculateMinimumDistance(trkPneg->perigeeParameters(),
                                                 trkPpos->perigeeParameters() );
    if (!result) return false;
    cache.m_distance = dist (result.value());
    if (cache.m_distance>maxDist) return false;

    //3D angle cut in the case of V0s, not used in the case of conversions
    double d_beta = (perPos->momentum().dot(perNeg->momentum())) /
                    (perPos->momentum().mag() * perNeg->momentum().mag());
    if(d_beta <m_MinTrkAngle) pass = false;

    return pass;
  }

  // -------------------------------------------------------------
  bool
  TrackPairsSelector::selectTrackPair(const Trk::Track* trkpos,
                                      const Trk::Track* trkneg) const
  {

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

    if (fabs(1. / tan(perPos->parameters()[Trk::theta]) -
             1. / tan(perNeg->parameters()[Trk::theta])) > detaCut)
      pass = false;

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

    double newDistance = 1000000.;
    std::optional<Trk::ITrkDistanceFinder::TwoPoints> result
      = m_distanceTool->CalculateMinimumDistance(*trkpos, *trkneg);
    if (!result) {
      pass = false;
    }
    else {
      newDistance = dist (result.value());
      if (newDistance>maxDist) pass = false;
    }

    //3D angle cut in the case of V0s, not used in the case of conversions
    double d_beta = (perPos->momentum().dot(perNeg->momentum())) /
                    (perPos->momentum().mag() * perNeg->momentum().mag());
    if(d_beta <m_MinTrkAngle) pass = false;

    return pass;
  }

  // -------------------------------------------------------------
  std::map<std::string, float>
  TrackPairsSelector::getLastValues(
    const TrackPairsSelector::Cache& cache) const
  {
    return {{"minimumDistanceTrk", cache.m_distance},
            {"deltaCotThetaTrk", cache.m_deltaCotTheta},
            {"deltaInitRadius", cache.m_deltaInit} };
  }

} // namespace InDet




