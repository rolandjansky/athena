/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/**
   @class AsgPhotonIsEMSelector
   @brief Photon isEM selector

   @author Jovan Mitrevski (UCSC) Karsten Koeneke (CERN)
   @date   Dec 2011 - Fab 2012

   11-MAR-2014 convert to ASG tool

*/

// Include this class's header
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "xAODEgamma/EgammaxAODHelpers.h"

double AsgPhotonIsEMSelector::getConversion1OverP(const xAOD::Vertex* vx) const
{
  auto tps = vx->trackParticleLinks();
  if (tps.size() == 1) {
    return fabs((*tps[0])->qOverP());
  } else if (tps.size() == 2) {
    auto p = (*tps[0])->p4() + (*tps[1])->p4();
    return 1.0/p.P();
  } else {
    ATH_MSG_WARNING("A conversion vertex has an incorrect size");
    return 1.0;
  }
}

//=============================================================================
// Standard constructor
//=============================================================================
AsgPhotonIsEMSelector::AsgPhotonIsEMSelector(std::string myname) :
  AsgTool(myname),
  m_rootTool(0)
{

  m_rootTool = new Root::TPhotonIsEMSelector();
  
  // Name of the PID
  declareProperty("isEMMask",
		  m_rootTool->isEMMask=egammaPID::PhotonTightAR,
		  "The mask to use");

  // Name of the PID
  declareProperty("PIDName",
		  m_rootTool->PIDName=egammaPID::IsEM,
		  "Name of the PID word to use");


  // boolean to force to test converted photon hypothesis
  declareProperty("ForceConvertedPhotonPID",
		  m_rootTool->forceConvertedPhotonPID=false,
		  "boolean to force to test converted photon hypothesis");

  // boolean to force to test non converted photon hypothesis
  declareProperty("ForceNonConvertedPhotonPID",
		  m_rootTool->forceNonConvertedPhotonPID=false,
		  "boolean to force to test non converted photon hypothesis");

  //
  // selection for Loose photons
  //
  // Eta binning
  declareProperty("CutBinEta_photonsLoose",m_rootTool->CutBinEta_photonsLoose,
		  "Eta binning for photonsLoose");
  // ET binning
  declareProperty("CutBinEnergy_photonsLoose",m_rootTool->CutBinEnergy_photonsLoose,
		  "E binning for photonsLoose");

  // Cut on hadronic leakage
  declareProperty("CutHadLeakage_photonsLoose",m_rootTool->CutHadLeakage_photonsLoose,
		  "Cut on hadronic leakage for photonsLoose");
  // Cut on E277
  declareProperty("e277_photonsLoose",m_rootTool->e277_photonsLoose,
		  "Cut on E277 for photonsLoose");
  // Cut on Reta37
  declareProperty("Reta37_photonsLoose",m_rootTool->Reta37_photonsLoose,
		  "Cut on Reta37 for photonsLoose");
  // Cut on Rphi33
  declareProperty("Rphi33_photonsLoose",m_rootTool->Rphi33_photonsLoose,
		  "Cut on Rphi33 for photonsLoose");
  // Cut on Weta2
  declareProperty("weta2_photonsLoose",m_rootTool->weta2_photonsLoose,
		  "Cut on Weta2 for photonsLoose");

  // new cuts on f3
  // Use f3core instead of f3
  declareProperty("useF3core", m_useF3core = false,
		  "Cut on f3 or f3core?");
  // Cut on fraction of energy to use 3rd sampling
  declareProperty("CutF3_photonsNonConverted",m_rootTool->CutF3_photonsNonConverted,
		  "Cut on fraction of energy to use 3rd sampling (f3 or f3core) for tight unconv");
  // Cut on fraction of energy to use 3rd sampling
  declareProperty("CutF3_photonsConverted",m_rootTool->CutF3_photonsConverted,
		  "Cut on fraction of energy to use 3rd sampling (f3 or f3core) for tight converted");


  //
  // selection for Tight photons (non-converted)
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
  // Cut on Emax2
  declareProperty("emax2r_photonsNonConverted",
		  m_rootTool->emax2r_photonsNonConverted,
		  "Cut on Emax2 for non converted photons");
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
  // selection for Tight photons (converted)
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
  // Cut on Emax2
  declareProperty("emax2r_photonsConverted",
		  m_rootTool->emax2r_photonsConverted,
		  "Cut on Emax2 for converted photons");
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
const Root::TAccept& AsgPhotonIsEMSelector::accept( const xAOD::IParticle* part ) const
{
  ATH_MSG_DEBUG("Entering accept( const xAOD::IParticle*");
  const xAOD::Photon* eg = dynamic_cast<const xAOD::Photon*>(part);
  if ( eg )
    {
      return accept(eg);
    }
  else
    {
      ATH_MSG_ERROR("AsgPhotonIsEMSelector::could not cast argument to accept");
      return m_acceptDummy;
    }
}

const Root::TAccept& AsgPhotonIsEMSelector::accept( const xAOD::Photon* eg ) const
{
  ATH_MSG_DEBUG("Entering accept( const xAOD::Photon* part )");  
  if ( eg )
    {
      StatusCode sc = execute(eg);
      if (sc.isFailure()) {
	ATH_MSG_ERROR("could not calculate isEM");
	return m_acceptDummy;
      }
      return m_rootTool->fillAccept();
    }
  else
    {
      ATH_MSG_ERROR("AsgPhotonIsEMSelector::accept was given a bad argument");
      return m_acceptDummy;
    }
}


// The stuff copied over from egammaPhotonCutIDTool

// A simple execute command wrapper
// ==============================================================
StatusCode AsgPhotonIsEMSelector::execute(const xAOD::Photon* eg) const
{
  // call main method
  return execute(eg,-999.);
}

// this one is potentially for the trigger
// ==============================================================
StatusCode AsgPhotonIsEMSelector::execute(const xAOD::Photon* eg, 
					  double trigEtTh) const
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
    ATH_MSG_DEBUG("eg == 0");
    // if object is bad then use the bit for "bad eta"
    iflag = (0x1 << egammaPID::ClusterEtaRange_Photon); 
    m_rootTool->setIsEM(iflag);
    return StatusCode::SUCCESS; 
  }

  // protection against bad clusters
  const xAOD::CaloCluster* cluster  = eg->caloCluster(); 
  if ( cluster == 0 ) {
    ATH_MSG_DEBUG("cluster == " << cluster);
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

  float ethad1, ethad, e233, e237, e277, weta2c, emax2, emax, emin, f1, weta1c, wtot, fracm, f3;

  bool allFound = true;
    
  // transverse energy in 1st scintillator of hadronic calorimeter
  allFound = allFound && eg->showerShapeValue(ethad1, xAOD::EgammaParameters::ethad1);
  // transverse energy in hadronic calorimeter
  allFound = allFound && eg->showerShapeValue(ethad, xAOD::EgammaParameters::ethad);
  // E(3*3) in 2nd sampling
  allFound = allFound && eg->showerShapeValue(e233, xAOD::EgammaParameters::e233);
  // E(3*7) in 2nd sampling
  allFound = allFound && eg->showerShapeValue(e237, xAOD::EgammaParameters::e237);
  // E(7*7) in 2nd sampling
  allFound = allFound && eg->showerShapeValue(e277, xAOD::EgammaParameters::e277);
  // shower width in 3 strips in 1st sampling
  allFound = allFound && eg->showerShapeValue(weta1c, xAOD::EgammaParameters::weta1);
  // shower width in 2nd sampling
  allFound = allFound && eg->showerShapeValue(weta2c, xAOD::EgammaParameters::weta2);
  // fraction of energy reconstructed in the 1st sampling
  allFound = allFound && eg->showerShapeValue(f1, xAOD::EgammaParameters::f1);
  // E of 2nd max between max and min in strips
  allFound = allFound && eg->showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1);
  // E of 1st max in strips
  allFound = allFound && eg->showerShapeValue(emax, xAOD::EgammaParameters::emaxs1);
  // E(min) in strips
  allFound = allFound && eg->showerShapeValue(emin, xAOD::EgammaParameters::emins1);
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
  double ep = 1.0; 		// default passes

  // retrieve associated track
  const xAOD::Vertex* vx = eg->vertex();
  if (vx != 0) {
    ep = energy * getConversion1OverP(vx);
  }

  const EMAmbiguityType::AmbiguityResult ambiguityResult = 
    EMAmbiguityType::PHOTON_AMBIGUITY;

  // modifiy et when dealing with trigger
  // to be sure that it will take the correct bin (VD)
  if(trigEtTh > 0) et = trigEtTh*1.01; 

  // apply calorimeter selection for Loose photons
  iflag = m_rootTool->calcIsEm(eta2,
			       et,
			       ethad1,
			       ethad,
			       e233,
			       e237,
			       e277,
			       weta2c,
			       emax2,
			       emax,
			       emin,
			       f1,
			       weta1c,
			       wtot,
			       fracm,
			       f3,
			       ep,
			       xAOD::EgammaHelpers::isConvertedPhoton(eg),
			       ambiguityResult);

  m_rootTool->setIsEM(iflag);  
  return StatusCode::SUCCESS;
}
