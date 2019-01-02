/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
//   Implementation file for class FTK_PixelClusterOnTrackTool
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// AlgTool used for FTK_PixelClusterOnTrack object production
///////////////////////////////////////////////////////////////////
// Created 9/11/2016 J. Baines based on version by I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "FTK_RecTools/FTK_PixelClusterOnTrackTool.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelConditionsTools/IModuleDistortionsTool.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "EventPrimitives/EventPrimitives.h"
#include "PixelGeoModel/IIBLParameterSvc.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Attribute.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrkRIO_OnTrack/check_cast.h"

using CLHEP::mm;
using CLHEP::micrometer;

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

  // avoid a lot of '/sqrt(12)' in loops, declare the constant here
  // constexpr sqrt is a gcc extension, unsupported by clang.
  //constexpr double TOPHAT_SIGMA = 1. / sqrt(12.);
  static double TOPHAT_SIGMA = 1. / sqrt(12.);
}


FTK_PixelClusterOnTrackTool::FTK_PixelClusterOnTrackTool
  (const std::string &t, const std::string &n, const IInterface *p) :
  ::AthAlgTool(t, n, p),
  m_pixDistoTool("PixelDistortionsTool", this),
  m_detStore(nullptr),
  m_disableDistortions(false),
  m_rel13like(false),
  m_pixelid(nullptr),
  m_applyNNcorrection(false),
  m_applydRcorrection(false),
  m_NNIBLcorrection(false),
  m_IBLAbsent(true),
  m_NnClusterizationFactory("InDet::NnClusterizationFactory/NnClusterizationFactory", this),
  m_storeGate("StoreGateSvc", n),
  m_IBLParameterSvc("IBLParameterSvc", n),
  m_dRMap(""),
  m_dRMapName("dRMap"),
  m_doNotRecalibrateNN(false),
  m_noNNandBroadErrors(false),
  m_usingTIDE_Ambi(false),
  m_splitClusterMapName("SplitClusterAmbiguityMap") {
  declareInterface<IRIO_OnTrackCreator>(this);

  declareProperty("PixelDistortionsTool", m_pixDistoTool, "Tool to retrieve pixel distortions");
  declareProperty("PositionStrategy", m_positionStrategy = 1, "Which calibration of cluster positions");
  declareProperty("ErrorStrategy", m_errorStrategy = 2, "Which calibration of cluster position errors");
  declareProperty("DisableDistortions", m_disableDistortions, "Disable simulation of module distortions");
  declareProperty("Release13like", m_rel13like, "Activate release-13 like settigs");
  declareProperty("applyNNcorrection", m_applyNNcorrection);
  declareProperty("applydRcorrection", m_applydRcorrection);
  declareProperty("NNIBLcorrection", m_NNIBLcorrection);
  declareProperty("EventStore", m_storeGate);
  declareProperty("NnClusterizationFactory", m_NnClusterizationFactory);
  declareProperty("SplitClusterAmbiguityMap", m_splitClusterMapName);//Remove Later
  declareProperty("dRMapName", m_dRMapName);  //This is a string to prevent the scheduler seeing trkAmbSolver as both creating and requiring the map
  declareProperty("doNotRecalibrateNN", m_doNotRecalibrateNN);
  declareProperty("m_noNNandBroadErrors", m_noNNandBroadErrors);
  declareProperty("RunningTIDE_Ambi", m_usingTIDE_Ambi);
}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

