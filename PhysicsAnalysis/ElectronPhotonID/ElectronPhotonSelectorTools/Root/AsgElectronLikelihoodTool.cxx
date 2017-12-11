/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AsgElectronLikelihoodTool
   @brief Electron selector tool to select objects in Asgena using an underlying pure ROOT tool.

   @author Karsten Koeneke
   @date   October 2012

   09-APR-2014, convert to ASGTool (Jovan Mitrevski)

*/

// Include this class's header
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/ElectronSelectorHelpers.h"
#include "AsgElectronPhotonIsEMSelectorConfigHelper.h"
#include "TElectronLikelihoodTool.h"
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
AsgElectronLikelihoodTool::AsgElectronLikelihoodTool(std::string myname) :
  AsgTool(myname),
  m_configFile(""),
  m_rootTool(0)
{

  // Create an instance of the underlying ROOT tool
  m_rootTool = new Root::TElectronLikelihoodTool( ("T"+myname).c_str() );

  // Declare the needed properties
  declareProperty("WorkingPoint",m_WorkingPoint="","The Working Point");
  declareProperty("ConfigFile",m_configFile="","The config file to use");
  declareProperty("usePVContainer", m_usePVCont=true, "Whether to use the PV container");
  declareProperty("nPVdefault", m_nPVdefault = 0, "The default number of PVs if not counted");
  declareProperty("primaryVertexContainer", m_primVtxContName="PrimaryVertices", "The primary vertex container name" );
  declareProperty("useCaloSumsContainer", m_useCaloSumsCont=true, "Whether to use the CaloSums container");
  declareProperty("fcalEtDefault", m_fcalEtDefault = 0, "The default FCal sum ET");
  declareProperty("CaloSumsContainer", m_CaloSumsContName="CaloSums", "The CaloSums container name" );



  //
  // Configurables in the root tool
  //
  // pdf file name. Managed in the Asg tool.
  declareProperty("inputPDFFileName",  m_pdfFileName="", "The input ROOT file name that holds the PDFs" );
  // the variable names, if non-standard - nope, it's done above!
  declareProperty("VariableNames",m_rootTool->VariableNames,"Variable names input to the LH");
  // The likelihood cut values
  declareProperty("CutLikelihood",m_rootTool->CutLikelihood,"Cut on likelihood discriminant");
  // The pileup-correction part of the likelihood cut values
  declareProperty("CutLikelihoodPileupCorrection",m_rootTool->CutLikelihoodPileupCorrection,"Pileup correction for LH discriminant");
  // The likelihood cut values - 4 GeV
  declareProperty("CutLikelihood4GeV",m_rootTool->CutLikelihood4GeV,"Cut on likelihood discriminant, 4 GeV special bin");
  // The pileup-correction part of the likelihood cut values - 4 GeV
  declareProperty("CutLikelihoodPileupCorrection4GeV",m_rootTool->CutLikelihoodPileupCorrection4GeV,"Pileup correction for LH discriminant, 4 GeV special bin");
  // do the conversion cut
  declareProperty("doCutConversion",m_rootTool->doCutConversion,"Apply the conversion bit cut");
  // do the ambiguity cut
  declareProperty("CutAmbiguity" ,m_rootTool->CutAmbiguity ,"Apply a cut on the ambiguity bit");
  // cut on b-layer
  declareProperty("CutBL",m_rootTool->CutBL,"Cut on b-layer");
  // cut on pixel hits
  declareProperty("CutPi",m_rootTool->CutPi,"Cut on pixel hits");
  // cut on d0
  declareProperty("CutA0",m_rootTool->CutA0,"Cut on d0");
  // cut on deltaEta
  declareProperty("CutDeltaEta",m_rootTool->CutDeltaEta,"Cut on deltaEta");
  // cut on deltaPhiRes
  declareProperty("CutDeltaPhiRes",m_rootTool->CutDeltaPhiRes,"Cut on deltaPhiRes");
  // cut on precision hits
  declareProperty("CutSi",m_rootTool->CutSi,"Cut on precision hits");
  // turn off f3 at high Et
  declareProperty("doRemoveF3AtHighEt",m_rootTool->doRemoveF3AtHighEt,"Turn off f3 at high Et");
  // turn off TRTPID at high Et
  declareProperty("doRemoveTRTPIDAtHighEt",m_rootTool->doRemoveTRTPIDAtHighEt,"Turn off TRTPID at high Et");
  // use smooth interpolation between LH bins
  declareProperty("doSmoothBinInterpolation",m_rootTool->doSmoothBinInterpolation,"use smooth interpolation between LH bins");
  // use binning for high ET LH
  declareProperty("useHighETLHBinning",m_rootTool->useHighETLHBinning,"Use binning for high ET LH");
  // use one extra bin for high ET LH
  declareProperty("useOneExtraHighETLHBin",m_rootTool->useOneExtraHighETLHBin,"Use one extra bin for high ET LH");
  // cut on Wstot above HighETBinThreshold
  declareProperty("CutWstotAtHighET",m_rootTool->CutWstotAtHighET,"Cut on Wstot above HighETBinThreshold");
  // cut on EoverP above HighETBinThreshold 
  declareProperty("CutEoverPAtHighET",m_rootTool->CutEoverPAtHighET,"Cut on EoverP above HighETBinThreshold");
  // ET threshold for using high ET cuts and bin
  declareProperty("HighETBinThreshold",m_rootTool->HighETBinThreshold,"ET threshold for using high ET cuts and bin");
  // do pileup-dependent transform on discriminant value
  declareProperty("doPileupTransform",m_rootTool->doPileupTransform,"Do pileup-dependent transform on discriminant value");
  // do centrality-dependent transform on discriminant value
  declareProperty("doCentralityTransform",m_rootTool->doCentralityTransform,"Do centrality-dependent transform on discriminant value");
  // reference disc for very hard cut; used by pileup transform
  declareProperty("DiscHardCutForPileupTransform",m_rootTool->DiscHardCutForPileupTransform,"Reference disc for very hard cut; used by pileup transform");
  // reference slope on disc for very hard cut; used by pileup transform
  declareProperty("DiscHardCutSlopeForPileupTransform",m_rootTool->DiscHardCutSlopeForPileupTransform,"Reference slope on disc for very hard cut; used by pileup transform");
  // reference quadratic par on disc for very hard cut; used by centrality transform
  declareProperty("DiscHardCutQuadForPileupTransform",m_rootTool->DiscHardCutQuadForPileupTransform,"Reference quadratic par on disc for very hard cut; used by centrality transform");
  // reference disc for a pileup independent loose menu; used by pileup transform
  declareProperty("DiscLooseForPileupTransform",m_rootTool->DiscLooseForPileupTransform,"Reference disc for pileup indepdendent loose menu; used by pileup transform");
  // reference disc for very hard cut; used by pileup transform - 4-7 GeV bin
  declareProperty("DiscHardCutForPileupTransform4GeV",m_rootTool->DiscHardCutForPileupTransform4GeV,"Reference disc for very hard cut; used by pileup transform. 4-7 GeV bin");
  // reference slope on disc for very hard cut; used by pileup transform - 4-7 GeV bin
  declareProperty("DiscHardCutSlopeForPileupTransform4GeV",m_rootTool->DiscHardCutSlopeForPileupTransform4GeV,"Reference slope on disc for very hard cut; used by pileup transform. 4-7 GeV bin");
  // reference quadratic par on disc for very hard cut; used by centrality transform in 4-7 GeV bin
  declareProperty("DiscHardCutQuadForPileupTransform4GeV",m_rootTool->DiscHardCutQuadForPileupTransform4GeV,"Reference quadratic par on disc for very hard cut; used by centrality transform in 4-7 GeV bin");
  // reference disc for a pileup independent loose menu; used by pileup transform - 4-7 GeV bin
  declareProperty("DiscLooseForPileupTransform4GeV",m_rootTool->DiscLooseForPileupTransform4GeV,"Reference disc for pileup indepdendent loose menu; used by pileup transform. 4-7 GeV bin");
  // max discriminant for which pileup transform is to be used
  declareProperty("DiscMaxForPileupTransform",m_rootTool->DiscMaxForPileupTransform,"Max discriminant for which pileup transform is to be used");
  // max nvtx or mu to be used in pileup transform
  declareProperty("PileupMaxForPileupTransform",m_rootTool->PileupMaxForPileupTransform,"Max nvtx or mu to be used in pileup transform");
  // Flag to tell the tool if it is a calo-only LH
  declareProperty("caloOnly", m_caloOnly=false, "Flag to tell the tool if it is a calo-only LH");
}


