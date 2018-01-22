/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/**
   @class AsgElectronIsEMSelector
   @brief Electron isEM selector

   @author Jovan Mitrevski (UCSC) Karsten Koeneke (CERN)
   @date   Dec 2011 - Fab 2012

   11-MAR-2014 convert to ASG tool (Jovan Mitrevski)

*/

#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/ElectronSelectorHelpers.h"
#include "AsgElectronPhotonIsEMSelectorConfigHelper.h"
#include "TElectronIsEMSelector.h"
#include "EGSelectorConfigurationMapping.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "PathResolver/PathResolver.h"
#include "TEnv.h"
#include <cstdint>

//=============================================================================
// Standard constructor
//=============================================================================
AsgElectronIsEMSelector::AsgElectronIsEMSelector(std::string myname) :
  AsgTool(myname),
  m_configFile(""),
  m_rootTool(0)
{

  m_rootTool = new Root::TElectronIsEMSelector(myname.c_str());

  declareProperty("WorkingPoint",m_WorkingPoint="","The Working Point");
  declareProperty("ConfigFile",m_configFile="",
		  "The config file to use (if not setting cuts one by one)");
  
  // Name of the quality to use
  declareProperty("isEMMask",
		  m_rootTool->isEMMask=egammaPID::EgPidUndefined, //All pass by default, if not specified
		  "The mask to use");

  // Boolean to use TRT outliers
  declareProperty("useTRTOutliers",m_rootTool->useTRTOutliers=false,
		  "Boolean to use TRT outliers");

  // Boolean to use TRT Xenon Hits 
  declareProperty("useTRTXenonHits",
		  m_rootTool->useTRTXenonHits = false, 
		  "Boolean to use TRT Xenon Hits"); 

  // Eta binning
  declareProperty("CutBinEta",m_rootTool->CutBinEta,
		  "Eta binning");

  // ET binning
  declareProperty("CutBinET",m_rootTool->CutBinET,
		  "ET binning");
  // Cut on fraction of energy to use 1st sampling
  declareProperty("CutF1",m_rootTool->CutF1,
		  "Cut on fraction of energy to use 1st sampling");
  // Cut on hadronic leakage
  declareProperty("CutHadLeakage",m_rootTool->CutHadLeakage,
		  "Cut on hadronic leakage");
  // Cut on lateral shower shape in 2nd sampling
  declareProperty("CutRphi33",m_rootTool->CutRphi33,
		  "Cut on lateral shower shape in 2nd sampling");
  // Cut on longitudinal shower shape in 2nd sampling
  declareProperty("CutReta37",m_rootTool->CutReta37,
		  "Cut on longitudinal shower shape in 2nd sampling");
  // Cut on shower width in 2nd sampling
  declareProperty("CutWeta2c",m_rootTool->CutWeta2c,
		  "Cut on shower width in 2nd sampling");
  // Cut on Demax 2 in 1st sampling
  declareProperty("CutDeltaEmax2",m_rootTool->CutDeltaEmax2,
		  "Cut on Demax 2 in 1st sampling");
  // Cut on Delta E in 1st sampling
  declareProperty("CutDeltaE",m_rootTool->CutDeltaE,
		  "Cut on Delta E in 1st sampling");
  // Cut on (Emax1-Emax2)/(Emax1+Emax2) in 1st sampling
  declareProperty("CutDEmaxs1",m_rootTool->CutDEmaxs1,
		  "Cut on (Emax1-Emax2)/(Emax1+Emax2) in 1st sampling");
  // Cut on total shower width in 1st sampling
  declareProperty("CutWtot",m_rootTool->CutWtot,
		  "Cut on total shower width in 1st sampling");
  // Cut on shower width in 3 strips
  declareProperty("CutWeta1c",m_rootTool->CutWeta1c,
		  "Cut on shower width in 3 strips");
  // Cut on fraction of energy outside core in 1st sampling
  declareProperty("CutFracm",m_rootTool->CutFracm,
		  "Cut on fraction of energy outside core in 1st sampling");

  declareProperty("useF3core",m_useF3core = false,
		  "Cut on f3 or f3core?");
  // Cut on fraction of energy to use 3rd sampling
  declareProperty("CutF3",m_rootTool->CutF3 ,
		  "Cut on fraction of energy to use 3rd sampling (f3 or f3core)");

  // cut on b-layer
  declareProperty("CutBL",m_rootTool->CutBL,
		  "Cut on b-layer");
  // cut on pixel hits
  declareProperty("CutPi",m_rootTool->CutPi,
		  "Cut on pixel hits");
  // cut on precision hits
  declareProperty("CutSi",m_rootTool->CutSi,
		  "Cut on precision hits");
  // cut on A0
  declareProperty("CutA0",m_rootTool->CutA0,
		  "Cut on transverse impact parameter");
  // cut on A0 for tight menu
  declareProperty("CutA0Tight",m_rootTool->CutA0Tight,
		  "Cut on transverse impact parameter for tight selection");
  // cut on Delta eta
  declareProperty("CutDeltaEta",m_rootTool->CutDeltaEta,
		  "cut on Delta eta");
  // cut on Delta eta for tight selection
  declareProperty("CutDeltaEtaTight",m_rootTool->CutDeltaEtaTight,
		  "cut on Delta eta for tight selection");
  // cut min on Delta phi (negative)
  declareProperty("CutminDeltaPhi",m_rootTool->CutminDeltaPhi,
		  "cut min on Delta phi");
  // cut max on Delta phi (negative)
  declareProperty("CutmaxDeltaPhi",m_rootTool->CutmaxDeltaPhi,
		  "cut max on Delta phi");
  // cut min on E/p
  declareProperty("CutminEp",m_rootTool->CutminEp,
		  "Cut min on E/p");
  // cut max on E/p
  declareProperty("CutmaxEp",m_rootTool->CutmaxEp,
		  "Cut max on E/p");

  // cuts on TRT
  declareProperty("CutBinEta_TRT",m_rootTool->CutBinEta_TRT,
		  "Eta binning in TRT");

  // cuts on TRT
  declareProperty("CutBinET_TRT",m_rootTool->CutBinET_TRT,
		  "ET binning in TRT");

  // cut on Number of TRT hits
  declareProperty("CutNumTRT",m_rootTool->CutNumTRT,
		  "cut on Number of TRT hits");
  // cut on Ratio of TR hits to Number of TRT hits
  declareProperty("CutTRTRatio",m_rootTool->CutTRTRatio,
		  "Cut on Ratio of TR hits to Number of TRT hits");
  // cut on Ratio of TR hits to Number of TRT hits for 90% efficiency
  declareProperty("CutTRTRatio90",m_rootTool->CutTRTRatio90,
		  "cut on Ratio of TR hits to Number of TRT hits for 90% efficiency");
  //cut on eProbabilityHT new TRT PID tool
  declareProperty("CutEProbabilityHT",m_rootTool->CutEProbabilityHT,
		  "Cut on eProabbiility new TRT PID Tool");
  
  // for the trigger needs:
  declareProperty("caloOnly", m_caloOnly=false, "Flag to tell the tool if its a calo only cutbase"); 
  declareProperty("trigEtTh", m_trigEtTh=-999. , "Trigger threshold"); 
  

}


