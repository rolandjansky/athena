/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

   @class AsgForwardElectronLikelihoodTool
   @brief Electron selector tool to select objects in Asgena using an underlying pure ROOT tool.

*/

// Include this class's header
#include "ElectronPhotonSelectorTools/AsgForwardElectronLikelihoodTool.h"

#include "EgammaAnalysisHelpers/AsgElectronPhotonIsEMSelectorConfigHelper.h"
#include "TForwardElectronLikelihoodTool.h"
#include "EGSelectorConfigurationMapping.h"

// STL includes
#include <string>
#include <cstdint>
#include <cmath>

//EDM includes
#include "xAODEgamma/Electron.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "TEnv.h"

#include "PathResolver/PathResolver.h"


//=============================================================================
// Standard constructor
//=============================================================================
AsgForwardElectronLikelihoodTool::AsgForwardElectronLikelihoodTool(std::string myname) :
  AsgTool(myname),
  m_configFile(""),
  m_rootForwardTool(new Root::TForwardElectronLikelihoodTool( ("T"+myname).c_str() ))
{

  // Declare the needed properties
  declareProperty("WorkingPoint",m_WorkingPoint="","The Working Point"     );
  declareProperty("ConfigFile"  ,m_configFile  ="","The config file to use");
  declareProperty("usePVContainer", m_usePVCont=true, "Whether to use the PV container");
  declareProperty("nPVdefault"    , m_nPVdefault = 0, "The default number of PVs if not counted");
  declareProperty("primaryVertexContainer", m_primVtxContName="PrimaryVertices", "The primary vertex container name" );
  //
  // Configurables in the root tool
  //
  // pdf file name. Managed in the Asg tool.
  declareProperty("inputPDFFileName",  m_pdfFileName="", "The input ROOT file name that holds the PDFs" );
  // the variable names, if non-standard - nope, it's done above!
  declareProperty("VariableNames",m_rootForwardTool->VariableNames,"Variable names input to the LH");
  
  // The likelihood cut values
  declareProperty("CutLikelihood",m_rootForwardTool->CutLikelihood,"Cut on likelihood discriminant");
  // do pileup-dependent discriminant cut
  declareProperty("DiscCutSlopeForPileupTransform",m_rootForwardTool->CutLikelihoodPileupCorrectionA,"Slope correction for pileup dependent discriminant cut");
  declareProperty("DiscCutForPileupTransform"     ,m_rootForwardTool->CutLikelihoodPileupCorrectionB,"Additional offset for pileup dependent discriminant cut");
  declareProperty("doPileupCorrection",m_rootForwardTool->doPileupCorrection,"Do pileup-dependent discriminant cut");

}

//=============================================================================
// Standard destructor
//=============================================================================
AsgForwardElectronLikelihoodTool::~AsgForwardElectronLikelihoodTool()
{
  if ( !(m_rootForwardTool->finalize()) )
    {
      ATH_MSG_ERROR ( "ERROR! Something went wrong at finalize!" );
    }
}

