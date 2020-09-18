/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/**
   @class AsgForwardElectronIsEMSelector
   @brief Electron isEM selector

   @author Jovan Mitrevski (UCSC) Karsten Koeneke (CERN)
   @date   Dec 2011 - Fab 2012

   11-MAR-2014 convert to ASG tool (Jovan Mitrevski)

*/
#include "ElectronPhotonSelectorTools/AsgForwardElectronIsEMSelector.h"
#include "AsgElectronPhotonIsEMSelectorConfigHelper.h"
#include "TForwardElectronIsEMSelector.h"
#include "EGSelectorConfigurationMapping.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/Vertex.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgTools/CurrentContext.h"
#include "PathResolver/PathResolver.h"
#include "TEnv.h"
#include <cstdint>
//=============================================================================
// Standard constructor
//=============================================================================
AsgForwardElectronIsEMSelector::AsgForwardElectronIsEMSelector(const std::string& myname) :
  AsgTool(myname),
  m_configFile{""},
  m_rootForwardTool{nullptr}
{
  m_rootForwardTool = new Root::TForwardElectronIsEMSelector(myname.c_str());

  declareProperty("WorkingPoint",m_WorkingPoint="","The Working Point");
  declareProperty("ConfigFile",m_configFile="","The config file to use (if not setting cuts one by one)");
  declareProperty("usePVContainer", m_usePVCont=true, "Whether to use the PV container");
  declareProperty("nPVdefault", m_nPVdefault = 0, "The default number of PVs if not counted");

  // Name of the quality to use
  declareProperty("isEMMask",
		  m_rootForwardTool->m_isEMMask=egammaPID::EgPidUndefined, //All pass by default, if not specified
		  "The mask to use");
  // Eta binning
  declareProperty("CutBinEta_ForwardElectron",       m_rootForwardTool->m_cutBinEta_ForwardElectron,
		  "Eta binning");
  // NPV  binning
  declareProperty("CutVxp_ForwardElectron",          m_rootForwardTool->m_cutVxp_ForwardElectron,
		  "nvtx binning");
  // Cut on lateral shower shape
  declareProperty("CutLATERAL_ForwardElectron",      m_rootForwardTool->m_cutLATERAL_ForwardElectron,
		  "Cut on lateral shower shape in 2nd sampling");
  // Cut on secondLambda
  declareProperty("CutSECONDLAMBDA_ForwardElectron", m_rootForwardTool->m_cutSECONDLAMBDA_ForwardElectron,
		  "Cut on secondLambda");
  // Cut on longitudinal
  declareProperty("CutLONGITUDINAL_ForwardElectron", m_rootForwardTool->m_cutLONGITUDINAL_ForwardElectron,
		  "Cut on longitudinal");
  // Cut on fracMax
  declareProperty("CutCELLMAXFRAC_ForwardElectron",  m_rootForwardTool->m_cutCELLMAXFRAC_ForwardElectron,
		  "Cut on fracMax");
  // Cut on centerlambda
  declareProperty("CutCENTERLAMBDA_ForwardElectron", m_rootForwardTool->m_cutCENTERLAMBDA_ForwardElectron,
		  "Cut on centerlambda");
  // Cut on secondR
  declareProperty("CutSECONDR_ForwardElectron",      m_rootForwardTool->m_cutSECONDR_ForwardElectron ,
		  "Cut on secondR)");
}


//=============================================================================
// Standard destructor
//=============================================================================
AsgForwardElectronIsEMSelector::~AsgForwardElectronIsEMSelector()
{
  delete m_rootForwardTool;
}

