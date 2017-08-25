/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/**
   @class AsgPhotonIsEMSelector
   @brief Photon isEM selector
   @author Jovan Mitrevski Karsten Koeneke
   @date   Dec 2011 - Fab 2012
   11-MAR-2014 convert to ASG tool
*/

// Include this class's header
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "AsgElectronPhotonIsEMSelectorConfigHelper.h"
#include "TPhotonIsEMSelector.h"
#include "EGSelectorConfigurationMapping.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "PathResolver/PathResolver.h"
#include "TEnv.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

//=============================================================================
// Standard constructor
//=============================================================================
AsgPhotonIsEMSelector::AsgPhotonIsEMSelector(std::string myname) :
  AsgTool(myname),
  m_configFile(""),
  m_rootTool(0)
{

  m_rootTool = new Root::TPhotonIsEMSelector(myname.c_str());

  declareProperty("WorkingPoint",m_WorkingPoint="","The Working Point");  
  declareProperty("ConfigFile",m_configFile="","The config file to use (if not setting cuts one by one)");

  // Name of the PID
  declareProperty("isEMMask",
		  m_rootTool->isEMMask=egammaPID::EgPidUndefined, //All pass by default, if not specified
		  "The mask to use");

  // boolean to force to test converted photon hypothesis
  declareProperty("ForceConvertedPhotonPID",
		  m_rootTool->forceConvertedPhotonPID=false,
		  "boolean to force to test converted photon hypothesis");

  // boolean to force to test non converted photon hypothesis
  declareProperty("ForceNonConvertedPhotonPID",
		  m_rootTool->forceNonConvertedPhotonPID=false,
		  "boolean to force to test non converted photon hypothesis");

  // new cuts on f3
  // Use f3core instead of f3
  declareProperty("useF3core", m_useF3core = false,
		  "Cut on f3 or f3core?");
  // Cut on fraction of energy to use 3rd sampling
  declareProperty("CutF3_photonsNonConverted",m_rootTool->CutF3_photonsNonConverted,
		  "Cut on fraction of energy to use 3rd sampling (f3 or f3core) for unconverted");
  // Cut on fraction of energy to use 3rd sampling
  declareProperty("CutF3_photonsConverted",m_rootTool->CutF3_photonsConverted,
		  "Cut on fraction of energy to use 3rd sampling (f3 or f3core) for converted");
  //
  // selection for non-converted photons
  //
  // Eta binning
  declareProperty("CutBinEta_photonsNonConverted",
		  m_rootTool->CutBinEta_photonsNonConverted,
		  "Eta binning for non converted photons");
  // ET binning
  declareProperty("CutBinEnergy_photonsNonConverted",
		  m_rootTool->CutBinEnergy_photonsNonConverted,
		  "E binning for photonsNonConverted");

  // Cut on hadronic leakage
  declareProperty("CutHadLeakage_photonsNonConverted",
		  m_rootTool->CutHadLeakage_photonsNonConverted,
		  "Cut on hadronic leakage for non converted photons");
  // Cut on E277
  declareProperty("e277_photonsNonConverted",
		  m_rootTool->e277_photonsNonConverted,
		  "Cut on E277 for non converted photons");
  // Cut on Reta37
  declareProperty("Reta37_photonsNonConverted",
		  m_rootTool->Reta37_photonsNonConverted,
		  "Cut on Reta37 for non converted photons");
  // Cut on Rphi33
  declareProperty("Rphi33_photonsNonConverted",
		  m_rootTool->Rphi33_photonsNonConverted,
		  "Cut on Rphi33 for non converted photons");
  // Cut on Weta2
  declareProperty("weta2_photonsNonConverted",
		  m_rootTool->weta2_photonsNonConverted,
		  "Cut on Weta2 for non converted photons");
  // eta binning in strips
  declareProperty("CutBinEtaStrips_photonsNonConverted",
		  m_rootTool->CutBinEtaStrips_photonsNonConverted,
		  "eta binning in strips for non converted photons");
  // Cut on energy in strips
  declareProperty("CutBinEnergyStrips_photonsNonConverted",
		  m_rootTool->CutBinEnergyStrips_photonsNonConverted,
		  "Cut on energy in strips for non converted photons");
  // Cut on fraction of energy to use 1st sampling
  declareProperty("f1_photonsNonConverted",
		  m_rootTool->f1_photonsNonConverted,
		  "Cut on fraction of energy to use 1st sampling for non converted photons");
  // Cut on Emax2 [NOT USED]
  // declareProperty("emax2r_photonsNonConverted",
  //                 m_rootTool->emax2r_photonsNonConverted,
  //		     "Cut on Emax2 for non converted photons");
  // Cut on Delta E
  declareProperty("deltae_photonsNonConverted",
		  m_rootTool->deltae_photonsNonConverted,
		  "Cut on Delta E for non converted photons");
  // Cut on (Emax1-Emax2)/(Emax1+Emax2) in 1st sampling
  declareProperty("DEmaxs1_photonsNonConverted",
		  m_rootTool->DEmaxs1_photonsNonConverted,
		  "Cut on (Emax1-Emax2)/(Emax1+Emax2) in 1st sampling for non converted photons");
  // Cut on total shower width in strips
  declareProperty("wtot_photonsNonConverted",
		  m_rootTool->wtot_photonsNonConverted,
		  "Cut on total shower width in strips for non converted photons");
  // Cut on fraction of energy outside core in strips
  declareProperty("fracm_photonsNonConverted",
		  m_rootTool->fracm_photonsNonConverted,
		  "Cut on fraction of energy outside core in strips for non converted photons");
  // Cut on shower width in 3 strips
  declareProperty("w1_photonsNonConverted",
		  m_rootTool->w1_photonsNonConverted,
		  "Cut on shower width in 3 strips for non converted photons");

  //================================================
  //
  // selection for converted photons
  //
  // Eta binning
  declareProperty("CutBinEta_photonsConverted",
		  m_rootTool->CutBinEta_photonsConverted,
		  "Eta binning for converted photons");
  // ET binning
  declareProperty("CutBinEnergy_photonsConverted",
		  m_rootTool->CutBinEnergy_photonsConverted,
		  "E binning for converted photons");

  // Cut on hadronic leakage
  declareProperty("CutHadLeakage_photonsConverted",
		  m_rootTool->CutHadLeakage_photonsConverted,
		  "Cut on hadronic leakage for converted photons");
  // Cut on E277
  declareProperty("e277_photonsConverted",
		  m_rootTool->e277_photonsConverted,
		  "Cut on E277 for converted photons");
  // Cut on Reta37
  declareProperty("Reta37_photonsConverted",
		  m_rootTool->Reta37_photonsConverted,
		  "Cut on Reta37 for converted photons");
  // Cut on Rphi33
  declareProperty("Rphi33_photonsConverted",
		  m_rootTool->Rphi33_photonsConverted,
		  "Cut on Rphi33 for converted photons");
  // Cut on Weta2
  declareProperty("weta2_photonsConverted",
		  m_rootTool->weta2_photonsConverted,
		  "Cut on Weta2 for converted photons");
  // eta binning in strips
  declareProperty("CutBinEtaStrips_photonsConverted",
		  m_rootTool->CutBinEtaStrips_photonsConverted,
		  "eta binning in strips for converted photons");
  // ET binning in strips
  declareProperty("CutBinEnergyStrips_photonsConverted",
		  m_rootTool->CutBinEnergyStrips_photonsConverted,
		  "Cut on energy in strips for converted photons");
  // Cut on fraction of energy to use 1st sampling
  declareProperty("f1_photonsConverted",
		  m_rootTool->f1_photonsConverted,
		  "Cut on fraction of energy to use 1st sampling for converted photons");
  // Cut on Emax2 [NOT USED]
  //declareProperty("emax2r_photonsConverted",
  //		  m_rootTool->emax2r_photonsConverted,
  //		  "Cut on Emax2 for converted photons");
  // Cut on Delta E
  declareProperty("deltae_photonsConverted",
		  m_rootTool->deltae_photonsConverted,
		  "Cut on Delta E for converted photons");
  // Cut on (Emax1-Emax2)/(Emax1+Emax2) in 1st sampling
  declareProperty("DEmaxs1_photonsConverted",
		  m_rootTool->DEmaxs1_photonsConverted,
		  "Cut on (Emax1-Emax2)/(Emax1+Emax2) in 1st sampling for converted photons");
  // Cut on total shower width in strips
  declareProperty("wtot_photonsConverted",
		  m_rootTool->wtot_photonsConverted,
		  "Cut on total shower width in strips for converted photons");
  // Cut on fraction of energy outside core in strips
  declareProperty("fracm_photonsConverted",
		  m_rootTool->fracm_photonsConverted,
		  "Cut on fraction of energy outside core in strips for converted photons");
  // Cut on shower width in 3 strips
  declareProperty("w1_photonsConverted",
		  m_rootTool->w1_photonsConverted,
		  "Cut on shower width in 3 strips for converted photons");
  // cut min on E/p
  declareProperty("CutminEp_photonsConverted",
		  m_rootTool->CutminEp_photonsConverted,
		  "Cut min on E/p for converted converted photons");
  // cut max on E/p
  declareProperty("CutmaxEp_photonsConverted",
		  m_rootTool->CutmaxEp_photonsConverted,
		  "Cut max on E/p for converted photons");
  
  // for the trigger needs:
  declareProperty("caloOnly", m_caloOnly=false, "Flag to tell the tool if its a calo only cutbase"); 
  declareProperty("trigEtTh", m_trigEtTh=-999. , "Trigger threshold"); 
  
}

