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
#include "ElectronPhotonSelectorTools/AsgElectronPhotonIsEMSelectorConfigHelper.h"

// STL includes
#include <string>
#include <cstdint>
#include <cmath>

#include "TSystem.h"

#include "xAODEgamma/Electron.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
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
  declareProperty("ConfigFile",m_configFile="","The config file to use");
  declareProperty("usePVContainer", m_usePVCont=true, "Whether to use the PV container");
  declareProperty("nPVdefault", m_nPVdefault = 11, "The default number of PVs if not counted");
  declareProperty("primaryVertexContainer", m_primVtxContName="PrimaryVertices", "The primary vertex container name" );


  //
  // Configurables in the root tool
  //
  // pdf file name. Managed in the Asg tool.
  declareProperty("inputPDFFileName",  m_pdfFileName="ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root", "The input ROOT file name that holds the PDFs" );
  // Operating Point. Managed in the Asg tool.
  declareProperty("OperatingPoint",m_operatingPoint=0,"Likelihood operating point");
  // the variable names, if non-standard - nope, it's done above!
  declareProperty("VariableNames",m_rootTool->VariableNames,"Variable names input to the LH");
  // The likelihood cut values
  declareProperty("CutLikelihood",m_rootTool->CutLikelihood,"Cut on likelihood discriminant");
  // The pileup-correction part of the likelihood cut values
  declareProperty("CutLikelihoodPileupCorrection",m_rootTool->CutLikelihoodPileupCorrection,"Pileup correction for LH discriminant");
  // do the conversion cut
  declareProperty("doCutConversion",m_rootTool->doCutConversion,"Apply the conversion bit cut");
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
  // do pileup-dependent transform on discriminant value
  declareProperty("doPileupTransform",m_rootTool->doPileupTransform,"Do pileup-dependent transform on discriminant value");
  // reference disc for very hard cut; used by pileup transform
  declareProperty("DiscHardCutForPileupTransform",m_rootTool->DiscHardCutForPileupTransform,"Reference disc for very hard cut; used by pileup transform");
  // reference slope on disc for very hard cut; used by pileup transform
  declareProperty("DiscHardCutSlopeForPileupTransform",m_rootTool->DiscHardCutSlopeForPileupTransform,"Reference slope on disc for very hard cut; used by pileup transform");
  // reference disc for a pileup independent loose menu; used by pileup transform
  declareProperty("DiscLooseForPileupTransform",m_rootTool->DiscLooseForPileupTransform,"Reference disc for pileup indepdendent loose menu; used by pileup transform");
  // max discriminant for which pileup transform is to be used
  declareProperty("DiscMaxForPileupTransform",m_rootTool->DiscMaxForPileupTransform,"Max discriminant for which pileup transform is to be used");
  // max nvtx or mu to be used in pileup transform
  declareProperty("PileupMaxForPileupTransform",m_rootTool->PileupMaxForPileupTransform,"Max nvtx or mu to be used in pileup transform");
  declareProperty("caloOnly", m_caloOnly=false, "Flag to tell the tool if its a calo only LH");
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
  // Add an input file that holds the PDFs