//=============================================================================
// Standard destructor
//=============================================================================
AsgElectronIsEMSelector::~AsgElectronIsEMSelector()
{
  if(finalize().isFailure()){
    ATH_MSG_ERROR ( "Failure in AsgElectronIsEMSelector finalize()");
  }
  delete m_rootTool;
}

StatusCode AsgElectronIsEMSelector::initialize()
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;

  if(!m_WorkingPoint.empty()){
    m_configFile=AsgConfigHelper::findConfigFile(m_WorkingPoint,EgammaSelectors::ElectronCutPointToConfFile);
  }

  if(!m_configFile.empty()) {    
    //find the file and read it in
    std::string filename = PathResolverFindCalibFile( m_configFile);
    if(filename=="")
      { 
	ATH_MSG_ERROR("Could not locate " << m_configFile );
	sc = StatusCode::FAILURE;
	return sc;      
      } 
    ATH_MSG_DEBUG("Configfile to use  " << m_configFile );
    TEnv env(filename.c_str());
    
    ///------- Read in the TEnv config ------///
    
    //Override the mask via the config only if it is not set 
    if(m_rootTool->isEMMask==egammaPID::EgPidUndefined){ 

      int default_mask = static_cast<int>(egammaPID::EgPidUndefined); 
      int mask(env.GetValue("isEMMask",default_mask));
      m_rootTool->isEMMask=static_cast<unsigned int> (mask);

    }
    //
    ATH_MSG_DEBUG("Read in the TEnv config ");
    //From here on the conf ovverides all other properties
    bool useTRTOutliers(env.GetValue("useTRTOutliers", true));
    m_rootTool->useTRTOutliers =useTRTOutliers;
    bool  useTRTXenonHits(env.GetValue(" useTRTXenonHits", false));
    m_rootTool->useTRTXenonHits =useTRTXenonHits;
    
    ///------- Use helpers to read in the cut arrays ------///
    m_rootTool->CutBinEta  =AsgConfigHelper::HelperFloat("CutBinEta",env);
    m_rootTool->CutBinET = AsgConfigHelper::HelperFloat("CutBinET",env);
    m_rootTool->CutF1 = AsgConfigHelper::HelperFloat("CutF1",env);
    m_rootTool->CutHadLeakage = AsgConfigHelper::HelperFloat("CutHadLeakage",env);
    m_rootTool->CutReta37 = AsgConfigHelper::HelperFloat("CutReta37",env);
    m_rootTool->CutRphi33 = AsgConfigHelper::HelperFloat("CutRphi33",env);
    m_rootTool->CutWeta2c = AsgConfigHelper::HelperFloat("CutWeta2c",env);
    m_rootTool->CutDeltaEmax2 = AsgConfigHelper::HelperFloat("CutDeltaEmax2",env);
    m_rootTool->CutDeltaE = AsgConfigHelper::HelperFloat("CutDeltaE",env);
    m_rootTool->CutDEmaxs1 = AsgConfigHelper::HelperFloat("CutDEmaxs1",env);
    m_rootTool->CutDeltaE = AsgConfigHelper::HelperFloat("CutDeltaE",env);
    m_rootTool->CutWtot = AsgConfigHelper::HelperFloat("CutWtot",env);
    m_rootTool->CutWeta1c = AsgConfigHelper::HelperFloat("CutWeta1c",env);
    m_rootTool->CutFracm = AsgConfigHelper::HelperFloat("CutFracm",env);
    m_rootTool->CutF3 = AsgConfigHelper::HelperFloat("CutF3",env);
    m_rootTool->CutBL = AsgConfigHelper::HelperInt("CutBL",env);
    m_rootTool->CutPi = AsgConfigHelper::HelperInt("CutPi",env);
    m_rootTool->CutSi = AsgConfigHelper::HelperInt("CutSi",env);
    m_rootTool->CutA0 = AsgConfigHelper::HelperFloat("CutA0",env);
    m_rootTool->CutA0Tight = AsgConfigHelper::HelperFloat("CutA0Tight",env);
    m_rootTool->CutDeltaEta = AsgConfigHelper::HelperFloat("CutDeltaEta",env);
    m_rootTool->CutDeltaEtaTight = AsgConfigHelper::HelperFloat("CutDeltaEtaTight",env);
    m_rootTool->CutminDeltaPhi = AsgConfigHelper::HelperFloat("CutminDeltaPhi",env);    
    m_rootTool->CutmaxDeltaPhi = AsgConfigHelper::HelperFloat("CutmaxDeltaPhi",env);    
    m_rootTool->CutminEp = AsgConfigHelper::HelperFloat("CutminEp",env);    
    m_rootTool->CutmaxEp = AsgConfigHelper::HelperFloat("CutmaxEp",env);    
    m_rootTool->CutBinEta_TRT = AsgConfigHelper::HelperFloat("CutBinEta_TRT",env); 
    m_rootTool->CutBinET_TRT = AsgConfigHelper::HelperFloat("CutBinET_TRT",env); 
    m_rootTool->CutNumTRT = AsgConfigHelper::HelperFloat("CutNumTRT",env);        
    m_rootTool->CutTRTRatio = AsgConfigHelper::HelperFloat("CutTRTRatio",env);    
    m_rootTool->CutTRTRatio90 = AsgConfigHelper::HelperFloat("CutTRTRatio90",env);  
    m_rootTool->CutEProbabilityHT = AsgConfigHelper::HelperFloat("CutEProbabilityHT",env);
    
  } else {
    ATH_MSG_INFO("Conf file empty. Just user Input");
  }

  ATH_MSG_INFO("operating point : " << this->getOperatingPointName() << " with mask: "<< m_rootTool->isEMMask  );

  // Get the message level and set the underlying ROOT tool message level accordingly
  m_rootTool->msg().setLevel(this->msg().level());

  // We need to initialize the underlying ROOT TSelectorTool
  if ( 0 == m_rootTool->initialize() )
    {
      ATH_MSG_ERROR("Could not initialize the TElectronIsEMSelector!");
      sc = StatusCode::FAILURE;
      return sc;
    }

  return sc ;
}


