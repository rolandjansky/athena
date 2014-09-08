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

// STL includes
#include <string>
#include <cstdint>
#include <cmath>

#include "TSystem.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODCaloEvent/CaloCluster.h"


#include "PathResolver/PathResolver.h"


//=============================================================================
// Standard constructor
//=============================================================================
AsgElectronLikelihoodTool::AsgElectronLikelihoodTool(std::string myname) :
  AsgTool(myname),
  m_rootTool(0)
{

  // Create an instance of the underlying ROOT tool
  m_rootTool = new Root::TElectronLikelihoodTool( ("T"+myname).c_str() );

  // Declare the needed properties
  declareProperty("usePVContainer", m_usePVCont=true, "Whether to use the PV container");
  declareProperty("nPVdefault", m_nPVdefault = 11, "The default number of PVs if not counted");
  declareProperty("primaryVertexContainer", m_primVtxContName="VxPrimaryCandidate", "The primary vertex container name" );

  // declareProperty("forceRecalculateLikelihood", m_forceCalcLH=false,
  //                 "Say if we should re-calculate the likelihood every time (otherwise tried to be taken from UserData)" );


  //
  // Configurables in the root tool
  //
  // pdf file name. Managed in the Asg tool.
  declareProperty("inputPDFFileName",  m_pdfFileName="ElectronPhotonSelectorTools/ElectronLikelihoodPdfs.root", "The input ROOT file name that holds the PDFs" );
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
  // cut on precision hits
  declareProperty("CutSi",m_rootTool->CutSi,"Cut on precision hits");
  // turn off f3 at high Et
  declareProperty("doRemoveF3AtHighEt",m_rootTool->doRemoveF3AtHighEt,"Turn off f3 at high Et");
}


//=============================================================================
// Standard destructor
//=============================================================================
AsgElectronLikelihoodTool::~AsgElectronLikelihoodTool()
{
  if(finalize().isFailure()){
    ATH_MSG_ERROR ( "Failure in AsgElectronLikelihoodTool finalize()");
  }
  if ( m_rootTool ) delete m_rootTool;
}


//=============================================================================
// Asgena initialize method
//=============================================================================
StatusCode AsgElectronLikelihoodTool::initialize()
{
  // Add an input file that holds the PDFs
  // For that, first, expand the given file name such that it becomes an absolute path
  const char* fname=0;
  if ( m_pdfFileName.find("/")==0 || m_pdfFileName.find("$")==0 || m_pdfFileName.find(".")==0 || m_pdfFileName.find(":")!=std::string::npos )
    {
      fname = gSystem->ExpandPathName( m_pdfFileName.c_str() );
    }  
  else
    {
      m_pdfFileName = PathResolverFindXMLFile( m_pdfFileName );
      if ( m_pdfFileName.empty() )
        {
          ATH_MSG_WARNING ( "Could NOT resolve file name " << m_pdfFileName );
        }
      fname = m_pdfFileName.c_str();
    }

  
  m_rootTool->setPDFFileName ( fname );

  // Get the name of the current operating point, and massage the other strings accordingly
  ATH_MSG_VERBOSE( "Going to massage the labels based on the provided operating point..." );

  // Get the message level and set the underlying ROOT tool message level accordingly
  bool debug(false);
  if ( this->msgLvl(MSG::VERBOSE) ) debug = true;
  m_rootTool->setDebug(debug);

  // Set the cut value to be used
  m_rootTool->setOperatingPoint( (LikeEnum::Menu)m_operatingPoint );
  
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
const Root::TAccept& AsgElectronLikelihoodTool::accept( const xAOD::Electron* eg ) const
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
  
  double et = cluster->e()/cosh(eta); 
  

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

  bool allFound = true;

  // retrieve associated track
  const xAOD::TrackParticle* t  = eg->trackParticle();    
  if (t)
    {
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

  // Get the conversion bit
  int convBit(0); // this no longer works
  // //convBit = eg->isem(0x1 << egammaPID::ConversionMatch_Electron) == 0;
  // unsigned int isem = (unsigned int)eg->egammaID (egammaPID::IsEM);
  // convBit = isem & (0x1 << egammaPID::ConversionMatch_Electron);

  // Get the number of primary vertices in this events
  double ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);


  // for now don't cache. 
  double likelihood = calculate(eg); 

  ATH_MSG_VERBOSE ( Form("PassVars: LH=%8.5f, eta=%8.5f, et=%8.5f, nSi=%i, nSiDeadSensors=%i, nPix=%i, nPixDeadSensors=%i, nBlayerHits=%i, nBlayerOutliers=%i, expectBlayer=%i, convBit=%i, ip=%8.5f",
                         likelihood, eta, et,
                         nSi, nSiDeadSensors, nPix, nPixDeadSensors,
                         nBlayerHits, nBlayerOutliers, expectBlayer,
                         convBit, ip ) );


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
                             ip
                             );
}

   




