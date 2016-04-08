/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonSmearer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_ParSimTools/MuonSmearer.h"
// ISF
#include "ISF_Event/ISFParticle.h"
// RandGauss
#include "CLHEP/Random/RandGauss.h"

// ROOFIT
#include <RooFit.h>
#include <RooMultiVarGaussian.h>
#include <RooArgList.h>
#include <RooRealVar.h>
#include <RooDataSet.h>


// C++ STL
#include <fstream>
#include <iostream>

// PathResolver
#include "PathResolver/PathResolver.h"

//using namespace RooFit;

//================ Constructor ================================================
iParSim::MuonSmearer::MuonSmearer(const std::string& t, const std::string& n, const IInterface*  p ) :
  AthAlgTool(t,n,p),
  m_filenamesMC12MuonSmearer(),
  m_filenameMC12MuonPtBins("MC12MuonParametrisationPtBins.txt"),
  m_filenameMC12MuonEtaBins("MC12MuonParametrisationEtaBins.txt"),
  m_pt_min(),
  m_pt_max(),
  m_eta_min(),
  m_eta_max(),
  m_para_code(1),
  m_file_para(0),
  m_randomSvc("AtDSFMTGenSvc", n),
  m_randomEngine(0),
  m_randomEngineName("ParSimRnd")

{

    declareInterface<iParSim::IChargedSmearer>(this);
    // Mode of the param simulation: 1 ID only, 2 MS only, 3 combined
    declareProperty("MuonSmearerMode"          ,    m_para_code                );
    // std::map containing names of the MC12MuonSmearer files
    declareProperty("filenamesMC12MuonSmearer" ,    m_filenamesMC12MuonSmearer );
    // file name fo the MC12 Muon Smearer pt bins file
    declareProperty("filenameMC12MuonPtBins"   ,    m_filenameMC12MuonPtBins   );
    // file name fo the MC12 Muon Smearer eta bins file
    declareProperty("filenameMC12MuonEtaBins"  ,    m_filenameMC12MuonEtaBins  );

}

//================ Destructor =================================================

iParSim::MuonSmearer::~MuonSmearer()
{}

//================ Initialisation =============================================
StatusCode iParSim::MuonSmearer::initialize()
{

  // open the file containing bin information and store it in the bin vectors
  std::string filenameMC12MuonPtBins = PathResolver::find_file (m_filenameMC12MuonPtBins, "DATAPATH");
  if (filenameMC12MuonPtBins != "") ATH_MSG_INFO( "[ --- ] Parameterisation Pt bins file found: " << filenameMC12MuonPtBins );
  else {
    ATH_MSG_ERROR( "[ --- ] Parameterisation Pt bins file not found" );
    return StatusCode::FAILURE;
  }
  std::string filenameMC12MuonEtaBins = PathResolver::find_file (m_filenameMC12MuonEtaBins, "DATAPATH");
  if (filenameMC12MuonEtaBins != "") ATH_MSG_INFO( "[ --- ] Parameterisation Eta bins file found: " << filenameMC12MuonEtaBins );
  else {
    ATH_MSG_ERROR( "[ --- ] Parameterisation Eta bins file not found" );
    return StatusCode::FAILURE;
  }
  if( !getBinsFromFile( filenameMC12MuonPtBins.c_str(), filenameMC12MuonEtaBins.c_str() ) ) {
      ATH_MSG_ERROR("[ --- ] Could not retrieve bins from file!!");
      return StatusCode::FAILURE;
  }
  else ATH_MSG_INFO("[ --- ] Bins successfully read from file.");
  
  // open the parametrisation root file
  ATH_MSG_INFO( "[ --- ] MuonSmearer is in mode " << m_para_code << ". Parametrisation file is: " << m_filenamesMC12MuonSmearer[m_para_code] );
  std::string resolvedFileName = PathResolver::find_file (m_filenamesMC12MuonSmearer[m_para_code], "DATAPATH");
  if (resolvedFileName != "") ATH_MSG_INFO( "[ --- ] Parameterisation file found: " << resolvedFileName );
  else {
    ATH_MSG_ERROR( "[ --- ] Parameterisation file not found" );
    return StatusCode::FAILURE;
  }
  m_file_para = TFile::Open( (resolvedFileName).c_str(),"READ" );
  if(!m_file_para){
    ATH_MSG_ERROR("[ --- ] unable to open the parametrisation file for the muon smearer (file does not exist?)");
    return StatusCode::FAILURE;
  }
  if(!m_file_para->IsOpen()){
    ATH_MSG_ERROR("[ --- ] unable to open the parametrisation file for the muon smearer (wrong or empty file?)");
    return StatusCode::FAILURE;
  }

  // Random number service
  if ( m_randomSvc.retrieve().isFailure() ) {
      ATH_MSG_ERROR( "[ --- ] Could not retrieve " << m_randomSvc );
      return StatusCode::FAILURE;
  }
  //Get own engine with own seeds:
  m_randomEngine = m_randomSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
      ATH_MSG_ERROR( "[ --- ] Could not get random engine '" << m_randomEngineName << "'" );
      return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE( "initialize() successful." );
  return StatusCode::SUCCESS;

}

