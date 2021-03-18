/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**

   @class AsgForwardElectronLikelihoodTool
   @brief Electron selector tool to select objects in Asgena using an underlying pure ROOT tool.

*/

// Include this class's header
#include "ElectronPhotonSelectorTools/AsgForwardElectronLikelihoodTool.h"
#include "EgammaAnalysisHelpers/AsgEGammaConfigHelper.h"
#include "TForwardElectronLikelihoodTool.h"
#include "EGSelectorConfigurationMapping.h"

// STL includes
#include <string>
#include <cstdint>
#include <cmath>

//EDM includes
#include "xAODEgamma/Electron.h"
#include "xAODTracking/Vertex.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "TEnv.h"
#include "PathResolver/PathResolver.h"
#include "AsgTools/CurrentContext.h"
#include "AsgDataHandles/ReadHandle.h"

//=============================================================================
// Standard constructor
//=============================================================================
AsgForwardElectronLikelihoodTool::AsgForwardElectronLikelihoodTool(const std::string& myname) : AsgTool(myname),
                                                                                                m_configFile{""},
                                                                                                m_rootForwardTool{nullptr}
{

  // Create an instance of the underlying ROOT tool
  m_rootForwardTool = new Root::TForwardElectronLikelihoodTool(("T" + myname).c_str());

  // Declare the needed properties
  declareProperty("WorkingPoint", m_WorkingPoint = "", "The Working Point");
  declareProperty("ConfigFile", m_configFile = "", "The config file to use");
  declareProperty("usePVContainer", m_usePVCont = true, "Whether to use the PV container");
  declareProperty("nPVdefault", m_nPVdefault = 0, "The default number of PVs if not counted");
  //
  // Configurables in the root tool
  //
  // pdf file name. Managed in the Asg tool.
  declareProperty("inputPDFFileName", m_pdfFileName = "", "The input ROOT file name that holds the PDFs");
  // the variable names, if non-standard - nope, it's done above!
  declareProperty("VariableNames", m_rootForwardTool->m_variableNames, "Variable names input to the LH");

  // The likelihood cut values
  declareProperty("CutLikelihood", m_rootForwardTool->m_cutLikelihood, "Cut on likelihood discriminant");
  // do pileup-dependent discriminant cut
  declareProperty("DiscCutSlopeForPileupTransform", m_rootForwardTool->m_cutLikelihoodPileupCorrectionA, "Slope correction for pileup dependent discriminant cut");
  declareProperty("DiscCutForPileupTransform", m_rootForwardTool->m_cutLikelihoodPileupCorrectionB, "Additional offset for pileup dependent discriminant cut");
  declareProperty("doPileupCorrection", m_rootForwardTool->m_doPileupCorrection, "Do pileup-dependent discriminant cut");
}

//=============================================================================
// Standard destructor
//=============================================================================
AsgForwardElectronLikelihoodTool::~AsgForwardElectronLikelihoodTool()
{
  delete m_rootForwardTool;
}