//=============================================================================
// Standard destructor
//=============================================================================
AsgElectronLikelihoodTool::~AsgElectronLikelihoodTool()
{
  if(finalize().isFailure()){
    ATH_MSG_ERROR ( "Failure in AsgElectronLikelihoodTool finalize()");
  }
  delete m_rootTool;
}


//=============================================================================
// Asgena initialize method
//=============================================================================
StatusCode AsgElectronLikelihoodTool::initialize()
{

  std::string PDFfilename(""); //Default

  if(!m_WorkingPoint.empty()){
    m_configFile=AsgConfigHelper::findConfigFile(m_WorkingPoint,EgammaSelectors::LHPointToConfFile);
    ATH_MSG_INFO("operating point : " << this->getOperatingPointName());
  }
  
  if(!m_configFile.empty()){
    std::string configFile = PathResolverFindCalibFile( m_configFile);
    if(configFile==""){ 
	ATH_MSG_ERROR("Could not locate " << m_configFile );
	return StatusCode::FAILURE;
    } 

    ATH_MSG_DEBUG("Configfile to use  " << m_configFile );
    TEnv env(configFile.c_str());
    
    // Get the input PDFs in the tool.
    ATH_MSG_DEBUG("Get the input PDFs in the tool ");
    
    if(!m_pdfFileName.empty())
      {  //If the property was set by the user, take that.
	ATH_MSG_INFO("Setting user specified PDF file " << m_pdfFileName);
	PDFfilename = m_pdfFileName;
      } else {
      if (m_configFile.find("dev/") != std::string::npos) {
	
        std::string PDFdevval = env.GetValue("inputPDFFileName", "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root");
        PDFfilename = ("dev/"+PDFdevval);
        ATH_MSG_DEBUG ( "Getting the input PDFs from: " << PDFfilename  );
      } else {
        PDFfilename = env.GetValue("inputPDFFileName", "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root");
        ATH_MSG_DEBUG ( "Getting the input PDFs from: " << PDFfilename );
      }
    }
    std::string filename = PathResolverFindCalibFile( PDFfilename );
    if (!filename.empty()){
      m_rootTool->setPDFFileName( filename.c_str() );
    }else{
      ATH_MSG_ERROR ("Could not find PDF file");
      return StatusCode::FAILURE;
    }

    m_rootTool->VariableNames =  env.GetValue("VariableNames","");
    m_rootTool->CutLikelihood = AsgConfigHelper::HelperDouble("CutLikelihood",env);
    m_rootTool->CutLikelihoodPileupCorrection = AsgConfigHelper::HelperDouble("CutLikelihoodPileupCorrection", env);
    m_rootTool->CutLikelihood4GeV = AsgConfigHelper::HelperDouble("CutLikelihood4GeV",env);
    m_rootTool->CutLikelihoodPileupCorrection4GeV = AsgConfigHelper::HelperDouble("CutLikelihoodPileupCorrection4GeV", env);
    // do the conversion cut
    m_rootTool->doCutConversion = env.GetValue("doCutConversion", false);
    // do the ambiguity cut
    m_rootTool->CutAmbiguity  = AsgConfigHelper::HelperInt("CutAmbiguity", env);
    // cut on b-layer
    m_rootTool->CutBL = AsgConfigHelper::HelperInt("CutBL",env);
    // cut on pixel hits
    m_rootTool->CutPi = AsgConfigHelper::HelperInt("CutPi", env);
    // cut on precision hits
    m_rootTool->CutSi = AsgConfigHelper::HelperInt("CutSi", env);
    // cut on d0
    m_rootTool->CutA0 = AsgConfigHelper::HelperDouble("CutA0", env);
    // cut on deltaEta
    m_rootTool->CutDeltaEta = AsgConfigHelper::HelperDouble("CutDeltaEta", env);
    // cut on deltaPhiRes
    m_rootTool->CutDeltaPhiRes = AsgConfigHelper::HelperDouble("CutDeltaPhiRes", env);
    // turn off f3 at high Et
    m_rootTool->doRemoveF3AtHighEt = env.GetValue("doRemoveF3AtHighEt", false);
    // turn off TRTPID at high Et
    m_rootTool->doRemoveTRTPIDAtHighEt = env.GetValue("doRemoveTRTPIDAtHighEt", false);
    // do smooth interpolation between bins
    m_rootTool->doSmoothBinInterpolation = env.GetValue("doSmoothBinInterpolation", false);
    m_caloOnly = env.GetValue("caloOnly", false);

    m_rootTool->useHighETLHBinning = env.GetValue("useHighETLHBinning", false);
    m_rootTool->useOneExtraHighETLHBin = env.GetValue("useOneExtraHighETLHBin", false);
    // cut on Wstot above HighETBinThreshold
    m_rootTool->CutWstotAtHighET = AsgConfigHelper::HelperDouble("CutWstotAtHighET", env);
    // cut on EoverP above HighETBinThreshold
    m_rootTool->CutEoverPAtHighET = AsgConfigHelper::HelperDouble("CutEoverPAtHighET", env);
    m_rootTool->HighETBinThreshold = env.GetValue("HighETBinThreshold", 125);

    m_rootTool->doPileupTransform = env.GetValue("doPileupTransform", false);
    m_rootTool->doCentralityTransform = env.GetValue("doCentralityTransform", false);
    m_rootTool->DiscHardCutForPileupTransform = AsgConfigHelper::HelperDouble("DiscHardCutForPileupTransform",env);
    m_rootTool->DiscHardCutSlopeForPileupTransform = AsgConfigHelper::HelperDouble("DiscHardCutSlopeForPileupTransform",env);
    m_rootTool->DiscHardCutQuadForPileupTransform = AsgConfigHelper::HelperDouble("DiscHardCutQuadForPileupTransform",env);
    m_rootTool->DiscLooseForPileupTransform = AsgConfigHelper::HelperDouble("DiscLooseForPileupTransform",env);
    m_rootTool->DiscHardCutForPileupTransform4GeV = AsgConfigHelper::HelperDouble("DiscHardCutForPileupTransform4GeV",env);
    m_rootTool->DiscHardCutSlopeForPileupTransform4GeV = AsgConfigHelper::HelperDouble("DiscHardCutSlopeForPileupTransform4GeV",env);
    m_rootTool->DiscHardCutQuadForPileupTransform4GeV = AsgConfigHelper::HelperDouble("DiscHardCutQuadForPileupTransform4GeV",env);
    m_rootTool->DiscLooseForPileupTransform4GeV = AsgConfigHelper::HelperDouble("DiscLooseForPileupTransform4GeV",env);
    m_rootTool->DiscMaxForPileupTransform = env.GetValue("DiscMaxForPileupTransform", 2.0);
    m_rootTool->PileupMaxForPileupTransform = env.GetValue("PileupMaxForPileupTransform", 50);


  } else{  //Error if it cant find the conf
      ATH_MSG_ERROR("Could not find configuration file");
      return StatusCode::FAILURE;
  }

///-----------End of text config----------------------------

  // Get the name of the current operating point, and massage the other strings accordingly
  ATH_MSG_VERBOSE( "Going to massage the labels based on the provided operating point..." );

  // Get the message level and set the underlying ROOT tool message level accordingly
  m_rootTool->msg().setLevel(this->msg().level());
  
  // We need to initialize the underlying ROOT TSelectorTool
  if ( 0 == m_rootTool->initialize() )
    {
      ATH_MSG_ERROR ( "ERROR! Could not initialize the TElectronLikelihoodTool!" );
      return StatusCode::FAILURE;
    }

  // Copy the now filled TAccept and TResult to the dummy
  m_acceptDummy = m_rootTool->getTAccept();
  m_resultDummy = m_rootTool->getTResult();

  return StatusCode::SUCCESS ;
}


