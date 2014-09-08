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

#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <cstdint>

//=============================================================================
// Standard constructor
//=============================================================================
AsgElectronIsEMSelector::AsgElectronIsEMSelector(std::string myname) :
  AsgTool(myname),
  m_rootTool(0)
{

  m_rootTool = new Root::TElectronIsEMSelector();
  
  // Name of the PID
  declareProperty("isEMMask",
		  m_rootTool->isEMMask=egammaPID::ElectronMediumPP,
		  "The mask to use");

  // Name of the PID
  declareProperty("PIDName",
		  m_rootTool->PIDName=egammaPID::IsEM,
		  "Name of the PID word to use");
  
  // Boolean to use b-layer prediction
  declareProperty("useBLayerHitPrediction", 
		  m_rootTool->useBLayerHitPrediction = true,
		  "Boolean to use b-layer prediction");

  // Boolean to use TRT outliers
  declareProperty("useTRTOutliers",m_rootTool->useTRTOutliers,
		  "Boolean to use TRT outliers");

  // Boolean to use BL outliers
  declareProperty("useBLOutliers",
		  m_rootTool->useBLOutliers = true,
		  "Boolean to use b-layer outliers");

  // Boolean to use PIX outliers
  declareProperty("usePIXOutliers",
		  m_rootTool->usePIXOutliers = true,
		  "Boolean to use PIX outliers");

  // Boolean to use SCT outliers
  declareProperty("useSCTOutliers",
		  m_rootTool->useSCTOutliers = true,
		  "Boolean to use SCT outliers");

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
  declareProperty("CutRphi37",m_rootTool->CutRphi37,
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
  declareProperty("CutF3",m_rootTool->CutF3,
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
  // cut on Number of TRT hits
  declareProperty("CutNumTRT",m_rootTool->CutNumTRT,
		  "cut on Number of TRT hits");
  // cut on Ratio of TR hits to Number of TRT hits
  declareProperty("CutTRTRatio",m_rootTool->CutTRTRatio,
		  "Cut on Ratio of TR hits to Number of TRT hits");
  // cut on Ratio of TR hits to Number of TRT hits for 90% efficiency
  declareProperty("CutTRTRatio90",m_rootTool->CutTRTRatio90,
		  "cut on Ratio of TR hits to Number of TRT hits for 90% efficiency");

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


//=============================================================================
// Asgena initialize method
//=============================================================================
StatusCode AsgElectronIsEMSelector::initialize()
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;

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
const Root::TAccept& AsgElectronIsEMSelector::accept( const xAOD::IParticle* part ) const
{
  ATH_MSG_DEBUG("Entering accept( const IParticle* part )");
  const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
  if ( eg )
    {
      return accept(eg);
    }
  else
    {
      ATH_MSG_ERROR("AsgElectronIsEMSelector::could not cast argument to accept");
      return m_acceptDummy;
    }
}

const Root::TAccept& AsgElectronIsEMSelector::accept( const xAOD::Electron* eg ) const
{
  ATH_MSG_DEBUG("Entering accept( const egamma* part )");  
  if ( eg )
    {
      StatusCode sc = execute(eg, -999., false);
      if (sc.isFailure()) {
	ATH_MSG_ERROR("could not calculate isEM");
	return m_acceptDummy;
      }
      return m_rootTool->fillAccept();
    }
  else
    {
      ATH_MSG_ERROR("AsgElectronIsEMSelector::accept was given a bad argument");
      return m_acceptDummy;
    }
}

const Root::TAccept& AsgElectronIsEMSelector::accept( const xAOD::Electron* eg, 
						      double trigEtTh, 
						      bool CaloCutsOnly) const
{
  ATH_MSG_DEBUG("Entering accept( const egamma* part )");  
  if ( eg )
    {
      StatusCode sc = execute(eg, trigEtTh, CaloCutsOnly);
      if (sc.isFailure()) {
	ATH_MSG_ERROR("could not calculate isEM");
	return m_acceptDummy;
      }
      return m_rootTool->fillAccept();
    }
  else
    {
      ATH_MSG_ERROR("AsgElectronIsEMSelector::accept was given a bad argument");
      return m_acceptDummy;
    }
}

const Root::TAccept& AsgElectronIsEMSelector::accept( const xAOD::Egamma* eg, 
						      double trigEtTh) const
{
  ATH_MSG_DEBUG("Entering accept( const egamma* part )");  
  if ( eg )
    {
      StatusCode sc = execute(eg, trigEtTh);
      if (sc.isFailure()) {
	ATH_MSG_ERROR("could not calculate isEM");
	return m_acceptDummy;
      }
      return m_rootTool->fillAccept();
    }
  else
    {
      ATH_MSG_ERROR("AsgElectronIsEMSelector::accept was given a bad argument");
      return m_acceptDummy;
    }
}

// The stuff copied over from egammaElectronCutIDTool

// A simple execute command wrapper
// ==============================================================
StatusCode AsgElectronIsEMSelector::execute(const xAOD::Electron* eg) const
{
  // call execute method 
  return execute(eg, -999., false);
}

// The real execute command
// ==============================================================
StatusCode AsgElectronIsEMSelector::execute(const xAOD::Electron* eg, 
					    double trigEtTh, 
					    bool CaloCutsOnly) const
{
  //
  // Particle identification for electrons based on cuts
  //

  ATH_MSG_DEBUG("entering execute(const egamma* eg...)");

  // initialisation
  unsigned int iflag = 0; 

  // protection against null pointer
  if (eg==0) {
    // if object is bad then use the bit for "bad eta"
    ATH_MSG_DEBUG("exiting because eg is empty");
    iflag = (0x1 << egammaPID::ClusterEtaRange_Electron); 
    m_rootTool->setIsEM(iflag);
    return StatusCode::SUCCESS; 
  }

  // retrieve associated cluster
  const xAOD::CaloCluster* cluster  = eg->caloCluster(); 

  // basic protection : isem is calculated only for objects
  // which have a cluster, a track, an EMTrackMatch object and an EMShower
  // which is likely for an electron
  if ( cluster == 0 ) {
    // if object is bad then use the bit for "bad eta"
    iflag = (0x1 << egammaPID::ClusterEtaRange_Electron); 
    m_rootTool->setIsEM(iflag);
    return StatusCode::SUCCESS; 
  }
  
  // eta position in second sampling
  const float eta2   = fabsf(cluster->etaBE(2));
  // transverse energy in calorimeter (using eta position in second sampling)
  const double energy =  cluster->e();
  double et = 0.;
  if (eta2<999.) {
    const double cosheta = cosh(eta2);
    et = (cosheta != 0.) ? energy /cosheta : 0.;
  }
  
  if (CaloCutsOnly) {

    //ATH_MSG_DEBUG("Doing CaloCutsOnly");
    //
    // apply only calo information
    //
    
    // apply calorimeter selection
    iflag = calocuts_electrons(eg, eta2, et, trigEtTh, 0);

    // should I apply ambiguity cuts here?
    // m_iflag = ambiguitycuts_electrons(eg, emConversion);

  } else {

    
    // apply calorimeter selection
    iflag = calocuts_electrons(eg, eta2, et, trigEtTh, 0);
    //ATH_MSG_DEBUG(std::hex << "after calo cuts, iflag = " << iflag); 

    // apply track selection 
    iflag = TrackCut(eg, eta2, et, energy, iflag);
    //ATH_MSG_DEBUG(std::hex << "after track cuts, iflag = " << iflag); 

    // // apply ambiguity cuts
    // iflag = ambiguitycuts_electrons(eg, emConversion, iflag);
    // //ATH_MSG_DEBUG(std::hex << "final isem = " << iflag); 

  }// end of switch(CaloCutsOnly)
    
  m_rootTool->setIsEM(iflag);  
  return StatusCode::SUCCESS;
}

// The only does calocutsonly
// ==============================================================
StatusCode AsgElectronIsEMSelector::execute(const xAOD::Egamma* eg, 
					    double trigEtTh) const
{
  //
  // Particle identification for electrons based on cuts
  //

  ATH_MSG_DEBUG("entering execute(const egamma* eg...)");

  // initialisation
  unsigned int iflag = 0; 

  // protection against null pointer
  if (eg==0) {
    // if object is bad then use the bit for "bad eta"
    ATH_MSG_DEBUG("exiting because eg is empty");
    iflag = (0x1 << egammaPID::ClusterEtaRange_Electron); 
    m_rootTool->setIsEM(iflag);
    return StatusCode::SUCCESS; 
  }

  // retrieve associated cluster
  const xAOD::CaloCluster* cluster  = eg->caloCluster(); 

  // basic protection : isem is calculated only for objects
  // which have a cluster, a track, an EMTrackMatch object and an EMShower
  // which is likely for an electron
  if ( cluster == 0 ) {
    // if object is bad then use the bit for "bad eta"
    iflag = (0x1 << egammaPID::ClusterEtaRange_Electron); 
    m_rootTool->setIsEM(iflag);
    return StatusCode::SUCCESS; 
  }
  
  // eta position in second sampling
  const float eta2   = fabsf(cluster->etaBE(2));
  // transverse energy in calorimeter (using eta position in second sampling)
  const double energy =  cluster->e();
  double et = 0.;
  if (eta2<999.) {
    const double cosheta = cosh(eta2);
    et = (cosheta != 0.) ? energy /cosheta : 0.;
  }
  
  // apply calorimeter selection
  iflag = calocuts_electrons(eg, eta2, et, trigEtTh, 0);
  
  // should I apply ambiguity cuts here?
  // m_iflag = ambiguitycuts_electrons(eg, emConversion);

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
  // EMShower : EMShower detail
  // trigETthr : threshold in ET to apply the cuts at trigger level
  // iflag: the starting isEM
  //


  float e233, e237, e277, ethad1, ethad, weta1c, weta2c, f1, emax2, emax, emin, wtot, fracm, f3;

  bool allFound = true;

  // E(3*3) in 2nd sampling
  allFound = allFound && eg->showerShapeValue(e233, xAOD::EgammaParameters::e233);
  // E(3*7) in 2nd sampling
  allFound = allFound && eg->showerShapeValue(e237, xAOD::EgammaParameters::e237);
  // E(7*7) in 2nd sampling
  allFound = allFound && eg->showerShapeValue(e277, xAOD::EgammaParameters::e277);
  // transverse energy in 1st scintillator of hadronic calorimeter
  allFound = allFound && eg->showerShapeValue(ethad1, xAOD::EgammaParameters::ethad1);
  // transverse energy in hadronic calorimeter
  allFound = allFound && eg->showerShapeValue(ethad, xAOD::EgammaParameters::ethad);
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

  if (!allFound) {
     // if object is bad then use the bit for "bad eta"
    ATH_MSG_WARNING("Have some variables missing.");
    iflag = (0x1 << egammaPID::ClusterEtaRange_Electron); 
    return iflag;
  }
   
  // change et value when dealing with trigger
  // to be sure that it will take the correct bin (VD)
  if(trigEtTh > 0) et = trigEtTh*1.01; 

  return m_rootTool->calocuts_electrons(eta2,
					et,
                                        e233,
					e237,
					e277,
					ethad1,
					ethad,
					weta1c,
					weta2c,
					f1,
					emax2,
					emax,
					emin,
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
    ATH_MSG_DEBUG("Something is bad with the variables as passed");
    // if object is bad then use the bit for "bad eta"
    iflag = (0x1 << egammaPID::ClusterEtaRange_Electron); 
    return StatusCode::SUCCESS; 
  }
 
  // Track quality cut
  // number of B-layer hits
  uint8_t nBL = 0;
  uint8_t nBLOutliers = 0;
  // number of Pixel hits
  uint8_t nPi = 0;
  uint8_t nPiOutliers = 0;
  // number of SCT hits
  uint8_t nSCT = 0;
  uint8_t nSCTOutliers = 0;
  uint8_t nTRThigh          = 0;
  uint8_t nTRThighOutliers  = 0;
  uint8_t nTRT         = 0;
  uint8_t nTRTOutliers = 0;
  uint8_t nTRTXenonHits = 0;
  uint8_t expectHitInBLayer = true;

  bool allFound = true;

  allFound = allFound && t->summaryValue(nBL, xAOD::numberOfBLayerHits);
  allFound = allFound && t->summaryValue(nPi, xAOD::numberOfPixelHits);
  allFound = allFound && t->summaryValue(nSCT, xAOD::numberOfSCTHits);
  allFound = allFound && t->summaryValue(nBLOutliers, xAOD::numberOfBLayerOutliers);
  allFound = allFound && t->summaryValue(nPiOutliers, xAOD::numberOfPixelOutliers);
  allFound = allFound && t->summaryValue(nSCTOutliers, xAOD::numberOfSCTOutliers);

  allFound = allFound && t->summaryValue(nTRThigh, xAOD::numberOfTRTHighThresholdHits);
  allFound = allFound && t->summaryValue(nTRThighOutliers, xAOD::numberOfTRTHighThresholdOutliers);
  allFound = allFound && t->summaryValue(nTRT, xAOD::numberOfTRTHits);
  allFound = allFound && t->summaryValue(nTRTOutliers, xAOD::numberOfTRTOutliers);
  allFound = allFound && t->summaryValue(nTRTXenonHits, xAOD::numberOfTRTXenonHits);

  allFound = allFound && t->summaryValue(expectHitInBLayer, xAOD::expectBLayerHit);


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
			      nBL,
			      nBLOutliers,
			      nPi,
			      nPiOutliers,
			      nSCT,
			      nSCTOutliers,
			      nTRThigh,
			      nTRThighOutliers,
			      nTRT,
			      nTRTOutliers,
			      nTRTXenonHits,
			      trackd0,
			      deltaeta,
			      deltaphi,
			      ep,
			      expectHitInBLayer,
			      iflag);
}

// // ==============================================================
// unsigned int AsgElectronIsEMSelector::ambiguitycuts_electrons(const xAOD::Electron* /* eg */, 
// 							      unsigned int iflag) const 
// {
//   if (!emConversion) {
//     iflag &= ~(0x1 << egammaPID::ConversionMatch_Electron); 
//     // set bit to 0 (passed)
//     return iflag;
//   } else {
//     return m_rootTool->ambiguitycuts_electrons(static_cast<EMAmbiguityType::AmbiguityResult>(emConversion->ambiguityResult()), iflag);
//   }
// }

