/**
   @class AsgForwardElectronLikelihoodTool
   @brief Electron selector tool to select objects in Asgena using an underlying pure ROOT tool.

*/

// Include this class's header
#include "ElectronPhotonSelectorTools/AsgForwardElectronLikelihoodTool.h"
#include "AsgElectronPhotonIsEMSelectorConfigHelper.h"
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
#include "TEnv.h"

#include "PathResolver/PathResolver.h"








//=============================================================================
// Standard constructor
//=============================================================================
AsgForwardElectronLikelihoodTool::AsgForwardElectronLikelihoodTool(std::string myname) :
  AsgTool(myname),
  m_configFile(""),
  m_rootForwardTool(0)
{

  // Create an instance of the underlying ROOT tool
  m_rootForwardTool = new Root::TForwardElectronLikelihoodTool( ("T"+myname).c_str() );

  // Declare the needed properties
  declareProperty("WorkingPoint",m_WorkingPoint="","The Working Point");
  declareProperty("ConfigFile",m_configFile="","The config file to use");
  declareProperty("usePVContainer", m_usePVCont=true, "Whether to use the PV container");
  declareProperty("nPVdefault", m_nPVdefault = 0, "The default number of PVs if not counted");
  declareProperty("primaryVertexContainer", m_primVtxContName="PrimaryVertices", "The primary vertex container name" );






  //
  // Configurables in the root tool
  //
  // pdf file name. Managed in the Asg tool.
  declareProperty("inputPDFFileName",  m_pdfFileName="", "The input ROOT file name that holds the PDFs" );
  // the variable names, if non-standard - nope, it's done above!
  declareProperty("VariableNames",m_rootForwardTool->VariableNames,"Variable names input to the LH");
  // The likelihood cut values
  declareProperty("CutLikelihoodA",m_rootForwardTool->CutLikelihoodA,"Cut on likelihood discriminant");
  // The pileup-correction part of the likelihood cut values
  declareProperty("CutLikelihoodPileupCorrection",m_rootForwardTool->CutLikelihoodPileupCorrection,"Pileup correction for LH discriminant");
 declareProperty("DiscCutForPileupTransform",m_rootForwardTool->CutLikelihoodPileupCorrectionB,"Reference disc used by pileup transform");
 declareProperty("DiscCutSlopeForPileupTransform",m_rootForwardTool->CutLikelihoodPileupCorrectionA,"Reference slope used by pileup transform");
  
  // do pileup-dependent transform on discriminant value
  declareProperty("doPileupTransform",m_rootForwardTool->doPileupTransform,"Do pileup-dependent transform on discriminant value");

}







//=============================================================================
// Standard destructor
//=============================================================================
AsgForwardElectronLikelihoodTool::~AsgForwardElectronLikelihoodTool()
{
  if(finalize().isFailure()){
    ATH_MSG_ERROR ( "Failure in AsgForwardElectronLikelihoodTool finalize()");
  }
  delete m_rootForwardTool;
}


