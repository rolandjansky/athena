///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AssocBuilder.cxx 
// Implementation file for class AssocBuilder
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "Gaudi/Property.h"

// CLHEP includes
#include "AthenaKernel/Units.h"

// FourMom includes
#include "FourMomUtils/P4Helpers.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomAssocs.h"

// AssociationKernel includes
#include "AssociationKernel/IAssocBuilderTool.h"

// AssociationComps includes
#include "AssocBuilder.h"

using Athena::Units::GeV;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AssocBuilder::AssocBuilder( const std::string& name, 
			    ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_assocTools( this )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  std::string descr;

  descr  = "List of input containers for which we'll try to find ";
  descr += "associations through each of the IAssocBuilderTools.";
  declareProperty( "Objects",
		   m_inputNames,
		   descr );
  std::vector<std::string> inputNames;
  inputNames.push_back( "ElectronCollection" );
  inputNames.push_back( "PhotonCollection" );
  inputNames.push_back( "MuidMuonCollection" );
  inputNames.push_back( "StacoMuonCollection" );
  inputNames.push_back( "Cone4TowerParticleJets" );
  inputNames.push_back( "ConeTowerParticleJets" );
  inputNames.push_back( "KtTowerParticleJets" );
  inputNames.push_back( "TauJetCollection" );
  m_inputNames.set(inputNames);
  m_inputNames.declareUpdateHandler( &AssocBuilder::setupInputContainers,
				     this );

  descr  = "List of input containers into which we'll try to find ";
  descr += "associations through each of the IAssocBuilderTools.";
  declareProperty( "Associations",
		   m_inputAssocNames,
		   descr );
  std::vector<std::string> inputAssocNames;
  inputAssocNames.push_back( "ElectronCollection" );
  inputAssocNames.push_back( "PhotonCollection" );
  inputAssocNames.push_back( "MuidMuonCollection" );
  inputAssocNames.push_back( "StacoMuonCollection" );
  inputAssocNames.push_back( "Cone4TowerParticleJets" );
  inputAssocNames.push_back( "ConeTowerParticleJets" );
  inputAssocNames.push_back( "KtTowerParticleJets" );
  inputAssocNames.push_back( "TauJetCollection" );
  m_inputAssocNames.set(inputAssocNames);
  m_inputAssocNames.declareUpdateHandler( &AssocBuilder::setupInputAssocContainers,
					  this );

  descr  = "Output location for the map of association (INav4Mom-INav4Mom) ";
  descr += "this algorithm is building.";
  declareProperty( "AssocsOutput",
		   m_assocsOutputName = "INav4MomAssocs",
		   descr );
  m_assocsOutputName.declareUpdateHandler( &AssocBuilder::setupAssocsOutput,
					   this );

  descr  = "List of IAssocBuilderTools' to be run by this algorithm.";
  declareProperty( "AssocTools",
		   m_assocTools,
		   descr );
  m_assocTools.push_back( "AssocBuilderTool/AssocTool" );
}

