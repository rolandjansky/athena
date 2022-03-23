/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AsgElectronChargeIDSelectorTool
   @brief Electron selector tool to select objects in Asgena using an underlying
   pure ROOT tool.

   @author Karsten Koeneke
   @date   October 2012
   09-APR-2014, convert to ASGTool (Jovan Mitrevski)
   22-AUG-2016, copied from AsgElectronLikelihoodTool (Kazuya Mochizuki)
*/

// Include this class's header
#include "ElectronPhotonSelectorTools/AsgElectronChargeIDSelectorTool.h"

// STL includes
#include <cmath>
#include <cstdint>
#include <string>
#include <sstream>

// EDM includes
#include "TClass.h"
#include "TEnv.h"
#include "TFile.h"
#include "TKey.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TROOT.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/Electron.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include "AsgDataHandles/ReadHandle.h"
#include "AsgTools/CurrentContext.h"
#include "PathResolver/PathResolver.h"

//=============================================================================
// Standard constructor
//=============================================================================
AsgElectronChargeIDSelectorTool::AsgElectronChargeIDSelectorTool(
  const std::string& myname)
  : AsgTool(myname) //,m_cutOnBDT(0)//,m_configFile("")//,m_rootTool(0)
{
  // Declare the needed properties
  declareProperty("WorkingPoint", m_WorkingPoint = "", "The Working Point");
  // declareProperty("ConfigFile",m_configFile="","The config file to use");
  declareProperty(
    "usePVContainer", m_usePVCont = true, "Whether to use the PV container");
  declareProperty(
    "nPVdefault", m_nPVdefault = 0, "The default number of PVs if not counted");
  // declareProperty("primaryVertexContainer",
  // m_primVtxContKey="PrimaryVertices", "The primary vertex container name" );

  declareProperty("TrainingFile",
                  m_trainingFile = "",
                  "The input ROOT file name holding training");
  declareProperty("CutOnBDT", m_cutOnBDT = 0, "Cut on BDT discriminant");
  m_pid_name = myname.data();
}

//=============================================================================
// Standard destructor
//=============================================================================
AsgElectronChargeIDSelectorTool::~AsgElectronChargeIDSelectorTool()
{
  for (auto* bdt : m_v_bdts)
    if (bdt)
      delete bdt;
}

