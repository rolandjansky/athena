/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// WriteINav4MomAssocs.cxx 
// Implementation file for class WriteINav4MomAssocs
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// NavFourMom includes
#include "NavFourMom/INav4MomAssocs.h"

// ElectronPhotonIDEvent includes
#include "egammaEvent/ElectronContainer.h"

// JetEvent includes
#include "JetEvent/JetCollection.h"

// AnalysisTest includes
#include "AnalysisTest/WriteINav4MomAssocs.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
WriteINav4MomAssocs::WriteINav4MomAssocs( const std::string& name, 
					  ISvcLocator* pSvcLocator ) : 
  Algorithm( name, pSvcLocator ),
  m_storeGate  ( 0 )
{
  //
  // Property declaration
  // 
  declareProperty( "Jets",         m_jetsName            = "AtlfastJetContainer" );
  declareProperty( "Electrons",    m_electronsName       = "AtlfastElectronCollection" );
  declareProperty( "AssocsOutput", m_inavAssocsOutputName = "INav4MomAssocs" );

}

/// Destructor
///////////////
WriteINav4MomAssocs::~WriteINav4MomAssocs()
{ 
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "Calling destructor" << endreq;
}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode WriteINav4MomAssocs::initialize()
{
  StatusCode sc;
  MsgStream log( msgSvc(), name() );

  log << MSG::INFO 
      << "Initializing " << name() << "..." 
      << endreq;

  // Get pointer to StoreGateSvc and cache it :
  sc = service( "StoreGateSvc", m_storeGate );
  if ( sc.isFailure() ) {
    log << MSG::ERROR 	
	<< "Unable to retrieve pointer to StoreGateSvc"
	<< endreq;
    return sc;
  }
  
  return sc;
}

StatusCode WriteINav4MomAssocs::finalize()
{
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO 
      << "Finalizing " << name() << "..." 
      << endreq;

  return StatusCode::SUCCESS;
}

StatusCode WriteINav4MomAssocs::execute()
{  
  MsgStream log( msgSvc(), name() );

  log << MSG::DEBUG << "Executing " << name() << "..." 
      << endreq;

  if ( this->symLink<ElectronContainer,INavigable4MomentumCollection>(m_electronsName).isFailure() ) {
    log << MSG::ERROR
	<< "Could not symLink " << m_electronsName << " to INav4Moms !"
	<< endreq;
    return StatusCode::FAILURE;
  }

  if ( this->symLink<JetCollection,INavigable4MomentumCollection>(m_jetsName).isFailure() ) {
    log << MSG::ERROR
	<< "Could not symLink " << m_jetsName << " to INav4Moms !"
	<< endreq;
    return StatusCode::FAILURE;
  }

  const INavigable4MomentumCollection * jets = 0;
  if ( m_storeGate->retrieve( jets, m_jetsName ).isFailure() ||
       0 == jets ) {

    log << MSG::ERROR
	<< "Could not retrieve INavigable4MomentumCollection at : "
	<< m_jetsName
	<< endreq;
    return StatusCode::FAILURE;
  }

  const INavigable4MomentumCollection * electrons = 0;
  if ( m_storeGate->retrieve( electrons, m_electronsName ).isFailure() ||
       0 == electrons ) {

    log << MSG::ERROR
	<< "Could not retrieve INavigable4MomentumCollection at : "
	<< m_electronsName
	<< endreq;
    return StatusCode::FAILURE;
  }

  return buildAssocs( jets, electrons );
}
/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

StatusCode 
WriteINav4MomAssocs::buildAssocs( const INavigable4MomentumCollection * coll1,
				  const INavigable4MomentumCollection * coll2 ) const
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "in buildINav4MomAssocs..." << endreq;

  INav4MomAssocs * assocs = new INav4MomAssocs;
  if ( m_storeGate->record( assocs, m_inavAssocsOutputName ).isFailure() ) {
    log << MSG::ERROR
	<< "Could not record INav4MomAssocs at : " << m_inavAssocsOutputName
	<< endreq;
    return StatusCode::SUCCESS;
  }
  if ( m_storeGate->setConst( assocs ).isFailure() ) {
    log << MSG::ERROR
	<< "Could not setConst INav4MomAssocs at : " << m_inavAssocsOutputName
	<< endreq;
    return StatusCode::SUCCESS;
  }

  INavigable4MomentumCollection::const_iterator end1 = coll1->end();
  INavigable4MomentumCollection::const_iterator end2 = coll2->end();

  for ( INavigable4MomentumCollection::const_iterator itr1 = coll1->begin();
	itr1 != end1;
	++itr1 ) {
    for ( INavigable4MomentumCollection::const_iterator itr2 = coll2->begin();
	  itr2 != end2;
	  ++itr2 ) {
      assocs->addAssociation( coll1, *itr1, coll2, *itr2 );
      if ( log.level() <= MSG::DEBUG ) {
	log << MSG::DEBUG
	    << "Assoc: [jet-ele] ene= " 
	    << (*itr1)->e() / CLHEP::GeV
	    << "\t"
	    << (*itr2)->e() / CLHEP::GeV
	    << endreq;
      }
    }//> end loop over electrons
  }//> end loop over jets

  log << MSG::DEBUG << "Print content of association map: [INav4MomAssocs]" 
      << endreq;
  INav4MomAssocs::object_iterator objEnd = assocs->endObject();
  for ( INav4MomAssocs::object_iterator objItr = assocs->beginObject();
	objItr != objEnd;
	++objItr ) {
    log << MSG::INFO
	<< "--> e= " << (*objItr)->e() / CLHEP::GeV
	<< "\tnAssocs= " << assocs->getNumberOfAssociations(objItr)
	<< endreq;
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