// =================================================================
AsgPhotonIsEMSelector::~AsgPhotonIsEMSelector() 
{
  if(finalize().isFailure()){
    ATH_MSG_ERROR ( "Failure in AsgPhotonIsEMSelector finalize()");
  }
  delete m_rootTool;
}

// =================================================================
StatusCode AsgPhotonIsEMSelector::initialize() 
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;

  if(!m_WorkingPoint.empty()){
    m_configFile=AsgConfigHelper::findConfigFile(m_WorkingPoint,EgammaSelectors::PhotonCutPointToConfFile);
    m_rootTool->isEMMask=AsgConfigHelper::findMask(m_WorkingPoint,EgammaSelectors::PhotonCutPointToMask);
  }

  if(!m_configFile.empty()){
    //find the file and read it in
    std::string filename = PathResolverFindCalibFile( m_configFile);
    if(filename==""){ 
	ATH_MSG_ERROR("Could not locate " << m_configFile );
      	sc = StatusCode::FAILURE;
	return sc;
      } 
    ATH_MSG_DEBUG("Configfile to use  " << m_configFile );
    TEnv env(filename.c_str());    
    ///------- Read in the TEnv config ------///
    ATH_MSG_DEBUG("Read in the TEnv config ");
    //Override the mask via the config only if it is not set     
    if(m_rootTool->isEMMask==egammaPID::EgPidUndefined){ 
      unsigned int mask(env.GetValue("isEMMask",static_cast<int>(egammaPID::EgPidUndefined)));
      m_rootTool->isEMMask=mask;
    }
    
    ///------- Use helpers to read in the cut arrays ------///
    m_rootTool->CutBinEta_photonsNonConverted             =AsgConfigHelper::HelperFloat("CutBinEta_photonsNonConverted",env);
    m_rootTool->CutBinEnergy_photonsNonConverted          =AsgConfigHelper::HelperFloat("CutBinEnergy_photonsNonConverted",env);
    m_rootTool->e277_photonsNonConverted                  =AsgConfigHelper::HelperFloat("e277_photonsNonConverted",env);
    m_rootTool->CutHadLeakage_photonsNonConverted         =AsgConfigHelper::HelperFloat("CutHadLeakage_photonsNonConverted",env);
    m_rootTool->Reta37_photonsNonConverted                =AsgConfigHelper::HelperFloat("Reta37_photonsNonConverted",env);
    m_rootTool->Rphi33_photonsNonConverted                =AsgConfigHelper::HelperFloat("Rphi33_photonsNonConverted",env);
    m_rootTool->weta2_photonsNonConverted                 =AsgConfigHelper::HelperFloat("weta2_photonsNonConverted",env);
    m_rootTool->CutBinEtaStrips_photonsNonConverted       =AsgConfigHelper::HelperFloat("CutBinEtaStrips_photonsNonConverted",env);
    m_rootTool->CutBinEnergyStrips_photonsNonConverted    =AsgConfigHelper::HelperFloat("CutBinEnergyStrips_photonsNonConverted",env);
    m_rootTool->f1_photonsNonConverted                    =AsgConfigHelper::HelperFloat("f1_photonsNonConverted",env);
    m_rootTool->deltae_photonsNonConverted                =AsgConfigHelper::HelperFloat("deltae_photonsNonConverted",env);
    m_rootTool->DEmaxs1_photonsNonConverted               =AsgConfigHelper::HelperFloat("DEmaxs1_photonsNonConverted",env);
    m_rootTool->wtot_photonsNonConverted                  =AsgConfigHelper::HelperFloat("wtot_photonsNonConverted",env);
    m_rootTool->fracm_photonsNonConverted                 =AsgConfigHelper::HelperFloat("fracm_photonsNonConverted",env);
    m_rootTool->w1_photonsNonConverted                    =AsgConfigHelper::HelperFloat("w1_photonsNonConverted",env);
    m_rootTool->CutF3_photonsNonConverted                 =AsgConfigHelper::HelperFloat("CutF3_photonsNonConverted",env);
    m_rootTool->CutBinEta_photonsConverted                =AsgConfigHelper::HelperFloat("CutBinEta_photonsConverted",env);
    m_rootTool->CutBinEnergy_photonsConverted             =AsgConfigHelper::HelperFloat("CutBinEnergy_photonsConverted",env);
    m_rootTool->e277_photonsConverted                     =AsgConfigHelper::HelperFloat("e277_photonsConverted",env);
    m_rootTool->CutHadLeakage_photonsConverted            =AsgConfigHelper::HelperFloat("CutHadLeakage_photonsConverted",env);
    m_rootTool->Reta37_photonsConverted                   =AsgConfigHelper::HelperFloat("Reta37_photonsConverted",env);
    m_rootTool->Rphi33_photonsConverted                   =AsgConfigHelper::HelperFloat("Rphi33_photonsConverted",env);
    m_rootTool->weta2_photonsConverted                    =AsgConfigHelper::HelperFloat("weta2_photonsConverted",env);
    m_rootTool->CutBinEtaStrips_photonsConverted          =AsgConfigHelper::HelperFloat("CutBinEtaStrips_photonsConverted",env);
    m_rootTool->CutBinEnergyStrips_photonsConverted       =AsgConfigHelper::HelperFloat("CutBinEnergyStrips_photonsConverted",env);
    m_rootTool->f1_photonsConverted                       =AsgConfigHelper::HelperFloat("f1_photonsConverted",env);
    m_rootTool->deltae_photonsConverted                   =AsgConfigHelper::HelperFloat("deltae_photonsConverted",env);
    m_rootTool->DEmaxs1_photonsConverted                  =AsgConfigHelper::HelperFloat("DEmaxs1_photonsConverted",env);
    m_rootTool->wtot_photonsConverted                     =AsgConfigHelper::HelperFloat("wtot_photonsConverted",env);
    m_rootTool->fracm_photonsConverted                    =AsgConfigHelper::HelperFloat("fracm_photonsConverted",env);
    m_rootTool->w1_photonsConverted                       =AsgConfigHelper::HelperFloat("w1_photonsConverted",env);
    m_rootTool->CutminEp_photonsConverted                 =AsgConfigHelper::HelperFloat("CutminEp_photonsConverted",env);
    m_rootTool->CutmaxEp_photonsConverted                 =AsgConfigHelper::HelperFloat("CutmaxEp_photonsConverted",env);
    m_rootTool->CutF3_photonsConverted                    =AsgConfigHelper::HelperFloat("CutF3_photonsConverted",env);
    
  } else {
    ATH_MSG_INFO("Conf file empty. Just user Input");
  }
  
  ATH_MSG_INFO("operating point : " << this->getOperatingPointName());
  // Get the message level and set the underlying ROOT tool message level accordingly
  m_rootTool->msg().setLevel(this->msg().level());

  // We need to initialize the underlying ROOT TSelectorTool
  if ( 0 == m_rootTool->initialize() )
    {
      ATH_MSG_ERROR("Could not initialize the TPhotonIsEMSelector!");
      sc = StatusCode::FAILURE;
      return sc;
    }

  return sc ;
}

