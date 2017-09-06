/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STD includes
#include <sstream>

// ROOT includes
#include "TROOT.h"
#include "TSystem.h"
#include "TH1.h"

// EDM includes
#include "xAODJet/Jet.h"

// Non-local includes
#include "PathResolver/PathResolver.h"
#ifndef ASGTOOL_STANDALONE
#  include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

// Local includes
#include "JetResolution/JERTool.h"

/*#ifdef ASGTOOL_ATHENA
static const bool autoConfigDefault = true;
#else
static const bool autoConfigDefault = false;
#endif
*/

// Just some constants for unit conversion
using namespace JER::units;

// Helper function in anonymous namespace
namespace
{
  template<class T> StatusCode getFromFile(const std::string& name,
                                           TFile* file, T*& obj)
  {
    obj = dynamic_cast<T*> ( file->Get(name.c_str()) );
    if(obj == nullptr)
      return StatusCode::FAILURE;

    // Take ownership of histograms from ROOT.
    if(T::Class()->InheritsFrom("TH1")){
      // This cast is invalid for non hist-types, hence the dynamic cast
      // is needed to make this compile generically. Coverity then spits
      // a warning if we don't actually test the result. Could probably
      // make this cleaner with some fancy template footwork, but meh.
      TH1* h = dynamic_cast<TH1*>(obj);
      if(h == nullptr) return StatusCode::FAILURE;
      h->SetDirectory(0);
    }

    // Remove TF1s from ROOT's control, to avoid ROOT
    // cleaning them up early in PyROOT.
    if(T::Class()->InheritsFrom("TF1"))
      gROOT->GetListOfFunctions()->Remove(obj);

    return StatusCode::SUCCESS;
  }
}

//-----------------------------------------------------------------------------
// Standard ASG tool constructor
//-----------------------------------------------------------------------------
JERTool::JERTool(const std::string& name)
    : asg::AsgMetadataTool(name),
      m_etaAxis(NULL),
      m_jetAlgo(JER::JETALG_UNDEFINED)
{
  declareProperty("PlotFileName", m_fileName = "JetResolution/Prerec2015_xCalib_2012JER_ReducedTo9NP_Plots_v2.root");
  //declareProperty("JetAlg", m_jetAlgo = JER::AKt4EM);
  declareProperty("CollectionName", m_collectionName = "AntiKt4EMTopoJets");

  // Not clear if we'll still use these options
  // Auto-config is only applied in Athena
  //declareProperty("UseAutoConfiguration", m_useAutoConfig = true,
  //                "Configure BeamEnergy and SimulationType from Metadata");
  //declareProperty("SimulationType", m_simulationType = "FullSim",
  //                "Specify simulation type: {FullSim, AFII}");

  for(unsigned int i = 0; i < JER::JETALG_N; ++i)
    for(unsigned int j = 0; j < m_nEtaBins; ++j)
      for(unsigned int k = 0; k < JER::JER_NP_ALL; ++k)
        m_jerNP[i][j][k] = nullptr;
}

//-----------------------------------------------------------------------------
// Copy constructor - necessary for reflex in Athena.
// I'm not sure exactly what the use cases might be, so I'll just implement a
// shallow copy for now. I should come back to this later.
//-----------------------------------------------------------------------------
JERTool::JERTool(const JERTool& other)
    : asg::AsgMetadataTool(other.name() + "_copy"),
      m_etaAxis(other.m_etaAxis),
      m_fileName(other.m_fileName),
      m_jetAlgo(other.m_jetAlgo),
      m_collectionName(other.m_collectionName)
      //m_useAutoConfig(other.m_useAutoConfig),
      //m_inputFile(other.m_inputFile)
      // TODO: fix this
      //m_jerNP(other.m_jerNP)
{}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
JERTool::~JERTool()
{
  if(m_etaAxis) delete m_etaAxis;
  // Clean up memory that was cloned from the input file
  for(unsigned int i = 0; i < JER::JETALG_N; ++i)
    for(unsigned int j = 0; j < m_nEtaBins; ++j)
      for(unsigned int k = 0; k < JER::JER_NP_ALL; ++k)
        delete m_jerNP[i][j][k];
  //for(auto mapPair : m_jerNP)
    //if(mapPair.second) delete mapPair.second;
}