//=============================================================================
// Asg/Athena initialize method
//=============================================================================
StatusCode AsgForwardElectronLikelihoodTool::initialize()
{

  std::string PDFfilename(""); //Default

  if(!m_WorkingPoint.empty()){
    m_configFile=AsgConfigHelper::findConfigFile(m_WorkingPoint,EgammaSelectors::ForwardLHPointToConfFile);
    ATH_MSG_INFO("operating point : " << this->getOperatingPointName());
  }

  if(!m_configFile.empty()){
    std::string configFile = PathResolverFindCalibFile( m_configFile);
    if(configFile==""){ 
      ATH_MSG_ERROR("Could not locate " << m_configFile );
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG("Get the input PDFs in the tool ");
    TEnv env;
    env.ReadFile(configFile.c_str(), kEnvLocal);
   
    // Get the input PDFs in the tool.
    ATH_MSG_DEBUG("Get the input PDFs in the tool ");
    
    if(!m_pdfFileName.empty())
      {  //If the property was set by the user, take that.
	ATH_MSG_INFO("Setting user specified PDF file " << m_pdfFileName);
	PDFfilename = m_pdfFileName;
      } else {
      if (m_configFile.find("dev/") != std::string::npos) {      
        std::string PDFdevval = env.GetValue("inputPDFFileName", "ElectronPhotonSelectorTools/offline/mc16_20180716/ForwardElectronLikelihoodPdfs.root"); // this is the first PDF ever released
        PDFfilename = ("dev/"+PDFdevval);
        ATH_MSG_DEBUG( "Getting the input PDFs from: " << PDFfilename  );
      } else {
        PDFfilename = env.GetValue("inputPDFFileName", "ElectronPhotonSelectorTools/offline/mc16_20180716/ForwardElectronLikelihoodPdfs.root");
        ATH_MSG_DEBUG( "Getting the input PDFs from: " << PDFfilename );
      }
    }
    std::string filename = PathResolverFindCalibFile( PDFfilename );
    
    if (!filename.empty()){
      m_rootForwardTool->setPDFFileName( filename );
    }else{
      ATH_MSG_ERROR ("Could not find PDF file");
      return StatusCode::FAILURE;
    }

    m_rootForwardTool->VariableNames = env.GetValue("VariableNames","");
    m_rootForwardTool->CutLikelihood = AsgConfigHelper::HelperDouble("CutLikelihood",env);
    m_rootForwardTool->CutLikelihoodPileupCorrectionA = AsgConfigHelper::HelperDouble("DiscCutSlopeForPileupCorrection",env);
    m_rootForwardTool->CutLikelihoodPileupCorrectionB = AsgConfigHelper::HelperDouble("DiscCutForPileupCorrection",env);
    m_rootForwardTool->doPileupCorrection = env.GetValue("doPileupCorrection", false);
	
  } else{  //Error if it cant find the conf
    ATH_MSG_ERROR("Could not find configuration file");
    return StatusCode::FAILURE;
  }

  
  ///-----------End of text config----------------------------

  // Get the name of the current operating point, and massage the other strings accordingly
  ATH_MSG_VERBOSE( "Going to massage the labels based on the provided operating point..." );
  
  // Get the message level and set the underlying ROOT tool message level accordingly
  m_rootForwardTool->msg().setLevel(this->msg().level());
  
  // We need to initialize the underlying ROOT TSelectorTool
  if ( 0 == m_rootForwardTool->initialize() )
    {
      ATH_MSG_ERROR ( "ERROR! Could not initialize the TForwardElectronLikelihoodTool!" );
      return StatusCode::FAILURE;
    }
  
  m_resultDummy = m_rootForwardTool->getTResult();
    
  return StatusCode::SUCCESS ;
  
}

//=============================================================================
// The main accept method: the actual cuts are applied here 
//=============================================================================
const Root::TAccept& AsgForwardElectronLikelihoodTool::accept( const xAOD::Egamma* eg, double mu ) const
{

  const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(eg);
  return accept(el, mu); 

}

//=============================================================================
// The main accept method: the actual cuts are applied here 
//=============================================================================
const Root::TAccept& AsgForwardElectronLikelihoodTool::accept( const xAOD::Electron* eg, double mu ) const
{
  if ( !eg )
    {
      ATH_MSG_ERROR("Failed, no egamma object.");
      return m_rootForwardTool->cleanTAccept();
    }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster )
    {
      ATH_MSG_WARNING("Failed, no cluster.");
      return m_rootForwardTool->cleanTAccept();
    }  
  
  const double energy =  cluster->e();
  const float eta = (cluster->eta());
  if ( fabs(eta) > 300.0 )
    {
      ATH_MSG_WARNING("Failed, eta > 3000.");
      return m_rootForwardTool->cleanTAccept();
    }
  
  // transverse energy of the electron (using the track eta) 
  double et = ( cosh(eta) != 0.) ? energy/cosh(eta) : 0.;
  double ip(0);
  
  // Get the number of primary vertices in this event
  if( mu < 0 ){ // use npv if mu is negative (not given)
    ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);
  }
  else {
    ip = mu;
  }
  
  // for now don't cache. 
  double likelihood = calculate(eg, ip); 

  // Get the answer from the underlying ROOT tool
  return m_rootForwardTool->accept( likelihood,
				    eta,
				    et,
				    ip
				    );
}




//=============================================================================
// Calculate method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
//=============================================================================
const Root::TResult& AsgForwardElectronLikelihoodTool::calculate( const xAOD::Egamma* eg, double mu ) const
{
  
  const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(eg);
  return calculate(el, mu);

}