//=============================================================================
// finalize method (now called by destructor)
//=============================================================================
StatusCode AsgElectronIsEMSelector::finalize()
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;

  if ( !(m_rootTool->finalize()) )
    {
      ATH_MSG_ERROR("Something went wrong at finalize!");
      sc = StatusCode::FAILURE;
    }

  return sc ;
}

//=============================================================================
// The main accept method: the actual cuts are applied here 
//=============================================================================
const Root::TAccept& AsgElectronIsEMSelector::accept( const xAOD::IParticle* part ) const{

  ATH_MSG_DEBUG("Entering accept( const IParticle* part )");
  if(part->type()==xAOD::Type::Electron || part->type()==xAOD::Type::Photon){
    return accept(static_cast<const xAOD::Egamma*> (part));
  }
  else{
    ATH_MSG_ERROR("AsgElectronIsEMSelector::could not convert argument to Electron/Photon");
    return m_acceptDummy;
  }
}

const Root::TAccept& AsgElectronIsEMSelector::accept( const xAOD::Egamma* eg ) const{

  ATH_MSG_DEBUG("Entering accept( const Egamma* part )");  
  if ( eg ){
    StatusCode sc = execute(eg);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("could not calculate isEM");
      return m_acceptDummy;
    }
    return m_rootTool->fillAccept();
  }
  else{
    ATH_MSG_ERROR("AsgElectronIsEMSelector::accept was given a bad argument");
    return m_acceptDummy;
  }
}

