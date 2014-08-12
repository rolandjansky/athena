///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METRegionsTool.cxx 
// Implementation file for class METRegionsTool 
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <vector>

// METReconstruction includes
#include "METReconstruction/METRegionsTool.h"

// MET EDM
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::MissingET;
  using xAOD::MissingETContainer;
  using xAOD::MissingETComposition;
  using xAOD::MissingETComponent;
  using xAOD::MissingETComponentMap;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METRegionsTool::METRegionsTool(const std::string& name) :
    AsgTool(name),
    METRefinerTool(name)
  {
    declareProperty( "InputMETContainer" , m_base_met_containerKey = ""  );
    declareProperty( "InputMETMap"       , m_base_met_mapKey       = ""  );
    declareProperty( "InputMETKey"       , m_base_met_inputKey     = ""  ); 
    declareProperty( "CentralEtaMax"     , m_eta_centralMax        = 1.5 );   
    declareProperty( "EndCapEtaMax"      , m_eta_endcapMax         = 3.2 );   
    declareProperty( "ForwardEtaMax"     , m_eta_forwardMax        = 10. );   
  }

  // Destructor
  ///////////////
  METRegionsTool::~METRegionsTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METRegionsTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    if( m_base_met_containerKey.size()==0 || m_base_met_inputKey.size()==0 ) {
      ATH_MSG_FATAL("Both InputMETContainer or InputMETKey must be provided.");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METRegionsTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
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

  StatusCode METRegionsTool::executeTool(xAOD::MissingET* metTerm_central, xAOD::MissingETComponentMap* metMap) 
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    // First retrieve the BaseMET
    const MissingETContainer* base_met_container = 0;
    if( evtStore()->retrieve( base_met_container, m_base_met_containerKey ).isFailure() ) {
      ATH_MSG_WARNING("Could not retrieve base MET container!");
      return StatusCode::SUCCESS;
    }

    // First retrieve the BaseMET
    const MissingETComponentMap* base_met_map = 0;
    if( evtStore()->retrieve( base_met_map, m_base_met_mapKey ).isFailure() ) {
      ATH_MSG_WARNING("Could not retrieve base MET map!");
      return StatusCode::SUCCESS;
    }

    // Create additional MET objects to hold EndCap and Forward
    MissingET* metTerm_endcap  = new MissingET(0.,0.,0.);
    MissingET* metTerm_forward = new MissingET(0.,0.,0.);

    ATH_MSG_DEBUG("Check term pointers " << metTerm_central << "," << metTerm_endcap << "," << metTerm_forward);

    // Set Names
    metTerm_central->setName( m_base_met_inputKey + "_Central" );
    metTerm_endcap ->setName( m_base_met_inputKey + "_EndCap"  );
    metTerm_forward->setName( m_base_met_inputKey + "_Forward" );

    ATH_MSG_DEBUG("Set up region terms, " << metTerm_central->name() << ", " << metTerm_endcap->name() << ", " << metTerm_forward->name());

    // Add to the Container 
    MissingETContainer* metCont = dynamic_cast<MissingETContainer*>( metTerm_central->container() );
    metCont->push_back( metTerm_endcap  );
    metCont->push_back( metTerm_forward );

    // Add to the Composition
    MissingETComposition::add( metMap, metTerm_endcap  );
    MissingETComposition::add( metMap, metTerm_forward );

    // Get the components of the base MET
    MissingETContainer::const_iterator iterBaseCont = base_met_container->find( m_base_met_inputKey );
    MissingETComponentMap::const_iterator iterBaseConstit = MissingETComposition::find( base_met_map, (*iterBaseCont) );

    if( iterBaseCont == base_met_container->end() ) {
      ATH_MSG_WARNING("Could not find base MET object " << m_base_met_inputKey << " in MET container!");
      return StatusCode::SUCCESS;
    }

    if( iterBaseConstit == metMap->end() ) {
      ATH_MSG_WARNING("Could not find base METComponent in MET Map!");
      return StatusCode::SUCCESS;
    }

    if(!(*iterBaseConstit)->empty()) {
      // Loop over component and add to appropriate MET and insert to METComposition
      vector<const IParticle*> objectList = (*iterBaseConstit)->objects();
      vector<const IParticle*> dummyList;

      for( vector<const IParticle*>::const_iterator iObj = objectList.begin(); iObj!=objectList.end(); ++iObj ) {
	MissingETBase::Types::weight_t objWeight = (*iterBaseConstit)->weight(*iObj);
	// Central
	if     ( fabs((*iObj)->eta()) < m_eta_centralMax ) {
	  metTerm_central->add((*iObj)->pt()*cos((*iObj)->phi())*objWeight.wpx(),
			       (*iObj)->pt()*sin((*iObj)->phi())*objWeight.wpy(),
			       (*iObj)->pt()*objWeight.wet());
	  MissingETComposition::insert( metMap, metTerm_central, *iObj, dummyList, objWeight );
	}
	// EndCap 
	else if( fabs((*iObj)->eta()) < m_eta_endcapMax ) {
	  metTerm_endcap->add((*iObj)->pt()*cos((*iObj)->phi())*objWeight.wpx(),
			      (*iObj)->pt()*sin((*iObj)->phi())*objWeight.wpy(),
			      (*iObj)->pt()*objWeight.wet());
	  MissingETComposition::insert( metMap, metTerm_endcap , *iObj, dummyList, objWeight  );
	} 
	// Forward 
	else if( fabs((*iObj)->eta()) < m_eta_forwardMax ) {
	  metTerm_forward->add((*iObj)->pt()*cos((*iObj)->phi())*objWeight.wpx(),
			       (*iObj)->pt()*sin((*iObj)->phi())*objWeight.wpy(),
			       (*iObj)->pt()*objWeight.wet());
	  MissingETComposition::insert( metMap, metTerm_forward, *iObj, dummyList, objWeight );
	}
      }
    }

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}