// The main result method: the actual likelihood is calculated here
const Root::TResult& AsgElectronLikelihoodTool::calculate( const xAOD::Electron* eg ) const
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

  bool allFound = true;

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


  float e233, e237, e277, ethad1, ethad, ws3, w2, f1, emax2, emax, f3;

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
  allFound = allFound && eg->showerShapeValue(ws3, xAOD::EgammaParameters::weta1);
  // shower width in 2nd sampling
  allFound = allFound && eg->showerShapeValue(w2, xAOD::EgammaParameters::weta2);
  // fraction of energy reconstructed in the 1st sampling
  allFound = allFound && eg->showerShapeValue(f1, xAOD::EgammaParameters::f1);
  // E of 2nd max between max and min in strips
  allFound = allFound && eg->showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1);
  // E of 1st max in strips
  allFound = allFound && eg->showerShapeValue(emax, xAOD::EgammaParameters::emaxs1);
  // fraction of energy reconstructed in the 3rd sampling
  allFound = allFound && eg->showerShapeValue(f3, xAOD::EgammaParameters::f3);

  double rHad1  = et != 0. ? ethad1/et : 0.;
  double rHad   = et != 0. ? ethad/et : 0.;
  
  float Reta   = e277 != 0 ? e237/e277 : 0.;
  float Rphi   = e237 != 0. ? e233 / e237 : 0.;
  
  float DEmaxs1 = (emax+emax2)>0. ? (emax-emax2)/(emax+emax2) : 0.0;
  

  // Delta eta,phi matching
  float deltaEta, deltaPhiRescaled2;

  allFound = allFound && eg->trackCaloMatchValue(deltaEta, xAOD::EgammaParameters::deltaEta1);

  // difference between the cluster phi (sampling 2) and the eta of the track extrapolated from the last measurement point.
  allFound = allFound && eg->trackCaloMatchValue(deltaPhiRescaled2, xAOD::EgammaParameters::deltaPhiRescaled2);
  
  // Get the number of primary vertices in this events
  double ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);

  // TRT high-to-low threshold hits ratio
  double rTRT(0.0);
  if ( nTRTXenon > 0 )
    {
      rTRT = ((double)(nTRThigh+nTRThighOutliers)) / ((double)(nTRTXenon)) ;
    }
  // else if ( nTRTXenon < 0 && (nTRT+nTRTOutliers) > 0 ) // this is always false as given
  //   {
  //     rTRT = ((double)(nTRThigh+nTRThighOutliers)) / ((double)(nTRT+nTRTOutliers)) ;
  //   }
  

  ATH_MSG_VERBOSE ( Form("Vars: eta=%8.5f, et=%8.5f, f3=%8.5f, rHad==%8.5f, rHad1=%8.5f, Reta=%8.5f, w2=%8.5f, f1=%8.5f, Emaxs1=%8.5f, deltaEta=%8.5f, d0=%8.5f, rTRT=%8.5f, d0sigma=%8.5f, Rphi=%8.5f, ws3=%8.5f, dpOverp=%8.5f, deltaPhiRescaled2=%8.5f, ip=%8.5f",
                         eta, et, f3, rHad, rHad1, Reta,
                         w2, f1, DEmaxs1,
                         deltaEta, d0, rTRT,
                         d0sigma, 
                         Rphi, ws3, dpOverp, deltaPhiRescaled2,
                         ip ) );


  if (!allFound) {
    ATH_MSG_WARNING("Have some variables missing.");
  }

  // Get the answer from the underlying ROOT tool
  return m_rootTool->calculate( eta,
                                et,
                                f3,
                                rHad,
                                rHad1,
                                Reta,
                                w2,
                                f1,
                                DEmaxs1,
                                deltaEta,
                                d0, //??? el_trackd0pv
                                rTRT,
                                d0sigma, //??? el_tracksigd0pv
                                Rphi,
                                dpOverp,
                                deltaPhiRescaled2,
                                ip
                                );
}





// Helper method to get the number of primary vertices
// ( This is horrible! We don't want to iterate over all vertices in the event for each electron!!! 
//   This is slow!)
unsigned int AsgElectronLikelihoodTool::getNPrimVertices() const
{
  // Get the number of vertices
  unsigned int nVtx(0);
  const xAOD::VertexContainer* vxContainer(NULL);
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


/// Get the name of the current operating point
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
