/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TGC_ResidualPullCalculator.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "EventPrimitives/EventPrimitives.h"

//================ Constructor =================================================

Muon::TGC_ResidualPullCalculator::TGC_ResidualPullCalculator(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p) {
  declareInterface<IResidualPullCalculator>(this);
}

//================ Initialisation =================================================

StatusCode Muon::TGC_ResidualPullCalculator::initialize()
{
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_MSG_DEBUG ("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ calculate residuals for TGC ==================================
void Muon::TGC_ResidualPullCalculator::residuals(
    std::vector<double>& residuals,
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkPar,
    const Trk::ResidualPull::ResidualType /*resType*/,
    const Trk::TrackState::MeasurementType) const {

  if (residuals.size()<1) residuals.resize(1);
  const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(measurement);
  if (!rot) {
    const Muon::CompetingMuonClustersOnTrack* muonCompClusters =
      dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(measurement);
    if (muonCompClusters) rot = muonCompClusters->containedROTs().empty() ? 0 :
      muonCompClusters->containedROTs().front();
  }
  if (!trkPar || !rot) {
    if( !trkPar ) ATH_MSG_WARNING ("No TrackParameters, cannot calculate residual/pull ");
    if( !rot )    ATH_MSG_WARNING ("No ROT, cannot calculate residual/pull ");
    return;
  }
  Identifier ID = rot->identify();
  
  if( m_idHelperSvc->isTgc(ID) ) {

    double sinAlpha = 0.0;
    
    bool isStrip = m_idHelperSvc->tgcIdHelper().isStrip(ID);

    // calculate residual
    if (isStrip) {
      // consistency check that Muon EDM is as expected
      if (measurement->localParameters().parameterKey() !=3) {
        ATH_MSG_WARNING ( "TGC ClusterOnTrack does not carry the expected "
                          << "LocalParameters structure!");
        return;
      }
      // get orientation angle of strip to rotate back from local frame to strip
      const Amg::MatrixX &covmat=measurement->localCovariance();
      double v0=0.5*(covmat(0,0)+covmat(1,1)-sqrt((covmat(0,0)+covmat(1,1))*(covmat(0,0)+covmat(1,1))-
						    4*(covmat(0,0)*covmat(1,1)-covmat(0,1)*covmat(0,1))));
      double v1=0.5*(covmat(0,0)+covmat(1,1)+sqrt((covmat(0,0)+covmat(1,1))*(covmat(0,0)+covmat(1,1))-
						    4*(covmat(0,0)*covmat(1,1)-covmat(0,1)*covmat(0,1))));
      sinAlpha=sin(0.5*asin(2*covmat(0,1)/(v0-v1)));

      double cosAlpha = sqrt(1 - sinAlpha*sinAlpha);

      // Calculate Residual for hit: res = (vec(x_hit) - vec(x_track)) * vec(n_perpendicular)
      residuals[Trk::loc1] =
	(measurement->localParameters()[Trk::locX] - trkPar->parameters()[Trk::locX]) * cosAlpha
	+ (measurement->localParameters()[Trk::locY] - trkPar->parameters()[Trk::locY]) * sinAlpha;
      
    } else {
      if (measurement->localParameters().parameterKey() != 1) {
        ATH_MSG_WARNING ("TGC ClusterOnTrack does not carry the expected "
                         << "LocalParameters structure!" );
        return;
      } else {
        // convention to be interpreted by TrkValTools: 2nd coordinate codes orientation of TGC
        residuals[Trk::loc1] = measurement->localParameters()[Trk::loc1]
          - trkPar->parameters()[Trk::loc1];
      }
    }
  } else {
    ATH_MSG_DEBUG ( "Input problem measurement is not TGC." );
    return;
  }
}

//================ calculate residuals and pulls for TGC ==================================
const Trk::ResidualPull* Muon::TGC_ResidualPullCalculator::residualPull(
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkPar,
    const Trk::ResidualPull::ResidualType resType,
    const Trk::TrackState::MeasurementType) const {

  const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(measurement);
  if (!rot) {
    const Muon::CompetingMuonClustersOnTrack* muonCompClusters =
      dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(measurement);
    if (muonCompClusters) rot = muonCompClusters->containedROTs().empty() ? 0 :
      muonCompClusters->containedROTs().front();
  }
  if (!trkPar || !rot) {
    if( !trkPar ) ATH_MSG_WARNING ("No TrackParameters, cannot calculate residual/pull ");
    if( !rot )    ATH_MSG_WARNING ("No ROT, cannot calculate residual/pull ");
    return nullptr;
  }
  Identifier ID = rot->identify();
  
  if( m_idHelperSvc->isTgc(ID) ) {

    // try to cast the track parameters to measured ones
    const AmgSymMatrix(5)* trkCov = trkPar->covariance();
    bool pullIsValid = (trkCov);

    double sinAlpha = 0.0;
    
    bool isStrip = m_idHelperSvc->tgcIdHelper().isStrip(ID);

    // calculate residual
    std::vector<double> residual(1);
    std::vector<double> pull(1);
    if (isStrip) {
      // consistency check that Muon EDM is as expected
      if (measurement->localParameters().parameterKey() !=3) {
        ATH_MSG_WARNING ( "TGC ClusterOnTrack does not carry the expected "
                          << "LocalParameters structure!");
        return nullptr;
      }
      // get orientation angle of strip to rotate back from local frame to strip
      const Amg::MatrixX &covmat=measurement->localCovariance();
      double v0=0.5*(covmat(0,0)+covmat(1,1)-sqrt((covmat(0,0)+covmat(1,1))*(covmat(0,0)+covmat(1,1))-
						    4*(covmat(0,0)*covmat(1,1)-covmat(0,1)*covmat(0,1))));
      double v1=0.5*(covmat(0,0)+covmat(1,1)+sqrt((covmat(0,0)+covmat(1,1))*(covmat(0,0)+covmat(1,1))-
						    4*(covmat(0,0)*covmat(1,1)-covmat(0,1)*covmat(0,1))));
      sinAlpha=sin(0.5*asin(2*covmat(0,1)/(v0-v1)));

      const MuonGM::TgcReadoutElement *ele = 
        dynamic_cast<const MuonGM::TgcReadoutElement*>(rot->detectorElement());
      if (!ele) {
        ATH_MSG_WARNING ("Could not obtain TGC detEl from TGC ROT, this is a bug!" );
        return nullptr;
      }

      double cosAlpha = sqrt(1 - sinAlpha*sinAlpha);

      // Calculate Residual for hit: res = (vec(x_hit) - vec(x_track)) * vec(n_perpendicular)
      residual[Trk::loc1] =
	(measurement->localParameters()[Trk::locX] - trkPar->parameters()[Trk::locX]) * cosAlpha
	+ (measurement->localParameters()[Trk::locY] - trkPar->parameters()[Trk::locY]) * sinAlpha;
      
      // Fill transformation matrix to transform covariance matrices
      Amg::MatrixX RotMat(2,2);
      RotMat(0,0) = cosAlpha;
      RotMat(0,1) = sinAlpha;
      RotMat(1,0) = -sinAlpha;
      RotMat(1,1) = cosAlpha;
      // get the covariance matrix of the ROT and rotate it by stereo angle
      Amg::MatrixX transformedROTCov = measurement->localCovariance().similarity(RotMat);

      if (pullIsValid) {
        // Get local error matrix for track to calc pull
        // Just use first local coordinates
        // and rotate by the stereo angle
	Amg::MatrixX subm = trkCov->block<2,2>(0,0);
	Amg::MatrixX transformedTrkCov = subm.similarity(RotMat);
        // calc pull now
        pull[Trk::loc1] = calcPull(residual[Trk::loc1],
                                   transformedROTCov(0,0),
                                   transformedTrkCov(0,0),
                                   resType);
      } else {
        pull[Trk::loc1] = calcPull(residual[Trk::loc1],
                                   transformedROTCov(0,0),
                                   0.,
                                   resType);
      }

    } else {
      if (measurement->localParameters().parameterKey() != 1) {
        ATH_MSG_WARNING ("TGC ClusterOnTrack does not carry the expected "
                         << "LocalParameters structure!" );
        return nullptr;
      } else {
        // convention to be interpreted by TrkValTools: 2nd coordinate codes orientation of TGC
        residual[Trk::loc1] = measurement->localParameters()[Trk::loc1]
          - trkPar->parameters()[Trk::loc1];
        // calculate pull
        if (pullIsValid)
          pull[Trk::loc1] = calcPull(residual[Trk::loc1],
                                     measurement->localCovariance()(Trk::loc1,Trk::loc1),
                                     (*trkCov)(Trk::loc1,Trk::loc1),
                                     resType);
        else 
          pull[Trk::loc1] = calcPull(residual[Trk::loc1],
                                     measurement->localCovariance()(Trk::loc1,Trk::loc1),
                                     0,
                                     resType);
      }
    }
    // create the Trk::ResidualPull.
    ATH_MSG_VERBOSE ( "Calculating Pull for channel " << m_idHelperSvc->toString(ID) << " residual " << residual[Trk::loc1] << " pull " << pull[Trk::loc1] );

    return new Trk::ResidualPull(residual, pull, pullIsValid, resType, 1, sinAlpha);

  } else {
    ATH_MSG_DEBUG ( "Input problem measurement is not TGC." );
    return nullptr;
  }
}

/////////////////////////////////////////////////////////////////////////////
/// calc pull in 1 dimension
/////////////////////////////////////////////////////////////////////////////
double Muon::TGC_ResidualPullCalculator::calcPull(
    const double residual,
    const double locMesCov,
    const double locTrkCov,
    const Trk::ResidualPull::ResidualType& resType ) const {
    if( locMesCov < 0 ) {
      ATH_MSG_WARNING("Bad ERROR " << locMesCov << "  " << locTrkCov << " using measured error ");
      return 0;
    }
    double ErrorSum;
    if (resType == Trk::ResidualPull::Unbiased) {
      if( locMesCov + locTrkCov > 0 ) ErrorSum = sqrt(locMesCov + locTrkCov);
      else{
	ATH_MSG_WARNING("Bad ERROR: measurement " << locMesCov << "  from track " << locTrkCov << ", using measured error ");
        ErrorSum = sqrt(locMesCov);
      } 
    } else if (resType == Trk::ResidualPull::Biased) {
        if ((locMesCov - locTrkCov) < 0.) {
            return 0;
        }
        ErrorSum = sqrt(locMesCov - locTrkCov);
    } else ErrorSum = sqrt(locMesCov);
    if (ErrorSum != 0) return residual/ErrorSum;
    return 0;
}