//=============================================================================
// Asgena initialize method
//=============================================================================
StatusCode
AsgElectronChargeIDSelectorTool::initialize()
{
  m_pid_name.ToLower(); // KM: List of 97% OPs with different PIDs below
  std::string op_name = "loose";
  bool op_isUserSpecified = false;
  if (m_cutOnBDT == 0) { // when cutOnBDT is unmodified, adjust it to the 97% OP
                         // in each PID menu
    if (m_pid_name.Contains("tight"))
      op_name = "tight", m_cutOnBDT = -0.109249; // Tight  (with data):
                                                 // -0.109249
    else if (m_pid_name.Contains("medium"))
      op_name = "medium",
      m_cutOnBDT = -0.257081; // Medium (with data): -0.257081
    else
      m_cutOnBDT = -0.337671; // Loose  (with data): -0.337671
  } else
    op_isUserSpecified = true;
  m_pid_name = "loose"; // Now only one training is provided, using loose PID
                        // but OP varies for differnt PID

  std::string display =
    op_isUserSpecified ? "user specified" : "97% signal-eff";
  ATH_MSG_INFO("OP to use: " << op_name << ", with cut on BDT: " << m_cutOnBDT
                             << ", which corresponds to " << display
                             << " working point.");

  std::string TrainingFile;
  if (!m_trainingFile
         .empty()) { // If the property was set by the user, take that.

    TrainingFile = PathResolverFindCalibFile(m_trainingFile);
    if (TrainingFile.empty()) { // Error if it cant find the conf
      ATH_MSG_ERROR("Could not locate " << m_trainingFile);
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO("trainingfile loaded from: " << TrainingFile);

  } else {
    ATH_MSG_ERROR("Could not find configuration file: \"" << m_trainingFile
                                                          << "\"");
    return StatusCode::FAILURE;
  }

  unsigned nfold = 1;
  std::unique_ptr<TFile> bdtfile(TFile::Open(TrainingFile.data(),"READ"));
  if (!bdtfile) {
    ATH_MSG_ERROR("Input file found to be empty!! " << TrainingFile);
    return StatusCode::FAILURE;
  } else {
    TIter next(bdtfile->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)next())) {
      TClass* clas = gROOT->GetClass(key->GetClassName());
      if (!clas->InheritsFrom("TDirectoryFile"))
        continue;
      TDirectory* td = (TDirectoryFile*)key->ReadObj();
      std::string dirName = td->GetName();
      if (dirName.find(m_pid_name) != std::string::npos) {
        std::string foldconf = dirName.substr(dirName.rfind('_') + 1);
        std::string s_nfold = foldconf.substr(foldconf.find('o') + 1);
        nfold = atoi(s_nfold.data());
        break;
      }
    }
  }
  ATH_MSG_INFO("ECIDS nfold configuration: " << nfold);

  m_inputVars.clear();
  const TString toaPath = Form("ECIDS_%s_0o%d/variables",m_pid_name.Data(),nfold);
  const TObjArray* toa = bdtfile->Get<TObjArray>(toaPath);
  if (toa) {
    const TObjString* tos = dynamic_cast<const TObjString*>(toa->At(0));
    if (tos) {
      std::istringstream commaSepVars(tos->GetString().Data());
      ATH_MSG_INFO("Variables for ECIDS " << commaSepVars.str());
      std::string inVar;
      while (std::getline(commaSepVars,inVar,',')) {
	m_inputVars.push_back(inVar);
	ATH_MSG_VERBOSE("Variables for ECIDS (check) " << inVar);
      }
    }
  }
  if (!m_inputVars.size()) {
    ATH_MSG_FATAL("Cannot access the list of input variables @"
                  << bdtfile->GetName() << " " << toaPath);
  }

  for (unsigned i_fold = 0; i_fold < nfold; i_fold++) {
    TString treename = Form("ECIDS_%s_%do%d/BDT",m_pid_name.Data(),i_fold,nfold);
    TTree* tree = (TTree*)bdtfile->Get(treename);
    ATH_MSG_VERBOSE("Trying to access a ttree with name: " << treename << " " << tree);
    m_v_bdts.push_back(new MVAUtils::BDT(tree));
  }

  ///-----------End of text config----------------------------

  // Setup primary vertex key handle
  ATH_CHECK(m_primVtxContKey.initialize(m_usePVCont));

  m_cutPosition_bdt = m_acceptInfo.addCut("bdt", "pass bdt");

  return StatusCode::SUCCESS;
}

//=============================================================================
// The main accept method: the actual cuts are applied here
//=============================================================================
asg::AcceptData
AsgElectronChargeIDSelectorTool::accept(const EventContext& ctx,
                                        const xAOD::Electron* eg,
                                        double mu) const
{

  double bdt = calculate(ctx, eg, mu);

  ATH_MSG_VERBOSE("\t accept( ctx, el, mu ), bdt=" << bdt);

  asg::AcceptData acceptBDT(&m_acceptInfo);
  acceptBDT.clear();

  acceptBDT.setCutResult(m_cutPosition_bdt, bdt > m_cutOnBDT);

  return acceptBDT;
}

//=============================================================================
// Accept method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
//=============================================================================
asg::AcceptData
AsgElectronChargeIDSelectorTool::accept(const EventContext& ctx,
                                        const xAOD::Egamma* eg,
                                        double mu) const
{
  double bdt = calculate(ctx, eg, mu);

  ATH_MSG_VERBOSE("\t accept( ctx, eg, mu ), bdt=" << bdt);

  asg::AcceptData acceptBDT(&m_acceptInfo);
  acceptBDT.clear();

  acceptBDT.setCutResult(m_cutPosition_bdt, bdt > m_cutOnBDT);

  return acceptBDT;
}