const Root::TAccept& AsgElectronIsEMSelector::accept( const xAOD::Electron* el) const{
  ATH_MSG_DEBUG("Entering accept( const Electron* part )");  
  return accept(static_cast<const xAOD::Egamma*> (el));
}

const Root::TAccept& AsgElectronIsEMSelector::accept( const xAOD::Photon* ph) const{
  ATH_MSG_DEBUG("Entering accept( const Photon* part )");  
  return accept(static_cast<const xAOD::Egamma*> (ph));  
}

unsigned int AsgElectronIsEMSelector::IsemValue() const {
  return m_rootTool->isEM(); 
}

//=============================================================================
/// Get the name of the current operating point
//=============================================================================
std::string AsgElectronIsEMSelector::getOperatingPointName() const{

  if(!m_WorkingPoint.empty()){
    return m_WorkingPoint;
  }
  else if (m_rootTool->isEMMask == egammaPID::ElectronLoosePP){ return "Loose"; }
  else if (m_rootTool->isEMMask == egammaPID::ElectronMediumPP ){ return "Medium"; }
  else if (m_rootTool->isEMMask == egammaPID::ElectronTightPP){ return "Tight"; }
  else if (m_rootTool->isEMMask == egammaPID::ElectronLoose1){return "Loose1";}
  else if (m_rootTool->isEMMask == egammaPID::ElectronMedium1){return "Medium1";}
  else if (m_rootTool->isEMMask == egammaPID::ElectronTight1){return "Tight1";}
  else if (m_rootTool->isEMMask == egammaPID::ElectronLooseHLT){return "LooseHLT";}
  else if (m_rootTool->isEMMask == egammaPID::ElectronMediumHLT){return "MediumHLT";}
  else if (m_rootTool->isEMMask == egammaPID::ElectronTightHLT){return "TightHLT";}
  else if (m_rootTool->isEMMask == 0){ return "0 No cuts applied"; }
  else{
    ATH_MSG_INFO( "Didn't recognize the given operating point with mask: " << m_rootTool->isEMMask );
    return "";
  }
}

///==========================================================================================//