//=============================================================================
// Asgena finalize method (now called by destructor)
//=============================================================================
StatusCode AsgElectronLikelihoodTool::finalize()
{
  if ( !(m_rootTool->finalize()) )
    {
      ATH_MSG_ERROR ( "ERROR! Something went wrong at finalize!" );
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}


//=============================================================================
// The main accept method: the actual cuts are applied here 
//=============================================================================
const Root::TAccept& AsgElectronLikelihoodTool::accept( const xAOD::Electron* eg, double mu ) const
{
  if ( !eg ){
    ATH_MSG_ERROR ("Failed, no egamma object.");
    return m_acceptDummy;
  }

  if( eg->author() == xAOD::EgammaParameters::AuthorFwdElectron ){
    ATH_MSG_WARNING("Failed, this is a forward electron! The AsgElectronLikelihoodTool is only suitable for central electrons!");
    return m_acceptDummy;
  }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster ){
    ATH_MSG_ERROR("exiting because cluster is NULL " << cluster);
    return m_acceptDummy;
  }  

  if( !cluster->hasSampling(CaloSampling::CaloSample::EMB2) && !cluster->hasSampling(CaloSampling::CaloSample::EME2) ){
    ATH_MSG_ERROR("Failed, cluster is missing samplings EMB2 and EME2");
    return m_acceptDummy;
  }

  const double energy =  cluster->e();
  const float eta = (cluster->etaBE(2)); 

  // transverse energy of the electron (using the track eta) 
  //  const double et = eg->pt(); 
  double et = 0.;
  if(eg->trackParticle() && !m_caloOnly) {
    et  = ( cosh(eg->trackParticle()->eta()) != 0.) ? energy/cosh(eg->trackParticle()->eta()) : 0.;
  } else 
    et  = ( cosh(eta) != 0.) ? energy/cosh(eta) : 0.;
  
  // number of track hits
  uint8_t nSiHitsPlusDeadSensors(0);
  uint8_t nPixHitsPlusDeadSensors(0);
  bool passBLayerRequirement(false); 
  float d0(0.0);
  float deltaEta=0, deltaPhiRescaled2=0;
  float wstot=0, EoverP=0;
  int convBit(0); // this no longer works
  uint8_t ambiguityBit(0); 
  double ip(0);

  bool allFound = true;
  std::string notFoundList = "";

  // Wstot for use when CutWstotAtHighET vector is filled
  if( !eg->showerShapeValue(wstot, xAOD::EgammaParameters::wtots1) ){
    allFound = false;
    notFoundList += "wtots1 ";
  }

  // get the ambiguity type from the decoration
  if ( m_rootTool->CutAmbiguity.size() ) {
    if ( eg->isAvailable<uint8_t>("ambiguityType") ) {
      static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");    
      ambiguityBit = acc(*eg);
    } else {
      allFound = false;
      notFoundList += "ambiguityType ";
    }
  }
  
  if(!m_caloOnly) {
      // retrieve associated track
      const xAOD::TrackParticle* t  = eg->trackParticle();    
      if (t) {
        nSiHitsPlusDeadSensors = ElectronSelectorHelpers::numberOfSiliconHitsAndDeadSensors(t);
        nPixHitsPlusDeadSensors = ElectronSelectorHelpers::numberOfPixelHitsAndDeadSensors(t);
        passBLayerRequirement = ElectronSelectorHelpers::passBLayerRequirement(t);
        d0 = t->d0();
        EoverP = fabs(t->qOverP()) * energy;
      }
      else {
        ATH_MSG_ERROR( "Failed, no track particle. et= " << et << "eta= " << eta );
        return m_acceptDummy;
      }

      if( !eg->trackCaloMatchValue(deltaEta, xAOD::EgammaParameters::deltaEta1) ){
        allFound = false;
        notFoundList += "deltaEta1 ";
      }
      if( !eg->trackCaloMatchValue(deltaPhiRescaled2, xAOD::EgammaParameters::deltaPhiRescaled2) ){
        allFound = false;
        notFoundList += "deltaPhiRescaled2 ";
      }

  } //if not calo ONly

  // Get the number of primary vertices or FCal ET in this event
  bool doCentralityTransform = m_rootTool->doCentralityTransform;
  if( mu < 0 ){ // use npv if mu is negative (not given)
    if (doCentralityTransform) ip = static_cast<double>(m_useCaloSumsCont ? this->getFcalEt() : m_fcalEtDefault);
    else ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);
  }
  else {
    ip = mu;
  }

  // for now don't cache. 
  double likelihood = calculate(eg, ip); 

  ATH_MSG_VERBOSE( Form("PassVars: LH=%8.5f, eta=%8.5f, et=%8.5f, nSiHitsPlusDeadSensors=%i, nHitsPlusPixDeadSensors=%i, passBLayerRequirement=%i, convBit=%i, ambiguityBit=%i, d0=%8.5f, deltaEta=%8.5f, deltaphires=%5.8f, wstot=%8.5f, EoverP=%8.5f, ip=%8.5f",
			likelihood, eta, et,
			nSiHitsPlusDeadSensors, nPixHitsPlusDeadSensors,
			passBLayerRequirement,
			convBit, ambiguityBit, d0, deltaEta, deltaPhiRescaled2,
			wstot, EoverP, ip ));
  
  if (!allFound) {
    ATH_MSG_ERROR("Skipping LH rectangular cuts! The following variables are missing: " << notFoundList);
    return m_acceptDummy;
  }
  
  // Get the answer from the underlying ROOT tool
  return m_rootTool->accept( likelihood,
                             eta,
                             et,
                             nSiHitsPlusDeadSensors,
                             nPixHitsPlusDeadSensors,
                             passBLayerRequirement,
                             convBit,
                             ambiguityBit,
                             d0,
                             deltaEta,
                             deltaPhiRescaled2,
                             wstot,
                             EoverP,
                             ip
                           );
}