//=============================================================================
// The main result method: the actual likelihood is calculated here
//=============================================================================
double
AsgElectronChargeIDSelectorTool::calculate(const EventContext& ctx,
                                           const xAOD::Electron* eg,
                                           double mu) const
{

  ATH_MSG_VERBOSE(
    "\t AsgElectronChargeIDSelectorTool::calculate( &ctx, *eg, mu= "
    << (&ctx) << ", " << eg << ", " << mu << " )");

  if (!eg) {
    ATH_MSG_ERROR("Failed, no egamma object.");
    return -1;
  }

  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if (!cluster) {
    ATH_MSG_ERROR("Failed, no cluster.");
    return -1;
  }

  const double energy = cluster->e();
  const float eta = cluster->etaBE(2);
  if (fabs(eta) > 300.0) {
    ATH_MSG_ERROR("Failed, eta range.");
    return -1;
  }

  double et = 0.; // transverse energy of the electron (using the track eta)
  if (eg->trackParticle())
    et = (cosh(eg->trackParticle()->eta()) != 0.)
           ? energy / cosh(eg->trackParticle()->eta())
           : 0.;
  else
    et = (cosh(eta) != 0.) ? energy / cosh(eta) : 0.;

  // number of track hits and other track quantities
  uint8_t nSCT(0);
  float trackqoverp(0.0);
  float trackqoverpsig(0.0);
  int charge(0.0);
  int lifeSign(0.0);
  float trackchi2(0.0);
  float avgCharge_SCTw(0.0);
  float d0(0.0);
  float z0(0.0);
  float phi0(0.0);
  float theta(0.0);
  float EoverP(0.0);
  float d0sigma(0.0);
  double dpOverp(0.0);
  float TRT_PID(0.0);
  // double trans_TRT_PID(0.0);
  float deltaPhi1 = 0, deltaPhi2 = 0;
  float deltaPhiFromLM = 0;
  float deltaPhiRescaled2 = 0; // deltaEta=0,
  // double rTRT(0.0);

  TVector2 el_cluster;
  el_cluster.SetMagPhi(cluster->energyBE(2) / cosh(eta), cluster->phiBE(2));

  bool allFound = true;
  // retrieve associated TrackParticle
  const xAOD::TrackParticle* t = eg->trackParticle();
  if (t) {
    trackqoverp = t->qOverP();
    charge = t->charge();
    d0 = t->d0();

    if (std::find(m_inputVars.begin(), m_inputVars.end(), "z0sinTheta") !=
        m_inputVars.end()) {
      z0 = t->z0();
      theta = t->theta();
    }

    if (std::find(m_inputVars.begin(), m_inputVars.end(), "chi2oftrackfit") !=
        m_inputVars.end())
      trackchi2 = t->chiSquared();

    phi0 = t->phi() + (d0 >= 0 ? M_PI / 2 : -M_PI / 2);
    TVector2 d0_direction;
    d0_direction.SetMagPhi(fabs(d0), phi0);
    float inner_product =
      el_cluster.X() * d0_direction.X() + el_cluster.Y() * d0_direction.Y();
    lifeSign = inner_product >= 0 ? 1 : -1;

    EoverP = energy * fabs(t->qOverP());
    if (std::find(m_inputVars.begin(), m_inputVars.end(), "d0Err") !=
          m_inputVars.end() or
        std::find(m_inputVars.begin(), m_inputVars.end(), "d0Sig") !=
          m_inputVars.end()) {
      float vard0 = t->definingParametersCovMatrix()(0, 0);
      if (vard0 > 0) {
        d0sigma = sqrtf(vard0);
      }
    }

    // KM: calculation of SCT-weighted charge
    float charge = 0, SCT = 0;
    for (unsigned TPit = 0; TPit < eg->nTrackParticles(); TPit++) {
      uint8_t temp_NSCTHits;
      if (eg->trackParticle(TPit)) {
        eg->trackParticle(TPit)->summaryValue(temp_NSCTHits,
                                              xAOD::numberOfSCTHits);

        SCT += temp_NSCTHits;
        charge += temp_NSCTHits * eg->trackParticle(TPit)->charge();
      } else
        ATH_MSG_WARNING("This electron has no track particle associated!!! "
                        "Assigning #SCT-hits= 0!!! ");
    }
    avgCharge_SCTw = SCT != 0 ? eg->charge() * charge / SCT : 0;

    const std::vector<float>& cov = t->definingParametersCovMatrixVec();
    trackqoverpsig = cov[14];

    if (std::find(m_inputVars.begin(), m_inputVars.end(), "nSctHits") !=
        m_inputVars.end())
      allFound = allFound && t->summaryValue(nSCT, xAOD::numberOfSCTHits);

    // Transform the TRT PID output for use in the LH tool.
    double fEpsilon = 1.0e-30; // to avoid zero division
    double pid_tmp = TRT_PID;
    if (pid_tmp >= 1.0)
      pid_tmp = 1.0 - 1.0e-15; // this number comes from TMVA
    else if (pid_tmp <= fEpsilon)
      pid_tmp = fEpsilon;

    if (std::find(m_inputVars.begin(), m_inputVars.end(), "deltaPoverP") !=
        m_inputVars.end()) {
      unsigned int index;
      if (t->indexOfParameterAtPosition(index, xAOD::LastMeasurement)) {

        double refittedTrack_LMqoverp =
          t->charge() / sqrt(std::pow(t->parameterPX(index), 2) +
                             std::pow(t->parameterPY(index), 2) +
                             std::pow(t->parameterPZ(index), 2));

        dpOverp = 1 - trackqoverp / (refittedTrack_LMqoverp);
      }
    }

  } else {
    allFound = false;
    ATH_MSG_WARNING("Failed, no track particle: et= " << et << "eta= " << eta);
  }

  float Rphi(0); // float Reta(0), Rphi(0),  Rhad1(0), Rhad(0), ws3(0), w2(0),
                 // f1(0), Eratio(0), f3(0);
  allFound =
    allFound &&
    eg->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi); // rphi e233/e237
  // allFound = allFound && eg->trackCaloMatchValue(deltaEta,
  // xAOD::EgammaParameters::deltaEta1);

  // difference between the cluster phi (sampling 2) and the eta of the track
  // extrapolated from the last measurement point.
  allFound = allFound &&
             eg->trackCaloMatchValue(deltaPhiRescaled2,
                                     xAOD::EgammaParameters::deltaPhiRescaled2);

  // if(m_map_inputs.find("deltaphi1"              )!= m_map_inputs.end())
  if (std::find(m_inputVars.begin(), m_inputVars.end(), "deltaphi1") !=
      m_inputVars.end())
    allFound = allFound && eg->trackCaloMatchValue(
                             deltaPhi1, xAOD::EgammaParameters::deltaPhi1);
  // if(m_map_inputs.find("deltaphi2"              )!= m_map_inputs.end() or
  //    m_map_inputs.find("deltaDeltaPhiFirstAndLM")!= m_map_inputs.end())
  if (std::find(m_inputVars.begin(), m_inputVars.end(), "deltaphi2") !=
        m_inputVars.end() or
      std::find(m_inputVars.begin(),
                m_inputVars.end(),
                "deltaDeltaPhiFirstAndLM") != m_inputVars.end())
    allFound = allFound && eg->trackCaloMatchValue(
                             deltaPhi2, xAOD::EgammaParameters::deltaPhi2);
  // if(m_map_inputs.find("deltaDeltaPhiFirstAndLM")!= m_map_inputs.end())
  if (std::find(m_inputVars.begin(),
                m_inputVars.end(),
                "deltaDeltaPhiFirstAndLM") != m_inputVars.end())
    allFound =
      allFound &&
      eg->trackCaloMatchValue(
        deltaPhiFromLM, xAOD::EgammaParameters::deltaPhiFromLastMeasurement);

  // Get the number of primary vertices in this event
  // double ip = static_cast<double>(m_nPVdefault);
  // if(mu < 0) // use npv if mu is negative (not given)
  //   ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() :
  //   m_nPVdefault);
  // else ip = mu;

  if (!allFound)
    ATH_MSG_FATAL("Missing input variable for ECIDS BDT calculation");

  const xAOD::EventInfo* eventInfo = nullptr;
  if (evtStore()->retrieve(eventInfo, "EventInfo").isFailure())
    ATH_MSG_WARNING(" Cannot access to event info ");
  // lumiBlock = eventInfo->lumiBlock(), runNumber = eventInfo->runNumber(),
  // eventNumber=eventInfo->eventNumber();
  // ATH_MSG_DEBUG("event_num%bdt_size="<<eventInfo->eventNumber()<<"%"<<unsigned(m_v_bdts.size())<<"=
  // "<<eventInfo->eventNumber()%unsigned(m_v_bdts.size()));
  unsigned bdt_index = eventInfo->eventNumber() % unsigned(m_v_bdts.size());

  std::vector<float> v_inputs;
  for (const auto& var : m_inputVars) {
    if (var == "pt")
      v_inputs.push_back(et);
    if (var == "eta")
      v_inputs.push_back(eta);
    if (var == "abs_eta")
      v_inputs.push_back(fabs(eta));
    if (var == "avgCharge_SCTw")
      v_inputs.push_back(avgCharge_SCTw);
    if (var == "d0")
      v_inputs.push_back(d0);
    if (var == "ld0")
      v_inputs.push_back(lifeSign * d0);
    if (var == "cd0")
      v_inputs.push_back(charge * d0);
    if (var == "EoverP")
      v_inputs.push_back(EoverP);
    if (var == "deltaphi1")
      v_inputs.push_back(deltaPhi1);
    if (var == "deltaphiRes")
      v_inputs.push_back(deltaPhiRescaled2);
    if (var == "Rphi")
      v_inputs.push_back(Rphi);
    if (var == "qoverpSig")
      v_inputs.push_back(trackqoverpsig);
    if (var == "nSctHits")
      v_inputs.push_back(nSCT);
    if (var == "z0sinTheta")
      v_inputs.push_back(z0 * sin(theta));
    if (var == "d0Err")
      v_inputs.push_back(d0sigma);
    if (var == "d0Sig")
      v_inputs.push_back(d0 / d0sigma);
    if (var == "deltaphi2")
      v_inputs.push_back(deltaPhi2);
    if (var == "chi2oftrackfit")
      v_inputs.push_back(trackchi2);
    if (var == "deltaPoverP")
      v_inputs.push_back(dpOverp);
    if (var == "deltaDeltaPhiFirstAndLM")
      v_inputs.push_back(deltaPhi2 - deltaPhiFromLM);
  }

  ATH_MSG_VERBOSE(
    "\t\t event# "
    << eventInfo->eventNumber() << std::endl
    << "xAOD variables: pt                           = " << et
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "pt") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: eta                     = " << eta
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "eta") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: abs_eta                 = " << fabs(eta)
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "abs_eta") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: avgCharge_SCTw          = " << avgCharge_SCTw
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "avgCharge_SCTw") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: d0                      = " << d0
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "d0") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: ld0                     = " << lifeSign * d0
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "ld0") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: cd0                     = " << charge * d0
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "cd0") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: EoverP                  = " << EoverP
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "EoverP") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: deltaphi1               = " << deltaPhi1
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "deltaphi1") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: deltaphiRes             = " << deltaPhiRescaled2
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "deltaphiRes") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: Rphi                    = " << Rphi
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "Rphi") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: qoverpSig               = " << trackqoverpsig
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "qoverpSig") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: nSctHits                = " << unsigned(nSCT)
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "nSctHits") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: z0sinTheta              = " << z0 * sin(theta)
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "z0sinTheta") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: d0Err                   = " << d0sigma
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "d0Err") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: d0Sig                   = " << d0 / d0sigma
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "d0Sig") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: deltaphi2               = " << deltaPhi2
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "deltaphi2") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: chi2oftrackfit          = " << trackchi2
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "chi2oftrackfit") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: deltaPoverP             = " << dpOverp
    << ",\t isRequested= "
    << (std::find(m_inputVars.begin(), m_inputVars.end(), "deltaPoverP") !=
        m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: deltaDeltaPhiFirstandLM = "
    << deltaPhi2 - deltaPhiFromLM << ",\t isRequested= "
    << (std::find(m_inputVars.begin(),
                  m_inputVars.end(),
                  "deltaDeltaPhiFirstAndLM") != m_inputVars.end())
    << std::endl
    << "\t\t xAOD variables: AllFound                = " << allFound);

  ////KM: dumping variables, only variables used by BDT
  // std::cout<<"\t\t event# "<<eventInfo->eventNumber()<<std::endl;
  // unsigned i=0;
  // for (auto inputVar: m_inputVars) {
  //   std::cout<<"\t kmdebug: "<<inputVar<<"\t = "<<v_inputs[i]<<std::endl;
  //   i++;
  // }

  // double bdt_output =
  // m_v_bdts.at(bdt_index)->GetGradBoostMVA(m_v_bdts.at(bdt_index)->GetPointers());
  double bdt_output = m_v_bdts.at(bdt_index)->GetGradBoostMVA(v_inputs);
  ATH_MSG_DEBUG("ECIDS-BDT= " << bdt_output);
  // std::cout<<"\t kmdebug: \t ECIDS-BDT= "<<bdt_output<<std::endl;

  return bdt_output;
}