//---------------start of text config------------------------
// TODO: Sort out this set operating point stuff.
  if(!m_configFile.empty()){
    std::string configFile = PathResolverFindCalibFile( m_configFile);
    TEnv env(configFile.c_str());

    // Get the input PDFs in the tool.
    std::string PDFfilename;
    
    if (m_configFile.find("dev/") != std::string::npos) {
      
      std::string PDFdevval = env.GetValue("inputPDFFileName", "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root");
      PDFfilename = ("dev/"+PDFdevval);
       ATH_MSG_DEBUG ( "Getting the input PDFs from: " << PDFfilename  );
    } else {
      PDFfilename = env.GetValue("inputPDFFileName", "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root");
       ATH_MSG_DEBUG ( "Getting the input PDFs from: " << PDFfilename );
    }
    
    std::string filename = PathResolverFindCalibFile( PDFfilename );
    if (!filename.empty()){
      m_pdfFileName = filename.c_str();
    }
    m_rootTool->setPDFFileName( m_pdfFileName );
    m_rootTool->VariableNames =  env.GetValue("VariableNames","");
    m_rootTool->CutLikelihood = AsgConfigHelper::HelperDouble("CutLikelihood",env);
    m_rootTool->CutLikelihoodPileupCorrection = AsgConfigHelper::HelperDouble("CutLikelihoodPileupCorrection", env);
    // do the conversion cut
    m_rootTool->doCutConversion = env.GetValue("doCutConversion", false);
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
    m_operatingPoint = env.GetValue("OperatingPoint", 0);
    m_caloOnly = env.GetValue("caloOnly", false);

    m_rootTool->doPileupTransform = env.GetValue("doPileupTransform", false);
    m_rootTool->DiscHardCutForPileupTransform = AsgConfigHelper::HelperDouble("DiscHardCutForPileupTransform",env);
    m_rootTool->DiscHardCutSlopeForPileupTransform = AsgConfigHelper::HelperDouble("DiscHardCutSlopeForPileupTransform",env);
    m_rootTool->DiscLooseForPileupTransform = AsgConfigHelper::HelperDouble("DiscLooseForPileupTransform",env);
    m_rootTool->DiscMaxForPileupTransform = env.GetValue("DiscMaxForPileupTransform", 2.0);
    m_rootTool->PileupMaxForPileupTransform = env.GetValue("PileupMaxForPileupTransform", 50);


  }  else{  //Do python config if m_configFile is empty.
    std::string filename = PathResolverFindCalibFile( m_pdfFileName );
	if (filename.empty()){
      ATH_MSG_WARNING ( "Could NOT resolve file name " << m_pdfFileName );
    }  else{
      ATH_MSG_INFO(" Path found = "<<filename);
    }
    const char* fname= filename.c_str();
    m_rootTool->setPDFFileName ( fname );

  // Set the cut value to be used
  }
///-----------End of text config----------------------------
  //First try the PathResolver
//  std::string filename = PathResolverFindCalibFile( m_pdfFileName );
//  if (filename.empty()){
//    ATH_MSG_WARNING ( "Could NOT resolve file name " << m_pdfFileName );
//  }  else{
//    ATH_MSG_INFO(" Path found = "<<filename);
//  }

 // const char* fname= filename.c_str();
 // m_rootTool->setPDFFileName ( fname );

  m_rootTool->setOperatingPoint( (LikeEnum::Menu)m_operatingPoint );
  // Get the name of the current operating point, and massage the other strings accordingly
  ATH_MSG_VERBOSE( "Going to massage the labels based on the provided operating point..." );

  // Get the message level and set the underlying ROOT tool message level accordingly
  bool debug(false);
  if ( this->msgLvl(MSG::VERBOSE) ) debug = true;
  m_rootTool->setDebug(debug);

  
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
  if ( !eg )
    {
      ATH_MSG_DEBUG ("Failed, no egamma object.");
      return m_acceptDummy;
    }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster )
    {
      ATH_MSG_DEBUG ("Failed, no cluster.");
      return m_acceptDummy;
    }  
  
  const float eta = (cluster->etaBE(2)); 
  if ( fabs(eta) > 300.0 )
    {
      ATH_MSG_DEBUG ("Failed, eta range.");
      return m_acceptDummy;
    }
  
  // transverse energy of the electron (using the track eta) 
  const double et = eg->pt(); 

  // number of track hits
  uint8_t nSi(0);
  uint8_t nSiDeadSensors(0);
  uint8_t nPix(0);
  uint8_t nSCT(0);
  uint8_t nPixDeadSensors(0); 
  uint8_t nSCTDeadSensors(0); 
  uint8_t expectBlayer(true);
  uint8_t nBlayerHits(0); 
  uint8_t nBlayerOutliers(0); 
  float d0(0.0);
  float deltaEta=0, deltaPhiRescaled2=0;
  int convBit(0); // this no longer works
  double ip(0);

  bool allFound = true;

  if(!m_caloOnly) {
      // retrieve associated track
      const xAOD::TrackParticle* t  = eg->trackParticle();    
      if (t)
        {
           d0 = t->d0();
           allFound = allFound && t->summaryValue(nPix, xAOD::numberOfPixelHits);
           allFound = allFound && t->summaryValue(nSCT, xAOD::numberOfSCTHits);
           nSi = nPix + nSCT;
           allFound = allFound && t->summaryValue(nPixDeadSensors, xAOD::numberOfPixelDeadSensors);
           allFound = allFound && t->summaryValue(nSCTDeadSensors, xAOD::numberOfSCTDeadSensors);
           nSiDeadSensors = nPixDeadSensors + nSCTDeadSensors;

           allFound = allFound && t->summaryValue(expectBlayer, xAOD::expectBLayerHit);
           allFound = allFound && t->summaryValue(nBlayerHits, xAOD::numberOfBLayerHits);
           allFound = allFound && t->summaryValue(nBlayerOutliers, xAOD::numberOfBLayerOutliers);

        }
      else
        {
          ATH_MSG_WARNING ( "Failed, no track particle: et= " << et << "eta= " << eta );
        }
      //if ( nSi < 4 ){ et = cluster->et(); }

      allFound = allFound && eg->trackCaloMatchValue(deltaEta, xAOD::EgammaParameters::deltaEta1);
      allFound = allFound && eg->trackCaloMatchValue(deltaPhiRescaled2, xAOD::EgammaParameters::deltaPhiRescaled2);

  } //if not calo ONly

  // Get the conversion bit
  // //convBit = eg->isem(0x1 << egammaPID::ConversionMatch_Electron) == 0;
  // unsigned int isem = (unsigned int)eg->egammaID (egammaPID::IsEM);
  // convBit = isem & (0x1 << egammaPID::ConversionMatch_Electron);

  // Get the number of primary vertices in this events
  if( mu < 0 ){
    ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);
  }
  else {
    ip = mu;
  }

  // for now don't cache. 
  double likelihood = calculate(eg, ip); 

  ATH_MSG_VERBOSE ( Form("PassVars: LH=%8.5f, eta=%8.5f, et=%8.5f, nSi=%i, nSiDeadSensors=%i, nPix=%i, nPixDeadSensors=%i, nBlayerHits=%i, nBlayerOutliers=%i, expectBlayer=%i, convBit=%i, d0=%8.5f, deltaEta=%8.5f, deltaphires=%5.8f, ip=%8.5f",
                         likelihood, eta, et,
                         nSi, nSiDeadSensors, nPix, nPixDeadSensors,
                         nBlayerHits, nBlayerOutliers, expectBlayer,
                         convBit, d0, deltaEta, deltaPhiRescaled2, ip ) );


  if (!allFound) {
    ATH_MSG_WARNING("Have some variables missing.");
  }

  // Get the answer from the underlying ROOT tool
  return m_rootTool->accept( likelihood,
                             eta,
                             et,
                             nSi,
                             nSiDeadSensors,
                             nPix,
                             nPixDeadSensors,
                             nBlayerHits,
                             nBlayerOutliers,
                             expectBlayer,
                             convBit,
			     d0,
			     deltaEta,
			     deltaPhiRescaled2,
                             ip
                             );
}