//=============================================================================
// Accept method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
//=============================================================================
const Root::TAccept& AsgElectronLikelihoodTool::accept( const xAOD::Egamma* eg, double mu) const
{
  if ( !eg ){
    ATH_MSG_ERROR ("Failed, no egamma object.");
    return m_acceptDummy;
  }

  // Call the main accept if this is not a calo-only LH
  if( !m_caloOnly ){
    const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(eg);
    return accept(el, mu); 
  }

  if( eg->author() == xAOD::EgammaParameters::AuthorFwdElectron ){
    ATH_MSG_WARNING("Failed, this is a forward electron! The AsgElectronLikelihoodTool is only suitable for central electrons!");
    return m_acceptDummy;
  }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster ){
    ATH_MSG_ERROR ("Failed, no cluster.");
    return m_acceptDummy;
  }  

  if( !cluster->hasSampling(CaloSampling::CaloSample::EMB2) && !cluster->hasSampling(CaloSampling::CaloSample::EME2) ){
    ATH_MSG_ERROR("Failed, cluster is missing samplings EMB2 and EME2");
    return m_acceptDummy;
  }
  
  const double energy =  cluster->e();
  const float eta = (cluster->etaBE(2)); 
  
  const double et  = ( cosh(eta) != 0.) ? energy/cosh(eta) : 0.;
  
  // Variables the EFCaloLH ignores
  uint8_t nSiHitsPlusDeadSensors(0);
  uint8_t nPixHitsPlusDeadSensors(0);
  bool passBLayerRequirement(false); 
  int convBit(0); // this no longer works
  uint8_t ambiguityBit(0);

  // Get the pileup or centrality information
  double ip(0);

  bool doCentralityTransform = m_rootTool->doCentralityTransform;
  if( mu < 0 ){ // use npv if mu is negative (not given)
    if (doCentralityTransform) ip = static_cast<double>(m_useCaloSumsCont ? this->getFcalEt() : m_fcalEtDefault);
    else ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);

  }
  else {
    ip = mu;
  }
  // for now don't cache. 
  double likelihood = calculate(eg, ip); 

  double deltaEta=0,deltaPhiRescaled2=0,d0=0;
  float wstot=0, EoverP=0;

  bool allFound = true;
  std::string notFoundList = "";

  // Wstot for use when CutWstotAtHighET vector is filled
  if( !eg->showerShapeValue(wstot, xAOD::EgammaParameters::wtots1) ){
    allFound = false; 
    notFoundList += "wtots1 ";
  }

  ATH_MSG_VERBOSE( Form("PassVars: LH=%8.5f, eta=%8.5f, et=%8.5f, nSiHitsPlusDeadSensors=%i, nPixHitsPlusDeadSensors=%i, passBLayerRequirement=%i, convBit=%i, ambiguityBit=%i, ip=%8.5f, wstot=%8.5f",
			likelihood, eta, et,
			nSiHitsPlusDeadSensors, nPixHitsPlusDeadSensors, 
			passBLayerRequirement,
			convBit, ambiguityBit, ip, wstot));
  

  if (!allFound) {
    ATH_MSG_ERROR("Skipping LH rectangular cuts! The following variables are missing: " << notFoundList);
    return m_acceptDummy;
  }

  // Get the answer from the underlying ROOT tool
  return m_rootTool->accept( likelihood,
                             eta,
                             et,
                             nSiHitsPlusDeadSensors,
                             nPixHitsPlusDeadSensors,
                             passBLayerRequirement,
                             convBit,
                             ambiguityBit,
                             d0,
                             deltaEta,
                             deltaPhiRescaled2,
                             wstot,
                             EoverP,
                             ip
                           );
}
   