//-----------------------------------------------------------------------------
// Initialize the tool
//-----------------------------------------------------------------------------
StatusCode JERTool::initialize()
{
  // Greet the user
  ATH_MSG_INFO("Initializing JER provider");

  // Auto-configuration currently disabled
  #ifdef ASGTOOL_ATHENA
  /*if(m_useAutoConfig){
    ATH_CHECK( autoConfigure() );
  }*/
  #endif

  // Determine the jet algorithm
  if(m_collectionName.find("AntiKt4EM") != std::string::npos)
    m_jetAlgo = JER::AKt4EM;

  //using AKt4EM JER for LCTopo jets (This is temporary until we get new EMTopo/LCTopo JER recommendations).
  else if(m_collectionName.find("AntiKt4LC") != std::string::npos)
    m_jetAlgo = JER::AKt4EM;
  /*
  else if(m_collectionName.find("AntiKt6EM") != std::string::npos)
    m_jetAlgo = JER::AKt6EM;
  else if(m_collectionName.find("AntiKt6LC") != std::string::npos)
    m_jetAlgo = JER::AKt6LC;
  */
  else{
    ATH_MSG_FATAL("No support for JER of " << m_collectionName << " jets!");
    return StatusCode::FAILURE;
  }

  // Print some information
  std::string description = "Unknown";
  if (m_collectionName.find("AntiKt4EM") != std::string::npos) description = "AntiKt4 EM+JES";
  else if (m_collectionName.find("AntiKt4LC") != std::string::npos) description = "AntiKt4 LC+JES";
  else
    ATH_MSG_ERROR("Not supported JetCollection: ");
  //DV This is temporary, since only EM is supported for now
  /* else if(m_jetAlgo == JER::AKt4LC) description = "AntiKt4 LCW+JES";
  else if(m_jetAlgo == JER::AKt6EM) description = "AntiKt6 EM+JES";
  else if(m_jetAlgo == JER::AKt6LC) description = "AntiKt6 LCW+JES";*/

  if (m_collectionName.find("AntiKt4EM") != std::string::npos)
    ATH_MSG_INFO("Retrieving AKt4EM JER for jets: " << description);

  else if (m_collectionName.find("AntiKt4LC") != std::string::npos)
    ATH_MSG_INFO("Retrieving AKt4EM JER for jets: " << description);

  // Load the inputs
  ATH_CHECK( loadJERInputs() );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Attempt auto-configuration of tool using Metadata. Athena only.
// We put stuff here again later if useful.
//-----------------------------------------------------------------------------
StatusCode JERTool::autoConfigure()
{
  // Identify sample as MC or data. Might be useful later.
  //bool isData = false;
  //std::string projectName;
  //ATH_CHECK( retrieveMetadata("/TagInfo", "project_name", projectName) );
  //ATH_MSG_DEBUG("Retrieved project name: " << projectName);
  //if(projectName.find("data") != std::string::npos){
  //  ATH_MSG_DEBUG("Identified sample as data.");
  //  isData = true;
  //}

  // Try to retrieve the simulation type.
  //if(!isData){
  //  std::string simulationFlavor;
  //  ATH_CHECK( retrieveMetadata("/Simulation/Parameters", "SimulationFlavour",
  //                              simulationFlavor) );
  //  ATH_MSG_DEBUG("Retrieved simulation flavor: " << simulationFlavor);
  //  // Convert into known string
  //  if(simulationFlavor == "default") m_simulationType = "FullSim";
  //  else if(simulationFlavor == "AtlfastII") m_simulationType = "AFII";
  //  else{
  //    ATH_MSG_ERROR("Unknown simulation type retrieved from Metadata: "
  //                  << simulationFlavor);
  //    return StatusCode::FAILURE;
  //  }
  //  ATH_MSG_INFO("Overriding simulation type from Metadata: "
  //               << m_simulationType);
  //}

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Load JER results from input file
//-----------------------------------------------------------------------------
StatusCode JERTool::loadJERInputs()
{
  // Open input file using PathResolver
  ATH_MSG_DEBUG("Using JER file " << m_fileName);
  auto inputFile = std::unique_ptr<TFile>
    ( TFile::Open(PathResolverFindCalibFile(m_fileName).c_str()) );
  if(!inputFile->IsOpen()){
    ATH_MSG_FATAL("JER input file " << m_fileName << " could not be found!");
    return StatusCode::FAILURE;
  }

  double etaBins[m_nEtaBins+1] = {0, 0.8, 1.2, 2.1, 2.8, 3.2, 3.6, 4.5};
  m_etaAxis = new TAxis(m_nEtaBins, etaBins);

  // Pull the JER NP graphs
  for(unsigned int i = 0; i < JER::JETALG_N; ++i) {
    for(unsigned int j = 0; j < m_nEtaBins; ++j) {
      for(unsigned int k = 0; k < JER::JER_NP_ALL; ++k) {
        std::stringstream graphName;
        graphName << "JER_unc_NP[" << i << "][" << j << "][" << k << "]";
        ATH_CHECK( getFromFile<TGraph>( graphName.str(), inputFile.get(),
                                        m_jerNP[i][j][k] ) );
      }
    }
  }

  // Now close the input file
  inputFile->Close();
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Retrieve the MC resolution
//-----------------------------------------------------------------------------
double JERTool::getRelResolutionMC(const xAOD::Jet* jet)
{
  // Hard-coded for now, put in graphs for next iteration, less error-prone
  // TODO: These give uninitialized value warnings in coverity.
  // I'm trying to fix them by giving an empty initializer list,
  // which should initialize all values to zero.
  double noise[m_nEtaBins] = {}; //7 eta bins
  double stochastic[m_nEtaBins] = {}; //7 eta bins
  double constant[m_nEtaBins] = {}; //7 eta bins

  double jerMC = 0.;

  // TODO: initialize these arrays only once, rather than at every request.
  // Or just wait until they are in ROOT files.

  // Cone of 0.4
  if (m_jetAlgo==JER::AKt4EM) {
    noise[0]=3.34;     stochastic[0]=0.627;     constant[0]=0.0234;
    noise[1]=3.05;     stochastic[1]=0.693;     constant[1]=0.0224;
    noise[2]=3.29;     stochastic[2]=0.658;     constant[2]=0.0300;
    noise[3]=2.56;     stochastic[3]=0.607;     constant[3]=0.0250;
    noise[4]=0.988;    stochastic[4]=0.753;     constant[4]=0.0228;
    noise[5]=2.74;     stochastic[5]=0.783;     constant[5]=0.0465;
    noise[6]=2.80;     stochastic[6]=0.623;     constant[6]=0.0000;
  }
  /*  if (m_jetAlgo==JER::AKt4LC){
    noise[0]=4.14;     stochastic[0]=0.625;     constant[0]=0.0180;
    noise[1]=3.87;     stochastic[1]=0.671;     constant[1]=0.0228;
    noise[2]=4.43;     stochastic[2]=0.542;     constant[2]=0.0242;
    noise[3]=3.36;     stochastic[3]=0.537;     constant[3]=0.00955;
    noise[4]=2.97;     stochastic[4]=0.563;     constant[4]=0.0175;
    noise[5]=4.22;     stochastic[5]=0.387;     constant[5]=0.0722;
    noise[6]=2.76;     stochastic[6]=0.626;     constant[6]=0.0105;
  }

  //Cone of 0.6
  if (m_jetAlgo==JER::AKt6EM){
    noise[0]=4.24;     stochastic[0]=0.633;     constant[0]=0.0226;
    noise[1]=3.97;     stochastic[1]=0.693;     constant[1]=0.0223;
    noise[2]=3.57;     stochastic[2]=0.695;     constant[2]=0.0285;
    noise[3]=1.95;     stochastic[3]=0.726;     constant[3]=0.0107;
    noise[4]=2.72;     stochastic[4]=0.776;     constant[4]=0.0204;
    noise[5]=4.41;     stochastic[5]=0.792;     constant[5]=0.0466;
    noise[6]=5.21;     stochastic[6]=0.293;     constant[6]=0.0392;
  }
  if (m_jetAlgo==JER::AKt6LC){
    noise[0]=5.54;     stochastic[0]=0.635;     constant[0]=0.0172;
    noise[1]=5.18;     stochastic[1]=0.675;     constant[1]=0.0218;
    noise[2]=5.78;     stochastic[2]=0.534;     constant[2]=0.0241;
    noise[3]=4.61;     stochastic[3]=0.558;     constant[3]=0.0000;
    noise[4]=4.53;     stochastic[4]=0.576;     constant[4]=0.00927;
    noise[5]=5.97;     stochastic[5]=0.453;     constant[5]=0.0575;
    noise[6]=4.40;     stochastic[6]=0.590;     constant[6]=0.0000;
  }*/

  // Truncate pt to [10, 1500] GeV. If this is uniformly done
  // everywhere, consider writing a function to truncate.
  double pt = std::min( std::max(jet->pt(), 10.*GeV), 1.5*TeV );
  int etaBin = getEtaBin(jet);

  // Multiplying by invGeV bug?
  jerMC = pow( pow(noise[etaBin]/(pt*invGeV), 2) +
               pow(stochastic[etaBin], 2)/(pt*invGeV) +
               pow(constant[etaBin], 2),
               0.5 );

  return jerMC;


}

//-----------------------------------------------------------------------------
// Retrieve the resolution in data
//-----------------------------------------------------------------------------
double JERTool::getRelResolutionData(const xAOD::Jet* jet)
{

  // hard-coded for now, put in graphs for next iteration, less error-prone
  double noise[m_nEtaBins]; //7 eta bins
  double stochastic[m_nEtaBins]; //7 eta bins
  double constant[m_nEtaBins]; //7 eta bins
  double jerData = 0.;

  //This won't be necessary once we move away from the hard-coded version
  noise[4]=0.;     stochastic[4]=0.;     constant[4]=0.;
  noise[5]=0.;     stochastic[5]=0.;     constant[5]=0.;
  noise[6]=0.;     stochastic[6]=0.;     constant[6]=0.;

  //Cone of 0.4
  if (m_jetAlgo==JER::AKt4EM) {
    noise[0]=3.325;     stochastic[0]=0.712728;     constant[0]=0.0297113;
    noise[1]=3.038;     stochastic[1]=0.671540;     constant[1]=0.0360607;
    noise[2]=3.339;     stochastic[2]=0.613501;     constant[2]=0.0444547;
    noise[3]=2.907;     stochastic[3]=0.458915;     constant[3]=0.0528269;
  }
  /*  if (m_jetAlgo==JER::AKt4LC){
    noise[0]=4.123;     stochastic[0]=0.738016;     constant[0]=0.0225649;
    noise[1]=3.656;     stochastic[1]=0.642889;     constant[1]=0.0390098;
    noise[2]=4.265;     stochastic[2]=0.584586;     constant[2]=0.0344256;
    noise[3]=3.375;     stochastic[3]=0.260094;     constant[3]=0.0497415;
  }

  //Cone of 0.6
  if (m_jetAlgo==JER::AKt6EM){
    noise[0]=4.342;     stochastic[0]=0.670101;    constant[0]=0.0298946;
    noise[1]=4.055;     stochastic[1]=0.75919;     constant[1]=0.0308167;
    noise[2]=3.959;     stochastic[2]=0.562952;    constant[2]=0.0422679;
    noise[3]=3.406;     stochastic[3]=0.476782;    constant[3]=0.0485427;
  }
  if (m_jetAlgo==JER::AKt6LC){
    noise[0]=5.502;     stochastic[0]=0.65593;     constant[0]=0.0258957;
    noise[1]=5.400;     stochastic[1]=0.777139;    constant[1]=0.0315415;
    noise[2]=5.665;     stochastic[2]=0.624364;    constant[2]=0.0311714;
    noise[3]=5.197;     stochastic[3]=0.508976;    constant[3]=0.0284552;
    }*/

  // Truncate pt to [10, 1500] GeV. If this is uniformly done
  // everywhere, consider writing a function to truncate.
  double pt = std::min( std::max(jet->pt(), 10.*GeV), 1.5*TeV );
  int etaBin = getEtaBin(jet);

  // Multiplying by invGeV bug?
  jerData = pow( pow(noise[etaBin]/(pt*invGeV), 2) +
                 pow(stochastic[etaBin], 2)/(pt*invGeV) +
                 pow(constant[etaBin], 2),
                 0.5 );

  return jerData;

}

//-----------------------------------------------------------------------------
// Calculate the resolution uncertainty
//-----------------------------------------------------------------------------
double JERTool::getUncertainty(const xAOD::Jet* jet, JER::Uncert errType)
{
  ATH_MSG_DEBUG("getUncertainty " << errType);

  // Truncate pt to [10, 1500] GeV. If this is uniformly done
  // everywhere, consider writing a function to truncate.
  double pt = std::min( std::max(jet->pt(), 10.*GeV), 1.5*TeV );
  int etaBin = getEtaBin(jet);
  TGraph** jerNP = m_jerNP[m_jetAlgo][etaBin];

  if(errType == JER::JER_CROSS_CALIB_ALL){
    // Combine all xCalib NPs in quadrature
    ATH_MSG_DEBUG("JER_CROSS_CALIB_FORWARD");
    double unc = 0;
    for(unsigned int i = JER::JER_CROSS_CALIB_50nsVs25ns;
        i < JER::JER_CROSS_CALIB_ALL-1; ++i){
      unc += pow( jerNP[i]->Eval(pt*invGeV), 2);
    }

    ATH_MSG_DEBUG("  quadratic sum = " << sqrt(unc));
    return sqrt(unc);
  }
  else if(errType == JER::JER_NP_ALL){
    // Combine all NPs in quadrature
    ATH_MSG_DEBUG("Calculating JER_NP_ALL");
    double unc = 0;
    for(unsigned int i = 0; i < JER::JER_NP_ALL; ++i){
      // Be careful: Noise forward unc is stored as relative;
      // need to use absolute (jerMC*unc)
      if(i == JER::JER_NOISE_FORWARD)
	unc += pow( (jerNP[i]->Eval(pt*invGeV)) * getRelResolutionMC(jet), 2);
      else
	unc += pow( jerNP[i]->Eval(pt*invGeV), 2);
    }

    ATH_MSG_DEBUG("  quadratic sum = " << sqrt(unc));
    return sqrt(unc);
  }

  else{
    // Return a single NP.
    // Using a map here would help to catch missing results.
    // Right now this would seg fault.
    return jerNP[errType]->Eval(pt*invGeV);
  }
}

//-----------------------------------------------------------------------------
// Get eta bin corresponding for this jet.
// Indexing convention of eta regions is off-by-one
// with respect to the TAxis (etaBin = TAxisBin - 1)
//-----------------------------------------------------------------------------
int JERTool::getEtaBin(const xAOD::Jet* jet)
{
  int bin = m_etaAxis->FindBin(fabs(jet->eta()));
  bin = std::min((int) m_nEtaBins, bin); // include overflow
  return bin - 1;
}