// Destructor
///////////////
AssocBuilder::~AssocBuilder()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode AssocBuilder::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  
  // setup properties
  setupInputContainers( m_inputNames );
  setupInputAssocContainers( m_inputAssocNames );
  setupAssocsOutput( m_assocsOutputName );

  // retrieve the association tools
  if ( !m_assocTools.retrieve().isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not retrieve Association tools !!" << endmsg
       << " [" << m_assocTools << "]");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode AssocBuilder::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode AssocBuilder::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  INav4MomAssocs * assocs = new INav4MomAssocs;
  if ( !evtStore()->record( assocs, m_assocsOutputName ).isSuccess() ) {
    ATH_MSG_WARNING
      ("Could not record the INav4MomAssocs container at ["
       << m_assocsOutputName << "] !!");
    delete assocs;
    assocs = 0;
    return StatusCode::RECOVERABLE;
  }

  if ( !evtStore()->setConst(assocs).isSuccess() ) {
    ATH_MSG_WARNING
      ("Could not set const INav4MomAssocs container at ["
       << m_assocsOutputName << "] !!");
  }

  if ( m_assocTools.empty() ) {
    ATH_MSG_WARNING ("No IAssocBuilderTool has been configured !");
    return StatusCode::RECOVERABLE;
  }

  bool allGood = true;

  for ( std::vector<std::string>::const_iterator 
	  objName = m_inputNames.value().begin(),
	  objEnd  = m_inputNames.value().end();
	objName != objEnd;
	++objName ) {
    const INavigable4MomentumCollection * objs = 0;
    if ( !evtStore()->retrieve( objs, *objName ).isSuccess() ||
	 0 == objs ) {
      ATH_MSG_WARNING
	("Could not retrieve INavigable4MomentumCollection at ["
	 << *objName << "] !!");
      allGood = false;
      continue;
    }

    for ( unsigned int objIdx = 0, objMax = objs->size(); 
	  objIdx != objMax; 
	  ++objIdx ) {
      ElementLink<INavigable4MomentumCollection> obj( *objs, objIdx );

      for ( std::vector<std::string>::const_iterator 
	      assName = m_inputAssocNames.value().begin(),
	      assEnd  = m_inputAssocNames.value().end();
	    assName != assEnd;
	    ++assName ) {
	
	const INavigable4MomentumCollection * assCont = 0;
	if ( !evtStore()->retrieve( assCont, *assName ).isSuccess() ||
	     0 == assCont ) {
	  ATH_MSG_WARNING
	    ("Could not retrieve INavigable4MomentumCollection at ["
	     << *assName << "] !!");
	  allGood = false;
	  continue;
	}

	for ( IAssocBuilderTools_t::const_iterator 
		assocTool    = m_assocTools.begin(),
		assocToolEnd = m_assocTools.end();
	      assocTool != assocToolEnd;
	      ++assocTool ) {
	  if ( !(*assocTool)->buildAssocs( obj, 
					   *assCont, *assocs ).isSuccess() ) {
	    ATH_MSG_WARNING ("Failed to build associations for an AlgTool !!");
	    allGood = false;
	  }
	} //> loop over association builder tools
      }//> loop over input container names (to be associated)
    }//> loop over input objects  
  } //> loop over input container names

  if ( msgLvl(MSG::DEBUG) ) {
    INav4MomAssocs::object_iterator endObj = assocs->endObject();
    msg(MSG::DEBUG) << " === INav4MomAssocs' content === " << endmsg;
    for ( INav4MomAssocs::object_iterator objItr = assocs->beginObject();
	  objItr != endObj;
	  ++objItr ) {
      const INavigable4Momentum* obj = objItr.getObject();
      msg() << "object: E= " << obj->e() / GeV
	    << "\tnAssocs= " << assocs->getNumberOfAssociations(objItr)
	    << endmsg;
      for ( INav4MomAssocs::asso_iterator assoItr = assocs->beginAssociation(objItr);
	    assoItr != assocs->endAssociation(objItr);
	    ++assoItr ) {
	const INavigable4Momentum * assoc = *assoItr;
	msg() << "\tE= " << assoc->e() / GeV
	      << "\tdR= " << P4Helpers::deltaR( obj, assoc )
	      << endmsg;
      }
    }
  }

  return allGood 
    ? StatusCode::SUCCESS
    : StatusCode::RECOVERABLE;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void AssocBuilder::setupInputContainers( Gaudi::Details::PropertyBase& /*inputName*/ )
{
  // nothing to do (yet ?)
  return;
}

void AssocBuilder::setupInputAssocContainers( Gaudi::Details::PropertyBase& /*inputAssocName*/ )
{
  // nothing to do (yet ?)
  return;
}

void AssocBuilder::setupAssocsOutput( Gaudi::Details::PropertyBase& /*assocsOutputName*/ )
{
  // nothing to do (yet ?)
  return;
}

