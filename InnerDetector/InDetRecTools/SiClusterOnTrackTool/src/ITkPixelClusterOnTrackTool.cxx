/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
//   Implementation file for class ITk::PixelClusterOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for PixelClusterOnTrack object production
///////////////////////////////////////////////////////////////////
// started 1.0 21/04/2004 I.Gavrilenko - see ChangeLog
///////////////////////////////////////////////////////////////////

#include "SiClusterOnTrackTool/ITkPixelClusterOnTrackTool.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetIdentifier/PixelID.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "EventPrimitives/EventPrimitives.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include <cmath>
#include "TrkRIO_OnTrack/check_cast.h"

//clustermap is most likely to be removed at later date
#define __clustermap


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

namespace
{
  // using x*x might be quicker than pow(x,2), depends on compiler optimisation
  inline double
  square(const double x) {
    return x * x;
  }

  double
  distance(const std::vector<Amg::Vector2D> &vectorOfPositions,
           const std::vector<Amg::Vector2D> &allLocalPositions,
           const std::vector<Amg::MatrixX> &allErrorMatrix,
           const int i, const int j, const int k) {
    return
      square(vectorOfPositions[i][0] - allLocalPositions[0][0]) / allErrorMatrix[0](0, 0) +
      square(vectorOfPositions[j][0] - allLocalPositions[1][0]) / allErrorMatrix[1](0, 0) +
      square(vectorOfPositions[k][0] - allLocalPositions[2][0]) / allErrorMatrix[2](0, 0) +
      square(vectorOfPositions[i][1] - allLocalPositions[0][1]) / allErrorMatrix[0](1, 1) +
      square(vectorOfPositions[j][1] - allLocalPositions[1][1]) / allErrorMatrix[1](1, 1) +
      square(vectorOfPositions[k][1] - allLocalPositions[2][1]) / allErrorMatrix[2](1, 1);
  }
}