//=============================================================================
// The main result method: the actual likelihood is calculated here
//=============================================================================
const Root::TResult& AsgElectronLikelihoodTool::calculate( const xAOD::Electron* eg, double mu ) const
{
  if ( !eg ){
    ATH_MSG_ERROR ("Failed, no egamma object.");
    return m_resultDummy;
  }

  if( eg->author() == xAOD::EgammaParameters::AuthorFwdElectron ){
    ATH_MSG_WARNING("Failed, this is a forward electron! The AsgElectronLikelihoodTool is only suitable for central electrons!");
    return m_resultDummy;
  }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster ){
    ATH_MSG_ERROR ("Failed, no cluster.");
    return m_resultDummy;
  }  

  if( !cluster->hasSampling(CaloSampling::CaloSample::EMB2) && !cluster->hasSampling(CaloSampling::CaloSample::EME2) ){
    ATH_MSG_ERROR("Failed, cluster is missing samplings EMB2 and EME2");
    return m_resultDummy;
  }

  const double energy =  cluster->e();
  const float eta = cluster->etaBE(2); 
  
  //double et = cluster->e()/cosh(eta); 
  // transverse energy of the electron (using the track eta) 
  //const double et = eg->pt(); 
  double et = 0.;
  if(eg->trackParticle() && !m_caloOnly) {
    et  = ( cosh(eg->trackParticle()->eta()) != 0.) ? energy/cosh(eg->trackParticle()->eta()) : 0.;
  } else 
    et  = ( cosh(eta) != 0.) ? energy/cosh(eta) : 0.;
  
  
  // number of track hits and other track quantities
  float trackqoverp(0.0);
  float d0(0.0);
  float d0sigma(0.0);
  double dpOverp(0.0);
  float TRT_PID(0.0);
  double trans_TRT_PID(0.0);
  float deltaEta=0, deltaPhiRescaled2=0;

  bool allFound = true;
  std::string notFoundList = "";

  if (!m_caloOnly){
  // retrieve associated TrackParticle
    const xAOD::TrackParticle* t = eg->trackParticle();    
    if (t)
      {
        trackqoverp = t->qOverP();
        d0 = t->d0();
        float vard0 = t->definingParametersCovMatrix()(0,0);
        if (vard0 > 0) {
	  d0sigma=sqrtf(vard0);
        }

        if( !t->summaryValue(TRT_PID, xAOD::eProbabilityHT) ){
          allFound = false; 
          notFoundList += "eProbabilityHT ";
        }

        //Transform the TRT PID output for use in the LH tool.
        double tau = 15.0; 
        double fEpsilon = 1.0e-30;  // to avoid zero division
	double pid_tmp = TRT_PID;
        if (pid_tmp >= 1.0) pid_tmp = 1.0 - 1.0e-15;  //this number comes from TMVA
        else if (pid_tmp <= fEpsilon) pid_tmp = fEpsilon;
        trans_TRT_PID = - log(1.0/pid_tmp - 1.0)*(1./double(tau));

        unsigned int index;
        if( t->indexOfParameterAtPosition(index, xAOD::LastMeasurement) ) {
	
	  double refittedTrack_LMqoverp  = 
	    t->charge() / sqrt(std::pow(t->parameterPX(index), 2) +
			     std::pow(t->parameterPY(index), 2) +
			     std::pow(t->parameterPZ(index), 2));
	
	  dpOverp = 1 - trackqoverp/(refittedTrack_LMqoverp);
        }
        else{
          allFound = false; 
          notFoundList += "deltaPoverP ";
        }
      
      }
    else
      {
        ATH_MSG_ERROR( "Failed, no track particle. et= " << et << "eta= " << eta );
        return m_resultDummy;
      }
  }  // if not calo Only

  float Reta(0), Rphi(0),  Rhad1(0), Rhad(0), w2(0), f1(0), Eratio(0), f3(0);

  // reta = e237/e277
  if( !eg->showerShapeValue(Reta, xAOD::EgammaParameters::Reta) ){
    allFound = false; 
    notFoundList += "Reta ";
  }
  // rphi e233/e237
  if( !eg->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi) ){
    allFound = false;
    notFoundList += "Rphi ";
  }
  // rhad1 = ethad1/et
  if( !eg->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1) ){
    allFound = false;
    notFoundList += "Rhad1 ";
  }
  // rhad = ethad/et
  if( !eg->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad) ){
    allFound = false;
    notFoundList += "Rhad ";
  }
  // shower width in 2nd sampling
  if( !eg->showerShapeValue(w2, xAOD::EgammaParameters::weta2) ){
    allFound = false;
    notFoundList += "weta2 ";
  }
  // fraction of energy reconstructed in the 1st sampling
  if( !eg->showerShapeValue(f1, xAOD::EgammaParameters::f1) ){
    allFound = false;
    notFoundList += "f1 ";
  }
  // E of 2nd max between max and min in strips
  if( !eg->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio) ){
    allFound = false;
    notFoundList += "Eratio ";
  }
  // fraction of energy reconstructed in the 3rd sampling
  if( !eg->showerShapeValue(f3, xAOD::EgammaParameters::f3) ){
    allFound = false;
    notFoundList += "f3 ";
  }

  if( !m_caloOnly){
    // deltaEta1
    if( !eg->trackCaloMatchValue(deltaEta, xAOD::EgammaParameters::deltaEta1) ){
      allFound = false;
      notFoundList += "deltaEta1 ";
    }
    // difference between the cluster phi (sampling 2) and the eta of the track extrapolated from the last measurement point.
    if( !eg->trackCaloMatchValue(deltaPhiRescaled2, xAOD::EgammaParameters::deltaPhiRescaled2) ){
      allFound = false;
      notFoundList += "deltaPhiRescaled2 ";
    }

  }

  // Get the number of primary vertices or FCal ET in this event
  double ip = static_cast<double>(m_nPVdefault);

  bool doCentralityTransform = m_rootTool->doCentralityTransform;
  if( mu < 0 ){ // use npv if mu is negative (not given)
    if (doCentralityTransform) ip = static_cast<double>(m_useCaloSumsCont ? this->getFcalEt() : m_fcalEtDefault);
    else ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);
  }
  else{
    ip = mu;
  }

  ATH_MSG_VERBOSE( Form("Vars: eta=5%8.5f, et=%8.5f, f3=%8.5f, rHad==%8.5f, rHad1=%8.5f, Reta=%8.5f, w2=%8.5f, f1=%8.5f, Emaxs1=%8.5f, deltaEta=%8.5f, d0=%8.5f, d0sigma=%8.5f, Rphi=%8.5f, dpOverp=%8.5f, deltaPhiRescaled2=%8.5f, TRT_PID=%8.5f, trans_TRT_PID=%8.5f, ip=%8.5f",
      eta, et, f3, Rhad, Rhad1, Reta,
      w2, f1, Eratio,
      deltaEta, d0,
      d0sigma, 
      Rphi, dpOverp, deltaPhiRescaled2,
      TRT_PID, trans_TRT_PID,
      ip ) );

  if (!allFound) {
    ATH_MSG_ERROR("Skipping LH calculation! The following variables are missing: " << notFoundList);
    return m_resultDummy;
  }

  // Get the answer from the underlying ROOT tool
  return m_rootTool->calculate( eta,
                                et,
                                f3,
                                Rhad,
                                Rhad1,
                                Reta,
                                w2,
                                f1,
                                Eratio,
                                deltaEta,
                                d0,
                                d0sigma,
                                Rphi,
                                dpOverp,
                                deltaPhiRescaled2,
                                trans_TRT_PID,
                                ip
                                );
}

