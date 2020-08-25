/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// WriteINav4MomAssocs.cxx 
// Implementation file for class WriteINav4MomAssocs
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "Gaudi/Property.h"

// NavFourMom includes
#include "NavFourMom/INav4MomAssocs.h"

// ElectronPhotonIDEvent includes
#include "egammaEvent/ElectronContainer.h"

// JetEvent includes
#include "JetEvent/JetCollection.h"

// AnalysisTest includes
#include "AnalysisTest/WriteINav4MomAssocs.h"
#include "AthenaKernel/Units.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
WriteINav4MomAssocs::WriteINav4MomAssocs( const std::string& name, 
					  ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
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
  ATH_MSG_DEBUG ( "Calling destructor" ) ;
}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode WriteINav4MomAssocs::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." ) ;
  return StatusCode::SUCCESS;
}

StatusCode WriteINav4MomAssocs::finalize()
{
  ATH_MSG_INFO ( "Finalizing " << name() << "..." ) ;
  return StatusCode::SUCCESS;
}

StatusCode WriteINav4MomAssocs::execute()
{  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." ) ;

  ATH_CHECK( (this->symLink<ElectronContainer,INavigable4MomentumCollection>(m_electronsName)) );
  
  ATH_CHECK( (this->symLink<JetCollection,INavigable4MomentumCollection>(m_jetsName)) );

  const INavigable4MomentumCollection * jets = 0;
  ATH_CHECK( evtStore()->retrieve( jets, m_jetsName ) );

  const INavigable4MomentumCollection * electrons = 0;
  ATH_CHECK( evtStore()->retrieve( electrons, m_electronsName ) );

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
  ATH_MSG_DEBUG ( "in buildINav4MomAssocs..." ) ;

  INav4MomAssocs * assocs = new INav4MomAssocs;
  ATH_CHECK( evtStore()->record( assocs, m_inavAssocsOutputName ) );
  ATH_CHECK( evtStore()->setConst( assocs ) );

  for (const INavigable4Momentum* m1 : *coll1) {
    for (const INavigable4Momentum* m2 : *coll2) {
      assocs->addAssociation( coll1, m1, coll2, m2 );
      ATH_MSG_DEBUG ( "Assoc: [jet-ele] ene= " 
                      << m1->e() / Athena::Units::GeV
                      << "\t"
                      << m2->e() / Athena::Units::GeV ) ;
    }//> end loop over electrons
  }//> end loop over jets

  INav4MomAssocs::object_iterator objEnd = assocs->endObject();
  for ( INav4MomAssocs::object_iterator objItr = assocs->beginObject();
	objItr != objEnd;
	++objItr ) {
    ATH_MSG_INFO ( "--> e= " << (*objItr)->e() / Athena::Units::GeV
                   << "\tnAssocs= " << assocs->getNumberOfAssociations(objItr) ) ;
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