double
AsgElectronChargeIDSelectorTool::calculate(const EventContext& ctx,
                                           const xAOD::Egamma* eg,
                                           double mu) const
{
  if (eg->type() == xAOD::Type::Electron) {
    const xAOD::Electron* el = static_cast<const xAOD::Electron*>(eg);
    return calculate(ctx, el, mu);
  }
  ATH_MSG_ERROR("Input is not an electron!!");
  return -19;
}

//=============================================================================
asg::AcceptData
AsgElectronChargeIDSelectorTool::accept(const xAOD::IParticle* part) const
{
  // Backward compatibility
  return accept(Gaudi::Hive::currentContext(), part);
}
asg::AcceptData
AsgElectronChargeIDSelectorTool::accept(const EventContext& ctx,
                                        const xAOD::IParticle* part) const
{
  if (part->type() == xAOD::Type::Electron) {
    const xAOD::Electron* el = static_cast<const xAOD::Electron*>(part);
    return accept(ctx, el);
  }
  ATH_MSG_ERROR("Input is not an electron");
  return asg::AcceptData(&m_acceptInfo);
}

double
AsgElectronChargeIDSelectorTool::calculate(const EventContext& ctx,
                                           const xAOD::IParticle* part) const
{
  if (part->type() == xAOD::Type::Electron) {
    const xAOD::Electron* el = static_cast<const xAOD::Electron*>(part);
    return calculate(ctx, el);
  }
  ATH_MSG_ERROR("Input is not an electron!!");
  return -19;
}

//=============================================================================
// Helper method to get the number of primary vertices
// We don't want to iterate over all vertices in the event for each electron!!!
//=============================================================================
unsigned int
AsgElectronChargeIDSelectorTool::getNPrimVertices(const EventContext& ctx) const
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
