/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/ElectronContainer.h"
#include "StoreGate/StoreGateSvc.h"

#include "PhotonAnalysisUtils/ElectronToPhotonAlg.h"
#include "PhotonAnalysisUtils/IElectronToPhotonTool.h"

#include "D3PDMakerUtils/TypeNameConversions.h"

#include "AthenaKernel/errorcheck.h"


ElectronToPhotonAlg::ElectronToPhotonAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator), 
    m_ElectronToPhotonTool( "ElectronToPhotonTool" ),
    m_resolver( 0 )
{ 
  declareProperty( "ElectronToPhotonTool", m_ElectronToPhotonTool, "Tool to convert electrons into photons" );
  declareProperty( "ElectronContainer"   , m_electronContainerName = "ElectronAODCollection");
}


ElectronToPhotonAlg::~ElectronToPhotonAlg() {}


StatusCode ElectronToPhotonAlg::initialize() {
  ServiceHandle<IClassIDSvc> clidsvc ("ClassIDSvc", this->name());
  m_resolver = new D3PD::SGKeyResolver(this->name(), evtStore(), m_electronContainerName);
  CLID clid;
  CHECK( D3PD::nameToCLID ("ElectronContainer", clid, this->name(), clidsvc) );
  CHECK( m_resolver->initialize (clid, "ElectronContainer") );
  CHECK( m_ElectronToPhotonTool.retrieve() );

  return StatusCode::SUCCESS;
}		 


StatusCode ElectronToPhotonAlg::finalize() {
  delete m_resolver;
  return StatusCode::SUCCESS;
}


StatusCode ElectronToPhotonAlg::execute() {
  MsgStream log( messageService(), name() );
  log << MSG::VERBOSE << "in execute()" << endreq; 

  PhotonContainer* ElectronToConvertedPhotonContainer   = new PhotonContainer;
  CHECK( evtStore()->record(ElectronToConvertedPhotonContainer, 
			    "PAUElectronToConvertedPhotonCollection"));

  PhotonContainer* ElectronToUnconvertedPhotonContainer = new PhotonContainer;
  CHECK( evtStore()->record(ElectronToUnconvertedPhotonContainer, 
			    "PAUElectronToUnconvertedPhotonCollection"));

  std::string egDetailContainerName="";

  ElectronContainer* electrons;
  CHECK( evtStore()->retrieve( electrons, m_resolver->key()) );
  ElectronContainer::const_iterator electronItr  = electrons->begin();
  ElectronContainer::const_iterator electronItrE = electrons->end();
  for ( ; electronItr != electronItrE ; ++electronItr ) {
    const Analysis::Electron* electron = *electronItr ;
    Analysis::Photon* converted   = m_ElectronToPhotonTool->ConvertElectronToPhoton(electron, true , egDetailContainerName);
    if(converted)      ElectronToConvertedPhotonContainer->push_back(converted);
      
    Analysis::Photon* unconverted = m_ElectronToPhotonTool->ConvertElectronToPhoton(electron, false, egDetailContainerName);
    if(unconverted)    ElectronToUnconvertedPhotonContainer->push_back(unconverted);
  }

  return StatusCode::SUCCESS ;
}

