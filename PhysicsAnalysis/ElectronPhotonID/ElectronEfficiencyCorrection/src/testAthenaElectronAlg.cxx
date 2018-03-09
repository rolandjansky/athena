/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronEfficiencyCorrection includes
#include "testAthenaElectronAlg.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "PATInterfaces/SystematicsTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"

testAthenaElectronAlg::testAthenaElectronAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),  
  m_electronSF("AsgElectronEfficiencyCorrectionTool/AsgElectronEfficiencyCorrectionTool",this){
  //declareProperty( "Property", m_nProperty ); //example property declaration
  declareProperty( "ElectronEfficiencyCorrectionTool", m_electronSF );
}
//
testAthenaElectronAlg::~testAthenaElectronAlg() {}


StatusCode testAthenaElectronAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  // Retrieve the tool
  ATH_CHECK( m_electronSF.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode testAthenaElectronAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  // Clean up
  ATH_CHECK( m_electronSF.release() );
  return StatusCode::SUCCESS;
}

StatusCode testAthenaElectronAlg::execute() {  
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
 
  //---------
  // electrons
  //---------
  const xAOD::ElectronContainer* electrons = 0;
  ATH_CHECK( evtStore()->retrieve( electrons, "Electrons") );
 
  // Let's create a shallow copy of the const electron container, and decorate it with the obtained SF
  auto inContShallowCopy = xAOD::shallowCopyContainer( *electrons );

  //creates a new electron container to hold the subset as well as the needed auxiliary container
  xAOD::ElectronContainer* myelectrons = new xAOD::ElectronContainer;
  xAOD::ElectronAuxContainer* myelectronsAux = new xAOD::ElectronAuxContainer;
  // You need to tell the jet container in which auxiliary container it should write its member variables
  myelectrons->setStore( myelectronsAux ); //gives it a new associated aux container
  // Also record to storegate: you must record both the container and the auxcontainer.
  // Note that storegate takes ownership of these objects, i.e., you must not try to delete them yourself.
  ATH_CHECK( evtStore()->record(myelectrons,  "MyElectrons" ) );
  ATH_CHECK( evtStore()->record(myelectronsAux, "MyElectronsAux" ) );

  // Loop over all Electrons in the shallow-copy container, decorate it with SF, and store in new xAOD file, in addition print out all SF (with get* function)

  CP::SystematicSet mySysSet;
  //mySysSet.insert(CP::SystematicVariation("EL_EFF_UncorrUncertainty", 1));
  m_electronSF->applySystematicVariation(mySysSet);

  for ( xAOD::Electron* el : *(inContShallowCopy.first) ) {

    if ( el->pt()<7000. ) continue;
    ATH_MSG_DEBUG( "  electron pt = " << el->pt() <<", electron eta = " << el->eta() );

    double SF; // for electron SF 
    if( m_electronSF->getEfficiencyScaleFactor(*el,SF) != CP::CorrectionCode::Ok){
      ATH_MSG_WARNING( "Couldn't get electron scale factor!" );
      continue;
    }

    if ( m_electronSF->applyEfficiencyScaleFactor(*el) == CP::CorrectionCode::Error ) {
      ATH_MSG_ERROR("ElectronEfficiencyCorrectionTool reported a CP::CorrectionCode::Error");
      return StatusCode::FAILURE;
    }

    //m_electronSF->applyEfficiencyScaleFactor(*el);
    ATH_MSG_DEBUG( "  electron SF, from get function = " << SF <<", electron SF from apply function " << el->auxdata< float >("SF") );
    
    myelectrons->push_back(el); // add it to a new electron container which will be writed to a new file
  }// and loop on electrons
 
  return StatusCode::SUCCESS;
}


