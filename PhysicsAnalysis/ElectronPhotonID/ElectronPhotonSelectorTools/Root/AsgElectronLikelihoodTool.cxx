/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AsgElectronLikelihoodTool
   @brief Electron selector tool to select objects in Athena using an underlying
   pure ROOT tool.
   @author Karsten Koeneke , Jovan Mitrevski
   @date   October 2012
   @update April 2014, converted to ASGTool by Jovan Mitrevski
*/

#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "EGSelectorConfigurationMapping.h"
#include "EgammaAnalysisHelpers/AsgEGammaConfigHelper.h"
#include "ElectronPhotonSelectorTools/ElectronSelectorHelpers.h"
#include "TElectronLikelihoodTool.h"
// STL includes
#include <cmath>
#include <cstdint>
#include <string>
// EDM includes
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/Vertex.h"
// Framework includes
#include "AsgDataHandles/ReadHandle.h"
#include "AsgTools/CurrentContext.h"
#include "PathResolver/PathResolver.h"
#include "TEnv.h"

// Standard constructor
AsgElectronLikelihoodTool::AsgElectronLikelihoodTool(const std::string& myname)
  : AsgTool(myname)
  , m_configFile{ "" }
  , m_rootTool{ nullptr }
{

  // Create an instance of the underlying ROOT tool
  m_rootTool = new Root::TElectronLikelihoodTool(("T" + myname).c_str());

  // Declare the needed properties
  // not having a config file results in
  // a failure
  declareProperty("WorkingPoint", m_WorkingPoint = "", "The Working Point");
  declareProperty("ConfigFile", m_configFile = "", "The config file to use");

  // pdf file name. If specified it overrides the one in the config file
  declareProperty("inputPDFFileName",
                  m_pdfFileName = "",
                  "The input ROOT file name that holds the PDFs");

  // Addtional properties that are not part of the config files
  declareProperty(
    "usePVContainer", m_usePVCont = true, "Whether to use the PV container");
  declareProperty(
    "nPVdefault", m_nPVdefault = 0, "The default number of PVs if not counted");
  declareProperty("useCaloSumsContainer",
                  m_useCaloSumsCont = true,
                  "Whether to use the CaloSums container");
  declareProperty(
    "fcalEtDefault", m_fcalEtDefault = 0, "The default FCal sum ET");
  declareProperty("skipDeltaPoverP",
                  m_skipDeltaPoverP = false,
                  "If true, it wil skip the check of deltaPoverP");
}

AsgElectronLikelihoodTool::~AsgElectronLikelihoodTool()
{
  delete m_rootTool;
}

