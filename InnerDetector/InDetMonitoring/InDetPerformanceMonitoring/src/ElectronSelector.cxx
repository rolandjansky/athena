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
  m_debug ( false )  
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
  std::cout << " --ElectronSelector::Init -- START -- " << std::endl;
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

  std::cout << " --ElectronSelector::Init -- COMPLETED -- "<< std::endl;
  return;
}