//=============================================================================
// The main result method: the actual likelihood is calculated here
//=============================================================================
const Root::TResult& AsgForwardElectronLikelihoodTool::calculate( const xAOD::Electron* eg, double mu ) const
{
  if ( !eg )
    {
      ATH_MSG_ERROR("Failed, no egamma object.");
      return m_resultDummy;
    }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster )
    {
      ATH_MSG_ERROR("Failed, no cluster.");
      return m_resultDummy;
    }  
  
  const double energy =  cluster->e();
  const float eta = cluster->eta(); 
  if ( fabs(eta) > 300.0 )
    {
      ATH_MSG_ERROR("Failed, eta range.");
      ATH_MSG_ERROR("checking at other place ." << eta);
      return m_resultDummy;
    }
  
  const double et = ( cosh(eta) != 0.) ? energy/cosh(eta) : 0.;

  // Shower shape variables
  double secondDensity(0),significance(0),secondLambda(0), lateral(0),  longitudinal(0), fracMax(0), secondR(0), centerLambda(0);

  bool allFound = true;
  std::string notFoundList = "";

  // secondLambda
  if ( !cluster->retrieveMoment(xAOD::CaloCluster::SECOND_LAMBDA,secondLambda) ) {
    allFound = false;
    notFoundList += "secondLambda ";
  }
  // lateral
  if ( !cluster->retrieveMoment(xAOD::CaloCluster::LATERAL,lateral) ) {
    allFound = false;
    notFoundList += "lateral ";
  }
  // longitudinal
  if ( !cluster->retrieveMoment(xAOD::CaloCluster::LONGITUDINAL, longitudinal) ) {
    allFound = false;
    notFoundList += "longitudinal ";
  }
  // fracMax
  if ( !cluster->retrieveMoment( xAOD::CaloCluster::ENG_FRAC_MAX, fracMax) ) {
    allFound = false;
    notFoundList += "fracMax ";
  }
  // secondR
  if ( !cluster->retrieveMoment( xAOD::CaloCluster::SECOND_R, secondR) ) {
    allFound = false;
    notFoundList += "secondR ";
  }
  // centerlambda
  if ( !cluster->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,centerLambda)     ) {
    allFound = false;
    notFoundList += "centerLambda ";
  }
  if ( !cluster->retrieveMoment(xAOD::CaloCluster::SECOND_ENG_DENS, secondDensity) ) {
    allFound = false;
    notFoundList += "secondDensity ";
  }
  if ( !cluster->retrieveMoment(xAOD::CaloCluster::SIGNIFICANCE, significance)     ) {
    allFound = false;
    notFoundList += "significance ";
  }
  
  
  // Get the number of primary vertices in this event
  double ip = static_cast<double>(m_nPVdefault);
  
  if(mu < 0){ // use npv if mu is negative (not given)
    ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);
    }
  else{
    ip = mu;
  }

  ATH_MSG_VERBOSE ( Form("Vars: eta=%8.5f, et=%8.5f, 2nd lambda=%8.5f, lateral=%8.5f, longitudinal=%8.5f, center lambda=%8.5f, frac max=%8.5f, secondR=%8.5f, significance=%8.5f, 2nd density=%8.5f, ip=%8.5f",
			 eta, et, secondLambda, lateral, longitudinal,
			 centerLambda, fracMax, secondR, significance, secondDensity, ip ) );
  
  
  if (!allFound) {
    ATH_MSG_ERROR("Skipping LH calculation! The following variables are missing: " << notFoundList);
    return m_resultDummy;
  }
  
  // Get the answer from the underlying ROOT tool
  return m_rootForwardTool->calculate( eta,
				       et,
				       secondLambda,
				       lateral,
				       longitudinal,
				       centerLambda,
				       fracMax,
				       secondR,
				       significance,
				       secondDensity,    
				       ip);
}

/** Method to get the plain TAccept from the underlying TSelectorToolBase*/
const Root::TAccept& AsgForwardElectronLikelihoodTool::getTAccept( ) const{
  return m_rootForwardTool->getTAccept();
}

/** Method to get the  TResult from the underlying TCalculatorToolBase*/
const Root::TResult& AsgForwardElectronLikelihoodTool::getTResult( ) const{
  return m_rootForwardTool->getTResult();
}

//=============================================================================
/// Get the name of the current operating point
//=============================================================================
std::string AsgForwardElectronLikelihoodTool::getOperatingPointName() const
{
  return m_WorkingPoint;
}
//=============================================================================
const Root::TAccept& AsgForwardElectronLikelihoodTool::accept(const xAOD::IParticle* part) const
{
  ATH_MSG_DEBUG("Entering accept( const IParticle* part )");
  const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
  if(eg)
    {
      return accept(eg);
    }
  else{
    ATH_MSG_WARNING("AsgForwardElectronLikelihoodTool::could not cast to const Electron");
    return m_rootForwardTool->cleanTAccept();
  }

}
//=============================================================================
const Root::TResult& AsgForwardElectronLikelihoodTool::calculate(const xAOD::IParticle* part ) const
{
  const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
  if (eg)
    {
      return calculate(eg);
    }
  else
    {
      ATH_MSG_WARNING ( " Could not cast to const Electron " );
      return m_resultDummy;
    }
}

//=============================================================================
// Helper method to get the number of primary vertices
// ( This is horrible! We don't want to iterate over all vertices in the event for each electron!!! 
//   This is slow!)
//=============================================================================
unsigned int AsgForwardElectronLikelihoodTool::getNPrimVertices() const
{
  static bool PVExists = true; 
  unsigned int nVtx(0);
  const xAOD::VertexContainer* vxContainer(0);
  if(PVExists)
  {
    if ( StatusCode::SUCCESS != evtStore()->retrieve( vxContainer, m_primVtxContName ) )
    {
      ATH_MSG_WARNING ( "Vertex container not found with name: " << m_primVtxContName );
      PVExists = false; // if retrieve failed, don't try to retrieve again
      return nVtx;
    }
    for ( unsigned int i=0; i<vxContainer->size(); i++ )
    {
      const xAOD::Vertex* vxcand = vxContainer->at(i);
      if ( vxcand->nTrackParticles() >= 2 ) nVtx++;
    }
  }
  return nVtx;
}