StatusCode
AsgElectronLikelihoodTool::initialize()
{
  std::string configFile, PDFfilename, resolvedPDF; // Default

  if (!m_WorkingPoint.empty()) {
    m_configFile = AsgConfigHelper::findConfigFile(
      m_WorkingPoint, EgammaSelectors::LHPointToConfFile);
    ATH_MSG_INFO("operating point : " << this->getOperatingPointName());
  }

  if (!m_configFile.empty()) {
    configFile = PathResolverFindCalibFile(m_configFile);
    if (configFile.empty()) {
      ATH_MSG_ERROR("Could not locate config " << m_configFile);
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Configfile to use  " << m_configFile);

    TEnv env;
    if (env.ReadFile(configFile.c_str(), kEnvLocal)) {
      ATH_MSG_ERROR("Could not open config " << configFile);
      return StatusCode::FAILURE;
    }

    // Get the input PDFs for the tool.
    // We need to see if the user had provided
    // an override, if not needs to be in the input
    // config file
    if (!m_pdfFileName.empty()) {
      // If the property was set by the user, take that.
      ATH_MSG_INFO("Setting user specified PDF file " << m_pdfFileName);
      PDFfilename = m_pdfFileName;
    } else {
      if (!env.Defined("inputPDFFileName")) {
        ATH_MSG_WARNING("will use default PDF filename "
                        "since none is specified in the config "
                        << m_configFile);
      }
      PDFfilename = env.GetValue(
        "inputPDFFileName",
        "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root");
      if (PDFfilename.empty()) {
        ATH_MSG_ERROR("empty inputPDFFilename in " << configFile);
        return StatusCode::FAILURE;
      }
      if (m_configFile.find("dev/") != std::string::npos) {
        PDFfilename.insert(0, "dev/");
      }
    }

    resolvedPDF = PathResolverFindCalibFile(PDFfilename);
    if (!resolvedPDF.empty()) {
      m_rootTool->setPDFFileName(resolvedPDF.c_str());
    } else {
      ATH_MSG_ERROR("Couldn't resolve PDF filename from "
                    << PDFfilename << ", config file = " << configFile);
      return StatusCode::FAILURE;
    }

    ///-----------Begin of  text config----------------------------
    // The following are all taken from the config
    // file
    m_rootTool->m_variableNames = env.GetValue("VariableNames", "");
    m_rootTool->m_cutLikelihood =
      AsgConfigHelper::HelperDouble("CutLikelihood", env);
    m_rootTool->m_cutLikelihoodPileupCorrection =
      AsgConfigHelper::HelperDouble("CutLikelihoodPileupCorrection", env);
    m_rootTool->m_cutLikelihood4GeV =
      AsgConfigHelper::HelperDouble("CutLikelihood4GeV", env);
    m_rootTool->m_cutLikelihoodPileupCorrection4GeV =
      AsgConfigHelper::HelperDouble("CutLikelihoodPileupCorrection4GeV", env);
    // do the ambiguity cut
    m_rootTool->m_cutAmbiguity =
      AsgConfigHelper::HelperInt("CutAmbiguity", env);
    // cut on b-layer
    m_rootTool->m_cutBL = AsgConfigHelper::HelperInt("CutBL", env);
    // cut on pixel hits
    m_rootTool->m_cutPi = AsgConfigHelper::HelperInt("CutPi", env);
    // cut on precision hits
    m_rootTool->m_cutSi = AsgConfigHelper::HelperInt("CutSi", env);
    // cut on d0
    m_rootTool->m_cutA0 = AsgConfigHelper::HelperDouble("CutA0", env);
    // cut on deltaEta
    m_rootTool->m_cutDeltaEta =
      AsgConfigHelper::HelperDouble("CutDeltaEta", env);
    // cut on deltaPhiRes
    m_rootTool->m_cutDeltaPhiRes =
      AsgConfigHelper::HelperDouble("CutDeltaPhiRes", env);
    // turn off f3 at high Et
    m_rootTool->m_doRemoveF3AtHighEt =
      env.GetValue("doRemoveF3AtHighEt", false);
    // turn off TRTPID at high Et
    m_rootTool->m_doRemoveTRTPIDAtHighEt =
      env.GetValue("doRemoveTRTPIDAtHighEt", false);
    // do smooth interpolation between bins
    m_rootTool->m_doSmoothBinInterpolation =
      env.GetValue("doSmoothBinInterpolation", false);
    m_caloOnly = env.GetValue("caloOnly", false);

    m_rootTool->m_useOneExtraHighETLHBin =
      env.GetValue("useOneExtraHighETLHBin", false);
    // cut on Wstot above HighETBinThreshold
    m_rootTool->m_cutWstotAtHighET =
      AsgConfigHelper::HelperDouble("CutWstotAtHighET", env);
    // cut on EoverP above HighETBinThreshold
    m_rootTool->m_cutEoverPAtHighET =
      AsgConfigHelper::HelperDouble("CutEoverPAtHighET", env);
    m_rootTool->m_highETBinThreshold = env.GetValue("HighETBinThreshold", 125);

    m_rootTool->m_doPileupTransform = env.GetValue("doPileupTransform", false);
    m_rootTool->m_doCentralityTransform =
      env.GetValue("doCentralityTransform", false);
    m_rootTool->m_discHardCutForPileupTransform =
      AsgConfigHelper::HelperDouble("DiscHardCutForPileupTransform", env);
    m_rootTool->m_discHardCutSlopeForPileupTransform =
      AsgConfigHelper::HelperDouble("DiscHardCutSlopeForPileupTransform", env);
    m_rootTool->m_discHardCutQuadForPileupTransform =
      AsgConfigHelper::HelperDouble("DiscHardCutQuadForPileupTransform", env);
    m_rootTool->m_discLooseForPileupTransform =
      AsgConfigHelper::HelperDouble("DiscLooseForPileupTransform", env);
    m_rootTool->m_discHardCutForPileupTransform4GeV =
      AsgConfigHelper::HelperDouble("DiscHardCutForPileupTransform4GeV", env);
    m_rootTool->m_discHardCutSlopeForPileupTransform4GeV =
      AsgConfigHelper::HelperDouble("DiscHardCutSlopeForPileupTransform4GeV",
                                    env);
    m_rootTool->m_discHardCutQuadForPileupTransform4GeV =
      AsgConfigHelper::HelperDouble("DiscHardCutQuadForPileupTransform4GeV",
                                    env);
    m_rootTool->m_discLooseForPileupTransform4GeV =
      AsgConfigHelper::HelperDouble("DiscLooseForPileupTransform4GeV", env);
    m_rootTool->m_discMaxForPileupTransform =
      env.GetValue("DiscMaxForPileupTransform", 2.0);
    m_rootTool->m_pileupMaxForPileupTransform =
      env.GetValue("PileupMaxForPileupTransform", 50);
    // if true, deltaEta1 will be corrected for the pear shape distortion of the
    // LAr
    m_correctDeltaEta = env.GetValue("doCorrectDeltaEta", false);
  } else { // Error if it cant find the conf
    ATH_MSG_ERROR("Could not find configuration file");
    return StatusCode::FAILURE;
  }
  ///-----------End of text config----------------------------

  // Setup primary vertex key handle
  ATH_CHECK(m_primVtxContKey.initialize(m_usePVCont));
  // Setup HI container key handle (must come after init from env)
  bool doCentralityTransform = m_rootTool->m_doCentralityTransform;
  ATH_CHECK(
    m_HIESContKey.initialize(doCentralityTransform && m_useCaloSumsCont));

  // Get the message level and set the underlying ROOT tool message level
  // accordingly
  m_rootTool->msg().setLevel(this->msg().level());

  // We need to initialize the underlying ROOT TSelectorTool
  if (m_rootTool->initialize().isFailure()) {
    ATH_MSG_ERROR("Could not initialize the TElectronLikelihoodTool! "
                  "Configuration details: "
                  << "working point = \"" << m_WorkingPoint
                  << "\", config file = \"" << m_configFile
                  << "\", resolved file  = \"" << configFile
                  << "\", PDF file = \"" << PDFfilename
                  << "\", resolved file = \"" << resolvedPDF);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// return the accept info object
const asg::AcceptInfo&
AsgElectronLikelihoodTool::getAcceptInfo() const
{
  return m_rootTool->getAcceptInfo();
}

// The main accept method: the actual cuts are applied here
asg::AcceptData
AsgElectronLikelihoodTool::accept(const EventContext& ctx,
                                  const xAOD::Electron* el,
                                  double mu) const
{
  if (!el) {
    ATH_MSG_ERROR("Failed, no electron object.");
    return m_rootTool->accept();
  }

  const xAOD::CaloCluster* cluster = el->caloCluster();
  if (!cluster) {
    ATH_MSG_ERROR("exiting because cluster is NULL " << cluster);
    return m_rootTool->accept();
  }

  if (!cluster->hasSampling(CaloSampling::CaloSample::EMB2) &&
      !cluster->hasSampling(CaloSampling::CaloSample::EME2)) {
    ATH_MSG_ERROR("Failed, cluster is missing samplings EMB2 and EME2");
    return m_rootTool->accept();
  }

  const double energy = cluster->e();
  const float eta = (cluster->etaBE(2));

  if (isForwardElectron(el, eta)) {
    ATH_MSG_WARNING(
      "Failed, this is a forward electron! The AsgElectronLikelihoodTool is "
      "only suitable for central electrons!");
    return m_rootTool->accept();
  }

  double et = 0.;
  if (el->trackParticle() && !m_caloOnly) {
    et = (cosh(el->trackParticle()->eta()) != 0.)
           ? energy / cosh(el->trackParticle()->eta())
           : 0.;
  } else
    et = (cosh(eta) != 0.) ? energy / cosh(eta) : 0.;

  // number of track hits
  uint8_t nSiHitsPlusDeadSensors(0);
  uint8_t nPixHitsPlusDeadSensors(0);
  bool passBLayerRequirement(false);
  float d0(0.0);
  float deltaEta = 0;
  float deltaPhiRescaled2 = 0;
  float wstot = 0;
  float EoverP = 0;
  uint8_t ambiguityBit(0);
  double ip(0);

  bool allFound = true;
  std::string notFoundList = "";

  // Wstot for use when CutWstotAtHighET vector is filled
  if (!el->showerShapeValue(wstot, xAOD::EgammaParameters::wtots1)) {
    allFound = false;
    notFoundList += "wtots1 ";
  }

  // get the ambiguity type from the decoration
  if (!m_rootTool->m_cutAmbiguity.empty()) {
    if (el->isAvailable<uint8_t>("ambiguityType")) {
      static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
      ambiguityBit = acc(*el);
    } else {
      allFound = false;
      notFoundList += "ambiguityType ";
    }
  }

  if (!m_caloOnly) {
    // retrieve associated track
    const xAOD::TrackParticle* t = el->trackParticle();
    if (t) {
      nSiHitsPlusDeadSensors =
        ElectronSelectorHelpers::numberOfSiliconHitsAndDeadSensors(t);
      nPixHitsPlusDeadSensors =
        ElectronSelectorHelpers::numberOfPixelHitsAndDeadSensors(t);
      passBLayerRequirement = ElectronSelectorHelpers::passBLayerRequirement(t);
      d0 = t->d0();
      EoverP = std::abs(t->qOverP()) * energy;
    } else {
      ATH_MSG_ERROR("Failed, no track particle. et= " << et << "eta= " << eta);
      return m_rootTool->accept();
    }

    if (!el->trackCaloMatchValue(deltaEta, xAOD::EgammaParameters::deltaEta1)) {
      allFound = false;
      notFoundList += "deltaEta1 ";
    }
    // correction of deltaEta1 for pear shape distortion
    else if (m_correctDeltaEta) {
      const static SG::AuxElement::Accessor<float> acc(
        "deltaEta1PearDistortion");
      if (acc.isAvailable(*el)) {
        deltaEta -= acc(*el);
      } else {
        allFound = false;
        notFoundList += "deltaEta1PearDistortion ";
      }
    }

    if (!el->trackCaloMatchValue(deltaPhiRescaled2,
                                 xAOD::EgammaParameters::deltaPhiRescaled2)) {
      allFound = false;
      notFoundList += "deltaPhiRescaled2 ";
    }

  } // if not calo ONly

  // Get the number of primary vertices or FCal ET in this event
  bool doCentralityTransform = m_rootTool->m_doCentralityTransform;
  if (mu < 0) { // use npv if mu is negative (not given)
    if (doCentralityTransform)
      ip = static_cast<double>(m_useCaloSumsCont ? this->getFcalEt(ctx)
                                                 : m_fcalEtDefault);
    else
      ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices(ctx)
                                           : m_nPVdefault);
  } else {
    ip = mu;
  }

  // for now don't cache.
  double likelihood = calculate(ctx, el, ip);

  if (!allFound) {
    ATH_MSG_ERROR(
      "Skipping LH rectangular cuts! The following variables are missing: "
      << notFoundList);
    return m_rootTool->accept();
  }

  // Get the answer from the underlying ROOT tool
  return m_rootTool->accept(likelihood,
                            eta,
                            et,
                            nSiHitsPlusDeadSensors,
                            nPixHitsPlusDeadSensors,
                            passBLayerRequirement,
                            ambiguityBit,
                            d0,
                            deltaEta,
                            deltaPhiRescaled2,
                            wstot,
                            EoverP,
                            ip);
}

// Accept method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
asg::AcceptData
AsgElectronLikelihoodTool::accept(const EventContext& ctx,
                                  const xAOD::Egamma* eg,
                                  double mu) const
{
  if (!eg) {
    ATH_MSG_ERROR("Failed, no egamma object.");
    return m_rootTool->accept();
  }
  // Call the main accept if this is not a calo-only LH
  if (!m_caloOnly) {
    if (eg->type() == xAOD::Type::Electron) {
      const xAOD::Electron* el = static_cast<const xAOD::Electron*>(eg);
      return accept(ctx, el, mu);
    }
    ATH_MSG_ERROR("Input is not an electron and not caloOnly is set");
    return m_rootTool->accept();
  }

  // Calo only LH
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if (!cluster) {
    ATH_MSG_ERROR("Failed, no cluster.");
    return m_rootTool->accept();
  }
  if (!cluster->hasSampling(CaloSampling::CaloSample::EMB2) &&
      !cluster->hasSampling(CaloSampling::CaloSample::EME2)) {
    ATH_MSG_ERROR("Failed, cluster is missing samplings EMB2 and EME2");
    return m_rootTool->accept();
  }

  const double energy = cluster->e();
  const float eta = (cluster->etaBE(2));
  if (isForwardElectron(eg, eta)) {
    ATH_MSG_WARNING(
      "Failed, this is a forward electron! The AsgElectronLikelihoodTool is "
      "only suitable for central electrons!");
    return m_rootTool->accept();
  }

  const double et = (cosh(eta) != 0.) ? energy / cosh(eta) : 0.;

  // Variables the EFCaloLH ignores
  uint8_t nSiHitsPlusDeadSensors(0);
  uint8_t nPixHitsPlusDeadSensors(0);
  bool passBLayerRequirement(false);
  uint8_t ambiguityBit(0);

  // Get the pileup or centrality information
  double ip(0);

  bool doCentralityTransform = m_rootTool->m_doCentralityTransform;
  if (mu < 0) { // use npv if mu is negative (not given)
    if (doCentralityTransform)
      ip = static_cast<double>(m_useCaloSumsCont ? this->getFcalEt(ctx)
                                                 : m_fcalEtDefault);
    else
      ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices(ctx)
                                           : m_nPVdefault);

  } else {
    ip = mu;
  }
  // for now don't cache.
  double likelihood = calculate(ctx, eg, ip);

  double deltaEta = 0;
  double deltaPhiRescaled2 = 0;
  double d0 = 0;
  float wstot = 0;
  float EoverP = 0;

  bool allFound = true;
  std::string notFoundList = "";

  // Wstot for use when CutWstotAtHighET vector is filled
  if (!eg->showerShapeValue(wstot, xAOD::EgammaParameters::wtots1)) {
    allFound = false;
    notFoundList += "wtots1 ";
  }

  if (!allFound) {
    ATH_MSG_ERROR(
      "Skipping LH rectangular cuts! The following variables are missing: "
      << notFoundList);
    return m_rootTool->accept();
  }

  // Get the answer from the underlying ROOT tool
  return m_rootTool->accept(likelihood,
                            eta,
                            et,
                            nSiHitsPlusDeadSensors,
                            nPixHitsPlusDeadSensors,
                            passBLayerRequirement,
                            ambiguityBit,
                            d0,
                            deltaEta,
                            deltaPhiRescaled2,
                            wstot,
                            EoverP,
                            ip);
}

// The main result method: the actual likelihood is calculated here
double
AsgElectronLikelihoodTool::calculate(const EventContext& ctx,
                                     const xAOD::Electron* el,
                                     double mu) const
{
  if (!el) {
    ATH_MSG_ERROR("Failed, no egamma object.");
    return -999;
  }
  const xAOD::CaloCluster* cluster = el->caloCluster();
  if (!cluster) {
    ATH_MSG_ERROR("Failed, no cluster.");
    return -999;
  }
  if (!cluster->hasSampling(CaloSampling::CaloSample::EMB2) &&
      !cluster->hasSampling(CaloSampling::CaloSample::EME2)) {
    ATH_MSG_ERROR("Failed, cluster is missing samplings EMB2 and EME2");
    return -999;
  }

  const double energy = cluster->e();
  const float eta = cluster->etaBE(2);

  if (isForwardElectron(el, eta)) {
    ATH_MSG_WARNING(
      "Failed, this is a forward electron! The AsgElectronLikelihoodTool is "
      "only suitable for central electrons!");
    return -999;
  }

  double et = 0.;
  if (el->trackParticle() && !m_caloOnly) {
    et = (cosh(el->trackParticle()->eta()) != 0.)
           ? energy / cosh(el->trackParticle()->eta())
           : 0.;
  } else {
    et = (cosh(eta) != 0.) ? energy / cosh(eta) : 0.;
  }

  // number of track hits and other track quantities
  float trackqoverp(0.0);
  float d0(0.0);
  float d0sigma(0.0);
  double dpOverp(0.0);
  float TRT_PID(0.0);
  double trans_TRT_PID(0.0);
  float deltaEta = 0;
  float deltaPhiRescaled2 = 0;

  bool allFound = true;
  std::string notFoundList = "";

  if (!m_caloOnly) {
    // retrieve associated TrackParticle
    const xAOD::TrackParticle* t = el->trackParticle();
    if (t) {
      trackqoverp = t->qOverP();
      d0 = t->d0();
      float vard0 = t->definingParametersCovMatrix()(0, 0);
      if (vard0 > 0) {
        d0sigma = sqrtf(vard0);
      }
      if (!t->summaryValue(TRT_PID, xAOD::eProbabilityHT)) {
        allFound = false;
        notFoundList += "eProbabilityHT ";
      }

      // Transform the TRT PID output for use in the LH tool.
      double tau = 15.0;
      double fEpsilon = 1.0e-30; // to avoid zero division
      double pid_tmp = TRT_PID;
      if (pid_tmp >= 1.0)
        pid_tmp = 1.0 - 1.0e-15; // this number comes from TMVA
      else if (pid_tmp <= fEpsilon)
        pid_tmp = fEpsilon;
      trans_TRT_PID = -log(1.0 / pid_tmp - 1.0) * (1. / double(tau));

      unsigned int index;
      if (t->indexOfParameterAtPosition(index, xAOD::LastMeasurement)) {

        double refittedTrack_LMqoverp =
          t->charge() / sqrt(std::pow(t->parameterPX(index), 2) +
                             std::pow(t->parameterPY(index), 2) +
                             std::pow(t->parameterPZ(index), 2));

        dpOverp = 1 - trackqoverp / (refittedTrack_LMqoverp);
      } else if (!m_skipDeltaPoverP) {
        allFound = false;
        notFoundList += "deltaPoverP ";
      }

    } else {
      ATH_MSG_ERROR("Failed, no track particle. et= " << et << "eta= " << eta);
      return -999;
    }
  } // if not calo Only

  float Reta(0);
  float Rphi(0);
  float Rhad1(0);
  float Rhad(0);
  float w2(0);
  float f1(0);
  float Eratio(0);
  float f3(0);

  // reta = e237/e277
  if (!el->showerShapeValue(Reta, xAOD::EgammaParameters::Reta)) {
    allFound = false;
    notFoundList += "Reta ";
  }
  // rphi e233/e237
  if (!el->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi)) {
    allFound = false;
    notFoundList += "Rphi ";
  }
  // rhad1 = ethad1/et
  if (!el->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1)) {
    allFound = false;
    notFoundList += "Rhad1 ";
  }
  // rhad = ethad/et
  if (!el->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad)) {
    allFound = false;
    notFoundList += "Rhad ";
  }
  // shower width in 2nd sampling
  if (!el->showerShapeValue(w2, xAOD::EgammaParameters::weta2)) {
    allFound = false;
    notFoundList += "weta2 ";
  }
  // fraction of energy reconstructed in the 1st sampling
  if (!el->showerShapeValue(f1, xAOD::EgammaParameters::f1)) {
    allFound = false;
    notFoundList += "f1 ";
  }
  // E of 2nd max between max and min in strips
  if (!el->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio)) {
    allFound = false;
    notFoundList += "Eratio ";
  }
  // fraction of energy reconstructed in the 3rd sampling
  if (!el->showerShapeValue(f3, xAOD::EgammaParameters::f3)) {
    allFound = false;
    notFoundList += "f3 ";
  }

  if (!m_caloOnly) {
    // deltaEta1
    if (!el->trackCaloMatchValue(deltaEta, xAOD::EgammaParameters::deltaEta1)) {
      allFound = false;
      notFoundList += "deltaEta1 ";
    }
    // correction of deltaEta1 for pear shape distortion
    else if (m_correctDeltaEta) {
      const static SG::AuxElement::Accessor<float> acc(
        "deltaEta1PearDistortion");
      if (acc.isAvailable(*el)) {
        deltaEta -= acc(*el);
      } else {
        allFound = false;
        notFoundList += "deltaEta1PearDistortion ";
      }
    }

    // difference between the cluster phi (sampling 2) and the eta of the track
    // extrapolated from the last measurement point.
    if (!el->trackCaloMatchValue(deltaPhiRescaled2,
                                 xAOD::EgammaParameters::deltaPhiRescaled2)) {
      allFound = false;
      notFoundList += "deltaPhiRescaled2 ";
    }
  }

  // Get the number of primary vertices or FCal ET in this event
  double ip = static_cast<double>(m_nPVdefault);

  bool doCentralityTransform = m_rootTool->m_doCentralityTransform;
  if (mu < 0) { // use npv if mu is negative (not given)
    if (doCentralityTransform)
      ip = static_cast<double>(m_useCaloSumsCont ? this->getFcalEt(ctx)
                                                 : m_fcalEtDefault);
    else
      ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices(ctx)
                                           : m_nPVdefault);
  } else {
    ip = mu;
  }

  if (!allFound) {
    ATH_MSG_ERROR(
      "Skipping LH calculation! The following variables are missing: "
      << notFoundList);
    return -999;
  }

  // Get the answer from the underlying ROOT tool
  return m_rootTool->calculate(eta,
                               et,
                               f3,
                               Rhad,
                               Rhad1,
                               Reta,
                               w2,
                               f1,
                               Eratio,
                               deltaEta,
                               d0,
                               d0sigma,
                               Rphi,
                               dpOverp,
                               deltaPhiRescaled2,
                               trans_TRT_PID,
                               ip);
}