unsigned int iParSim::MuonSmearer::pdg() const 
{
  return 13;
}

/** Creates a new ISFParticle from a given ParticleState, universal transport tool */
bool iParSim::MuonSmearer::smear(xAOD::TrackParticle* xaodTP) const
{

  // Input from MC event generator
  const double d0_truth     = xaodTP->d0();
  const double z0_truth     = xaodTP->z0();
  const double phi_truth    = xaodTP->phi0();
  const double theta_truth  = xaodTP->theta();
  const double eta_truth    = xaodTP->eta();
  const double qOverP_truth = xaodTP->qOverP();
  const double pt_truth     = xaodTP->pt();
  const double charge_truth = xaodTP->charge();

  // Decorate Track Particles with the Truth
  xaodTP->auxdata<float>("truth_d0")     = d0_truth;
  xaodTP->auxdata<float>("truth_z0")     = z0_truth;
  xaodTP->auxdata<float>("truth_phi")    = phi_truth;
  xaodTP->auxdata<float>("truth_theta")  = theta_truth;
  xaodTP->auxdata<float>("truth_qOverP") = qOverP_truth;
  xaodTP->auxdata<float>("truth_pt")     = pt_truth;
  xaodTP->auxdata<float>("truth_eta")    = eta_truth;
  
  // create 5 dim vector to store corrections  
  RooArgList xVec;// this is filled with the output of the MVG; e.g. this are the corrections to our 5 track params
  RooRealVar* x;
  for (int i = 0; i < 5; i++) {    
    std::string xname = "x";
    if     (i == 0) xname = "d0_corr";
    else if(i == 1) xname = "z0_corr";
    else if(i == 2) xname = "phi_corr";
    else if(i == 3) xname = "theta_corr";
    else            xname = "qoverp_corr";

    x = new RooRealVar(xname.c_str(), xname.c_str(), 0.);
    xVec.add(*x);
  }
  
  // get pt and eta bins 
  int ptbin  = getPtBin(pt_truth);
  int etabin = getEtaBin(eta_truth);
          
  if(ptbin < 0 || etabin < 0){
    if(ptbin  < 0){ 
      ATH_MSG_DEBUG("No pt bin available for : "  << pt_truth <<". Particle will not be smeared.");
      return false;
    }
    if(etabin < 0){ 
      ATH_MSG_DEBUG("No eta bin available for : " << eta_truth <<". Particle will not be smeared."); 
      return false;
    }
  }

  // The covariance matrix and the offset vector
  TMatrixDSym* cov   = new TMatrixDSym(5);
  TVectorD*    muVec = new TVectorD(5);

  // Retrieve the covariance matrix and muVec from the root file and store them in "cov" and "muVec"
  TString name;
  name.Form("covmat_ptbin%.2i_etabin%.2i",ptbin,etabin);
  m_file_para->GetObject(name,cov);
  if(!cov){
    ATH_MSG_DEBUG("No covariance matrix available for pt bin : " << ptbin << " and eta bin : " << etabin);
    return false;
  }  
  name.Form("meanvec_ptbin%.2i_etabin%.2i",ptbin,etabin);
  m_file_para->GetObject(name,muVec);
  if(!muVec){
    ATH_MSG_DEBUG("No mean vector available for pt bin : " << ptbin << " and eta bin : " << etabin);
    return false;
  }
  // flip sign of mean qoverp, because mean was computed using absolute values of |qoverp|: |qoverp_reco| - |qoverp_truth|
  // this is not needed for qoverp elements of covariance matrix, because the matrix was computed using no absolute values
  (*muVec)[4] *= charge_truth;

  //  now make the multivariate Gaussian
  RooMultiVarGaussian mvg ("mvg", "mvg", xVec, *muVec, *cov);
  RooDataSet* data = mvg.generate(xVec,1);

  const double d0corr     = data->get(0)->getRealValue("d0_corr");   
  const double z0corr     = data->get(0)->getRealValue("z0_corr");
  const double phicorr    = data->get(0)->getRealValue("phi_corr");
  const double thetacorr  = data->get(0)->getRealValue("theta_corr");  
  const double qoverpcorr = data->get(0)->getRealValue("qoverp_corr");

  // Decorate Track Particles with the Truth difference
  xaodTP->auxdata<float>("d0corr")     = d0corr;
  xaodTP->auxdata<float>("z0corr")     = z0corr;
  xaodTP->auxdata<float>("phicorr")    = phicorr;
  xaodTP->auxdata<float>("thetacorr")  = thetacorr;
  xaodTP->auxdata<float>("qOverPcorr") = qoverpcorr;

  xaodTP->setDefiningParameters(d0_truth + d0corr, z0_truth + z0corr,
                                phi_truth + phicorr, theta_truth + thetacorr,
                                qOverP_truth + qoverpcorr);

  // Defining parameters covariance matrix
  double covMatrix[15] = {
                  (*cov)(0,0),
                  (*cov)(1,0), (*cov)(1,1),
                  (*cov)(2,0), (*cov)(2,1), (*cov)(2,2),
                  (*cov)(3,0), (*cov)(3,1), (*cov)(3,2), (*cov)(3,3),
                  (*cov)(4,0), (*cov)(4,1), (*cov)(4,2), (*cov)(4,3), (*cov)(4,4)
  };
  const std::vector< float > covMatrixVec( covMatrix, covMatrix + 15 );               
  xaodTP->setDefiningParametersCovMatrixVec(covMatrixVec);
  
  delete cov;
  delete muVec;

  return true;
}