StatusCode AsgForwardElectronIsEMSelector::initialize()
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;

 if(!m_WorkingPoint.empty()){
    m_configFile=AsgConfigHelper::findConfigFile(m_WorkingPoint,EgammaSelectors::ForwardElectronCutPointToConfFile);
  }
  
  if(!m_configFile.empty()) {
    //find the file and read it in
    std::string filename = PathResolverFindCalibFile( m_configFile);
    if(filename.empty())
    { 
        ATH_MSG_ERROR("Could not locate " << m_configFile );
        sc = StatusCode::FAILURE;
        return sc;      
    } 
    ATH_MSG_DEBUG("Configfile to use  " << m_configFile );
    TEnv env(filename.c_str());

        
    ///------- Read in the TEnv config ------///

    //Override the mask via the config only if it is not set 
    if(m_rootForwardTool->m_isEMMask==egammaPID::EgPidUndefined){ 
      unsigned int mask(env.GetValue("isEMMask",static_cast<int>(egammaPID::EgPidUndefined)));
      m_rootForwardTool->m_isEMMask=mask;
    }
    //
    ATH_MSG_DEBUG("Read in the TEnv config ");
    ///------- Use helpers to read in the cut arrays ------///
    m_rootForwardTool->m_cutBinEta_ForwardElectron       = AsgConfigHelper::HelperFloat("CutBinEta_ForwardElectron",env);
    m_rootForwardTool->m_cutVxp_ForwardElectron          = AsgConfigHelper::HelperFloat("CutVxp_ForwardElectron",env);
    m_rootForwardTool->m_cutSECONDLAMBDA_ForwardElectron = AsgConfigHelper::HelperFloat("CutSECONDLAMBDA_ForwardElectron",env);
    m_rootForwardTool->m_cutLATERAL_ForwardElectron      = AsgConfigHelper::HelperFloat("CutLATERAL_ForwardElectron",env);
    m_rootForwardTool->m_cutLONGITUDINAL_ForwardElectron = AsgConfigHelper::HelperFloat("CutLONGITUDINAL_ForwardElectron",env);
    m_rootForwardTool->m_cutCELLMAXFRAC_ForwardElectron  = AsgConfigHelper::HelperFloat("CutCELLMAXFRAC_ForwardElectron",env);
    m_rootForwardTool->m_cutCENTERLAMBDA_ForwardElectron = AsgConfigHelper::HelperFloat("CutCENTERLAMBDA_ForwardElectron",env);
    m_rootForwardTool->m_cutSECONDR_ForwardElectron      = AsgConfigHelper::HelperFloat("CutSECONDR_ForwardElectron", env);
  } else {
    ATH_MSG_INFO("Conf file empty. Just user Input");
  }

  ATH_CHECK( m_primVtxContKey.initialize(m_usePVCont) );
 
  ATH_MSG_INFO("operating point : " << this->getOperatingPointName() << " with mask: "<< m_rootForwardTool->m_isEMMask  );

  // Get the message level and set the underlying ROOT tool message level accordingly
  m_rootForwardTool->msg().setLevel(this->msg().level());
  // We need to initialize the underlying ROOT TSelectorTool
  if ( m_rootForwardTool->initialize().isFailure() )
    {
      ATH_MSG_ERROR("Could not initialize the TForwardElectronIsEMSelector!");
      sc = StatusCode::FAILURE;
      return sc;
    }

  return sc ;
}


//=============================================================================
// finalize method (now called by destructor)
//=============================================================================
StatusCode AsgForwardElectronIsEMSelector::finalize()
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;

  return sc ;
}

//=============================================================================
// return the accept info object
//=============================================================================

const asg::AcceptInfo& AsgForwardElectronIsEMSelector::getAcceptInfo() const
{
    return m_rootForwardTool->getAcceptInfo();
}

//=============================================================================
// The main accept method: the actual cuts are applied here 
//=============================================================================
asg::AcceptData
AsgForwardElectronIsEMSelector::accept(const xAOD::IParticle* part ) const{
  return accept(Gaudi::Hive::currentContext(), part);
}

asg::AcceptData
AsgForwardElectronIsEMSelector::accept(const EventContext& ctx, const xAOD::IParticle* part ) const{

  ATH_MSG_DEBUG("Entering accept( const IParticle* part )");
  if(part->type()==xAOD::Type::Electron || part->type()==xAOD::Type::Photon){
    return accept(ctx, static_cast<const xAOD::Egamma*> (part));
  }
  
    ATH_MSG_ERROR("AsgForwardElectronIsEMSelector::could not convert argument to Electron/Photon");
    return m_rootForwardTool->accept();
  
}

asg::AcceptData
AsgForwardElectronIsEMSelector::accept( const EventContext& ctx, const xAOD::Egamma* eg ) const{

  ATH_MSG_DEBUG("Entering accept( const Egamma* part )");
  if ( eg ){
    unsigned int isEM = ~0;
    StatusCode sc = execute(ctx, eg, isEM);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("could not calculate isEM");
      return m_rootForwardTool->accept();
    }
    return m_rootForwardTool->fillAccept(isEM);
  }
  
    ATH_MSG_ERROR("AsgForwardElectronIsEMSelector::accept was given a bad argument");
    return m_rootForwardTool->accept();
  
}

asg::AcceptData
AsgForwardElectronIsEMSelector::accept( const EventContext& ctx, const xAOD::Electron* el) const{
  ATH_MSG_DEBUG("Entering accept( const EventContext& ctx, const Electron* part )");
  return accept(ctx, static_cast<const xAOD::Egamma*> (el));
}

asg::AcceptData
AsgForwardElectronIsEMSelector::accept( const EventContext& ctx, const xAOD::Photon* ph) const{
  ATH_MSG_DEBUG("Entering accept( const EventContext& ctx, const Photon* part )");
  return accept(ctx, static_cast<const xAOD::Egamma*> (ph));
}

