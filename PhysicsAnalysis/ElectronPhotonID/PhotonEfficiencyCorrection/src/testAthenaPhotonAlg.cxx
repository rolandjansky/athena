/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonEfficiencyCorrection includes
#include "testAthenaPhotonAlg.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODCore/ShallowCopy.h"

#include "PhotonEfficiencyCorrection/IAsgPhotonEfficiencyCorrectionTool.h"

testAthenaPhotonAlg::testAthenaPhotonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ),  m_photonSF("AsgPhotonEfficiencyCorrectionTool/AsgPhotonEfficiencyCorrectionTool",this){

  //declareProperty( "Property", m_nProperty ); //example property declaration
  declareProperty( "PhotonEfficiencyCorrectionTool", m_photonSF );
}
//
testAthenaPhotonAlg::~testAthenaPhotonAlg() {}


StatusCode testAthenaPhotonAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  // Retrieve the tool
  ATH_CHECK( m_photonSF.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode testAthenaPhotonAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  // Clean up
  ATH_CHECK( m_photonSF.release() );
  return StatusCode::SUCCESS;
}

StatusCode testAthenaPhotonAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

//----------------------------
 // Event information
 //--------------------------- 
 const xAOD::EventInfo* eventInfo = 0; //NOTE: Everything that comes from the storegate direct from the input files is const!

 // ask the event store to retrieve the xAOD EventInfo container
 //ATH_CHECK( evtStore()->retrieve( eventInfo, "EventInfo") );  // the second argument ("EventInfo") is the key name
 ATH_CHECK( evtStore()->retrieve( eventInfo) );
 // if there is only one container of that type in the xAOD (as with the EventInfo container), you do not need to pass
 // the key name, the default will be taken as the only key name in the xAOD 
 
 // check if data or MC
 bool isMC = true;
 if(!eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION ) ){
   isMC = false;
 }
 if(!isMC) ATH_MSG_ERROR("This is data, no scale factors should be used on the data!");
 
 //---------
 // photons
 //---------
 const xAOD::PhotonContainer* photons = 0;
 ATH_CHECK( evtStore()->retrieve( photons, "Photons") );
 ATH_MSG_DEBUG("Found "<<photons->size() <<" photons in event, itterate....");
 
// Let's create a shallow copy of the const photon container, and decorate it with the obtained SF
auto inContShallowCopy = xAOD::shallowCopyContainer( *photons );

//creates a new photon container to hold the subset as well as the needed auxiliary container
  xAOD::PhotonContainer* myphotons = new xAOD::PhotonContainer;
  xAOD::PhotonAuxContainer* myphotonsAux = new xAOD::PhotonAuxContainer;
  // You need to tell the photon container in which auxiliary container it should write its member variables
  myphotons->setStore( myphotonsAux ); //gives it a new associated aux container

  // Also record to storegate: you must record both the container and the auxcontainer.
  // Note that storegate takes ownership of these objects, i.e., you must not try to delete them yourself.
  ATH_CHECK( evtStore()->record(myphotons,  "MyPhotons" ) );
  ATH_CHECK( evtStore()->record(myphotonsAux, "MyPhotonsAux" ) );

// Loop over all Photons in the shallow-copy container, decorate it with SF, and store in new xAOD file, in addition print out all SF (with get* function)
for ( xAOD::Photon* ph : *(inContShallowCopy.first) ) {

  //if ( std::abs(ph->eta()) > 2.37 || ph->pt()<15000. ) continue;
  //if (std::abs(ph->eta())>=1.37 && std::abs(ph->eta())<=1.52) continue;
  ATH_MSG_DEBUG( "  photon pt = " << ph->pt() <<", photon eta = " << ph->eta() );

  double SF, SFerr; // for photon SF and the apropriate error
  if(!m_photonSF->getEfficiencyScaleFactor(*ph,SF)){
    ATH_MSG_WARNING( "Couldn't get photon scale factor!" );
	continue;
  }
  if(!m_photonSF->getEfficiencyScaleFactorError(*ph,SFerr)){
    ATH_MSG_WARNING( "Couldn't get photon scale factor uncertainty!" );
	continue;
  }
  ATH_MSG_DEBUG( "  photon SF = " << SF <<", photon SF sys error = " << SFerr );
  
  // applyEfficiencyScaleFactor - decorate the object (*photon)
  if ( m_photonSF->applyEfficiencyScaleFactor(*ph) == CP::CorrectionCode::Error ) {
    ATH_MSG_ERROR("PhotonEfficiencyCorrectionTool reported a CP::CorrectionCode::Error");
    return StatusCode::FAILURE;
  }

  myphotons->push_back(ph); // add it to a new photon container which will be writed to a new file
} // and loop on photons

  return StatusCode::SUCCESS;
}