//=============================================================================
// Asg/Athena initialize method
//=============================================================================
StatusCode AsgForwardElectronLikelihoodTool::initialize()
{

  ATH_MSG_INFO("initialize : WP " << m_WorkingPoint.size() << " " << m_configFile.size());

  std::string PDFfilename(""); //Default

  if (!m_WorkingPoint.empty())
  {
    m_configFile = AsgConfigHelper::findConfigFile(m_WorkingPoint, EgammaSelectors::ForwardLHPointToConfFile);
    ATH_MSG_INFO("operating point : " << this->getOperatingPointName());
  }

  if (!m_configFile.empty())
  {
    std::string configFile = PathResolverFindCalibFile(m_configFile);
    if (configFile.empty())
    {
      ATH_MSG_ERROR("Could not locate " << m_configFile);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Get the input PDFs in the tool ");
    TEnv env(configFile.c_str());

    // Get the input PDFs in the tool.
    ATH_MSG_DEBUG("Get the input PDFs in the tool ");

    if (!m_pdfFileName.empty())
    { //If the property was set by the user, take that.
      ATH_MSG_INFO("Setting user specified PDF file " << m_pdfFileName);
      PDFfilename = m_pdfFileName;
    }
    else
    {
      if (m_configFile.find("dev/") != std::string::npos)
      {
        std::string PDFdevval = env.GetValue("inputPDFFileName", "ElectronPhotonSelectorTools/offline/mc16_20180716/ForwardElectronLikelihoodPdfs.root"); // this is the first PDF ever released
        PDFfilename = ("dev/" + PDFdevval);
        ATH_MSG_DEBUG("Getting the input PDFs from: " << PDFfilename);
      }
      else
      {
        PDFfilename = env.GetValue("inputPDFFileName", "ElectronPhotonSelectorTools/offline/mc16_20180716/ForwardElectronLikelihoodPdfs.root");
        ATH_MSG_DEBUG("Getting the input PDFs from: " << PDFfilename);
      }
    }
    std::string filename = PathResolverFindCalibFile(PDFfilename);

    if (!filename.empty())
    {
      m_rootForwardTool->setPDFFileName(filename.c_str());
    }
    else
    {
      ATH_MSG_ERROR("Could not find PDF file");
      return StatusCode::FAILURE;
    }

    m_rootForwardTool->m_variableNames = env.GetValue("VariableNames", "");
    m_rootForwardTool->m_cutLikelihood = AsgConfigHelper::HelperDouble("CutLikelihood", env);
    m_rootForwardTool->m_cutLikelihoodPileupCorrectionA = AsgConfigHelper::HelperDouble("DiscCutSlopeForPileupCorrection", env);
    m_rootForwardTool->m_cutLikelihoodPileupCorrectionB = AsgConfigHelper::HelperDouble("DiscCutForPileupCorrection", env);
    m_rootForwardTool->m_doPileupCorrection = env.GetValue("doPileupCorrection", false);
  }
  else
  { //Error if it cant find the conf
    ATH_MSG_ERROR("Could not find configuration file");
    return StatusCode::FAILURE;
  }

  ///-----------End of text config----------------------------

  // Setup primary vertex key handle
  ATH_CHECK( m_primVtxContKey.initialize(m_usePVCont) );

  // Get the name of the current operating point, and massage the other strings accordingly
  ATH_MSG_VERBOSE("Going to massage the labels based on the provided operating point...");

  // Get the message level and set the underlying ROOT tool message level accordingly
  m_rootForwardTool->msg().setLevel(this->msg().level());

  // We need to initialize the underlying ROOT TSelectorTool
  if ( m_rootForwardTool->initialize().isFailure() )
  {
    ATH_MSG_ERROR("ERROR! Could not initialize the TForwardElectronLikelihoodTool!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// return the accept info object
//=============================================================================

const asg::AcceptInfo& AsgForwardElectronLikelihoodTool::getAcceptInfo() const
{
    return m_rootForwardTool->getAcceptInfo();
}

//=============================================================================
// The main accept method: the actual cuts are applied here
//=============================================================================
asg::AcceptData AsgForwardElectronLikelihoodTool::accept( const xAOD::Egamma* eg, double mu ) const
{

  const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(eg);
  return accept(el, mu); 

}

asg::AcceptData AsgForwardElectronLikelihoodTool::accept(const xAOD::Electron *eg, double mu) const
{

  //Backwards compatbility
  return accept(Gaudi::Hive::currentContext(), eg, mu);
}

asg::AcceptData AsgForwardElectronLikelihoodTool::accept(const EventContext& ctx, const xAOD::Egamma* eg, double mu) const
{

  const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(eg);
  return accept(ctx, el, mu); 
}

//=============================================================================
// The main accept method: the actual cuts are applied here
//=============================================================================
asg::AcceptData AsgForwardElectronLikelihoodTool::accept(const EventContext& ctx, const xAOD::Electron *eg, double mu) const
{
  if (!eg)
  {
    ATH_MSG_ERROR("Failed, no egamma object.");
    return m_rootForwardTool->accept();
  }

  const xAOD::CaloCluster *cluster = eg->caloCluster();
  if (!cluster)
  {
    ATH_MSG_WARNING("Failed, no cluster.");
    return m_rootForwardTool->accept();
  }

  const double energy = cluster->e();
  const float eta = (cluster->eta());
  if (fabs(eta) > 300.0)
  {
    ATH_MSG_WARNING("Failed, eta > 3000.");
    return m_rootForwardTool->accept();
  }

  // transverse energy of the electron (using the track eta)
  double et = (cosh(eta) != 0.) ? energy / cosh(eta) : 0.;
  double ip(0);

  // Get the number of primary vertices in this event
  if (mu < 0)
  { // use npv if mu is negative (not given)
    ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices(ctx) : m_nPVdefault);
  }
  else
  {
    ip = mu;
  }

  // for now don't cache.
  double likelihood = calculate(ctx, eg, ip);

  // Get the answer from the underlying ROOT tool
  return m_rootForwardTool->accept(likelihood,
                                   eta,
                                   et,
                                   ip);
}

//=============================================================================
// Calculate method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
//=============================================================================
double AsgForwardElectronLikelihoodTool::calculate(const xAOD::Egamma *eg, double mu) const
{

  const xAOD::Electron *el = dynamic_cast<const xAOD::Electron *>(eg);
  return calculate(el, mu);
}

double AsgForwardElectronLikelihoodTool::calculate( const xAOD::Electron* el, double mu ) const
{
  //Backward compatbility
  return calculate(Gaudi::Hive::currentContext(), el, mu);
}

double AsgForwardElectronLikelihoodTool::calculate(const EventContext& ctx, const xAOD::Egamma* eg, double mu) const
{

  const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(eg);
  return calculate(ctx, el, mu); 
}

//=============================================================================
// The main result method: the actual likelihood is calculated here
//=============================================================================
double AsgForwardElectronLikelihoodTool::calculate( const EventContext& ctx, const xAOD::Electron *eg, double mu) const
{
  if (!eg)
  {
    ATH_MSG_ERROR("Failed, no egamma object.");
    return -999;
  }

  const xAOD::CaloCluster *cluster = eg->caloCluster();
  if (!cluster)
  {
    ATH_MSG_ERROR("Failed, no cluster.");
    return -999;
  }

  const double energy = cluster->e();
  const float eta = cluster->eta();
  if (fabs(eta) > 300.0)
  {
    ATH_MSG_ERROR("Failed, eta range.");
    ATH_MSG_ERROR("checking at other place ." << eta);
    return -999;
  }

  const double et = (cosh(eta) != 0.) ? energy / cosh(eta) : 0.;

  // Shower shape variables
  double secondDensity(0), significance(0), secondLambda(0), lateral(0), longitudinal(0), fracMax(0), secondR(0), centerLambda(0);

  bool allFound = true;
  std::string notFoundList = "";

  // secondLambda
  if (!cluster->retrieveMoment(xAOD::CaloCluster::SECOND_LAMBDA, secondLambda))
  {
    allFound = false;
    notFoundList += "secondLambda ";
  }
  // lateral
  if (!cluster->retrieveMoment(xAOD::CaloCluster::LATERAL, lateral))
  {
    allFound = false;
    notFoundList += "lateral ";
  }
  // longitudinal
  if (!cluster->retrieveMoment(xAOD::CaloCluster::LONGITUDINAL, longitudinal))
  {
    allFound = false;
    notFoundList += "longitudinal ";
  }
  // fracMax
  if (!cluster->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_MAX, fracMax))
  {
    allFound = false;
    notFoundList += "fracMax ";
  }
  // secondR
  if (!cluster->retrieveMoment(xAOD::CaloCluster::SECOND_R, secondR))
  {
    allFound = false;
    notFoundList += "secondR ";
  }
  // centerlambda
  if (!cluster->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA, centerLambda))
  {
    allFound = false;
    notFoundList += "centerLambda ";
  }
  if (!cluster->retrieveMoment(xAOD::CaloCluster::SECOND_ENG_DENS, secondDensity))
  {
    allFound = false;
    notFoundList += "secondDensity ";
  }
  if (!cluster->retrieveMoment(xAOD::CaloCluster::SIGNIFICANCE, significance))
  {
    allFound = false;
    notFoundList += "significance ";
  }

  // Get the number of primary vertices in this event
  double ip = static_cast<double>(m_nPVdefault);

  if (mu < 0)
  { // use npv if mu is negative (not given)
    ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices(ctx) : m_nPVdefault);
  }
  else
  {
    ip = mu;
  }

  ATH_MSG_VERBOSE(Form("Vars: eta=%8.5f, et=%8.5f, 2nd lambda=%8.5f, lateral=%8.5f, longitudinal=%8.5f, center lambda=%8.5f, frac max=%8.5f, secondR=%8.5f, significance=%8.5f, 2nd density=%8.5f, ip=%8.5f",
                       eta, et, secondLambda, lateral, longitudinal,
                       centerLambda, fracMax, secondR, significance, secondDensity, ip));

  if (!allFound)
  {
    ATH_MSG_ERROR("Skipping LH calculation! The following variables are missing: " << notFoundList);
    return -999;
  }

  // Get the answer from the underlying ROOT tool
  return m_rootForwardTool->calculate(eta,
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


//=============================================================================
/// Get the name of the current operating point
//=============================================================================
std::string AsgForwardElectronLikelihoodTool::getOperatingPointName() const
{
  return m_WorkingPoint;
}
//=============================================================================
asg::AcceptData AsgForwardElectronLikelihoodTool::accept(const xAOD::IParticle* part) const
{
  //Backwards compatibility
  return accept(Gaudi::Hive::currentContext(), part);
}

asg::AcceptData AsgForwardElectronLikelihoodTool::accept(const EventContext& ctx, const xAOD::IParticle *part) const
{
 if(part->type() == xAOD::Type::Electron){
    const xAOD::Electron* el = static_cast<const xAOD::Electron*>(part);
    return accept(ctx, el);
  }
  ATH_MSG_ERROR("Input is not an electron");
  return m_rootForwardTool->accept();
}
//=============================================================================
double AsgForwardElectronLikelihoodTool::calculate(const xAOD::IParticle* part) const
{
  //Backwards compatibily
  return calculate(Gaudi::Hive::currentContext(), part);
}

double AsgForwardElectronLikelihoodTool::calculate(const EventContext& ctx, const xAOD::IParticle* part) const
{
  if(part->type() == xAOD::Type::Electron){
    const xAOD::Electron* el = static_cast<const xAOD::Electron*>(part);
    return calculate(ctx, el);
  }
  ATH_MSG_ERROR ( "Input is not an electron" );
  return -999;
}


//=============================================================================
// Helper method to get the number of primary vertices
// ( This is horrible! We don't want to iterate over all vertices in the event for each electron!!!
//   This is slow!)
//=============================================================================
unsigned int AsgForwardElectronLikelihoodTool::getNPrimVertices(const EventContext& ctx) const
{
  unsigned int nVtx(0);
  SG::ReadHandle<xAOD::VertexContainer> vtxCont (m_primVtxContKey, ctx); 
  for ( unsigned int i = 0; i < vtxCont->size(); i++ ) {
      const xAOD::Vertex* vxcand = vtxCont->at(i);
      if ( vxcand->nTrackParticles() >= 2 ) nVtx++;
  }
  return nVtx;
}