//=============================================================================
// Calculate method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
//=============================================================================
const Root::TResult& AsgElectronLikelihoodTool::calculate( const xAOD::Egamma* eg, double mu ) const
{
  if ( !eg ){
    ATH_MSG_ERROR ("Failed, no egamma object.");
    return m_resultDummy;
  }

  if( !m_caloOnly ){
    const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(eg);
    return calculate(el, mu);
  }

  if( eg->author() == xAOD::EgammaParameters::AuthorFwdElectron ){
    ATH_MSG_WARNING("Failed, this is a forward electron! The AsgElectronLikelihoodTool is only suitable for central electrons!");
    return m_resultDummy;
  }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster ){
    ATH_MSG_ERROR ("Failed, no cluster.");
    return m_resultDummy;
  }  

  if( !cluster->hasSampling(CaloSampling::CaloSample::EMB2) && !cluster->hasSampling(CaloSampling::CaloSample::EME2) ){
    ATH_MSG_ERROR("Failed, cluster is missing samplings EMB2 and EME2");
    return m_resultDummy;
  }
  
  const double energy =  cluster->e();
  const float eta = cluster->etaBE(2); 
  
  const double et  = ( cosh(eta) != 0.) ? energy/cosh(eta) : 0.;

  // Track variables that the EFCaloLH will not use
  float d0(0.0);
  float d0sigma(0.0);
  double dpOverp(0.0);

  float deltaEta=0, deltaPhiRescaled2=0;
  float TRT_PID(0.0);

  // Calo Variables
  float Reta(0), Rphi(0),  Rhad1(0), Rhad(0), w2(0), f1(0), Eratio(0), f3(0);

  bool allFound = true;
  std::string notFoundList = "";

  // reta = e237/e277
  if( !eg->showerShapeValue(Reta, xAOD::EgammaParameters::Reta) ){
    allFound = false; 
    notFoundList += "Reta ";
  }
  // rphi e233/e237
  if( !eg->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi) ){
    allFound = false; 
    notFoundList += "Rphi ";
  }
  // rhad1 = ethad1/et
  if( !eg->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1) ){
    allFound = false; 
    notFoundList += "Rhad1 ";
  }
  // rhad = ethad/et
  if( !eg->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad) ){
    allFound = false; 
    notFoundList += "Rhad ";
  }
  // shower width in 2nd sampling
  if( !eg->showerShapeValue(w2, xAOD::EgammaParameters::weta2) ){
    allFound = false; 
    notFoundList += "weta2 ";
  }
  // fraction of energy reconstructed in the 1st sampling
  if( !eg->showerShapeValue(f1, xAOD::EgammaParameters::f1) ){
    allFound = false; 
    notFoundList += "f1 ";
  }
  // E of 2nd max between max and min in strips
  if( !eg->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio) ){
    allFound = false; 
    notFoundList += "Eratio ";
  }
  // fraction of energy reconstructed in the 3rd sampling
  if( !eg->showerShapeValue(f3, xAOD::EgammaParameters::f3) ){
    allFound = false; 
    notFoundList += "f3 ";
  }

  // Get the pileup or centrality information
  double ip(0);

  bool doCentralityTransform = m_rootTool->doCentralityTransform;
  if( mu < 0 ){ // use npv if mu is negative (not given)
    if (doCentralityTransform) ip = static_cast<double>(m_useCaloSumsCont ? this->getFcalEt() : m_fcalEtDefault);
    else ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);
  }
  else {
    ip = mu;
  }

  ATH_MSG_VERBOSE(Form("Vars: eta=%8.5f, et=%8.5f, f3=%8.5f, rHad==%8.5f, rHad1=%8.5f, Reta=%8.5f, w2=%8.5f, f1=%8.5f, Emaxs1=%8.5f, deltaEta=%8.5f, d0=%8.5f, d0sigma=%8.5f, Rphi=%8.5f, dpOverp=%8.5f, deltaPhiRescaled2=%8.5f, TRT_PID=%8.5f, ip=%8.5f",
				 eta, et, f3, Rhad, Rhad1, Reta,
				 w2, f1, Eratio,
				 deltaEta, d0,
				 d0sigma, 
				 Rphi, dpOverp, deltaPhiRescaled2,
				 TRT_PID,
                                 ip ));

  if (!allFound) {
    ATH_MSG_ERROR("Skipping LH calculation! The following variables are missing: " << notFoundList);
    return m_resultDummy;
  }

  // Get the answer from the underlying ROOT tool
  return m_rootTool->calculate( eta,
                                et,
                                f3,
                                Rhad,
                                Rhad1,
                                Reta,
                                w2,
                                f1,
                                Eratio,
                                deltaEta,
                                d0,
                                d0sigma,
                                Rphi,
                                dpOverp,
                                deltaPhiRescaled2,
				TRT_PID,
                                ip
                                );
}