// ==============================================================
StatusCode AsgElectronIsEMSelector::execute(const xAOD::Egamma* eg ) const{
  //
  // Particle identification for electrons based on cuts
  //
  ATH_MSG_DEBUG("entering execute(const Egamma* eg...)");
  // initialisation
  unsigned int iflag = 0; 
  // protection against null pointer
  if (eg==0) {
    // if object is bad then use the bit for "bad eta"
    ATH_MSG_ERROR("exiting because el is NULL");
    iflag = (0x1 << egammaPID::ClusterEtaRange_Electron); 
    m_rootTool->setIsEM(iflag);
    return StatusCode::SUCCESS; 
  }
  // retrieve associated cluster
  const xAOD::CaloCluster* cluster  = eg->caloCluster(); 
  if ( cluster == 0 ) {
    // if object is bad then use the bit for "bad eta"
    ATH_MSG_ERROR("exiting because cluster is NULL");
    iflag = (0x1 << egammaPID::ClusterEtaRange_Electron); 
    m_rootTool->setIsEM(iflag);
    return StatusCode::SUCCESS; 
  }
  // eta position in second sampling
  const float eta2   = fabsf(cluster->etaBE(2));
  // energy in calorimeter 
  const double energy =  cluster->e();
  // transverse energy of the electron (using the track eta) 
  // const double et = el->pt();
  double et = ( cosh(eta2) != 0.) ? energy/cosh(eta2) : 0.; ;

  //see if we have an electron, with track, for eta 
  const xAOD::Electron* el =0;
  if(eg->type()==xAOD::Type::Electron) {
    el =static_cast<const xAOD::Electron*> (eg);
  }
  if (el && el->trackParticle() && !m_caloOnly){ 
    et  = ( cosh(el->trackParticle()->eta()) != 0.) ? energy/cosh(el->trackParticle()->eta()) : 0.; 
  }  

  //Call the calocuts using the egamma object
  iflag = calocuts_electrons(eg, eta2, et, m_trigEtTh, 0);
  
  //Call the calo cuts using the el , if available and we want to apply them
  if(el && el->trackParticle() &&!m_caloOnly ){
    iflag = TrackCut(el, eta2, et, energy, iflag);
  }
  
  m_rootTool->setIsEM(iflag);  
  return StatusCode::SUCCESS;

}

// ======================================================================
unsigned int AsgElectronIsEMSelector::calocuts_electrons(const xAOD::Egamma* eg, 
							 float eta2, double et,
							 double trigEtTh,
							 unsigned int iflag) const
{
 
  //
  // apply cut-based selection based on calo information
  // eg : xAOD::Electron object
  // trigETthr : threshold in ET to apply the cuts at trigger level
  // iflag: the starting isEM
  //

  float Reta(0), Rphi(0), Rhad1(0), Rhad(0), e277(0), weta1c(0), weta2c(0), 
    f1(0), emax2(0), Eratio(0), DeltaE(0), wtot(0), fracm(0), f3(0);

  bool allFound = true;
  // Reta
  allFound = allFound && eg->showerShapeValue(Reta, xAOD::EgammaParameters::Reta);
  // Rphi
  allFound = allFound && eg->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi);
  // transverse energy in 1st scintillator of hadronic calorimeter
  allFound = allFound && eg->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1);
  // transverse energy in hadronic calorimeter
  allFound = allFound && eg->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad);
  // E(7*7) in 2nd sampling
  allFound = allFound && eg->showerShapeValue(e277, xAOD::EgammaParameters::e277);
  // shower width in 3 strips in 1st sampling
  allFound = allFound && eg->showerShapeValue(weta1c, xAOD::EgammaParameters::weta1);
  // shower width in 2nd sampling
  allFound = allFound && eg->showerShapeValue(weta2c, xAOD::EgammaParameters::weta2);
  // fraction of energy reconstructed in the 1st sampling
  allFound = allFound && eg->showerShapeValue(f1, xAOD::EgammaParameters::f1);
  // E of 2nd max between max and min in strips
  allFound = allFound && eg->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio);
  // E of 1st max in strips
  allFound = allFound && eg->showerShapeValue(DeltaE, xAOD::EgammaParameters::DeltaE);
  // total shower width in 1st sampling
  allFound = allFound && eg->showerShapeValue(wtot, xAOD::EgammaParameters::wtots1);
  // E(+/-3)-E(+/-1)/E(+/-1)
  allFound = allFound && eg->showerShapeValue(fracm, xAOD::EgammaParameters::fracs1);

  if (m_useF3core) {
    allFound = allFound && eg->showerShapeValue(f3, xAOD::EgammaParameters::f3core);
  } else {
    allFound = allFound && eg->showerShapeValue(f3, xAOD::EgammaParameters::f3);
  }    

  if (!allFound) {
    // if object is bad then use the bit for "bad eta"
    ATH_MSG_WARNING("Have some variables missing.");
    iflag = (0x1 << egammaPID::ClusterEtaRange_Electron); 
    return iflag;
  }
   
  // For cut-based triggers above 20 GeV threshold, the online cut values on the discriminant variables are always taken from the 20 GeV optimisation.
  //  if(et > 20000 )  { if(trigEtTh > 0) et = trigEtTh*1.01; }
  
  
  return m_rootTool->calocuts_electrons(eta2,
					et,
                                        Reta, //replacing e233
					Rphi,  //replacing e237,
					Rhad1, //replacing ethad1,
					Rhad, //replacing ethad,
					e277,
					weta1c,
					weta2c,
					f1,
					emax2, // emax2
					Eratio, // emax
					DeltaE, //emin,
					wtot,
					fracm,
					f3,
					iflag,
                                        trigEtTh);
}