//=============================================================================
/// Get the name of the current operating point
//=============================================================================
std::string AsgForwardElectronIsEMSelector::getOperatingPointName() const
{

  if (m_rootForwardTool->m_isEMMask == egammaPID::ID_ForwardElectron){ return "Forw Id"; }
  
    ATH_MSG_INFO( "Didn't recognize the given operating point with mask: " << m_rootForwardTool->m_isEMMask );
    return "";
  
}

///==========================================================================================//
StatusCode AsgForwardElectronIsEMSelector::execute(const EventContext& ctx, const xAOD::Egamma* eg, unsigned int& isEM) const{
  //
  // Particle identification for electrons based on cuts
  //
  ATH_MSG_DEBUG("entering execute(const Egamma* eg...)");
  // initialisation
  isEM = 0; 
  // protection against null pointer
  if (eg==nullptr) {
    // if object is bad then use the bit for "bad eta"
    ATH_MSG_DEBUG("exiting because el is NULL");
    isEM = (0x1 << egammaPID::BinEta_ForwardElectron); 
    return StatusCode::SUCCESS; 
  }

  // retrieve associated cluster
  const xAOD::CaloCluster* cluster  = eg->caloCluster(); 
  if ( cluster == nullptr ) {
    // if object is bad then use the bit for "bad eta"
    ATH_MSG_DEBUG("exiting because cluster is NULL");
    isEM = (0x1 << egammaPID::BinEta_ForwardElectron); 
    return StatusCode::SUCCESS; 
  }


  float nvtx = static_cast<int>(m_usePVCont ? this->getNPrimVertices(ctx) : m_nPVdefault);
  float eta = fabs(cluster->etaBE(2)) ;

  //see if we have an electron, with track, for eta 
  const xAOD::Electron* el = nullptr;
  if(eg->type()==xAOD::Type::Electron) {
    el =static_cast<const xAOD::Electron*> (eg);
  }

  if(el){
    eta = fabs(el->eta());
  }
  //Call the calocuts using the egamma object
  isEM = calocuts_electrons(eg, eta, nvtx, 0); 

  return StatusCode::SUCCESS;

}

// ======================================================================
//
unsigned int AsgForwardElectronIsEMSelector::calocuts_electrons(const xAOD::Egamma* eg,
								float eta, float nvtx,
								unsigned int iflag) const
{


  // apply cut-based selection based on calo information
  // eg : xAOD::Electron object
  // trigETthr : threshold in ET to apply the cuts at trigger level
  // iflag: the starting isEM
  //

  //std::cout<<" NVTX "<<nvtx<<" eta2 "<<eta2<<" eta "<<eta2<<std::endl;
  const xAOD::CaloCluster* cluster2 = eg->caloCluster();
  double secondLambda(0);
  double lateral(0);
  double longitudinal(0);
  double fracMax(0);
  double centerLambda(0);
  double secondR(0);

  bool allFound = true;

  // secondLambda
  allFound = allFound && cluster2->retrieveMoment(xAOD::CaloCluster::SECOND_LAMBDA,secondLambda);
  // lateral
  allFound = allFound && cluster2->retrieveMoment(xAOD::CaloCluster::LATERAL,lateral);
  // longitudinal
  allFound = allFound && cluster2->retrieveMoment(xAOD::CaloCluster::LONGITUDINAL, longitudinal);
  // fracMax
  allFound = allFound && cluster2->retrieveMoment( xAOD::CaloCluster::ENG_FRAC_MAX, fracMax);
  // secondR
  allFound = allFound && cluster2->retrieveMoment( xAOD::CaloCluster::SECOND_R, secondR); 
  // centerlambda
  allFound = allFound && cluster2->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,centerLambda); 

  if (!allFound) {
    // if object is bad then use the bit for "bad eta"
    ATH_MSG_WARNING("Have some variables missing.");
    iflag = (0x1 << egammaPID::BinEta_ForwardElectron);
    return iflag;
  }

  return m_rootForwardTool->calocuts_electrons(eta,
					       nvtx,
					       secondLambda, 
					       lateral,  
					       longitudinal, 
					       fracMax, 
					       centerLambda,
					       secondR,
					       iflag);
}

//=============================================================================
//// Helper method to get the number of primary vertices
//// ( This is horrible! We don't want to iterate over all vertices in the event for each electron!!! 
////   This is slow!)
////=============================================================================
unsigned int AsgForwardElectronIsEMSelector::getNPrimVertices(const EventContext& ctx) const
{
  unsigned int nVtx(0);
  SG::ReadHandle<xAOD::VertexContainer> vtxCont (m_primVtxContKey, ctx); 
  if (!vtxCont.isValid()){
    return nVtx;
  }
  for ( unsigned int i = 0; i < vtxCont->size(); i++ ) {
    const xAOD::Vertex* vtxcand = vtxCont->at(i);
    if ( vtxcand->nTrackParticles() >= 3 ) nVtx++;
  }
  return nVtx;
}
