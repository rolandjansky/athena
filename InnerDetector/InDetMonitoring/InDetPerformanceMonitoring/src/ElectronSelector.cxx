//==================================================================================
//
//  ElectronSelector.cxx :       Class designed to reconstruct di-electrons events
//                        in particular Z0 -> e+ e- events.
//==================================================================================

//==================================================================================
// Include files...
//==================================================================================

// This files header
#include "InDetPerformanceMonitoring/ElectronSelector.h"
// Package Headers
#include "InDetPerformanceMonitoring/PerfMonServices.h"
#include <sstream>
// ATLAS headers
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Random/RandFlat.h"

#include "GaudiKernel/IToolSvc.h"

//#include "xAODMuon/Muon.h"
//#include "xAODMuon/MuonContainer.h"



// Local debug variables. Global scope, but only accessible by this file.
static const float CGeV              =  1.0e-3;  // Conversion factor to remove evil MeV
                                                 // nonsense.

// Static declarations
unsigned int ElectronSelector::s_uNumInstances;

//==================================================================================
// Public Methods
//==================================================================================
ElectronSelector::ElectronSelector():
  m_doDebug ( true )  
{
  ++s_uNumInstances;
  
  std::stringstream xTmp;  xTmp << s_uNumInstances;
  m_xSampleName     = "ElectronSelector_" + xTmp.str();
  
  m_pxElectron = nullptr;
  
  m_msgStream =  new MsgStream(PerfMonServices::getMessagingService(), "InDetPerformanceMonitoring" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ElectronSelector::~ElectronSelector()
{
  --s_uNumInstances;
  delete m_msgStream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ElectronSelector::Init()
{
  if (m_doDebug) std::cout << " --ElectronSelector::Init -- START -- " << std::endl;
  ISvcLocator* serviceLocator = Gaudi::svcLocator();
  IToolSvc* toolSvc;
  StatusCode sc = serviceLocator->service("ToolSvc", toolSvc, true);
  
  if ( sc.isFailure() || toolSvc == 0 ) {
    (*m_msgStream) << MSG::ERROR << "  * ElectronSelector::Init * Unable to retrieve ToolSvc " << endreq;
    return;
  }
  
  PARENT::Init();

  //---Electron Likelihood tool---
  // m_doIDCuts = true;
  (*m_msgStream) << MSG::INFO << "ElectronSelector::Init -- Setting up electron LH tool." << endreq;
  m_LHTool2015 = new AsgElectronLikelihoodTool ("m_LHTool2015");
  //  if((m_LHTool2015->setProperty("primaryVertexContainer",m_VxPrimContainerName)).isFailure())
  //  ATH_MSG_WARNING("Failure setting primary vertex container " << m_VxPrimContainerName << "in electron likelihood tool");

  if((m_LHTool2015->setProperty("WorkingPoint","MediumLHElectron")).isFailure())
    (*m_msgStream) << MSG::WARNING << "Failure loading ConfigFile for electron likelihood tool: MediumLHElectron " << endreq;

  StatusCode lhm = m_LHTool2015->initialize();
  if(lhm.isFailure())
    (*m_msgStream) << MSG::WARNING << "Electron likelihood tool initialize() failed!" << endreq;

  if (m_doDebug) std::cout << " --ElectronSelector::Init -- COMPLETED -- "<< endreq;
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ElectronSelector::PrepareElectronList(const xAOD::ElectronContainer* pxElecContainer)
{
  if (m_doDebug) std::cout << " --ElectronSelector::PrepareElectronList -- START  -- " << endreq;
  typedef xAOD::ElectronContainer::const_iterator electron_iterator;
  electron_iterator iter    = pxElecContainer->begin();
  electron_iterator iterEnd = pxElecContainer->end();
  
  // Loop over the Electrons                                                                                                                                                       
  int allelectroncount = 0;
  int goodelectroncount = 0;
  for(; iter != iterEnd ; iter++) {
    allelectroncount++;
    (*m_msgStream) << MSG::DEBUG  << " --ElectronSelector::PrepareElectronList -- candiate electron " << allelectroncount 
		   << " has author " << (*iter)->author(xAOD::EgammaParameters::AuthorElectron)
		   << endreq;
    const xAOD::Electron * ELE = (*iter);
    if ( RecordElectron(ELE) ) goodelectroncount++;
  }

  if (m_doDebug) std::cout << " -- ElectronSelector::PrepareElectronList -- m_pxElTrackList.size() = " << m_pxElTrackList.size() << endreq;
  if (m_doDebug) std::cout << " -- ElectronSelector::PrepareElectronList -- COMPLETED -- electroncount -- good / all = " 
			   << goodelectroncount << " / " << allelectroncount 
			   << endreq;

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ElectronSelector::RecordElectron (const xAOD::Electron * thisElec)
{
  if (m_doDebug) std::cout << " --ElectronSelector::RecordRlectron -- START  -- " << endreq;
  
  // start assuming electron candidate is good 
  bool electronisgood = true;

  //Get the track particle                                                                                                                                                        
  const xAOD::TrackParticle* theTrackParticle = thisElec->trackParticle();
  
  if (!theTrackParticle) {
    electronisgood = false;
    if (m_doDebug) std::cout << "   -- electron fails trackparticle  -- " << endreq;
  }

  if (electronisgood && thisElec->author(xAOD::EgammaParameters::AuthorElectron) != 1) {
    electronisgood = false;
    if (m_doDebug) std::cout << "   -- electron fails author  -- " << thisElec->author(xAOD::EgammaParameters::AuthorElectron) << endreq;
  }

  if (electronisgood) {
    m_pxElTrackList.push_back(theTrackParticle);
    if (m_doDebug) std::cout << "   time to store this electron :) " << endreq;
  }

  if (m_doDebug) std::cout << " -- ElectronSelector::RecordElectrons -- COMPLETED  -- " << endreq;

  return electronisgood;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////                                                                      
void ElectronSelector::OrderElectronList()
{
  if (m_doDebug) std::cout << " -- ElectronSelector::OrderElectronList -- START  -- list size: " << m_pxElTrackList.size( ) << endreq;
  if (m_pxElTrackList.size( )>= 2) { // we need at least 2 electrons
  }
  if (m_doDebug) std::cout << " -- ElectronSelector::OrderElectronList -- COMPLETED  -- " << endreq;
  return;
}