// ===============================================================
StatusCode AsgPhotonIsEMSelector::finalize()
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
const Root::TAccept& AsgPhotonIsEMSelector::accept( const xAOD::IParticle* part ) const{

  ATH_MSG_DEBUG("Entering accept( const IParticle* part )");
  if(part->type()==xAOD::Type::Photon || part->type()==xAOD::Type::Electron){
    return accept(static_cast<const xAOD::Egamma*> (part));
  }
  else{
    ATH_MSG_ERROR("AsgElectronIsEMSelector::could not convert argument to Photon/Electron");
    return m_acceptDummy;
  }

}

const Root::TAccept& AsgPhotonIsEMSelector::accept( const xAOD::Egamma* eg ) const{
 
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

const Root::TAccept& AsgPhotonIsEMSelector::accept( const xAOD::Photon* ph) const{
  ATH_MSG_DEBUG("Entering accept( const Photon* part )");  
  return accept(static_cast<const xAOD::Egamma*> (ph));  
}

const Root::TAccept& AsgPhotonIsEMSelector::accept( const xAOD::Electron* el) const{
  ATH_MSG_DEBUG("Entering accept( const Electron* part )");  
  return accept(static_cast<const xAOD::Egamma*> (el));  
}

  /** The value of the isem **/
unsigned int AsgPhotonIsEMSelector::IsemValue() const {
    return m_rootTool->isEM(); 
}

//=============================================================================
/// Get the name of the current operating point
//=============================================================================
std::string AsgPhotonIsEMSelector::getOperatingPointName() const{
 
  if (m_rootTool->isEMMask == egammaPID::PhotonLoose){ return "Loose"; }
  else if (m_rootTool->isEMMask == egammaPID::PhotonMedium ){ return "Medium"; }
  else if (m_rootTool->isEMMask == egammaPID::PhotonTight){ return "Tight"; }
  else if (m_rootTool->isEMMask == egammaPID::PhotonLooseEF ){ return "LooseEF"; }
  else if (m_rootTool->isEMMask == egammaPID::PhotonMediumEF){ return "MediumEF"; }
  else if (m_rootTool->isEMMask == 0){ return "0 No cuts applied"; }
  else{
    ATH_MSG_ERROR( "Didn't recognize the given operating point with mask: " << m_rootTool->isEMMask );
    return "";
  }
}

// A simple execute command wrapper
// ==============================================================
StatusCode AsgPhotonIsEMSelector::execute(const xAOD::Egamma* eg) const
{
  //
  // Particle identification for photons based on cuts
  // trigEtTh : threshold in ET as applied by trigger
  //

  ATH_MSG_DEBUG("Executing AsgPhotonIsEMSelector");

  // initialisation
  unsigned int iflag = 0;

  // protection against null pointer
  if (eg==0) {
    ATH_MSG_ERROR("eg == 0");
    // if object is bad then use the bit for "bad eta"
    iflag = (0x1 << egammaPID::ClusterEtaRange_Photon); 
    m_rootTool->setIsEM(iflag);
    return StatusCode::SUCCESS; 
  }

  // protection against bad clusters
  const xAOD::CaloCluster* cluster  = eg->caloCluster(); 
  if ( cluster == 0 ) {
    ATH_MSG_ERROR("exiting because cluster is NULL " << cluster);
    // if object is bad then use the bit for "bad eta"
    iflag = (0x1 << egammaPID::ClusterEtaRange_Photon); 
    m_rootTool->setIsEM(iflag);
    return StatusCode::SUCCESS;  
  }

  // Fill variables
  // eta position in second sampling
  const float eta2   = fabsf(cluster->etaBE(2));
  // transverse energy in calorimeter (using eta position in second sampling)
  const double energy =  cluster->e();
  double et = 0.;
  if (eta2<999.) {
    const double cosheta = cosh(eta2);
    et = (cosheta != 0.) ? energy/cosheta : 0.;
  }

  float Rhad1(0), Rhad(0), Reta(0), Rphi(0), e277(0), weta2c(0), //emax2(0), 
    Eratio(0), DeltaE(0), f1(0), weta1c(0), wtot(0), fracm(0), f3(0);

  bool allFound = true;
    
  // variables based on HCAL
  // transverse energy in 1st scintillator of hadronic calorimeter/ET
  allFound = allFound && eg->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1);
  // transverse energy in hadronic calorimeter/ET
  allFound = allFound && eg->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad);

  // variables based on S2 of EM CAL
  // E(7*7) in 2nd sampling
  allFound = allFound && eg->showerShapeValue(e277, xAOD::EgammaParameters::e277);
  // E(3*7)/E(7*7) in 2nd sampling
  allFound = allFound && eg->showerShapeValue(Reta, xAOD::EgammaParameters::Reta);
  // E(3*3)/E(3*7) in 2nd sampling
  allFound = allFound && eg->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi);
  // shower width in 2nd sampling
  allFound = allFound && eg->showerShapeValue(weta2c, xAOD::EgammaParameters::weta2);

  // variables based on S1 of EM CAL
  // fraction of energy reconstructed in the 1st sampling
  allFound = allFound && eg->showerShapeValue(f1, xAOD::EgammaParameters::f1);
  // shower width in 3 strips in 1st sampling
  allFound = allFound && eg->showerShapeValue(weta1c, xAOD::EgammaParameters::weta1);
  // E of 2nd max between max and min in strips [NOT USED]
  // allFound = allFound && eg->showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1);
  // (E of 1st max in strips-E of 2nd max)/(E of 1st max+E of 2nd max)
  allFound = allFound && eg->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio);
  // E(2nd max)-E(min) in strips
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

  // cut on E/p
  double ep = 1.0; // default passes
  
  if (m_caloOnly){
    ATH_MSG_DEBUG("Doing CaloCutsOnly");
  } else {
    if (xAOD::EgammaHelpers::isConvertedPhoton(eg)) {
      const xAOD::Photon *ph = dynamic_cast<const xAOD::Photon*>(eg);
      if(!ph){
	ATH_MSG_WARNING("Can not cast egamma to photon for the e/p cut");
      }else{
	float p = xAOD::EgammaHelpers::momentumAtVertex(ph).mag();
	if (p!=0.){
	  ep = energy / p;
	}
	else{
	  ep = 9999999.;
	}
      }
    }
  }

  // modifiy et when dealing with trigger
  // to be sure that it will take the correct bin (VD)
  if(m_trigEtTh > 0) et = m_trigEtTh*1.01; 

  // apply calorimeter selection for photons
  iflag = m_rootTool->calcIsEm(eta2,
			       et,
			       Rhad1,
			       Rhad,
			       e277,
			       Reta,
			       Rphi,
			       weta2c,
			       //emax2,
			       f1,
			       Eratio,
			       DeltaE,
			       weta1c,
			       wtot,
			       fracm,
			       f3,
			       ep,
			       xAOD::EgammaHelpers::isConvertedPhoton(eg));
  m_rootTool->setIsEM(iflag);  
  return StatusCode::SUCCESS;
}

const Root::TAccept& AsgPhotonIsEMSelector::getTAccept( ) const {
  return m_rootTool->getTAccept();
}
