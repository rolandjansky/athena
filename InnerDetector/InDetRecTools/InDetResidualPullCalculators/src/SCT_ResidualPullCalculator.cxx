/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// SCT_ResidualPullCalculator.cxx
//   Source file for class SCT_ResidualPullCalculator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "SCT_ResidualPullCalculator.h"

#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"


//////////////////////////////
/// constructor
//////////////////////////////
InDet::SCT_ResidualPullCalculator::SCT_ResidualPullCalculator(const std::string& type, const std::string& name, const IInterface* parent)
        : AthAlgTool(type,name,parent) {
    declareInterface<IResidualPullCalculator>(this);
}

///////////////////////////////
/// initialize
///////////////////////////////
StatusCode InDet::SCT_ResidualPullCalculator::initialize() {
    ATH_MSG_DEBUG ("initialise successful in "<<name());
    return StatusCode::SUCCESS;
}

/////////////////////////////////
/// calc residual and pull for SCT measurements
/////////////////////////////////
const Trk::ResidualPull* InDet::SCT_ResidualPullCalculator::residualPull(
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkPar,
    const Trk::ResidualPull::ResidualType resType,
    const Trk::TrackState::MeasurementType) const {

    // check the input:
    const InDet::SCT_ClusterOnTrack *sctROT = dynamic_cast<const InDet::SCT_ClusterOnTrack*>(measurement);
    if (!sctROT) return 0;
    if (!trkPar) return 0;

    // if no covariance for the track parameters is given the pull calculation is not valid
    bool pullIsValid = trkPar->covariance()!=0;

    // residual and pull are always 1-dim in the end
    std::vector<double> residual(1);
    std::vector<double> pull(1);
    double sinAlpha = 0.0;

    // check if we have a 2-dim SCT cluster
    if (sctROT->localParameters().parameterKey() == 1) {
        // the (easy) 1-dim case:
        residual[Trk::loc1] = sctROT->localParameters()[Trk::loc1] - trkPar->parameters()[Trk::loc1];
        if (pullIsValid) {
            pull[Trk::loc1] = calcPull(residual[Trk::loc1],
                                       sctROT->localCovariance()(Trk::loc1,Trk::loc1),
                                       (*trkPar->covariance())(Trk::loc1,Trk::loc1),
                                       resType, pullIsValid);
        } else {
            pull[Trk::loc1] = calcPull(residual[Trk::loc1],
                                       sctROT->localCovariance()(Trk::loc1,Trk::loc1),
                                       0,
                                       resType, pullIsValid);
        }
    } else {
        // the 2-dim case:
        // get the stereo angle (preferrably) from the PrepRawData
        const InDet::SCT_Cluster *sctRIO = sctROT->prepRawData();
        if (!sctRIO) {
            Amg::Vector2D p(sctROT->localParameters()[Trk::locX],
                                 sctROT->localParameters()[Trk::locY]);
            sinAlpha = sctROT->detectorElement()->sinStereoLocal(p);
        } else {
            // Get angle of strip in local frame with respect to the etaAxis
            // the position of the ROT is lorentz-drift corrected by up to 3 micron,
            // leading to a strip frame slightly (~5murad) different than the original one
            sinAlpha = sctRIO->detectorElement()->sinStereoLocal(sctRIO->localPosition());
        }
        double cosAlpha = sqrt(1 - sinAlpha*sinAlpha);

        // Calculate Residual for hit: res = (vec(x_track) - vec(x_hit)) * vec(n_perpendicular)
        residual[Trk::loc1] =
            (measurement->localParameters()[Trk::locX] -
             trkPar->parameters()[Trk::locX]) * cosAlpha
            + (measurement->localParameters()[Trk::locY] -
               trkPar->parameters()[Trk::locY]) * sinAlpha;

        // Fill transformation matrix to transform covariance matrices
	Amg::MatrixX RotMat(2,2);
        RotMat(0,0) = cosAlpha;
        RotMat(0,1) = sinAlpha;
        RotMat(1,0) = -sinAlpha;
        RotMat(1,1) = cosAlpha;
        // get the covariance matrix of the ROT and rotate it by stereo angle
	Amg::MatrixX transformedROTCov = sctROT->localCovariance().similarity(RotMat);

        if (trkPar->covariance()) {
	  // Get local error matrix for track to calc pull
	  // Just use first local coordinates
	  // and rotate by the stereo angle
	  Amg::MatrixX RotMatTrk(2,2);
	  RotMatTrk(0,0) = (*trkPar->covariance())(0,0);
	  RotMatTrk(0,1) = (*trkPar->covariance())(0,1);
	  RotMatTrk(1,0) = (*trkPar->covariance())(1,0);
	  RotMatTrk(1,1) = (*trkPar->covariance())(1,1);

	  Amg::MatrixX transformedTrkCov = RotMatTrk.similarity(RotMat);
            // calc pull now
            pull[Trk::loc1] = calcPull(residual[Trk::loc1],
                                       transformedROTCov(0,0),
                                       transformedTrkCov(0,0),
                                       resType, pullIsValid);
        } else {
            pull[Trk::loc1] = calcPull(residual[Trk::loc1],
                                       transformedROTCov(0,0),
                                       0.,
                                       resType, pullIsValid);
        }

    }

    // create the Trk::ResidualPull:
    // ParameterKey is always 1, because otherwise we rotated it back
    return new Trk::ResidualPull(residual, pull, pullIsValid, resType, 1, sinAlpha);
}