//=============================================================================
// Accept method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
//=============================================================================
const Root::TAccept& AsgElectronLikelihoodTool::accept( const xAOD::Egamma* eg, double mu) const
{
  if( !m_caloOnly )
    {
      const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(eg);
      return accept(el, mu); 
    }
  if ( !eg )
    {
      ATH_MSG_DEBUG ("Failed, no egamma object.");
      return m_acceptDummy;
    }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster )
    {
      ATH_MSG_DEBUG ("Failed, no cluster.");
      return m_acceptDummy;
    }  
  
  const float eta = (cluster->etaBE(2)); 
  if ( fabs(eta) > 300.0 )
    {
      ATH_MSG_DEBUG ("Failed, eta range.");
      return m_acceptDummy;
    }
  
  // transverse energy of the electron (using the track eta) 
  const double et = eg->pt(); 

  // Variables the EFCaloLH ignores
  uint8_t nSi(0);
  uint8_t nSiDeadSensors(0);
  uint8_t nPix(0);
  uint8_t nPixDeadSensors(0); 
  uint8_t expectBlayer(true);
  uint8_t nBlayerHits(0); 
  uint8_t nBlayerOutliers(0); 
  int convBit(0);

  // Temporary until mu input is sorted

  double ip(0);
  if( mu < 0 ){
    ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);
  }
  else {
    ip = mu;
  }
  // for now don't cache. 
  double likelihood = calculate(eg, ip); 

  ATH_MSG_VERBOSE ( Form("PassVars: LH=%8.5f, eta=%8.5f, et=%8.5f, nSi=%i, nSiDeadSensors=%i, nPix=%i, nPixDeadSensors=%i, nBlayerHits=%i, nBlayerOutliers=%i, expectBlayer=%i, convBit=%i, ip=%8.5f",
                         likelihood, eta, et,
                         nSi, nSiDeadSensors, nPix, nPixDeadSensors,
                         nBlayerHits, nBlayerOutliers, expectBlayer,
                         convBit, ip ) );

  double deltaEta=0,deltaPhiRescaled2=0,d0=0;

  // Get the answer from the underlying ROOT tool
  return m_rootTool->accept( likelihood,
                             eta,
                             et,
                             nSi,
                             nSiDeadSensors,
                             nPix,
                             nPixDeadSensors,
                             nBlayerHits,
                             nBlayerOutliers,
                             expectBlayer,
                             convBit,
			     d0,
			     deltaEta,
			     deltaPhiRescaled2,
                             ip
                             );
}
   




