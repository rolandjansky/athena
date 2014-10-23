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

#include "xAODEgamma/Electron.h"
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
  declareProperty("primaryVertexContainer", m_primVtxContName="PrimaryVertices", "The primary vertex container name" );


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

  //First try the PathResolver
  std::string filename = PathResolverFindCalibFile( m_pdfFileName );
  if (filename.empty()){
    ATH_MSG_WARNING ( "Could NOT resolve file name " << m_pdfFileName );
  }  else{
    ATH_MSG_INFO(" Path found = "<<filename);
  }

  const char* fname= filename.c_str();
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

//=============================================================================
// Accept method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
//=============================================================================
const Root::TAccept& AsgElectronLikelihoodTool::accept( const xAOD::Egamma* eg,
                                                        bool CaloCutsOnly) const
{
  if( !CaloCutsOnly )
    {
      return accept( (xAOD::IParticle*) eg);
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
  
  double et = cluster->e()/cosh(eta); 

  // Variables the EFCaloLH ignores
  uint8_t nSi(0);
  uint8_t nSiDeadSensors(0);
  uint8_t nPix(0);
  uint8_t nPixDeadSensors(0); 
  uint8_t expectBlayer(true);
  uint8_t nBlayerHits(0); 
  uint8_t nBlayerOutliers(0); 
  int convBit(0);

  // Should make this mu in the end
  double ip = static_cast<double>(m_nPVdefault);

  // for now don't cache. 
  double likelihood = calculate(eg, CaloCutsOnly); 

  ATH_MSG_VERBOSE ( Form("PassVars: LH=%8.5f, eta=%8.5f, et=%8.5f, nSi=%i, nSiDeadSensors=%i, nPix=%i, nPixDeadSensors=%i, nBlayerHits=%i, nBlayerOutliers=%i, expectBlayer=%i, convBit=%i, ip=%8.5f",
                         likelihood, eta, et,
                         nSi, nSiDeadSensors, nPix, nPixDeadSensors,
                         nBlayerHits, nBlayerOutliers, expectBlayer,
                         convBit, ip ) );


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
   




//=============================================================================
// The main result method: the actual likelihood is calculated here
//=============================================================================
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
  float deltaEta=0, deltaPhiRescaled2=0;

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
                         eta, et, f3, Rhad, Rhad1, Reta,
                         w2, f1, Eratio,
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
                                ip
                                );
}

//=============================================================================
// Calculate method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
//=============================================================================
const Root::TResult& AsgElectronLikelihoodTool::calculate( const xAOD::Egamma* eg, 
                                                           bool CaloCutsOnly) const
{
  if( !CaloCutsOnly )
    {
      return calculate( (xAOD::IParticle*) eg);
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
  double ip = static_cast<double>(m_nPVdefault);

  ATH_MSG_VERBOSE ( Form("Vars: eta=%8.5f, et=%8.5f, f3=%8.5f, rHad==%8.5f, rHad1=%8.5f, Reta=%8.5f, w2=%8.5f, f1=%8.5f, Emaxs1=%8.5f, deltaEta=%8.5f, d0=%8.5f, rTRT=%8.5f, d0sigma=%8.5f, Rphi=%8.5f, ws3=%8.5f, dpOverp=%8.5f, deltaPhiRescaled2=%8.5f, ip=%8.5f",
                         eta, et, f3, Rhad, Rhad1, Reta,
                         w2, f1, Eratio,
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
                                ip
                                );
}

//=============================================================================
// Inline methods
//=============================================================================
const Root::TAccept& AsgElectronLikelihoodTool::accept(const xAOD::IParticle* part) const
{
  ATH_MSG_DEBUG("Entering accept( const IParticle* part )");
  if(part->type()==xAOD::Type::Electron){
    const xAOD::Electron* eg =static_cast<const xAOD::Electron*> (part);
    return accept(eg);
  }
  else{
    ATH_MSG_ERROR("AsgElectronLikelihoodTool::could not convert argument to accept");
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
      ATH_MSG_ERROR ( " Could not cast to const egamma " );
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