// Calculate method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
double
AsgElectronLikelihoodTool::calculate(const EventContext& ctx,
                                     const xAOD::Egamma* eg,
                                     double mu) const
{
  if (!eg) {
    ATH_MSG_ERROR("Failed, no egamma object.");
    return -999;
  }

  if (!m_caloOnly) {
    if (eg->type() == xAOD::Type::Electron) {
      const xAOD::Electron* el = static_cast<const xAOD::Electron*>(eg);
      return calculate(ctx, el);
    }

    ATH_MSG_ERROR("Input is not an electron and not Calo Only is required");
    return -999;
  }

  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if (!cluster) {
    ATH_MSG_ERROR("Failed, no cluster.");
    return -999;
  }

  if (!cluster->hasSampling(CaloSampling::CaloSample::EMB2) &&
      !cluster->hasSampling(CaloSampling::CaloSample::EME2)) {
    ATH_MSG_ERROR("Failed, cluster is missing samplings EMB2 and EME2");
    return -999;
  }

  const double energy = cluster->e();
  const float eta = cluster->etaBE(2);

  if (isForwardElectron(eg, eta)) {
    ATH_MSG_WARNING(
      "Failed, this is a forward electron! The AsgElectronLikelihoodTool is "
      "only suitable for central electrons!");
    return -999;
  }

  const double et = (cosh(eta) != 0.) ? energy / cosh(eta) : 0.;

  // Track variables that the EFCaloLH will not use
  float d0(0.0);
  float d0sigma(0.0);
  double dpOverp(0.0);

  float deltaEta = 0;
  float deltaPhiRescaled2 = 0;
  float TRT_PID(0.0);

  // Calo Variables
  float Reta(0);
  float Rphi(0);
  float Rhad1(0);
  float Rhad(0);
  float w2(0);
  float f1(0);
  float Eratio(0);
  float f3(0);

  bool allFound = true;
  std::string notFoundList = "";

  // reta = e237/e277
  if (!eg->showerShapeValue(Reta, xAOD::EgammaParameters::Reta)) {
    allFound = false;
    notFoundList += "Reta ";
  }
  // rphi e233/e237
  if (!eg->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi)) {
    allFound = false;
    notFoundList += "Rphi ";
  }
  // rhad1 = ethad1/et
  if (!eg->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1)) {
    allFound = false;
    notFoundList += "Rhad1 ";
  }
  // rhad = ethad/et
  if (!eg->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad)) {
    allFound = false;
    notFoundList += "Rhad ";
  }
  // shower width in 2nd sampling
  if (!eg->showerShapeValue(w2, xAOD::EgammaParameters::weta2)) {
    allFound = false;
    notFoundList += "weta2 ";
  }
  // fraction of energy reconstructed in the 1st sampling
  if (!eg->showerShapeValue(f1, xAOD::EgammaParameters::f1)) {
    allFound = false;
    notFoundList += "f1 ";
  }
  // E of 2nd max between max and min in strips
  if (!eg->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio)) {
    allFound = false;
    notFoundList += "Eratio ";
  }
  // fraction of energy reconstructed in the 3rd sampling
  if (!eg->showerShapeValue(f3, xAOD::EgammaParameters::f3)) {
    allFound = false;
    notFoundList += "f3 ";
  }

  // Get the pileup or centrality information
  double ip(0);

  bool doCentralityTransform = m_rootTool->m_doCentralityTransform;
  if (mu < 0) { // use npv if mu is negative (not given)
    if (doCentralityTransform)
      ip = static_cast<double>(m_useCaloSumsCont ? this->getFcalEt(ctx)
                                                 : m_fcalEtDefault);
    else
      ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices(ctx)
                                           : m_nPVdefault);
  } else {
    ip = mu;
  }

  if (!allFound) {
    ATH_MSG_ERROR(
      "Skipping LH calculation! The following variables are missing: "
      << notFoundList);
    return -999;
  }

  // Get the answer from the underlying ROOT tool
  return m_rootTool->calculate(eta,
                               et,
                               f3,
                               Rhad,
                               Rhad1,
                               Reta,
                               w2,
                               f1,
                               Eratio,
                               deltaEta,
                               d0,
                               d0sigma,
                               Rphi,
                               dpOverp,
                               deltaPhiRescaled2,
                               TRT_PID,
                               ip);
}

