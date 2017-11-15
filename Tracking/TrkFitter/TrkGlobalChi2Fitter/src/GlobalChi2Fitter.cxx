/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkFitterUtils/TrackFitInputPreparator.h"
#include "TrkGlobalChi2Fitter/GlobalChi2Fitter.h"
#include "TrkGlobalChi2Fitter/GXFMaterialEffects.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/TrapezoidBounds.h"

#include "TrkGeometry/Layer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/MaterialLayer.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/TrackingGeometry.h"
// #include "TrkGeometry/EntryLayerProvider.h"

#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

#include "TrkExUtils/TransportJacobian.h"

#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkToolInterfaces/ITrkMaterialProviderTool.h"

#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"


#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkVertexOnTrack/VertexOnTrack.h"
#include "TrkSegment/TrackSegment.h"


#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExInterfaces/INavigator.h"
#include "TrkExInterfaces/IEnergyLossCalculator.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExInterfaces/IMaterialEffectsUpdator.h"

#include "TrkDetDescrInterfaces/IMaterialEffectsOnTrackProvider.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictDetDescr/IdDictManager.h"

#include "TDecompChol.h"
#include "TrkAlgebraUtils/AlSymMat.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"

using CLHEP::Hep3Vector;
using CLHEP::HepVector;
using CLHEP::HepMatrix;
using CLHEP::HepRotation;
using CLHEP::HepSymMatrix;
using CLHEP::MeV;
using CLHEP::mm;


namespace Trk {
  std::vector<HepMatrix> GlobalChi2Fitter::m_derivpool;

  GlobalChi2Fitter::GlobalChi2Fitter(const std::string &t, const std::string &n,
                                     const IInterface *p) :
    AthAlgTool(t, n, p),
    m_ROTcreator(""),
    m_broadROTcreator(""),
    m_updator(""),
    m_extrapolator("Trk::Extrapolator/CosmicsExtrapolator"),
    m_scattool("Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator"),
    m_elosstool("Trk::EnergyLossUpdator/AtlasEnergyLossUpdator"),
    m_matupdator(""),
    m_propagator("Trk::StraightLinePropagator/CosmicsPropagator"),
    m_navigator("Trk::Navigator/CosmicsNavigator"),
    m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
    m_calotool("Rec::MuidMaterialEffectsOnTrackProvider/MuidMaterialEffectsOnTrackProvider"),
    m_calotoolparam(""),
    m_cmeotp("Trk::CaloMaterialEffectsOnTrackProvider/CaloMaterialEffectsOnTrackProvider"),
    m_fieldService("AtlasFieldSvc", n),
    m_trackingGeometrySvc("", n),
    // m_trackingVolumesSvc    ("TrackingVolumesSvc/TrackingVolumesSvc",n),
    m_trackingGeometry(nullptr),
    m_caloEntrance(nullptr),
    m_msEntrance(nullptr),
    m_option_allowEmptyROT{},
    m_signedradius{},
    m_calomat{}, m_extmat{}, m_idmat{},
    m_derivmat(nullptr),
    m_fullcovmat(nullptr),
    m_DetID(nullptr),
    m_decomposesegments{},
    m_getmaterialfromtrack{},
    m_domeastrackpar{},
    m_storemat{},
    m_chi2cut{},
    m_scalefactor{},
    m_redoderivs{},
    m_reintoutl{},
    m_matfilled{},
    m_inputPreparator{},
    m_maxit{},
    m_nfits{}, m_nsuccessfits{}, m_matrixinvfailed{}, m_notenoughmeas{}, m_propfailed{}, m_invalidangles{},
    m_notconverge{}, m_highchi2{}, m_lowmomentum{},
    m_kinkallowed{},
    m_acceleration{},
    m_numderiv{},
    m_lastiter{},
    m_miniter{},
    m_fiteloss{},
    m_asymeloss{},
    m_fieldpropnofield(nullptr),
    m_fieldpropfullfield(nullptr),
    m_fieldprop(nullptr),
    m_prefit{},
    // m_derivpool{}, static member
    m_a{}, m_ainv{},
    m_particleMasses{},
    m_residuals{},
    m_updatescat{},
    m_useCaloTG(false),
    m_caloMaterialProvider("Trk::TrkMaterialProviderTool/TrkMaterialProviderTool"),
    m_rejectLargeNScat(false){
    // tools and services
    declareProperty("ExtrapolationTool", m_extrapolator);
    declareProperty("MeasurementUpdateTool", m_updator);
    declareProperty("RotCreatorTool", m_ROTcreator);
    declareProperty("BroadRotCreatorTool", m_broadROTcreator);
    declareProperty("MultipleScatteringTool", m_scattool);
    declareProperty("EnergyLossTool", m_elosstool);
    declareProperty("MaterialUpdateTool", m_matupdator);
    declareProperty("PropagatorTool", m_propagator);
    declareProperty("NavigatorTool", m_navigator);
    declareProperty("ResidualPullCalculatorTool", m_residualPullCalculator, "Tool to calculate residuals and pulls");
    declareProperty("TrackingGeometrySvc", m_trackingGeometrySvc);

    declareProperty("DecomposeSegments", m_decomposesegments = true);
    declareProperty("StraightLine", m_straightlineprop = true);
    declareProperty("OutlierCut", m_outlcut = 5.);
    declareProperty("MaxOutliers", m_maxoutliers = 10);
    declareProperty("SignedDriftRadius", m_signedradius = true);
    declareProperty("Momentum", m_p = 0.);
    declareProperty("ExtrapolatorMaterial", m_extmat = true);
    declareProperty("MuidMat", m_calomat = false); // use Calo tool
    declareProperty("MuidTool", m_calotool);
    declareProperty("MuidToolParam", m_calotoolparam);
    declareProperty("FillDerivativeMatrix", m_fillderivmatrix = false);
    declareProperty("RecalibrateSilicon", m_sirecal = false);
    declareProperty("RecalibrateTRT", m_trtrecal = false);
    declareProperty("MaxIterations", m_maxit = 30);
    declareProperty("GetMaterialFromTrack", m_getmaterialfromtrack = true);
    declareProperty("MeasuredTrackParameters", m_domeastrackpar = true);
    declareProperty("StoreMaterialOnTrack", m_storemat = true);
    declareProperty("ReintegrateOutliers", m_reintoutl = false);
    declareProperty("TrackChi2PerNDFCut", m_chi2cut = 1.e15);
    declareProperty("RecalculateDerivatives", m_redoderivs = false);
    declareProperty("TRTExtensionCuts", m_extensioncuts = true);
    declareProperty("TRTTubeHitCut", m_scalefactor = 2.5);
    declareProperty("PrintDerivatives", m_printderivs = false);
    declareProperty("Acceleration", m_acceleration = false);
    declareProperty("KinkFinding", m_kinkfinding = false);
    declareProperty("NumericalDerivs", m_numderiv = false);
    declareProperty("FitEnergyLoss", m_fiteloss = false);
    declareProperty("AsymmetricEnergyLoss", m_asymeloss = true);
    declareProperty("MinimumIterations", m_miniter = 1);
    // declareProperty("FastMaterial",m_fastmat=false);
    declareProperty("UseCaloTG", m_useCaloTG);
    declareProperty("CaloMaterialProvider", m_caloMaterialProvider);

#ifdef GXFDEBUGCODE
    declareProperty("Truth", m_truth = false);
    declareProperty("PRDTruthCollectionPixel", m_multiTruthCollectionPixelName = "PRD_MultiTruthPixel");
    declareProperty("PRDTruthCollectionSCT", m_multiTruthCollectionSCTName = "PRD_MultiTruthSCT");
    declareProperty("PRDTruthCollectionTRT", m_multiTruthCollectionTRTName = "PRD_MultiTruthTRT");
    declareProperty("PRDTruthCollectionMDT", m_multiTruthCollectionMDTName = "MDT_TruthMap");
    declareProperty("PRDTruthCollectionRPC", m_multiTruthCollectionRPCName = "RPC_TruthMap");
    declareProperty("PRDTruthCollectionTGC", m_multiTruthCollectionTGCName = "TGC_TruthMap");
    declareProperty("PRDTruthCollectionCSC", m_multiTruthCollectionCSCName = "CSC_TruthMap");
    m_barcode = 0;
#endif

    declareProperty("FixBrem", m_fixbrem = -1);
    declareProperty("RejectLargeNScat", m_rejectLargeNScat=false);

    declareInterface<IGlobalTrackFitter>(this);
    // m_miniter=1;
    m_fieldpropnofield = new MagneticFieldProperties(Trk::NoField);
    m_fieldpropfullfield = new MagneticFieldProperties(Trk::FullField);
    m_straightline = m_straightlineprop;
    m_fieldprop = m_straightline ? m_fieldpropnofield : m_fieldpropfullfield;

    if (GlobalChi2Fitter::m_derivpool.empty()) {
      GlobalChi2Fitter::m_derivpool.resize(100);
    }
    for (int i = 0; i < 100; i++) {
      m_derivpool[i] = HepMatrix(5, 50, 0);
    }

    m_fastmat = true;
    m_matvecmuonupstream = 0;
    m_matvecmuondownstream = 0;
    m_matvecidupstream = 0;
    m_matveciddownstream = 0;
  }

  StatusCode
  GlobalChi2Fitter::initialize() {
    if (m_ROTcreator.name() != "") {
      if (m_ROTcreator.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not get " << m_ROTcreator.type() << endmsg;
        return StatusCode::FAILURE;
      }
    }

    if (m_broadROTcreator.name() != "") {
      if (m_broadROTcreator.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not get " << m_broadROTcreator.type() << endmsg;
        return StatusCode::FAILURE;
      }
    }


    if (m_updator.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not get " << m_updator.type() << endmsg;
      return StatusCode::FAILURE;
    }

    if (m_extrapolator.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not get " << m_extrapolator.type() << endmsg;
      return StatusCode::FAILURE;
    }

    if (m_navigator.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not get " << m_navigator.type() << endmsg;
      return StatusCode::FAILURE;
    }

    if (m_residualPullCalculator.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not retrieve " << m_residualPullCalculator << " (to calculate residuals and pulls) " <<
        endmsg;
      return StatusCode::FAILURE;

      ;
    }

    if (m_propagator.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not get " << m_propagator.type() << endmsg;
      return StatusCode::FAILURE;
    }

    if (m_calomat) {
      if (m_calotool.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not get " << m_calotool.type() << endmsg;
        return StatusCode::FAILURE;
      }
      if (!m_calotoolparam.empty() && m_calotoolparam.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not get " << m_calotoolparam.type() << endmsg;
        return StatusCode::FAILURE;
      }
    }


    if (m_scattool.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not get " << m_scattool.type() << endmsg;
      return StatusCode::FAILURE;
    }

    if (m_elosstool.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not get " << m_elosstool.type() << endmsg;
      return StatusCode::FAILURE;
    }

    if (m_matupdator.name() != "" && m_matupdator.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not get " << m_matupdator.type() << endmsg;
      return StatusCode::FAILURE;
    }

    if (!m_fieldService.retrieve()) {
      ATH_MSG_FATAL("Failed to retrieve " << m_fieldService);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Retrieved " << m_fieldService);


    // need an Atlas id-helper to identify sub-detectors, take the one from detStore
    if (detStore()->retrieve(m_DetID, "AtlasID").isFailure()) {
      ATH_MSG_ERROR("Could not get AtlasDetectorID helper");
      return StatusCode::FAILURE;
    }

    if (m_fillderivmatrix && m_acceleration) {
      msg(MSG::WARNING) << "FillDerivativeMatrix option selected, switching off acceleration!" << endmsg;
      m_acceleration = false;
    }

    if (!m_trackingGeometrySvc.empty()) {
      StatusCode sc = m_trackingGeometrySvc.retrieve();
      if (sc.isFailure()) {
        msg(MSG::ERROR) << " failed to retrieve geometry Svc " << m_trackingGeometrySvc << endmsg;
        return StatusCode::FAILURE;
      }
      msg(MSG::INFO) << "  geometry Svc " << m_trackingGeometrySvc << " retrieved " << endmsg;
    }

    if (m_useCaloTG) {
      StatusCode sc = m_caloMaterialProvider.retrieve();
      if (sc.isFailure()) {
        msg(MSG::ERROR) << " failed to retrieve " << m_caloMaterialProvider << endmsg;
        return StatusCode::FAILURE;
      }
      msg(MSG::INFO) << m_caloMaterialProvider << " retrieved " << endmsg;
    }

    msg(MSG::INFO) << "fixed momentum: " << m_p << endmsg;
    m_inputPreparator = new TrackFitInputPreparator;

    m_nfits = 0;
    m_nsuccessfits = 0;
    m_matrixinvfailed = 0;
    m_notenoughmeas = 0;
    m_propfailed = 0;
    m_invalidangles = 0;
    m_notconverge = 0;
    m_highchi2 = 0;
    m_lowmomentum = 0;
    m_matfilled = false;
    m_idmat = true;
    m_hitcount = 0;
    m_energybalance = 0;
    return StatusCode::SUCCESS;
  }

  StatusCode
  GlobalChi2Fitter::finalize() {
    delete m_inputPreparator;

    msg(MSG::INFO) << "finalize()" << endmsg;
    msg(MSG::INFO) << m_nfits << " attempted track fits" << endmsg;
    msg(MSG::INFO) << m_nsuccessfits << " successful track fits" << endmsg;
    msg(MSG::INFO) << m_matrixinvfailed << " track fits failed because of a matrix inversion failure" << endmsg;
    msg(MSG::INFO) << m_notenoughmeas << " tracks were rejected by the outlier logic" << endmsg;
    msg(MSG::INFO) << m_propfailed << " track fits failed because of a propagation failure" << endmsg;
    msg(MSG::INFO) << m_invalidangles << " track fits failed because of an invalid angle (theta/phi)" << endmsg;
    msg(MSG::INFO) << m_notconverge << " track fits failed because the fit did not converge" << endmsg;
    msg(MSG::INFO) << m_highchi2 << " tracks did not pass the chi^2 cut" << endmsg;
    msg(MSG::INFO) << m_lowmomentum << " tracks were killed by the energy loss update" << endmsg;
    msg(MSG::INFO) << m_energybalance << " tracks failed the energy balance cut" << endmsg;
    return StatusCode::SUCCESS;
  }

// combined fit of two tracks
// --------------------------------
  Track *
  GlobalChi2Fitter::fit(const Track &intrk1,
                        const Track &intrk2,
                        const RunOutlierRemoval,
                        const ParticleHypothesis) const {
    ATH_MSG_DEBUG("--> entering GlobalChi2Fitter::fit(Track,Track,)");

    if (!m_straightlineprop) {
      m_straightline = (!m_fieldService->solenoidOn() && !m_fieldService->toroidOn());
    }
    m_fieldprop = m_straightline ? m_fieldpropnofield : m_fieldpropfullfield;
#ifdef GXFDEBUGCODE
    if (m_truth) {
      retrieveTruth();
    }
#endif
    bool firstismuon = false;
    const Track *indettrack = &intrk1;
    const Track *muontrack = &intrk2;
    int nmeas1 = (int) intrk1.measurementsOnTrack()->size();
    const RIO_OnTrack *testrot = dynamic_cast<const RIO_OnTrack *>((*intrk1.measurementsOnTrack())[nmeas1 - 1]);
    const CompetingRIOsOnTrack *testcrot = 0;
    if (!testrot) {
      testcrot = dynamic_cast<const CompetingRIOsOnTrack *>((*intrk1.measurementsOnTrack())[nmeas1 - 1]);
      if (testcrot) {
        testrot = &testcrot->rioOnTrack(0);
      }
    }
    if (!testrot) {
      testrot = dynamic_cast<const RIO_OnTrack *>((*intrk1.measurementsOnTrack())[nmeas1 - 2]);
      if (!testrot) {
        testcrot = dynamic_cast<const CompetingRIOsOnTrack *>((*intrk1.measurementsOnTrack())[nmeas1 - 2]);
        if (testcrot) {
          testrot = &testcrot->rioOnTrack(0);
        }
      }
    }
    if (testrot && !m_DetID->is_indet(testrot->identify()) && m_DetID->is_muon(testrot->identify())) {
      firstismuon = true;
      muontrack = &intrk1;
      indettrack = &intrk2;
    }
    bool muonisstraight = muontrack->info().trackProperties(TrackInfo::StraightTrack);
    // std::cout << "muon track: " << intrk2 << std::endl;
    bool measphi = false;

    for (int i = 0; i < (int) muontrack->measurementsOnTrack()->size(); i++) {
      const CompetingRIOsOnTrack *crot =
        dynamic_cast<const CompetingRIOsOnTrack *>((*muontrack->measurementsOnTrack())[i]);
      const RIO_OnTrack *rot = 0;
      if (crot) {
        rot = &crot->rioOnTrack(0);
      } else {
        rot = dynamic_cast<const RIO_OnTrack *>((*muontrack->measurementsOnTrack())[i]);
      }
      if (rot && !m_DetID->is_mdt(rot->identify())) {
        const Surface *surf = &rot->associatedSurface();
        Amg::Vector3D measdir = surf->transform().rotation().col(0);
        double dotprod1 = measdir.dot(Amg::Vector3D(0, 0, 1));
        double dotprod2 = measdir.dot(Amg::Vector3D(surf->center().x(), surf->center().y(), 0) / surf->center().perp());
        if (std::abs(dotprod1) < .5 && std::abs(dotprod2) < .5) {
          measphi = true;
          break;
        }
      }
    }

    const IPropagator *prop = &*m_propagator;
    const TrackParameters *firstidpar = 0, *lastidpar = 0;

    DataVector<const TrackParameters>::const_iterator parit = indettrack->trackParameters()->begin();

    while (!firstidpar && parit != indettrack->trackParameters()->end()) {
      if ((**parit).covariance() && !dynamic_cast<const PerigeeSurface *>(&(**parit).associatedSurface())) {
        firstidpar = *parit;
      }
      parit++;
    }

    parit = indettrack->trackParameters()->end();
    do {
      parit--;
      if ((**parit).covariance() && !dynamic_cast<const PerigeeSurface *>(&(**parit).associatedSurface())) {
        lastidpar = *parit;
      }
    } while (!lastidpar && parit != indettrack->trackParameters()->begin());

    if (!firstidpar || !lastidpar) {
      return 0;
    }

    const TrackParameters *parforcalo = firstismuon ? firstidpar : lastidpar;
    if (!m_fieldService->solenoidOn()) {
      const AmgVector(5) &newpars = parforcalo->parameters();
      parforcalo = parforcalo->associatedSurface().createTrackParameters(newpars[0], newpars[1], newpars[2], newpars[3],
                                                                         1 / 5000., 0);
    }

    std::vector<MaterialEffectsOnTrack> calomeots;
    if (!m_useCaloTG) {
      if (!m_calotool.empty()) {
        calomeots = m_calotool->extrapolationSurfacesAndEffects(
          *m_navigator->highestVolume(), *prop, *parforcalo,
          parforcalo->associatedSurface(), Trk::anyDirection, Trk::muon);
      }
    }else {
      msg(MSG::VERBOSE) << "Updating Calorimeter TSOS in Muon Combined Fit ..." << endmsg;
      m_caloMaterialProvider->getCaloMEOT(*indettrack, *muontrack, calomeots);
    }


    // std::cout << "eloss: " << calomeots[1].energyLoss()->deltaE() << std::endl;
    if (firstismuon) {
      std::reverse(calomeots.begin(), calomeots.end());
    }
    if (calomeots.empty()) {
      msg(MSG::WARNING) << "No calorimeter material collected, failing fit" << endmsg;
      return 0;
    }

    Track *track = 0;
    bool tmp = m_calomat;
    m_calomat = false;
    bool tmp2 = m_extmat;
    bool tmp4 = m_idmat;

    const TrackParameters *measperid = indettrack->perigeeParameters();
    const TrackParameters *measpermuon = muontrack->perigeeParameters();
    double qoverpid = measperid ? measperid->parameters()[Trk::qOverP] : 0;
    double qoverpmuon = measpermuon ? measpermuon->parameters()[Trk::qOverP] : 0;
    const AmgSymMatrix(5) * errmatid = measperid ? measperid->covariance() : 0;
    const AmgSymMatrix(5) * errmatmuon = measpermuon ? measpermuon->covariance() : 0;
    // std::cout << "firstismuon: " << firstismuon << " measperid: " << measperid << " measpermuon: " << measpermuon <<
    // " pmuon: " << 1/qoverpmuon << std::endl;
    // bool isparametrized=false;
    if (!firstismuon && errmatid && errmatmuon && qoverpmuon != 0 && !m_calotoolparam.empty() && !m_useCaloTG) {
      double piderror = sqrt((*errmatid)(4, 4)) / (qoverpid * qoverpid);
      double pmuonerror = sqrt((*errmatmuon)(4, 4)) / (qoverpmuon * qoverpmuon);
      double energyerror = sqrt(
        calomeots[1].energyLoss()->sigmaDeltaE() * calomeots[1].energyLoss()->sigmaDeltaE() + piderror * piderror + pmuonerror *
        pmuonerror);
      // std::cout << "energyerror: " << energyerror << " p id: " << 1/qoverpid << " p muon: " << 1/qoverpmuon << "
      // eloss: " << std::abs(calomeots[1].energyLoss()->deltaE()) << std::endl;
      if ((std::abs(calomeots[1].energyLoss()->deltaE()) - std::abs(1 / qoverpid) + std::abs(1 / qoverpmuon)) /
          energyerror > 5) {
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "Changing from measured to parametrized energy loss" << endmsg;
        }
        calomeots = m_calotoolparam->extrapolationSurfacesAndEffects(
          *m_navigator->highestVolume(), *prop, *parforcalo,
          parforcalo->associatedSurface(), Trk::anyDirection, Trk::muon);
        // isparametrized=true;
        if (calomeots.empty()) {
          msg(MSG::WARNING) << "No calorimeter material collected, failing fit" << endmsg;
          if (!m_fieldService->solenoidOn()) {
            delete parforcalo;
          }
          return 0;
        }
      }
    }
    if (!m_fieldService->solenoidOn()) {
      delete parforcalo;
    }

    int nfits = m_nfits;
    bool firstfitwasattempted = false;

    GXFTrajectory trajectory;
    if (!m_caloEntrance) {
      m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();
      if (m_trackingGeometry) {
        m_caloEntrance = m_trackingGeometry->trackingVolume("InDet::Containers::InnerDetector");
      }
      if (!m_caloEntrance) {
        msg(MSG::ERROR) << "calo entrance not available" << endmsg;
        return 0;
      }
    }

    if ((!m_fieldService->toroidOn() && !m_fieldService->solenoidOn()) ||
        (m_getmaterialfromtrack && !muonisstraight && measphi &&
         muontrack->info().trackFitter() != Trk::TrackInfo::Unknown && qoverpid * qoverpmuon > 0)) {
      track = mainCombinationStrategy(intrk1, intrk2, trajectory, calomeots);
      if (m_nfits == nfits + 1) {
        firstfitwasattempted = true;
      }
      if (!track) {
        m_hitcount = 0;
      }
    }
    m_extmat = tmp2;
    m_idmat = tmp4;

    if (!track && !firstfitwasattempted && (m_fieldService->toroidOn() || m_fieldService->solenoidOn())) {
      GXFTrajectory trajectory2;
      trajectory = trajectory2;
      track = backupCombinationStrategy(intrk1, intrk2, trajectory, calomeots);
      /* if (!track && !isparametrized && m_fittercode!=FitterStatusCode::Success) {
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Changing from measured to parametrized energy loss" << endmsg;
         calomeots=m_calotoolparam->extrapolationSurfacesAndEffects(*m_navigator->highestVolume(),*prop,
            *parforcalo,*parforcalo->associatedSurface(),Trk::anyDirection,Trk::muon);
         isparametrized=true;
         if (calomeots.size()==3){
          trajectory=trajectory2;
          track=backupCombinationStrategy(intrk1,intrk2,trajectory,calomeots);
         }
         } */
    }

    /* int index=(int)trajectory.residuals().size()-trajectory.numberOfBrems();
       double calopull=std::abs(trajectory.residuals()[index]/trajectory.errors()[index]);
       double thept= track ? track->perigeeParameters()->pT() : 999999999;
       bool elossok=true;
       if (m_fieldService->solenoidOn() && m_fieldService->toroidOn() && track &&
          track->trackParameters()->front()->charge()!=track->trackParameters()->back()->charge()) elossok=false;
       if (!firstismuon && m_fieldService->toroidOn() && thept<20000 && (calopull>5 || (calopull>3 && thept<10000)))
          elossok=false;
       if (!elossok) {
       delete track;
       track=0;
       m_energybalance++;
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Track failed energy balance cut" << endmsg;
       }  */
    bool pseudoupdated = false;
    if (track) {
      for (int i = 0; i < (int) trajectory.trackStates().size(); i++) {
        GXFTrackState *pseudostate = trajectory.trackStates()[i];
        if (!pseudostate) {
          continue; // avoid coverity defect
        }
        if (pseudostate->measurementType() != TrackState::Pseudo ||
            pseudostate->trackStateType() != TrackState::Fittable) {
          continue;
        }
        if (!pseudostate || pseudostate->fitQuality()->chiSquared() < 10) {
          continue;
        }
        const TrackParameters *pseudopar = pseudostate->trackParameters();
        const TrackParameters *updpar = m_updator->removeFromState(*pseudopar,
                                                                   pseudostate->measurement()->localParameters(),
                                                                   pseudostate->measurement()->localCovariance());
        if (!updpar) {
          continue;
        }
        Amg::MatrixX covMatrix(1, 1);
        covMatrix(0, 0) = 100;
        PseudoMeasurementOnTrack *newpseudo =
          new PseudoMeasurementOnTrack(LocalParameters(DefinedParameter(updpar->parameters()[Trk::locY], Trk::locY)),
                                       covMatrix, pseudopar->associatedSurface());
        delete updpar;
        pseudostate->setMeasurement(newpseudo);
        double errors[5];
        errors[0] = errors[2] = errors[3] = errors[4] = -1;
        errors[1] = 10;
        pseudostate->setMeasurementErrors(errors);
        pseudoupdated = true;
      }
      if (pseudoupdated) {
        Track *oldtrack = track;
        trajectory.setConverged(false);
        m_matfilled = true;
        track =
          myfit(trajectory, *oldtrack->perigeeParameters(), false,
                (m_fieldService->toroidOn() || m_fieldService->solenoidOn()) ? muon : nonInteracting);
        m_matfilled = false;
        delete oldtrack;
      }
    }

    m_nfits = nfits + 1;
    if (track) {
      track->info().addPatternReco(intrk1.info());
      track->info().addPatternReco(intrk2.info());
      m_nsuccessfits++;
    }
    m_calomat = tmp;
    m_extmat = tmp2;
    m_idmat = tmp4;
    // if (track) std::cout << "track: " << *track << std::endl;
    cleanup();
    return track;
  }

  Track *
  GlobalChi2Fitter::mainCombinationStrategy(const Track &intrk1,
                                            const Track &intrk2,
                                            GXFTrajectory &trajectory,
                                            std::vector<MaterialEffectsOnTrack> &calomeots) const {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "--> entering GlobalChi2Fitter::mainCombinationStrategy" << endmsg;
    }

    bool firstismuon = false;
    double mass = m_particleMasses.mass[muon];

    const Track *indettrack = &intrk1;
    const Track *muontrack = &intrk2;
    int nmeas1 = (int) intrk1.measurementsOnTrack()->size();
    const RIO_OnTrack *testrot = dynamic_cast<const RIO_OnTrack *>((*intrk1.measurementsOnTrack())[nmeas1 - 1]);
    const CompetingRIOsOnTrack *testcrot = 0;
    if (!testrot) {
      testcrot = dynamic_cast<const CompetingRIOsOnTrack *>((*intrk1.measurementsOnTrack())[nmeas1 - 1]);
      if (testcrot) {
        testrot = &testcrot->rioOnTrack(0);
      }
    }
    if (!testrot) {
      testrot = dynamic_cast<const RIO_OnTrack *>((*intrk1.measurementsOnTrack())[nmeas1 - 2]);
      if (!testrot) {
        testcrot = dynamic_cast<const CompetingRIOsOnTrack *>((*intrk1.measurementsOnTrack())[nmeas1 - 2]);
        if (testcrot) {
          testrot = &testcrot->rioOnTrack(0);
        }
      }
    }
    if (testrot && !m_DetID->is_indet(testrot->identify()) && m_DetID->is_muon(testrot->identify())) {
      firstismuon = true;
      muontrack = &intrk1;
      indettrack = &intrk2;
    }
    const TrackParameters *firstidpar = 0, *lastidpar = 0;
    DataVector<const TrackParameters>::const_iterator parit = indettrack->trackParameters()->begin();

    while (!firstidpar && parit != indettrack->trackParameters()->end()) {
      if ((**parit).covariance() && !dynamic_cast<const PerigeeSurface *>(&(**parit).associatedSurface())) {
        firstidpar = *parit;
      }
      parit++;
    }
    parit = indettrack->trackParameters()->end();
    do {
      parit--;
      if ((**parit).covariance() && !dynamic_cast<const PerigeeSurface *>(&(**parit).associatedSurface())) {
        lastidpar = *parit;
      }
    } while (!lastidpar && parit != indettrack->trackParameters()->begin());

    if (!firstidpar || !lastidpar) {
      return 0;
    }

    DataVector<const TrackStateOnSurface>::const_iterator tsosit =
      firstismuon ? muontrack->trackStateOnSurfaces()->end() : muontrack->trackStateOnSurfaces()->begin();
    if (firstismuon) {
      tsosit--;
    }
    const MeasurementBase *closestmuonmeas = 0;   // muon hit closest to ID track
    const TrackParameters *tp_closestmuon = 0;  // muon track parameters closest to ID track


    while (!closestmuonmeas) {
      closestmuonmeas = 0;
      const TrackParameters *thispar = (**tsosit).trackParameters();
      if ((**tsosit).measurementOnTrack()) {
        closestmuonmeas = (**tsosit).measurementOnTrack();
        if (thispar) {
          const AmgVector(5) &parvec = thispar->parameters();
          tp_closestmuon = thispar->associatedSurface().createTrackParameters(parvec[0], parvec[1], parvec[2],
                                                                              parvec[3], parvec[4], 0);
        }
        break;
      }
      if (firstismuon) {
        tsosit--;
      } else {
        tsosit++;
      }
    }

    PropDirection propdir = firstismuon ? Trk::alongMomentum : oppositeMomentum;
    const TrackParameters *tmppar = 0;
    if (!m_msEntrance) {
      m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();
      if (m_trackingGeometry) {
        m_msEntrance = m_trackingGeometry->trackingVolume("MuonSpectrometerEntrance");
      }
      if (!m_msEntrance) {
        msg(MSG::ERROR) << "MS entrance not available" << endmsg;
      }
    }
    if (tp_closestmuon && m_msEntrance) {
      tmppar = m_extrapolator->extrapolateToVolume(*tp_closestmuon, *m_msEntrance, propdir, nonInteracting);
    }
    const std::vector<const TrackStateOnSurface *> *matvec = 0;
    if (tmppar) {
      Surface *muonsurf = 0;
      const CylinderSurface *cylmuonsurf = dynamic_cast<const CylinderSurface *>(&tmppar->associatedSurface());
      const DiscSurface *discmuonsurf = dynamic_cast<const DiscSurface *>(&tmppar->associatedSurface());
      if (cylmuonsurf) {
        Amg::Transform3D *trans = new Amg::Transform3D(cylmuonsurf->transform());
        const CylinderBounds &cylbounds = cylmuonsurf->bounds();
        double radius = cylbounds.r();
        double hlength = cylbounds.halflengthZ();
        muonsurf = new CylinderSurface(trans, radius + 1, hlength);
      }else if (discmuonsurf) {
        double newz =
          ((discmuonsurf->center().z() > 0) ? discmuonsurf->center().z() + 1 : discmuonsurf->center().z() - 1);
        Amg::Vector3D newpos(discmuonsurf->center().x(), discmuonsurf->center().y(), newz);
        Amg::Transform3D *trans = new Amg::Transform3D(discmuonsurf->transform());
        trans->translation() << newpos;
        const DiscBounds *discbounds = dynamic_cast<const DiscBounds *>(&discmuonsurf->bounds());
        if (discbounds) {
          double rmin = discbounds->rMin();
          double rmax = discbounds->rMax();
          muonsurf = new DiscSurface(trans, rmin, rmax);
        }
      }
      if (muonsurf) {
        matvec = m_extrapolator->extrapolateM(*tp_closestmuon, *muonsurf, propdir, false, muon);
        delete muonsurf;
      }

      delete tmppar;
    }

    if (matvec && !matvec->empty()) {
      for (int i = 0; i < (int) m_matvec.size(); i++) {
        delete m_matvec[i];
      }
      m_matvec = *matvec;
      delete matvec;
      delete m_matvec.back();
      m_matvec.pop_back();
      for (int i = 0; i < (int) m_matvec.size(); i++) {
        propdir = firstismuon ? Trk::alongMomentum : oppositeMomentum;
        const MaterialEffectsOnTrack *meff =
          dynamic_cast<const MaterialEffectsOnTrack *>(m_matvec[i]->materialEffectsOnTrack());
        if (!meff) {
          continue; // avoid coverity defect
        }
        const Surface *matsurf = &meff->associatedSurface();
        tmppar = m_propagator->propagateParameters(*tp_closestmuon, *matsurf, propdir, false, *m_fieldprop,
                                                   Trk::nonInteracting);
        if (!tmppar) {
          propdir = !firstismuon ? Trk::alongMomentum : oppositeMomentum;
          tmppar = m_propagator->propagateParameters(*tp_closestmuon, *matsurf, propdir, false, *m_fieldprop,
                                                     Trk::nonInteracting);
        }
        delete tp_closestmuon;
        if (!tmppar) {
          return 0;
        }
        AmgVector(5) newpars = tmppar->parameters();
        if (newpars[Trk::qOverP] != 0) {
          double sign = (newpars[Trk::qOverP] > 0) ? 1 : -1;
          double de = std::abs(meff->energyLoss()->deltaE());
          double oldp = std::abs(1 / newpars[Trk::qOverP]);
          double newp2 = oldp * oldp + (!firstismuon ? 2 : -2) * de * sqrt(mass * mass + oldp * oldp) + de * de;
          if (newp2 > 0) {
            newpars[Trk::qOverP] = sign / sqrt(newp2);
          }
        }
        tp_closestmuon = tmppar->associatedSurface().createTrackParameters(newpars[0], newpars[1], newpars[2],
                                                                           newpars[3], newpars[4], 0);
        delete tmppar;
      }
      if (!firstismuon) {
        std::reverse(m_matvec.begin(), m_matvec.end());
      }
    }else {
      delete matvec;
    }

    DataVector<const TrackStateOnSurface>::const_iterator beginStates = intrk1.trackStateOnSurfaces()->begin();
    DataVector<const TrackStateOnSurface>::const_iterator itStates = beginStates;
    DataVector<const TrackStateOnSurface>::const_iterator endState = firstismuon ? tsosit +
                                                                     1 : intrk1.trackStateOnSurfaces()->end();
    DataVector<const TrackStateOnSurface>::const_iterator beginStates2 =
      !firstismuon ? tsosit : intrk2.trackStateOnSurfaces()->begin();
    DataVector<const TrackStateOnSurface>::const_iterator itStates2 = beginStates2;
    DataVector<const TrackStateOnSurface>::const_iterator endState2 = intrk2.trackStateOnSurfaces()->end();


    for (; itStates != endState; ++itStates) {
      if (firstismuon && dynamic_cast<const PseudoMeasurementOnTrack *>((*itStates)->measurementOnTrack())) {
        continue;
      }
      bool tmpgetmat = m_getmaterialfromtrack;
      if ((*itStates)->materialEffectsOnTrack()) {
        if (firstismuon) {
          m_extmat = false;
        } else {
          m_idmat = false;
        }
        const MaterialEffectsOnTrack *meot =
          dynamic_cast<const MaterialEffectsOnTrack *>((*itStates)->materialEffectsOnTrack());
        if (meot && (!meot->scatteringAngles() || !meot->energyLoss())) {
          m_getmaterialfromtrack = true;  // always take calorimeter layers
        }
      }
      makeProtoState(trajectory, *itStates);
      m_getmaterialfromtrack = tmpgetmat;
    }
    if (!firstismuon && intrk1.info().trackProperties(TrackInfo::SlimmedTrack)) {
      trajectory.trackStates().back()->setTrackParameters(0);
    }
    const TrackParameters *firstscatpar = 0, *lastscatpar = 0, *origlastidpar = lastidpar, *origfirstidpar = firstidpar;
    double newqoverpid = 0;

    if (!firstismuon) {
      double de = std::abs(calomeots[1].energyLoss()->deltaE());
      double sigmade = std::abs(calomeots[1].energyLoss()->sigmaDeltaE());

      double pbefore = std::abs(1 / firstidpar->parameters()[Trk::qOverP]);
      double pafter = std::abs(1 / tp_closestmuon->parameters()[Trk::qOverP]);
      double elosspull = (pbefore - pafter - de) / sigmade;
      if (std::abs(elosspull) > 10) {
        // if (!firstismuon) {
        if (elosspull > 10) {
          newqoverpid = 1 / (de + pafter + 10 * sigmade);
        } else {
          newqoverpid = 1 / (de + pafter - 10 * sigmade);
        }
        /* }
           else {
           if (elosspull>10) newqoverpid=1/(-de+pbefore-10*sigmade);
           else newqoverpid=1/(-de+pbefore+10*sigmade);
           } */
        if (tp_closestmuon->parameters()[Trk::qOverP] * newqoverpid < 0) {
          newqoverpid *= -1;
        }
        const AmgVector(5) &newpar = firstidpar->parameters();
        firstidpar = firstidpar->associatedSurface().createTrackParameters(newpar[0], newpar[1], newpar[2], newpar[3],
                                                                           newqoverpid, 0);
      }
      // else newqoverpid=idscatpar->parameters()[Trk::qOverP];
      lastidpar = m_extrapolator->extrapolateToVolume(*firstidpar, *m_caloEntrance, alongMomentum, Trk::muon);
    }

    if (!lastidpar) {
      lastidpar = origlastidpar;
    }
    firstscatpar = m_propagator->propagateParameters(*(firstismuon ? tp_closestmuon : lastidpar),
                                                     calomeots[0].associatedSurface(), Trk::alongMomentum, false, *m_fieldprop,
                                                     Trk::nonInteracting);
    if (lastidpar != origlastidpar) {
      delete lastidpar;
    }
    if (firstidpar != origfirstidpar) {
      delete firstidpar;
    }

    if (!firstscatpar) {
      delete tp_closestmuon;
      return 0;
    }
    lastscatpar = m_propagator->propagateParameters(*(firstismuon ? firstidpar : tp_closestmuon),
                                                    calomeots[2].associatedSurface(), Trk::oppositeMomentum, false, *m_fieldprop,
                                                    Trk::nonInteracting);

    if (!lastscatpar) {
      delete tp_closestmuon;
      delete firstscatpar;
      return 0;
    }

    TransportJacobian *jac1 = 0, *jac2 = 0;
    std::unique_ptr<const TrackParameters> elosspar;
    double firstscatphi = 0, secondscatphi = 0, firstscattheta = 0, secondscattheta = 0, muonscatphi = 0,
           muonscattheta = 0;
    const TrackParameters *idscatpar = !firstismuon ? firstscatpar : lastscatpar;
    const TrackParameters *muonscatpar = firstismuon ? firstscatpar : lastscatpar;
    newqoverpid = idscatpar->parameters()[Trk::qOverP];
    Amg::Vector3D calosegment = lastscatpar->position() - firstscatpar->position();
    muonscatphi = calosegment.phi() - muonscatpar->parameters()[Trk::phi];
    if (std::abs(std::abs(muonscatphi) - 2 * M_PI) < std::abs(muonscatphi)) {
      muonscatphi += (muonscatphi < 0 ? 2 * M_PI : -2 * M_PI);
    }
    muonscattheta = calosegment.theta() - muonscatpar->parameters()[Trk::theta];
    // std::cout << "idscatpar: " << *idscatpar << " radius: " << idscatpar->position().perp() << " muonscatpar: " <<
    // *muonscatpar << " radius: " << muonscatpar->position().perp() << std::endl;
    const TrackParameters *startPar = m_idmat ? lastidpar : indettrack->perigeeParameters();

    if (m_fieldService->toroidOn()) {
      // double oldp=std::abs(1/lastscatpar->parameters()[Trk::qOverP]);

      // newqoverpid=1/sqrt(oldp*oldp+2*de*sqrt(mass*mass+oldp*oldp)+de*de);
      // newqoverpid=idscatpar->parameters()[Trk::qOverP];
    }
    for (int i = 0; i < 2; i++) {
      const TrackParameters *tmpelosspar = 0;
      AmgVector(5) params1 = muonscatpar->parameters();
      params1[Trk::phi] += muonscatphi;
      params1[Trk::theta] += muonscattheta;

      if (!correctAngles(params1[Trk::phi], params1[Trk::theta])) {
        delete tp_closestmuon;
        delete firstscatpar;
        delete lastscatpar;
        return 0;
      }
      const TrackParameters *tmppar1 = muonscatpar->associatedSurface().createTrackParameters(params1[0], params1[1],
                                                                                              params1[2], params1[3],
                                                                                              params1[4], 0);
      PropDirection propdir = !firstismuon ? oppositeMomentum : alongMomentum;
      tmpelosspar = m_propagator->propagateParameters(*tmppar1,
                                                      calomeots[1].associatedSurface(), propdir, false, *m_fieldprop, jac1,
                                                      Trk::nonInteracting);
      if (m_numderiv) {
        delete jac1;
        jac1 = numericalDerivatives(firstscatpar, &calomeots[1].associatedSurface(), propdir);
      }
      delete tmppar1;

      if (!tmpelosspar || !jac1) {
        delete jac1;
        delete tp_closestmuon;
        delete firstscatpar;
        delete lastscatpar;
        if (tmpelosspar != elosspar.get()) {
          delete tmpelosspar;
        }
        // @TODO
        // according to coverity elosspar cannot be non NULL here
        // because elosspar is initially NULL and only set in the last loop iteration  (i==1)
        // is this intended ?
        // delete elosspar;
        return 0;
      }

      const AmgVector(5) &newpars = tmpelosspar->parameters();
      // newpars[Trk::qOverP]= tp_closestmuon->parameters()[Trk::qOverP];
      const TrackParameters *elosspar2 = tmpelosspar->associatedSurface().createTrackParameters(newpars[0], newpars[1],
                                                                                                newpars[2], newpars[3],
                                                                                                newqoverpid, 0);
      if (i == 1) {
        elosspar.reset(tmpelosspar);
      } else {
        delete tmpelosspar;
      }
      const TrackParameters *scat2 = m_propagator->propagateParameters(*elosspar2,
                                                                       !firstismuon ? calomeots[0].associatedSurface() : calomeots[
                                                                         2].associatedSurface(), propdir, false, *m_fieldprop, jac2,
                                                                       Trk::nonInteracting);
      if (m_numderiv) {
        delete jac2;
        jac2 = numericalDerivatives(elosspar2,
                                    !firstismuon ? &calomeots[0].associatedSurface() : &calomeots[2].associatedSurface(),
                                    !firstismuon ? Trk::oppositeMomentum : Trk::alongMomentum);
      }
      delete elosspar2;
      if (!scat2 || !jac2) {
        delete scat2;
        delete jac1;
        delete jac2;
        delete tp_closestmuon;
        delete firstscatpar;
        delete lastscatpar;
        return 0;
      }
      double jac3[5][5];
      for (int j = 0; j < 5; j++) {
        for (int k = 0; k < 5; k++) {
          jac3[j][k] = 0;
          for (int l = 0; l < 5; l++) {
            jac3[j][k] += (*jac2)(j, l) * (*jac1)(l, k);
          }
        }
      }
      delete jac1;
      delete jac2;
      jac1 = jac2 = 0;
      HepMatrix jac4(2, 2, 0);
      jac4[0][0] = jac3[0][2];
      jac4[1][1] = jac3[1][3];
      jac4[0][1] = jac3[0][3];
      jac4[1][0] = jac3[1][2];
      int ierr = 0;
      jac4.invert(ierr);
      double dloc1 = idscatpar->parameters()[Trk::loc1] - scat2->parameters()[Trk::loc1];
      double dloc2 = idscatpar->parameters()[Trk::loc2] - scat2->parameters()[Trk::loc2];
      const Trk::CylinderSurface *cylsurf = dynamic_cast<const Trk::CylinderSurface *>(&scat2->associatedSurface());
      const Trk::DiscSurface *discsurf = dynamic_cast<const Trk::DiscSurface *>(&scat2->associatedSurface());

      if (cylsurf) {
        double length = 2 * M_PI * cylsurf->bounds().r();
        if (fabs(fabs(dloc1) - length) < fabs(dloc1)) {
          if (dloc1 > 0) {
            dloc1 -= length;
          } else {
            dloc1 += length;
          }
        }
      }
      if (discsurf) {
        if (fabs(fabs(dloc2) - 2 * M_PI) < fabs(dloc2)) {
          if (dloc2 > 0) {
            dloc2 -= 2 * M_PI;
          } else {
            dloc2 += 2 * M_PI;
          }
        }
      }
      double dphi = jac4[0][0] * dloc1 + jac4[0][1] * dloc2;
      double dtheta = jac4[1][0] * dloc1 + jac4[1][1] * dloc2;
      // std::cout << "i: " << i << " dphi: " << dphi << " dtheta: " << dtheta << " distance: " <<
      // (idscatpar->position()-scat2->position()).mag() << " loc1: " << idscatpar->parameters()[Trk::loc1] << " " <<
      // scat2->parameters()[Trk::loc1] << " " << dloc1 << " loc2: " << idscatpar->parameters()[Trk::loc2] << " " <<
      // scat2->parameters()[Trk::loc2] << " " << dloc2 << std::endl;
      if (i == 1) {
        dphi = dtheta = 0;
      }
      muonscatphi += dphi;
      muonscattheta += dtheta;
      double idscatphi = idscatpar->parameters()[Trk::phi] - (scat2->parameters()[Trk::phi] + dphi);
      if (std::abs(std::abs(idscatphi) - 2 * M_PI) < std::abs(idscatphi)) {
        idscatphi += ((idscatphi < 0) ? 2 * M_PI : -2 * M_PI);
      }
      double idscattheta = idscatpar->parameters()[Trk::theta] - (scat2->parameters()[Trk::theta] + dtheta);

      if (firstismuon) {
        firstscatphi = muonscatphi;
        secondscatphi = idscatphi;
        firstscattheta = muonscattheta;
        secondscattheta = idscattheta;
      }else {
        firstscatphi = -idscatphi;
        secondscatphi = -muonscatphi;
        firstscattheta = -idscattheta;
        secondscattheta = -muonscattheta;
      }
      if (i == 1 && m_fieldService->toroidOn() && !firstismuon) {
        AmgVector(5) params2 = scat2->parameters();
        params2[Trk::phi] += idscatphi;
        params2[Trk::theta] += idscattheta;
        // params2[Trk::qOverP]=idscatpar->parameters()[Trk::qOverP];
        // params2[Trk::phi]=(idscatpar->position()-firstidpar->position()).phi();
        // params2[Trk::theta]=idscattheta;
        if (!correctAngles(params2[Trk::phi], params2[Trk::theta])) {
          delete scat2;
          delete tp_closestmuon;
          delete firstscatpar;
          delete lastscatpar;
          return 0;
        }

        const TrackParameters *tmpidpar = scat2->associatedSurface().createTrackParameters(params2[0], params2[1],
                                                                                           params2[2], params2[3],
                                                                                           params2[4], 0);
        delete idscatpar;
        idscatpar = firstscatpar = tmpidpar;

        startPar = m_extrapolator->extrapolateToVolume(*idscatpar, *m_caloEntrance, oppositeMomentum,
                                                       Trk::nonInteracting);
        if (startPar) {
          Amg::Vector3D trackdir = startPar->momentum().unit();
          Amg::Vector3D curvZcrossT = -(trackdir.cross(Amg::Vector3D(0, 0, 1)));
          Amg::Vector3D curvU = curvZcrossT.unit();
          Amg::Vector3D curvV = trackdir.cross(curvU);
          Amg::RotationMatrix3D rot = Amg::RotationMatrix3D::Identity();
          rot.col(0) = curvU;
          rot.col(1) = curvV;
          rot.col(2) = trackdir;
          Amg::Transform3D *trans = new Amg::Transform3D;
          trans->linear().matrix() << rot;
          trans->translation() << startPar->position() - .1 *trackdir;
          PlaneSurface curvlinsurf(trans);
          const TrackParameters *curvlinpar = m_extrapolator->extrapolateDirectly(*startPar, curvlinsurf,
                                                                                  Trk::oppositeMomentum,
                                                                                  Trk::nonInteracting);
          if (curvlinpar) {
            delete startPar;
            startPar = curvlinpar;
          }
        }
        delete firstscatpar;
        firstscatpar = scat2;
      }else {
        delete scat2;
      }
    }

    delete tp_closestmuon;


    GXFMaterialEffects *firstscatmeff = new GXFMaterialEffects(&calomeots[0]);
    GXFMaterialEffects *elossmeff = new GXFMaterialEffects(&calomeots[1]);
    GXFMaterialEffects *secondscatmeff = new GXFMaterialEffects(&calomeots[2]);

    double pull1 = fabs(firstscatphi / firstscatmeff->sigmaDeltaPhi());
    double pull2 = fabs(secondscatphi / secondscatmeff->sigmaDeltaPhi());

    if (firstismuon) {
      for (int i = 0; i < (int) m_matvec.size(); i++) {
        makeProtoState(trajectory, m_matvec[i], -1, true);
      }
    }

    firstscatmeff->setScatteringAngles(firstscatphi, firstscattheta);
    secondscatmeff->setScatteringAngles(secondscatphi, secondscattheta);


//  if (m_fieldService->toroidOn()){
    // if (!firstismuon)
    // elossmeff->setdelta_p(0.001*(1/std::abs(lastscatpar->parameters()[Trk::qOverP])-1/std::abs(newqoverpid)));
    // else elossmeff->setdelta_p(0.001*(1/std::abs(newqoverpid)-1/std::abs(firstscatpar->parameters()[Trk::qOverP])));
    if (!firstismuon) {
      elossmeff->setdelta_p(1000 * (lastscatpar->parameters()[Trk::qOverP] - newqoverpid));
    } else {
      elossmeff->setdelta_p(1000 * (newqoverpid - firstscatpar->parameters()[Trk::qOverP]));
    }

    elossmeff->setSigmaDeltaE(calomeots[1].energyLoss()->sigmaDeltaE());

//  }
    trajectory.addMaterialState(new GXFTrackState(firstscatmeff, firstscatpar), -1, true);
    trajectory.addMaterialState(new GXFTrackState(elossmeff, elosspar.release()), -1, true);
    trajectory.addMaterialState(new GXFTrackState(secondscatmeff, lastscatpar), -1, true);

    if (!firstismuon) {
      for (int i = 0; i < (int) m_matvec.size(); i++) {
        makeProtoState(trajectory, m_matvec[i], -1, true);
      }
    }

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "pull1: " << pull1 << " pull2: " << pull2 << endmsg;
    }
    if (!startPar) {
      return 0;
    }
    if ((pull1 > 5 || pull2 > 5) &&
        (pull1 > 25 || pull2 > 25 ||
         dynamic_cast<const StraightLineSurface *>(&closestmuonmeas->associatedSurface()))) {
      if (startPar != lastidpar && startPar != indettrack->perigeeParameters()) {
        delete startPar;
      }
      return 0;
    }

    // GXFTrackState *firstpseudostate=0,*secondpseudostate=0;
    bool largegap = false; // special treatment for tracks missing the EM station
    double previousz = 0;
    for (itStates2 = beginStates2; itStates2 != endState2; ++itStates2) {
      const MaterialEffectsBase *meff = (*itStates2)->materialEffectsOnTrack();
      const TrackParameters *tpar = (*itStates2)->trackParameters();
      const MeasurementBase *meas = (*itStates2)->measurementOnTrack();
      if (meff) {
        if (!firstismuon) {
          const MaterialEffectsOnTrack *mefot = dynamic_cast<const MaterialEffectsOnTrack *>(meff);
          if (mefot && mefot->energyLoss() && std::abs(mefot->energyLoss()->deltaE()) > 250 &&
              mefot->energyLoss()->sigmaDeltaE() < 1.e-9) {
            if (startPar != lastidpar && startPar != indettrack->perigeeParameters()) {
              delete startPar;
            }
            return 0;
          }
          m_extmat = false;
        }else {
          m_idmat = false;
        }
      }


      bool ispseudo = (dynamic_cast<const PseudoMeasurementOnTrack *>(meas));
      if (ispseudo && !(itStates2 == beginStates2 || itStates2 == beginStates2 + 1) && !largegap) {
        continue;
      }
      makeProtoState(trajectory, *itStates2);

      if (itStates2 == endState2 - 1 && dynamic_cast<const AtaStraightLine *>(tpar) && tpar->position().perp() > 9000 &&
          std::abs(tpar->position().z()) < 13000) {
        const TrackParameters *pseudopar = tpar->clone();
        Amg::MatrixX covMatrix(1, 1);
        covMatrix(0, 0) = 100;
        PseudoMeasurementOnTrack *newpseudo =
          new PseudoMeasurementOnTrack(LocalParameters(DefinedParameter(pseudopar->parameters()[Trk::locY], Trk::locY)),
                                       covMatrix, pseudopar->associatedSurface());
        GXFTrackState *pseudostate = new GXFTrackState(newpseudo, pseudopar, true);
        pseudostate->setTrackParameters(pseudopar);
        pseudostate->setMeasurementType(TrackState::Pseudo);
        double errors[5];
        errors[0] = errors[2] = errors[3] = errors[4] = -1;
        errors[1] = 10;
        pseudostate->setMeasurementErrors(errors);
        trajectory.addMeasurementState(pseudostate);
        ispseudo = true;
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "Adding pseudomeasurement" << endmsg;
        }
      }
      if (std::abs(trajectory.trackStates().back()->position().z()) > 20000 && std::abs(previousz) < 12000) {
        largegap = true;
      }
      if (trajectory.trackStates().back()->trackStateType() == TrackState::Fittable) {
        previousz = trajectory.trackStates().back()->position().z();
      }
      /*if (ispseudo){
         if (!firstpseudostate) firstpseudostate=trajectory.trackStates().back();
         else secondpseudostate=trajectory.trackStates().back();
         }*/
    }
    Track *track = myfit(trajectory, *startPar, false,
                         (m_fieldService->toroidOn() || m_fieldService->solenoidOn()) ? muon : nonInteracting);
    if (startPar != lastidpar && startPar != indettrack->perigeeParameters()) {
      delete startPar;
    }
    return track;
  }

  Track *
  GlobalChi2Fitter::backupCombinationStrategy(const Track &intrk1,
                                              const Track &intrk2,
                                              GXFTrajectory &trajectory,
                                              std::vector<MaterialEffectsOnTrack> &calomeots) const {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "--> entering GlobalChi2Fitter::backupCombinationStrategy" << endmsg;
    }
    // std::cout << "intrk1: " << intrk1 << " intrk2: " << intrk2 << std::endl;
    bool firstismuon = false;
    const Track *indettrack = &intrk1;
    // const Track *muontrack=&intrk2;
    int nmeas1 = (int) intrk1.measurementsOnTrack()->size();
    const RIO_OnTrack *testrot = dynamic_cast<const RIO_OnTrack *>((*intrk1.measurementsOnTrack())[nmeas1 - 1]);
    const CompetingRIOsOnTrack *testcrot = 0;
    if (!testrot) {
      testcrot = dynamic_cast<const CompetingRIOsOnTrack *>((*intrk1.measurementsOnTrack())[nmeas1 - 1]);
      if (testcrot) {
        testrot = &testcrot->rioOnTrack(0);
      }
    }
    if (!testrot) {
      testrot = dynamic_cast<const RIO_OnTrack *>((*intrk1.measurementsOnTrack())[nmeas1 - 2]);
      if (!testrot) {
        testcrot = dynamic_cast<const CompetingRIOsOnTrack *>((*intrk1.measurementsOnTrack())[nmeas1 - 2]);
        if (testcrot) {
          testrot = &testcrot->rioOnTrack(0);
        }
      }
    }
    if (testrot && !m_DetID->is_indet(testrot->identify()) && m_DetID->is_muon(testrot->identify())) {
      firstismuon = true;
      // muontrack=&intrk1;
      indettrack = &intrk2;
    }


    DataVector<const TrackStateOnSurface>::const_iterator beginStates = intrk1.trackStateOnSurfaces()->begin();
    DataVector<const TrackStateOnSurface>::const_iterator itStates = beginStates;
    DataVector<const TrackStateOnSurface>::const_iterator endState = intrk1.trackStateOnSurfaces()->end();
    DataVector<const TrackStateOnSurface>::const_iterator beginStates2 = intrk2.trackStateOnSurfaces()->begin();
    DataVector<const TrackStateOnSurface>::const_iterator itStates2 = beginStates2;
    DataVector<const TrackStateOnSurface>::const_iterator endState2 = intrk2.trackStateOnSurfaces()->end();



    const TrackParameters *firstidpar = (*indettrack->trackParameters())[1];
    // const TrackParameters *lastidpar=indettrack->trackParameters()->back();
    const TrackParameters *lastidpar = m_extrapolator->extrapolateToVolume(*firstidpar, *m_caloEntrance, alongMomentum,
                                                                           Trk::muon);
    if (!lastidpar) {
      lastidpar = indettrack->trackParameters()->back()->clone();
    }
    std::unique_ptr<const TrackParameters> firstscatpar;
    std::unique_ptr<const TrackParameters> lastscatpar;
    std::unique_ptr<const TrackParameters> elosspar;

    double charge = (lastidpar->parameters()[Trk::qOverP] < 0) ? -1 : 1;

    Perigee startper(lastidpar->position(), lastidpar->momentum(), charge, lastidpar->position());

    if (!firstismuon) {
      firstscatpar.reset(m_propagator->propagateParameters(*lastidpar, calomeots[0].associatedSurface(),
                                                           Trk::alongMomentum, false, *m_fieldprop,
                                                           Trk::nonInteracting));
      delete lastidpar;
      if (!firstscatpar) {
        return 0;
      }
      std::unique_ptr<const TrackParameters> tmppar(m_propagator->propagateParameters(*firstscatpar,
                                                                                      calomeots[1].associatedSurface(), Trk::alongMomentum, false, *m_fieldprop,
                                                                                      Trk::nonInteracting));
      if (!tmppar) {
        return 0;
      }
      double sign = (tmppar->parameters()[Trk::qOverP] < 0) ? -1 : 1;
      double mass = m_particleMasses.mass[muon];

      double oldp = std::abs(1 / tmppar->parameters()[Trk::qOverP]);
      double de = std::abs(calomeots[1].energyLoss()->deltaE());

      double newp2 = oldp * oldp - 2 * de * sqrt(mass * mass + oldp * oldp) + de * de;
      if (newp2 < 4.e6) {
        newp2 = 4.e6;
      }
      double newqoverp = sign / sqrt(newp2);


      const AmgVector(5) &pars = tmppar->parameters();
      elosspar.reset(tmppar->associatedSurface().createTrackParameters(pars[0], pars[1], pars[2], pars[3], newqoverp,
                                                                       0));
      lastscatpar.reset(m_propagator->propagateParameters(*elosspar, calomeots[2].associatedSurface(),
                                                          Trk::alongMomentum, false, *m_fieldprop,
                                                          Trk::nonInteracting));
      if (!lastscatpar) {
        return 0;
      }
    }else {
      lastscatpar.reset(m_propagator->propagateParameters(*firstidpar, calomeots[2].associatedSurface(),
                                                          Trk::oppositeMomentum, false, *m_fieldprop,
                                                          Trk::nonInteracting));
      if (!lastscatpar) {
        return 0;
      }
      elosspar.reset(m_propagator->propagateParameters(*lastscatpar, calomeots[1].associatedSurface(),
                                                       Trk::oppositeMomentum, false, *m_fieldprop,
                                                       Trk::nonInteracting));
      if (!elosspar) {
        return 0;
      }
      double sign = (elosspar->parameters()[Trk::qOverP] < 0) ? -1 : 1;
      double newqoverp = sign /
                         (1. / std::abs(elosspar->parameters()[Trk::qOverP]) +
                          std::abs(calomeots[1].energyLoss()->deltaE()));


      const AmgVector(5) &pars = elosspar->parameters();

      std::unique_ptr<const TrackParameters> tmppar(elosspar->associatedSurface().createTrackParameters(pars[0],
                                                                                                        pars[1],
                                                                                                        pars[2],
                                                                                                        pars[3],
                                                                                                        newqoverp, 0));
      firstscatpar.reset(m_propagator->propagateParameters(*tmppar, calomeots[0].associatedSurface(),
                                                           Trk::oppositeMomentum, false, *m_fieldprop,
                                                           Trk::nonInteracting));
      if (!firstscatpar) {
        return 0;
      }
    }

    for (; itStates != endState; ++itStates) {
      if (firstismuon && dynamic_cast<const PseudoMeasurementOnTrack *>((*itStates)->measurementOnTrack())) {
        continue;
      }

      if ((*itStates)->materialEffectsOnTrack()) {
        if (!firstismuon) {
          m_idmat = false;
        } else {
          continue;
        }
      }
      if (firstismuon) {
        makeProtoState(trajectory, *itStates);
      }
    }

    std::unique_ptr<GXFMaterialEffects> firstscatmeff(new GXFMaterialEffects(&calomeots[0]));
    std::unique_ptr<GXFMaterialEffects> elossmeff(new GXFMaterialEffects(&calomeots[1]));
    std::unique_ptr<GXFMaterialEffects> secondscatmeff(new GXFMaterialEffects(&calomeots[2]));
    // double oldsigmaphi1=firstscatmeff->sigmaDeltaPhi();
    // double oldsigmaphi2=secondscatmeff->sigmaDeltaPhi();
    // firstscatmeff->setScatteringSigmas(0.001,firstscatmeff->sigmaDeltaTheta());
    // secondscatmeff->setScatteringSigmas(0.001,secondscatmeff->sigmaDeltaTheta());
    double dp = 0, sigmadp = 0;
//  if (m_fieldService->toroidOn()){
    sigmadp = calomeots[1].energyLoss()->sigmaDeltaE();
    elossmeff->setSigmaDeltaE(sigmadp);
    // elossmeff->setSigmaDeltaE(-1);

    // dp=0.001*(1/std::abs(lastscatpar->parameters()[Trk::qOverP])-1/std::abs(firstscatpar->parameters()[Trk::qOverP]));
    dp = 1000 * (lastscatpar->parameters()[Trk::qOverP] - firstscatpar->parameters()[Trk::qOverP]);
    elossmeff->setdelta_p(dp);
//  }
    trajectory.addMaterialState(new GXFTrackState(firstscatmeff.release(), firstscatpar.release()), -1, true);
    trajectory.addMaterialState(new GXFTrackState(elossmeff.release(), elosspar.release()), -1, true);
    trajectory.addMaterialState(new GXFTrackState(secondscatmeff.release(), lastscatpar.release()), -1, true);
    GXFTrackState *secondscatstate = trajectory.trackStates().back();

    // const TrackParameters
    // *pseudopar1=0,*pseudopar2=0,*firstmdtpar=0,*lastmdtpar=0,*firsttriggerpar=0,*lasttriggerpar=0;

    // bool hascsc=false,hastgc=false;
    // const CompetingRIOsOnTrack *crot=dynamic_cast<const
    // CompetingRIOsOnTrack*>(*muontrack->measurementsOnTrack()->begin());
    // const RIO_OnTrack *firstmuonrot=0,*firstmdtrot=0,*lastmdtrot=0;
    // if (crot) firstmuonrot=crot->rioOnTrack(0);
    // else firstmuonrot=dynamic_cast<const RIO_OnTrack*>(*muontrack->measurementsOnTrack()->begin());
    // if (firstmuonrot && m_DetID->is_csc(firstmuonrot->identify())) hascsc=true;
    // if (firstmuonrot && m_DetID->is_tgc(firstmuonrot->identify())) hastgc=true;
    const Surface *triggersurf1 = 0, *triggersurf2 = 0;

    Amg::Vector3D triggerpos1(0, 0, 0), triggerpos2(0, 0, 0);

    bool seenmdt = false, mdtbetweenphihits = false;
    int nphi = 0;
    for (itStates2 = (!firstismuon ? beginStates2 : endState - 1);
         itStates2 != (!firstismuon ? endState2 : beginStates - 1); (!firstismuon ? ++itStates2 : --itStates2)) {
      if (!(*itStates2)->measurementOnTrack() || (*itStates2)->type(TrackStateOnSurface::Outlier)) {
        continue;
      }
      const Surface *surf = &(*itStates2)->measurementOnTrack()->associatedSurface();
      const CompetingRIOsOnTrack *crot = dynamic_cast<const CompetingRIOsOnTrack *>((*itStates2)->measurementOnTrack());
      const RIO_OnTrack *rot = 0;
      if (crot) {
        rot = &crot->rioOnTrack(0);
      } else {
        rot = dynamic_cast<const RIO_OnTrack *>((*itStates2)->measurementOnTrack());
      }
      if (rot && m_DetID->is_mdt(rot->identify()) && triggersurf1) {
        seenmdt = true;
      }
      if (rot && (m_DetID->is_tgc(rot->identify()) || m_DetID->is_rpc(rot->identify()))) {
        bool measphi = true;
        Amg::Vector3D measdir = surf->transform().rotation().col(0);
        double dotprod1 = measdir.dot(Amg::Vector3D(0, 0, 1));
        double dotprod2 = measdir.dot(Amg::Vector3D(surf->center().x(), surf->center().y(), 0) / surf->center().perp());
        if (std::abs(dotprod1) > .5 || std::abs(dotprod2) > .5) {
          measphi = false;
        }
        if (measphi) {
          nphi++;
          Amg::Vector3D thispos =
            (*itStates2)->trackParameters() ? (*itStates2)->trackParameters()->position() : rot->globalPosition();
          if (triggersurf1) {
            triggerpos2 = thispos;
            triggersurf2 = surf;
            if (seenmdt) {
              mdtbetweenphihits = true;
            }
          }else {
            triggerpos1 = thispos;
            triggersurf1 = surf;
          }
        }
      }
    }
    // std::cout << "trigsurf1: " << triggersurf1 << " trigpos1: " << triggerpos1 << " trigsurf2: " << triggersurf2 << "
    // trigpos2: " << triggerpos2 << std::endl;
    double mdttrig1 = 999999, mdttrig2 = 999999;
    const Surface *mdtsurf1 = 0, *mdtsurf2 = 0;

    for (itStates2 = (!firstismuon ? beginStates2 : endState - 1);
         itStates2 != (!firstismuon ? endState2 : beginStates - 1); (!firstismuon ? ++itStates2 : --itStates2)) {
      const Surface *surf = 0;
      if ((*itStates2)->measurementOnTrack() && !(*itStates2)->type(TrackStateOnSurface::Outlier)) {
        surf = &(*itStates2)->measurementOnTrack()->associatedSurface();
      }
      if (!surf) {
        continue;
      }
      const CompetingRIOsOnTrack *crot = dynamic_cast<const CompetingRIOsOnTrack *>((*itStates2)->measurementOnTrack());
      const RIO_OnTrack *rot = 0;
      if (crot) {
        rot = &crot->rioOnTrack(0);
      } else {
        rot = dynamic_cast<const RIO_OnTrack *>((*itStates2)->measurementOnTrack());
      }
      bool thisismdt = false;
      if (rot && m_DetID->is_mdt(rot->identify())) {
        thisismdt = true;
      }
      if (thisismdt) {
        // if ((*itStates2)->trackParameters()) {
        Amg::Vector3D globpos =
          (*itStates2)->trackParameters() ? (*itStates2)->trackParameters()->position() : (*itStates2)->
           measurementOnTrack()
           ->globalPosition();
        if (triggerpos1.mag() > 1 && (globpos - triggerpos1).mag() < mdttrig1) {
          mdttrig1 = (globpos - triggerpos1).mag();
          mdtsurf1 = surf;
        }
        if (triggerpos2.mag() > 1 && (globpos - triggerpos2).mag() < mdttrig2) {
          mdttrig2 = (globpos - triggerpos2).mag();
          mdtsurf2 = surf;
        }
        // }
        /*if ( !firstmdtrot){
           firstmdtrot=rot;
           }
           lastmdtrot=rot; */
      }
    }


    GXFTrackState *pseudostate1 = 0, *pseudostate2 = 0, *firstpseudostate = 0;
    std::vector<GXFTrackState *> outlierstates, outlierstates2;
    outlierstates.reserve(10);
    outlierstates2.reserve(10);
    PseudoMeasurementOnTrack *newpseudo = 0;
    for (itStates2 = beginStates2; itStates2 != endState2; ++itStates2) {
      const MeasurementBase *pseudo =
        dynamic_cast<const PseudoMeasurementOnTrack *>((*itStates2)->measurementOnTrack());
      const StraightLineSurface *slsurf =
        (*itStates2)->measurementOnTrack() ? dynamic_cast<const StraightLineSurface *>(&(*itStates2)->measurementOnTrack()
                                                                                        ->associatedSurface()) : 0;
      if (slsurf && !firstismuon && !newpseudo &&
          (((itStates2 == beginStates2 || itStates2 == beginStates2 + 1) &&
            std::abs((*itStates2)->measurementOnTrack()->globalPosition().z()) < 10000) /* || !nphi*/)) {
        // pseudo=(*itStates2)->measurementOnTrack();
        const TrackParameters *par2 =
          ((*itStates2)->trackParameters() &&
           nphi > 99) ? (*itStates2)->trackParameters()->clone() : m_propagator->propagateParameters(
            *secondscatstate->trackParameters(), *slsurf, alongMomentum, false, *m_fieldprop, Trk::nonInteracting);
        if (!par2) {
          continue;
        }
        Amg::MatrixX covMatrix(1, 1);
        covMatrix(0, 0) = 100;
        newpseudo =
          new PseudoMeasurementOnTrack(LocalParameters(DefinedParameter(par2->parameters()[Trk::locY], Trk::locY)),
                                       covMatrix, par2->associatedSurface());
        firstpseudostate = new GXFTrackState(newpseudo, par2, true);
        firstpseudostate->setTrackParameters(par2);
        firstpseudostate->setMeasurementType(TrackState::Pseudo);
        double errors[5];
        errors[0] = errors[2] = errors[3] = errors[4] = -1;
        errors[1] = 10;
        firstpseudostate->setMeasurementErrors(errors);
        trajectory.addMeasurementState(firstpseudostate);
        msg(MSG::DEBUG) << "Adding PseudoMeasurement" << endmsg;
        continue;
      }
      if (pseudo && !firstismuon) {
        continue;
      }
      if ((**itStates2).materialEffectsOnTrack()) {
        if (firstismuon) {
          m_idmat = false;
        } else {
          continue;
        }
      }

      if (!firstismuon) {
        if ((**itStates2).measurementOnTrack() &&
            &(**itStates2).measurementOnTrack()->associatedSurface() == triggersurf1 && mdtsurf1) {
          Amg::Transform3D *transf = new Amg::Transform3D(mdtsurf1->transform());
          transf->translation() << triggerpos1;
          StraightLineSurface slsurf(transf);
          Amg::MatrixX covMatrix(1, 1);
          covMatrix(0, 0) = 100;

          PseudoMeasurementOnTrack *newpseudo =
            new PseudoMeasurementOnTrack(LocalParameters(DefinedParameter(0, Trk::locY)),
                                         covMatrix, slsurf);
          pseudostate1 = new GXFTrackState(newpseudo, 0, true);
          pseudostate1->setMeasurementType(TrackState::Pseudo);
          double errors[5];
          errors[0] = errors[2] = errors[3] = errors[4] = -1;
          errors[1] = 10;
          pseudostate1->setMeasurementErrors(errors);
          trajectory.addMeasurementState(pseudostate1);
          outlierstates2.push_back(pseudostate1);
        }
        if ((**itStates2).measurementOnTrack() &&
            &(**itStates2).measurementOnTrack()->associatedSurface() == triggersurf2 && mdtbetweenphihits && mdtsurf2) {
          Amg::Transform3D *transf = new Amg::Transform3D(mdtsurf2->transform());
          transf->translation() << triggerpos2;
          StraightLineSurface slsurf(transf);
          Amg::MatrixX covMatrix(1, 1);
          covMatrix(0, 0) = 100;

          PseudoMeasurementOnTrack *newpseudo =
            new PseudoMeasurementOnTrack(LocalParameters(DefinedParameter(0, Trk::locY)),
                                         covMatrix, slsurf);
          pseudostate2 = new GXFTrackState(newpseudo, 0, true);
          pseudostate2->setMeasurementType(TrackState::Pseudo);
          double errors[5];
          errors[0] = errors[2] = errors[3] = errors[4] = -1;
          errors[1] = 10;
          pseudostate2->setMeasurementErrors(errors);
          trajectory.addMeasurementState(pseudostate2);
          outlierstates2.push_back(pseudostate2);
        }
        makeProtoState(trajectory, *itStates2);

        if ((trajectory.trackStates().back()->measurementType() == TrackState::TGC ||
             (trajectory.trackStates().back()->measurementType() == TrackState::RPC &&
              trajectory.trackStates().back()->measuresPhi())) &&
            trajectory.trackStates().back()->trackStateType() == TrackState::Fittable) {
          outlierstates.push_back(trajectory.trackStates().back());
          trajectory.setOutlier((int) trajectory.trackStates().size() - 1, true);
        }
      }
    }

    trajectory.setNumberOfPerigeeParameters(0);

    Track *track = 0;

    trajectory.setPrefit(2);
    const TrackParameters *startpar2 = &startper;// trajectory.referenceParameters();
    m_matfilled = true;
    bool tmpacc = m_acceleration;
    m_acceleration = false;
    myfit(trajectory, *startpar2, false, muon);
    m_acceleration = tmpacc;

    m_matfilled = false;
    if (!firstismuon && /*m_fieldService->toroidOn() && */ trajectory.converged() &&
        std::abs(trajectory.residuals().back() / trajectory.errors().back()) > 10) {
      return 0;
    }


    if (trajectory.converged()) {
      if (firstpseudostate) {
        const TrackParameters *par2 = firstpseudostate->trackParameters();
        Amg::MatrixX covMatrix(1, 1);
        covMatrix(0, 0) = 100;

        PseudoMeasurementOnTrack *newpseudo =
          new PseudoMeasurementOnTrack(LocalParameters(DefinedParameter(par2->parameters()[Trk::locY], Trk::locY)),
                                       covMatrix, par2->associatedSurface());
        firstpseudostate->setMeasurement(newpseudo);
        firstpseudostate->setRecalibrated(false);
      }

      for (int j = 0; j < (int) trajectory.trackStates().size(); j++) {
        for (int i = 0; i < (int) outlierstates2.size(); i++) {
          if (trajectory.trackStates()[j] == outlierstates2[i]) {
            trajectory.setOutlier(j, true);
          }
        }
        for (int i = 0; i < (int) outlierstates.size(); i++) {
          if (trajectory.trackStates()[j] == outlierstates[i]) {
            trajectory.setOutlier(j, false);
          }
        }
      }
      for (itStates = (firstismuon ? beginStates2 : endState - 1);
           itStates != (firstismuon ? endState2 : beginStates - 1); (firstismuon ? ++itStates : --itStates)) {
        if (dynamic_cast<const PseudoMeasurementOnTrack *>((*itStates)->measurementOnTrack())) {
          continue;
        }
        makeProtoState(trajectory, *itStates, (firstismuon ? -1 : 0));
      }
      trajectory.reset();
      trajectory.setPrefit(0);
      trajectory.setNumberOfPerigeeParameters(5);
      track = myfit(trajectory, *firstidpar, false, muon);
      m_matfilled = false;
    }
    return track;
  }

  Track *
  GlobalChi2Fitter::fit(const Track &inputTrack,
                        const RunOutlierRemoval runOutlier,
                        const ParticleHypothesis matEffects) const {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "--> entering GlobalChi2Fitter::fit(Track,,)" << endmsg;
    }
#ifdef GXFDEBUGCODE
    if (m_truth) {
      retrieveTruth();
    }
#endif
    if (inputTrack.trackStateOnSurfaces()->empty()) {
      msg(MSG::WARNING) << "Track with zero track states, cannot perform fit" << endmsg;
      return 0;
    }
    if (inputTrack.trackParameters()->empty()) {
      msg(MSG::WARNING) << "Track without track parameters, cannot perform fit" << endmsg;
      return 0;
    }
    const TrackParameters *minpar = inputTrack.perigeeParameters(), *firstidpar = 0, *lastidpar = 0;
    if (!minpar) {
      minpar = *(inputTrack.trackParameters()->begin());
    }
    bool deleteminpar = false;
    bool tmpgetmat = m_getmaterialfromtrack;
    if (matEffects == Trk::nonInteracting || inputTrack.info().trackFitter() == TrackInfo::Unknown) {
      m_getmaterialfromtrack = false;
    }
    DataVector<const TrackStateOnSurface>::const_iterator itStates = inputTrack.trackStateOnSurfaces()->begin();
    DataVector<const TrackStateOnSurface>::const_iterator endState = inputTrack.trackStateOnSurfaces()->end();
    GXFTrajectory trajectory;
    trajectory.trackStates().reserve(inputTrack.trackStateOnSurfaces()->size());
    // const RIO_OnTrack *firstrot=0,*lastrot=0;
    const Surface *firsthitsurf = 0, *lasthitsurf = 0;
    bool hasid = false, hasmuon = false;
    bool iscombined = false, seenphimeas = false, phiem = false, phibo = false;
    for (; itStates != endState; ++itStates) {
      if (!(**itStates).measurementOnTrack() && (**itStates).materialEffectsOnTrack() &&
          matEffects != inputTrack.info().particleHypothesis()) {
        continue;
      }

      if ((**itStates).measurementOnTrack()) {
        const Surface *surf = &(**itStates).measurementOnTrack()->associatedSurface();
        /* const RIO_OnTrack *rot=dynamic_cast<const RIO_OnTrack*>((**itStates).measurementOnTrack());
           if (!rot){
           const CompetingRIOsOnTrack *crot=dynamic_cast<const
              CompetingRIOsOnTrack*>((**itStates).measurementOnTrack());
           if (crot) rot=crot->rioOnTrack(0);
           }*/
        Identifier hitid = surf->associatedDetectorElementIdentifier();
        if (!hitid.is_valid()) {
          const CompetingRIOsOnTrack *crot =
            dynamic_cast<const CompetingRIOsOnTrack *>((**itStates).measurementOnTrack());
          if (crot) {
            hitid = crot->rioOnTrack(0).identify();
          }
        }
        if (hitid.is_valid()) {
          // if (!firstrot) firstrot=rot;
          // lastrot=rot;
          if (!firsthitsurf) {
            firsthitsurf = surf;
          }
          lasthitsurf = surf;
          if (m_DetID->is_indet(hitid)) {
            hasid = true;
            if (hasmuon) {
              iscombined = true;
            }
            if ((**itStates).trackParameters()) {
              lastidpar = (**itStates).trackParameters();
              if (!firstidpar) {
                firstidpar = lastidpar;
              }
            }
          } else {
            if (/*!seenphimeas &&*/ !(**itStates).type(TrackStateOnSurface::Outlier)) {
              Amg::Vector3D measdir = surf->transform().rotation().col(0);
              double dotprod1 = measdir.dot(Amg::Vector3D(0, 0, 1));
              double dotprod2 = measdir.dot(Amg::Vector3D(surf->center().x(), surf->center().y(),
                                                          0) / surf->center().perp());
              if (std::abs(dotprod1) < .5 && std::abs(dotprod2) < .5) {
                seenphimeas = true;
                if (std::abs(surf->center().z()) > 13000) {
                  phiem = true;
                }
                if (surf->center().perp() > 9000 && std::abs(surf->center().z()) < 13000) {
                  phibo = true;
                }
              }
            }
            hasmuon = true;
            if (hasid) {
              iscombined = true;
            }
          }
        }
        if (iscombined && seenphimeas && (phiem || phibo)) {
          const PseudoMeasurementOnTrack *pseudo =
            dynamic_cast<const PseudoMeasurementOnTrack *>((**itStates).measurementOnTrack());
          if (pseudo) {
            continue;
          }
        }
      }
      makeProtoState(trajectory, *itStates);
    }

    if (m_getmaterialfromtrack && trajectory.numberOfScatterers() != 0 && (hasmuon || m_acceleration)) {
      m_matfilled = true;
    }

    if (firstidpar == lastidpar) {
      firstidpar = lastidpar = 0;
    }
    if (iscombined && !m_matfilled &&
        m_DetID->is_indet(firsthitsurf->associatedDetectorElementIdentifier()) !=
        m_DetID->is_indet(lasthitsurf->associatedDetectorElementIdentifier()) && firstidpar) {
      if (m_DetID->is_indet(firsthitsurf->associatedDetectorElementIdentifier())) {
        minpar = lastidpar;
      } else {
        minpar = firstidpar;
      }
    }
    bool tmpacc = m_acceleration;
    bool tmpfiteloss = m_fiteloss;
    bool tmpsirecal = m_sirecal;
    Track *tmptrack = 0;
    if (matEffects == Trk::proton || matEffects == Trk::kaon || matEffects == Trk::electron) {
      ATH_MSG_DEBUG("call myfit(GXFTrajectory,TP,,)");
      m_fiteloss = true;
      m_sirecal = false;
      if (matEffects == Trk::electron) {
        m_asymeloss = true;
      }
      tmptrack = myfit(trajectory, *minpar, false, matEffects);
      m_sirecal = tmpsirecal;
      if (!tmptrack) {
        m_matfilled = false;
        m_getmaterialfromtrack = tmpgetmat;
        m_acceleration = tmpacc;
        m_fiteloss = tmpfiteloss;
        cleanup();
        return 0;
      }
      int nscats = 0;
      bool isbrem = false;
      double bremdp = 0;
      for (int i = 0; i < (int) trajectory.trackStates().size(); i++) {
        GXFTrackState *state = trajectory.trackStates()[i];
        GXFMaterialEffects *meff = state->materialEffects();
        if (meff) {
          nscats++;
          const TrackParameters *layerpars = state->trackParameters();
          const MaterialProperties *matprop = meff->materialProperties();
          double p = 1. / std::abs(layerpars->parameters()[Trk::qOverP] - .0005 * meff->delta_p());
          const Amg::Vector2D *locpos = state->surface()->globalToLocal(layerpars->position());
          const Amg::Vector3D *layerNormal = state->surface()->normal(*locpos);
          double costracksurf = 1.;
          if (layerNormal) {
            costracksurf = fabs(layerNormal->dot(layerpars->momentum().unit()));
          }else {
            msg(MSG::WARNING) << "No normal on surface found!" << endmsg;
          }
          delete layerNormal;
          delete locpos;
          double oldde = meff->deltaE();
          EnergyLoss *eloss = 0;
          double sigmascat = 0;
          if (matprop) {
            eloss = m_elosstool->energyLoss(*matprop, p, 1. / costracksurf, Trk::alongMomentum, matEffects);
            sigmascat = sqrt(m_scattool->sigmaSquare(*matprop, p, 1. / costracksurf, matEffects));
            // meff->setEloss(eloss);
            if (eloss) {
              meff->setDeltaE(eloss->deltaE());
            }
            delete eloss;
          }else {
            MaterialProperties tmpprop(1., meff->x0(), 0., 0., 0., 0.);
            sigmascat = sqrt(m_scattool->sigmaSquare(tmpprop, p, 1. / costracksurf, matEffects));
          }
          // std::cout << "p: " << p << " delta p: " << .001*meff->delta_p() << " sigma phi: " <<
          // sigmascat/sin(layerpars->parameters()[Trk::theta]) << " theta: " << sigmascat << std::endl;
          meff->setScatteringSigmas(sigmascat / sin(layerpars->parameters()[Trk::theta]), sigmascat);

          if (matEffects == electron) {
            state->setTrackStateType(TrackState::Scatterer);
            meff->setDeltaE(oldde);
            if (!meff->isKink()) {
              meff->setSigmaDeltaE(0);
            } else {
              isbrem = true;
              bremdp = meff->delta_p();
            }
            /* if (meff->isKink()){
               double thisqoverp=state->trackParameters()->parameters()[Trk::qOverP];
               double deltaqoverp=meff->delta_p();
               meff->setDeltaE(std::abs(1/(thisqoverp-.001*deltaqoverp))-std::abs(1/thisqoverp));
               } */
          }else if (eloss) {
            meff->setSigmaDeltaE(eloss->sigmaDeltaE());
          }
        }
      }
      const AmgVector(5) &refpars = trajectory.referenceParameters()->parameters();
      minpar = trajectory.referenceParameters()->associatedSurface().createTrackParameters(refpars[0], refpars[1],
                                                                                           refpars[2], refpars[3],
                                                                                           refpars[4], 0);
      deleteminpar = true;
      trajectory.reset();
      m_matfilled = true;
      if (matEffects == Trk::electron) {
        if (!isbrem) {
          trajectory.brems().clear();
        } else {
          trajectory.brems().resize(1);
          trajectory.brems()[0] = bremdp;
        }
        m_asymeloss = false;
        trajectory.setNumberOfScatterers(nscats);
        trajectory.setNumberOfBrems((isbrem ? 1 : 0));
      }
    }

    ATH_MSG_DEBUG("call myfit(GXFTrajectory,TP,,)");
    Track *track = myfit(trajectory, *minpar, runOutlier, matEffects);
    if (deleteminpar) {
      delete minpar;
    }
    bool pseudoupdated = false;
    if (track && hasid && hasmuon) {
      for (int i = 0; i < (int) trajectory.trackStates().size(); i++) {
        GXFTrackState *pseudostate = trajectory.trackStates()[i];
        if (!pseudostate || pseudostate->measurementType() != TrackState::Pseudo ||
            pseudostate->fitQuality()->chiSquared() < 10) {
          continue;
        }
        const TrackParameters *pseudopar = pseudostate->trackParameters();
        const TrackParameters *updpar = m_updator->removeFromState(*pseudopar,
                                                                   pseudostate->measurement()->localParameters(),
                                                                   pseudostate->measurement()->localCovariance());
        if (!updpar) {
          continue;
        }
        Amg::MatrixX covMatrix(1, 1);
        covMatrix(0, 0) = 100;
        PseudoMeasurementOnTrack *newpseudo =
          new PseudoMeasurementOnTrack(LocalParameters(DefinedParameter(updpar->parameters()[Trk::locY], Trk::locY)),
                                       covMatrix, pseudopar->associatedSurface());
        delete updpar;
        pseudostate->setMeasurement(newpseudo);
        double errors[5];
        errors[0] = errors[2] = errors[3] = errors[4] = -1;
        errors[1] = 10;
        pseudostate->setMeasurementErrors(errors);
        pseudoupdated = true;
      }
      if (pseudoupdated) {
        Track *oldtrack = track;
        trajectory.setConverged(false);
        m_matfilled = true;
        track = myfit(trajectory, *oldtrack->perigeeParameters(), false, muon);
        m_matfilled = false;
        delete oldtrack;
      }
    }


    m_matfilled = false;
    m_getmaterialfromtrack = tmpgetmat;
    m_acceleration = tmpacc;
    m_fiteloss = tmpfiteloss;
    // if (track) std::cout << "track: " << *track << std::endl;

    if (track) {
      m_nsuccessfits++;
      const TrackInfo old_info = inputTrack.info();
      track->info().addPatternReco(old_info);
    }
    if (tmptrack) {
      delete tmptrack;
    }
    cleanup();
    return track;
  }

  Track *
  GlobalChi2Fitter::fit(const PrepRawDataSet &prds,
                        const TrackParameters &param,
                        const RunOutlierRemoval runOutlier,
                        const ParticleHypothesis matEffects) const {
    MeasurementSet rots;

    // const TrackParameters *prevparam=param.cloneWithoutError();
    // PlaneSurface planesurf(new Translate3D(param.position().x(),param.position().y(),param.position().z()));
    // AtaPlane
    // planepar(param.position(),param.parameters()[Trk::phi],param.parameters()[Trk::theta],param.parameters()[Trk::qOverP],planesurf);
    // const TrackParameters *hitparam=&planepar;
    // bool hastrt=false;
    for (PrepRawDataSet::const_iterator it = prds.begin(); it != prds.end(); it++) {
      const Surface &prdsurf = (**it).detectorElement()->surface((**it).identify());
      /* const TrackParameters
         *hitparam=m_extrapolator->extrapolateDirectly(*prevparam,prdsurf,Trk::anyDirection,false,Trk::nonInteracting);
         if (!hitparam){
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "fit prds: propagation failed, check seed parameters, param: " <<
            param << " surface: " << (**it).detectorElement()->surface((**it).identify()) << endmsg;
         while (!rots.empty()){
           delete *(rots.end()-1);
         rots.pop_back();
         }
         m_propfailed++;
         m_nfits++;
         delete prevparam;
         return 0;
         } */
      const RIO_OnTrack *rot = 0;
      const PlaneSurface *plsurf = dynamic_cast<const PlaneSurface *>(&prdsurf);
      const StraightLineSurface *slsurf = !plsurf ? dynamic_cast<const StraightLineSurface *>(&prdsurf) : 0;
      if (!slsurf && !plsurf) {
        ATH_MSG_ERROR("Surface is neither PlaneSurface nor StraightLineSurface!");
      }
      // if (slsurf) hastrt=true;
      if (!m_broadROTcreator.empty() && slsurf) {
        rot = m_broadROTcreator->correct(**it, param);
      } else if (slsurf) {
        AtaStraightLine atasl(slsurf->center(), param.parameters()[Trk::phi],
                              param.parameters()[Trk::theta], param.parameters()[Trk::qOverP], *slsurf);
        rot = m_ROTcreator->correct(**it, atasl);
      }else if (plsurf) {
        if (param.covariance()) {
          AtaPlane atapl(plsurf->center(), param.parameters()[Trk::phi],
                         param.parameters()[Trk::theta], param.parameters()[Trk::qOverP], *plsurf, new AmgSymMatrix(5)(
                           *param.covariance()));
          rot = m_ROTcreator->correct(**it, atapl);
        }else {
          AtaPlane atapl(plsurf->center(), param.parameters()[Trk::phi],
                         param.parameters()[Trk::theta], param.parameters()[Trk::qOverP], *plsurf);
          rot = m_ROTcreator->correct(**it, atapl);
        }
      }
      if (rot) {
        rots.push_back(rot);
      }
      // delete prevparam;
      // prevparam=hitparam;
    }
    // delete prevparam;
    Track *track = fit(rots, param, runOutlier, matEffects);
    for (MeasurementSet::const_iterator it = rots.begin(); it != rots.end(); it++) {
      delete *it;
    }
    return track;
  }

  Track *
  GlobalChi2Fitter::fit(const Track &inputTrack,
                        const MeasurementSet &addMeasColl,
                        const RunOutlierRemoval runOutlier,
                        const ParticleHypothesis matEffects) const {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "--> entering GlobalChi2Fitter::fit(Track,Meas'BaseSet,,)" << endmsg;
    }
#ifdef GXFDEBUGCODE
    if (m_truth) {
      retrieveTruth();
    }
#endif

    const TrackParameters *minpar = inputTrack.perigeeParameters();
    if (!minpar) {
      minpar = *(inputTrack.trackParameters()->begin());
    }

    MeasurementSet hitColl;

    // collect MBs from Track (speed: assume this method is used for extending track at end)
    ATH_MSG_VERBOSE("add MeasurementBase objects from Track to new set");
    DataVector<const TrackStateOnSurface>::const_iterator itStates = inputTrack.trackStateOnSurfaces()->begin();
    DataVector<const TrackStateOnSurface>::const_iterator endState = inputTrack.trackStateOnSurfaces()->end();
    GXFTrajectory trajectory;

    bool old_reintoutl = m_reintoutl;
    m_reintoutl = false;
    bool tmpasymeloss = m_asymeloss;
    if (matEffects == electron) {
      m_asymeloss = true;
    }
    // bool seenfirsthit=false;
    for (; itStates != endState; ++itStates) {
      // if ((**itStates).measurementOnTrack()) seenfirsthit=true;
      // if (!m_acceleration || seenfirsthit)
      makeProtoState(trajectory, *itStates);
      // else if ((**itStates).type(TrackStateOnSurface::Scatterer)) {
      //  std::cout << "lay: " << (**itStates).surface().associatedLayer() << std::endl;
      //  trajectory.upstreamMaterialLayers().push_back(std::make_pair((**itStates).surface().associatedLayer(),0));
      // }
      if (matEffects == electron && !trajectory.trackStates().empty() &&
          trajectory.trackStates().back()->materialEffects() &&
          trajectory.trackStates().back()->materialEffects()->sigmaDeltaE() > 50.001) {
        trajectory.trackStates().back()->materialEffects()->setKink(true);
      }
    }
    m_reintoutl = old_reintoutl;
    // add MBs from input list
    MeasurementSet::const_iterator itSet = addMeasColl.begin();
    MeasurementSet::const_iterator itSetEnd = addMeasColl.end();

    for (; itSet != itSetEnd; ++itSet) {
      if (!(*itSet)) {
        msg(MSG::WARNING) << "There is an empty MeasurementBase object in the track! Skip this object.." << endmsg;
      }else {
        makeProtoStateFromMeasurement(trajectory, *itSet);
      }
    }
    // fit set of MeasurementBase using main method, start with first TrkParameter in inputTrack

    ATH_MSG_VERBOSE("call myfit(GXFTrajectory,TP,,)");
    Track *track = myfit(trajectory, *minpar, runOutlier, matEffects);
    m_asymeloss = tmpasymeloss;
    if (track) {
      double oldqual =
        (inputTrack.fitQuality()->numberDoF() !=
         0) ? inputTrack.fitQuality()->chiSquared() / inputTrack.fitQuality()->numberDoF() : -999;
      double newqual =
        (track->fitQuality()->numberDoF() !=
         0) ? track->fitQuality()->chiSquared() / track->fitQuality()->numberDoF() : -999;

      if (m_extensioncuts && runOutlier && newqual > 2 && newqual > 2 * oldqual) {
        ATH_MSG_DEBUG("Extension rejected");

        delete track;
        m_highchi2++;
        track = 0;
      }
    }

    if (track) {
      m_nsuccessfits++;
    }
    cleanup();
    return track;
  }

// extend a track fit to include an additional set of PrepRawData objects
// --------------------------------
  Track *
  GlobalChi2Fitter::fit(const Track &intrk,
                        const PrepRawDataSet &prds,
                        const RunOutlierRemoval runOutlier,
                        const ParticleHypothesis matEffects) const {
    MeasurementSet rots;
    // const TrackParameters *prevparam=intrk.trackParameters()->back()->cloneWithoutError();
    const TrackParameters *hitparam = intrk.trackParameters()->back();

    for (PrepRawDataSet::const_iterator it = prds.begin(); it != prds.end(); it++) {
      const Surface &prdsurf = (**it).detectorElement()->surface((**it).identify());
      /* const TrackParameters
         *hitparam=m_extrapolator->extrapolateDirectly(*prevparam,prdsurf,Trk::anyDirection,false,Trk::nonInteracting);
         if (!hitparam){
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "fit track+prds: propagation failed, check seed parameters, param: "
            << *intrk.trackParameters()->back() << " surface: " << (**it).detectorElement()->surface((**it).identify())
            << endmsg;
         while (!rots.empty()){
           delete *(rots.end()-1);
         rots.pop_back();
         }
         m_propfailed++;
         m_nfits++;
         delete prevparam;
         return 0;
         } */
      Amg::VectorX parameterVector = hitparam->parameters();
      std::unique_ptr<const TrackParameters> trackparForCorrect(hitparam->associatedSurface().createTrackParameters(
                                                                  parameterVector[Trk::loc1],
                                                                  parameterVector[Trk::loc2], parameterVector[Trk::phi],
                                                                  parameterVector[Trk::theta],
                                                                  parameterVector[Trk::qOverP], new AmgSymMatrix(5)(
                                                                    *hitparam->covariance())));
      const RIO_OnTrack *rot = 0;
      if (!m_broadROTcreator.empty() && dynamic_cast<const StraightLineSurface *>(&prdsurf)) {
        rot = m_broadROTcreator->correct(**it, *hitparam);
      } else {
        rot = m_ROTcreator->correct(**it, *trackparForCorrect);
      }
      if (rot) {
        rots.push_back(rot);
      }
      // delete prevparam;
      // prevparam=hitparam;
    }
    // delete prevparam;
    Track *track = fit(intrk, rots, runOutlier, matEffects);
    for (MeasurementSet::const_iterator it = rots.begin(); it != rots.end(); it++) {
      delete *it;
    }
    return track;
  }

  Track *
  GlobalChi2Fitter::fit(const MeasurementSet &rots,
                        const TrackParameters &param,
                        const RunOutlierRemoval runOutlier,
                        const ParticleHypothesis matEffects) const {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "--> entering GlobalChi2Fitter::fit(Meas'BaseSet,,)" << endmsg;
    }

#ifdef GXFDEBUGCODE
    if (m_truth) {
      retrieveTruth();
    }
#endif

    MeasurementSet::const_iterator itSet = rots.begin();
    MeasurementSet::const_iterator itSetEnd = rots.end();
    GXFTrajectory trajectory;
    for (; itSet != itSetEnd; ++itSet) {
      if (!(*itSet)) {
        msg(MSG::WARNING) << "There is an empty MeasurementBase object in the track! Skip this object.." << endmsg;
      }else {
        makeProtoStateFromMeasurement(trajectory, *itSet);
      }
    }
    const TrackParameters *startpar = &param;
    bool deletestartpar = false;
    if (matEffects == muon && trajectory.numberOfSiliconHits() + trajectory.numberOfTRTHits() == 0) {
      m_matfilled = true; // don't collect material for prefit
      trajectory.setPrefit(2);
      myfit(trajectory, *startpar, runOutlier, matEffects);
      m_matfilled = false;
      if (!trajectory.converged()) {
        return 0;
      }

      trajectory.setConverged(false);
      const TrackParameters *firstpar = trajectory.trackStates()[0]->trackParameters();
      const TrackParameters *lastpar = trajectory.trackStates().back()->trackParameters();
      PerigeeSurface persurf(firstpar->position() - 10 * firstpar->momentum().unit());
      if (trajectory.trackStates().front()->measurementType() == TrackState::Pseudo) {
        Amg::MatrixX covMatrix(1, 1);
        covMatrix(0, 0) = 100;

        PseudoMeasurementOnTrack *newpseudo =
          new PseudoMeasurementOnTrack(LocalParameters(DefinedParameter(firstpar->parameters()[Trk::locY], Trk::locY)),
                                       covMatrix, firstpar->associatedSurface());
        trajectory.trackStates().front()->setMeasurement(newpseudo);
      }
      if (trajectory.trackStates().back()->measurementType() == TrackState::Pseudo) {
        Amg::MatrixX covMatrix(1, 1);
        covMatrix(0, 0) = 100;

        PseudoMeasurementOnTrack *newpseudo =
          new PseudoMeasurementOnTrack(LocalParameters(DefinedParameter(lastpar->parameters()[Trk::locY], Trk::locY)),
                                       covMatrix, lastpar->associatedSurface());
        trajectory.trackStates().back()->setMeasurement(newpseudo);
      }
      if (!m_straightline) {
        trajectory.setPrefit(3);
        const AmgVector(5) &refpars = trajectory.referenceParameters()->parameters();
        startpar = trajectory.referenceParameters()->associatedSurface().createTrackParameters(refpars[0], refpars[1],
                                                                                               refpars[2], refpars[3],
                                                                                               refpars[4], 0);

        trajectory.reset();
        myfit(trajectory, *startpar, runOutlier, matEffects);
        m_matfilled = true;
        delete startpar;
        if (!trajectory.converged()) {
          cleanup();
          return 0;
        }
      }
      const AmgVector(5) &refpars = trajectory.referenceParameters()->parameters();
      startpar = trajectory.referenceParameters()->associatedSurface().createTrackParameters(refpars[0], refpars[1],
                                                                                             refpars[2], refpars[3],
                                                                                             refpars[4], 0);


      trajectory.reset();
      trajectory.setPrefit(0);
      deletestartpar = true;
      if (trajectory.trackStates().front()->measurementType() == TrackState::Pseudo) {
        firstpar = trajectory.trackStates().front()->trackParameters();

        Amg::MatrixX covMatrix(1, 1);
        covMatrix(0, 0) = 100;

        PseudoMeasurementOnTrack *newpseudo =
          new PseudoMeasurementOnTrack(LocalParameters(DefinedParameter(firstpar->parameters()[Trk::locY], Trk::locY)),
                                       covMatrix, firstpar->associatedSurface());
        trajectory.trackStates().front()->setMeasurement(newpseudo);
        double errors[5];
        errors[0] = errors[2] = errors[3] = errors[4] = -1;
        errors[1] = 10;
        trajectory.trackStates().front()->setMeasurementErrors(errors);
      }
      if (trajectory.trackStates().back()->measurementType() == TrackState::Pseudo) {
        lastpar = trajectory.trackStates().back()->trackParameters();

        Amg::MatrixX covMatrix(1, 1);
        covMatrix(0, 0) = 100;

        PseudoMeasurementOnTrack *newpseudo =
          new PseudoMeasurementOnTrack(LocalParameters(DefinedParameter(lastpar->parameters()[Trk::locY], Trk::locY)),
                                       covMatrix, lastpar->associatedSurface());
        trajectory.trackStates().back()->setMeasurement(newpseudo);
        double errors[5];
        errors[0] = errors[2] = errors[3] = errors[4] = -1;
        errors[1] = 10;
        trajectory.trackStates().back()->setMeasurementErrors(errors);
      }
    }
    Track *track = 0;
    if (startpar) {
      track = myfit(trajectory, *startpar, runOutlier, matEffects);
    }
    if (deletestartpar) {
      delete startpar;
    }
    if (track) {
      m_nsuccessfits++;
    }
    m_matfilled = false;
    cleanup();
    return track;
  }

  void
  GlobalChi2Fitter::makeProtoState(GXFTrajectory &trajectory, const TrackStateOnSurface *tsos, int index,
                                   bool copytp) const {
    if ((tsos->type(TrackStateOnSurface::Scatterer) || tsos->type(TrackStateOnSurface::BremPoint) ||
         tsos->type(TrackStateOnSurface::CaloDeposit) ||
         tsos->type(TrackStateOnSurface::InertMaterial)) && m_getmaterialfromtrack) {
      if (m_acceleration && trajectory.numberOfHits() == 0) {
        return;
      }
      const MaterialEffectsOnTrack *meff = dynamic_cast<const MaterialEffectsOnTrack *>(tsos->materialEffectsOnTrack());
      if (!meff) {
        return; // This should never happen
      }
      GXFMaterialEffects *newmeff = 0;

      if (meff->scatteringAngles() || meff->energyLoss() || !tsos->type(TrackStateOnSurface::Scatterer) ||
          !tsos->trackParameters()) {
        newmeff = new GXFMaterialEffects(meff);
      } else {
        Trk::MaterialProperties matprop(meff->thicknessInX0(), 1., 0., 0., 0., 0.);
        double sigmascat =
          sqrt(m_scattool->sigmaSquare(matprop, std::abs(1. / tsos->trackParameters()->parameters()[Trk::qOverP]), 1.,
                                       Trk::muon));
        Trk::ScatteringAngles *newsa =
          new Trk::ScatteringAngles(0, 0, sigmascat / sin(tsos->trackParameters()->parameters()[Trk::theta]),
                                    sigmascat);
        Trk::MaterialEffectsOnTrack newmeot(meff->thicknessInX0(), newsa, 0, tsos->surface());
        newmeff = new GXFMaterialEffects(&newmeot);
      }
      if (meff->energyLoss() && meff->energyLoss()->sigmaDeltaE() > 0 &&
          ((tsos->type(TrackStateOnSurface::BremPoint) && meff->scatteringAngles()) ||
           (/* m_fieldService->toroidOn() && */ (!meff->scatteringAngles() ||
                                                 meff->thicknessInX0() ==
                                                 0 /* || std::abs(meff->energyLoss()->deltaE())>50 */)))) {
        newmeff->setSigmaDeltaE(meff->energyLoss()->sigmaDeltaE());
        if (tsos->trackParameters() && !trajectory.trackStates().empty() &&
            (**trajectory.trackStates().rbegin()).trackParameters()) {
          // double
          // delta_p=1/std::abs(tsos->trackParameters()->parameters()[Trk::qOverP])-1/std::abs((**trajectory.trackStates().rbegin()).trackParameters()->parameters()[Trk::qOverP]);
          double delta_p = 1000 *
                           (tsos->trackParameters()->parameters()[Trk::qOverP] -
                            (**trajectory.trackStates().rbegin()).trackParameters()->parameters()[Trk::qOverP]);
          newmeff->setdelta_p(delta_p);
        }
      }
      trajectory.addMaterialState(new GXFTrackState(newmeff,
                                                    (copytp ? tsos->trackParameters()->clone() : tsos->trackParameters())), index,
                                  copytp);
    }
    if (tsos->type(TrackStateOnSurface::Measurement) || tsos->type(TrackStateOnSurface::Outlier)) {
      bool isoutlier = false;
      if (tsos->type(TrackStateOnSurface::Outlier) && !m_reintoutl) {
        isoutlier = true;
      }
      makeProtoStateFromMeasurement(trajectory, tsos->measurementOnTrack(), tsos->trackParameters(), isoutlier, index);
      // if (trajectory.prefit() && (ptsos[0]->measurementType()==TrackState::RPC ||
      // ptsos[0]->measurementType()==TrackState::TGC)) isoutlier=true;
    }
  }

  void
  GlobalChi2Fitter::makeProtoStateFromMeasurement(GXFTrajectory &trajectory, const MeasurementBase *measbase,
                                                  const TrackParameters *trackpar, bool isoutlier, int index) const {
    const Segment *seg = 0;

    if (!measbase->associatedSurface().associatedDetectorElementIdentifier().is_valid()) {
      seg = dynamic_cast<const Segment *>(measbase);
    }
    int imax = 1;
    if (seg && m_decomposesegments) {
      imax = (int) seg->numberOfMeasurementBases();
    }
    std::string string1, string2;
    for (int i = 0; i < imax; i++) {
      const MeasurementBase *measbase2 = (seg && m_decomposesegments) ? seg->measurement(i) : measbase;
      const TrackParameters *newtrackpar = (seg && m_decomposesegments) ? 0 : trackpar;
      GXFTrackState *ptsos = new GXFTrackState(measbase2, newtrackpar);
      const Amg::MatrixX &covmat = measbase2->localCovariance();
      double sinstereo = 0;
      double errors[5];
      errors[0] = errors[1] = errors[2] = errors[3] = errors[4] = -1;
      TrackState::MeasurementType hittype = TrackState::unidentified;
      // if (measbase2!=measbase) rot=dynamic_cast<const RIO_OnTrack *>(measbase2);
      Identifier hitid = measbase2->associatedSurface().associatedDetectorElementIdentifier();
      const CompetingRIOsOnTrack *crot = 0;
      if (!hitid.is_valid()) {
        crot = dynamic_cast<const CompetingRIOsOnTrack *>(measbase2);
        if (crot) {
          hitid = crot->rioOnTrack(0).identify();
        }
      }
      bool measphi = false;
      if (hitid.is_valid() && measbase2->localParameters().contains(Trk::locX)) {
        if (msgLvl(MSG::DEBUG)) {
          if (!crot) {
            crot = dynamic_cast<const CompetingRIOsOnTrack *>(measbase2);
          }
          string1 = crot ? "CompetingRIOsOnTrack " : "";
        }
        bool rotated = false;
        if (m_DetID->is_indet(hitid) && !m_DetID->is_muon(hitid)) {
          if (m_DetID->is_pixel(hitid)) {
            if (msgLvl(MSG::DEBUG)) {
              string2 = "Pixel hit";
            }
            hittype = TrackState::Pixel;
          }else if (m_DetID->is_sct(hitid)) {
            // if (!dynamic_cast<const TrapezoidBounds*>(&measbase2->associatedSurface().bounds())) {
            if (covmat.cols() == 1 || covmat(1, 0) == 0) {
              if (msgLvl(MSG::DEBUG)) {
                string2 = "SCT barrel hit";
              }
            }else {
              if (msgLvl(MSG::DEBUG)) {
                string2 = "SCT endcap hit";
              }
              rotated = true;
            }
            hittype = TrackState::SCT;
          }else if (m_DetID->is_trt(hitid)) {
            if (msgLvl(MSG::DEBUG)) {
              string2 = "TRT hit";
            }
            hittype = TrackState::TRT;
          }
        }else { // Muon hit
          if (m_DetID->is_rpc(hitid)) {
            if (msgLvl(MSG::DEBUG)) {
              string2 = "RPC hit";
            }
            hittype = TrackState::RPC;
            if (measbase->localParameters().parameterKey() != 1) {
              msg(MSG::WARNING) << "Corrupt RPC hit, skipping it" << endmsg;
              delete ptsos;
              continue;
            }
          }else if (m_DetID->is_mdt(hitid)) {
            if (msgLvl(MSG::DEBUG)) {
              string2 = "MDT hit";
            }
            hittype = TrackState::MDT;
          }else if (m_DetID->is_tgc(hitid)) {
            if (!dynamic_cast<const TrapezoidBounds *>(&measbase2->associatedSurface().bounds())) {
              string2 = "TGC wire hit";
            } else {
              if (msgLvl(MSG::DEBUG)) {
                string2 = "TGC strip hit";
              }
              rotated = true;
            }
            hittype = TrackState::TGC;
          }else if (m_DetID->is_csc(hitid)) {
            if (msgLvl(MSG::DEBUG)) {
              string2 = "CSC hit";
            }
            hittype = TrackState::CSC;
          }
        }
        if (rotated) {
          double v0 = 0.5 *
                      (covmat(0,
                              0) +
                       covmat(1,
                              1) -
                       sqrt((covmat(0,
                                    0) +
                             covmat(1,
                                    1)) *
                            (covmat(0,
                                    0) +
          covmat(1, 1)) - 4 * (covmat(0, 0) * covmat(1, 1) - covmat(0, 1) * covmat(0, 1))));
          double v1 = 0.5 *
                      (covmat(0,
                              0) +
                       covmat(1,
                              1) +
                       sqrt((covmat(0,
                                    0) +
                             covmat(1,
                                    1)) *
                            (covmat(0,
                                    0) +
          covmat(1, 1)) - 4 * (covmat(0, 0) * covmat(1, 1) - covmat(0, 1) * covmat(0, 1))));
          sinstereo = sin(0.5 * asin(2 * covmat(0, 1) / (v0 - v1)));
          errors[0] = sqrt(v0);
        }else {
          errors[0] = sqrt(covmat(0, 0));
          if (hittype == TrackState::Pixel) {
            errors[1] = sqrt(covmat(1, 1));
          }
        }
        if (hittype == TrackState::RPC || hittype == TrackState::TGC || hittype == TrackState::CSC) {
          const Surface *surf = &measbase2->associatedSurface();
          Amg::Vector3D measdir = surf->transform().rotation().col(0);
          double dotprod1 = measdir.dot(Amg::Vector3D(0, 0, 1));
          double dotprod2 =
            measdir.dot(Amg::Vector3D(surf->center().x(), surf->center().y(), 0) / surf->center().perp());
          if (std::abs(dotprod1) < .5 && std::abs(dotprod2) < .5) {
            measphi = true;
          }
        }
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "#" << m_hitcount << " " << string1 << string2 << " pos=(" <<
            measbase2->globalPosition().x() << "," << measbase2->globalPosition().y() << "," <<
            measbase2->globalPosition().z() << ") "; // print out the hit type
          if (measphi) {
            msg(MSG::DEBUG) << " measures phi";
          }
          if (hittype == TrackState::MDT) {
            msg(MSG::DEBUG) << " radius=" << measbase2->localParameters()[Trk::locX];
          }
          if (isoutlier) {
            msg(MSG::DEBUG) << " outlier";
          }
#ifdef GXFDEBUGCODE
          const RIO_OnTrack *rot = 0;
          if (crot) {
            rot = &crot->rioOnTrack(0);
          } else {
            rot = dynamic_cast<const RIO_OnTrack *>(measbase2);
          }
          // std::cout << "rot: " << rot << std::endl;
          // if (!rot) std::cout << "measbase: " << *measbase2 << std::endl;
          if (m_truth && rot) {
            printTruth(rot->identify());
          }
#endif
          msg(MSG::DEBUG) << endmsg;
        }
      }else { // PseudoMeasurement, VertexOnTrack or Segment
        const Trk::LocalParameters &psmpar = measbase2->localParameters();
        // @TODO coverity complains about index shadowing the method argument index
        // this is solved by renaming index in this block by param_index
        int param_index = 0;
        if (psmpar.contains(Trk::locRPhi)) {
          errors[0] = sqrt(covmat(0, 0));
          param_index++;
        }
        if (psmpar.contains(Trk::locZ)) {
          errors[1] = sqrt(covmat(param_index, param_index));
          param_index++;
        }
        if (psmpar.contains(Trk::phi)) {
          errors[2] = sqrt(covmat(param_index, param_index));
          param_index++;
        }
        if (psmpar.contains(Trk::theta)) {
          errors[3] = sqrt(covmat(param_index, param_index));
          param_index++;
        }
        if (psmpar.contains(Trk::qOverP)) {
          errors[4] = sqrt(covmat(param_index, param_index));
          param_index++;
        }
        if (dynamic_cast<const PseudoMeasurementOnTrack *>(measbase2)) {
          hittype = TrackState::Pseudo;
          if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << "PseudoMeasurement, pos=" << measbase2->globalPosition() << endmsg; // print out the hit
                                                                                                   // type
          }
        }else if (dynamic_cast<const VertexOnTrack *>(measbase2)) {
          hittype = TrackState::Vertex;
          if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << "VertexOnTrack, pos=" << measbase2->globalPosition() << endmsg; // print out the hit type
          }
        }else if (dynamic_cast<const Segment *>(measbase2)) {
          hittype = TrackState::Segment;
          if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << "Segment, pos=" << measbase2->globalPosition() << endmsg; // print out the hit type
          }
        }
      }
      if (errors[0] > 0 || errors[1] > 0 || errors[2] > 0 || errors[3] > 0 || errors[4] > 0) {
        ptsos->setMeasurementErrors(errors);
        ptsos->setSinStereo(sinstereo);
        ptsos->setMeasurementType(hittype);
        ptsos->setMeasuresPhi(measphi);
        if (isoutlier && !m_reintoutl) {
          ptsos->setTrackStateType(TrackState::GeneralOutlier);
        }
        // @TODO here index really is supposed to refer to the method argument index ?
        bool ok = trajectory.addMeasurementState(ptsos, index);
        if (!ok) {
          msg(MSG::WARNING) << "Duplicate hit on track" << endmsg;
        }
      }else {
        delete ptsos;
        msg(MSG::WARNING) << "Measurement error is zero or negative, drop hit" << endmsg;
      }
      m_hitcount++;
    }
  }

  bool
  GlobalChi2Fitter::processTrkVolume(const Trk::TrackingVolume *tvol) const {
    if (!tvol) {
      return false;
    }
    const Trk::BinnedArray< Trk::Layer > *confinedLayers = tvol->confinedLayers();
    // loop over confined layers
    // std::cout << "confined layers: " << confinedLayers << " volumes: " << tvol->confinedVolumes() << std::endl;
    if (confinedLayers) {
      const std::vector<const Trk::Layer *> &layerVector = confinedLayers->arrayObjects();
      std::vector<const Trk::Layer *>::const_iterator layerIter = layerVector.begin();
      // loop over layers
      for (; layerIter != layerVector.end(); ++layerIter) {
        // push_back the layer
        if (*layerIter) {
          // get the layerIndex
          const Trk::LayerIndex &layIndex = (*layerIter)->layerIndex();
          // skip navigaion layers for the moment
          // std::cout << "layindex: " << layIndex.value() << " matprop: " << (*layerIter)->layerMaterialProperties() <<
          // std::endl;
          if (!layIndex.value() || !(*layerIter)->layerMaterialProperties()) {
            continue;
          }
          const CylinderLayer *cyllay = dynamic_cast<const CylinderLayer *>((*layerIter));
          const DiscLayer *disclay = dynamic_cast<const DiscLayer *>((*layerIter));
          if (disclay) {
            if (disclay->center().z() < 0) {
              m_negdiscs.push_back(disclay);
            } else {
              m_posdiscs.push_back(disclay);
            }
          }else if (cyllay) {
            m_barrelcylinders.push_back(cyllay);
          }else {
            return false;
          }
        }
      }
    }
    // const EntryLayerProvider *elp=tvol->entryLayerProvider();
    const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > &bsurf = tvol->boundarySurfaces();
    // if (elp) {
    for (std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >::const_iterator it = bsurf.begin();
         it != bsurf.end(); it++) {
      // const std::vector<const Trk::Layer*>& layerVector = elp->layers();
      // std::vector<const Trk::Layer*>::const_iterator layerIter = layerVector.begin();
      const Layer *layer = (*it)->surfaceRepresentation().materialLayer();
      // loop over layers
      // for ( ; layerIter != layerVector.end(); ++layerIter) {
      // push_back the layer
      if (layer) {
        // get the layerIndex
        const Trk::LayerIndex &layIndex = layer->layerIndex();
        // skip navigaion layers for the moment
        // std::cout << "layindex: " << layIndex.value() << " matprop: " << (*layerIter)->layerMaterialProperties() <<
        // std::endl;
        if (!layIndex.value() || !layer->layerMaterialProperties()) {
          continue;
        }
        const CylinderSurface *cylsurf = dynamic_cast<const CylinderSurface *>(&layer->surfaceRepresentation());
        const DiscSurface *discsurf = dynamic_cast<const DiscSurface *>(&layer->surfaceRepresentation());

        if (discsurf) {
          if (discsurf->center().z() < 0 &&
              std::find(m_negdiscs.begin(), m_negdiscs.end(), layer) == m_negdiscs.end()) {
            m_negdiscs.push_back(layer);
          } else if (discsurf->center().z() > 0 &&
                     std::find(m_posdiscs.begin(), m_posdiscs.end(), layer) == m_posdiscs.end()) {
            m_posdiscs.push_back(layer);
          }
        }else if (cylsurf &&
                  std::find(m_barrelcylinders.begin(), m_barrelcylinders.end(), layer) == m_barrelcylinders.end()) {
          m_barrelcylinders.push_back(layer);
        }
        if (!cylsurf && !discsurf) {
          return false;
        }
      }
      // }
    }
    const Trk::BinnedArray< Trk::TrackingVolume > *confinedVolumes = tvol->confinedVolumes();
    // get the confined volumes and loop over it -> call recursively
    if (confinedVolumes) {
      const std::vector<const Trk::TrackingVolume *> &volumes = confinedVolumes->arrayObjects();
      std::vector<const Trk::TrackingVolume *>::const_iterator volIter = volumes.begin();
      std::vector<const Trk::TrackingVolume *>::const_iterator volIterEnd = volumes.end();
      for (; volIter != volIterEnd; ++volIter) {
        if (*volIter) {
          bool ok = processTrkVolume(*volIter);
          if (!ok) {
            return false;
          }
        }
      }
    }
    return true;
  }

  class GXFlayersort: public std::binary_function<const std::pair<const Layer *, const Layer *> &,
                                                  const std::pair<const Layer *, const Layer *> &, bool > {
public:
    /** Default Constructor */
    GXFlayersort() {
    }

    bool
    operator () (const std::pair<const Layer *, const Layer *> &one,
                 const std::pair<const Layer *, const Layer *> &two) const {
      const CylinderSurface *cyl1 = 0;

      if (one.first) {
        cyl1 = (const CylinderSurface *) &one.first->surfaceRepresentation();
      }
      const DiscSurface *disc1 = 0;
      if (one.second) {
        disc1 = (const DiscSurface *) &one.second->surfaceRepresentation();
      }
      const CylinderSurface *cyl2 = 0;
      if (two.first) {
        cyl2 = (const CylinderSurface *) &two.first->surfaceRepresentation();
      }
      const DiscSurface *disc2 = 0;
      if (two.second) {
        disc2 = (const DiscSurface *) &two.second->surfaceRepresentation();
      }

      if (cyl1 && cyl2) {
        if (std::abs(cyl1->center().z() - cyl2->center().z()) > 1.) {
          return(std::abs(cyl1->center().z()) < std::abs(cyl2->center().z()));
        }
        return(cyl1->bounds().r() < cyl2->bounds().r());
      }
      if (disc1 && disc2) {
        const DiscBounds *discbounds1 = (const DiscBounds *) &disc1->bounds();
        const DiscBounds *discbounds2 = (const DiscBounds *) &disc2->bounds();
        if (discbounds1->rMax() < discbounds2->rMin() + 1) {
          return true;
        }
        if (discbounds1->rMin() > discbounds2->rMax() - 1) {
          return false;
        }
        return(std::abs(disc1->center().z()) < std::abs(disc2->center().z()));
      }
      if (cyl1 && disc2) {
        const DiscBounds *discbounds = (const DiscBounds *) &disc2->bounds();
        if (cyl1->bounds().r() > discbounds->rMax() - 1) {
          return false;
        }
        if (cyl1->bounds().r() < discbounds->rMin() + 1) {
          return true;
        }

        return(std::abs(cyl1->center().z()) < std::abs(disc2->center().z()));
      }
      // if (disc1 && cyl2) {
      if (!disc1 || !cyl2) {
        throw std::logic_error("Unhandled surface combination.");
      }
      const DiscBounds *discbounds = (const DiscBounds *) &disc1->bounds();
      if (cyl2->bounds().r() > discbounds->rMax() - 1) {
        return true;
      }
      if (cyl2->bounds().r() < discbounds->rMin() + 1) {
        return false;
      }

      return(std::abs(cyl2->center().z()) > std::abs(disc1->center().z()));
      // }
    }
  };

  class GXFlayersort2: public std::binary_function<const Layer *, const Layer *, bool > {
public:
    /** Default Constructor */
    GXFlayersort2() {
    }

    bool
    operator () (const Layer *one, const Layer *two) const {
      const CylinderSurface *cyl1 = dynamic_cast<const CylinderSurface *>(&one->surfaceRepresentation());
      const DiscSurface *disc1 = dynamic_cast<const DiscSurface *>(&one->surfaceRepresentation());
      const CylinderSurface *cyl2 = dynamic_cast<const CylinderSurface *>(&two->surfaceRepresentation());
      const DiscSurface *disc2 = dynamic_cast<const DiscSurface *>(&two->surfaceRepresentation());

      if (cyl1 && cyl2) {
        return(cyl1->bounds().r() < cyl2->bounds().r());
      }
      if (disc1 && disc2) {
        return(std::abs(disc1->center().z()) < std::abs(disc2->center().z()));
      }
      return false;
    }
  };

  void
  GlobalChi2Fitter::addIDMaterialFast(GXFTrajectory &trajectory, const TrackParameters *refpar2,
                                      ParticleHypothesis matEffects) const {
    if (!m_caloEntrance) {
      m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();
      // if (m_trackingGeometry) m_caloEntrance = m_trackingGeometry->trackingVolume("InDet::Containers::EntryVolume");
      if (m_trackingGeometry) {
        m_caloEntrance = m_trackingGeometry->trackingVolume("InDet::Containers::InnerDetector");
      }
      if (!m_caloEntrance) {
        msg(MSG::ERROR) << "calo entrance not available" << endmsg;
        return;
      }
    }
    if (m_negdiscs.empty() && m_posdiscs.empty() && m_barrelcylinders.empty()) {
      bool ok = processTrkVolume(m_caloEntrance);
      if (!ok) {
        ATH_MSG_DEBUG("Falling back to slow material collection");
        m_fastmat = false;
        addMaterial(trajectory, refpar2, matEffects);
        return;
      }
      std::stable_sort(m_negdiscs.begin(), m_negdiscs.end(), GXFlayersort2());
      std::stable_sort(m_posdiscs.begin(), m_posdiscs.end(), GXFlayersort2());
      // std::reverse(m_negdiscs.begin(),m_negdiscs.end());
      std::stable_sort(m_barrelcylinders.begin(), m_barrelcylinders.end(), GXFlayersort2());
      // std::cout << "nbarrelcyl: " << m_barrelcylinders.size() << " nposdisc: " << m_posdiscs.size() << " nnegdisc: "
      // << m_negdiscs.size() << std::endl;
    }
    const TrackParameters *refpar = refpar2, *firstsipar = 0, *lastsipar = 0;
    bool hasmat = false;
    int indexoffset = 0, lastmatindex = 0;
    // int lastsiindex=0;
    std::vector<GXFTrackState *> oldstates = trajectory.trackStates();
    std::vector<GXFTrackState *> &states = trajectory.trackStates();
    GXFTrackState *lastsistate = 0;
    for (int i = 0; i < (int) oldstates.size(); i++) {
      if (oldstates[i]->materialEffects()) {
        hasmat = true;
        lastmatindex = i;
      }
      if (oldstates[i]->measurementType() == TrackState::Pixel || oldstates[i]->measurementType() == TrackState::SCT) {
        if (!firstsipar) {
          if (!oldstates[i]->trackParameters()) {
            const TrackParameters *tmppar = m_propagator->propagateParameters(*refpar,
                                                                              *oldstates[i]->surface(), alongMomentum, false, *m_fieldprop,
                                                                              Trk::nonInteracting);
            if (!tmppar) {
              return;
            }
            oldstates[i]->setTrackParameters(tmppar);
          }
          firstsipar = oldstates[i]->trackParameters();
        }
        lastsistate = oldstates[i];
        // lastsiindex=i;
      }
    }
    if (!lastsistate) {
      throw std::logic_error("No track state");
    }
    if (!lastsistate->trackParameters()) {
      const TrackParameters *tmppar = m_propagator->propagateParameters(*refpar,
                                                                        *lastsistate->surface(), alongMomentum, false, *m_fieldprop,
                                                                        Trk::nonInteracting);
      if (!tmppar) {
        return;
      }
      lastsistate->setTrackParameters(tmppar);
    }
    lastsipar = lastsistate->trackParameters();


    if (hasmat) {
      refpar = lastsipar;
      indexoffset = lastmatindex;
    }else {
      refpar = firstsipar;
    }
    double firstz = firstsipar->position().z();
    double firstr = firstsipar->position().perp();
    double firstz2 = hasmat ? lastsipar->position().z() : firstsipar->position().z();
    double firstr2 = hasmat ? lastsipar->position().perp() : firstsipar->position().perp();

    double lastz, lastr;

    GXFTrackState *firststate = oldstates.front();
    GXFTrackState *laststate = oldstates.back();
    lastz = laststate->position().z();
    lastr = laststate->position().perp();
    std::vector<std::pair<const Layer *, const Layer *> > layers;
    const Layer *startlayer = firststate->surface()->associatedLayer();
    const Layer *startlayer2 = hasmat ? lastsistate->surface()->associatedLayer() : 0;
    const Layer *endlayer = laststate->surface()->associatedLayer();
    std::vector<std::pair<const Layer *, const Layer *> > &upstreamlayers = trajectory.upstreamMaterialLayers();
    layers.reserve(30);
    upstreamlayers.reserve(5);
    double tantheta = tan(refpar->parameters()[Trk::theta]);
    double slope = (tantheta != 0) ? 1 / tantheta : 0;// (lastz-firstz)/(lastr-firstr);
    if (slope != 0) {
      std::vector<const Layer *>::const_iterator it, itend;
      if (lastz > 0) {
        it = m_posdiscs.begin();
        itend = m_posdiscs.end();
      }else {
        it = m_negdiscs.begin();
        itend = m_negdiscs.end();
      }
      for (; it != itend; it++) {
        if (fabs((*it)->surfaceRepresentation().center().z()) > fabs(lastz)) {
          break;
        }
        // if (fabs((*it)->center().z())<fabs(firstz)) continue;
        const DiscBounds *discbounds = (const DiscBounds *) (&(*it)->surfaceRepresentation().bounds());
        if (discbounds->rMax() < firstr || discbounds->rMin() > lastr) {
          continue;
        }
        double rintersect = firstr + ((*it)->surfaceRepresentation().center().z() - firstz) / slope;
        if (rintersect < discbounds->rMin() - 50 || rintersect > discbounds->rMax() + 50) {
          continue;
        }
        if (/*(*it)==startlayer || (*it)==startlayer2 || */ (*it) == endlayer) {
          continue;
        }
        if (fabs((*it)->surfaceRepresentation().center().z()) < fabs(firstz) || (*it) == startlayer) {
          upstreamlayers.push_back(std::make_pair((Layer *) 0, (*it)));
        }
        if ((*it) != startlayer &&
            (fabs((*it)->surfaceRepresentation().center().z()) > fabs(firstz2) || (*it) == startlayer2)) {
          layers.push_back(std::make_pair((Layer *) 0, (*it)));
        }
        // std::cout << "r: " << rintersect << " z: " << (*it)->center().z() << " surf: " << (**it) << std::endl;
      }
    }
    for (std::vector<const Layer *>::const_iterator it = m_barrelcylinders.begin(); it != m_barrelcylinders.end();
         it++) {
      if ((*it)->surfaceRepresentation().bounds().r() > lastr) {
        break;
      }
      // if ((*it)->bounds().r()<firstr) continue;
      double zintersect = firstz + ((*it)->surfaceRepresentation().bounds().r() - firstr) * slope;
      if (fabs(zintersect - (*it)->surfaceRepresentation().center().z()) >
          ((const CylinderSurface *) (&(*it)->surfaceRepresentation()))->bounds().halflengthZ() + 50) {
        continue;
      }
      if (/*(*it)==startlayer || (*it)==startlayer2 ||*/ (*it) == endlayer) {
        continue;
      }
      // std::cout << "firstr: " << firstr << " r: " << (*it)->bounds().r() << std::endl;
      if ((*it)->surfaceRepresentation().bounds().r() < firstr || (*it) == startlayer) {
        upstreamlayers.push_back(std::make_pair((*it), (Layer *) 0));
      }
      if ((*it) != startlayer && ((*it)->surfaceRepresentation().bounds().r() > firstr2 || (*it) == startlayer2)) {
        layers.push_back(std::make_pair((*it), (Layer *) 0));
      }
      // std::cout << "r: " << (*it)->bounds().r() << " z: " << zintersect << " surf: " << (**it) << std::endl;
    }
    // if (startlayer && startlayer->layerMaterialProperties())
    // upstreamlayers.push_back(std::make_pair(startlayer,9999));

    // if (startlayer2 && startlayer2->layerMaterialProperties()) layers.push_back(std::make_pair(startlayer2,0));

    std::sort(layers.begin(), layers.end(), GXFlayersort());
    std::sort(upstreamlayers.begin(), upstreamlayers.end(), GXFlayersort());
    // std::cout << "number of layers: " << layers.size() << std::endl;
    std::vector<GXFTrackState *> newstates;

    trajectory.setTrackStates(newstates);
    states.reserve(oldstates.size() + layers.size());
    int layerindex = 0;
    for (int i = 0; i <= indexoffset; i++) {
      states.push_back(oldstates[i]);
    }
    const TrackParameters *parforextrap = refpar;
    double field[3];
    double pos[3];

    for (int i = indexoffset + 1; i < (int) oldstates.size(); i++) {
      double rmeas = oldstates[i]->position().perp();
      double zmeas = oldstates[i]->position().z();

      while (layerindex < (int) layers.size()) {
        // const Surface &surf=layers[layerindex].first->surfaceRepresentation();
        const CylinderSurface *cylsurf = 0;
        if (layers[layerindex].first) {
          cylsurf = (const CylinderSurface *) (&layers[layerindex].first->surfaceRepresentation());
        }
        const DiscSurface *discsurf = 0;
        if (layers[layerindex].second) {
          discsurf = (const DiscSurface *) (&layers[layerindex].second->surfaceRepresentation());
        }
        /* if (cylsurf) std::cout << "cyl rmeas: " << rmeas << " zmeas: " << zmeas << " bounds: " <<
           cylsurf->bounds().r() << " z: " <<
           cylsurf->center().z() << std::endl;
           if (discsurf) {
           const DiscBounds *discbounds=(const DiscBounds*)&discsurf->bounds();
           std::cout << "disc rmeas: " << rmeas << " zmeas: " << zmeas << " zdisc: " << discsurf->center().z() << "
              rmin: " <<
           discbounds->rMin() << " rmax: " << discbounds->rMax() << std::endl;
           } */
        // if (cylsurf && /* !measisendcap && */ cylsurf->bounds().r()>rmeas) break;
        // if (discsurf && /* measisendcap && */ fabs(discsurf->center().z())>fabs(zmeas)) break;
        // std::cout << "zmeas: " << zmeas << " layer index: " << layerindex << std::endl;


        // if (rlayer>rmeas) break;
        if (oldstates[i]->trackParameters()) {
          if (cylsurf) {
            double rlayer = cylsurf->bounds().r();
            if (fabs(rmeas - rlayer) < fabs(parforextrap->position().perp() - rlayer)) {
              parforextrap = oldstates[i]->trackParameters();
            }
          }else {
            if (!discsurf) {
              throw std::logic_error("Unhandled surface.");
            }
            double zlayer = discsurf->center().z();
            if (fabs(zmeas - zlayer) < fabs(parforextrap->position().z() - zlayer)) {
              parforextrap = oldstates[i]->trackParameters();
            }
          }
        }
        Amg::Vector3D intersect;
        double currentqoverp =
          (matEffects != Trk::electron) ? parforextrap->parameters()[Trk::qOverP] : refpar2->parameters()[Trk::qOverP];
        double costracksurf;
        pos[0] = parforextrap->position().x();
        pos[1] = parforextrap->position().y();
        pos[2] = parforextrap->position().z();
        m_fieldService->getFieldZR(pos, field);
        double sinphi = sin(parforextrap->parameters()[Trk::phi0]);
        double cosphi = cos(parforextrap->parameters()[Trk::phi0]);
        double sintheta = sin(parforextrap->parameters()[Trk::theta]);
        double costheta = cos(parforextrap->parameters()[Trk::theta]);
        double tantheta = tan(parforextrap->parameters()[Trk::theta]);
        double r = (std::abs(currentqoverp) > 1e-10) ? -sintheta / (currentqoverp * 300. * field[2]) : 1e6;
        double xc = parforextrap->position().x() - r * sinphi;
        double yc = parforextrap->position().y() + r * cosphi;
        // double signdeltaphi= (r<0) ? -1.: 1.;
        double phi0 = atan2(parforextrap->position().y() - yc, parforextrap->position().x() - xc);

        double z0 = parforextrap->position().z();
        if (discsurf) { // endcap layer
          double delta_s = (discsurf->center().z() - z0) / costheta;
          double delta_phi = delta_s * sintheta / r;
          double x = xc + fabs(r) * cos(phi0 + delta_phi);
          double y = yc + fabs(r) * sin(phi0 + delta_phi);
          // std::cout << "deltaphi: " << delta_phi << " delta_s: " << delta_s << " r: " << r << "intersect: " << x << "
          // " << y << " " << surf->center().z() << std::endl;
          intersect = Amg::Vector3D(x, y, discsurf->center().z());
          double perp = intersect.perp();
          // if (!measisendcap && perp>rmeas) break;
          const DiscBounds *discbounds = (const DiscBounds *) (&discsurf->bounds());
          if (perp > discbounds->rMax() || perp < discbounds->rMin()) {
            // std::cout << "out of bounds, perp: " << perp << " rmin: " << discbounds->rMin() << " rmax: " <<
            // discbounds->rMax() << std::endl;
            layerindex++;
            continue;
          }
          costracksurf = fabs(costheta);
        }else {
          if (!cylsurf) {
            throw std::logic_error("Unhandled surface.");
          }
          double d = xc * xc + yc * yc;
          double rcyl = cylsurf->bounds().r();
          double mysqrt = ((r + rcyl) * (r + rcyl) - d) * (d - (r - rcyl) * (r - rcyl));
          if (mysqrt < 0) {
            layerindex++;
            continue;
          }
          mysqrt = sqrt(mysqrt);
          double firstterm = xc / 2 + (xc * (rcyl * rcyl - r * r)) / (2 * d);
          double secondterm = (mysqrt * yc) / (2 * d);
          double x1 = firstterm + secondterm;
          double x2 = firstterm - secondterm;
          firstterm = yc / 2 + (yc * (rcyl * rcyl - r * r)) / (2 * d);
          secondterm = (mysqrt * xc) / (2 * d);
          double y1 = firstterm - secondterm;
          double y2 = firstterm + secondterm;
          double x = parforextrap->position().x();
          double y = parforextrap->position().y();
          double dist1 = (x - x1) * (x - x1) + (y - y1) * (y - y1);
          double dist2 = (x - x2) * (x - x2) + (y - y2) * (y - y2);
          if (dist1 < dist2) {
            x = x1;
            y = y1;
          }else {
            x = x2;
            y = y2;
          }
          double phi1 = atan2(y - yc, x - xc);
          double deltaphi = phi1 - phi0;
          if (std::abs(deltaphi - 2 * M_PI) < std::abs(deltaphi)) {
            deltaphi -= 2 * M_PI;
          }
          if (std::abs(deltaphi + 2 * M_PI) < std::abs(deltaphi)) {
            deltaphi += 2 * M_PI;
          }
          double delta_z = r * deltaphi / tantheta;
          double z = z0 + delta_z;
          // if (measisendcap && fabs(z)>fabs(zmeas)) break;

          intersect = Amg::Vector3D(x, y, z);

          if (fabs(z - cylsurf->center().z()) > cylsurf->bounds().halflengthZ()) {
            // std::cout << "out of bounds, z: " << z << " center z: " << cylsurf->center().z() << " halflength z: " <<
            // cylsurf->bounds().halflengthZ() << std::endl;
            layerindex++;
            continue;
          }
          Amg::Vector3D normal(x, y, 0);
          double phidir = parforextrap->parameters()[Trk::phi] + deltaphi;
          if (std::abs(phidir - 2 * M_PI) < std::abs(phidir)) {
            phidir -= 2 * M_PI;
          }
          if (std::abs(phidir + 2 * M_PI) < std::abs(phidir)) {
            phidir += 2 * M_PI;
          }
          Amg::Vector3D trackdir(cos(phidir) * sintheta, sin(phidir) * sintheta, costheta);
          costracksurf = fabs(normal.unit().dot(trackdir));
          // std::cout << "parforextrap: " << parforextrap->position() << " layer radius: " << rlayer << " intersect: "
          // << intersect << " costracksurf: " << costracksurf << std::endl;
        }
        if (cylsurf && /* !measisendcap && */ cylsurf->bounds().r() > rmeas) {
          break;
        }
        if (discsurf && /* measisendcap && */ fabs(discsurf->center().z()) > fabs(zmeas)) {
          break;
        }
        const Layer *layer = layers[layerindex].first;
        if (!layer) {
          layer = layers[layerindex].second;
        }
        const MaterialProperties *matprop = layer->layerMaterialProperties()->fullMaterial(intersect);
        // std::cout << "matprop: " << matprop << std::endl;
        if (!matprop) {
          layerindex++;
          continue;
        }
        double X0 = matprop->thicknessInX0();

        double actualx0 = X0 / costracksurf;
        double de =
          -fabs((matprop->thickness() / costracksurf) *
                m_elosstool->dEdX(*matprop, (m_p ? fabs(m_p) : fabs(1. / currentqoverp)), matEffects));
        double sigmascat =
          sqrt(m_scattool->sigmaSquare(*matprop, (m_p ? fabs(m_p) : fabs(1. / currentqoverp)), 1. / costracksurf,
                                       matEffects));
        GXFMaterialEffects *meff = new GXFMaterialEffects;
        meff->setDeltaE(de);
        meff->setScatteringSigmas(sigmascat / sintheta, sigmascat);
        meff->setX0(actualx0);
        meff->setSurface(&layer->surfaceRepresentation());
        meff->setMaterialProperties(matprop);
        EnergyLoss *eloss = 0;
        if (m_fiteloss || (matEffects == electron && m_asymeloss)) {
          eloss = m_elosstool->energyLoss(*matprop, (m_p ? fabs(m_p) : fabs(
                                                       1. / currentqoverp)), 1. / costracksurf, alongMomentum,
                                          matEffects);
          if (eloss) {
            meff->setSigmaDeltaE(eloss->sigmaDeltaE());
          }
        }
        if (matEffects == electron && m_asymeloss) {
          meff->setDeltaE(-5);
          if (!trajectory.numberOfTRTHits()) {
            meff->setScatteringSigmas(0, 0);
          }
          meff->setSigmaDeltaE(50);
          if (eloss) {
            // std::cout << "sigma pos: " << eloss->sigmaPlusDeltaE() << " sigma neg: " << eloss->sigmaMinusDeltaE() <<
            // std::endl;
            meff->setSigmaDeltaEPos(eloss->sigmaPlusDeltaE());
            meff->setSigmaDeltaENeg(eloss->sigmaMinusDeltaE());
          }
        }
        if (eloss) {
          delete eloss;
        }

        GXFTrackState *matstate = new GXFTrackState(meff);
        matstate->setPosition(intersect);
        trajectory.addMaterialState(matstate);
        // std::cout << "zmeas: " << zmeas << " rmeas: " << rmeas << " measisendcap: " << measisendcap << " matsurf: "
        // << layer->surfaceRepresentation() << std::endl;
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "X0: " << meff->x0();
          msg(MSG::DEBUG) << " qoverp: " << currentqoverp;
          msg(MSG::DEBUG) << " sigmascat " << meff->sigmaDeltaTheta();
          msg(MSG::DEBUG) << " eloss: " << meff->deltaE() << " sigma eloss: " << meff->sigmaDeltaE();
          msg(MSG::DEBUG) << endmsg;
        }
        layerindex++;
      }

      states.push_back(oldstates[i]);
    }
  }

  void
  GlobalChi2Fitter::addMaterial(GXFTrajectory &trajectory, const TrackParameters *refpar2,
                                ParticleHypothesis matEffects) const {
    if (!refpar2) {
      return;
    }
    ATH_MSG_VERBOSE("GlobalChi2Fitter::addMaterial, refpar=" << *refpar2);
    const MeasurementBase *firstmuonhit = 0, *lastmuonhit = 0, *firstidhit = 0, *lastidhit = 0, *firsthit = 0,
                          *lasthit = 0;
    std::vector<GXFTrackState *> &states = trajectory.trackStates();
    std::vector<GXFTrackState *> matstates, newstates;
    const std::vector<const TrackStateOnSurface *> *matvec = 0;
    const TrackParameters *startmatpar1 = 0;
    const TrackParameters *startmatpar2 = 0;
    const TrackParameters *firstidpar = 0;
    const TrackParameters *lastidpar = 0;
    const TrackParameters *firstsiliconpar = 0;
    const TrackParameters *lastsiliconpar = 0;
    const TrackParameters *firstmatpar = 0;
    // const TrackParameters *lastmatpar= 0;
    const TrackParameters *firstcalopar = 0;
    const TrackParameters *lastcalopar = 0;
    const TrackParameters *firstmuonpar = 0;
    const TrackParameters *lastmuonpar = 0;

    // GXFTrackState *firstmuonstate=0;
    // int index_firstmuonhit=0,index_lastmuonhit=0;
    int npseudomuon1 = 0, npseudomuon2 = 0;

    for (int i = 0; i < (int) states.size(); i++) {
      TrackState::MeasurementType meastype = states[i]->measurementType();
      TrackState::TrackStateType tstype = states[i]->trackStateType();
      const TrackParameters *tp = states[i]->trackParameters();
      GXFMaterialEffects *meff = states[i]->materialEffects();
      if (meastype == TrackState::Pseudo) {
        if (!firstidhit) {
          npseudomuon1++;
        } else {
          npseudomuon2++;
        }
        continue;
      }
      if (tstype == TrackState::Fittable || tstype == TrackState::GeneralOutlier) {
        if (!firsthit) {
          firsthit = states[i]->measurement();
          if (m_acceleration) {
            if (!tp) {
              tp = m_extrapolator->extrapolate(*refpar2, *states[i]->surface(), alongMomentum, false, matEffects);
              if (!tp) {
                return;
              }
              states[i]->setTrackParameters(tp);
            }
            refpar2 = tp;  // When acceleration is enabled, material collection starts from first hit
          }
        }
        lasthit = states[i]->measurement();
        if (meastype == TrackState::Pixel || meastype == TrackState::SCT || meastype == TrackState::TRT) {
          if (!firstidhit) {
            firstidhit = states[i]->measurement();
          }
          if (!firstidpar && tp) {
            firstidpar = tp;
          }

          lastidhit = states[i]->measurement();
          if (tp) {
            lastidpar = tp;
          }
          if (tp && meastype != TrackState::TRT) {
            if (!firstsiliconpar) {
              firstsiliconpar = tp;
            }
            lastsiliconpar = tp;
          }
        }
        if (meastype == TrackState::RPC || meastype == TrackState::CSC || meastype == TrackState::TGC ||
            meastype == TrackState::MDT) {
          if (!firstmuonhit) {
            firstmuonhit = states[i]->measurement();
            // index_firstmuonhit=i;
            // firstmuonstate=states[i];
            if (tp) {
              firstmuonpar = tp;
            }
          }
          lastmuonhit = states[i]->measurement();
          // index_lastmuonhit=i;
          if (tp) {
            lastmuonpar = tp;
          }
        }
      }
      if (tstype == TrackState::Scatterer || tstype == TrackState::Brem) {
        if (meff->deltaE() == 0) {
          if (!firstcalopar) {
            firstcalopar = states[i]->trackParameters();
          }
          lastcalopar = states[i]->trackParameters();
        }
        if (!firstmatpar) {
          firstmatpar = states[i]->trackParameters();
        }
        // lastmatpar=states[i]->trackParameters();
      }
    }
    const TrackParameters *refpar = 0;
    AmgVector(5) newpars = refpar2->parameters();
    if (m_straightline && m_p != 0) {
      newpars[Trk::qOverP] = 1 / m_p;
    }
    refpar =
      refpar2->associatedSurface().createTrackParameters(newpars[0], newpars[1], newpars[2], newpars[3], newpars[4], 0);

    bool dodelete = false;
    if (firstmatpar) {
      startmatpar1 = firstsiliconpar;
      startmatpar2 = lastsiliconpar;
    }
    if (!startmatpar1 || (firstidhit && firstmuonhit)) {
      startmatpar1 = startmatpar2 = refpar;
    } else if (m_straightline && m_p != 0) {
      AmgVector(5) newpars = startmatpar1->parameters();
      newpars[Trk::qOverP] = 1 / m_p;
      startmatpar1 = startmatpar1->associatedSurface().createTrackParameters(newpars[0], newpars[1], newpars[2],
                                                                             newpars[3], newpars[4], 0);
      newpars = startmatpar2->parameters();
      newpars[Trk::qOverP] = 1 / m_p;
      startmatpar2 = startmatpar2->associatedSurface().createTrackParameters(newpars[0], newpars[1], newpars[2],
                                                                             newpars[3], newpars[4], 0);
      dodelete = true;
    }

    double mass = trajectory.mass();
    if (mass > 200 * MeV && startmatpar2 == refpar) {
      const AmgVector(5) &newpars = startmatpar2->parameters();
      double oldp = std::abs(1 / newpars[Trk::qOverP]);
      double sign = (newpars[Trk::qOverP] < 0) ? -1 : 1;
      startmatpar2 = startmatpar2->associatedSurface().createTrackParameters(newpars[0], newpars[1],
                                                                             newpars[2], newpars[3],
                                                                             sign /
                                                                             sqrt(oldp * oldp + 2 * 100 * MeV *
                                                                                  sqrt(oldp * oldp + mass * mass) +
                                                                                  100 * MeV * 100 * MeV), 0);
      dodelete = true;
    }

    if (firstidhit && trajectory.numberOfSiliconHits() > 0 && m_idmat) {
      DistanceSolution distsol = firstidhit->associatedSurface().straightLineDistanceEstimate(
        refpar->position(), refpar->momentum().unit());
      double distance = 0;
      if (distsol.numberOfSolutions() == 1) {
        distance = distsol.first();
      } else if (distsol.numberOfSolutions() == 2) {
        distance = (std::abs(distsol.first()) < std::abs(distsol.second())) ? distsol.first() : distsol.second();
      }

      if (distance < 0 && distsol.numberOfSolutions() > 0 && !m_acceleration) {
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "Obtaining upstream layers from Extrapolator" << endmsg;
        }
        const Surface *destsurf = &firstidhit->associatedSurface();
        const TrackParameters *tmppar = 0;
        if (firstmuonhit) {
          if (!m_caloEntrance) {
            m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();
            if (m_trackingGeometry) {
              m_caloEntrance = m_trackingGeometry->trackingVolume("InDet::Containers::InnerDetector");
            }
            if (!m_caloEntrance) {
              msg(MSG::ERROR) << "calo entrance not available" << endmsg;
            }
          }
          if (m_caloEntrance) {
            tmppar = m_extrapolator->extrapolateToVolume(*startmatpar1, *m_caloEntrance, oppositeMomentum,
                                                         Trk::nonInteracting);
            if (tmppar) {
              destsurf = &tmppar->associatedSurface();
            }
          }
        }
        m_matvecidupstream =
          m_extrapolator->extrapolateM(*startmatpar1, *destsurf, oppositeMomentum, false, matEffects);
        matvec = m_matvecidupstream;
        if (tmppar) {
          delete tmppar;
        }
        if (matvec && !matvec->empty()) {
          for (int i = (int) matvec->size() - 1; i > -1; i--) {
            const MaterialEffectsBase *meb = (*matvec)[i]->materialEffectsOnTrack();
            if (meb) {
              const MaterialEffectsOnTrack *meot = dynamic_cast<const MaterialEffectsOnTrack *>(meb);
              if (meot) {
                GXFMaterialEffects *meff = new GXFMaterialEffects(meot);
                meff->setSigmaDeltaE(0);
                matstates.push_back(new GXFTrackState(meff, (*matvec)[i]->trackParameters()));
              }
            }
          }
        }
      }
    }

    if (lastidhit && trajectory.numberOfSiliconHits() > 0 && m_idmat) {
      DistanceSolution distsol = lastidhit->associatedSurface().straightLineDistanceEstimate(
        refpar->position(), refpar->momentum().unit());
      double distance = 0;
      if (distsol.numberOfSolutions() == 1) {
        distance = distsol.first();
      } else if (distsol.numberOfSolutions() == 2) {
        distance = (std::abs(distsol.first()) < std::abs(distsol.second())) ? distsol.first() : distsol.second();
      }

      if (distance > 0 && distsol.numberOfSolutions() > 0) {
        ATH_MSG_DEBUG("Obtaining downstream ID layers from Extrapolator");
        const Surface *destsurf = &lastidhit->associatedSurface();
        const TrackParameters *tmppar = 0;
        Surface *calosurf = 0;
        if (firstmuonhit) {
          if (!m_caloEntrance) {
            m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();
            if (m_trackingGeometry) {
              m_caloEntrance = m_trackingGeometry->trackingVolume("InDet::Containers::InnerDetector");
            }
            if (!m_caloEntrance) {
              msg(MSG::ERROR) << "calo entrance not available" << endmsg;
            }
          }
          /* if (!m_caloEntrance2){
             m_caloEntrance2=new Trk::Volume(m_trackingVolumesSvc->volume(ITrackingVolumesSvc::CalorimeterEntryLayer));
             if (!m_caloEntrance2) msg(MSG::ERROR) << "calo entrance not available" << endmsg;
             } */
          if (m_caloEntrance) {
            tmppar = m_extrapolator->extrapolateToVolume(*startmatpar2, *m_caloEntrance, Trk::alongMomentum,
                                                         Trk::nonInteracting);
          }
          if (tmppar) {
            const CylinderSurface *cylcalosurf = dynamic_cast<const CylinderSurface *>(&tmppar->associatedSurface());
            const DiscSurface *disccalosurf = dynamic_cast<const DiscSurface *>(&tmppar->associatedSurface());
            if (cylcalosurf) {
              Amg::Transform3D *trans = new Amg::Transform3D(cylcalosurf->transform());
              const CylinderBounds &cylbounds = cylcalosurf->bounds();
              double radius = cylbounds.r();
              double hlength = cylbounds.halflengthZ();
              calosurf = new CylinderSurface(trans, radius - 1, hlength);
            }else {
              double newz =
                ((disccalosurf->center().z() > 0) ? disccalosurf->center().z() - 1 : disccalosurf->center().z() + 1);
              Amg::Vector3D newpos(disccalosurf->center().x(), disccalosurf->center().y(), newz);
              Amg::Transform3D *trans = new Amg::Transform3D(disccalosurf->transform());
              trans->translation() << newpos;
              const DiscBounds *discbounds = dynamic_cast<const DiscBounds *>(&disccalosurf->bounds());
              double rmin = discbounds->rMin();
              double rmax = discbounds->rMax();
              calosurf = new DiscSurface(trans, rmin, rmax);
            }
            destsurf = calosurf;
          }
        }
        m_matveciddownstream = m_extrapolator->extrapolateM(*startmatpar2, *destsurf, alongMomentum, false, matEffects);
        matvec = m_matveciddownstream;
        if (tmppar) {
          delete tmppar;
        }
        if (calosurf) {
          delete calosurf;
        }
        if (matvec && !matvec->empty()) {
          for (int i = 0; i < (int) matvec->size(); i++) {
            const Trk::MaterialEffectsBase *meb = (*matvec)[i]->materialEffectsOnTrack();
            if (meb) {
              const MaterialEffectsOnTrack *meot = dynamic_cast<const MaterialEffectsOnTrack *>(meb);
              if (meot) {
                GXFMaterialEffects *meff = new GXFMaterialEffects(meot);
                if (m_fiteloss && meot->energyLoss() /*&& (matEffects==muon || matEffects==pion)*/) {
                  meff->setSigmaDeltaE(meot->energyLoss()->sigmaDeltaE());
                }
                if (matEffects == electron && m_asymeloss) {
                  meff->setDeltaE(-5);
                  if (!trajectory.numberOfTRTHits()) {
                    meff->setScatteringSigmas(0, 0);
                  }
                  meff->setSigmaDeltaE(50);
                }

                matstates.push_back(new GXFTrackState(meff, (*matvec)[i]->trackParameters()));
                // if (m_caloEntrance) std::cout << "tp inside calo: " <<
                // m_caloEntrance->inside((*matvec)[i]->trackParameters()->position()) << " ref point inside calo: " <<
                // m_caloEntrance->inside((*matvec)[i]->trackParameters()->associatedSurface()->globalReferencePoint())
                // << std::endl;
                // if (m_caloEntrance2) std::cout << "tp inside calo: " <<
                // m_caloEntrance2->inside((*matvec)[i]->trackParameters()->position()) << " ref point inside calo: " <<
                // m_caloEntrance2->inside((*matvec)[i]->trackParameters()->associatedSurface()->globalReferencePoint())
                // << std::endl;
              }
            }
          }
        }else {
          msg(MSG::WARNING) << "No material layers collected from Extrapolator" << endmsg;
        }
      }
    }
    if (dodelete) {
      if (startmatpar1 != refpar) {
        delete startmatpar1;
      }
      delete startmatpar2;
    }


    if (m_calomat && firstmuonhit && firstidhit) {
      const IPropagator *prop = &*m_propagator;

      m_calomeots = m_calotool->extrapolationSurfacesAndEffects(
        *m_navigator->highestVolume(), *prop, *lastidpar, firstmuonhit->associatedSurface(), alongMomentum, muon);
      if (!m_calomeots.empty()) {
        const TrackParameters *prevtrackpars = lastidpar;
        if (lasthit == lastmuonhit) {
          for (int i = 0; i < (int) m_calomeots.size(); i++) {
            // if (i==1 && !m_fieldService->toroidOn()) continue;
            PropDirection propdir = alongMomentum;
            const TrackParameters *layerpar = m_propagator->propagateParameters(*prevtrackpars,
                                                                                m_calomeots[i].associatedSurface(), propdir, false, *m_fieldprop,
                                                                                nonInteracting);
            if (!layerpar) {
              m_propfailed++;
              return;
            }

            GXFMaterialEffects *meff = new GXFMaterialEffects(&m_calomeots[i]);
            if (i == 2) {
              lastcalopar = layerpar;
            }
            if (i == 1) {
              double qoverp = layerpar->parameters()[Trk::qOverP];

              double qoverpbrem = 0;
              if (npseudomuon2 < 2 && firstmuonpar && fabs(firstmuonpar->parameters()[Trk::qOverP]) > 1.e-9) {
                qoverpbrem = firstmuonpar->parameters()[Trk::qOverP];
              } else {
                double sign = (qoverp > 0) ? 1 : -1;
                qoverpbrem = sign / (1 / std::abs(qoverp) - std::abs(m_calomeots[i].energyLoss()->deltaE()));
              }
              const AmgVector(5) &newpar = layerpar->parameters();

              const TrackParameters *newlayerpar = layerpar->associatedSurface().createTrackParameters(newpar[0],
                                                                                                       newpar[1],
                                                                                                       newpar[2],
                                                                                                       newpar[3],
                                                                                                       qoverpbrem, 0);
              delete layerpar;
              layerpar = newlayerpar;
              meff->setdelta_p(1000 * (qoverpbrem - qoverp));
            }

            matstates.push_back(new GXFTrackState(meff, layerpar));
            prevtrackpars = layerpar;
          }
        }
        if (firsthit == firstmuonhit && (!m_getmaterialfromtrack || lasthit == lastidhit)) {
          prevtrackpars = firstidpar;
          for (int i = 0; i < (int) m_calomeots.size(); i++) {
            // if (i==1 && !m_fieldService->toroidOn()) continue;
            PropDirection propdir = oppositeMomentum;
            const TrackParameters *layerpar = m_propagator->propagateParameters(*prevtrackpars,
                                                                                m_calomeots[i].associatedSurface(), propdir, false, *m_fieldprop,
                                                                                nonInteracting);
            if (i == 2 /*&& m_fieldService->toroidOn()*/) {
              delete prevtrackpars;
            }
            if (!layerpar) {
              m_propfailed++;
              return;
            }
            GXFMaterialEffects *meff = new GXFMaterialEffects(&m_calomeots[i]);
            if (i == 2) {
              firstcalopar = layerpar;
            }

            prevtrackpars = layerpar;
            if (i == 1) {
              double qoverpbrem = layerpar->parameters()[Trk::qOverP];
              double qoverp = 0;
              if (npseudomuon1 < 2 && lastmuonpar && fabs(lastmuonpar->parameters()[Trk::qOverP]) > 1.e-9) {
                qoverp = lastmuonpar->parameters()[Trk::qOverP];
              } else {
                double sign = (qoverpbrem > 0) ? 1 : -1;
                qoverp = sign / (1 / std::abs(qoverpbrem) + std::abs(m_calomeots[i].energyLoss()->deltaE()));
              }
              meff->setdelta_p(1000 * (qoverpbrem - qoverp));
              const AmgVector(5) &newpar = layerpar->parameters();

              prevtrackpars = layerpar->associatedSurface().createTrackParameters(newpar[0], newpar[1], newpar[2],
                                                                                  newpar[3], qoverp, 0);
            }
            matstates.insert(matstates.begin(), new GXFTrackState(meff, layerpar));
          }
        }
      }else {
        msg(MSG::WARNING) << "No material layers collected in calorimeter" << endmsg;
        for (int i = 0; i < (int) matstates.size(); i++) {
          delete matstates[i];
        }
        delete refpar;
        return;
      }
    }

    if (lasthit == lastmuonhit && m_extmat) {
      const Trk::TrackParameters *muonpar1 = 0;
      if (lastcalopar) {
        if (!m_msEntrance) {
          m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();
          if (m_trackingGeometry) {
            m_msEntrance = m_trackingGeometry->trackingVolume("MuonSpectrometerEntrance");
          }
          if (!m_msEntrance) {
            msg(MSG::ERROR) << "MS entrance not available" << endmsg;
          }
        }
        if (m_msEntrance && m_msEntrance->inside(lastcalopar->position())) {
          muonpar1 = m_extrapolator->extrapolateToVolume(*lastcalopar, *m_msEntrance, Trk::alongMomentum,
                                                         Trk::nonInteracting);
          if (muonpar1) {
            Amg::Vector3D trackdir = muonpar1->momentum().unit();
            Amg::Vector3D curvZcrossT = -(trackdir.cross(Amg::Vector3D(0, 0, 1)));
            Amg::Vector3D curvU = curvZcrossT.unit();
            Amg::Vector3D curvV = trackdir.cross(curvU);
            Amg::RotationMatrix3D rot = Amg::RotationMatrix3D::Identity();
            rot.col(0) = curvU;
            rot.col(1) = curvV;
            rot.col(2) = trackdir;
            Amg::Transform3D *trans = new Amg::Transform3D;
            trans->linear().matrix() << rot;
            trans->translation() << muonpar1->position() - .1 *trackdir;
            PlaneSurface curvlinsurf(trans);

            const TrackParameters *curvlinpar = m_extrapolator->extrapolateDirectly(*muonpar1, curvlinsurf,
                                                                                    Trk::alongMomentum,
                                                                                    Trk::nonInteracting);
            if (curvlinpar) {
              delete muonpar1;
              muonpar1 = curvlinpar;
            }
          }
        }else {
          muonpar1 = lastcalopar;
        }
      }else {
        muonpar1 = refpar;
      }

      DistanceSolution distsol;
      if (muonpar1) {
        distsol = lastmuonhit->associatedSurface().straightLineDistanceEstimate(
          muonpar1->position(), muonpar1->momentum().unit());
      }
      double distance = 0;
      if (distsol.numberOfSolutions() == 1) {
        distance = distsol.first();
      } else if (distsol.numberOfSolutions() == 2) {
        distance = (std::abs(distsol.first()) < std::abs(distsol.second())) ? distsol.first() : distsol.second();
      }

      if ((distance > 0)and(distsol.numberOfSolutions() > 0) and firstmuonhit) {
        distsol = firstmuonhit->associatedSurface().straightLineDistanceEstimate(
          muonpar1->position(), muonpar1->momentum().unit());
        distance = 0;
        if (distsol.numberOfSolutions() == 1) {
          distance = distsol.first();
        } else if (distsol.numberOfSolutions() == 2) {
          distance = (std::abs(distsol.first()) < std::abs(distsol.second())) ? distsol.first() : distsol.second();
        }
        if (distance < 0 && distsol.numberOfSolutions() > 0 && !firstidhit) {
          if (firstmuonpar) {
            AmgVector(5) newpars = firstmuonpar->parameters();
            if (m_straightline && m_p != 0) {
              newpars[Trk::qOverP] = 1 / m_p;
            }
            muonpar1 = firstmuonpar->associatedSurface().createTrackParameters(newpars[0], newpars[1], newpars[2],
                                                                               newpars[3], newpars[4], 0);
          }else {
            const TrackParameters *tmppar = m_propagator->propagateParameters(*muonpar1,
                                                                              firstmuonhit->associatedSurface(), oppositeMomentum, false, *m_fieldprop,
                                                                              nonInteracting);
            if (tmppar) {
              muonpar1 = tmppar;
            }
          }
        }
        const TrackParameters *prevtp = muonpar1;
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "Obtaining downstream layers from Extrapolator" << endmsg;
        }
        // std::cout << "prevtp: " << *prevtp << std::endl;
        m_matvecmuondownstream = m_extrapolator->extrapolateM(*prevtp,
                                                              *states.back()->surface(), alongMomentum, false,
                                                              Trk::nonInteractingMuon);
        matvec = m_matvecmuondownstream;
	if(matvec->size()>1000 && m_rejectLargeNScat){
	  ATH_MSG_DEBUG("too many scatterers: "<<matvec->size());
	  return;
	}
        if (matvec && !matvec->empty()) {
          for (int j = 0; j < (int) matvec->size(); j++) {
            const MaterialEffectsBase *meb = (*matvec)[j]->materialEffectsOnTrack();
            if (meb) {
              const MaterialEffectsOnTrack *meot = dynamic_cast<const MaterialEffectsOnTrack *>(meb);
              if (meot && j < (int) matvec->size() - 1) {
                GXFMaterialEffects *meff = new GXFMaterialEffects(meot);
                if (!m_straightline && meot->energyLoss() && std::abs(meff->deltaE()) > 25 &&
                    std::abs((*matvec)[j]->trackParameters()->position().z()) < 13000) {
                  meff->setSigmaDeltaE(meot->energyLoss()->sigmaDeltaE());
                }
                matstates.push_back(new GXFTrackState(meff, (*matvec)[j]->trackParameters()));
              }
            }
          }
        }
      }
      if (muonpar1 != refpar && muonpar1 != lastcalopar) {
        delete muonpar1;
      }
    }

    if (firsthit == firstmuonhit && m_extmat && firstcalopar) {
      const Trk::TrackParameters *muonpar1 = 0;
      if (firstcalopar) {
        if (!m_msEntrance) {
          m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();
          if (m_trackingGeometry) {
            m_msEntrance = m_trackingGeometry->trackingVolume("MuonSpectrometerEntrance");
          }
          if (!m_msEntrance) {
            msg(MSG::ERROR) << "MS entrance not available" << endmsg;
          }
        }
        if (m_msEntrance && m_msEntrance->inside(firstcalopar->position())) {
          muonpar1 = m_extrapolator->extrapolateToVolume(*firstcalopar, *m_msEntrance, Trk::oppositeMomentum,
                                                         Trk::nonInteracting);
          if (muonpar1) {
            Amg::Vector3D trackdir = muonpar1->momentum().unit();
            Amg::Vector3D curvZcrossT = -(trackdir.cross(Amg::Vector3D(0, 0, 1)));
            Amg::Vector3D curvU = curvZcrossT.unit();
            Amg::Vector3D curvV = trackdir.cross(curvU);
            Amg::RotationMatrix3D rot = Amg::RotationMatrix3D::Identity();
            rot.col(0) = curvU;
            rot.col(1) = curvV;
            rot.col(2) = trackdir;
            Amg::Transform3D *trans = new Amg::Transform3D;
            trans->linear().matrix() << rot;
            trans->translation() << muonpar1->position() - .1 *trackdir;
            PlaneSurface curvlinsurf(trans);

            const TrackParameters *curvlinpar = m_extrapolator->extrapolateDirectly(*muonpar1, curvlinsurf,
                                                                                    Trk::oppositeMomentum,
                                                                                    Trk::nonInteracting);
            if (curvlinpar) {
              delete muonpar1;
              muonpar1 = curvlinpar;
            }
          }
        }else {
          muonpar1 = firstcalopar;
        }
      }else {
        muonpar1 = refpar;
      }
      DistanceSolution distsol;
      if (muonpar1) {
        distsol = firstmuonhit->associatedSurface().straightLineDistanceEstimate(
          muonpar1->position(), muonpar1->momentum().unit());
      }
      double distance = 0;
      if (distsol.numberOfSolutions() == 1) {
        distance = distsol.first();
      } else if (distsol.numberOfSolutions() == 2) {
        distance = (std::abs(distsol.first()) < std::abs(distsol.second())) ? distsol.first() : distsol.second();
      }
      // std::cout << "muonpar1: " << muonpar1 << " distsol: " << distsol.numberOfSolutions() << " distance: " <<
      // distance << std::endl;
      if (distance < 0 && distsol.numberOfSolutions() > 0) {
        const TrackParameters *prevtp = muonpar1;
        msg(MSG::DEBUG) << "Collecting upstream muon material from extrapolator" << endmsg;

        m_matvecmuonupstream = m_extrapolator->extrapolateM(*prevtp,
                                                            *states[0]->surface(), oppositeMomentum, false,
                                                            Trk::nonInteractingMuon);
        matvec = m_matvecmuonupstream;
        if (matvec && !matvec->empty()) {
          msg(MSG::DEBUG) << "Retrieved " << matvec->size() << " material states" << endmsg;
          for (int j = 0; j < (int) matvec->size(); j++) {
            const MaterialEffectsBase *meb = (*matvec)[j]->materialEffectsOnTrack();
            if (meb) {
              const MaterialEffectsOnTrack *meot = dynamic_cast<const MaterialEffectsOnTrack *>(meb);
              if (meot && j < (int) matvec->size() - 1) {
                GXFMaterialEffects *meff = new GXFMaterialEffects(meot);
                if (!m_straightline && meot->energyLoss() && std::abs(meff->deltaE()) > 25 &&
                    std::abs((*matvec)[j]->trackParameters()->position().z()) < 13000) {
                  meff->setSigmaDeltaE(meot->energyLoss()->sigmaDeltaE());
                }
                matstates.insert(matstates.begin(), new GXFTrackState(meff, (*matvec)[j]->trackParameters()));
              }
            }
          }
        }
      }
      if (muonpar1 != refpar && muonpar1 != firstcalopar) {
        delete muonpar1;
      }
    }
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Number of layers: " << matstates.size() << endmsg;
    }

    // Now insert the material states into the trajectory
    std::vector<GXFTrackState *> oldstates = states;
    trajectory.setTrackStates(newstates);
    states.reserve(oldstates.size() + matstates.size());
    int layerno = 0, firstlayerno = -1;
    if (m_acceleration) {
      states.push_back(oldstates[0]);
    }
    double cosphi = cos(refpar->parameters()[Trk::phi0]);
    double sinphi = sin(refpar->parameters()[Trk::phi0]);
    for (int i = m_acceleration ? 1 : 0; i < (int) oldstates.size(); i++) {
      bool addlayer = true;
      while (addlayer && layerno < (int) matstates.size()) {
        addlayer = false;
	//std::cout<<"add "<<matstates.size()<<" states"<<std::endl;
        const TrackParameters *layerpar = matstates[layerno]->trackParameters();


        DistanceSolution distsol = oldstates[i]->surface()->straightLineDistanceEstimate(
          layerpar->position(), layerpar->momentum().unit());
        double distance = 0;
        if (distsol.numberOfSolutions() == 1) {
          distance = distsol.first();
        } else if (distsol.numberOfSolutions() == 2) {
          distance = (std::abs(distsol.first()) < std::abs(distsol.second())) ? distsol.first() : distsol.second();
        }

        if (distance > 0 && distsol.numberOfSolutions() > 0) {
          addlayer = true;
        }

        if (dynamic_cast<const AtaCylinder *>(layerpar)) {
          // Hep3Vector diffvec=states[i]->position()-layerparams[layerno]->position();
          double cylinderradius = layerpar->associatedSurface().bounds().r();
          double trackimpact = std::abs(-refpar->position().x() * sinphi + refpar->position().y() * cosphi);
          if (trackimpact > cylinderradius - 5 * mm) {
            delete matstates[layerno];
            layerno++;
            continue;
          }
        }
        if (i == (int) oldstates.size() - 1) {
          addlayer = true; // force insertion of last layer, to be consistent with extrapolator
        }
        if (addlayer) {
          GXFMaterialEffects *meff = matstates[layerno]->materialEffects();
          if (meff->sigmaDeltaPhi() > .4 || (meff->sigmaDeltaPhi() == 0 && meff->sigmaDeltaE() <= 0)) {
            if (meff->sigmaDeltaPhi() > .4) {
              msg(MSG::WARNING) << "Material state with excessive scattering, skipping it" << endmsg;
            }
            if (meff->sigmaDeltaPhi() == 0) {
              msg(MSG::WARNING) << "Material state with zero scattering, skipping it" << endmsg;
            }
            delete matstates[layerno];
            layerno++;
            continue;
          }
          if (firstlayerno < 0) {
            firstlayerno = layerno;
          }

          // trajectory.addMaterialState(meff,layerparams[layerno],i,true);
          trajectory.addMaterialState(matstates[layerno]);
          // const TrackParameters *layerpars=matstates[layerno]->trackParameters();
          if (layerpar && matEffects != pion && matEffects != muon) {
            const TrackingVolume *tvol = m_navigator->volume(layerpar->position());
            const Layer *lay = 0;
            if (tvol) {
              lay = (tvol->closestMaterialLayer(layerpar->position(), layerpar->momentum().normalized())).object;
            }
            // if (lay) std::cout << "mat: " << lay->layerMaterialProperties() << " sensvol: " <<
            // tvol->sensitiveVolume() << " inside: ";
            // if (tvol->sensitiveVolume()) std::cout << tvol->sensitiveVolume()->inside(layerpar->position());
            // std::cout << std::endl;
            const MaterialProperties *matprop = lay ? lay->fullUpdateMaterialProperties(*layerpar) : 0;
            meff->setMaterialProperties(matprop);
          }
	  /*
          if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << "X0: " << meff->x0();
            if (layerpar) {
              msg(MSG::DEBUG) << " qoverp: " << layerpar->parameters()[Trk::qOverP];
            }
            msg(MSG::DEBUG) << " sigmascat " << meff->sigmaDeltaTheta();
            msg(MSG::DEBUG) << " eloss: " << meff->deltaE() << " sigma eloss: " << meff->sigmaDeltaE();
            msg(MSG::DEBUG) << endmsg;
          }
	  */
          layerno++;
        }
      }
      states.push_back(oldstates[i]);
    }
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Total X0: " << trajectory.totalX0() << " total eloss: " << trajectory.totalEnergyLoss() <<
        endmsg;
    }

    for (; layerno < (int) matstates.size(); layerno++) {
      delete matstates[layerno];
    }
    delete refpar;

    return;
  }

  const TrackParameters *
  GlobalChi2Fitter::makePerigee(const TrackParameters &param, ParticleHypothesis matEffects) const {
    const PerigeeSurface *persurf = dynamic_cast<const PerigeeSurface *>(&param.associatedSurface());
    const TrackParameters *per = 0;

    if (persurf && (!m_acceleration || persurf->center().perp() > 5)) {
      const AmgVector(5) &pars = param.parameters();
      const TrackParameters *newper = param.associatedSurface().createTrackParameters(pars[0], pars[1], pars[2],
                                                                                      pars[3], pars[4], 0);
      return newper;
    }
    if (m_acceleration) {
      return 0;
    }
    PerigeeSurface tmppersf;
    per = m_extrapolator->extrapolate(param, tmppersf, oppositeMomentum, false, matEffects);
    if (!per) {
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "Cannot make Perigee with starting parameters" << endmsg;
      }
      return 0;
    }
    return per;
  }

#ifdef GXFDEBUGCODE
  void
  GlobalChi2Fitter::retrieveTruth() const {
    if (evtStore()->contains<PRD_MultiTruthCollection>(m_multiTruthCollectionPixelName)) {
      StatusCode sc = evtStore()->retrieve(m_truthCollectionPixel, m_multiTruthCollectionPixelName);
      if (sc.isFailure()) {
        msg(MSG::INFO) << " could not open PRD_MultiTruthCollection : " << m_multiTruthCollectionPixelName << endmsg;
        sc = StatusCode::SUCCESS; // not having truth information is not a failure
        m_truthCollectionPixel = 0;
      }
    }

    if (evtStore()->contains<PRD_MultiTruthCollection>(m_multiTruthCollectionSCTName)) {
      StatusCode sc = evtStore()->retrieve(m_truthCollectionSCT, m_multiTruthCollectionSCTName);
      if (sc.isFailure()) {
        msg(MSG::INFO) << " could not open PRD_MultiTruthCollection : " << m_multiTruthCollectionSCTName << endmsg;
        sc = StatusCode::SUCCESS; // not having truth information is not a failure
        m_truthCollectionSCT = 0;
      }
    }

    if (evtStore()->contains<PRD_MultiTruthCollection>(m_multiTruthCollectionTRTName)) {
      StatusCode sc = evtStore()->retrieve(m_truthCollectionTRT, m_multiTruthCollectionTRTName);
      if (sc.isFailure()) {
        msg(MSG::INFO) << " could not open PRD_MultiTruthCollection : " << m_multiTruthCollectionTRTName << endmsg;
        sc = StatusCode::SUCCESS; // not having truth information is not a failure
        m_truthCollectionTRT = 0;
      }
    }
    // std::cout << "retrieving mdt truth" << std::endl;
    // std::cout << "mdt truth: " << m_multiTruthCollectionMDTName << " " <<
    // evtStore()->contains<PRD_MultiTruthCollection>(m_multiTruthCollectionMDTName) << std::endl;
    if (evtStore()->contains<PRD_MultiTruthCollection>(m_multiTruthCollectionMDTName)) {
      StatusCode sc = evtStore()->retrieve(m_truthCollectionMDT, m_multiTruthCollectionMDTName);
      if (sc.isFailure()) {
        msg(MSG::INFO) << " could not open PRD_MultiTruthCollection : " << m_multiTruthCollectionMDTName << endmsg;
        sc = StatusCode::SUCCESS; // not having truth information is not a failure
        m_truthCollectionMDT = 0;
      }
      // else std::cout << "mdt truth retrieved" << std::endl;
    }
    if (evtStore()->contains<PRD_MultiTruthCollection>(m_multiTruthCollectionRPCName)) {
      StatusCode sc = evtStore()->retrieve(m_truthCollectionRPC, m_multiTruthCollectionRPCName);
      if (sc.isFailure()) {
        msg(MSG::INFO) << " could not open PRD_MultiTruthCollection : " << m_multiTruthCollectionRPCName << endmsg;
        sc = StatusCode::SUCCESS; // not having truth information is not a failure
        m_truthCollectionRPC = 0;
      }
    }
    if (evtStore()->contains<PRD_MultiTruthCollection>(m_multiTruthCollectionTGCName)) {
      StatusCode sc = evtStore()->retrieve(m_truthCollectionTGC, m_multiTruthCollectionTGCName);
      if (sc.isFailure()) {
        msg(MSG::INFO) << " could not open PRD_MultiTruthCollection : " << m_multiTruthCollectionTGCName << endmsg;
        sc = StatusCode::SUCCESS; // not having truth information is not a failure
        m_truthCollectionTGC = 0;
      }
    }
    if (evtStore()->contains<PRD_MultiTruthCollection>(m_multiTruthCollectionCSCName)) {
      StatusCode sc = evtStore()->retrieve(m_truthCollectionCSC, m_multiTruthCollectionCSCName);
      if (sc.isFailure()) {
        msg(MSG::INFO) << " could not open PRD_MultiTruthCollection : " << m_multiTruthCollectionCSCName << endmsg;
        sc = StatusCode::SUCCESS; // not having truth information is not a failure
        m_truthCollectionCSC = 0;
      }
    }
  }

  void
  GlobalChi2Fitter::printTruth(Identifier id) const {
    std::pair<PRD_MultiTruthCollection::const_iterator, PRD_MultiTruthCollection::const_iterator> range;

    if (m_DetID->is_indet(id) && !m_DetID->is_muon(id)) {
      if (m_DetID->is_pixel(id)) {
        range = m_truthCollectionPixel->equal_range(id);
      } else if (m_DetID->is_sct(id)) {
        range = m_truthCollectionSCT->equal_range(id);
      } else if (m_DetID->is_trt(id)) {
        range = m_truthCollectionTRT->equal_range(id);
      }
    }else {
      if (m_DetID->is_mdt(id)) {
        range = m_truthCollectionMDT->equal_range(id);
      } else if (m_DetID->is_rpc(id)) {
        range = m_truthCollectionRPC->equal_range(id);
      } else if (m_DetID->is_tgc(id)) {
        range = m_truthCollectionTGC->equal_range(id);
      } else if (m_DetID->is_csc(id)) {
        range = m_truthCollectionCSC->equal_range(id);
      }
    }
    int nparticles = 0;
    for (PRD_MultiTruthCollection::const_iterator i = range.first; i != range.second; i++) {
      int barcode = i->second.barcode();
      m_barcode = barcode;
      int pdgid = i->second.cptr()->pdg_id();
      msg(MSG::DEBUG) << " barcode: " << barcode << " pdgid: " << pdgid;
      nparticles++;
    }
    if (nparticles > 1) {
      msg(MSG::DEBUG) << " is shared!";
    }
    // std::cout << "nparticles: " << nparticles << std::endl;
  }
#endif

  Track *
  GlobalChi2Fitter::myfit(GXFTrajectory &trajectory,
                          const TrackParameters &param,
                          const RunOutlierRemoval runOutlier,
                          const ParticleHypothesis matEffects) const {
    m_fittercode = FitterStatusCode::Success;
    m_straightline = m_straightlineprop;
    if (!m_straightline) {
      if (trajectory.numberOfSiliconHits() + trajectory.numberOfTRTHits() == trajectory.numberOfHits()) {
        m_straightline = !m_fieldService->solenoidOn();
      } else if (!trajectory.prefit() && trajectory.numberOfSiliconHits() + trajectory.numberOfTRTHits() == 0) {
        m_straightline = !m_fieldService->toroidOn();
      } else {
        m_straightline = (!m_fieldService->solenoidOn() && !m_fieldService->toroidOn());
      }
    }
    m_fieldprop = m_straightline ? m_fieldpropnofield : m_fieldpropfullfield;
    m_lastiter = 0;

    if (trajectory.numberOfPerigeeParameters() == -1) {
      m_nfits++;
      if (m_straightline) {
        trajectory.setNumberOfPerigeeParameters(4);
      } else {
        trajectory.setNumberOfPerigeeParameters(5);
      }
    }
    if (trajectory.nDOF() < 0) {
      msg(MSG::WARNING) << "Not enough measurements, reject track" << endmsg;
      // cleanup();
      return 0;
    }

    m_hitcount = 0;
    m_updatescat = false;
    m_phiweight.clear();
    m_firstmeasurement.clear();
    m_lastmeasurement.clear();

    if (matEffects != nonInteracting && param.parameters()[Trk::qOverP] == 0 && m_p == 0) {
      msg(MSG::WARNING) << "Attempt to apply material corrections with q/p=0, reject track" << endmsg;
      // cleanup();
      return 0;
    }

    if (matEffects == Trk::electron && m_straightline) {
      msg(MSG::WARNING) << "Electron fit requires helix track model" << endmsg;
      // cleanup();
      return 0;
    }

    double mass = m_particleMasses.mass[matEffects];
    trajectory.setMass(mass);

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "start param: " << param << " pos: " << param.position() << " pt: " << param.pT() << endmsg;
    }
    const TrackParameters *per = makePerigee(param, matEffects); // When acceleration is enabled, perigee is constructed
                                                                 // below instead of in makePerigee()
    if (!m_acceleration && !per) {
      m_fittercode = FitterStatusCode::ExtrapolationFailure;
      m_propfailed++;
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "Propagation to perigee failed 1" << endmsg;
      }
      // cleanup();
      return 0;
    }
    if (matEffects != Trk::nonInteracting && !m_matfilled) {
      if (m_fastmat && m_acceleration &&
          trajectory.numberOfSiliconHits() + trajectory.numberOfTRTHits() == trajectory.numberOfHits() &&
          (m_matupdator.empty() || m_trackingGeometrySvc.empty())) {
        ATH_MSG_WARNING("Tracking Geometry Service and/or Material Updator Tool not configured");
        ATH_MSG_WARNING("Falling back to slow material collection");
        m_fastmat = false;
      }
      if (!m_fastmat || !m_acceleration ||
          trajectory.numberOfSiliconHits() + trajectory.numberOfTRTHits() != trajectory.numberOfHits()) {
        addMaterial(trajectory, per ? per : &param, matEffects);
      } else {
        addIDMaterialFast(trajectory, per ? per : &param, matEffects);
      }
    }
    // std::cout << "accel: " << m_acceleration << " refpar: " << trajectory.referenceParameters() << " per: " << per <<
    // std::endl;
    if (m_acceleration && !trajectory.referenceParameters() && !per) {
      Amg::Vector3D vertex;
      if (trajectory.numberOfScatterers() >= 2) {
        GXFTrackState *scatstate = 0, *scatstate2 = 0;
        int scatindex = 0;
        for (std::vector<GXFTrackState *>::iterator it = trajectory.trackStates().begin();
             it != trajectory.trackStates().end(); it++) {
          if ((**it).trackStateType() == TrackState::Scatterer) {
            if (scatindex == trajectory.numberOfScatterers() / 2 || (**it).materialEffects()->deltaE() == 0) {
              scatstate2 = (*it);
              break;
            }
            scatindex++;
            scatstate = (*it);
          }
        }
        // @TODO coverity complains about a possible null pointer dereferencing in scatstate->... or scatstate2->...
        // it seems to me that if (**it).materialEffects()->deltaE()==0 of the first scatterer
        // than scatstate will be NULL.
        if (!scatstate || !scatstate2) {
          throw std::logic_error("Invalid scatterer");
        }
        vertex = .49 * (scatstate->position() + scatstate2->position());
      }else {
        int nstates = (int) trajectory.trackStates().size();
        // std::cout << "nstates: " << nstates << std::endl;
        vertex = .49 *
                 (trajectory.trackStates()[nstates / 2 - 1]->position() +
                  trajectory.trackStates()[nstates / 2]->position());
      }

      PerigeeSurface persurf(vertex);
      const TrackParameters *nearestpar = 0;
      double mindist = 99999;
      std::vector<GXFTrackState *> mymatvec;
      for (std::vector<GXFTrackState *>::iterator it = trajectory.trackStates().begin();
           it != trajectory.trackStates().end(); it++) {
        if (!(**it).trackParameters()) {
          continue;
        }
        double distance = persurf.straightLineDistanceEstimate(
          (**it).trackParameters()->position(), (**it).trackParameters()->momentum().unit()).first();
        bool insideid = (!m_caloEntrance || m_caloEntrance->inside((**it).trackParameters()->position()));
        // if ((**it).materialEffects()) std::cout << "mat: " << (**it).materialEffects() << " distance: " << distance
        // << " dE: " << (**it).materialEffects()->deltaE() << " sigdphi: " << (**it).materialEffects()->sigmaDeltaPhi()
        // << " dphi: " << (**it).materialEffects()->deltaPhi() << std::endl;
        if (((**it).measurement() && insideid) ||
            ((**it).materialEffects() && distance > 0 &&
             ((**it).materialEffects()->deltaE() == 0 ||
              ((**it).materialEffects()->sigmaDeltaPhi() == 0 && !insideid) ||
              (**it).materialEffects()->deltaPhi() != 0))) {
          double dist = ((**it).trackParameters()->position() - vertex).perp();
          if (dist < mindist) {
            mindist = dist;
            nearestpar = (**it).trackParameters();
            mymatvec.clear();
            continue;
          }
        }
        if ((**it).materialEffects()) {
          if (distance > 0) {
            mymatvec.push_back(*it);
          }
        }
      }
      if (!nearestpar) {
        nearestpar = &param;
      }
      // std::cout << "nearestpar: " << *nearestpar << " pt: " << nearestpar->pT() << std::endl;
      for (int i = 0; i < (int) mymatvec.size(); i++) {
        Trk::PropDirection propdir = Trk::alongMomentum;
        const Surface *matsurf = mymatvec[i]->surface();
        DistanceSolution distsol = matsurf->straightLineDistanceEstimate(nearestpar->position(),
                                                                         nearestpar->momentum().unit());
        double distance = 0;
        if (distsol.numberOfSolutions() == 1) {
          distance = distsol.first();
        } else if (distsol.numberOfSolutions() == 2) {
          distance = (std::abs(distsol.first()) < std::abs(distsol.second())) ? distsol.first() : distsol.second();
        }
        if (distance < 0 && distsol.numberOfSolutions() > 0) {
          propdir = oppositeMomentum;
        }
        const TrackParameters *tmppar = m_propagator->propagateParameters(*nearestpar, *matsurf, propdir, false,
                                                                          *m_fieldprop, Trk::nonInteracting);
        if (!tmppar) {
          propdir = (propdir == oppositeMomentum) ? alongMomentum : oppositeMomentum;
          tmppar = m_propagator->propagateParameters(*nearestpar, *matsurf, propdir, false, *m_fieldprop,
                                                     Trk::nonInteracting);
          if (!tmppar) {
            if (i != 0) {
              delete nearestpar;
            }
            m_fittercode = FitterStatusCode::ExtrapolationFailure;
            m_propfailed++;
            if (msgLvl(MSG::DEBUG)) {
              msg(MSG::DEBUG) << "Propagation to perigee failed 2" << endmsg;
            }
            // cleanup();
            return 0;
          }
        }
        if (i != 0) {
          delete nearestpar;
        }

        AmgVector(5) newpars = tmppar->parameters();
        if (mymatvec[i]->materialEffects()->sigmaDeltaE() > 0) {
          newpars[Trk::qOverP] += .001 * mymatvec[i]->materialEffects()->delta_p();
        } else if (newpars[Trk::qOverP] != 0) {
          double sign = (newpars[Trk::qOverP] > 0) ? 1 : -1;
          double de = std::abs(mymatvec[i]->materialEffects()->deltaE());
          double oldp = std::abs(1 / newpars[Trk::qOverP]);
          double newp2 = oldp * oldp - 2 * de * sqrt(mass * mass + oldp * oldp) + de * de;
          if (newp2 > 0) {
            newpars[Trk::qOverP] = sign / sqrt(newp2);
          }
        }
        nearestpar = tmppar->associatedSurface().createTrackParameters(newpars[0], newpars[1], newpars[2], newpars[3],
                                                                       newpars[4], 0);
        delete tmppar;
      }
      // std::cout << "persurf: " << persurf << " nearest par: " << *nearestpar << std::endl;
      per =
        dynamic_cast<const Perigee *>(m_propagator->propagateParameters(*nearestpar, persurf, Trk::anyDirection, false,
                                                                        *m_fieldprop, Trk::nonInteracting));
      if (!mymatvec.empty()) {
        delete nearestpar;
      }
      if (per && (matEffects == Trk::proton || matEffects == Trk::kaon)) {
        double sign = (per->parameters()[Trk::qOverP] < 0) ? -1. : 1.;
        double oldp = 1. / std::abs(per->parameters()[Trk::qOverP]);
        double toteloss = std::abs(trajectory.totalEnergyLoss());
        double newp = sqrt(oldp * oldp + 2 * toteloss * sqrt(oldp * oldp + mass * mass) + toteloss * toteloss);
        AmgVector(5) params = per->parameters();
        params[Trk::qOverP] = sign / newp;
        const TrackParameters *newper = per->associatedSurface().createTrackParameters(params[0], params[1], params[2],
                                                                                       params[3], params[4], 0);
        delete per;
        per = newper;
      }
      if (!per) {
        m_fittercode = FitterStatusCode::ExtrapolationFailure;
        m_propfailed++;
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "Propagation to perigee failed 3" << endmsg;
        }
        // cleanup();
        return 0;
      }
      PerigeeSurface persurf2(per->position());
      const TrackParameters *oldper = per;
      per = persurf2.createTrackParameters(0, 0, per->parameters()[Trk::phi],
                                           per->parameters()[Trk::theta], per->parameters()[Trk::qOverP], 0);
      delete oldper;
    }else if (!per) {
      per = makePerigee(param, matEffects);
    }
    if (!per && !trajectory.referenceParameters()) {
      m_fittercode = FitterStatusCode::ExtrapolationFailure;
      m_propfailed++;
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "Propagation to perigee failed 4" << endmsg;
      }
      // cleanup();
      return 0;
    }
    if (m_straightline && per) {
      if (trajectory.numberOfPerigeeParameters() == -1) {
        trajectory.setNumberOfPerigeeParameters(4);
      }
      const AmgVector(5) &pars = per->parameters();
      const TrackParameters *oldper = per;
      per = per->associatedSurface().createTrackParameters(pars[0], pars[1], pars[2], pars[3], 0, 0);
      delete oldper;
    }else if (trajectory.numberOfPerigeeParameters() == -1) {
      trajectory.setNumberOfPerigeeParameters(5);
    }

    // if (&param!=trajectory.referenceParameters())
    if (per) {
      trajectory.setReferenceParameters(per);
    }


    int nfitpar = trajectory.numberOfFitParameters();
    int nperpars = trajectory.numberOfPerigeeParameters();
    int nscat = trajectory.numberOfScatterers();
    int nbrem = trajectory.numberOfBrems();

    // save ROOT error message level
    int originalErrorLevel = gErrorIgnoreLevel;
    gErrorIgnoreLevel = 10000;

    m_a.ResizeTo(nfitpar, nfitpar);
    TDecompChol lu;
    TVectorD b(nfitpar);

    if (trajectory.trackStates().size() > m_derivpool.size()) {
      int diff = (int) trajectory.trackStates().size() - (int) m_derivpool.size();
      m_derivpool.reserve(trajectory.trackStates().size() + 50);
      for (int i = 0; i < diff + 50; i++) {
        m_derivpool.push_back(*m_derivpool.begin());
      }
    }
    if (nfitpar > m_derivpool.begin()->num_col()) {
      for (int i = 0; i < (int) m_derivpool.size(); i++) {
        m_derivpool[i] = HepMatrix(5, nfitpar + 50, 0);
      }
    }
    for (int i = 0; i < (int) trajectory.trackStates().size(); i++) {
      if (trajectory.trackStates()[i]->materialEffects()) {
        continue;
      }
      for (int j = 0; j < 5; j++) {
        for (int k = 0; k < nfitpar; k++) {
          m_derivpool[i][j][k] = 0;
        }
      }
      trajectory.trackStates()[i]->setDerivatives(m_derivpool[i]);
    }
    bool doderiv = true;
    int it = 0;
    int tmpminiter = m_miniter;
    for (; it < m_maxit; it++) {
      m_lastiter = it;
      if (it >= m_maxit - 1) {
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "Fit did not converge" << endmsg;
        }
        m_fittercode = FitterStatusCode::NoConvergence;
        m_notconverge++;
        gErrorIgnoreLevel = originalErrorLevel;
        // cleanup();
        m_miniter = tmpminiter;
        return 0;
      }
      if (!trajectory.converged()) {
        m_fittercode = runIteration(trajectory, it, m_a, b, lu, doderiv);
        if (m_fittercode != FitterStatusCode::Success) {
          if (m_fittercode == FitterStatusCode::ExtrapolationFailure) {
            m_propfailed++;
          }else if (m_fittercode == FitterStatusCode::InvalidAngles) {
            m_invalidangles++;
          } else if (m_fittercode == FitterStatusCode::ExtrapolationFailureDueToSmallMomentum) {
            m_lowmomentum++;
          }
          gErrorIgnoreLevel = originalErrorLevel;
          // cleanup();
          m_miniter = tmpminiter;
          return 0;
        }
        int nhits = trajectory.numberOfHits(), ntrthits = trajectory.numberOfTRTHits(),
            nsihits = trajectory.numberOfSiliconHits();
        double redchi2 = (trajectory.nDOF() > 0) ? trajectory.chi2() / trajectory.nDOF() : 0;
        double prevredchi2 = (trajectory.nDOF() > 0) ? trajectory.prevchi2() / trajectory.nDOF() : 0;

        if (it > 0 && it < 4 &&
            ((redchi2 < prevredchi2 && (redchi2 > prevredchi2 - 1 || redchi2 < 2)) || nsihits + ntrthits == nhits) &&
            (runOutlier || m_trtrecal) && ntrthits > 0) {
          if (!(it == 1 && nsihits == 0 && trajectory.nDOF() > 0 && trajectory.chi2() / trajectory.nDOF() > 3)) {
            if (msgLvl(MSG::DEBUG)) {
              msg(MSG::DEBUG) << "Running TRT cleaner" << endmsg;
            }
            runTrackCleanerTRT(trajectory, m_a, b, lu, runOutlier, m_trtrecal, it);
            if (m_fittercode != FitterStatusCode::Success) {
              if (msgLvl(MSG::DEBUG)) {
                msg(MSG::DEBUG) << "TRT cleaner failed, returning null..." << endmsg;
              }
              gErrorIgnoreLevel = originalErrorLevel;
              // cleanup();
              m_miniter = tmpminiter;
              return 0;
            }
          }
          // if (!m_trtrecal) it=-1;
        }
        if (!trajectory.converged()) {
          m_fittercode = updateFitParameters(trajectory, b, lu);
          if (m_fittercode != FitterStatusCode::Success) {
            if (m_fittercode == FitterStatusCode::InvalidAngles) {
              m_invalidangles++;
            }
            // cleanup();
            m_miniter = tmpminiter;
            return 0;
          }
        }
      }else {
        break;
      }
    }

    m_miniter = tmpminiter;

    if (!trajectory.prefit()) {
      m_ainv.ResizeTo(m_a.GetNcols(), m_a.GetNcols());
      double *myarray = m_a.GetMatrixArray();
      double *myarrayinv = m_ainv.GetMatrixArray();
      AlSymMat weight(m_a.GetNcols());
      for (int i = 0; i < m_a.GetNcols(); ++i) {
        for (int j = 0; j <= i; ++j) {
          weight[i][j] = myarray[i * m_a.GetNcols() + j];
        }
      }
      int ok = weight.invert();
      for (int i = 0; i < m_a.GetNcols(); ++i) {
        for (int j = 0; j <= i; ++j) {
          myarrayinv[i * m_a.GetNcols() + j] = myarrayinv[j * m_a.GetNcols() + i] = weight[i][j];
        }
      }
      // bool ok=lu.Invert(ainv);
      if (ok) {
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "matrix inversion failed!" << endmsg;
        }
        m_matrixinvfailed++;
        m_fittercode = FitterStatusCode::MatrixInversionFailure;
        gErrorIgnoreLevel = originalErrorLevel;
        // cleanup();
        return 0;
      }
    }
    GXFTrajectory *finaltrajectory = &trajectory;
    if ((runOutlier || m_sirecal) && trajectory.numberOfSiliconHits() == trajectory.numberOfHits()) {
      calculateTrackErrors(trajectory, m_ainv, true);
      finaltrajectory = runTrackCleanerSilicon(trajectory, m_a, m_ainv, b, runOutlier);
    }
    // if (trajectory.numberOfSiliconHits()+trajectory.numberOfTRTHits()!=trajectory.numberOfHits() && !m_straightline
    // && !trajectory.prefit()) runTrackCleanerMDT(trajectory,a,ainv,b,lu);

    // We're done with the ROOT stuff, so we can reset the error level
    gErrorIgnoreLevel = originalErrorLevel;

    if (m_fittercode != FitterStatusCode::Success) {
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "Silicon cleaner failed, returning null..." << endmsg;
      }
      if (finaltrajectory != &trajectory) {
        delete finaltrajectory;
      }
      // cleanup();
      return 0;
    }
    if (m_domeastrackpar && !finaltrajectory->prefit()) {
      calculateTrackErrors(*finaltrajectory, m_ainv, false);
    }
    if (!m_acceleration && !finaltrajectory->prefit()) {
      if (nperpars == 5) {
        for (int i = 0; i < m_a.GetNcols(); i++) {
          m_ainv[4][i] *= .001;
          m_ainv[i][4] *= .001;
        }
        // ainv[4][4]*=.001;
      }
      for (int bremno = 0; bremno < nbrem; bremno++) {
        for (int i = 0; i < m_a.GetNcols(); i++) {
          m_ainv[nperpars + 2 * nscat + bremno][i] *= .001;
          m_ainv[i][nperpars + 2 * nscat + bremno] *= .001;
        }
        // ainv[nperpars+2*nscat+bremno][nperpars+2*nscat+bremno]*=.001;
      }

      AmgSymMatrix(5) * errmat = new AmgSymMatrix(5);
      errmat->setZero();
      int nperparams = finaltrajectory->numberOfPerigeeParameters();
      for (int i = 0; i < nperparams; i++) {
        for (int j = 0; j < nperparams; j++) {
          (*errmat)(j, i) = m_ainv[j][i];
        }
      }
      if (m_straightline) {
        (*errmat)(4, 4) = 1e-20;
      }


      const AmgVector(5) &perpars = finaltrajectory->referenceParameters()->parameters();
      const TrackParameters *measper =
        finaltrajectory->referenceParameters()->associatedSurface().createTrackParameters(perpars[0], perpars[1],
                                                                                          perpars[2], perpars[3],
                                                                                          perpars[4], errmat);
      finaltrajectory->setReferenceParameters(measper);
      if (m_fillderivmatrix) {
        delete m_fullcovmat;
        m_fullcovmat = new Amg::MatrixX(m_ainv.GetNcols(), m_ainv.GetNcols());
        m_fullcovmat->setZero();
        for (int col_i = 0; col_i < m_ainv.GetNcols(); col_i++) {
          for (int row_j = col_i; row_j < m_ainv.GetNcols(); row_j++) {
            // Coverity does not like the parameters to be called j and i
            // because in the method declaration the parameters are called i and j.
            // Coverity assumes the arguments to be swapped ...
            m_fullcovmat->fillSymmetric(row_j, col_i, m_ainv[row_j][col_i]);
          }
        }
      }
    }
    Track *track = 0;
    if (finaltrajectory->prefit() > 0) {
      if (finaltrajectory != &trajectory) {
        delete finaltrajectory;
      }
      // cleanup();
      return 0;
    }
    if (finaltrajectory->numberOfOutliers() <= m_maxoutliers || !runOutlier) {
      track = makeTrack(*finaltrajectory, matEffects);
    } else {
      m_notenoughmeas++;
      m_fittercode = FitterStatusCode::OutlierLogicFailure;
    }
    double cut = (finaltrajectory->numberOfSiliconHits() == finaltrajectory->numberOfHits()) ? 999 : m_chi2cut;
    if (runOutlier && track &&
        (track->fitQuality()->numberDoF() != 0 &&
         track->fitQuality()->chiSquared() / track->fitQuality()->numberDoF() > cut)) {
      delete track;
      track = 0;
      m_highchi2++;
    }
    if (!track) {
      ATH_MSG_DEBUG("Track rejected");
    }
    if (finaltrajectory != &trajectory) {
      delete finaltrajectory;
    }
    // cleanup();
    return track;
  }

  void
  GlobalChi2Fitter::fillResiduals(GXFTrajectory &trajectory, int it, TMatrixDSym &a, TVectorD &b, TDecompChol &lu,
                                  bool &doderiv) const {
    ATH_MSG_DEBUG("fillResiduals");
    std::vector<GXFTrackState *> &states = trajectory.trackStates();
    double chi2 = 0;

    int scatno = 0, bremno = 0, measno = 0;

    int nbrem = trajectory.numberOfBrems();
    // int nbremupstream=trajectory.numberOfUpstreamBrems();
    // int nscat=trajectory.numberOfScatterers();

    int nperpars = trajectory.numberOfPerigeeParameters();
    int nfitpars = trajectory.numberOfFitParameters();

    std::vector<double> &res = trajectory.residuals();
    std::vector<std::vector<double> > &weightderiv = trajectory.weightedResidualDerivatives();
    int nidhits = trajectory.numberOfSiliconHits() + trajectory.numberOfTRTHits();
    int nsihits = trajectory.numberOfSiliconHits();
    int ntrthits = trajectory.numberOfTRTHits();
    int nhits = trajectory.numberOfHits();
    int nmeas = (int) res.size();
    std::vector<double> &error = trajectory.errors();
    ParamDefsAccessor paraccessor;
    bool scatwasupdated = false;

    GXFTrackState *state_maxbrempull = 0;
    int bremno_maxbrempull = 0;
    double maxbrempull = 0;
    /* TMatrixD ainv;
       double *myarray=0;
       if (m_asymeloss && trajectory.mass()<100){
       ainv.ResizeTo(a.GetNcols(),a.GetNcols());
       bool ok=lu.Invert(ainv);
       myarray=ainv.GetMatrixArray();
       }
     */
    for (int hitno = 0; hitno < (int) states.size(); hitno++) {
      GXFTrackState *state = states[hitno];
      const TrackParameters *currenttrackpar = state->trackParameters();
      TrackState::TrackStateType statetype = state->trackStateType();
      TrackState::MeasurementType hittype = state->measurementType();
      const MeasurementBase *measbase = state->measurement();

      if (statetype == TrackState::Fittable) {
        if (hittype == TrackState::Pseudo && it <= 100 && it > 1 && trajectory.nDOF() != 0 &&
            std::abs((trajectory.prevchi2() - trajectory.chi2()) / trajectory.nDOF()) < 15
/*&& !trajectory.prefit()*/ && !state->surface()->isFree() && nidhits < trajectory.numberOfHits() &&
            (nperpars == 0 || nidhits > 0) && (!state->isRecalibrated() /*|| !trajectory.prefit()*/)) {
          Amg::MatrixX covMatrix(1, 1);
          covMatrix(0, 0) = 100;

          PseudoMeasurementOnTrack *newpseudo =
            new PseudoMeasurementOnTrack(LocalParameters(DefinedParameter(
                                                           currenttrackpar->parameters()[Trk::locY],
                                                           Trk::locY)),
                                         covMatrix,
                                         currenttrackpar->associatedSurface());
          state->setMeasurement(newpseudo);
          measbase = newpseudo;
        }
        double *errors = state->measurementErrors();
        m_residualPullCalculator->residuals(m_residuals, measbase, currenttrackpar, ResidualPull::Biased, hittype);
        for (int i = 0; i < 5; i++) {
          if (!measbase->localParameters().contains(paraccessor.pardef[i]) ||
              (i > 0 && (hittype == TrackState::SCT || hittype == TrackState::TGC))) {
            continue;
          }
          error[measno] =
            (trajectory.prefit() > 0 &&
             (hittype == TrackState::MDT || (hittype == TrackState::CSC && !state->measuresPhi()))) ? 2 : errors[i];

          res[measno] = m_residuals[i];
          if (i == 2 && std::abs(std::abs(res[measno]) - 2 * M_PI) < std::abs(res[measno])) {
            if (res[measno] < 0) {
              res[measno] += 2 * M_PI;
            } else {
              res[measno] -= 2 * M_PI;
            }
          }
          measno++;
        }
      }else if (statetype == TrackState::GeneralOutlier) {
        double *errors = state->measurementErrors();
        for (int i = 0; i < 5; i++) {
          if (errors[i] > 0) {
            error[measno] = errors[i];
            measno++;
          }
        }
      }
      if (statetype == TrackState::Scatterer && (!trajectory.prefit() || state->materialEffects()->deltaE() == 0)) {
        double deltaphi = state->materialEffects()->deltaPhi();
        double measdeltaphi = state->materialEffects()->measuredDeltaPhi();
        double sigmadeltaphi = state->materialEffects()->sigmaDeltaPhi();
        double deltatheta = state->materialEffects()->deltaTheta();
        double sigmadeltatheta = state->materialEffects()->sigmaDeltaTheta();
        // if (trajectory.prefit()==1) sigmadeltatheta*=5;
        if (m_updatescat && (sigmadeltaphi >= 0.001 || (sigmadeltaphi >= 0.0001 && !trajectory.prefit())) &&
            (state->materialEffects()->deltaE() == 0 || !m_msEntrance->inside(state->trackParameters()->position())) &&
            it >= 2) {
          state->materialEffects()->setMeasuredDeltaPhi(deltaphi);
          measdeltaphi = state->materialEffects()->measuredDeltaPhi();
          double newsigmadeltaphi = 0.001;
          if (trajectory.prefit() == 0) {
            newsigmadeltaphi = 0.0001;
          }
          if (newsigmadeltaphi < sigmadeltaphi) {
            a[nperpars + 2 * scatno][nperpars + 2 * scatno] += 1 / (newsigmadeltaphi * newsigmadeltaphi) - 1 /
                                                               (sigmadeltaphi * sigmadeltaphi);
            state->materialEffects()->setScatteringSigmas(newsigmadeltaphi, sigmadeltatheta);
            trajectory.scatteringSigmas()[scatno] = std::make_pair(newsigmadeltaphi, sigmadeltatheta);
            scatwasupdated = true;
          }
        }
        if (trajectory.prefit() != 1) {
          b[nperpars + 2 * scatno] -= (deltaphi - measdeltaphi) / (sigmadeltaphi * sigmadeltaphi);
          b[nperpars + 2 * scatno + 1] -= deltatheta / (sigmadeltatheta * sigmadeltatheta);
        }else {
          b[nperpars + scatno] -= deltatheta / (sigmadeltatheta * sigmadeltatheta);
        }
        // sigmadeltaphi=sigmadeltatheta/currenttrackpar->sinTheta();
        chi2 += deltaphi * deltaphi / (sigmadeltaphi * sigmadeltaphi) + deltatheta * deltatheta /
                (sigmadeltatheta * sigmadeltatheta);

        scatno++;
      }

      if (state->materialEffects() && state->materialEffects()->sigmaDeltaE() > 0) {
        double averagenergyloss = std::abs(state->materialEffects()->deltaE());

        double qoverpbrem = 1000 * states[hitno]->trackParameters()->parameters()[Trk::qOverP];
        double pbrem = 1 / std::abs(qoverpbrem);
        double qoverp = qoverpbrem - state->materialEffects()->delta_p();
        // std::cout << "qoverp: " << qoverp << " qoverpbrem: " << qoverpbrem << std::endl;
        double p = 1 / std::abs(qoverp);
        // double
        // p=0.001/std::abs((states[hitno]->trackParameters()->parameters()[Trk::qOverP]-0.001*state->materialEffects()->delta_p()));
        // double signbeforebrem= (qoverp>0) ? 1 : -1;
        /* double sign=1,sign2=1;
           if (qoverpbrem*qoverp<0) {
           if (bremno<nbremupstream) sign=-1;
           else sign2=-1;
           } */
        // double signafterbrem= (p>0) ? 1 : -1;
        double mass = .001 * trajectory.mass();
        // res[nmeas-nbrem+bremno]=.001*averagenergyloss-sqrt(1./(qoverp*qoverp)+mass*mass)+signafterbrem*sqrt(1./(qoverpbrem*qoverpbrem)+mass*mass);
        res[nmeas - nbrem + bremno] = .001 * averagenergyloss - std::sqrt(p * p + mass * mass) + std::sqrt(
          pbrem * pbrem + mass * mass);
        double sigde = state->materialEffects()->sigmaDeltaE();
        // double sigdeave=state->materialEffects()->sigmaDeltaEAve();
        double sigdepos = state->materialEffects()->sigmaDeltaEPos();
        double sigdeneg = state->materialEffects()->sigmaDeltaENeg();
        error[nmeas - nbrem + bremno] = .001 * state->materialEffects()->sigmaDeltaE();
        if (state->materialEffects()->isKink()) {
          maxbrempull = -999999999;
          state_maxbrempull = 0;
        }
        if (m_asymeloss && it > 0 && !trajectory.prefit() && sigde > 0 && sigde != sigdepos && sigde != sigdeneg) {
          double elosspull = res[nmeas - nbrem + bremno] / (.001 * sigde);

          if (trajectory.mass() > 100) {
            if (elosspull < -1) {
              state->materialEffects()->setSigmaDeltaE(sigdepos);
            } else if (elosspull > 1) {
              state->materialEffects()->setSigmaDeltaE(sigdeneg);
            }
            error[nmeas - nbrem + bremno] = .001 * state->materialEffects()->sigmaDeltaE();
          }else if (!trajectory.numberOfTRTHits() || it >= 3) {
// if (bremno==4) elosspull=-999;
// double sigdp=std::abs(((bremno>=nbremupstream) ? pbrem : p)-
// 1/(sqrt(myarray[(nfitpars-nbrem+bremno)*nfitpars+nfitpars-nbrem+bremno])+ ((bremno>=nbremupstream) ? 1/pbrem :
// 1/p)));
// elosspull=0;
// if (sigdp<.001*sigde) elosspull=res[nmeas-nbrem+bremno]/sqrt(1e-6*sigde*sigde-sigdp*sigdp);
// std::cout << "sigdqp: " << sqrt(myarray[(nfitpars-nbrem+bremno)*nfitpars+nfitpars-nbrem+bremno]) << " sigdp: " <<
// sigdp << " elosspull: " << elosspull << std::endl;
            if (!state->materialEffects()->isKink() &&
                ((elosspull < -.2 && m_fixbrem == -1 && elosspull < maxbrempull) ||
                 (m_fixbrem >= 0 && bremno == m_fixbrem))) {
              bremno_maxbrempull = bremno;
              state_maxbrempull = state;
              maxbrempull = elosspull;
            }
          }
          // else state->materialEffects()->setSigmaDeltaE(sigdeave);
        }
        if (it > 0 && hitno >= 2 && !m_calotoolparam.empty() && !trajectory.prefit() &&
            state->materialEffects()->sigmaDeltaPhi() == 0 && state->materialEffects()->isMeasuredEloss() &&
            res[nmeas - nbrem + bremno] / (.001 * state->materialEffects()->sigmaDeltaEAve()) > 2.5) {
          const TrackParameters *parforcalo =
            trajectory.prefit() ? trajectory.referenceParameters() : states[hitno - 2]->trackParameters();
          const IPropagator *prop = &*m_propagator;
          std::vector<MaterialEffectsOnTrack> calomeots = m_calotoolparam->extrapolationSurfacesAndEffects(
            *m_navigator->highestVolume(), *prop, *parforcalo,
            parforcalo->associatedSurface(), Trk::anyDirection, Trk::muon);
          if (calomeots.size() == 3) {
            averagenergyloss = std::abs(calomeots[1].energyLoss()->deltaE());
            double newres = .001 * averagenergyloss - sqrt(p * p + mass * mass) + sqrt(pbrem * pbrem + mass * mass);
            double newerr = .001 * calomeots[1].energyLoss()->sigmaDeltaE();
            if (std::abs(newres / newerr) < std::abs(res[nmeas - nbrem + bremno] / error[nmeas - nbrem + bremno])) {
              if (msgLvl(MSG::DEBUG)) {
                msg(MSG::DEBUG) << "Changing from measured to parametrized energy loss" << endmsg;
              }
              state->materialEffects()->setEloss(calomeots[1].energyLoss()->clone());
              state->materialEffects()->setSigmaDeltaE(calomeots[1].energyLoss()->sigmaDeltaE());
              res[nmeas - nbrem + bremno] = newres;
              error[nmeas - nbrem + bremno] = newerr;
            }
          }
          state->materialEffects()->setMeasuredEloss(false);
        }
        bremno++;
      }
    }
    measno = 0;
    for (; measno < nmeas; measno++) {
      if (error[measno] == 0) {
        continue;
      }
      chi2 += res[measno] * (1. / (error[measno] * error[measno])) * res[measno];

      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "res[" << measno << "]: " << res[measno] << " error[" << measno << "]: " <<
          error[measno] << " res/err: " << res[measno] / error[measno] << endmsg;
      }
    }
    if (!doderiv && (scatwasupdated /* || nbrem>0*/)) {
      lu.SetMatrix(a);
    }
    /* if (trajectory.nDOF()>0 && chi2/trajectory.nDOF()>1.e4){
       for (int i=0;i<nmeas-nbrem;i++){
        if (error[measno]<2) error[measno]=2;
       }
       }  */

    double oldchi2 = trajectory.chi2();
    trajectory.setPrevChi2(oldchi2);
    trajectory.setChi2(chi2);
    double oldredchi2 = (trajectory.nDOF() > 0)  ? oldchi2 / trajectory.nDOF() : 0;
    double newredchi2 = (trajectory.nDOF() > 0)  ? chi2 / trajectory.nDOF() : 0;
    if (trajectory.prefit() > 0 &&
        ((newredchi2 < 2 && it != 0) ||
         (newredchi2 < oldredchi2 + .1 && fabs(newredchi2 - oldredchi2) < 1 && it != 1))) {
      trajectory.setConverged(true);
    }

    double maxdiff = (nsihits != 0 && nsihits + ntrthits == nhits && chi2 < oldchi2) ? 200 : 1.;
    maxdiff = 1;
    int miniter = (nsihits != 0 && nsihits + ntrthits == nhits) ? 1 : 2;
    if (miniter < m_miniter) {
      miniter = m_miniter;
    }
    if (it >= miniter && fabs(oldchi2 - chi2) < maxdiff) {
      trajectory.setConverged(true);
    }

    if (state_maxbrempull && trajectory.converged()) {
      state_maxbrempull->materialEffects()->setSigmaDeltaE(10 * state_maxbrempull->materialEffects()->sigmaDeltaEPos());
      state_maxbrempull->materialEffects()->setKink(true);
      trajectory.setConverged(false);
      // doderiv=true;
      double olderror = error[nmeas - nbrem + bremno_maxbrempull];
      double newerror = .001 * state_maxbrempull->materialEffects()->sigmaDeltaE();
      // std::cout << "old error: " << olderror << " new error: " << newerror << std::endl;
      error[nmeas - nbrem + bremno_maxbrempull] = .001 * state_maxbrempull->materialEffects()->sigmaDeltaE();
      double *myarray2 = a.GetMatrixArray();
      for (int i = 0; i < nfitpars; i++) {
        if (weightderiv[nmeas - nbrem + bremno_maxbrempull][i] == 0) {
          continue;
        }

        for (int j = i; j < nfitpars; j++) {
          myarray2[i * nfitpars + j] -= weightderiv[nmeas - nbrem + bremno_maxbrempull][i] *
                                        weightderiv[nmeas - nbrem + bremno_maxbrempull][j] *
                                        (1 - olderror * olderror / (newerror * newerror));
          if (i != j) {
            myarray2[j * nfitpars + i] -= weightderiv[nmeas - nbrem + bremno_maxbrempull][i] *
                                          weightderiv[nmeas - nbrem + bremno_maxbrempull][j] *
                                          (1 - olderror * olderror / (newerror * newerror));
          }
        }
        weightderiv[nmeas - nbrem + bremno_maxbrempull][i] *= olderror / newerror;
      }
      lu.SetMatrix(a);
      trajectory.setChi2(1e15);
      doderiv = true;
    }
  }

  void
  GlobalChi2Fitter::fillDerivatives(GXFTrajectory &trajectory, bool onlybrem) const {
    ATH_MSG_DEBUG("fillDerivatives");

    std::vector<GXFTrackState *> &states = trajectory.trackStates();
    int scatno = 0, bremno = 0, measno = 0;
    int nscatupstream = trajectory.numberOfUpstreamScatterers();
    int nbremupstream = trajectory.numberOfUpstreamBrems();
    int nscat = trajectory.numberOfScatterers();
    int nbrem = trajectory.numberOfBrems();
    int nperparams = trajectory.numberOfPerigeeParameters();
    // int nfitpar=trajectory.numberOfFitParameters();
    std::vector<std::vector<double> > &weightderiv = trajectory.weightedResidualDerivatives();
    std::vector<double> &error = trajectory.errors();

    int nmeas = (int) weightderiv.size();
    // std::cout << "nbrem: " << trajectory.numberOfBrems() << " nfitpar: " << trajectory.numberOfFitParameters() << "
    // nmeas: " << nmeas << " nfitpar2: " << weightderiv[0].size() << std::endl;


    ParamDefsAccessor paraccessor;
    for (int hitno = 0; hitno < (int) states.size(); hitno++) {
      GXFTrackState *state = states[hitno];
      if (onlybrem && (!state->materialEffects() || state->materialEffects()->sigmaDeltaE() <= 0)) {
        continue;
      }

      TrackState::TrackStateType statetype = state->trackStateType();
      TrackState::MeasurementType hittype = state->measurementType();
      const MeasurementBase *measbase = state->measurement();

      int scatmin = (scatno < nscatupstream) ? scatno : nscatupstream;
      int scatmax = (scatno < nscatupstream) ? nscatupstream : scatno;
      int bremmin = (bremno < nbremupstream) ? bremno : nbremupstream;
      int bremmax = (bremno < nbremupstream) ? nbremupstream : bremno;
      HepMatrix &derivatives = state->derivatives();

      if (statetype == TrackState::Fittable) {
        double sinstereo = 0;
        if (hittype == TrackState::SCT || hittype == TrackState::TGC) {
          sinstereo = state->sinStereo();
        }
        double cosstereo = (sinstereo == 0) ? 1. : sqrt(1 - sinstereo * sinstereo);
        for (int i = 0; i < 5; i++) {
          if (!measbase->localParameters().contains(paraccessor.pardef[i]) ||
              (i > 0 && (hittype == TrackState::SCT || hittype == TrackState::TGC))) {
            continue;
          }
          if (trajectory.numberOfPerigeeParameters() > 0) {
            if (i == 0 && sinstereo != 0) {
              weightderiv[measno][0] = (derivatives[0][0] * cosstereo + sinstereo * derivatives[1][0]) / error[measno];
              weightderiv[measno][1] = (derivatives[0][1] * cosstereo + sinstereo * derivatives[1][1]) / error[measno];
              weightderiv[measno][2] = (derivatives[0][2] * cosstereo + sinstereo * derivatives[1][2]) / error[measno];
              weightderiv[measno][3] = (derivatives[0][3] * cosstereo + sinstereo * derivatives[1][3]) / error[measno];
              if (!m_straightline) {
                weightderiv[measno][4] = (derivatives[0][4] * cosstereo + sinstereo * derivatives[1][4]) /
                                         error[measno];
              }
            }else {
              weightderiv[measno][0] = derivatives[i][0] / error[measno];
              weightderiv[measno][1] = derivatives[i][1] / error[measno];
              weightderiv[measno][2] = derivatives[i][2] / error[measno];
              weightderiv[measno][3] = derivatives[i][3] / error[measno];
              if (!m_straightline) {
                weightderiv[measno][4] = derivatives[i][4] / error[measno];
              }
            }
          }
          for (int j = scatmin; j < scatmax; j++) {
            int index = nperparams + ((trajectory.prefit() != 1) ? 2 * j : j);
            double thisderiv = 0;
            double sign = (j < nscatupstream) ? -1 : 1;
            sign = 1;
            if (i == 0 && sinstereo != 0) {
              thisderiv = sign * (derivatives[0][index] * cosstereo + sinstereo * derivatives[1][index]);
            } else {
              thisderiv = sign * derivatives[i][index];
            }
            weightderiv[measno][index] = thisderiv / error[measno];
            if (trajectory.prefit() != 1) {
              index++;
              if (i == 0 && sinstereo != 0) {
                thisderiv = sign * (derivatives[0][index] * cosstereo + sinstereo * derivatives[1][index]);
              } else {
                thisderiv = sign * derivatives[i][index];
              }
              weightderiv[measno][index] = thisderiv / error[measno];
            }
          }
          for (int j = bremmin; j < bremmax; j++) {
            double thisderiv = 0;
            int index = j + nperparams + 2 * nscat;
            if (i == 0 && sinstereo != 0) {
              thisderiv = derivatives[0][index] * cosstereo + sinstereo * derivatives[1][index];
            } else {
              thisderiv = derivatives[i][index];
            }
            weightderiv[measno][index] = thisderiv / error[measno];
          }
          measno++;
        }
      }else if (statetype == TrackState::GeneralOutlier) {
        double *errors = state->measurementErrors();
        for (int i = 0; i < 5; i++) {
          if (errors[i] > 0) {
            measno++;
          }
        }
      }else if (statetype == TrackState::Scatterer &&
                (!trajectory.prefit() || state->materialEffects()->deltaE() == 0)) {
        scatno++;
      }

      if (state->materialEffects() && state->materialEffects()->sigmaDeltaE() > 0) {
        double qoverp = 0, qoverpbrem = 0;
        // double pbrem=0.001/std::abs(states[hitno]->trackParameters()->parameters()[Trk::qOverP]);
        // double p=pbrem-state->materialEffects()->delta_p();
        qoverpbrem = 1000 * states[hitno]->trackParameters()->parameters()[Trk::qOverP];

        qoverp = qoverpbrem - state->materialEffects()->delta_p();
        double sign = (qoverp > 0) ? 1 : -1;
        // double p=std::abs(1/qoverp);
        // qoverp=sign/p;
        // sign=1;
        double sign2 = (qoverpbrem > 0) ? 1 : -1;
        if (qoverp * qoverpbrem < 0) {
          // if (bremno<nbremupstream) sign=-1;
          // else sign2=-1;
        }
        double mass = .001 * trajectory.mass();
        // std::vector<double> &pjac=state->materialEffects()->momentumJacobians();
        if (trajectory.numberOfPerigeeParameters() > 0) {
          weightderiv[nmeas - nbrem +
                      bremno][4] =
            /*pjac[0]* */ (-sign / (qoverp * qoverp * sqrt(1 + mass * mass * qoverp * qoverp)) + sign2 /
                           (qoverpbrem * qoverpbrem * sqrt(1 + mass * mass * qoverpbrem * qoverpbrem))) /
            error[nmeas - nbrem + bremno];
        }
        if (bremno < nbremupstream) {
//        weightderiv[nmeas-nbrem+bremno][nperparams+2*nscat+bremno]=(-p/sqrt(p*p+mass*mass))/error[nmeas-nbrem+bremno];
// for (int bremno2=bremno+1;bremno2<nbremupstream;bremno2++) {

// weightderiv[nmeas-nbrem+bremno][nperparams+2*nscat+bremno2]=pjac[bremno2+1]*(-p/sqrt(p*p+mass*mass)+pbrem/sqrt(pbrem*pbrem+mass*mass))/error[nmeas-nbrem+bremno];
//      }

          weightderiv[nmeas - nbrem + bremno][nperparams + 2 * nscat +
                                              bremno] =
            (sign / (qoverp * qoverp * sqrt(1 + mass * mass * qoverp * qoverp))) /
            error[nmeas - nbrem + bremno];
          for (int bremno2 = bremno + 1; bremno2 < nbremupstream; bremno2++) {
            weightderiv[nmeas - nbrem + bremno][nperparams + 2 * nscat +
                                                bremno2] =
              -(-sign / (qoverp * qoverp * sqrt(1 + mass * mass * qoverp * qoverp)) + sign2 /
                (qoverpbrem * qoverpbrem * sqrt(1 + mass * mass * qoverpbrem * qoverpbrem))) /
              error[nmeas - nbrem + bremno];
          }
        }else {
// weightderiv[nmeas-nbrem+bremno][nperparams+2*nscat+bremno]=(-pbrem/sqrt(pbrem*pbrem+mass*mass))/error[nmeas-nbrem+bremno];
// for (int bremno2=bremno+1;bremno2<nbremupstream;bremno2++) {

// weightderiv[nmeas-nbrem+bremno][nperparams+2*nscat+bremno2]=pjac[bremno2+1]*(p/sqrt(p*p+mass*mass)-pbrem/sqrt(pbrem*pbrem+mass*mass))/error[nmeas-nbrem+bremno];
//      }

          weightderiv[nmeas - nbrem + bremno][nperparams + 2 * nscat +
                                              bremno] =
            (sign2 / (qoverpbrem * qoverpbrem * sqrt(1 + mass * mass * qoverpbrem * qoverpbrem))) /
            error[nmeas - nbrem + bremno];
          for (int bremno2 = nbremupstream; bremno2 < bremno; bremno2++) {
            weightderiv[nmeas - nbrem + bremno][nperparams + 2 * nscat +
                                                bremno2] =
              (-sign / (qoverp * qoverp * sqrt(1 + mass * mass * qoverp * qoverp)) + sign2 /
               (qoverpbrem * qoverpbrem * sqrt(1 + mass * mass * qoverpbrem * qoverpbrem))) /
              error[nmeas - nbrem + bremno];
          }
        }
        bremno++;
      }
    }
  }

  FitterStatusCode
  GlobalChi2Fitter::runIteration(GXFTrajectory &trajectory, int it, TMatrixDSym &a, TVectorD &b, TDecompChol &lu,
                                 bool &doderiv) const {
    int measno = 0;
    int nfitpars = trajectory.numberOfFitParameters();
    int nperpars = trajectory.numberOfPerigeeParameters();
    // int upstreamscatpars= 2*trajectory.numberOfUpstreamScatterers();
    // int upstreambrempars= trajectory.numberOfUpstreamBrems();
    int scatpars = 2 * trajectory.numberOfScatterers();
    int nupstreamstates = trajectory.numberOfUpstreamStates();
    int nbrem = trajectory.numberOfBrems();
    // int nscat=trajectory.numberOfScatterers();
    double oldchi2 = trajectory.chi2();
    double oldredchi2 = (trajectory.nDOF() > 0)  ? oldchi2 / trajectory.nDOF() : 0;
    int nsihits = trajectory.numberOfSiliconHits(), ntrthits = trajectory.numberOfTRTHits(),
        nhits = trajectory.numberOfHits();

    if (m_phiweight.empty()) {
      // m_phiweight.resize(trajectory.numberOfScatterers());
      m_phiweight.assign(trajectory.numberOfScatterers(), 1);
    }
    // bool iscombined= (nsihits+ntrthits>0 && nsihits+ntrthits!=nhits);
    FitterStatusCode fsc = calculateTrackParameters(trajectory, doderiv);
    if (fsc != FitterStatusCode::Success) {
      return fsc;
    }
    b.Zero();
    fillResiduals(trajectory, it, a, b, lu, doderiv);

    double newredchi2 = (trajectory.nDOF() > 0)  ? trajectory.chi2() / trajectory.nDOF() : 0;

    if (msgLvl(MSG::DEBUG)) {
      print(trajectory);

      std::cout << "old chi2: " << oldchi2 << "/" << trajectory.nDOF() << "=" << oldredchi2 << " new chi2: " <<
        trajectory.chi2() << "/" << trajectory.nDOF() << "=" << newredchi2 << std::endl;
    }

    if (trajectory.prefit() > 0 && trajectory.converged()) {
      return FitterStatusCode::Success;
    }


    std::vector<double> &res = trajectory.residuals();
    std::vector<double> &error = trajectory.errors();
    std::vector<std::pair<double, double> > &scatsigmas = trajectory.scatteringSigmas();

    int nmeas = (int) res.size();

    std::vector<std::vector<double> > &weightderiv = trajectory.weightedResidualDerivatives();


    if (doderiv) {
      calculateDerivatives(trajectory);
      fillDerivatives(trajectory, !doderiv);
    }

    if (m_firstmeasurement.empty()) {
      m_firstmeasurement.resize(nfitpars);
      m_lastmeasurement.resize(nfitpars);
      for (int i = 0; i < nperpars; i++) {
        m_firstmeasurement[i] = 0;
        m_lastmeasurement[i] = nmeas - nbrem;
      }
      measno = 0;
      int scatno = 0, bremno = 0;
      for (int i = 0; i < (int) trajectory.trackStates().size(); i++) {
        GXFTrackState *state = trajectory.trackStates()[i];
        GXFMaterialEffects *meff = state->materialEffects();
        if (!meff) {
          measno += state->numberOfMeasuredParameters();
        }
        if (meff) {
          if (meff->sigmaDeltaTheta() != 0 && (!trajectory.prefit() || meff->deltaE() == 0)) {
            if (i < nupstreamstates) {
              m_lastmeasurement[nperpars + 2 * scatno] = m_lastmeasurement[nperpars + 2 * scatno + 1] = measno;
              m_firstmeasurement[nperpars + 2 * scatno] = m_firstmeasurement[nperpars + 2 * scatno + 1] = 0;
            }else {
              m_lastmeasurement[nperpars + 2 * scatno] = m_lastmeasurement[nperpars + 2 * scatno + 1] = nmeas - nbrem;
              m_firstmeasurement[nperpars + 2 * scatno] = m_firstmeasurement[nperpars + 2 * scatno + 1] = measno;
            }
            scatno++;
          }
          if (meff->sigmaDeltaE() > 0) {
            if (i < nupstreamstates) {
              m_firstmeasurement[nperpars + scatpars + bremno] = 0;
              m_lastmeasurement[nperpars + scatpars + bremno] = measno;
            }else {
              m_firstmeasurement[nperpars + scatpars + bremno] = measno;
              m_lastmeasurement[nperpars + scatpars + bremno] = nmeas - nbrem;
            }

            bremno++;
          }
        }
      }
    }

    double *myarray = doderiv ? a.GetMatrixArray() : 0;

    for (int k = 0; k < nfitpars; k++) {
      int minmeas = 0, maxmeas = nmeas - nbrem;
      maxmeas = m_lastmeasurement[k];
      minmeas = m_firstmeasurement[k];

      for (measno = minmeas; measno < maxmeas; measno++) {
        double tmp = res[measno] * (1. / error[measno]) * weightderiv[measno][k];
        b[k] += tmp;
      }

      if (k == 4 || k >= nperpars + scatpars) {
        for (measno = nmeas - nbrem; measno < nmeas; measno++) {
          b[k] += res[measno] * (1. / error[measno]) * weightderiv[measno][k];
        }
      }
      if (doderiv) {
        for (int l = k; l < nfitpars; l++) {
          maxmeas = std::min(m_lastmeasurement[k], m_lastmeasurement[l]);
          minmeas = std::max(m_firstmeasurement[k], m_firstmeasurement[l]);
          double tmp = 0;
          for (measno = minmeas; measno < maxmeas; measno++) {
            tmp += weightderiv[measno][k] * weightderiv[measno][l];
          }
          myarray[l * nfitpars + k] = tmp;
          if (l != k) {
            myarray[k * nfitpars + l] = tmp;
          }
        }
      }
    }
    if (doderiv) {
      /* if (newredchi2>1.e4){
         double scale=1.e4/newredchi2;
         a*=scale;
         } */

      int scatno = 0;
      for (int k = nperpars; k < nperpars + scatpars; k += 2) {
        // std::cout << "sigma phi: " << scatsigmas[scatno].first << " theta: " << scatsigmas[scatno].second <<
        // std::endl;
        myarray[k * nfitpars + k] += 1. / (scatsigmas[scatno].first * scatsigmas[scatno].first);
        myarray[(k + 1) * nfitpars + k + 1] += 1. / (scatsigmas[scatno].second * scatsigmas[scatno].second);
        scatno++;
      }

      for (int measno = nmeas - nbrem; measno < nmeas; measno++) {
        for (int k = 4; k < nfitpars; k++) {
          if (k == 5) {
            k = nperpars + scatpars;
          }
          for (int l = k; l < nfitpars; l++) {
            if (l == 5) {
              l = nperpars + scatpars;
            }
            double tmp = weightderiv[measno][k] * weightderiv[measno][l];
            myarray[l * nfitpars + k] += tmp;
            if (l != k) {
              myarray[k * nfitpars + l] += tmp;
            }
          }
        }
      }
    }
    unsigned int scatno = 0;
    bool weightchanged = false;
    for (int i = 0; i < (int) trajectory.trackStates().size(); i++) {
      GXFTrackState *thisstate = trajectory.trackStates()[i];
      GXFMaterialEffects *meff = thisstate->materialEffects();
      if (meff) {
        const PlaneSurface *plsurf = dynamic_cast<const PlaneSurface *>(thisstate->surface());
        if (meff->deltaE() == 0 || (!trajectory.prefit() && plsurf)) {
          weightchanged = true;
          if (!myarray) {
            myarray = a.GetMatrixArray();
          }

          if (!trajectory.prefit()) {
            // double phipull=std::abs(meff->deltaPhi()/meff->sigmaDeltaPhi());
            if (thisstate->materialEffects()->sigmaDeltaPhi()!=0) {
                if (scatno>=m_phiweight.size()) {
                  std::stringstream message;
                  message << "scatno is out of range " << scatno << " !< " <<  m_phiweight.size();
                  throw std::range_error(message.str());
            }

            if (!doderiv) {
              myarray[(2 * scatno + nperpars) * nfitpars + 2 * scatno + nperpars] /= m_phiweight[scatno];
            }
            if (it == 0) {
              m_phiweight[scatno] = 1.00000001;
            } else if (it == 1) {
              m_phiweight[scatno] = 1.0000001;
            } else if (it <= 3) {
              m_phiweight[scatno] = 1.0001;
            } else if (it <= 6) {
              m_phiweight[scatno] = 1.01;
            } else {
              m_phiweight[scatno] = 1.1;
            }
            myarray[(2 * scatno + nperpars) * nfitpars + 2 * scatno + nperpars] *= m_phiweight[scatno];
           }
          }
          // else if (trajectory.prefit()==1) myarray[(2*scatno+nperpars)*nfitpars+2*scatno+nperpars]*=99;
          else if (trajectory.prefit() >= 2) {
            // if (newredchi2<10 || (newredchi2>oldredchi2-100 && newredchi2<oldredchi2))
            // myarray[(2*scatno+nperpars)*nfitpars+2*scatno+nperpars]*=1.001;
//            else {
            if (newredchi2 > oldredchi2 - 1 && newredchi2 < oldredchi2) {
              myarray[(2 * scatno + nperpars) * nfitpars + 2 * scatno + nperpars] *= 1.0001;
              myarray[(2 * scatno + nperpars + 1) * nfitpars + 2 * scatno + nperpars + 1] *= 1.0001;
            }else if (newredchi2 > oldredchi2 - 25 && newredchi2 < oldredchi2) {
              myarray[(2 * scatno + nperpars) * nfitpars + 2 * scatno + nperpars] *= 1.001;
              myarray[(2 * scatno + nperpars + 1) * nfitpars + 2 * scatno + nperpars + 1] *= 1.0001;
            }else {
              myarray[(2 * scatno + nperpars) * nfitpars + 2 * scatno + nperpars] *= 1.1;
              myarray[(2 * scatno + nperpars + 1) * nfitpars + 2 * scatno + nperpars + 1] *= 1.001;
            }
          }
        }
        if (thisstate->materialEffects()->sigmaDeltaPhi() != 0 &&
            (!trajectory.prefit() || thisstate->materialEffects()->deltaE() == 0)) {
          scatno++;
        }
      }
    }
    if ((/*trajectory.prefit()==1 || */ trajectory.prefit() == 2) && doderiv && trajectory.numberOfBrems() > 0 &&
        (newredchi2 < oldredchi2 - 25 || newredchi2 > oldredchi2)) {
      myarray[24] *= 1.001;
    }
    // else if (trajectory.prefit()==3 && trajectory.numberOfBrems()>0  && newredchi2>100) myarray[24]*=1.1;

    if (doderiv || weightchanged) {
      lu.SetMatrix(a);
    }


    if (m_printderivs) {
      for (measno = 0; measno < nmeas; measno++) {
        for (int k = 0; k < nfitpars; k++) {
          if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << "deriv[" << measno << "][" << k << "]=" << weightderiv[measno][k] * error[measno] <<
              " error: " << error[measno] << endmsg;
          }
        }
      }
    }


    if (trajectory.converged()) {
      if (!trajectory.prefit() && nsihits + ntrthits != nhits) {
        unsigned int scatno = 0;
        double *myarray = a.GetMatrixArray();
        for (int i = 0; i < (int) trajectory.trackStates().size(); i++) {
          GXFTrackState *thisstate = trajectory.trackStates()[i];
          //if (thisstate->materialEffects()) {
          if (thisstate->materialEffects() && thisstate->materialEffects()->sigmaDeltaPhi()!=0){
            if (scatno>=m_phiweight.size()) {
             std::stringstream message;
             message << "scatno is out of range " << scatno << " !< " <<  m_phiweight.size();
             throw std::range_error(message.str());
            }
            const PlaneSurface *plsurf = dynamic_cast<const PlaneSurface *>(thisstate->surface());
            if (thisstate->materialEffects()->deltaE() == 0 || plsurf) {
              myarray[(2 * scatno + nperpars) * nfitpars + 2 * scatno + nperpars] /= m_phiweight[scatno];
              m_phiweight[scatno] = 1;
            }
            if (thisstate->materialEffects()->sigmaDeltaPhi() != 0) {
              scatno++;
            }
          }
        }
        lu.SetMatrix(a);
      }
      return FitterStatusCode::Success;
    }

    if (!m_redoderivs && it < 5 && (newredchi2 < 2 || (newredchi2 < oldredchi2 && newredchi2 > oldredchi2 - .5)) &&
        !trajectory.prefit()) {
      doderiv = false;
    }
    // if (it>0 && newredchi2>oldredchi2+.0001) doderiv=true;

    return FitterStatusCode::Success;
  }

  FitterStatusCode
  GlobalChi2Fitter::updateFitParameters(GXFTrajectory &trajectory, TVectorD &b, TDecompChol &lu) const {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "UpdateFitParameters" << endmsg;
    }

    const TrackParameters *refpar = trajectory.referenceParameters();
    double d0 = refpar->parameters()[Trk::d0];
    double z0 = refpar->parameters()[Trk::z0];
    double phi = refpar->parameters()[Trk::phi0];
    double theta = refpar->parameters()[Trk::theta];
    double qoverp = refpar->parameters()[Trk::qOverP];
    int nscat = trajectory.numberOfScatterers();
    int nbrem = trajectory.numberOfBrems();
    int nperparams = trajectory.numberOfPerigeeParameters();
    // int nfitpar=trajectory.numberOfFitParameters();
    bool ok = false;
    TVectorD result = lu.Solve(b, ok);
    /*if (trajectory.prefit()>=2 && nperparams==0){
       double maxphiscat=std::max(std::abs(result[0]),std::abs(result[2]));
       if (maxphiscat>.02) {
       result[0]*=0.02/maxphiscat;
       result[2]*=0.02/maxphiscat;
       }
       }*/
    if (trajectory.numberOfPerigeeParameters() > 0) {
      d0 += result[0];
      z0 += result[1];
      phi += result[2];
      theta += result[3];
      qoverp = (m_straightline) ? 0 : .001 * result[4] + qoverp;
    }

    if (!correctAngles(phi, theta)) {
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "angles out of range: " << theta << " " << phi << endmsg;
      }

      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "Fit failed" << endmsg;
      }

      return FitterStatusCode::InvalidAngles;
    }

    std::vector<std::pair<double, double> > &scatangles = trajectory.scatteringAngles();
    std::vector<double> &delta_ps = trajectory.brems();
    for (int i = 0; i < nscat; i++) {
      scatangles[i].first += result[2 * i + nperparams];
      scatangles[i].second += result[2 * i + nperparams + 1];
    }
    /*if (trajectory.prefit()==2){
       GXFTrackState *laststate=trajectory.trackStates().back();
       HepMatrix &derivs=laststate->derivatives();
       double dr=derivs[0][0]*result[0]+derivs[0][1]*result[1]+derivs[0][2]*result[2]+derivs[0][3]*result[3];
       double dz=derivs[1][0]*result[0]+derivs[1][1]*result[1]+derivs[1][2]*result[2]+derivs[1][3]*result[3];
       //std::cout << "dr: " << dr << " dz: " << dz << " locz: " <<
          laststate->trackParameters()->parameters()[Trk::locZ] << std::endl;
       }*/
    for (int i = 0; i < nbrem; i++) {
      delta_ps[i] += result[nperparams + 2 * nscat + i];
    }

    const TrackParameters *newper = trajectory.referenceParameters()->associatedSurface().createTrackParameters(d0, z0,
                                                                                                                phi,
                                                                                                                theta,
                                                                                                                qoverp,
                                                                                                                0);
    trajectory.setReferenceParameters(newper);
    trajectory.setScatteringAngles(scatangles);
    trajectory.setBrems(delta_ps);
    return FitterStatusCode::Success;
  }

  void
  GlobalChi2Fitter::print(GXFTrajectory &trajectory) const {
    const TrackParameters *per = trajectory.referenceParameters();
    double d0 = per->parameters()[Trk::d0];
    double z0 = per->parameters()[Trk::z0];
    double phi = per->parameters()[Trk::phi0];
    double theta = per->parameters()[Trk::theta];
    double qoverp = per->parameters()[Trk::qOverP];
    std::cout << "d0: " << d0 << " z0: " << z0 << " theta: " << theta << " phi: " << phi << " qoverp: " << qoverp;

    if (qoverp != 0) {
      std::cout << " pt: " << sin(theta) / qoverp;
    }
    std::cout << std::endl;

    std::vector<std::pair<double, double> > scatangles = trajectory.scatteringAngles();
    std::vector<std::pair<double, double> > scatsigmas = trajectory.scatteringSigmas();
    for (int i = 0; i < (int) scatangles.size(); i++) {
      if (i == 0) {
        std::cout << "scattering angles (phi, theta, sigmaphi, sigmatheta, pullphi, pulltheta): " << std::endl;
      }
      std::cout << scatangles[i].first << " " << scatangles[i].second << " " << scatsigmas[i].first << " " <<
        scatsigmas[i].second << " " << scatangles[i].first / scatsigmas[i].first << " " << scatangles[i].second /
        scatsigmas[i].second << std::endl;
    }
    std::vector<double> qoverpbrem = trajectory.brems();
    for (int i = 0; i < (int) qoverpbrem.size(); i++) {
      if (i == 0) {
        std::cout << "brem qoverp: " << std::endl;
      }
      std::cout << qoverpbrem[i] << std::endl;
    }
  }

  void
  GlobalChi2Fitter::runTrackCleanerTRT(GXFTrajectory &trajectory, TMatrixDSym &a, TVectorD &b, TDecompChol &lu,
                                       bool runOutlier, bool trtrecal, int it) const {
    double scalefactor = m_scalefactor;

    if (it == 1 && trajectory.numberOfSiliconHits() + trajectory.numberOfTRTHits() == trajectory.numberOfHits()) {
      scalefactor *= 2;
    }
    std::vector<GXFTrackState *> &states = trajectory.trackStates();
    std::vector<double> &res = trajectory.residuals();
    std::vector<double> &err = trajectory.errors();
    std::vector<std::vector<double> > &weightderiv = trajectory.weightedResidualDerivatives();
    double *myarray = a.GetMatrixArray();
    int nfitpars = trajectory.numberOfFitParameters();
    int nperpars = trajectory.numberOfPerigeeParameters();
    int nscats = trajectory.numberOfScatterers();
    int hitno = 0, measno = 0;
    bool outlierremoved = false, hitrecalibrated = false;
    for (int stateno = 0; stateno < (int) states.size(); stateno++) {
      GXFTrackState *state = states[stateno];
      TrackState::TrackStateType statetype = state->trackStateType();
      if (statetype == TrackState::Fittable) { // Hit is not (yet) an outlier
        TrackState::MeasurementType hittype = state->measurementType();

        if (hittype == TrackState::TRT) {
          if (runOutlier &&
              fabs(state->trackParameters()->parameters()[Trk::driftRadius]) > 1.05 * state->surface()->bounds().r()) {
            if (msgLvl(MSG::DEBUG)) {
              msg(MSG::DEBUG) << "Removing TRT hit #" << hitno << endmsg;
            }
            trajectory.setOutlier(stateno);
            outlierremoved = true;


            double *errors = state->measurementErrors();
            double olderror = errors[0];
            for (int i = 0; i < nfitpars; i++) {
              if (weightderiv[measno][i] == 0) {
                continue;
              }
              b[i] -= res[measno] * weightderiv[measno][i] / olderror;
              for (int j = i; j < nfitpars; j++) {
                myarray[j * nfitpars + i] -= weightderiv[measno][i] * weightderiv[measno][j];
                if (i != j) {
                  myarray[i * nfitpars + j] -= weightderiv[measno][i] * weightderiv[measno][j];
                }
              }
              weightderiv[measno][i] = 0;
            }
            res[measno] = 0;
          } else if (trtrecal) {
            double *errors = state->measurementErrors();
            double olderror = errors[0];
            const Trk::RIO_OnTrack *oldrot = dynamic_cast<const Trk::RIO_OnTrack *>(state->measurement());
            if (!oldrot) {
              continue;
            }
            double oldradius = oldrot->localParameters()[Trk::driftRadius];
            if (oldrot->prepRawData()) {
              double dcradius = oldrot->prepRawData()->localPosition()[Trk::driftRadius];
              double dcerror = sqrt(oldrot->prepRawData()->localCovariance()(Trk::driftRadius, Trk::driftRadius));
              double trackradius = state->trackParameters()->parameters()[Trk::driftRadius];

              const Trk::RIO_OnTrack *newrot = 0;
              double distance = std::abs(std::abs(trackradius) - dcradius);
              if (distance < scalefactor * dcerror && (olderror > 1. || trackradius * oldradius < 0)) {
                newrot = m_ROTcreator->correct(*oldrot->prepRawData(), *state->trackParameters());
              }
              if (distance > scalefactor * dcerror && olderror < 1.) {
                newrot = m_broadROTcreator->correct(*oldrot->prepRawData(), *state->trackParameters());
              }
              if (newrot) {
                ATH_MSG_DEBUG("Recalibrating TRT hit #" << hitno);
                hitrecalibrated = true;
                double newradius = newrot->localParameters()[Trk::driftRadius];
                double newerror = sqrt(newrot->localCovariance()(Trk::driftRadius, Trk::driftRadius));
                if ((measno < 0)or(measno >= (int) res.size())) {
                  throw std::runtime_error("'res' array index out of range in TrkGlobalChi2Fitter/src/GlobalChi2Fitter.cxx:" + std::to_string(
                                             __LINE__));
                }
                double oldres = res[measno];
                double newres = newradius - state->trackParameters()->parameters()[Trk::driftRadius];
                errors[0] = newerror;
                state->setMeasurement(newrot);

                for (int i = 0; i < nfitpars; i++) {
                  if (weightderiv[measno][i] == 0) {
                    continue;
                  }

                  b[i] -= weightderiv[measno][i] * (oldres / olderror - (newres * olderror) / (newerror * newerror));
                  for (int j = i; j < nfitpars; j++) {
                    double weight = 1;
                    if (!m_phiweight.empty() && i == j && i >= nperpars && i < nperpars + 2 * nscats &&
                        (i - nperpars) % 2 == 0) {
                      weight = m_phiweight[(i - nperpars) / 2];
                    }
                    // std::cout << "weight: " << weight << std::endl;
                    myarray[j * nfitpars + i] += weightderiv[measno][i] * weightderiv[measno][j] *
                                                 ((olderror * olderror) / (newerror * newerror) - 1) * weight;
                    if (i != j) {
                      myarray[i * nfitpars + j] += weightderiv[measno][i] * weightderiv[measno][j] *
                                                   ((olderror * olderror) / (newerror * newerror) - 1);
                    }
                  }
                  weightderiv[measno][i] *= olderror / newerror;
                }
                res[measno] = newres;
                err[measno] = newerror;
              }
            }
          }
        }
      }
      if (statetype == TrackState::Fittable || statetype == TrackState::GeneralOutlier) {
        hitno++;
        measno += state->numberOfMeasuredParameters();
      }
    }
    if (trajectory.nDOF() < 0) {
      m_fittercode = FitterStatusCode::OutlierLogicFailure;
      m_notenoughmeas++;
    }
    if (outlierremoved || hitrecalibrated) {
      lu.SetMatrix(a);
      trajectory.setConverged(false);
      // bool doderiv=true;
      // runIteration(trajectory,it,a,b,lu,doderiv);


      m_miniter = it + 2;
      /* if (trajectory.converged()) {
         FitterStatusCode fsc=updateFitParameters(trajectory,b,lu);
         if (fsc!=FitterStatusCode::Success) {
          m_fittercode=FitterStatusCode::OutlierLogicFailure;
          m_notenoughmeas++;
         }
         } */
    }
  }

  void
  GlobalChi2Fitter::runTrackCleanerMDT(GXFTrajectory &trajectory, TMatrixDSym &a, TMatrixDSym &fullcov, TVectorD &b,
                                       TDecompChol &lu) const {
    std::vector<GXFTrackState *> &states = trajectory.trackStates();
    double *myarray = a.GetMatrixArray();
    std::vector<double> &res = trajectory.residuals();
    std::vector<std::vector<double> > &weightderiv = trajectory.weightedResidualDerivatives();
    std::vector<double> &err = trajectory.errors();

    int nfitpars = trajectory.numberOfFitParameters();
    // int nhits=trajectory.numberOfHits();
    bool trackok = false;

    for (int outlit = 0; outlit < 3; outlit++) {
      trackok = true;
      double maxmdtpull = -1;
      int hitno = 0, hitno_maxmdtpull = -1;
      int measno_maxmdtpull = -1;
      int stateno_maxmdtpull = 0;
      GXFTrackState *state_maxmdtpull = 0;
      int measno = 0;

      for (int stateno = 0; stateno < (int) states.size(); stateno++) {
        GXFTrackState *state = states[stateno];

        TrackState::TrackStateType statetype = state->trackStateType();
        TrackState::MeasurementType hittype = state->measurementType();
        if (statetype == TrackState::Fittable && hittype == TrackState::MDT && state->trackCovariance()) { // Hit is not
                                                                                                           // (yet) an
                                                                                                           // outlier
          double *errors = state->measurementErrors();
          AmgSymMatrix(5) &trackcov = *state->trackCovariance();
          const AmgSymMatrix(5) &hitcov = state->measurement()->localCovariance();
          double weight1 = -1;
          if (hitcov(0, 0) > trackcov(0, 0)) {
            weight1 = errors[0] * errors[0] - trackcov(0, 0);
          }

          double mdtpull = weight1 > 0 ? std::abs(res[measno] / std::sqrt(weight1)) : -1;


          if (mdtpull > maxmdtpull &&
              (state->measurement()->localParameters()[Trk::locX] * state->trackParameters()->parameters()[Trk::locX] >
               0 ||
               state->isRecalibrated())) {
            maxmdtpull = mdtpull;
            measno_maxmdtpull = measno;
            // state_maxmdtpull=state; not used
            stateno_maxmdtpull = stateno;
            hitno_maxmdtpull = hitno;
          }else if (state->measurement()->localParameters()[Trk::locX] *
                    state->trackParameters()->parameters()[Trk::locX] < 0 && !state->isRecalibrated()) {
            const RIO_OnTrack *rot = dynamic_cast<const RIO_OnTrack *>(state->measurement());
            if (rot) {
              const AmgSymMatrix(5) &oldcovmat = rot->localCovariance();
              double olderror = sqrt(oldcovmat(0, 0));

              const PrepRawData *prd = rot->prepRawData();
              const RIO_OnTrack *newrot = 0;
              if (prd && !state->isRecalibrated()) {
                newrot = m_ROTcreator->correct(*prd, *state->trackParameters());
              }
              if (newrot) {
                if (msgLvl(MSG::DEBUG)) {
                  msg(MSG::DEBUG) << "Flipping MDT hit #" << hitno << " hit radius=" <<
                    state->measurement()->localParameters()[Trk::locX] << " track radius=" <<
                    state->trackParameters()->parameters()[Trk::locX] << endmsg;
                }
                trackok = false;
                state->setMeasurement(newrot);
                double newres = newrot->localParameters()[Trk::locX] -
                                state->trackParameters()->parameters()[Trk::locX];
                const AmgSymMatrix(5) &newcovmat = newrot->localCovariance();
                double newerror = std::sqrt(newcovmat(0, 0));
                trackok = false;
                double oldres = res[measno];
                res[measno] = newres;
                err[measno] = newerror;
                for (int i = 0; i < nfitpars; i++) {
                  if (weightderiv[measno][i] == 0) {
                    continue;
                  }
                  b[i] -= weightderiv[measno][i] * (oldres / olderror - (newres * olderror) / (newerror * newerror));
                  for (int j = i; j < nfitpars; j++) {
                    myarray[j * nfitpars + i] += weightderiv[measno][i] * weightderiv[measno][j] *
                                                 ((olderror * olderror) / (newerror * newerror) - 1);
                    if (i != j) {
                      myarray[i * nfitpars + j] += weightderiv[measno][i] * weightderiv[measno][j] *
                                                   ((olderror * olderror) / (newerror * newerror) - 1);
                    }
                  }
                  weightderiv[measno][i] *= olderror / newerror;
                }
              }
            }
          }
        }


        if (statetype == TrackState::Fittable || statetype == TrackState::GeneralOutlier) {
          hitno++;
          measno += state->numberOfMeasuredParameters();
        }
      }
      double maxpull = maxmdtpull;

      ATH_MSG_DEBUG(" maxmdtpull: " << maxmdtpull << " hitno_maxmdtpull: " << hitno_maxmdtpull);

      if (maxpull > 4) {
        trackok = false;

        state_maxmdtpull = trajectory.trackStates()[stateno_maxmdtpull];
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "Removing outlier, hitno=" << hitno_maxmdtpull << ", measno=" << measno_maxmdtpull <<
            " pull=" << maxmdtpull << " hit radius=" << state_maxmdtpull->measurement()->localParameters()[Trk::locX] <<
            " track radius=" << state_maxmdtpull->trackParameters()->parameters()[Trk::locX] << endmsg;
        }
        double olderror = state_maxmdtpull->measurementErrors()[0];
        if ((measno_maxmdtpull < 0)or(measno_maxmdtpull >= (int) res.size())) {
          throw std::runtime_error("'res' array index out of range in TrkGlobalChi2Fitter/src/GlobalChi2Fitter.cxx:" + std::to_string(
                                     __LINE__));
        }
        double oldres = res[measno_maxmdtpull];
        res[measno_maxmdtpull] = 0;
        for (int i = 0; i < nfitpars; i++) {
          if (weightderiv[measno_maxmdtpull][i] == 0) {
            continue;
          }
          b[i] -= weightderiv[measno_maxmdtpull][i] * oldres / olderror;
          for (int j = i; j < nfitpars; j++) {
            myarray[j * nfitpars + i] -= weightderiv[measno_maxmdtpull][i] * weightderiv[measno_maxmdtpull][j];
            if (i != j) {
              myarray[i * nfitpars + j] -= weightderiv[measno_maxmdtpull][i] * weightderiv[measno_maxmdtpull][j];
            }
          }
          weightderiv[measno_maxmdtpull][i] = 0;
        }
        trajectory.setOutlier(stateno_maxmdtpull);
      }


      if (!trackok) {
        trajectory.setConverged(false);
        bool doderiv = m_redoderivs;
        lu.SetMatrix(a);
        m_fittercode = updateFitParameters(trajectory, b, lu);
        if (m_fittercode != FitterStatusCode::Success) {
          m_notenoughmeas++;
          return;
        }

        for (int it = 1; it < m_maxit; it++) {
          if (it == m_maxit - 1) {
            if (msgLvl(MSG::DEBUG)) {
              msg(MSG::DEBUG) << "Fit did not converge" << endmsg;
            }
            m_fittercode = FitterStatusCode::NoConvergence;
            m_notconverge++;
            return;
          }
          if (!trajectory.converged()) {
            m_fittercode = runIteration(trajectory, it, a, b, lu, doderiv);
            if (m_fittercode != FitterStatusCode::Success) {
              m_notenoughmeas++;
              return;
            }
            if (!trajectory.converged()) {
              m_fittercode = updateFitParameters(trajectory, b, lu);
              if (m_fittercode != FitterStatusCode::Success) {
                m_notenoughmeas++;
                return;
              }
            }
          }else {
            bool invok = lu.Invert(fullcov);
            if (!invok) {
              if (msgLvl(MSG::DEBUG)) {
                msg(MSG::DEBUG) << "matrix inversion failed!" << endmsg;
              }
              m_matrixinvfailed++;
              m_fittercode = FitterStatusCode::MatrixInversionFailure;
              return;
            }
            calculateTrackErrors(trajectory, fullcov, true);
            break;
          }
        }
      }else {
        break;
      }
    }
  }

  GXFTrajectory *
  GlobalChi2Fitter::runTrackCleanerSilicon(GXFTrajectory &trajectory, TMatrixDSym &a, TMatrixDSym &fullcov, TVectorD &b,
                                           bool runoutlier) const {
    bool trackok = false;
    GXFTrajectory *oldtrajectory = &trajectory;
    std::unique_ptr<GXFTrajectory> cleanup_oldtrajectory;
    GXFTrajectory *newtrajectory = 0;
    std::unique_ptr<GXFTrajectory> cleanup_newtrajectory;

    // the oldtracjectory will be returned, so in case newtrajectory==oldtrajectory
    // the cleanup_newtrajectory == NULL and cleanup_oldtrajectory = oldtrajectory, otherwise
    // cleanup_newtrajectory will destroy the object oldtrajectory is pointing to.

    while (!trackok && oldtrajectory->nDOF() > 0) {
      trackok = true;
      std::vector<GXFTrackState *> &states = oldtrajectory->trackStates();
      std::vector<double> &res = oldtrajectory->residuals();
      std::vector<double> &err = oldtrajectory->errors();
      std::vector<std::vector<double> > &weightderiv = oldtrajectory->weightedResidualDerivatives();
      int nfitpars = oldtrajectory->numberOfFitParameters();
      int nhits = oldtrajectory->numberOfHits();
      int nsihits = oldtrajectory->numberOfSiliconHits();
      if (nhits != nsihits) {
        return &trajectory;
      }
      double maxsipull = -1;
      int hitno = 0, hitno_maxsipull = -1;
      int measno_maxsipull = -1;
      int stateno_maxsipull = 0;
      GXFTrackState *state_maxsipull = 0;
      int measno = 0;

      int n3sigma = 0;
      double cut = m_outlcut;
      double cut2 = m_outlcut - 1.;
      int noutl = oldtrajectory->numberOfOutliers();

      if (noutl > 0) {
        cut2 = cut - 1.25;
      }
      for (int stateno = 0; stateno < (int) states.size(); stateno++) {
        GXFTrackState *state = states[stateno];

        TrackState::TrackStateType statetype = state->trackStateType();
        if (statetype == TrackState::Fittable) { // Hit is not (yet) an outlier
          TrackState::MeasurementType hittype = state->measurementType();

          if ((hittype == TrackState::Pixel || hittype == TrackState::SCT) && state->trackCovariance()) { // Look for
                                                                                                          // silicon hit
                                                                                                          // with
                                                                                                          // largest
                                                                                                          // pull
            double *errors = state->measurementErrors();
            AmgSymMatrix(5) &trackcov = *state->trackCovariance();
            const Amg::MatrixX &hitcov = state->measurement()->localCovariance();
            double sinstereo = state->sinStereo();
            double cosstereo = (sinstereo == 0) ? 1 : sqrt(1 - sinstereo * sinstereo);
            double weight1 = -1;
            if (hitcov(0, 0) > trackcov(0, 0)) {
              if (sinstereo == 0) {
                weight1 = errors[0] * errors[0] - trackcov(0, 0);
              } else {
                weight1 = errors[0] * errors[0] -
                          (trackcov(0,
                                    0) * cosstereo * cosstereo + 2 *
                           trackcov(1, 0) * cosstereo * sinstereo + trackcov(1, 1) * sinstereo * sinstereo);
              }
            }
            double weight2 =
              (hittype == TrackState::Pixel &&
               hitcov(1, 1) > trackcov(1, 1)) ? errors[1] * errors[1] - trackcov(1, 1) : -1;

            double sipull1 = weight1 > 0 ? std::abs(res[measno] / sqrt(weight1)) : -1;
            double sipull2 =
              (hittype == TrackState::Pixel && weight2 > 0) ? std::abs(res[measno + 1] / sqrt(weight2)) : -1;
            sipull1 = std::max(sipull1, sipull2);


            if (sipull1 > maxsipull) {
              maxsipull = sipull1;
              measno_maxsipull = measno;
              state_maxsipull = state;
              stateno_maxsipull = stateno;
              hitno_maxsipull = hitno;
            }
            if (hittype == TrackState::Pixel && sipull1 > cut2) {
              n3sigma++;
            }
          }
        }


        if (statetype == TrackState::Fittable || statetype == TrackState::GeneralOutlier) {
          hitno++;
          measno += state->numberOfMeasuredParameters();
        }
      }
      double maxpull = maxsipull;

      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << " maxsipull: " << maxsipull << " hitno_maxsipull: " << hitno_maxsipull << " n3sigma: " <<
          n3sigma << " cut: " << cut << " cut2: " << cut2 << endmsg;
      }

      // TMatrixDSym *olda=&a;
      TMatrixDSym *newap = &a;
      // TVectorD *oldb=&b;
      TVectorD *newbp = &b;
      TMatrixDSym newa(nfitpars);
      TVectorD newb(nfitpars);
      TDecompChol newlu;

      if (maxpull > 2 && oldtrajectory->chi2() / oldtrajectory->nDOF() > .25 * m_chi2cut) {
        state_maxsipull = oldtrajectory->trackStates()[stateno_maxsipull];

        const RIO_OnTrack *rot = dynamic_cast<const RIO_OnTrack *>(state_maxsipull->measurement());

        const PrepRawData *prd = rot ? rot->prepRawData() : 0;
        std::unique_ptr<const RIO_OnTrack> broadrot;
        double *olderror = state_maxsipull->measurementErrors();
        TrackState::MeasurementType hittype_maxsipull = state_maxsipull->measurementType();
        const TrackParameters *trackpar_maxsipull = state_maxsipull->trackParameters();
        Amg::VectorX parameterVector = trackpar_maxsipull->parameters();
        std::unique_ptr<const TrackParameters> trackparForCorrect(
          trackpar_maxsipull->associatedSurface().createTrackParameters(parameterVector[Trk::loc1],
                                                                        parameterVector[Trk::loc2],
                                                                        parameterVector[Trk::phi],
                                                                        parameterVector[Trk::theta],
                                                                        parameterVector[Trk::qOverP], new AmgSymMatrix(
                                                                          5)(*state_maxsipull->trackCovariance())));
        double newerror[5];
        newerror[0] = newerror[1] = newerror[2] = newerror[3] = newerror[4] = -1;
        double newpull = -1, newpull1 = -1, newpull2 = -1, newres1 = -1, newres2 = -1;
        double newsinstereo = 0;

        if (prd && !state_maxsipull->isRecalibrated() && maxpull > 2.5 &&
            oldtrajectory->chi2() / trajectory.nDOF() > .3 * m_chi2cut && m_sirecal) {
          broadrot.reset(m_broadROTcreator->correct(*prd, *trackparForCorrect));
        }
        if (broadrot) {
          const Amg::MatrixX &covmat = broadrot->localCovariance();
          newerror[0] = sqrt(covmat(0, 0));
          if (state_maxsipull->sinStereo() != 0) {
            double v0 = 0.5 *
                        (covmat(0,
                                0) +
                         covmat(1,
                                1) -
                         sqrt((covmat(0,
                                      0) +
                               covmat(1,
                                      1)) *
                              (covmat(0,
                                      0) +
            covmat(1, 1)) - 4 * (covmat(0, 0) * covmat(1, 1) - covmat(0, 1) * covmat(0, 1))));
            double v1 = 0.5 *
                        (covmat(0,
                                0) +
                         covmat(1,
                                1) +
                         sqrt((covmat(0,
                                      0) +
                               covmat(1,
                                      1)) *
                              (covmat(0,
                                      0) +
            covmat(1, 1)) - 4 * (covmat(0, 0) * covmat(1, 1) - covmat(0, 1) * covmat(0, 1))));
            newsinstereo = sin(0.5 * asin(2 * covmat(0, 1) / (v0 - v1)));
            newerror[0] = sqrt(v0);
          }
          double cosstereo = (newsinstereo == 0) ? 1. : sqrt(1 - newsinstereo * newsinstereo);
          if (cosstereo != 1.) {
            newres1 = cosstereo *
                      (broadrot->localParameters()[Trk::locX] - trackpar_maxsipull->parameters()[Trk::locX]) +
                      newsinstereo *
                      (broadrot->localParameters()[Trk::locY] - trackpar_maxsipull->parameters()[Trk::locY]);
          } else {
            newres1 = broadrot->localParameters()[Trk::locX] - trackpar_maxsipull->parameters()[Trk::locX];
          }
          newpull1 = std::abs(newres1 / newerror[0]);
          if (hittype_maxsipull == TrackState::Pixel) {
            newerror[1] = sqrt(covmat(1, 1));
            newres2 = broadrot->localParameters()[Trk::locY] - trackpar_maxsipull->parameters()[Trk::locY];
            newpull2 = std::abs(newres2 / newerror[1]);
          }
          newpull = std::max(newpull1, newpull2);
        }


        if (broadrot && newpull < m_outlcut &&
            (newerror[0] > 1.5 * olderror[0] || newerror[1] > 1.5 * std::abs(olderror[1]))) {
          trackok = false;
          newtrajectory = oldtrajectory;
          double *myarray = a.GetMatrixArray();
          double oldres1 = res[measno_maxsipull];
          res[measno_maxsipull] = newres1;
          err[measno_maxsipull] = newerror[0];
          for (int i = 0; i < nfitpars; i++) {
            if (weightderiv[measno_maxsipull][i] == 0) {
              continue;
            }
            b[i] -= weightderiv[measno_maxsipull][i] *
                    (oldres1 / olderror[0] - (newres1 * olderror[0]) / (newerror[0] * newerror[0]));
            for (int j = i; j < nfitpars; j++) {
              myarray[j * nfitpars + i] += weightderiv[measno_maxsipull][i] * weightderiv[measno_maxsipull][j] *
                                           ((olderror[0] * olderror[0]) / (newerror[0] * newerror[0]) - 1);
              if (i != j) {
                myarray[i * nfitpars + j] += weightderiv[measno_maxsipull][i] * weightderiv[measno_maxsipull][j] *
                                             ((olderror[0] * olderror[0]) / (newerror[0] * newerror[0]) - 1);
              }
            }
            weightderiv[measno_maxsipull][i] *= olderror[0] / newerror[0];
          }
          if (hittype_maxsipull == TrackState::Pixel) {
            double oldres2 = res[measno_maxsipull + 1];
            res[measno_maxsipull + 1] = newres2;
            err[measno_maxsipull + 1] = newerror[1];
            for (int i = 0; i < nfitpars; i++) {
              if (weightderiv[measno_maxsipull + 1][i] == 0) {
                continue;
              }
              b[i] -= weightderiv[measno_maxsipull + 1][i] *
                      (oldres2 / olderror[1] - (newres2 * olderror[1]) / (newerror[1] * newerror[1]));
              for (int j = i; j < nfitpars; j++) {
                myarray[i * nfitpars + j] += weightderiv[measno_maxsipull + 1][i] *
                                             weightderiv[measno_maxsipull + 1][j] *
                                             ((olderror[1] * olderror[1]) / (newerror[1] * newerror[1]) - 1);
                if (i != j) {
                  myarray[j * nfitpars + i] += weightderiv[measno_maxsipull + 1][i] *
                                               weightderiv[measno_maxsipull + 1][j] *
                                               ((olderror[1] * olderror[1]) / (newerror[1] * newerror[1]) - 1);
                }
              }
              weightderiv[measno_maxsipull + 1][i] *= olderror[1] / newerror[1];
            }
          }
          if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << "Recovering outlier, hitno=" << hitno_maxsipull << " measno=" << measno_maxsipull <<
              " pull="
                            << maxsipull << " olderror_0=" << olderror[0] << " newerror_0=" << newerror[0] <<
              " olderror_1=" << olderror[1] << " newerror_1=" << newerror[1] << endmsg;
          }

          state_maxsipull->setMeasurement(broadrot.release());
          state_maxsipull->setSinStereo(newsinstereo);
          state_maxsipull->setMeasurementErrors(newerror);
        }else if (((((n3sigma < 2 && maxsipull > cut2 && maxsipull < cut) || n3sigma > 1) &&
                    (oldtrajectory->chi2() / oldtrajectory->nDOF() > .3 * m_chi2cut
                     || noutl > 1)) ||
                   maxsipull > cut) && (oldtrajectory->nDOF() > 1 || hittype_maxsipull == TrackState::SCT) &&
                  runoutlier) {
          trackok = false;
          if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << "Removing outlier, hitno=" << hitno_maxsipull << ", measno=" << measno_maxsipull <<
              " pull=" << maxsipull << endmsg;
          }
          newa = a;
          newb = b;
          newap = &newa;
          newbp = &newb;
          cleanup_newtrajectory.reset(new GXFTrajectory(*oldtrajectory));
          newtrajectory = cleanup_newtrajectory.get();
          double *myarray = newa.GetMatrixArray();
          std::vector<double> &newres = newtrajectory->residuals();
          // std::vector<double> &newerr=newtrajectory->errors();
          std::vector<std::vector<double> > &newweightderiv = newtrajectory->weightedResidualDerivatives();
          if ((measno_maxsipull < 0)or(measno_maxsipull >= (int) res.size())) {
            throw std::runtime_error("'res' array index out of range in TrkGlobalChi2Fitter/src/GlobalChi2Fitter.cxx:" + std::to_string(
                                       __LINE__));
          }
          double oldres1 = res[measno_maxsipull];
          newres[measno_maxsipull] = 0;
          for (int i = 0; i < nfitpars; i++) {
            if (weightderiv[measno_maxsipull][i] == 0) {
              continue;
            }
            newb[i] -= weightderiv[measno_maxsipull][i] * oldres1 / olderror[0];
            for (int j = i; j < nfitpars; j++) {
              myarray[j * nfitpars + i] -= weightderiv[measno_maxsipull][i] * weightderiv[measno_maxsipull][j];
              if (i != j) {
                myarray[i * nfitpars + j] -= weightderiv[measno_maxsipull][i] * weightderiv[measno_maxsipull][j];
              }
            }
            newweightderiv[measno_maxsipull][i] = 0;
          }
          if (hittype_maxsipull == TrackState::Pixel) {
            double oldres2 = res[measno_maxsipull + 1];
            newres[measno_maxsipull + 1] = 0;
            for (int i = 0; i < nfitpars; i++) {
              if (weightderiv[measno_maxsipull + 1][i] == 0) {
                continue;
              }
              newb[i] -= weightderiv[measno_maxsipull + 1][i] * oldres2 / olderror[1];
              for (int j = i; j < nfitpars; j++) {
                if (weightderiv[measno_maxsipull + 1][j] == 0) {
                  continue;
                }
                myarray[j * nfitpars + i] -= weightderiv[measno_maxsipull + 1][i] *
                                             weightderiv[measno_maxsipull + 1][j];
                if (i != j) {
                  myarray[i * nfitpars + j] -= weightderiv[measno_maxsipull + 1][i] *
                                               weightderiv[measno_maxsipull + 1][j];
                }
              }
              newweightderiv[measno_maxsipull + 1][i] = 0;
            }
          }
          newtrajectory->setOutlier(stateno_maxsipull);
        }
      }
      if (!trackok) {
        newtrajectory->setConverged(false);
        bool doderiv = m_redoderivs;
        newlu.SetMatrix(*newap);
        m_fittercode = updateFitParameters(*newtrajectory, *newbp, newlu);
        if (m_fittercode != FitterStatusCode::Success) {
          m_notenoughmeas++;
          return 0;
        }

        for (int it = 0; it < m_maxit; it++) {
          if (it == m_maxit - 1) {
            if (msgLvl(MSG::DEBUG)) {
              msg(MSG::DEBUG) << "Fit did not converge" << endmsg;
            }
            m_fittercode = FitterStatusCode::NoConvergence;
            m_notconverge++;
            return 0;
          }
          if (!newtrajectory->converged()) {
            m_fittercode = runIteration(*newtrajectory, it, *newap, *newbp, newlu, doderiv);
            if (m_fittercode != FitterStatusCode::Success) {
              m_notenoughmeas++;
              return 0;
            }
            if (!newtrajectory->converged()) {
              m_fittercode = updateFitParameters(*newtrajectory, *newbp, newlu);
              if (m_fittercode != FitterStatusCode::Success) {
                m_notenoughmeas++;

                return 0;
              }
            }
          }else {
            double oldchi2 = oldtrajectory->chi2() / oldtrajectory->nDOF();
            double newchi2 = (newtrajectory->nDOF() > 0) ? newtrajectory->chi2() / newtrajectory->nDOF() : 0;
            double mindiff = 0;
            if (newtrajectory->nDOF() != oldtrajectory->nDOF() && maxsipull > cut2) {
              mindiff = (oldchi2 > .33 * m_chi2cut || noutl > 0) ? .8 : 1.;
              if (noutl == 0 && maxsipull < cut - .5 && oldchi2 < .5 * m_chi2cut) {
                mindiff = 2.;
              }
            }
            if (newchi2 > oldchi2 || (newchi2 > oldchi2 - mindiff && newchi2 > .33 * oldchi2)) {
              if (msgLvl(MSG::DEBUG)) {
                msg(MSG::DEBUG) << "Outlier not confirmed, keeping old trajectory" << endmsg;
              }
              if (oldchi2 > m_chi2cut) {
                m_fittercode = FitterStatusCode::OutlierLogicFailure;
                m_notenoughmeas++;
                return 0;
              }
              cleanup_oldtrajectory.release();
              return oldtrajectory;
            }
            if (oldtrajectory != newtrajectory) {
              cleanup_oldtrajectory = std::move(cleanup_newtrajectory);
              oldtrajectory = newtrajectory;
              a = newa;
              b = newb;
            }
            double *myarray = a.GetMatrixArray();
            double *myarrayinv = fullcov.GetMatrixArray();
            AlSymMat weight(a.GetNcols());
            for (int i = 0; i < a.GetNcols(); ++i) {
              for (int j = 0; j <= i; ++j) {
                weight[i][j] = myarray[i * a.GetNcols() + j];
              }
            }
            int ok = weight.invert();
            for (int i = 0; i < a.GetNcols(); ++i) {
              for (int j = 0; j <= i; ++j) {
                myarrayinv[i * a.GetNcols() + j] = myarrayinv[j * a.GetNcols() + i] = weight[i][j];
              }
            }

            // bool ok=newlu.Invert(fullcov);
            if (ok) {
              if (msgLvl(MSG::DEBUG)) {
                msg(MSG::DEBUG) << "matrix inversion failed!" << endmsg;
              }
              m_matrixinvfailed++;
              m_fittercode = FitterStatusCode::MatrixInversionFailure;
              return 0;
            }
            break;
          }
        }
      }
      if (!trackok) {
        calculateTrackErrors(*oldtrajectory, fullcov, true);
      }
    }
    if (oldtrajectory->nDOF() > 0 && oldtrajectory->chi2() / oldtrajectory->nDOF() > m_chi2cut && runoutlier) {
      m_fittercode = FitterStatusCode::OutlierLogicFailure;
      m_notenoughmeas++;
      return 0;
    }
    cleanup_oldtrajectory.release();
    return oldtrajectory;
  }

  TrackStateOnSurface *
  GlobalChi2Fitter::makeTSOS(GXFTrackState *state, ParticleHypothesis matEffects) const {
    //
    // Convert internal track state into "official" TrackStateOnSurface
    //

    const TrackParameters *trackpar = state->trackParameters(true);
    const MeasurementBase *measurement = state->measurement(true);
    const FitQualityOnSurface *fitQual = state->fitQuality(true);
    GXFMaterialEffects *gxfmeff = state->materialEffects();
    const MaterialEffectsBase *mateff = 0;
    TrackState::TrackStateType tstype = state->trackStateType();
    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;

    if (state->trackCovariance()) {
      state->setTrackCovariance(0);
    }
    if (gxfmeff && (tstype == TrackState::Scatterer || tstype == TrackState::Brem)) {
      if (tstype == TrackState::Scatterer) {
        typePattern.set(TrackStateOnSurface::Scatterer);
      }
      if (gxfmeff->sigmaDeltaE() > 0) {
        if (gxfmeff->sigmaDeltaTheta() == 0) {
          typePattern.set(TrackStateOnSurface::CaloDeposit);
        } else {
          typePattern.set(TrackStateOnSurface::BremPoint);
          if (matEffects == electron && trackpar && false) {
            double pafter = std::abs(1 / trackpar->parameters()[Trk::qOverP]);
            double pbefore = std::abs(1 / (trackpar->parameters()[Trk::qOverP] - .001 * gxfmeff->delta_p()));
            double mass = m_particleMasses.mass[matEffects];
            double eafter = sqrt(pafter * pafter + mass * mass);
            double ebefore = sqrt(pbefore * pbefore + mass * mass);
            double z = eafter / ebefore;
            double beta = pbefore / ebefore;
            double sigqoverp = gxfmeff->sigmaDeltaE() / (beta * beta * pbefore * pbefore);
            mateff =
              new EstimatedBremOnTrack(gxfmeff->x0(), z, 0, sigqoverp * sigqoverp, trackpar->associatedSurface());
          }
        }
      }
      if (!mateff) {
        mateff = state->materialEffects()->makeMEOT();
      }
    }else {
      if (tstype == TrackState::Fittable) {
        typePattern.set(TrackStateOnSurface::Measurement);
        if (fitQual && msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "FitQualityOnSurface chi2: " << fitQual->chiSquared() << " / " << fitQual->numberDoF() <<
            endmsg;
        }
        if (fitQual && (fitQual->chiSquared() > 1.e5 || fitQual->chiSquared() < 0)) {
          double newchi2 = 0;
          int ndf = fitQual->numberDoF();
          if (fitQual->chiSquared() < 0) {
            newchi2 = 0;
          } else if (fitQual->chiSquared() > 1.e5) {
            newchi2 = 1.e5;
          }
          delete fitQual;
          fitQual = new FitQualityOnSurface(newchi2, ndf);
        }
      }else if (tstype == TrackState::GeneralOutlier) {
        typePattern.set(TrackStateOnSurface::Outlier);
      }
    }

    return new TrackStateOnSurface(measurement, trackpar, fitQual, mateff, typePattern);
  }

  Track *
  GlobalChi2Fitter::makeTrack(GXFTrajectory &oldtrajectory, ParticleHypothesis matEffects) const {
    //
    // Convert internal trajectory into track
    //


    DataVector<const TrackStateOnSurface> *trajectory = new DataVector<const TrackStateOnSurface>;

    std::vector<GXFTrackState *> &states = oldtrajectory.trackStates();
    if (m_fillderivmatrix) {
      std::vector<std::vector<double> > &derivs = oldtrajectory.weightedResidualDerivatives();
      std::vector<double> &errors = oldtrajectory.errors();
      int nrealmeas = 0;
      for (int hitno = 0; hitno < (int) states.size(); hitno++) {
        if (states[hitno]->trackStateType() == TrackState::Fittable &&
            (dynamic_cast<const RIO_OnTrack *>(states[hitno]->measurement()) ||
             dynamic_cast<const CompetingRIOsOnTrack *>(states[hitno]->measurement()))) {
          nrealmeas += states[hitno]->numberOfMeasuredParameters();
        }
      }
      delete m_derivmat;
      m_derivmat = new Amg::MatrixX(nrealmeas, oldtrajectory.numberOfFitParameters());
      m_derivmat->setZero();
      int measindex = 0, measindex2 = 0;
      int nperpars = oldtrajectory.numberOfPerigeeParameters();
      int nscat = oldtrajectory.numberOfScatterers();
      for (int hitno = 0; hitno < (int) states.size(); hitno++) {
        if (states[hitno]->trackStateType() == TrackState::Fittable &&
            (dynamic_cast<const RIO_OnTrack *>(states[hitno]->measurement()) ||
             dynamic_cast<const CompetingRIOsOnTrack *>(states[hitno]->measurement()))) {
          for (int i = measindex; i < measindex + states[hitno]->numberOfMeasuredParameters(); i++) {
            for (int j = 0; j < oldtrajectory.numberOfFitParameters(); j++) {
              (*m_derivmat)(i, j) = derivs[measindex2][j] * errors[measindex2];
              if ((j == 4 && !m_straightline) || j >= nperpars + 2 * nscat) {
                (*m_derivmat)(i, j) *= 1000;
              }
            }
            measindex2++;
          }
          measindex += states[hitno]->numberOfMeasuredParameters();
        }else if (!states[hitno]->materialEffects()) {
          measindex2 += states[hitno]->numberOfMeasuredParameters();
        }
      }
    }
    GXFTrackState *firstmeasstate = 0, *lastmeasstate = 0;
    bool foundbrem = false;
    for (int hitno = 0; hitno < (int) states.size(); hitno++) {
      if (states[hitno]->measurementType() == TrackState::Pseudo &&
          states[hitno]->trackStateType() == TrackState::GeneralOutlier) {
        if (states[hitno]->trackCovariance()) {
          states[hitno]->setTrackCovariance(0);
        }
        continue;
      }
      if (matEffects == electron && states[hitno]->materialEffects() && states[hitno]->materialEffects()->isKink()) {
        foundbrem = true;
      }
      const TrackStateOnSurface *trackState = makeTSOS(states[hitno], matEffects);
      trajectory->push_back(trackState);
      if (trackState->measurementOnTrack()) {
        if (!firstmeasstate) {
          firstmeasstate = states[hitno];
        }
        lastmeasstate = states[hitno];
      }
    }

    const FitQuality *qual = new FitQuality(oldtrajectory.chi2(), oldtrajectory.nDOF());

    ATH_MSG_VERBOSE("making Trk::Track...");
    TrackInfo info;
    if (matEffects != electron) {
      info = TrackInfo(TrackInfo::GlobalChi2Fitter, matEffects);
    } else {
      info = TrackInfo(TrackInfo::GlobalChi2Fitter, Trk::electron);
      info.setTrackProperties(TrackInfo::BremFit);
      if (foundbrem) {
        info.setTrackProperties(TrackInfo::BremFitSuccessful);
      }
    }
    if (m_straightline) {
      info.setTrackProperties(TrackInfo::StraightTrack);
    }

    if (!firstmeasstate) {
      throw std::logic_error("no first measurement.");
    }
    const TrackParameters *per = 0;
    if (m_acceleration && !m_matupdator.empty()) {
      const TrackParameters *prevpar = firstmeasstate->trackParameters();
      const TrackParameters *tmppar = firstmeasstate->trackParameters();
      std::vector<std::pair<const Layer *, const Layer *> > &upstreamlayers = oldtrajectory.upstreamMaterialLayers();
      for (int i = (int) upstreamlayers.size() - 1; i >= 0; i--) {
        if (!prevpar) {
          break;
        }
        PropDirection propdir = oppositeMomentum;
        const Layer *layer = upstreamlayers[i].first;
        if (!layer) {
          layer = upstreamlayers[i].second;
        }
        DistanceSolution distsol = layer->surfaceRepresentation().straightLineDistanceEstimate(
          prevpar->position(), prevpar->momentum().unit());
        double distance = 0;
        if (distsol.numberOfSolutions() == 1) {
          distance = distsol.first();
        } else if (distsol.numberOfSolutions() == 2) {
          distance = (std::abs(distsol.first()) < std::abs(distsol.second())) ? distsol.first() : distsol.second();
          if (fabs(distance) < 0.01) {
            continue;
          }
          if (distsol.first() * distsol.second() < 0 && prevpar != firstmeasstate->trackParameters()) {
            continue;
          }
        }
        if (prevpar == firstmeasstate->trackParameters() && distance > 0) {
          propdir = alongMomentum;
        }

        const TrackParameters *layerpar = m_propagator->propagate(*prevpar,
                                                                  layer->surfaceRepresentation(), propdir, true, *m_fieldprop,
                                                                  nonInteracting);
        if (!layerpar) {
          continue;
        }
        if (layer->surfaceRepresentation().bounds().inside(layerpar->localPosition())) {
          /* std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
              typePattern.set(TrackStateOnSurface::Scatterer);
              const TrackStateOnSurface *scattsos=new
                 TrackStateOnSurface(0,layerpar->cloneWithoutError(),0,0,typePattern);
              trajectory->insert(trajectory->begin(),scattsos); */
          const TrackParameters *updatedpar = m_matupdator->update(layerpar, *layer, oppositeMomentum, matEffects);
          if (updatedpar && updatedpar != layerpar) {
            delete layerpar;
            layerpar = updatedpar;
          }
        }
        if (prevpar != firstmeasstate->trackParameters()) {
          delete prevpar;
        }
        prevpar = layerpar;
      }
      const Layer *startlayer = firstmeasstate->trackParameters()->associatedSurface().associatedLayer();
      if (startlayer && startlayer->layerMaterialProperties()) {
        double startfactor = startlayer->layerMaterialProperties()->alongPostFactor();
        const DiscSurface *discsurf = dynamic_cast<const DiscSurface *>(startlayer);
        if (discsurf && discsurf->center().z() * discsurf->normal().z() < 0) {
          startfactor = startlayer->layerMaterialProperties()->oppositePostFactor();
        }
        if (startfactor > 0.5) {
          const TrackParameters *updatedpar = m_matupdator->update(
            firstmeasstate->trackParameters(), *startlayer, oppositeMomentum, matEffects);
          if (updatedpar && updatedpar != firstmeasstate->trackParameters()) {
            firstmeasstate->setTrackParameters(updatedpar);
          }
        }
      }
      // @TODO Coverity complains about a possible NULL pointer dereferencing in lastmeasstate->...
      // Now an exception is thrown if there is no firstmeastate. Thus if the code here is
      // reached then there should be a firstmeasstate and a lastmeasstate

      //    if (!lastmeasstate) throw std::logic_error("no last measurement.");
      const Layer *endlayer = lastmeasstate->trackParameters()->associatedSurface().associatedLayer();
      if (endlayer && endlayer->layerMaterialProperties()) {
        double endfactor = endlayer->layerMaterialProperties()->alongPreFactor();
        const DiscSurface *discsurf = dynamic_cast<const DiscSurface *>(endlayer);
        if (discsurf && discsurf->center().z() * discsurf->normal().z() < 0) {
          endfactor = endlayer->layerMaterialProperties()->oppositePreFactor();
        }
        if (endfactor > 0.5) {
          const TrackParameters *updatedpar = m_matupdator->update(
            lastmeasstate->trackParameters(), *endlayer, alongMomentum, matEffects);
          if (updatedpar && updatedpar != lastmeasstate->trackParameters()) {
            lastmeasstate->setTrackParameters(updatedpar);
          }
        }
      }
      if (prevpar) {
        per = m_propagator->propagate(*prevpar, PerigeeSurface(Amg::Vector3D(0, 0,
                                                                             0)), oppositeMomentum, false, *m_fieldprop,
                                      nonInteracting);
      }
      // std::cout << "prevpar: " << prevpar << " per: " << per << std::endl;
      if (prevpar != tmppar) {
        delete prevpar;
      }
      if (!per) {
        delete trajectory;
        delete qual;
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "Failed to extrapolate to perigee, returning 0" << endmsg;
        }
        m_fittercode = FitterStatusCode::ExtrapolationFailure;

        m_propfailed++;
        return 0;
      }
    }
    // @TODO Coverity complains about a possible NULL pointer dereferencing in firstmeasstate->...
    // now an exception is thrown if firstmeasstate is null. But does the code allow for firstmeasstate to
    // be null ?
    else if (m_acceleration && firstmeasstate->trackParameters()) {
      per = m_extrapolator->extrapolate(*firstmeasstate->trackParameters(), PerigeeSurface(Amg::Vector3D(0, 0,
                                                                                                         0)), oppositeMomentum, false,
                                        matEffects);
    } else {
      per = oldtrajectory.referenceParameters(true);
    }

    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(TrackStateOnSurface::Perigee);
    const TrackStateOnSurface *pertsos = new TrackStateOnSurface(0, per, 0, 0, typePattern);
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Final perigee: " << *per << " pos: " << per->position() << " pT: " << per->pT() << endmsg;
    }
    if (!m_acceleration) {
      trajectory->insert(trajectory->begin() + oldtrajectory.numberOfUpstreamStates(), pertsos);
    } else {
      trajectory->insert(trajectory->begin(), pertsos);
    }

    Track *track = new Track(info, trajectory, qual);

    return track;
  }

  GlobalChi2Fitter::~GlobalChi2Fitter() {
    delete m_fieldpropnofield;
    delete m_fieldpropfullfield;
  }

  FitterStatusCode
  GlobalChi2Fitter::calculateTrackParameters(GXFTrajectory &trajectory, bool calcderiv) const {
    //
    // Loop over states, calculate track parameters and (optionally) jacobian at each state
    //
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "CalculateTrackParameters" << endmsg;
    }

    std::vector<GXFTrackState *> &states = trajectory.trackStates();
    int nstatesupstream = trajectory.numberOfUpstreamStates();
    int bremno = trajectory.numberOfUpstreamBrems() - 1;

    GXFTrackState *prevstate = 0;
    const TrackParameters *prevtrackpar = trajectory.referenceParameters();
    for (int hitno = nstatesupstream - 1; hitno >= 0; hitno--) {
      TrackState::TrackStateType prevtstype = prevstate ? prevstate->trackStateType() : TrackState::AnyState;
      TransportJacobian *jac = 0;
      const Surface *surf = states[hitno]->surface();

      const TrackParameters *currenttrackpar = 0;
      Trk::PropDirection propdir = Trk::oppositeMomentum;

      DistanceSolution distsol = surf->straightLineDistanceEstimate(prevtrackpar->position(),
                                                                    prevtrackpar->momentum().unit());
      double distance = 0;
      if (distsol.numberOfSolutions() == 1) {
        distance = distsol.first();
      } else if (distsol.numberOfSolutions() == 2) {
        distance = (std::abs(distsol.first()) < std::abs(distsol.second())) ? distsol.first() : distsol.second();
      }
      if (distance > 0 && distsol.numberOfSolutions() > 0 && prevtrackpar != trajectory.referenceParameters()) {
        propdir = Trk::alongMomentum;
      }

      bool curvpar = false;
      // if (states[hitno]->trackStateType()==TrackState::Scatterer ||
      // states[hitno]->trackStateType()==TrackState::Brem) curvpar=true;
      if (calcderiv && !m_numderiv) {
        currenttrackpar = m_propagator->propagateParameters(*prevtrackpar, *surf, propdir, false, *m_fieldprop, jac,
                                                            Trk::nonInteracting, curvpar);
      } else {
        currenttrackpar = m_propagator->propagateParameters(*prevtrackpar, *surf, propdir, false, *m_fieldprop,
                                                            Trk::nonInteracting, curvpar);
      }

      if (!currenttrackpar) {
        if (jac) {
          delete jac;
          jac = 0;
        }
        propdir = ((propdir == Trk::oppositeMomentum) ? Trk::alongMomentum : Trk::oppositeMomentum);
        if (calcderiv && !m_numderiv) {
          currenttrackpar = m_propagator->propagateParameters(*prevtrackpar, *surf, propdir, false, *m_fieldprop, jac,
                                                              Trk::nonInteracting, curvpar);
        } else {
          currenttrackpar = m_propagator->propagateParameters(*prevtrackpar, *surf, propdir, false, *m_fieldprop,
                                                              Trk::nonInteracting, curvpar);
        }
      }

      if (currenttrackpar && m_numderiv && calcderiv) {
        delete jac;
        jac = numericalDerivatives(prevtrackpar, surf, propdir);
      }

      if (propdir == Trk::alongMomentum && currenttrackpar && msgLvl(MSG::DEBUG) &&
          (prevtrackpar->position() - currenttrackpar->position()).mag() > 5 * mm) {
        msg(MSG::DEBUG) << "Propagation in wrong direction" << endmsg;
        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "upstream prevtrackpar: " << *prevtrackpar << " current par: " << *currenttrackpar <<
            endmsg;
        }
      }
      if (!currenttrackpar) {
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "propagation failed, prev par: " << *prevtrackpar << " pos: " <<
            prevtrackpar->position() << " destination surface: " << *surf << endmsg;
        }
        if (jac) {
          delete jac;
        }
        if (hitno != nstatesupstream - 1 && (prevtstype == TrackState::Scatterer || prevtstype == TrackState::Brem)) {
          delete prevtrackpar;
        }
        return FitterStatusCode::ExtrapolationFailure;
      }
      if (hitno != nstatesupstream - 1 && (prevtstype == TrackState::Scatterer || prevtstype == TrackState::Brem)) {
        delete prevtrackpar;
      }
      states[hitno]->setTrackParameters(currenttrackpar);
      surf = states[hitno]->surface();

      if (calcderiv && !jac) {
        msg(MSG::WARNING) << "Jacobian is null" << endmsg;
        return FitterStatusCode::ExtrapolationFailure;
      }

      if (jac) {
        if (states[hitno]->materialEffects() && states[hitno]->materialEffects()->deltaE() != 0) {
          if (states[hitno]->materialEffects()->sigmaDeltaE() <= 0 && !m_straightline) {
            double p = 1 / std::abs(currenttrackpar->parameters()[Trk::qOverP]);
            double de = std::abs(states[hitno]->materialEffects()->deltaE());
            double mass = trajectory.mass();
            double newp = sqrt(p * p + 2 * de * sqrt(mass * mass + p * p) + de * de);
            (*jac)(4, 4) = ((p + p * de / sqrt(p * p + mass * mass)) / newp) * p * p / (newp * newp);
          }else {
            // double p=1/std::abs(currenttrackpar->parameters()[Trk::qOverP]);
            //  double newp=p-1.e3*states[hitno]->materialEffects()->delta_p();
            //  (*jac)[4][4]=p*p/(newp*newp);
          }
        }
        states[hitno]->setJacobian(jac);
      }
      GXFMaterialEffects *meff = states[hitno]->materialEffects();

      if (meff && hitno != 0) {
        double newphi = currenttrackpar->parameters()[Trk::phi0] - meff->deltaPhi();
        double newtheta = currenttrackpar->parameters()[Trk::theta] - meff->deltaTheta();
        bool ok = correctAngles(newphi, newtheta);
        if (!ok) {
          if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << "Angles out of range, phi: " << newphi << " theta: " << newtheta << endmsg;
          }
          return FitterStatusCode::InvalidAngles;
        }
        double newqoverp;
        double sign = (currenttrackpar->parameters()[Trk::qOverP] < 0) ? -1. : 1.;
        if (meff->sigmaDeltaE() <= 0 /*&& trajectory.prefit()!=2*/) {
          if (std::abs(currenttrackpar->parameters()[Trk::qOverP]) < 1.e-12) {
            newqoverp = 0.;
          } else {
            double mass = trajectory.mass();
            double oldp = std::abs(1 / currenttrackpar->parameters()[Trk::qOverP]);
            newqoverp = sign / sqrt(oldp * oldp + 2 * std::abs(meff->deltaE()) * sqrt(
                                      mass * mass + oldp * oldp) + meff->deltaE() * meff->deltaE());
          }
        }else {
          // newqoverp=.001*sign/(std::abs(1/(1000*currenttrackpar->parameters()[Trk::qOverP]))-meff->delta_p());
          newqoverp = currenttrackpar->parameters()[Trk::qOverP] - .001 * meff->delta_p();
          bremno--;
        }
        currenttrackpar = surf->createTrackParameters(currenttrackpar->parameters()[0],
                                                      currenttrackpar->parameters()[1], newphi, newtheta, newqoverp,
                                                      NULL);
      }
      prevtrackpar = currenttrackpar;
      prevstate = states[hitno];
    }


    prevtrackpar = trajectory.referenceParameters();
    bremno = trajectory.numberOfUpstreamBrems();
    for (int hitno = nstatesupstream; hitno < (int) states.size(); hitno++) {
      TransportJacobian *jac = 0;
      const Surface *surf = states[hitno]->surface();
      const TrackParameters *currenttrackpar = 0;
      Trk::PropDirection propdir = Trk::alongMomentum;
      DistanceSolution distsol = surf->straightLineDistanceEstimate(prevtrackpar->position(),
                                                                    prevtrackpar->momentum().unit());
      double distance = 0;
      if (distsol.numberOfSolutions() == 1) {
        distance = distsol.first();
      } else if (distsol.numberOfSolutions() == 2) {
        distance = (std::abs(distsol.first()) < std::abs(distsol.second())) ? distsol.first() : distsol.second();
      }
      if (distance < 0 && distsol.numberOfSolutions() > 0 && prevtrackpar != trajectory.referenceParameters()) {
        propdir = Trk::oppositeMomentum;
      }
      // std::cout << "jac: " << jac << " prevtrackpar: " << *prevtrackpar << std::endl;
      bool curvpar = false;
      // if (states[hitno]->trackStateType()==TrackState::Scatterer ||
      // states[hitno]->trackStateType()==TrackState::Brem) curvpar=true;
      if (calcderiv && !m_numderiv) {
        currenttrackpar = m_propagator->propagateParameters(*prevtrackpar, *surf, propdir, false, *m_fieldprop, jac,
                                                            Trk::nonInteracting, curvpar);
      } else {
        currenttrackpar = m_propagator->propagateParameters(*prevtrackpar, *surf, propdir, false, *m_fieldprop,
                                                            Trk::nonInteracting, curvpar);
      }

      if (!currenttrackpar) {
        propdir = ((propdir == Trk::oppositeMomentum) ? Trk::alongMomentum : Trk::oppositeMomentum);
        if (jac) {
          delete jac;
          jac = 0;
        }
        if (calcderiv && !m_numderiv) {
          currenttrackpar = m_propagator->propagateParameters(*prevtrackpar, *surf, propdir, false, *m_fieldprop, jac,
                                                              Trk::nonInteracting, curvpar);
        } else {
          currenttrackpar = m_propagator->propagateParameters(*prevtrackpar, *surf, propdir, false, *m_fieldprop,
                                                              Trk::nonInteracting, curvpar);
        }
      }
      if (currenttrackpar && m_numderiv && calcderiv) {
        delete jac;
        jac = numericalDerivatives(prevtrackpar, surf, propdir);
      }

      if (currenttrackpar && propdir == Trk::oppositeMomentum && msgLvl(MSG::DEBUG) &&
          (prevtrackpar->position() - currenttrackpar->position()).mag() > 5 * mm) {
        msg(MSG::DEBUG) << "Propagation in wrong direction" << endmsg;
        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "downstream prevtrackpar: " << *prevtrackpar << " surf: " <<
            prevtrackpar->associatedSurface() << " current par: " << *currenttrackpar << " surf: " <<
            currenttrackpar->associatedSurface() << endmsg;
        }
      }
      if (!currenttrackpar) {
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "propagation failed, prev par: " << *prevtrackpar << " pos: " <<
            prevtrackpar->position() << " destination surface: " << *surf << endmsg;
        }
        if (jac) {
          delete jac;
        }

        return FitterStatusCode::ExtrapolationFailure;
      }

      if (jac) {
        if (states[hitno]->materialEffects() && states[hitno]->materialEffects()->deltaE() != 0) {
          if (states[hitno]->materialEffects()->sigmaDeltaE() <= 0 && !m_straightline) {
            double p = 1 / std::abs(currenttrackpar->parameters()[Trk::qOverP]);
            double de = std::abs(states[hitno]->materialEffects()->deltaE());
            // double de=states[hitno]->materialEffects()->deltaE();
            double mass = trajectory.mass();
            double newp = p * p - 2 * de * sqrt(mass * mass + p * p) + de * de;
            if (newp > 0) {
              newp = sqrt(newp);
            }
            (*jac)(4, 4) = ((p - p * de / sqrt(p * p + mass * mass)) / newp) * p * p / (newp * newp);
          }else {
            // double p=1/std::abs(currenttrackpar->parameters()[Trk::qOverP]);
            // double newp=p+1e3*states[hitno]->materialEffects()->delta_p();
            // (*jac)[4][4]=p*p/(newp*newp);
          }
        }
        states[hitno]->setJacobian(jac);
      }

      if (calcderiv && !jac) {
        msg(MSG::WARNING) << "Jacobian is null" << endmsg;
        delete currenttrackpar;
        return FitterStatusCode::ExtrapolationFailure;
      }
      GXFMaterialEffects *meff = states[hitno]->materialEffects();

      if (meff) {
        AmgVector(5) newpars = currenttrackpar->parameters();

        double newphi = currenttrackpar->parameters()[Trk::phi0] + meff->deltaPhi();
        double newtheta = currenttrackpar->parameters()[Trk::theta] + meff->deltaTheta();
        bool ok = correctAngles(newphi, newtheta);
        if (!ok) {
          if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << "Angles out of range, phi: " << newphi << " theta: " << newtheta << endmsg;
          }
          delete currenttrackpar;
          return FitterStatusCode::InvalidAngles;
        }
        double newqoverp;
        double sign = (currenttrackpar->parameters()[Trk::qOverP] < 0) ? -1. : 1.;
        if (meff->sigmaDeltaE() <= 0 /* && trajectory.prefit()!=2*/) {
          if (std::abs(currenttrackpar->parameters()[Trk::qOverP]) < 1.e-12) {
            newqoverp = 0.;
          } else {
            double mass = trajectory.mass();
            double oldp = std::abs(1 / currenttrackpar->parameters()[Trk::qOverP]);
            double newp2 = oldp * oldp - 2 * std::abs(meff->deltaE()) * sqrt(mass * mass + oldp * oldp) +
                           meff->deltaE() * meff->deltaE();
            if (newp2 < 0) {
              if (msgLvl(MSG::DEBUG)) {
                msg(MSG::DEBUG) << "Track killed by energy loss update" << endmsg;
              }
              delete currenttrackpar;
              return FitterStatusCode::ExtrapolationFailureDueToSmallMomentum;
            }
            newqoverp = sign / sqrt(newp2);
          }
        }else {
          newqoverp = currenttrackpar->parameters()[Trk::qOverP] + .001 * meff->delta_p();
          // newqoverp=1e-3*sign/(std::abs(1/(1e3*currenttrackpar->parameters()[Trk::qOverP]))+meff->delta_p());

          /* if (bremno==0 && m_calosigmarz>0){
             if (dynamic_cast<const DiscSurface*>(surf)) newpars[Trk::locR ]+=meff->deltaRZ();
             else newpars[Trk::locZ ]+=meff->deltaRZ();
             } */
          bremno++;
        }

        newpars[Trk::phi] = newphi;
        newpars[Trk::theta] = newtheta;
        newpars[Trk::qOverP] = newqoverp;
        const TrackParameters *oldpar = currenttrackpar;
        currenttrackpar = surf->createTrackParameters(newpars[0], newpars[1], newpars[2], newpars[3], newpars[4], 0);
        // currenttrackpar=new CurvilinearParameters(currenttrackpar->position(),newphi,newtheta,newqoverp);
        // const CurvilinearParameters *curvparm=dynamic_cast<const CurvilinearParameters *>(currenttrackpar);
        // const TrackParameters *tmppar=curvparm->cloneAtSurface(surf);
        // currenttrackpar=tmppar->cloneToNew(newpars);
        // delete tmppar;
        delete oldpar;
      }
      states[hitno]->setTrackParameters(currenttrackpar);
      prevtrackpar = currenttrackpar;
    }
    return FitterStatusCode::Success;
  }

  void
  GlobalChi2Fitter::calculateDerivatives(GXFTrajectory &trajectory) const {
    //
    // Loop over states, calculate derivatives of local track parameters w.r.t. fit parameters
    //
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "CalculateDerivatives" << endmsg;
    }

    std::vector<GXFTrackState *> &states = trajectory.trackStates();
    int scatno = trajectory.numberOfUpstreamScatterers() - 1;
    int bremno = trajectory.numberOfUpstreamBrems() - 1;
    int nstatesupstream = trajectory.numberOfUpstreamStates();
    int nscatupstream = trajectory.numberOfUpstreamScatterers();
    int nbremupstream = trajectory.numberOfUpstreamBrems();
    int nscats = trajectory.numberOfScatterers();
    int nperpars = trajectory.numberOfPerigeeParameters();
    // int nfitpars=trajectory.numberOfFitParameters();
    double initialarray[25] = {
      0, 0, 0, 0, 0,
      0, 0, 0, 0, 0,
      0, 0, 0, 0, 0,
      0, 0, 0, 0, 0,
      0, 0, 0, 0, 1
    };
    typedef ROOT::Math::SMatrix<double, 5> SMatrix55;
    SMatrix55 initialjac(initialarray, 25);
    SMatrix55 jacvertex(initialjac);
    std::vector<SMatrix55> jacscat(trajectory.numberOfScatterers(), initialjac);
    int maxk[5] = {
      4, 4, 4, 4, 5
    };
    std::vector<SMatrix55> jacbrem(trajectory.numberOfBrems(), initialjac);
    // std::vector<double> pbrem(trajectory.numberOfBrems());

    // double sign=(trajectory.referenceParameters()->parameters()[Trk::qOverP]>0) ? 1 : -1;
    GXFTrackState *prevstate = 0, *state = 0;
    for (int hitno = nstatesupstream - 1; hitno >= 0; hitno--) {
      state = states[hitno];
      HepMatrix &derivmat = state->derivatives();
      bool fillderivmat = false;
      TrackState::TrackStateType tstype = state->trackStateType();
      if (tstype != TrackState::Scatterer && tstype != TrackState::Brem) {
        fillderivmat = true;
      }
      int jmin = 0, jmax = 4, jminbrem = 0, jmaxbrem = 4;
      if (hitno == 0) {
        if (!fillderivmat) {
          break;
        }
        jmin = 2;
        jmax = 3;
        jminbrem = 4;
      }
      double (*jac)[5] = state->jacobian();
      if (hitno == nstatesupstream - 1) {
        for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 5; j++) {
            jacvertex(i, j) = jac[i][j];
          }
        }
        jacvertex(4, 4) = jac[4][4];
      }else {
        for (int scatindex = nscatupstream - 1; scatindex > scatno; scatindex--) {
          if (prevstate && prevstate->trackStateType() == TrackState::Scatterer && scatindex == scatno + 1 &&
              (!trajectory.prefit() || prevstate->materialEffects()->deltaE() == 0)) {
            for (int i = 0; i < 4; i++) {
              for (int j = jmin; j <= jmax; j++) {
                jacscat[scatindex](i, j) = jac[i][j];
              }
            }
            jacscat[scatindex](4, 4) = jac[4][4];
          }else {
            SMatrix55 &tmpjac2 = jacscat[scatindex];
            SMatrix55 &tmpjac = initialjac;
            double *myarray = tmpjac2.Array();
            for (int i = 0; i < 4; i++) {
              // double tmparray[5];
              int myindex;
              for (int j = jmin; j <= jmax; j++) {
                double tmp = 0;
                myindex = j;
                for (int k = 0; k < maxk[j]; k++) {
                  // tmp+=jac[i][k]*jacscat[scatindex](k,j);
                  tmp += jac[i][k] * myarray[myindex];
                  myindex += 5;
                }
                tmpjac(i, j) = tmp;
                // tmparray[j]=tmp;
              }
              /* myindex=5*i+jmin;
                 for (int j=jmin;j<=jmax;j++) {
                 myarray[myindex] = tmparray[j];
                 myindex++;
                 } */
            }
            jacscat[scatindex] = tmpjac;
            jacscat[scatindex](4, 4) = jac[4][4] * jacscat[scatindex](4, 4);
          }
          if (fillderivmat) {
            for (int i = 0; i < 4; i++) {
              derivmat[i][nperpars + 2 * scatindex] = -jacscat[scatindex](i, 2);
              derivmat[i][nperpars + 2 * scatindex + 1] = -jacscat[scatindex](i, 3);
            }
          }
        }
        for (int bremindex = nbremupstream - 1; bremindex > bremno; bremindex--) {
          if (prevstate && prevstate->materialEffects() && prevstate->materialEffects()->sigmaDeltaE() > 0 &&
              bremindex == bremno + 1) {
            for (int i = 0; i < 4; i++) {
              for (int j = jminbrem; j <= jmaxbrem; j++) {
                jacbrem[bremindex](i, j) = jac[i][j];
              }
            }
            jacbrem[bremindex](4, 4) = jac[4][4];
          }else {
            SMatrix55 &tmpjac = initialjac;// jacbrem[bremindex];
            for (int i = 0; i < 4; i++) {
              for (int j = jminbrem; j <= jmaxbrem; j++) {
                double tmp = 0;
                for (int k = 0; k < maxk[j]; k++) {
                  tmp += jac[i][k] * jacbrem[bremindex](k, j);
                }
                tmpjac(i, j) = tmp;
              }
            }
            jacbrem[bremindex] = tmpjac;
            jacbrem[bremindex](4, 4) = jac[4][4] * jacbrem[bremindex](4, 4);
          }
          /* if (state->materialEffects() && state->materialEffects()->sigmaDeltaE()>0) {
             double ploc=1/std::abs(1000*state->trackParameters()->parameters()[Trk::qOverP]);
             state->materialEffects()->momentumJacobians()[bremindex+1]=jacbrem[bremindex](4,4)*ploc*ploc/(jac[4][4]*pbrem[bremindex]*pbrem[bremindex]);
             } */
          if (fillderivmat) {
            // double p=pbrem[bremindex];
            // for (int i=0;i<5;i++)
            // derivmat[i][nperpars+2*nscats+bremindex]=0.001*jacbrem[bremindex](i,4)*(sign/(p*p));
            for (int i = 0; i < 5; i++) {
              derivmat[i][nperpars + 2 * nscats + bremindex] = -0.001 * jacbrem[bremindex](i, 4);
            }
          }
        }
        // SMatrix55 &tmpjac=jacvertex;
        SMatrix55 &tmpjac = initialjac;
        for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 5; j++) {
            double tmp = 0;
            for (int k = 0; k < maxk[j]; k++) {
              tmp += jac[i][k] * jacvertex(k, j);
            }
            tmpjac(i, j) = tmp;
          }
        }
        jacvertex = tmpjac;
        jacvertex(4, 4) = jac[4][4] * jacvertex(4, 4);
      }
      /* if (state->materialEffects() && state->materialEffects()->sigmaDeltaE()>0) {
         pbrem[bremno]=std::abs(1/(1000*state->trackParameters()->parameters()[Trk::qOverP]))-state->materialEffects()->delta_p();

         //pbrem[bremno]-=state->materialEffects()->delta_p();
         state->materialEffects()->momentumJacobians()[0]=jacvertex(4,4)/jac[4][4];
         } */
      if (fillderivmat) {
        for (int i = 0; i < 4; i++) {
          for (int j = 0; j < nperpars; j++) {
            derivmat[i][j] = jacvertex(i, j);
          }
          if (nperpars == 5) {
            derivmat[i][4] *= .001;
          }
        }
        if (nperpars == 5) {
          derivmat[4][4] = .001 * jacvertex(4, 4);
        }
      }

      if (tstype == TrackState::Scatterer &&
          (!trajectory.prefit() || states[hitno]->materialEffects()->deltaE() == 0)) {
        scatno--;
      }
      if (states[hitno]->materialEffects() && states[hitno]->materialEffects()->sigmaDeltaE() > 0) {
        bremno--;
      }
      prevstate = states[hitno];
    }

    scatno = nscatupstream;
    bremno = nbremupstream;
    prevstate = 0;

    for (int hitno = nstatesupstream; hitno < (int) states.size(); hitno++) {
      state = states[hitno];
      HepMatrix &derivmat = state->derivatives();
      bool fillderivmat = false;
      TrackState::TrackStateType tstype = state->trackStateType();
      int imax = 3;
      if (tstype != TrackState::Scatterer && tstype != TrackState::Brem) {
        fillderivmat = true;
        // if (trajectory.prefit() && states[hitno]->measurementType()!=TrackState::Pseudo )
        // imax=states[hitno]->numberOfMeasuredParameters()-1;
      }
      int jmin = 0, jmax = 4, jminbrem = 0, jmaxbrem = 4;
      if (hitno == (int) states.size() - 1) {
        if (!fillderivmat) {
          break;
        }
        jmin = 2;
        jmax = 3;
        jminbrem = 4;
      }
      double (*jac)[5] = state->jacobian();

      if (hitno == nstatesupstream) {
        for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 5; j++) {
            jacvertex(i, j) = jac[i][j];
          }
        }
        jacvertex(4, 4) = jac[4][4];
      }else {
        for (int scatindex = nscatupstream; scatindex < scatno; scatindex++) {
          if (prevstate && prevstate->trackStateType() == TrackState::Scatterer && scatindex == scatno - 1 &&
              (!trajectory.prefit() || prevstate->materialEffects()->deltaE() == 0)) {
            for (int i = 0; i < 4; i++) {
              for (int j = jmin; j <= jmax; j++) {
                jacscat[scatindex](i, j) = jac[i][j];
              }
            }
            jacscat[scatindex](4, 4) = jac[4][4];
          }else {
            SMatrix55 &tmpjac2 = jacscat[scatindex];
            SMatrix55 &tmpjac = initialjac;
            double *myarray = tmpjac2.Array();
            for (int i = 0; i < 4; i++) {
              // double tmparray[5];
              int myindex;
              for (int j = jmin; j <= jmax; j++) {
                double tmp = 0;
                myindex = j;
                for (int k = 0; k < maxk[j]; k++) {
                  // tmp+=jac[i][k]*jacscat[scatindex](k,j);
                  tmp += jac[i][k] * myarray[myindex];
                  myindex += 5;
                }
                tmpjac(i, j) = tmp;
                // tmparray[j]=tmp;
              }
              /* myindex=5*i+jmin;
                 for (int j=jmin;j<=jmax;j++) {
                 myarray[myindex] = tmparray[j];
                 myindex++;
                 } */
            }
            jacscat[scatindex] = tmpjac;
            jacscat[scatindex](4, 4) = jacscat[scatindex](4, 4) * jac[4][4];
          }
          if (fillderivmat) {
            for (int i = 0; i <= imax; i++) {
              derivmat[i][nperpars + 2 * scatindex] = jacscat[scatindex](i, 2);
              derivmat[i][nperpars + 2 * scatindex + 1] = jacscat[scatindex](i, 3);
            }
          }
        }
        for (int bremindex = nbremupstream; bremindex < bremno; bremindex++) {
          if (prevstate && prevstate->materialEffects() && prevstate->materialEffects()->sigmaDeltaE() > 0 &&
              bremindex == bremno - 1) {
            for (int i = 0; i < 4; i++) {
              for (int j = jminbrem; j <= jmaxbrem; j++) {
                jacbrem[bremindex](i, j) = jac[i][j];
              }
            }
            jacbrem[bremindex](4, 4) = jac[4][4];
          }else {
            SMatrix55 &tmpjac = initialjac;// jacbrem[bremindex];
            for (int i = 0; i < 4; i++) {
              for (int j = jminbrem; j <= jmaxbrem; j++) {
                double tmp = 0;
                for (int k = 0; k < maxk[j]; k++) {
                  tmp += jac[i][k] * jacbrem[bremindex](k, j);
                }
                tmpjac(i, j) = tmp;
              }
            }
            jacbrem[bremindex] = tmpjac;
            jacbrem[bremindex](4, 4) = jacbrem[bremindex](4, 4) * jac[4][4];
          }
          /* if (state->materialEffects() && state->materialEffects()->sigmaDeltaE()>0) {
             double
                ploc=1/std::abs(1000*state->trackParameters()->parameters()[Trk::qOverP])-state->materialEffects()->delta_p();
             state->materialEffects()->momentumJacobians()[bremindex+1]=jacbrem[bremindex](4,4)*ploc*ploc/(jac[4][4]*pbrem[bremindex]*pbrem[bremindex]);
             } */

          if (fillderivmat) {
            // double p=pbrem[bremindex];
            for (int i = 0; i <= 4; i++) {
              // derivmat[i][nperpars+2*nscats+bremindex]=.001*jacbrem[bremindex](i,4)*(-sign/(p*p));
              derivmat[i][nperpars + 2 * nscats + bremindex] = .001 * jacbrem[bremindex](i, 4);
            }
          }
        }
        // SMatrix55 &tmpjac=jacvertex;
        SMatrix55 &tmpjac = initialjac;
        for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 5; j++) {
            double tmp = 0;
            for (int k = 0; k < maxk[j]; k++) {
              tmp += jac[i][k] * jacvertex(k, j);
            }
            tmpjac(i, j) = tmp;
          }
        }
        jacvertex = tmpjac;
        jacvertex(4, 4) = jacvertex(4, 4) * jac[4][4];
      }
      /* if (state->materialEffects() && state->materialEffects()->sigmaDeltaE()>0) {

         pbrem[bremno]=std::abs(1/(1000*state->trackParameters()->parameters()[Trk::qOverP]));
         state->materialEffects()->momentumJacobians()[0]=jacvertex(4,4)/jac[4][4];
         } */

      if (fillderivmat) {
        for (int i = 0; i <= imax; i++) {
          for (int j = 0; j < nperpars; j++) {
            derivmat[i][j] = jacvertex(i, j);
          }
          if (nperpars == 5) {
            derivmat[i][4] *= .001;
          }
        }
        if (nperpars == 5) {
          derivmat[4][4] = 0.001 * jacvertex(4, 4);
        }
      }


      if (tstype == TrackState::Scatterer &&
          (!trajectory.prefit() || states[hitno]->materialEffects()->deltaE() == 0)) {
        scatno++;
      }
      if (states[hitno]->materialEffects() && states[hitno]->materialEffects()->sigmaDeltaE() > 0) {
        bremno++;
      }
      prevstate = states[hitno];
    }
  }

  void
  GlobalChi2Fitter::calculateTrackErrors(GXFTrajectory &trajectory, TMatrixDSym &fullcovmat, bool onlylocal) const {
    //
    // Calculate track errors at each state, except scatterers and brems
    //

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "CalculateTrackErrors" << endmsg;
    }

    std::vector<GXFTrackState *> &states = trajectory.trackStates();
    int nstatesupstream = trajectory.numberOfUpstreamStates();
    int nscatupstream = trajectory.numberOfUpstreamScatterers();
    int nbremupstream = trajectory.numberOfUpstreamBrems();
    int nfitpars = trajectory.numberOfFitParameters();
    int nscats = trajectory.numberOfScatterers();
    // int nbrems=trajectory.numberOfBrems();
    int nperpars = trajectory.numberOfPerigeeParameters();
    int hitno = 0;
    int scatno = nscatupstream, bremno = nbremupstream;
    std::vector<int> indices(states.size());
    GXFTrackState *prevstate = 0;
    int i = nstatesupstream;
    // int firstbremindex=nperpars+2*nscats;
    for (int j = 0; j < (int) states.size(); j++) {
      if (j < nstatesupstream) {
        i--;
        indices[j] = i;
      }else {
        indices[j] = j;
      }
    }
    double *myarray = fullcovmat.GetMatrixArray();
    std::vector<int> rowindices[5];
    for (int i = 0; i < 5; i++) {
      rowindices[i].reserve(nfitpars);
    }
    for (int stateno = 0; stateno < (int) states.size(); stateno++) {
      if (stateno == 0 || stateno == nstatesupstream) {
        scatno = nscatupstream;
        bremno = nbremupstream;
        prevstate = 0;
      }
      int index = indices[stateno];
      // std::cout << "index: " << index << std::endl;
      GXFTrackState *state = states[index];
      if (state->materialEffects()) {
        if (state->trackStateType() == TrackState::Scatterer) { // Skip scatterers
          if (index >= nstatesupstream) {
            scatno++;
          } else {
            scatno--;
          }
        }
        if (state->materialEffects()->sigmaDeltaE() > 0) { // Skip brem points
          if (index >= nstatesupstream) {
            bremno++;
          } else {
            bremno--;
          }
        }
        prevstate = state;
        continue;
      }

      if (!state->trackCovariance()) {
        AmgSymMatrix(5) * newcov = new AmgSymMatrix(5);
        newcov->setZero();
        state->setTrackCovariance(newcov);
      }
      AmgSymMatrix(5) &trackerrmat = *state->trackCovariance();


      if (prevstate &&
          (prevstate->trackStateType() == TrackState::Fittable ||
           prevstate->trackStateType() == TrackState::GeneralOutlier) && !onlylocal) {
        double (*jac)[5] = state->jacobian();
        AmgSymMatrix(5) &prevcov = *states[indices[stateno - 1]]->trackCovariance();
        errors1(jac, prevcov, trackerrmat, onlylocal);
      }else {
        int maxl = m_straightline ? 3 : 4;
        int minm[5] = {
          0, 0, 0, 0, 0
        };
        if (onlylocal) {
          TrackState::MeasurementType hittype = state->measurementType();
          if (hittype == TrackState::Pixel || state->sinStereo() != 0) {
            maxl = 1;
          } else {
            maxl = 0;
          }
          if (hittype == TrackState::Pixel) {
            minm[1] = 1;
          }
        }
        HepMatrix &derivatives = state->derivatives();
        // Only consider scatterers and brems which give non-zero derivatives
        int scatmin = (scatno < nscatupstream) ? scatno : nscatupstream;
        int scatmax = (scatno < nscatupstream) ? nscatupstream : scatno;
        int bremmin = (bremno < nbremupstream) ? bremno : nbremupstream;
        int bremmax = (bremno < nbremupstream) ? nbremupstream : bremno;
        for (int i = 0; i < 4; i++) {
          rowindices[i].clear();
          for (int j = 0; j < nperpars; j++) {
            rowindices[i].push_back(j);
          }
          for (int j = nperpars + 2 * scatmin; j < nperpars + 2 * scatmax; j++) {
            rowindices[i].push_back(j);
          }
          for (int j = nperpars + 2 * nscats + bremmin; j < nperpars + 2 * nscats + bremmax; j++) {
            rowindices[i].push_back(j);
          }
        }
        if (!m_straightline) {
          rowindices[4].clear();
          rowindices[4].push_back(4);
          for (int j = nperpars + 2 * nscats + bremmin; j < nperpars + 2 * nscats + bremmax; j++) {
            rowindices[4].push_back(j);
          }
        }
        errors2(derivatives, trackerrmat, myarray, rowindices, maxl, minm, onlylocal, nfitpars);
      }

      if (!onlylocal) {
        const MeasurementBase *measurement = state->measurement();
        const Amg::MatrixX &meascov = measurement->localCovariance();
        int j = 0;
        ParamDefsAccessor paraccessor;
        int indices[5] = {
          -1, -1, -1, -1, -1
        };
        bool errorok = true;
        for (int i = 0; i < 5; i++) {
          if (measurement->localParameters().contains(paraccessor.pardef[i])) {
            if (state->trackStateType() == TrackState::Fittable && trackerrmat(i, i) > meascov(j, j)) {
              errorok = false;
              double scale = sqrt(meascov(j, j) / trackerrmat(i, i));
              trackerrmat(i, i) = meascov(j, j);
              for (int k = 0; k < 5; k++) {
                if (k != i) {
                  trackerrmat(k, i) *= scale;
                }
              }
              indices[i] = j;
            }
            j++;
          }
        }
        for (int i = 0; i < 5; i++) {
          if (indices[i] == -1) {
            continue;
          }
          for (int j = 0; j < 5; j++) {
            if (indices[j] == -1) {
              continue;
            }
            trackerrmat(i, j) = meascov(indices[i], indices[j]);
          }
        }
        if (m_straightline) {
          trackerrmat(4, 4) = 1e-20;
        }

        const TrackParameters *trackpar = 0, *tmptrackpar = state->trackParameters();

        AmgSymMatrix(5) * trkerrmat = state->trackCovariance(true);
        const AmgVector(5) &tpars = tmptrackpar->parameters();
        trackpar =
          tmptrackpar->associatedSurface().createTrackParameters(tpars[0], tpars[1], tpars[2], tpars[3], tpars[4],
                                                                 trkerrmat);
        state->setTrackParameters(trackpar);
        const FitQualityOnSurface *fitQual = 0;
        if (state->trackStateType() == TrackState::Fittable) {
          if (errorok && trajectory.nDOF() > 0) {
            fitQual = m_updator->fullStateFitQuality(*trackpar,
                                                     measurement->localParameters(), measurement->localCovariance());
          } else {
            fitQual = new FitQualityOnSurface(0, state->numberOfMeasuredParameters());
          }
        }
        state->setFitQuality(fitQual);
      }
      prevstate = state;
      hitno++;
    }
  }

  TransportJacobian *
  GlobalChi2Fitter::numericalDerivatives(const TrackParameters *prevpar, const Surface *surf,
                                         PropDirection propdir) const {
    ParamDefsAccessor paraccessor;
    double J[25] = {
      1, 0, 0, 0, 0,
      0, 1, 0, 0, 0,
      0, 0, 1, 0, 0,
      0, 0, 0, 1, 0,
      0, 0, 0, 0, 1
    };
    TransportJacobian *jac = new TransportJacobian(J);
    const TrackParameters *tmpprevpar = prevpar;
    // const CurvilinearParameters *curvpar=dynamic_cast<const CurvilinearParameters *>(prevpar);
    // if (curvpar) tmpprevpar=new
    // AtaPlane(0,0,curvpar->parameters()[Trk::phi0],curvpar->parameters()[Trk::theta],curvpar->parameters()[Trk::qOverP],curvpar->associatedSurface());
    double eps[5] = {
      0.01, 0.01, 0.00001, 0.00001, 0.000000001
    };

    const AmgVector(5) &vec = tmpprevpar->parameters();
    const Trk::CylinderSurface *cylsurf = dynamic_cast<const Trk::CylinderSurface *>(surf);
    const Trk::DiscSurface *discsurf = dynamic_cast<const Trk::DiscSurface *>(surf);
    const Trk::CylinderSurface *thiscylsurf =
      dynamic_cast<const Trk::CylinderSurface *>(&tmpprevpar->associatedSurface());
    const Trk::DiscSurface *thisdiscsurf = dynamic_cast<const Trk::DiscSurface *>(&tmpprevpar->associatedSurface());

    for (int i = 0; i < 5; i++) {
      AmgVector(5) vecpluseps = vec, vecminuseps = vec;

      if (thisdiscsurf && i == 1) {
        eps[i] /= vec[0];
      }

      vecpluseps[paraccessor.pardef[i]] += eps[i];
      vecminuseps[paraccessor.pardef[i]] -= eps[i];
      if (thiscylsurf && i == 0) {
        if (vecpluseps[0] / thiscylsurf->bounds().r() > M_PI) {
          vecpluseps[0] -= 2 * M_PI * thiscylsurf->bounds().r();
        }
        if (vecminuseps[0] / thiscylsurf->bounds().r() < -M_PI) {
          vecminuseps[0] += 2 * M_PI * thiscylsurf->bounds().r();
        }
      }
      if (thisdiscsurf && i == 1) {
        if (vecpluseps[i] > M_PI) {
          vecpluseps[i] -= 2 * M_PI;
        }
        if (vecminuseps[i] < -M_PI) {
          vecminuseps[i] += 2 * M_PI;
        }
      }
      correctAngles(vecminuseps[Trk::phi], vecminuseps[Trk::theta]);
      correctAngles(vecpluseps[Trk::phi], vecpluseps[Trk::theta]);

      const TrackParameters *parpluseps = tmpprevpar->associatedSurface().createTrackParameters(vecpluseps[0],
                                                                                                vecpluseps[1],
                                                                                                vecpluseps[2],
                                                                                                vecpluseps[3],
                                                                                                vecpluseps[4], 0);
      const TrackParameters *parminuseps = tmpprevpar->associatedSurface().createTrackParameters(vecminuseps[0],
                                                                                                 vecminuseps[1],
                                                                                                 vecminuseps[2],
                                                                                                 vecminuseps[3],
                                                                                                 vecminuseps[4], 0);
      const TrackParameters *newparpluseps = m_propagator->propagateParameters(*parpluseps, *surf, propdir, false,
                                                                               *m_fieldprop, Trk::nonInteracting);
      const TrackParameters *newparminuseps = m_propagator->propagateParameters(*parminuseps, *surf, propdir, false,
                                                                                *m_fieldprop, Trk::nonInteracting);
      PropDirection propdir2 = (propdir == Trk::alongMomentum) ? Trk::oppositeMomentum : Trk::alongMomentum;
      if (!newparpluseps) {
        newparpluseps = m_propagator->propagateParameters(*parpluseps, *surf, propdir2, false, *m_fieldprop,
                                                          Trk::nonInteracting);
      }
      if (!newparminuseps) {
        newparminuseps = m_propagator->propagateParameters(*parminuseps, *surf, propdir2, false, *m_fieldprop,
                                                           Trk::nonInteracting);
      }
      delete parpluseps;
      delete parminuseps;
      if (!newparpluseps || !newparminuseps) {
        delete newparpluseps;
        delete newparminuseps;
        delete jac;
        if (tmpprevpar != prevpar) {
          delete tmpprevpar;
        }
        return 0;
      }

      for (int j = 0; j < 5; j++) {
        double diff = newparpluseps->parameters()[paraccessor.pardef[j]] -
                      newparminuseps->parameters()[paraccessor.pardef[j]];
        if (cylsurf && j == 0) {
          double length = 2 * M_PI * cylsurf->bounds().r();
          if (fabs(fabs(diff) - length) < fabs(diff)) {
            if (diff > 0) {
              diff -= length;
            } else {
              diff += length;
            }
          }
        }
        if (discsurf && j == 1) {
          if (fabs(fabs(diff) - 2 * M_PI) < fabs(diff)) {
            if (diff > 0) {
              diff -= 2 * M_PI;
            } else {
              diff += 2 * M_PI;
            }
          }
        }

        (*jac)(j, i) = diff / (2 * eps[i]);
      }

      delete newparpluseps;
      delete newparminuseps;
    }
    if (tmpprevpar != prevpar) {
      delete tmpprevpar;
    }
    return jac;
  }

  int
  GlobalChi2Fitter::iterationsOfLastFit() const {
    return m_lastiter;
  }

  void
  GlobalChi2Fitter::setMinIterations(int iter) {
    m_miniter = iter;
  }

  bool
  GlobalChi2Fitter::correctAngles(double &phi, double &theta) const {
    if (theta > M_PI) {
      theta = M_PI - theta;
      phi += M_PI;
    }
    if (theta < 0) {
      theta = -theta;
      phi += M_PI;
    }
    if (phi > M_PI) {
      phi -= 2 * M_PI;
    }
    if (phi < -M_PI) {
      phi += 2 * M_PI;
    }
    if (theta < 0 || theta > M_PI || phi < -M_PI || phi > M_PI) {
      return false;
    }
    return true;
  }

  void
  GlobalChi2Fitter::errors1(double (*jac)[5], AmgSymMatrix(5) &prevcov, AmgSymMatrix(5) &trackerrmat,
                            bool onlylocal) const {
    // propagate error from previous state to current state

    double tmp3 = 0;

    for (int l = 0; l < 4; l++) {
      for (int m = 0; m < l; m++) {
        if (!onlylocal && trackerrmat(l, m) != 0) {
          continue;
        }
        tmp3 = 0;
        for (int j = 0; j < 5; j++) {
          for (int k = 0; k < 5; k++) {
            tmp3 += jac[l][j] * prevcov(j, k) * jac[m][k];
          }
        }
        trackerrmat(l, m) = trackerrmat(m, l) = tmp3;
      }

      tmp3 = 0;
      for (int k = 0; k < 5; k++) {
        tmp3 += prevcov(4, k) * jac[l][k];
      }
      trackerrmat(4, l) = trackerrmat(l, 4) = tmp3;
      if (!onlylocal && trackerrmat(l, l) != 0) {
        continue;
      }
      tmp3 = 0;
      for (int j = 0; j < 5; j++) {
        for (int k = 0; k < j; k++) {
          tmp3 += 2 * jac[l][j] * prevcov(j, k) * jac[l][k];
        }
        tmp3 += jac[l][j] * prevcov(j, j) * jac[l][j];
      }
      trackerrmat(l, l) = tmp3;
    }
    trackerrmat(4, 4) = prevcov(4, 4);
  }

  void
  GlobalChi2Fitter::errors2(HepMatrix &derivatives, AmgSymMatrix(5) &trackerrmat, double *myarray,
                            std::vector<int> *rowindices, int &maxl, int *minm, bool onlylocal, int nfitpars) const {
    // Project global error matrix onto current state

    double tmp3 = 0;
    int j, k, rowindex;

    for (int l = 0; l <= maxl; l++) {
      for (int m = minm[l]; m < l; m++) {
        if (!onlylocal && trackerrmat(l, m) != 0) {
          continue;
        }
        tmp3 = 0;
        for (int j2 = 0; j2 < (int) rowindices[l].size(); j2++) {
          j = rowindices[l][j2];
          rowindex = j * nfitpars;
          for (int k2 = 0; k2 < (int) rowindices[m].size(); k2++) {
            k = rowindices[m][k2];
            tmp3 += derivatives[l][j] * myarray[rowindex + k] * derivatives[m][k];
          }
        }
        trackerrmat(l, m) = trackerrmat(m, l) = tmp3;
      }

      if (!onlylocal && trackerrmat(l, l) != 0) {
        continue;
      }
      tmp3 = 0;
      for (int j2 = 0; j2 < (int) rowindices[l].size(); j2++) {
        j = rowindices[l][j2];
        rowindex = j * nfitpars;
        for (int k2 = 0; k2 < j2; k2++) {
          k = rowindices[l][k2];
          tmp3 += 2 * derivatives[l][j] * myarray[rowindex + k] * derivatives[l][k];
        }
        tmp3 += derivatives[l][j] * myarray[rowindex + j] * derivatives[l][j];
      }
      trackerrmat(l, l) = tmp3;
    }
    // std::cout << " error2 " << Amg::toString(trackerrmat,10) << std::endl;
  }

  void
  GlobalChi2Fitter::cleanup() const {
    if (!m_calomeots.empty()) {
      m_calomeots.clear();
    }
    if (m_matvecmuonupstream) {
      for (int i = 0; i < (int) m_matvecmuonupstream->size(); i++) {
        delete (*m_matvecmuonupstream)[i];
      }
      delete m_matvecmuonupstream;
      m_matvecmuonupstream = 0;
    }
    if (m_matvecmuondownstream) {
      for (int i = 0; i < (int) m_matvecmuondownstream->size(); i++) {
        delete (*m_matvecmuondownstream)[i];
      }
      delete m_matvecmuondownstream;
      m_matvecmuondownstream = 0;
    }
    if (m_matvecidupstream) {
      for (int i = 0; i < (int) m_matvecidupstream->size(); i++) {
        delete (*m_matvecidupstream)[i];
      }
      delete m_matvecidupstream;
      m_matvecidupstream = 0;
    }
    if (m_matveciddownstream) {
      for (int i = 0; i < (int) m_matveciddownstream->size(); i++) {
        delete (*m_matveciddownstream)[i];
      }
      delete m_matveciddownstream;
      m_matveciddownstream = 0;
    }
    if (!m_matvec.empty()) {
      for (int j = 0; j < (int) m_matvec.size(); j++) {
        delete m_matvec[j];
      }
      m_matvec.clear();
    }
  }
}