//================================================================
unsigned int AsgElectronIsEMSelector::TrackCut(const xAOD::Electron* eg, 
					       float eta2, double et, double energy,
					       unsigned int iflag) const
{
  // apply track cuts for electron identification
  //  - Track quality cuts
  //  - (eta,phi) and E/p matching between ID and ECAL
  //  - use of TRT
  // eg : egamma object
  // iflag: the starting isEM to use
  //  
  // retrieve associated track
  const xAOD::TrackParticle* t  = eg->trackParticle();    

  //ATH_MSG_DEBUG("CaloCutsOnly is false");
  // protection against bad pointers
  if ( t == 0 ) {
    ATH_MSG_ERROR("Something is bad with the variables as passed");
    // if object is bad then use the bit for "bad eta"
    iflag = (0x1 << egammaPID::ClusterEtaRange_Electron); 
    return StatusCode::SUCCESS; 
  }
 
  // Track quality cuts
  uint8_t nSiHitsPlusDeadSensors = ElectronSelectorHelpers::numberOfSiliconHitsAndDeadSensors(t);
  uint8_t nPixHitsPlusDeadSensors = ElectronSelectorHelpers::numberOfPixelHitsAndDeadSensors(t);
  bool passBLayerRequirement = ElectronSelectorHelpers::passBLayerRequirement(t);

  // TRT information
  uint8_t nTRThigh          = 0;
  uint8_t nTRThighOutliers  = 0;
  uint8_t nTRT              = 0;
  uint8_t nTRTOutliers      = 0;
  uint8_t nTRTXenonHits     = 0;
  float   TRT_PID           = 0.0; 

  bool allFound = true;

  allFound = allFound && t->summaryValue(nTRThigh, xAOD::numberOfTRTHighThresholdHits);
  allFound = allFound && t->summaryValue(nTRThighOutliers, xAOD::numberOfTRTHighThresholdOutliers);
  allFound = allFound && t->summaryValue(nTRT, xAOD::numberOfTRTHits);
  allFound = allFound && t->summaryValue(nTRTOutliers, xAOD::numberOfTRTOutliers);
  allFound = allFound && t->summaryValue(nTRTXenonHits, xAOD::numberOfTRTXenonHits);
  allFound = allFound && t->summaryValue(TRT_PID, xAOD::eProbabilityHT);

  const float trackd0 = fabsf(t->d0());
  
  // Delta eta,phi matching
  float deltaeta, deltaphi;

  allFound = allFound && eg->trackCaloMatchValue(deltaeta, xAOD::EgammaParameters::deltaEta1);
  allFound = allFound && eg->trackCaloMatchValue(deltaphi, xAOD::EgammaParameters::deltaPhi2);
  
  // E/p
  const double ep = energy * fabs(t->qOverP());

  if (!allFound) {
    // if object is bad then use the bit for "bad eta"
    ATH_MSG_WARNING("Have some variables missing.");
    iflag = (0x1 << egammaPID::ClusterEtaRange_Electron); 
    return iflag;
  }

  return m_rootTool->TrackCut(eta2,
			      et,
                              passBLayerRequirement,
                              nPixHitsPlusDeadSensors,
                              nSiHitsPlusDeadSensors,
			      nTRThigh,
			      nTRThighOutliers,
			      nTRT,
			      nTRTOutliers,
			      nTRTXenonHits,
			      TRT_PID,
			      trackd0,
			      deltaeta,
			      deltaphi,
			      ep,
			      iflag);
}

const Root::TAccept& AsgElectronIsEMSelector::getTAccept( ) const{
    return m_rootTool->getTAccept();
}

//  LocalWords:  const el