// Get the name of the current operating point
std::string
AsgElectronLikelihoodTool::getOperatingPointName() const
{
  return m_WorkingPoint;
}

// aceept with IParticle and no EventContext, backwards compatibility
asg::AcceptData
AsgElectronLikelihoodTool::accept(const xAOD::IParticle* part) const
{
  // Backwards compatibility
  return accept(Gaudi::Hive::currentContext(), part);
}

asg::AcceptData
AsgElectronLikelihoodTool::accept(const EventContext& ctx,
                                  const xAOD::IParticle* part) const
{
  if (part->type() == xAOD::Type::Electron) {
    const xAOD::Electron* el = static_cast<const xAOD::Electron*>(part);
    return accept(ctx, el);
  }
  ATH_MSG_ERROR("Input is not an electron");
  return m_rootTool->accept();
}

double
AsgElectronLikelihoodTool::calculate(const EventContext& ctx,
                                     const xAOD::IParticle* part) const
{
  if (part->type() == xAOD::Type::Electron) {
    const xAOD::Electron* el = static_cast<const xAOD::Electron*>(part);
    return calculate(ctx, el);
  }

  ATH_MSG_ERROR("Input is not an electron");
  return -999;
}

// Helper method to get the number of primary vertices
// We don't want to iterate over all vertices in the event for each electron!!!
unsigned int
AsgElectronLikelihoodTool::getNPrimVertices(const EventContext& ctx) const
{
  unsigned int nVtx(0);
  SG::ReadHandle<xAOD::VertexContainer> vtxCont(m_primVtxContKey, ctx);
  for (unsigned int i = 0; i < vtxCont->size(); i++) {
    const xAOD::Vertex* vxcand = vtxCont->at(i);
    if (vxcand->nTrackParticles() >= 2)
      nVtx++;
  }
  return nVtx;
}