/** Method to get the plain TAccept */
const Root::TAccept& AsgElectronLikelihoodTool::getTAccept( ) const{
  return m_rootTool->getTAccept();
}

/** Method to get the plain TResult */
const Root::TResult& AsgElectronLikelihoodTool::getTResult( ) const{
  return m_rootTool->getTResult();
}


//=============================================================================
/// Get the name of the current operating point
//=============================================================================
std::string AsgElectronLikelihoodTool::getOperatingPointName() const
{
  return m_WorkingPoint;
}
//=============================================================================
const Root::TAccept& AsgElectronLikelihoodTool::accept(const xAOD::IParticle* part) const
{
  ATH_MSG_DEBUG("Entering accept( const IParticle* part )");
  const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
  if(eg)
    {
      return accept(eg);
    }
  else{
    ATH_MSG_ERROR("AsgElectronLikelihoodTool::could not cast to const Electron");
    return m_acceptDummy;
  }
}

const Root::TResult& AsgElectronLikelihoodTool::calculate(const xAOD::IParticle* part) const
{
  const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
  if (eg)
    {
      return calculate(eg);
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
unsigned int AsgElectronLikelihoodTool::getNPrimVertices() const
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

//=============================================================================
// Helper method to get FCal ET for centrality determination
//=============================================================================
double AsgElectronLikelihoodTool::getFcalEt() const
{
  static bool CaloSumsExists = true; 
  double fcalEt(0.);
  const xAOD::HIEventShapeContainer* HIESContainer(0);
  if(CaloSumsExists){
    if ( StatusCode::SUCCESS != evtStore()->retrieve( HIESContainer, m_CaloSumsContName ) )
    {
      ATH_MSG_WARNING ( "CaloSums container not found with name: " << m_CaloSumsContName );
      CaloSumsExists = false; // if retrieve failed, don't try to retrieve again
      return fcalEt;
    }
    xAOD::HIEventShapeContainer::const_iterator es_itr = HIESContainer->begin();
    xAOD::HIEventShapeContainer::const_iterator es_end = HIESContainer->end();
    for (; es_itr != es_end; es_itr++){
      double et = (*es_itr)->et();
      const std::string name = (*es_itr)->auxdataConst<std::string>("Summary");
      if (name == "FCal") fcalEt = et*1.e-6;
    }
  }
  return fcalEt;
}