FTK_PixelClusterOnTrackTool::~FTK_PixelClusterOnTrackTool() {
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode
FTK_PixelClusterOnTrackTool::initialize() {
  StatusCode sc = AthAlgTool::initialize();

  ATH_MSG_INFO(name() << " initialize()");
  ATH_MSG_DEBUG("Error strategy is" << m_errorStrategy);
  ATH_MSG_DEBUG("Release 13 flag is" << m_rel13like);

  if (m_IBLParameterSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING("Could not retrieve IBLParameterSvc");
  } else {
    m_IBLParameterSvc->setBoolParameters(m_applyNNcorrection, "doPixelClusterSplitting");
    m_IBLParameterSvc->setBoolParameters(m_IBLAbsent, "IBLAbsent");
  }

  ATH_CHECK(m_clusterErrorKey.initialize());

  // get the error scaling tool
  if (!m_pixelErrorScalingKey.key().empty()) {
    ATH_CHECK(m_pixelErrorScalingKey.initialize());
    ATH_MSG_DEBUG("Detected need for scaling Pixel errors.");
  }

  // the NN corrections
  if (m_applyNNcorrection) {
    msg(MSG::WARNING) << " NNcorrection not possible for FTK clusters - using default correction" << endmsg;
    m_applyNNcorrection=false;
  }

  // get the module distortions tool
  if (!m_disableDistortions) {
    if (!m_pixDistoTool.empty()) {
      sc = m_pixDistoTool.retrieve();
      if (sc != StatusCode::SUCCESS) {
        msg(MSG::ERROR) << "Can't get pixel distortions tool " << endmsg;
      } else {
        ATH_MSG_INFO("Pixel distortions tool retrieved");
      }
    } else {
      ATH_MSG_INFO("No PixelDistortionsTool selected.");
    }
  } else {
    ATH_MSG_INFO("No PixelDistortions will be simulated.");
  }

  if (detStore()->retrieve(m_pixelid, "PixelID").isFailure()) {
    ATH_MSG_FATAL("Could not get Pixel ID helper");
    return StatusCode::FAILURE;
  }

  sc = service("DetectorStore", m_detStore);
  if (!sc.isSuccess() || 0 == m_detStore) {
    return msg(MSG:: ERROR) << "Could not find DetStore" << endmsg, StatusCode::FAILURE;
  }
  if (m_storeGate.retrieve().isFailure()) {
    ATH_MSG_WARNING("Can not retrieve " << m_storeGate << ". Exiting.");
    return StatusCode::FAILURE;
  }
   
  m_dRMap = SG::ReadHandleKey<InDet::DRMap>(m_dRMapName);
  ATH_CHECK( m_dRMap.initialize() );
  // Include IBL calibration constants
  //Moved to initialize to remove statics and prevent repitition
  
  constexpr double phimin=-0.27, phimax=0.27;
  for (int i=0; i<=NBINPHI; i++) m_phix[i]=phimin+i*(phimax-phimin)/NBINPHI;
  constexpr double etacen[NBINETA]={-0.,1.,1.55,1.9,2.15,2.35};
  m_etax[0]=0.; m_etax[NBINETA]=2.7;
  for (int i=0; i<NBINETA-1; i++) m_etax[i+1]=(etacen[i]+etacen[i+1])/2.;

  //UGLY!
#include "IBL_calibration.h"
  //  

  ATH_CHECK(m_lorentzAngleTool.retrieve());
   
  return sc;
}



///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode
FTK_PixelClusterOnTrackTool::finalize() {
  return AlgTool::finalize();
}

void
FTK_PixelClusterOnTrackTool::FillFromDataBase() const{
    if (m_fX.empty()) {
      const CondAttrListCollection *atrlistcol = nullptr;
      if (StatusCode::SUCCESS == m_detStore->retrieve(atrlistcol, "/PIXEL/PixelClustering/PixelCovCorr")) {
        // loop over objects in collection
        for (CondAttrListCollection::const_iterator citr = atrlistcol->begin(); citr != atrlistcol->end(); ++citr) {
          std::vector<float> fx, fy, fb, fc, fd;
          const coral::AttributeList &atrlist = citr->second;

          fx.push_back(atrlist["fX1"].data<float>());
          fx.push_back(atrlist["fX2"].data<float>());
          fx.push_back(atrlist["fX3"].data<float>());
          fx.push_back(atrlist["fX4"].data<float>());
          fx.push_back(atrlist["fX5"].data<float>());
          fx.push_back(atrlist["fX6"].data<float>());
          fx.push_back(atrlist["fX7"].data<float>());
          m_fX.emplace_back(std::move(fx));

          fy.push_back(atrlist["fY1"].data<float>());
          fy.push_back(atrlist["fY2"].data<float>());
          fy.push_back(atrlist["fY3"].data<float>());
          fy.push_back(atrlist["fY4"].data<float>());
          fy.push_back(atrlist["fY5"].data<float>());
          fy.push_back(atrlist["fY6"].data<float>());
          fy.push_back(atrlist["fY7"].data<float>());
          m_fY.emplace_back(std::move(fy));

          fb.push_back(atrlist["fB1"].data<float>());
          fb.push_back(atrlist["fB2"].data<float>());
          fb.push_back(atrlist["fB3"].data<float>());
          fb.push_back(atrlist["fB4"].data<float>());
          fb.push_back(atrlist["fB5"].data<float>());
          fb.push_back(atrlist["fB6"].data<float>());
          fb.push_back(atrlist["fB7"].data<float>());
          m_fB.emplace_back(std::move(fb));

          fc.push_back(atrlist["fC1"].data<float>());
          fc.push_back(atrlist["fC2"].data<float>());
          fc.push_back(atrlist["fC3"].data<float>());
          fc.push_back(atrlist["fC4"].data<float>());
          fc.push_back(atrlist["fC5"].data<float>());
          fc.push_back(atrlist["fC6"].data<float>());
          fc.push_back(atrlist["fC7"].data<float>());
          m_fC.emplace_back(std::move(fc));

          fd.push_back(atrlist["fD1"].data<float>());
          fd.push_back(atrlist["fD2"].data<float>());
          fd.push_back(atrlist["fD3"].data<float>());
          fd.push_back(atrlist["fD4"].data<float>());
          fd.push_back(atrlist["fD5"].data<float>());
          fd.push_back(atrlist["fD6"].data<float>());
          fd.push_back(atrlist["fD7"].data<float>());
          m_fD.emplace_back(std::move(fd));
        }
      }else {
        msg(MSG::ERROR) << "Cannot find covariance corrections for key "
                        << "/PIXEL/PixelClustering/PixelCovCorr" << " - no correction " << endmsg;
        
      }
    }
}


///////////////////////////////////////////////////////////////////
// Trk::SiClusterOnTrack  production
///////////////////////////////////////////////////////////////////


const InDet::PixelClusterOnTrack *
FTK_PixelClusterOnTrackTool::correct
  (const Trk::PrepRawData &rio, const Trk::TrackParameters &trackPar) const {


  if (!m_applyNNcorrection ||
      ((dynamic_cast<const InDetDD::SiDetectorElement *>(rio.detectorElement()))->isBlayer() && !m_NNIBLcorrection &&
       !m_IBLAbsent)) {
    return correctDefault(rio, trackPar);
  }else {
    if (m_errorStrategy == 0 || m_errorStrategy == 1) {
      // version from Giacinto
      if (m_noNNandBroadErrors) {
        return 0;
      }
      // if we try broad errors, get Pixel Cluster to test if it is split
      const InDet::PixelCluster *pix = dynamic_cast<const InDet::PixelCluster *>(&rio);
      if (!pix) {
        return 0;
      }
      if (pix->isSplit()) {
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
FTK_PixelClusterOnTrackTool::correctDefault
  (const Trk::PrepRawData &rio, const Trk::TrackParameters &trackPar) const {
  //  const InDet::SiCluster* SC = dynamic_cast<const InDet::SiCluster*> (&rio);
  const InDet::PixelCluster *pix = nullptr;

  if (!(pix = dynamic_cast<const InDet::PixelCluster *>(&rio))) {
    return 0;
  }

  ATH_MSG_VERBOSE("Correct called with Error strategy " << m_errorStrategy);

  // PixelClusterOnTrack production
  //
  Trk::LocalParameters locpar;
  Amg::Vector3D glob(pix->globalPosition());


  // Get pointer to detector element
  const InDetDD::SiDetectorElement *element = pix->detectorElement();
  if (!element) {
    return 0;
  }
  bool blayer = element->isBlayer();
  IdentifierHash iH = element->identifyHash();

  double errphi = -1;
  double erreta = -1;

  if (pix->rdoList().empty()) {
    ATH_MSG_WARNING("Pixel RDO-list size is 0, check integrity of pixel clusters! stop ROT creation.");
    return NULL;
  } else {

    // get candidate track angle in module local frame
    Amg::Vector3D my_track = trackPar.momentum();
    Amg::Vector3D my_normal = element->normal();
    Amg::Vector3D my_phiax = element->phiAxis();
    Amg::Vector3D my_etaax = element->etaAxis();
    float trkphicomp = my_track.dot(my_phiax);
    float trketacomp = my_track.dot(my_etaax);
    float trknormcomp = my_track.dot(my_normal);
    double bowphi = atan2(trkphicomp, trknormcomp);
    double boweta = atan2(trketacomp, trknormcomp);
    float etatrack = trackPar.eta();

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
    double angle = atan(tan(bowphi) - readoutside * tanl);

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
    std::vector<Identifier>::const_iterator oneRDO = rdos.begin();
    Identifier rId = *oneRDO;
    const int row = m_pixelid->phi_index(rId);
    const int col = m_pixelid->eta_index(rId);

    InDetDD::SiLocalPosition centroid = pix->localPosition();
    double shift = m_lorentzAngleTool->getLorentzShift(iH);
    int nrows = pix->width().colRow()[Trk::locX];
    int ncol = pix->width().colRow()[Trk::locX];
    double ang = 999.;
    double eta = 999.;
    ATH_MSG_VERBOSE(" row " << row << " col " << col << " nrows " << nrows << 
		    " ncol " << ncol << " locX " << centroid.xPhi() << " locY " << centroid.xEta());
   
    SG::ReadCondHandle<PixelCalib::PixelOfflineCalibData> offlineCalibData(m_clusterErrorKey);

    //    ATH_MSG_VERBOSE ( << "Position strategy = "
    //    << m_positionStrategy << "omegaphi = " << omegaphi )

    // TOT interpolation for collision data
    // Force IBL to use digital clustering and broad errors.
    if (m_positionStrategy > 0 && omegaphi > -0.5 && omegaeta > -0.5) {
      localphi = centroid.xPhi() + shift;
      localeta = centroid.xEta();
      // barrel
      if (element->isBarrel()) {
        ang = 180 * angle * M_1_PI; //M_1_PI in cmath, = 1/pi
        double delta = 0.;
        if (m_IBLAbsent || !blayer) {
         delta = offlineCalibData->getPixelChargeInterpolationParameters()->getDeltaXbarrel(nrows, ang, 1);
        } else {             // special calibration for IBL
          if (angle < m_phix[0] || angle > m_phix[NBINPHI] || nrows != 2) {
            delta = 0.;
          }else {
            int bin = -1;
            while (angle > m_phix[bin + 1]) {
              bin++;
            }
            if ((bin >= 0)and(bin < NBINPHI)) {
              delta = m_calphi[bin];
            } else {
              ATH_MSG_ERROR("bin out of range in line " << __LINE__ << " of FTK_PixelClusterOnTrackTool.cxx.");
            }
          }
          if (offlineCalibData->getPixelChargeInterpolationParameters()->getVersion()<-1) {
            delta = offlineCalibData->getPixelChargeInterpolationParameters()->getDeltaXbarrel(nrows, ang, 0);
          }
        }
        localphi += delta * (omegaphi - 0.5);
	ATH_MSG_VERBOSE(" shifting localphi by delta * (omegaphi - 0.5) = " << delta << " * " << (omegaphi - 0.5)); 
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
        if (m_IBLAbsent || !blayer) {
          delta = offlineCalibData->getPixelChargeInterpolationParameters()->getDeltaYbarrel(ncol, etaloc, 1);
      	  ATH_MSG_VERBOSE(" Barrel Pixel: shifting localeta by delta * (omegaeta - 0.5) = " << delta << " * " << (omegaeta - 0.5)); 
        } else {     // special calibration for IBL
          etaloc = fabs(etaloc);
          if (etaloc < m_etax[0] || etaloc > m_etax[NBINETA]) {
            delta = 0.;
          } else {
            int bin = -1;
            while (etaloc > m_etax[bin + 1]) {
              bin++;
            }
            if ((bin >= 0)and(bin < NBINETA)) {
              if (ncol == bin) {
                delta = m_caleta[bin][0];
              } else if (ncol == bin + 1) {
                delta = m_caleta[bin][1];
              } else if (ncol == bin + 2) {
                delta = m_caleta[bin][2];
              } else {
                delta = 0.;
              }
            } else {// bin out of range of array indices
              ATH_MSG_ERROR("bin out of range in line " << __LINE__ << " of FTK_PixelClusterOnTrackTool.cxx.");
            }

          }
          if (offlineCalibData->getPixelChargeInterpolationParameters()->getVersion()<-1) {
            delta = offlineCalibData->getPixelChargeInterpolationParameters()->getDeltaYbarrel(ncol, std::fabs(etaloc), 0);
          }
	  ATH_MSG_VERBOSE(" IBL: shifting localeta by delta * (omegaeta - 0.5) = " << delta << " * " << (omegaeta - 0.5)); 
        }
        localeta += delta * (omegaeta - 0.5);
      }else {
        // collision endcap data
        if (m_positionStrategy == 1) {
          double deltax = offlineCalibData->getPixelChargeInterpolationParameters()->getDeltaXendcap();
          double deltay = offlineCalibData->getPixelChargeInterpolationParameters()->getDeltaYendcap();
          localphi += deltax * (omegaphi - 0.5);
          localeta += deltay * (omegaeta - 0.5);
	  ATH_MSG_VERBOSE(" endcap pixel shifting localphi by deltax * (omegaphi - 0.5) = " << deltax << " * " << (omegaphi - 0.5)); 
	  ATH_MSG_VERBOSE(" endcap pixel shifting localeta by deltay * (omegaeta - 0.5) = " << deltay << " * " << (omegaeta - 0.5)); 
        }
        // SR1 cosmics endcap data
        // some parametrization dependent on track angle
        // would be better here
        else if (m_positionStrategy == 20) {
          double deltax = 35 * micrometer;
          double deltay = 35 * micrometer;
          localphi += deltax * (omegaphi - 0.5);
          localeta += deltay * (omegaeta - 0.5);
        }
      }
    }
// digital
    else {
      localphi = centroid.xPhi() + shift;
      localeta = centroid.xEta();
    }

    InDet::SiWidth width = pix->width();

    // Error strategies

    // For very shallow tracks the cluster can easily break as
    // the average charge per pixel is of the order of the threshold
    // In this case, an error equal to the geometrical projection
    // of the track path in silicon onto the module surface seems
    // appropriate
    if (fabs(angle) > 1) {
      errphi = 250 * micrometer * tan(fabs(angle)) * TOPHAT_SIGMA;
      erreta = width.z() > 250 * micrometer * tan(fabs(boweta)) ?
               width.z() * TOPHAT_SIGMA : 250 * micrometer * tan(fabs(boweta)) * TOPHAT_SIGMA;
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
      if (element->isBarrel()) {
        if (m_IBLAbsent || !blayer) {
          int ibin = offlineCalibData->getPixelClusterOnTrackErrorData()->getBarrelBinPhi(ang, nrows);
          errphi = offlineCalibData->getPixelClusterOnTrackErrorData()->getPixelBarrelPhiError(ibin);
        } else {       // special calibration for IBL
          if (angle < m_phix[0] || angle > m_phix[NBINPHI]) {
            errphi = width.phiR() * TOPHAT_SIGMA;
          } else {
            int bin = -1;// cannot be used as array index, which will happen if angle<m_phix[bin+1]
            while (angle > m_phix[bin + 1]) {
              bin++;
            }
            if ((bin >= 0)and(bin < NBINPHI)) {
              if (nrows == 1) {
                errphi = m_calerrphi[bin][0];
              } else if (nrows == 2) {
                errphi = m_calerrphi[bin][1];
              } else {
                errphi = m_calerrphi[bin][2];
              }
            } else {
              ATH_MSG_ERROR("bin out of range in line " << __LINE__ << " of FTK_PixelClusterOnTrackTool.cxx.");
            }
          }
        }

        if (m_rel13like) {
          int ibin = offlineCalibData->getPixelClusterErrorData()->getBarrelBin(eta, ncol, nrows);
          erreta = offlineCalibData->getPixelClusterErrorData()->getPixelBarrelEtaError(ibin);
        }else if (m_IBLAbsent || !blayer) {
          int ibin = offlineCalibData->getPixelClusterOnTrackErrorData()->getBarrelBinEta(std::fabs(etatrack), ncol, nrows);
          erreta = offlineCalibData->getPixelClusterOnTrackErrorData()->getPixelBarrelEtaError(ibin);
        } else {    // special calibration for IBL
          double etaloc = fabs(etatrack);
          if (etaloc < m_etax[0] || etaloc > m_etax[NBINETA]) {
            erreta = width.z() * TOPHAT_SIGMA;
          } else {
            int bin = 0;
            while (etaloc > m_etax[bin + 1]) {
              ++bin;
            }
            if (bin >= NBINETA) {
              ATH_MSG_ERROR("bin out of range in line " << __LINE__ << " of FTK_PixelClusterOnTrackTool.cxx.");
            } else {
              if (ncol == bin) {
                erreta = m_calerreta[bin][0];
              } else if (ncol == bin + 1) {
                erreta = m_calerreta[bin][1];
              } else if (ncol == bin + 2) {
                erreta = m_calerreta[bin][2];
              } else {
                erreta = width.z() * TOPHAT_SIGMA;
              }
            }
          }
        }
      }else {
        int ibin = offlineCalibData->getPixelClusterErrorData()->getEndcapBin(ncol, nrows);
        errphi = offlineCalibData->getPixelClusterErrorData()->getPixelEndcapPhiError(ibin);
        erreta = offlineCalibData->getPixelClusterErrorData()->getPixelEndcapRError(ibin);
      }
      if (errphi > erreta) {
        erreta = width.z() * TOPHAT_SIGMA;
      }
    }

    Amg::Vector2D locpos = Amg::Vector2D(localphi, localeta);
    if (element->isBarrel() && !m_disableDistortions) {
      correctBow(element->identify(), locpos, bowphi, boweta);
    }


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
    //SG::ReadCondHandle<PixelRIO_OnTrackErrorScaling> error_scaling( m_pixelErrorScalingKey );
    SG::ReadCondHandle<RIO_OnTrackErrorScaling> error_scaling( m_pixelErrorScalingKey );
    cov = check_cast<PixelRIO_OnTrackErrorScaling>(*error_scaling)->getScaledCovariance( cov,  *m_pixelid, element->identify() );
  }
  bool isbroad = (m_errorStrategy == 0) ? true : false;

  ATH_MSG_VERBOSE("returning corrected cluster at locX " << locpar.get(Trk::locX) << " locY " << locpar.get(Trk::locY));

  return new InDet::PixelClusterOnTrack(pix, locpar, cov, iH, glob, pix->gangedPixel(), isbroad);
}

void
FTK_PixelClusterOnTrackTool::correctBow(const Identifier &id, Amg::Vector2D &localpos, const double phi,
                                           const double theta) const {
  Amg::Vector3D dir(tan(phi), tan(theta), 1.);
  Amg::Vector2D newpos =
    m_pixDistoTool->correctReconstruction(id, localpos, dir);

  localpos = newpos;
  return;
}

const InDet::PixelClusterOnTrack *
FTK_PixelClusterOnTrackTool::correct
  (const Trk::PrepRawData &rio, const Trk::TrackParameters &trackPar,
   const InDet::PixelClusterStrategy strategy) const {
  int initial_errorStrategy;
  const InDet::PixelClusterOnTrack *newROT;

  switch (strategy) {
  case InDet::PIXELCLUSTER_OUTLIER: // if cluster is outlier, increase errors
  case InDet::PIXELCLUSTER_SHARED:
    initial_errorStrategy = m_errorStrategy;
    if (!m_rel13like) {
      m_errorStrategy = 0; // error as size of cluster /sqrt(12)
    }
    newROT = correct(rio, trackPar);
    m_errorStrategy = initial_errorStrategy;
    return newROT;

  default:
    return correct(rio, trackPar);
  }
}

// GP: NEW correct() method in case of NN based calibration  */
const InDet::PixelClusterOnTrack *
FTK_PixelClusterOnTrackTool::correctNN
  (const Trk::PrepRawData &rio, const Trk::TrackParameters &trackPar) const {
  
  return correct(rio, trackPar);
}

bool
FTK_PixelClusterOnTrackTool::getErrorsDefaultAmbi(const InDet::PixelCluster *pixelPrepCluster,
                                                     const Trk::TrackParameters &trackPar,
                                                     Amg::Vector2D &finalposition,
                                                     Amg::MatrixX &finalerrormatrix) const {
  std::vector<Amg::Vector2D> vectorOfPositions;
  int numberOfSubclusters = 1;

  vectorOfPositions.push_back(pixelPrepCluster->localPosition());



  // A.S. hack for the moment: isSplit is also set for modified (non-split) 1-pixel-clusters ... is this needed anyway ?
  //
  // if (pixelPrepCluster->isSplit() && numberOfSubclusters<2)
  // {
  // msg(MSG::WARNING) << " Cluster is split but no further clusters found in split cluster map." << endmsg;
  // }

  // now you have numberOfSubclusters and the vectorOfPositions (Amg::Vector2D)

  const Trk::AtaPlane *parameters = dynamic_cast<const Trk::AtaPlane *>(&trackPar);
  if (parameters == 0) {
    msg(MSG::WARNING) << "Parameters are not at a plane ! Aborting cluster correction... " << endmsg;
    return false;
  }

  std::vector<Amg::Vector2D>     allLocalPositions;
  std::vector<Amg::MatrixX>      allErrorMatrix;

  allLocalPositions =
    m_NnClusterizationFactory->estimatePositions(*pixelPrepCluster,
                                                 parameters->associatedSurface(),
                                                 *parameters,
                                                 allErrorMatrix,
                                                 numberOfSubclusters);

  if (allLocalPositions.size() == 0) {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << " Cluster cannot be treated by NN. Giving back to default clusterization " << endmsg;
    }
    return false;
  }

  if (allLocalPositions.size() != size_t(numberOfSubclusters)) {
    msg(MSG::WARNING) << "Returned position vector size " << allLocalPositions.size() <<
    " not according to expected number of subclusters: " << numberOfSubclusters << ". Abort cluster correction..." <<
    endmsg;
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

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << " Old pix (1) x: " << vectorOfPositions[0][0] << " y: " << vectorOfPositions[0][1] << endmsg;
      msg(MSG::DEBUG) << " Old pix (2) x: " << vectorOfPositions[1][0] << " y: " << vectorOfPositions[1][1] << endmsg;
      msg(MSG::DEBUG) << " Pix (1) x: " << allLocalPositions[0][0] << " +/- " << sqrt(allErrorMatrix[0](0, 0))
                      << " y: " << allLocalPositions[0][1] << " +/- " << sqrt(allErrorMatrix[0](1, 1)) << endmsg;
      msg(MSG::DEBUG) << " Pix (2) x: " << allLocalPositions[1][0] << " +/- " << sqrt(allErrorMatrix[1](0, 0))
                      << " y: " << allLocalPositions[1][1] << " +/- " << sqrt(allErrorMatrix[1](1, 1)) << endmsg;
      msg(MSG::DEBUG) << " Old (1) new (1) dist: " << sqrt(distancesq1) << " Old (1) new (2) " << sqrt(distancesq2) <<
      endmsg;
    }

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
FTK_PixelClusterOnTrackTool::getErrorsTIDE_Ambi(const InDet::PixelCluster *pixelPrepCluster,
                                                   const Trk::TrackParameters &trackPar,
                                                   Amg::Vector2D &finalposition,
                                                   Amg::MatrixX &finalerrormatrix) const {
  std::vector<Amg::Vector2D> vectorOfPositions;

  int numberOfSubclusters = 1;
  // now you have numberOfSubclusters and the vectorOfPositions (Amg::Vector2D)

  const Trk::AtaPlane *parameters = dynamic_cast<const Trk::AtaPlane *>(&trackPar);
  if (parameters == 0) {
    ATH_MSG_WARNING("Parameters are not at a plane ! Aborting cluster correction... ");
    return false;
  }

  std::vector<Amg::Vector2D>     allLocalPositions;
  std::vector<Amg::MatrixX>      allErrorMatrix;
  allLocalPositions =
    m_NnClusterizationFactory->estimatePositions(*pixelPrepCluster,
                                                 parameters->associatedSurface(),
                                                 *parameters,
                                                 allErrorMatrix,
                                                 numberOfSubclusters);

  if (allLocalPositions.empty()) {
    ATH_MSG_DEBUG(
      " Cluster cannot be treated by NN. Giving back to default clusterization, too big: " <<
    pixelPrepCluster->tooBigToBeSplit());
    return false;
  }

  if (allLocalPositions.size() != size_t(numberOfSubclusters)) {
    ATH_MSG_WARNING(
      "Returned position vector size " << allLocalPositions.size() << " not according to expected number of subclusters: " << numberOfSubclusters <<
    ". Abort cluster correction...");
    return false;
  }

  bool correctdR = false;
  
  std::array<float, 3> correctiondR = {0., 0., 0.};
  
  if(m_applydRcorrection){
    SG::ReadHandle<InDet::DRMap> mapHandle(m_dRMap);
    if(!mapHandle.isValid()){
       ATH_MSG_ERROR("Error retreving " << m_dRMap.key());
       return false;
    }
  
    if(!mapHandle->empty() && pixelPrepCluster->isSplit() ){

    const InDetDD::SiDetectorElement *det = pixelPrepCluster->detectorElement();

    if (det->isBarrel()) {
      int pixelLayer = m_pixelid->layer_disk(det->identify());

      float correctiondRX = 1.0; // correction factor to be applied to covariance(!)
      float correctiondRZ = 1.0; // correction factor to be applied to covariance(!)

      const InDetDD::PixelModuleDesign *design(dynamic_cast<const InDetDD::PixelModuleDesign *>(&det->design()));
      if (! design){
        ATH_MSG_WARNING("Design cannot be cast to a pixel design");
        return false;
      }
      const float pitchX = design->phiPitch();
      const float pitchZ = design->etaPitch();

      ATH_MSG_DEBUG("FTK_PixelClusterOnTrackTool: Trying to apply dR correction for pixelLayer " << pixelLayer);

      InDet::DRMap::const_iterator it = mapHandle->find(pixelPrepCluster);
      if (it != mapHandle->end()) {
        correctdR = true;
        float mindX = it->second.first;
        float mindZ = it->second.second;
        float mindXOverPitch = mindX / pitchX;
        float mindZOverPitch = mindZ / pitchZ;

        float dR = sqrt(mindX * mindX + mindZ * mindZ);

        ATH_MSG_DEBUG(" ---- Min dX ---- " << mindX << " mindZ " << mindZ);

        if (dR > 0. && dR < 2.0) {
          correctiondRX = splineIBLPullX(mindXOverPitch, pixelLayer);
          correctiondRZ = splineIBLPullX(mindZOverPitch, pixelLayer);

          if (correctiondRX > 3.75) {
            correctiondRX = 3.75;
          }
          if (correctiondRZ > 3.75) {
            correctiondRZ = 3.75;
          }
        }else {
          correctiondRX = 2.35;
          correctiondRZ = 1.70;
        }

        ATH_MSG_DEBUG(
          "FTK_PixelClusterOnTrackTool: Correction factor calculation for distX/pitch= " << mindXOverPitch << " -> " << correctiondRX << " distZ/pitch= " << mindZOverPitch << " -> " <<
        correctiondRZ);
      }else {
        ATH_MSG_WARNING("Split Pixel cluster not found in dRmap! " << pixelPrepCluster);
      }

      ATH_MSG_DEBUG(
        " ++++ Hit Error ++++ Layer " << pixelLayer << " Correction Flag " << m_applydRcorrection << " Split " << pixelPrepCluster->isSplit() << " Scaling " << correctiondRX << " " <<
      correctiondRZ);

      correctiondRX *= correctiondRX;
      correctiondRZ *= correctiondRZ;

      correctiondR[0] = correctiondRX * correctiondRX;
      correctiondR[1] = correctiondRZ * correctiondRZ;
      correctiondR[2] = correctiondRX * correctiondRZ;
    }
   }
  }
  // AKM: now the not so nice part find the best match position option
  // Takes the error into account to scale the importance of the measurement

  if (numberOfSubclusters == 1) {
    finalposition = allLocalPositions[0];

    if (correctdR) {
      allErrorMatrix[0](0, 0) *= correctiondR[0];
      allErrorMatrix[0](1, 1) *= correctiondR[1];
      allErrorMatrix[0](0, 1) *= correctiondR[2];
      allErrorMatrix[0](1, 0) *= correctiondR[2];
      ATH_MSG_DEBUG(
        " ++++ Hit Error ++++ " << numberOfSubclusters << " Split " << pixelPrepCluster->isSplit() << " Scaling " <<
      sqrt(correctiondR[0]) << " " << sqrt(correctiondR[1]) << " Rescaled ErrX " << sqrt(allErrorMatrix[0](0,
                                                                                                           0)) << " Rescaled ErrZ " <<
      sqrt(allErrorMatrix[0](1, 1)));
    }

    finalerrormatrix = allErrorMatrix[0];
    return true;
  }

  // Get the track parameters local position
  const Amg::Vector2D localpos = trackPar.localPosition();
  // Use the track parameters cov to weight distcances
  Amg::Vector2D localerr(0.01, 0.05);
  if (trackPar.covariance()) {
    localerr = Amg::Vector2D(sqrt((*trackPar.covariance())(0, 0)), sqrt((*trackPar.covariance())(1, 1)));
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

  if (correctdR) {
    allErrorMatrix[index](0, 0) *= correctiondR[0];
    allErrorMatrix[index](1, 1) *= correctiondR[1];
    allErrorMatrix[index](0, 1) *= correctiondR[2];
    allErrorMatrix[index](1, 0) *= correctiondR[2];
    ATH_MSG_DEBUG(
      " ++++ Hit Error ++++ " << numberOfSubclusters << " Split " << pixelPrepCluster->isSplit() << " Scaling " <<
    sqrt(correctiondR[0]) << " " << sqrt(correctiondR[1]) << " Rescaled ErrX " << sqrt(allErrorMatrix[index](0,
                                                                                                             0)) << " Rescaled ErrZ " <<
    sqrt(allErrorMatrix[index](1, 1)));
  }

  finalerrormatrix = allErrorMatrix[index];
  return true;
}

double
FTK_PixelClusterOnTrackTool::splineIBLPullX(float x, int layer) const {
  FillFromDataBase();
  const int fNp = m_fY[layer].size();
  const int fKstep = 0;
  const double fDelta = -1;
  const auto result = std::minmax_element(std::begin(m_fX[layer]), std::end(m_fX[layer]));
  const double fXmin = *(result.first);
  const double fXmax = *(result.second);
  //const double fXmin = *std::min_element(std::begin(m_fX[layer]), std::end(m_fX[layer]));
  //const double fXmax = *std::max_element(std::begin(m_fX[layer]), std::end(m_fX[layer]));

  int klow = 0;

  if (x <= fXmin) {
    klow = 0;
  } else if (x >= fXmax) {
    klow = fNp - 1;
  } else {
    if (fKstep) {
      // Equidistant knots, use histogramming
      klow = int((x - fXmin) / fDelta);
      if (klow < fNp - 1) {
        klow = fNp - 1;
      }
    } else {
      int khig = fNp - 1, khalf;
      // Non equidistant knots, binary search
      while (khig - klow > 1) {
        if (x > m_fX[layer][khalf = (klow + khig) / 2]) {
          klow = khalf;
        } else {
          khig = khalf;
        }
      }
    }
  }
  // Evaluate now
  double dx = x - m_fX[layer][klow];
  return(m_fY[layer][klow] + dx * (m_fB[layer][klow] + dx * (m_fC[layer][klow] + dx * m_fD[layer][klow])));
}