//=============================================================================
// Asgena initialize method
//=============================================================================
StatusCode AsgForwardElectronLikelihoodTool::initialize()
{

  std::string PDFfilename(""); //Default

  if(!m_WorkingPoint.empty()){ // PS: this is still to be implemented in the mapping
    m_configFile=AsgConfigHelper::findConfigFile(m_WorkingPoint,EgammaSelectors::LHPointToConfFile);
    ATH_MSG_INFO("operating point : " << this->getOperatingPointName());
  }

  if(!m_configFile.empty()){
    std::string configFile = PathResolverFindCalibFile( m_configFile);
    if(configFile==""){ 
      ATH_MSG_ERROR("Could not locate " << m_configFile );
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG("Get the input PDFs in the tool ");
    TEnv env(configFile.c_str());
   
    // Get the input PDFs in the tool.
    ATH_MSG_DEBUG("Get the input PDFs in the tool ");
    
    if(!m_pdfFileName.empty())
      {  //If the property was set by the user, take that.
	ATH_MSG_INFO("Setting user specified PDF file " << m_pdfFileName);
	PDFfilename = m_pdfFileName;
      } else {
      if (m_configFile.find("dev/") != std::string::npos) {

	// PS: hardcode here proper LH PDFs
        std::string PDFdevval = env.GetValue("inputPDFFileName", "/hepustc/home/hanlinxuy/HaxuTaP/tP/ElectronPhotonSelectorTools/data/ForwardElectronLikelihoodPdfs.root");
        PDFfilename = ("dev/"+PDFdevval);
        ATH_MSG_INFO( "Getting the input PDFs from: " << PDFfilename  );
      } else {
        PDFfilename = env.GetValue("inputPDFFileName", "/hepustc/home/hanlinxuy/HaxuTaP/tP/ElectronPhotonSelectorTools/data/ForwardElectronLikelihoodPdfs.root");
        ATH_MSG_INFO( "Getting the input PDFs from: " << PDFfilename );
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
    std::cout<<"m_rootForwardTool->CutLikelihood  "<<m_rootForwardTool->CutLikelihood.size()<<std::endl;
    m_rootForwardTool->CutLikelihoodPileupCorrectionA = AsgConfigHelper::HelperDouble("DiscCutSlopeForPileupTransform",env);
    m_rootForwardTool->CutLikelihoodPileupCorrectionB = AsgConfigHelper::HelperDouble("DiscCutForPileupTransform",env);
    m_rootForwardTool->doPileupTransform = env.GetValue("doPileupTransform", false);
	
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
  
  // Copy the now filled TAccept and TResult to the dummy
  m_acceptDummy = m_rootForwardTool->getTAccept();
  m_resultDummy = m_rootForwardTool->getTResult();
    
  return StatusCode::SUCCESS ;
  
}


//=============================================================================
// Asgena finalize method (now called by destructor)
//=============================================================================
StatusCode AsgForwardElectronLikelihoodTool::finalize()
{
  if ( !(m_rootForwardTool->finalize()) )
    {
      ATH_MSG_ERROR ( "ERROR! Something went wrong at finalize!" );
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
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
      ATH_MSG_INFO("Failed, no egamma object.");
      return m_acceptDummy;
    }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster )
    {
      ATH_MSG_INFO("Failed, no cluster.");
      return m_acceptDummy;
    }  
  
  const double energy =  cluster->e();
  // const float eta = (cluster->etaBE(2)); 
  const float eta = (cluster->eta()); // PS the eta variables were inconsistent
  if ( fabs(eta) > 300.0 )
    {
      ATH_MSG_INFO("Failed, eta range.");
      ATH_MSG_INFO(" we were checking . " << eta);
      return m_acceptDummy;
    }
  
  // transverse energy of the electron (using the track eta) 
  //  const double et = eg->pt(); 
  double et = ( cosh(eta) != 0.) ? energy/cosh(eta) : 0.;
  
  // PS: I don't understand these lines, we don't need the variales here, only in the calculate function
  // double secondDensity(0),significance(0),secondLambda(0), lateral(0),  longitudinal(0), fracMax(0), secondR(0), centerLambda(0);
  double ip(0);
  
  // bool allFound = true;

  // // secondLambda
  // allFound = cluster->retrieveMoment(xAOD::CaloCluster::SECOND_LAMBDA,secondLambda)     && allFound;
  // // lateral
  // allFound = cluster->retrieveMoment(xAOD::CaloCluster::LATERAL,lateral) 		&& allFound;
  // // longitudinal
  // allFound = cluster->retrieveMoment(xAOD::CaloCluster::LONGITUDINAL, longitudinal) 	&& allFound;
  // // fracMax
  // allFound = cluster->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_MAX, fracMax) 		&& allFound;
  // // secondR
  // allFound = cluster->retrieveMoment(xAOD::CaloCluster::SECOND_R, secondR)  		&& allFound;
  // // center lambda
  // allFound = cluster->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,centerLambda)  	&& allFound;
  // allFound = cluster->retrieveMoment(xAOD::CaloCluster::SIGNIFICANCE, significance) 	&& allFound;
  // allFound = cluster->retrieveMoment(xAOD::CaloCluster::SECOND_ENG_DENS, secondDensity) && allFound;
    
  // Get the number of primary vertices in this event
  if( mu < 0 ){ // use npv if mu is negative (not given)
    ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);
  }
  else {
    ip = mu;
  }

  // if (!allFound) {
  //   ATH_MSG_WARNING("Have some variables missing.");
  // }

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
const Root::TResult& AsgForwardElectronLikelihoodTool::calculate( const xAOD::Egamma* eg, double mu, int set ) const
{
  
  const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(eg);
  return calculate(el, mu, set);

}


//=============================================================================
// The main result method: the actual likelihood is calculated here
//=============================================================================
const Root::TResult& AsgForwardElectronLikelihoodTool::calculate( const xAOD::Electron* eg, double mu , int set  ) const
{
  if ( !eg )
    {
      ATH_MSG_INFO("Failed, no egamma object.");
      return m_resultDummy;
    }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster )
    {
      ATH_MSG_INFO("Failed, no cluster.");
      return m_resultDummy;
    }  
  
  const double energy =  cluster->e();
  // float eta2 = cluster->etaBE(2); // PS: is inconsitent with above where etaBE2 is used
  const float eta = cluster->eta(); 
  if ( fabs(eta) > 300.0 )
    {
      ATH_MSG_INFO("Failed, eta range.");
      ATH_MSG_INFO("checking at other place ." << eta);
      return m_resultDummy;
    }
  
  const double et = ( cosh(eta) != 0.) ? energy/cosh(eta) : 0.;

  // Shower shape variables
  double secondDensity(0),significance(0),secondLambda(0), lateral(0),  longitudinal(0), fracMax(0), secondR(0), centerLambda(0);

  bool allFound = true;

  // secondLambda
  allFound = cluster->retrieveMoment(xAOD::CaloCluster::SECOND_LAMBDA,secondLambda)     && allFound;
  // lateral
  allFound = cluster->retrieveMoment(xAOD::CaloCluster::LATERAL,lateral)	        && allFound;
  // longitudinal
  allFound = cluster->retrieveMoment(xAOD::CaloCluster::LONGITUDINAL, longitudinal)     && allFound;
  // fracMax
  allFound = cluster->retrieveMoment( xAOD::CaloCluster::ENG_FRAC_MAX, fracMax)         && allFound;
  // secondR
  allFound = cluster->retrieveMoment( xAOD::CaloCluster::SECOND_R, secondR) 	        && allFound;
  // centerlambda
  allFound = cluster->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,centerLambda)     && allFound;
  allFound = cluster->retrieveMoment(xAOD::CaloCluster::SECOND_ENG_DENS, secondDensity) && allFound;
  allFound = cluster->retrieveMoment(xAOD::CaloCluster::SIGNIFICANCE, significance)     && allFound;


   // Get the number of primary vertices in this event
  double ip = static_cast<double>(m_nPVdefault);

  if(mu < 0){ // use npv if mu is negative (not given)
    ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);
    }
  else{
    ip = mu;
  }

  ATH_MSG_VERBOSE ( Form("Vars: eta=%8.5f, et=%8.5f, 2nd lambda=%8.5f, lateral=%8.5f, longitudinal=%8.5f, center lambda=%8.5f, frac max=%8.5f, secondR=%8.5f, significance=%8.5f, 2nd density=%8.5f, ip=%8.5f, set=%d",
			 eta, et, secondLambda, lateral, longitudinal,
			 centerLambda, fracMax, secondR, significance, secondDensity, ip, set ) );
  
  
  if (!allFound) {
    ATH_MSG_WARNING("Have some variables missing.");
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
				       ip,
				       set);
}

/** Method to get the plain TAccept */
const Root::TAccept& AsgForwardElectronLikelihoodTool::getTAccept( ) const{
  return m_rootForwardTool->getTAccept();
}

/** Method to get the plain TResult */
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
    ATH_MSG_ERROR("AsgForwardElectronLikelihoodTool::could not cast to const Electron");
    return m_acceptDummy;
  }

}
//=============================================================================
const Root::TResult& AsgForwardElectronLikelihoodTool::calculate(const xAOD::IParticle* part, int set ) const
{
  const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
  if (eg)
    {
      return calculate(eg,set);
    }
  else
    {
      ATH_MSG_ERROR ( " Could not cast to const Electron " );
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