// Helper method to get FCal ET for centrality determination
double
AsgElectronLikelihoodTool::getFcalEt(const EventContext& ctx) const
{
  double fcalEt(0.);
  SG::ReadHandle<xAOD::HIEventShapeContainer> HIESCont(m_HIESContKey, ctx);
  xAOD::HIEventShapeContainer::const_iterator es_itr = HIESCont->begin();
  xAOD::HIEventShapeContainer::const_iterator es_end = HIESCont->end();
  for (; es_itr != es_end; ++es_itr) {
    double et = (*es_itr)->et();
    const std::string name = (*es_itr)->auxdataConst<std::string>("Summary");
    if (name == "FCal")
      fcalEt = et * 1.e-6;
  }
  return fcalEt;
}

bool
AsgElectronLikelihoodTool::isForwardElectron(const xAOD::Egamma* eg,
                                             const float eta) const
{

  static const SG::AuxElement::ConstAccessor<uint16_t> accAuthor("author");

  if (accAuthor.isAvailable(*eg)) {

    // cannot just do eg->author() because it isn't always filled
    // at trigger level
    if (accAuthor(*eg) == xAOD::EgammaParameters::AuthorFwdElectron) {
      ATH_MSG_WARNING(
        "Failed, this is a forward electron! The AsgElectronLikelihoodTool is "
        "only suitable for central electrons!");
      return true;
    }
  } else {
    // Check for fwd via eta range the old logic
    if (std::abs(eta) > 2.5) {
      ATH_MSG_WARNING("Failed, cluster->etaBE(2) range due to "
                      << eta << " seems like a fwd electron");
      return true;
    }
  }

  return false;
}