//=============================================================================
// The main result method: the actual likelihood is calculated here
//=============================================================================
const Root::TResult& AsgElectronLikelihoodTool::calculate( const xAOD::Electron* eg, double mu ) const
{
  if ( !eg )
    {
      ATH_MSG_DEBUG ("Failed, no egamma object.");
      return m_resultDummy;
    }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster )
    {
      ATH_MSG_DEBUG ("Failed, no cluster.");
      return m_resultDummy;
    }  
  
  const float eta = cluster->etaBE(2); 
  if ( fabs(eta) > 300.0 )
    {
      ATH_MSG_DEBUG ("Failed, eta range.");
      return m_resultDummy;
    }
  
  double et = cluster->e()/cosh(eta); 
  

  // number of track hits and other track quantities
  //int nSi(0);
  uint8_t nTRThigh(0); 
  uint8_t nTRThighOutliers(0); 
  uint8_t nTRT(0); 
  uint8_t nTRTOutliers(0);
  uint8_t nTRTXenon(0); 
  float trackqoverp(0.0);
  float d0(0.0);
  float d0sigma(0.0);
  double dpOverp(0.0);
  float TRT_PID(0.0);
  float deltaEta=0, deltaPhiRescaled2=0;
  double rTRT(0.0);

  bool allFound = true;
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

      //nSi = summary->get(Trk::numberOfPixelHits)+summary->get(Trk::numberOfSCTHits);
        allFound = allFound && t->summaryValue(nTRThigh, xAOD::numberOfTRTHighThresholdHits);
        allFound = allFound && t->summaryValue(nTRThighOutliers, xAOD::numberOfTRTHighThresholdOutliers);
        allFound = allFound && t->summaryValue(nTRT, xAOD::numberOfTRTHits);
        allFound = allFound && t->summaryValue(nTRTOutliers, xAOD::numberOfTRTOutliers);
        allFound = allFound && t->summaryValue(nTRTXenon, xAOD::numberOfTRTXenonHits);
        allFound = allFound && t->summaryValue(TRT_PID, xAOD::eProbabilityComb);

        unsigned int index;
        if( t->indexOfParameterAtPosition(index, xAOD::LastMeasurement) ) {
	
	  double refittedTrack_LMqoverp  = 
	    t->charge() / sqrt(std::pow(t->parameterPX(index), 2) +
			     std::pow(t->parameterPY(index), 2) +
			     std::pow(t->parameterPZ(index), 2));
	
	  dpOverp = 1 - trackqoverp/(refittedTrack_LMqoverp);
        }
      
      }
    else
      {
        ATH_MSG_WARNING ( "Failed, no track particle: et= " << et << "eta= " << eta );
      }
    //if ( nSi < 4 ){ et = cluster->et(); }
  }  // if not calo Only

  //float e233(0), e237(0), e277(0), ethad1(0), ethad(0), ws3(0), w2(0), f1(0), emax2(0), emax(0), f3(0);
  float Reta(0), Rphi(0),  Rhad1(0), Rhad(0), ws3(0), w2(0), f1(0), Eratio(0), f3(0);

  // reta = e237/e277
  allFound = allFound && eg->showerShapeValue(Reta, xAOD::EgammaParameters::Reta);
  // rphi e233/e237
  allFound = allFound && eg->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi);
  // E(7*7) in 2nd sampling
 // allFound = allFound && eg->showerShapeValue(e277, xAOD::EgammaParameters::e277);
  // rhad1 = ethad1/et
  allFound = allFound && eg->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1);
  // rhad = ethad/et
  allFound = allFound && eg->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad);
  // shower width in 3 strips in 1st sampling
  allFound = allFound && eg->showerShapeValue(ws3, xAOD::EgammaParameters::weta1);
  // shower width in 2nd sampling
  allFound = allFound && eg->showerShapeValue(w2, xAOD::EgammaParameters::weta2);
  // fraction of energy reconstructed in the 1st sampling
  allFound = allFound && eg->showerShapeValue(f1, xAOD::EgammaParameters::f1);
  // E of 2nd max between max and min in strips
  allFound = allFound && eg->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio);
  // E of 1st max in strips