namespace ITk
{

PixelClusterOnTrackTool::PixelClusterOnTrackTool
  (const std::string &t, const std::string &n, const IInterface *p) :
  ::AthAlgTool(t, n, p),
  m_pixelid(nullptr),
  m_NnClusterizationFactory("InDet::NnClusterizationFactory/NnClusterizationFactory", this),
  m_doNotRecalibrateNN(false),
  m_noNNandBroadErrors(false),
  m_usingTIDE_Ambi(false),
  m_splitClusterMapKey("")
  {
  declareInterface<IRIO_OnTrackCreator>(this);

  declareProperty("PositionStrategy", m_positionStrategy = 1, "Which calibration of cluster positions");
  declareProperty("NnClusterizationFactory", m_NnClusterizationFactory);
  declareProperty("SplitClusterAmbiguityMap", m_splitClusterMapKey);//Remove Later
  declareProperty("doNotRecalibrateNN", m_doNotRecalibrateNN);
  declareProperty("m_noNNandBroadErrors", m_noNNandBroadErrors);
  declareProperty("RunningTIDE_Ambi", m_usingTIDE_Ambi);
}


///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode
PixelClusterOnTrackTool::initialize() {

  ATH_MSG_DEBUG(name() << " initialize()");

  m_errorStrategy = m_errorStrategyProperty;
  ATH_MSG_DEBUG("Error strategy is" << m_errorStrategy);

  m_applyNNcorrection = m_applyNNcorrectionProperty;

  ATH_CHECK(m_clusterITkErrorKey.initialize());

  ATH_CHECK(m_clusterSplitProbContainer.initialize( !m_clusterSplitProbContainer.key().empty()));

  // get the error scaling tool
  ATH_CHECK(m_pixelErrorScalingKey.initialize(!m_pixelErrorScalingKey.key().empty()));
  if (!m_pixelErrorScalingKey.key().empty()) ATH_MSG_DEBUG("Detected need for scaling Pixel errors.");

  ATH_CHECK (detStore()->retrieve(m_pixelid, "PixelID"));

  m_applyNNcorrection &= !m_splitClusterMapKey.key().empty();
  ATH_CHECK(m_splitClusterMapKey.initialize(m_applyNNcorrection));
  ATH_CHECK(m_NnClusterizationFactory.retrieve( DisableTool{!m_applyNNcorrection} ));

  ATH_CHECK(m_lorentzAngleTool.retrieve());
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////
// Trk::SiClusterOnTrack  production
///////////////////////////////////////////////////////////////////


const InDet::PixelClusterOnTrack *
PixelClusterOnTrackTool::correct
  (const Trk::PrepRawData &rio, const Trk::TrackParameters &trackPar) const {

  if (not m_applyNNcorrection){
    return correctDefault(rio, trackPar);
  }else {
    if (m_errorStrategy == 0 || m_errorStrategy == 1) {
      // version from Giacinto
      if (m_noNNandBroadErrors) {
        return nullptr;
      }
      // if we try broad errors, get Pixel Cluster to test if it is split
      const InDet::PixelCluster *pix = dynamic_cast<const InDet::PixelCluster *>(&rio);
      if (!pix) {
        return nullptr;
      }
      const Trk::ClusterSplitProbabilityContainer::ProbabilityInfo &splitProb = getClusterSplittingProbability(pix);
      if (splitProb.isSplit()) {
        return correctNN(rio, trackPar);
      } else {
        return correctDefault(rio, trackPar);
      }
    } else {
      return correctNN(rio, trackPar);
    }
  }
}



/** The correct method produces a PixelClusterOnTrack using the
 *  measured PixelCluster and the track prediction.
 */
const InDet::PixelClusterOnTrack *
PixelClusterOnTrackTool::correctDefault
  (const Trk::PrepRawData &rio, const Trk::TrackParameters &trackPar) const {
  using CLHEP::micrometer;

  const double TOPHAT_SIGMA = 1. / std::sqrt(12.);

  const InDet::PixelCluster *pix = nullptr;

  if (!(pix = dynamic_cast<const InDet::PixelCluster *>(&rio))) {
    return nullptr;
  }

  ATH_MSG_VERBOSE("Correct called with Error strategy " << m_errorStrategy);

  // PixelClusterOnTrack production
  //
  Trk::LocalParameters locpar;
  Amg::Vector3D glob(pix->globalPosition());


  // Get pointer to detector element
  const InDetDD::SiDetectorElement *element = pix->detectorElement();
  if (!element) {
    return nullptr;
  }
  IdentifierHash iH = element->identifyHash();

  double errphi = -1;
  double erreta = -1;

  if (pix->rdoList().empty()) {
    ATH_MSG_WARNING("Pixel RDO-list size is 0, check integrity of pixel clusters! stop ROT creation.");
    return nullptr;
  } else {
    const InDetDD::PixelModuleDesign *design =
      dynamic_cast<const InDetDD::PixelModuleDesign *>(&element->design());

    // get candidate track angle in module local frame
    Amg::Vector3D my_track = trackPar.momentum();
    const Amg::Vector3D& my_normal = element->normal();
    const Amg::Vector3D& my_phiax = element->phiAxis();
    const Amg::Vector3D& my_etaax = element->etaAxis();
    float trkphicomp = my_track.dot(my_phiax);
    float trketacomp = my_track.dot(my_etaax);
    float trknormcomp = my_track.dot(my_normal);
    double bowphi = std::atan2(trkphicomp, trknormcomp);
    double boweta = std::atan2(trketacomp, trknormcomp);

    float tanl = m_lorentzAngleTool->getTanLorentzAngle(iH);
    int readoutside = element->design().readoutSide();

    // map the angles of inward-going tracks onto [-PI/2, PI/2]
    if (bowphi > M_PI *0.5) {
      bowphi -= M_PI;
    }
    if (bowphi < -M_PI *0.5) {
      bowphi += M_PI;
    }
    // finally, subtract the Lorentz angle effect
    // the readoutside term is needed because of a bug in old
    // geometry versions (CSC-01-* and CSC-02-*)
    double angle = std::atan(std::tan(bowphi) - readoutside * tanl);

    // settle the sign/pi periodicity issues
    double thetaloc = -999.;
    if (boweta > -0.5 * M_PI && boweta < M_PI / 2.) { //M_PI_2 in cmath
      thetaloc = M_PI_2 - boweta;
    }else if (boweta > M_PI_2 && boweta < M_PI) {
      thetaloc = 1.5 * M_PI - boweta;
    } else { // 3rd quadrant
      thetaloc = -M_PI_2 - boweta;
    }
    double etaloc = -1 * log(tan(thetaloc * 0.5));

    // try to understand...
    const Identifier element_id = element->identify();
    int PixEtaModule = m_pixelid->eta_module(element_id);
    int PixPhiModule = m_pixelid->phi_module(element_id);
    double PixTrkPt = trackPar.pT();
    double PixTrkEta = trackPar.eta();
    ATH_MSG_VERBOSE("tanl = " << tanl << " readout side is " << readoutside <<
                    " module " << PixEtaModule << " " << PixPhiModule <<
                    " track pt, eta = " << PixTrkPt << " " << PixTrkEta <<
                    " track momentum phi, norm = " << trkphicomp << " " <<
                    trknormcomp << " bowphi = " << bowphi << " angle = " << angle);

    float omegaphi = pix->omegax();
    float omegaeta = pix->omegay();
    double localphi = -9999.;
    double localeta = -9999.;

    const std::vector<Identifier> & rdos = pix->rdoList();
    InDetDD::SiLocalPosition meanpos(0, 0, 0);
    int rowmin = 9999;
    int rowmax = -9999;
    int colmin = 9999;
    int colmax = -9999;
    for (const auto & rId:rdos) {
      const int row = m_pixelid->phi_index(rId);
      const int col = m_pixelid->eta_index(rId);
      rowmin = std::min(rowmin, row);
      rowmax = std::max(rowmax,row);
      colmin = std::min(colmin, col);
      colmax = std::max(colmax, col);
      meanpos += design->positionFromColumnRow(col, row);
    }
    meanpos = meanpos / rdos.size();
    InDetDD::SiLocalPosition pos1 =
      design->positionFromColumnRow(colmin, rowmin);
    InDetDD::SiLocalPosition pos2 =
      design->positionFromColumnRow(colmax, rowmin);
    InDetDD::SiLocalPosition pos3 =
      design->positionFromColumnRow(colmin, rowmax);
    InDetDD::SiLocalPosition pos4 =
      design->positionFromColumnRow(colmax, rowmax);

    InDetDD::SiLocalPosition centroid = 0.25 * (pos1 + pos2 + pos3 + pos4);
    double shift = m_lorentzAngleTool->getLorentzShift(iH);
    int nrows = rowmax - rowmin + 1;
    int ncol = colmax - colmin + 1;

    // TOT interpolation for collision data
    SG::ReadCondHandle<ITk::PixelOfflineCalibData> offlineITkCalibDataHandle(m_clusterITkErrorKey);

    if (m_positionStrategy > 0 && omegaphi > -0.5 && omegaeta > -0.5) {
      localphi = centroid.xPhi() + shift;
      localeta = centroid.xEta();

      std::pair<double,double> delta = offlineITkCalibDataHandle->getClusterErrorData()->getDelta(&element_id,nrows,angle,ncol,etaloc);
      double delta_phi = nrows != 1 ? delta.first : 0.;
      double delta_eta = ncol != 1 ? delta.second : 0.;
      localphi += delta_phi*(omegaphi-0.5);
      localeta += delta_eta*(omegaeta-0.5);
    }
    // digital
    else {
      localphi = meanpos.xPhi() + shift;
      localeta = meanpos.xEta();
    }

    const InDet::SiWidth& width = pix->width();

    // Error strategies

    // For very shallow tracks the cluster can easily break as
    // the average charge per pixel is of the order of the threshold
    // In this case, an error equal to the geometrical projection
    // of the track path in silicon onto the module surface seems
    // appropriate
    if (std::abs(angle) > 1) {
      errphi = 250 * micrometer * std::tan(std::abs(angle)) * TOPHAT_SIGMA;
      erreta = width.z() > 250 * micrometer * std::tan(std::abs(boweta)) ?
               width.z() * TOPHAT_SIGMA : 250 * micrometer * std::tan(std::abs(boweta)) * TOPHAT_SIGMA;
      ATH_MSG_VERBOSE("Shallow track with tanl = " << tanl << " bowphi = " <<
                      bowphi << " angle = " << angle << " width.z = " << width.z() <<
                      " errphi = " << errphi << " erreta = " << erreta);
    }else if (m_errorStrategy == 0) {
      errphi = width.phiR() * TOPHAT_SIGMA;
      erreta = width.z() * TOPHAT_SIGMA;
    }else if (m_errorStrategy == 1) {
      errphi = (width.phiR() / nrows) * TOPHAT_SIGMA;
      erreta = (width.z() / ncol) * TOPHAT_SIGMA;
    }else if (m_errorStrategy == 2) {
      std::pair<double,double> delta_err = offlineITkCalibDataHandle->getClusterErrorData()->getDeltaError(&element_id);
      errphi = nrows != 1 ? delta_err.first : (width.phiR()/nrows)*TOPHAT_SIGMA;
      erreta = ncol != 1 ? delta_err.second : (width.z()/ncol)*TOPHAT_SIGMA;
    }

    Amg::Vector2D locpos = Amg::Vector2D(localphi, localeta);
    locpar = Trk::LocalParameters(locpos);
    centroid = InDetDD::SiLocalPosition(localeta, localphi, 0.);
    glob = element->globalPosition(centroid);
  }

  // Error matrix production

  Amg::MatrixX cov = pix->localCovariance();

  // corrected phi error
  if (errphi > 0) {
    cov(0, 0) = errphi * errphi;
  }
  if (erreta > 0) {
    cov(1, 1) = erreta * erreta;
  }

  ATH_MSG_VERBOSE(" errphi =  " << errphi << " erreta = " << erreta);

  // create new copy of error matrix
  if (!m_pixelErrorScalingKey.key().empty()) {
    SG::ReadCondHandle<RIO_OnTrackErrorScaling> error_scaling( m_pixelErrorScalingKey );
    cov = check_cast<PixelRIO_OnTrackErrorScaling>(*error_scaling)->getScaledCovariance( cov,  *m_pixelid, element->identify() );
  }
  bool isbroad = m_errorStrategy == 0;
  return new InDet::PixelClusterOnTrack(pix, locpar, cov, iH, glob, pix->gangedPixel(), isbroad);
}


const InDet::PixelClusterOnTrack *
PixelClusterOnTrackTool::correct
  (const Trk::PrepRawData &rio, const Trk::TrackParameters &trackPar,
  const ITk::PixelClusterStrategy strategy) const {
  int initial_errorStrategy;
  const InDet::PixelClusterOnTrack *newROT;

  switch (strategy) {
  case PixelClusterStrategy::OUTLIER: // if cluster is outlier, increase errors
  case PixelClusterStrategy::SHARED:
    initial_errorStrategy = m_errorStrategy;
    m_errorStrategy = 0; // error as size of cluster /sqrt(12)
    newROT = correct(rio, trackPar);
    m_errorStrategy = initial_errorStrategy;
    return newROT;

  default:
    return correct(rio, trackPar);
  }
}

// GP: NEW correct() method in case of NN based calibration  */
const InDet::PixelClusterOnTrack *
PixelClusterOnTrackTool::correctNN
  (const Trk::PrepRawData &rio,
   const Trk::TrackParameters &trackPar) const {
  const InDet::PixelCluster *pixelPrepCluster = dynamic_cast<const InDet::PixelCluster *>(&rio);

  if (pixelPrepCluster == nullptr) {
    ATH_MSG_WARNING("This is not a pixel cluster, return 0.");
    return nullptr;
  }

  const InDetDD::SiDetectorElement *element = pixelPrepCluster->detectorElement();
  if (!element) {
    ATH_MSG_WARNING("Cannot access detector element. Aborting cluster correction...");
    return nullptr;
  }

  IdentifierHash iH = element->identifyHash();

  if (m_doNotRecalibrateNN) {
    Amg::Vector3D glob(pixelPrepCluster->globalPosition());

    Amg::Vector2D locpos = pixelPrepCluster->localPosition();
    Trk::LocalParameters locpar = Trk::LocalParameters(locpos);
    Amg::MatrixX cov = pixelPrepCluster->localCovariance();

    return new InDet::PixelClusterOnTrack(pixelPrepCluster, locpar, cov, iH, glob,
                                          pixelPrepCluster->gangedPixel(), false);
  }



  Amg::Vector2D finalposition;
  Amg::MatrixX finalerrormatrix;

  if (m_usingTIDE_Ambi) {
    if (!getErrorsTIDE_Ambi(pixelPrepCluster, trackPar, finalposition, finalerrormatrix)) {
      return correctDefault(rio, trackPar);
    }
  }else {
    if (!getErrorsDefaultAmbi(pixelPrepCluster, trackPar, finalposition, finalerrormatrix)) {
      return correctDefault(rio, trackPar);
    }
  }

  ATH_MSG_DEBUG( " Old position x: " << pixelPrepCluster->localPosition()[0]
      << " +/- " << std::sqrt(pixelPrepCluster->localCovariance()(0, 0))
      << " y: " << pixelPrepCluster->localPosition()[1]
      << " +/- " << std::sqrt(pixelPrepCluster->localCovariance()(1, 1)) <<"\n"
      << " Final position x: " << finalposition[0]
      << " +/- " << std::sqrt(finalerrormatrix(0, 0))
      << " y: " << finalposition[1] << " +/- "
      <<std::sqrt(finalerrormatrix(1, 1)) );

  Amg::MatrixX cov = finalerrormatrix;
  // create new copy of error matrix
  if (!m_pixelErrorScalingKey.key().empty()) {
    SG::ReadCondHandle<RIO_OnTrackErrorScaling> error_scaling( m_pixelErrorScalingKey );
    cov = check_cast<PixelRIO_OnTrackErrorScaling>(*error_scaling)->getScaledCovariance( cov,  *m_pixelid, element->identify() );
  }

  InDetDD::SiLocalPosition centroid = InDetDD::SiLocalPosition(finalposition[1],
                                                               finalposition[0],
                                                               0);
  Trk::LocalParameters locpar = Trk::LocalParameters(finalposition);

  const Amg::Vector3D &glob = element->globalPosition(centroid);


  return new InDet::PixelClusterOnTrack(pixelPrepCluster, locpar, cov, iH,
                                        glob,
                                        pixelPrepCluster->gangedPixel(),
                                        false);
}

bool
PixelClusterOnTrackTool::getErrorsDefaultAmbi(const InDet::PixelCluster *pixelPrepCluster,
                                              const Trk::TrackParameters &trackPar,
                                              Amg::Vector2D &finalposition,
                                              Amg::MatrixX &finalerrormatrix) const {
  std::vector<Amg::Vector2D> vectorOfPositions;
  int numberOfSubclusters = 1;
  vectorOfPositions.push_back(pixelPrepCluster->localPosition());

  if (m_applyNNcorrection){
    SG::ReadHandle<InDet::PixelGangedClusterAmbiguities> splitClusterMap(m_splitClusterMapKey);
    InDet::PixelGangedClusterAmbiguities::const_iterator mapBegin = splitClusterMap->begin();
    InDet::PixelGangedClusterAmbiguities::const_iterator mapEnd = splitClusterMap->end();
    for (InDet::PixelGangedClusterAmbiguities::const_iterator mapIter = mapBegin; mapIter != mapEnd; ++mapIter) {
      const InDet::SiCluster *first = (*mapIter).first;
      const InDet::SiCluster *second = (*mapIter).second;
      if (first == pixelPrepCluster && second != pixelPrepCluster) {
        ATH_MSG_DEBUG("Found additional split cluster in ambiguity map (+=1).");
        numberOfSubclusters += 1;
        const InDet::SiCluster *otherOne = second;
        const InDet::PixelCluster *pixelAddCluster = dynamic_cast<const InDet::PixelCluster *>(otherOne);
        if (pixelAddCluster == nullptr) {
          ATH_MSG_WARNING("Pixel ambiguity map has empty pixel cluster. Please DEBUG!");
          continue;
        }
        vectorOfPositions.push_back(pixelAddCluster->localPosition());

        ATH_MSG_DEBUG( "Found one more pixel cluster. Position x: "
                       << pixelAddCluster->localPosition()[0] << "y: " << pixelAddCluster->localPosition()[1]);
      }// find relevant element of map
    }// iterate over map
  }

  // now you have numberOfSubclusters and the vectorOfPositions (Amg::Vector2D)

  if (trackPar.surfaceType() != Trk::SurfaceType::Plane ||
      trackPar.type() != Trk::AtaSurface) {
    ATH_MSG_WARNING(
      "Parameters are not at a plane ! Aborting cluster correction... ");
    return false;
  }

  std::vector<Amg::Vector2D> allLocalPositions;
  std::vector<Amg::MatrixX> allErrorMatrix;
  allLocalPositions =
    m_NnClusterizationFactory->estimatePositions(*pixelPrepCluster,
                                                 trackPar.associatedSurface(),
                                                 trackPar,
                                                 allErrorMatrix,
                                                 numberOfSubclusters);

  if (allLocalPositions.empty()) {
    ATH_MSG_DEBUG( " Cluster cannot be treated by NN. Giving back to default clusterization " );

    return false;
  }

  if (allLocalPositions.size() != size_t(numberOfSubclusters)) {
    ATH_MSG_WARNING( "Returned position vector size " << allLocalPositions.size() <<
    " not according to expected number of subclusters: " << numberOfSubclusters << ". Abort cluster correction..." );
    return false;
  }


  // GP: now the not so nice part of matching the new result with the old one...
  // Takes the error into account to improve the matching

  if (numberOfSubclusters == 1) {
    finalposition = allLocalPositions[0];
    finalerrormatrix = allErrorMatrix[0];
  }

  else if (numberOfSubclusters == 2) {
    double distancesq1 =
      square(vectorOfPositions[0][0] - allLocalPositions[0][0]) / allErrorMatrix[0](0, 0) +
      square(vectorOfPositions[1][0] - allLocalPositions[1][0]) / allErrorMatrix[1](0, 0) +
      square(vectorOfPositions[0][1] - allLocalPositions[0][1]) / allErrorMatrix[0](1, 1) +
      square(vectorOfPositions[1][1] - allLocalPositions[1][1]) / allErrorMatrix[1](1, 1);

    double distancesq2 =
      square(vectorOfPositions[1][0] - allLocalPositions[0][0]) / allErrorMatrix[0](0, 0) +
      square(vectorOfPositions[0][0] - allLocalPositions[1][0]) / allErrorMatrix[1](0, 0) +
      square(vectorOfPositions[1][1] - allLocalPositions[0][1]) / allErrorMatrix[0](1, 1) +
      square(vectorOfPositions[0][1] - allLocalPositions[1][1]) / allErrorMatrix[1](1, 1);

    ATH_MSG_DEBUG(
     " Old pix (1) x: " << vectorOfPositions[0][0] << " y: " << vectorOfPositions[0][1] << "\n"
      << " Old pix (2) x: " << vectorOfPositions[1][0] << " y: " << vectorOfPositions[1][1] << "\n"
      << " Pix (1) x: " << allLocalPositions[0][0] << " +/- " << std::sqrt(allErrorMatrix[0](0, 0))
      << " y: " << allLocalPositions[0][1] << " +/- " << std::sqrt(allErrorMatrix[0](1, 1)) <<"\n"
      << " Pix (2) x: " << allLocalPositions[1][0] << " +/- " << std::sqrt(allErrorMatrix[1](0, 0))
      << " y: " << allLocalPositions[1][1] << " +/- " << std::sqrt(allErrorMatrix[1](1, 1)) << "\n"
      << " Old (1) new (1) dist: " << std::sqrt(distancesq1) << " Old (1) new (2) " << std::sqrt(distancesq2) );


    if (distancesq1 < distancesq2) {
      finalposition = allLocalPositions[0];
      finalerrormatrix = allErrorMatrix[0];
    }else {
      finalposition = allLocalPositions[1];
      finalerrormatrix = allErrorMatrix[1];
    }
  }


  else if (numberOfSubclusters == 3) {
    double distances[6];

    distances[0] = distance(vectorOfPositions, allLocalPositions, allErrorMatrix, 0, 1, 2);
    distances[1] = distance(vectorOfPositions, allLocalPositions, allErrorMatrix, 0, 2, 1);
    distances[2] = distance(vectorOfPositions, allLocalPositions, allErrorMatrix, 1, 0, 2);
    distances[3] = distance(vectorOfPositions, allLocalPositions, allErrorMatrix, 1, 2, 0);
    distances[4] = distance(vectorOfPositions, allLocalPositions, allErrorMatrix, 2, 0, 1);
    distances[5] = distance(vectorOfPositions, allLocalPositions, allErrorMatrix, 2, 1, 0);

    int smallestDistanceIndex = -10;
    double minDistance = 1e10;

    for (int i = 0; i < 6; i++) {
      ATH_MSG_VERBOSE(" distance n.: " << i << " distance is: " << distances[i]);

      if (distances[i] < minDistance) {
        minDistance = distances[i];
        smallestDistanceIndex = i;
      }
    }

    ATH_MSG_DEBUG(" The minimum distance is : " << minDistance << " for index: " << smallestDistanceIndex);

    if (smallestDistanceIndex == 0 || smallestDistanceIndex == 1) {
      finalposition = allLocalPositions[0];
      finalerrormatrix = allErrorMatrix[0];
    }
    if (smallestDistanceIndex == 2 || smallestDistanceIndex == 4) {
      finalposition = allLocalPositions[1];
      finalerrormatrix = allErrorMatrix[1];
    }
    if (smallestDistanceIndex == 3 || smallestDistanceIndex == 5) {
      finalposition = allLocalPositions[2];
      finalerrormatrix = allErrorMatrix[2];
    }
  }
  return true;
}

bool
PixelClusterOnTrackTool::getErrorsTIDE_Ambi(const InDet::PixelCluster *pixelPrepCluster,
                                            const Trk::TrackParameters &trackPar,
                                            Amg::Vector2D &finalposition,
                                            Amg::MatrixX &finalerrormatrix) const {
  const Trk::ClusterSplitProbabilityContainer::ProbabilityInfo &splitProb = getClusterSplittingProbability(pixelPrepCluster);
  std::vector<Amg::Vector2D> vectorOfPositions;
  int numberOfSubclusters = 1;
  if(m_applyNNcorrection){
    SG::ReadHandle<InDet::PixelGangedClusterAmbiguities> splitClusterMap(m_splitClusterMapKey);
    numberOfSubclusters = 1 + splitClusterMap->count(pixelPrepCluster);

    if (splitClusterMap->count(pixelPrepCluster) == 0 && splitProb.isSplit()) {
      numberOfSubclusters = 2;
    }
    if (splitClusterMap->count(pixelPrepCluster) != 0 && !splitProb.isSplit()) {
      numberOfSubclusters = 1;
    }
  }

  // now you have numberOfSubclusters and the vectorOfPositions (Amg::Vector2D)
  if (trackPar.surfaceType() != Trk::SurfaceType::Plane ||
      trackPar.type() != Trk::AtaSurface) {
    ATH_MSG_WARNING("Parameters are not at a plane surface ! Aborting cluster "
                    "correction... ");
    return false;
  }

  std::vector<Amg::Vector2D> allLocalPositions;
  std::vector<Amg::MatrixX> allErrorMatrix;
  allLocalPositions = m_NnClusterizationFactory->estimatePositions(
    *pixelPrepCluster,
    trackPar.associatedSurface(),
    trackPar,
    allErrorMatrix,
    numberOfSubclusters);

  if (allLocalPositions.empty()) {
    ATH_MSG_DEBUG(
      " Cluster cannot be treated by NN. Giving back to default clusterization, too big: " <<
      splitProb.isTooBigToBeSplit());
    return false;
  }

  if (allLocalPositions.size() != size_t(numberOfSubclusters)) {
    ATH_MSG_WARNING(
      "Returned position vector size " << allLocalPositions.size() << " not according to expected number of subclusters: " << numberOfSubclusters <<
    ". Abort cluster correction...");
    return false;
  }

  // AKM: now the not so nice part find the best match position option
  // Takes the error into account to scale the importance of the measurement

  if (numberOfSubclusters == 1) {
    finalposition = allLocalPositions[0];
    finalerrormatrix = allErrorMatrix[0];
    return true;
  }

  // Get the track parameters local position
  const Amg::Vector2D localpos = trackPar.localPosition();
  // Use the track parameters cov to weight distcances
  Amg::Vector2D localerr(0.01, 0.05);
  if (trackPar.covariance()) {
    localerr = Amg::Vector2D(std::sqrt((*trackPar.covariance())(0, 0)), std::sqrt((*trackPar.covariance())(1, 1)));
  }

  double minDistance(1e300);
  int index(0);

  for (unsigned int i(0); i < allLocalPositions.size(); ++i) {
    double distance =
      square(localpos[0] - allLocalPositions[i][0]) / localerr[0]
      + square(localpos[1] - allLocalPositions[i][1]) / localerr[1];

    if (distance < minDistance) {
      index = i;
      minDistance = distance;
    }
  }

  finalposition = allLocalPositions[index];
  finalerrormatrix = allErrorMatrix[index];
  return true;
}

} // namespace ITk