/////////////////////////////////
/// calc residuals for SCT measurements
/////////////////////////////////
void InDet::SCT_ResidualPullCalculator::residuals(
    std::vector<double>& residuals,
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkPar,
    const Trk::ResidualPull::ResidualType,
    const Trk::TrackState::MeasurementType) const {

    // check the input:
    const InDet::SCT_ClusterOnTrack *sctROT = dynamic_cast<const InDet::SCT_ClusterOnTrack*>(measurement);
    if (!sctROT) return;
    if (!trkPar) return;
    if (residuals.size() < 5) residuals.resize(5); // 5 is convention for the interface
    double sinAlpha = 0.0;

    // check if we have a 2-dim SCT cluster
    if (sctROT->localParameters().parameterKey() == 1) {
        // the (easy) 1-dim case:
        residuals[Trk::loc1] = sctROT->localParameters()[Trk::loc1] - trkPar->parameters()[Trk::loc1];
    } else {
        // the 2-dim case:
        // get the stereo angle (preferrably) from the PrepRawData
        const InDet::SCT_Cluster *sctRIO = sctROT->prepRawData();
        if (!sctRIO) {
            Amg::Vector2D p(sctROT->localParameters()[Trk::locX],
                                 sctROT->localParameters()[Trk::locY]);
            sinAlpha = sctROT->detectorElement()->sinStereoLocal(p);
        } else {
            // Get angle of strip in local frame with respect to the etaAxis
            // the position of the ROT is lorentz-drift corrected by up to 3 micron,
            // leading to a strip frame slightly (~5murad) different than the original one
            sinAlpha = sctRIO->detectorElement()->sinStereoLocal(sctRIO->localPosition());
        }
        double cosAlpha = sqrt(1 - sinAlpha*sinAlpha);

        // Calculate Residual for hit: res = (vec(x_track) - vec(x_hit)) * vec(n_perpendicular)
        residuals[Trk::loc1] =
            (measurement->localParameters()[Trk::locX] -
             trkPar->parameters()[Trk::locX]) * cosAlpha
            + (measurement->localParameters()[Trk::locY] -
               trkPar->parameters()[Trk::locY]) * sinAlpha;

    }
}


/////////////////////////////////////////////////////////////////////////////
/// calc pull in 1 dimension
/////////////////////////////////////////////////////////////////////////////
double InDet::SCT_ResidualPullCalculator::calcPull(
    const double residual,
    const double locMesCov,
    const double locTrkCov,
    const Trk::ResidualPull::ResidualType& resType,
    bool&        pullIsValid ) const {

    double ErrorSum;
    if (resType == Trk::ResidualPull::Unbiased) {
      if ((locMesCov + locTrkCov) < 0.) {
        pullIsValid=false;
        return 0;
      }
        ErrorSum = sqrt(locMesCov + locTrkCov);
    } else if (resType == Trk::ResidualPull::Biased) {
      if ((locMesCov - locTrkCov) < 0.) {
        pullIsValid=false;
        return 0;
      }
      ErrorSum = sqrt(locMesCov - locTrkCov);
    } else {
      if (locMesCov < 0.) {
        pullIsValid=false;
        return 0;
      }
      ErrorSum = sqrt(locMesCov);
    }
    if (ErrorSum != 0) {
        return residual/ErrorSum;
    }
    pullIsValid=false;
    return 0;
}