//  allFound = allFound && eg->showerShapeValue(emax, xAOD::EgammaParameters::emaxs1);
  // fraction of energy reconstructed in the 3rd sampling
  allFound = allFound && eg->showerShapeValue(f3, xAOD::EgammaParameters::f3);

 // double rHad1  = et != 0. ? ethad1/et : 0.;
 // double rHad   = et != 0. ? ethad/et : 0.;
  
 // float Reta   = e277 != 0 ? e237/e277 : 0.;
 // float Rphi   = e237 != 0. ? e233 / e237 : 0.;
  
  //float Eratio = (emax+emax2)>0. ? (emax-emax2)/(emax+emax2) : 0.0;
  

  // Delta eta,phi matching

  if( !m_caloOnly){
    allFound = allFound && eg->trackCaloMatchValue(deltaEta, xAOD::EgammaParameters::deltaEta1);

  // difference between the cluster phi (sampling 2) and the eta of the track extrapolated from the last measurement point.
    allFound = allFound && eg->trackCaloMatchValue(deltaPhiRescaled2, xAOD::EgammaParameters::deltaPhiRescaled2);
  

    // TRT high-to-low threshold hits ratio
    if ( nTRTXenon > 0 )
      {
        rTRT = ((double)(nTRThigh+nTRThighOutliers)) / ((double)(nTRTXenon)) ;
      }
  // else if ( nTRTXenon < 0 && (nTRT+nTRTOutliers) > 0 ) // this is always false as given
  //   {
  //     rTRT = ((double)(nTRThigh+nTRThighOutliers)) / ((double)(nTRT+nTRTOutliers)) ;
  //   }
  }

  // Get the number of primary vertices in this events

  double ip = static_cast<double>(m_nPVdefault);

  if(mu < 0){  // use npv if mu is negative (not given)
    if (!m_caloOnly){
      ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);
    }
  }
  else{
    ip = mu;
  }

  if (!m_caloOnly){
    ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);
  }


  ATH_MSG_VERBOSE ( Form("Vars: eta=5%8.5f, et=%8.5f, f3=%8.5f, rHad==%8.5f, rHad1=%8.5f, Reta=%8.5f, w2=%8.5f, f1=%8.5f, Emaxs1=%8.5f, deltaEta=%8.5f, d0=%8.5f, rTRT=%8.5f, d0sigma=%8.5f, Rphi=%8.5f, ws3=%8.5f, dpOverp=%8.5f, deltaPhiRescaled2=%8.5f, TRT_PID=%8.5f, ip=%8.5f",
                         eta, et, f3, Rhad, Rhad1, Reta,
                         w2, f1, Eratio,
                         deltaEta, d0, rTRT,
                         d0sigma, 
                         Rphi, ws3, dpOverp, deltaPhiRescaled2,
			 TRT_PID,
                         ip ) );


  if (!allFound) {
    ATH_MSG_WARNING("Have some variables missing.");
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
                                d0, //??? el_trackd0pv
                                rTRT,
                                d0sigma, //??? el_tracksigd0pv
                                Rphi,
                                dpOverp,
                                deltaPhiRescaled2,
				TRT_PID,
                                ip
                                );
}