bool iParSim::MuonSmearer::getBinsFromFile(std::string ptFile, std::string etaFile){

  // clear the input vectors
  m_pt_min.clear();
  m_pt_max.clear();
  m_eta_min.clear();
  m_eta_max.clear();
  
  // Open input files for pt and eta bin
  std::ifstream infile_ptbins(ptFile.c_str());
  std::ifstream infile_etabins(etaFile.c_str());
 
  if (infile_ptbins.fail()) {
    ATH_MSG_ERROR("Could not open file PtBins.txt");
    return false;
  } 
  if (infile_etabins.fail()) {
    ATH_MSG_ERROR("Could not open file EtaBins.txt");
    return false;
  }
  
 // Read the input files
  double dummy;
  while (!infile_ptbins.eof()) {
    infile_ptbins >> dummy;
    if (infile_ptbins.eof()) break;
    m_pt_min.push_back(dummy);
    ATH_MSG_DEBUG("pt bin min: " << m_pt_min);
    infile_ptbins >> dummy;
    m_pt_max.push_back(dummy);
  }
  while (!infile_etabins.eof()) {
    infile_etabins >> dummy;
    if (infile_etabins.eof()) break;
    m_eta_min.push_back(dummy);
    ATH_MSG_DEBUG("eta bin min: " << m_eta_min);
    infile_etabins >> dummy;
    m_eta_max.push_back(dummy);
  }
  
  infile_ptbins.close();
  infile_etabins.close();
  
  if(m_pt_min.size() != m_pt_max.size() || m_eta_min.size() != m_eta_max.size()){
    ATH_MSG_ERROR("Sizes of minimum and maximum bin vectors are not compatible!");
    return false;
  }
  
  return true;
}

int iParSim::MuonSmearer::getEtaBin(const double eta) const{
  
  for(unsigned int i =0 ; i < m_eta_min.size() ; i++){
    if(eta > m_eta_min[i] && eta <= m_eta_max[i]) return i;
  }
  return -1;
}

int iParSim::MuonSmearer::getPtBin(const double pt) const{
  
  for(unsigned int i =0 ; i < m_pt_min.size() ; i++){
    if(pt > m_pt_min[i] && pt <= m_pt_max[i]) return i;
  }
  return -1;
}

StatusCode iParSim::MuonSmearer::finalize()
{ 
    m_file_para->Close();
    delete m_file_para;

    ATH_MSG_VERBOSE( "finalize() successful " );
    return StatusCode::SUCCESS;
}