//=============================================================================
// Calculate method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
//=============================================================================
const Root::TResult& AsgElectronLikelihoodTool::calculate( const xAOD::Egamma* eg, double mu ) const
{
  if( !m_caloOnly )
    {
      const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(eg);
      return calculate(el, mu);
    }
  if ( !eg )
    {
      ATH_MSG_DEBUG ("Failed, no egamma object.");
      return m_resultDummy;
    }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster )
    {
      ATH_MSG_DEBUG ("Failed, no cluster.");
      return m_resultDummy;
    }  
  
  const float eta = cluster->etaBE(2); 
  if ( fabs(eta) > 300.0 )
    {
      ATH_MSG_DEBUG ("Failed, eta range.");
      return m_resultDummy;
    }
  
  double et = cluster->e()/cosh(eta); 
  
  // Track variables that the EFCaloLH will not use
  float d0(0.0);
  float d0sigma(0.0);
  double dpOverp(0.0);

  float deltaEta=0, deltaPhiRescaled2=0;
  double rTRT(0.0);
  float TRT_PID(0.0);

  // Calo Variables
  float Reta(0), Rphi(0),  Rhad1(0), Rhad(0), ws3(0), w2(0), f1(0), Eratio(0), f3(0);

  bool allFound = true;

  // reta = e237/e277
  allFound = allFound && eg->showerShapeValue(Reta, xAOD::EgammaParameters::Reta);
  // rphi e233/e237
  allFound = allFound && eg->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi);
  // rhad1 = ethad1/et
  allFound = allFound && eg->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1);
  // rhad = ethad/et
  allFound = allFound && eg->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad);
  // shower width in 3 strips in 1st sampling
  allFound = allFound && eg->showerShapeValue(ws3, xAOD::EgammaParameters::weta1);
  // shower width in 2nd sampling
  allFound = allFound && eg->showerShapeValue(w2, xAOD::EgammaParameters::weta2);
  // fraction of energy reconstructed in the 1st sampling
  allFound = allFound && eg->showerShapeValue(f1, xAOD::EgammaParameters::f1);
  // E of 2nd max between max and min in strips
  allFound = allFound && eg->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio);
  // fraction of energy reconstructed in the 3rd sampling
  allFound = allFound && eg->showerShapeValue(f3, xAOD::EgammaParameters::f3);

  // Get the number of primary vertices in this events
  // Should make this mu!
  double ip(0);
  if( mu < 0){
    ip = static_cast<double>(m_nPVdefault);
  } else {
    ip = mu;
  }
  ATH_MSG_VERBOSE ( Form("Vars: eta=%8.5f, et=%8.5f, f3=%8.5f, rHad==%8.5f, rHad1=%8.5f, Reta=%8.5f, w2=%8.5f, f1=%8.5f, Emaxs1=%8.5f, deltaEta=%8.5f, d0=%8.5f, rTRT=%8.5f, d0sigma=%8.5f, Rphi=%8.5f, ws3=%8.5f, dpOverp=%8.5f, deltaPhiRescaled2=%8.5f, TRT_PID=%8.5f, ip=%8.5f",
                         eta, et, f3, Rhad, Rhad1, Reta,
                         w2, f1, Eratio,
                         deltaEta, d0, rTRT,
                         d0sigma, 
                         Rphi, ws3, dpOverp, deltaPhiRescaled2,
			 TRT_PID,
                         ip ) );


  if (!allFound) {
    ATH_MSG_WARNING("Have some variables missing.");
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
                                rTRT,
                                d0sigma,
                                Rphi,
                                dpOverp,
                                deltaPhiRescaled2,
				TRT_PID,
                                ip
                                );
}

//=============================================================================
// Inline methods
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
  // Get the number of vertices
  unsigned int nVtx(0);
  const xAOD::VertexContainer* vxContainer(0);
  if ( StatusCode::SUCCESS != evtStore()->retrieve( vxContainer, m_primVtxContName ) )
    {
      ATH_MSG_ERROR ( "Vertex container not found with name:" << m_primVtxContName );
      return nVtx;
    }
  for ( unsigned int i=0; i<vxContainer->size(); i++ )
    {
      const xAOD::Vertex* vxcand = vxContainer->at(i);
      if ( vxcand->nTrackParticles() >= 2 ) nVtx++;
    }
  return nVtx;
}


//=============================================================================
/// Get the name of the current operating point
//=============================================================================
std::string AsgElectronLikelihoodTool::getOperatingPointName( const LikeEnum::Menu operating_point ) const
{
  if ( operating_point == LikeEnum::VeryLoose ){ return "VeryLoose"; }
  else if ( operating_point == LikeEnum::Loose ){ return "Loose"; }
  else if ( operating_point == LikeEnum::Medium ){ return "Medium"; }
  else if ( operating_point == LikeEnum::Tight ){ return "Tight"; }
  else if ( operating_point == LikeEnum::VeryTight ){ return "VeryTight"; }
  else if ( operating_point == LikeEnum::LooseRelaxed ){ return "LooseRelaxed"; }
  else if ( operating_point == LikeEnum::CustomOperatingPoint ){ return "CustomOperatingPoint"; }
  else
    {
      ATH_MSG_ERROR( "Didn't recognize the given operating point enum: " << operating_point );
      return "";
    }
}
