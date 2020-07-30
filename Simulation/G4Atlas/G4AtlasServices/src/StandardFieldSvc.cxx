/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StandardFieldSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "StoreGate/ReadCondHandle.h"

// Field map
#include "MagFieldElements/AtlasFieldMap.h"

// PathResolver
#include "PathResolver/PathResolver.h"

// ROOT
#include "TFile.h"
#include "TTree.h"


//-----------------------------------------------------------------------------
// Constructor for the StandardFieldSvc
//-----------------------------------------------------------------------------
StandardFieldSvc::StandardFieldSvc(const std::string& name,
                                   ISvcLocator* pSvcLocator)
    : G4MagFieldSvcBase(name, pSvcLocator)
{}


//-----------------------------------------------------------------------------
// Initialize the service
//-----------------------------------------------------------------------------
StatusCode StandardFieldSvc::initialize()
{
  ATH_MSG_INFO( "Initializing " << name() );

  // Either initialize the field map - used for solenoid and toroid, or the field service for the forward field
  if (m_useMagFieldSvc) {
      ATH_CHECK( m_magFieldSvc.retrieve() );
      ATH_MSG_INFO( "initialize: using field service  " << m_magFieldSvc.name() );
  }
  else {
      // Create field map
      ATH_CHECK( createFieldMap() );
      ATH_MSG_INFO( "initialize: using created map for field cache  ");
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Create and retrieve the magnetic field
//-----------------------------------------------------------------------------
G4MagneticField* StandardFieldSvc::makeField()
{
  ATH_MSG_INFO( "StandardFieldSvc::makeField" );

  AtlasField* af{nullptr};
  
  // Either initialize the field map - used for solenoid and toroid, or the field service for the forward field
  if (m_useMagFieldSvc) {
      af = new AtlasField( &*m_magFieldSvc );
  }
  else {
      af = new AtlasField(m_fieldMap.get());
  }
  
  return (af);
}


//-----------------------------------------------------------------------------
// Create field map
//-----------------------------------------------------------------------------
StatusCode StandardFieldSvc::createFieldMap()
{
  ATH_MSG_INFO( "StandardFieldSvc::createFieldMap" );


    // Select map file according to the value of the currents which indicate which map is 'on'

    // determine the map to load
    std::string mapFile;
    if ( solenoidOn() && toroidOn() ) mapFile = m_fullMapFilename;
    else if ( solenoidOn() )          mapFile = m_soleMapFilename;
    else if ( toroidOn() )            mapFile = m_toroMapFilename;
    else {
        // all magnets OFF. no need to read map
        return StatusCode::SUCCESS;
    }
        
    ATH_MSG_INFO ( "StandardFieldSvc::createFieldMap: Set map currents from FieldSvc: solenoid/toroid " 
                   << m_mapSoleCurrent << "," << m_mapToroCurrent);
    ATH_MSG_INFO ( "StandardFieldSvc::createFieldMap: Use map file " << mapFile);

        
    // find the path to the map file
    std::string resolvedMapFile = PathResolver::find_file( mapFile.c_str(), "DATAPATH" );
    if ( resolvedMapFile.empty() ) {
        ATH_MSG_ERROR( "StandardFieldSvc::createFieldMap: Field map file " << mapFile << " not found" );
        return StatusCode::FAILURE;
    }
    // Do checks and extract root file to initialize the map
    if ( resolvedMapFile.find(".root") == std::string::npos ) {
        ATH_MSG_ERROR("StandardFieldSvc::createFieldMap: input file name '" << resolvedMapFile << "' does not end with .root");
        return StatusCode::FAILURE;
    } 

    std::unique_ptr<TFile> rootfile = std::make_unique<TFile>(resolvedMapFile.c_str(), "OLD");
    if ( ! rootfile.get() ) {
        ATH_MSG_ERROR("StandardFieldSvc::createFieldMap: failed to open " << resolvedMapFile);
        return StatusCode::FAILURE;
    }
    if ( !rootfile->cd() ) {
        // could not make it current directory
        ATH_MSG_ERROR("StandardFieldSvc::createFieldMap: unable to cd() into the ROOT field map TFile");
        rootfile->Close();
        return StatusCode::FAILURE; 
    }
    // open the tree
    TTree* tree = (TTree*)rootfile->Get("BFieldMap");
    if ( tree == nullptr ) {
        // no tree
        ATH_MSG_ERROR("StandardFieldSvc::createFieldMap: TTree 'BFieldMap' does not exist in ROOT field map");
        rootfile->Close();
        return StatusCode::FAILURE;
    }

    // create map
    m_fieldMap = std::make_unique<MagField::AtlasFieldMap>();

    // initialize map
    if (!m_fieldMap->initializeMap( rootfile.get(), m_mapSoleCurrent, m_mapToroCurrent )) {
        // failed to initialize the map
        ATH_MSG_ERROR("StandardFieldSvc::createFieldMap: unable to initialize the map for AtlasFieldMap for file " << resolvedMapFile);
        rootfile->Close();
        return StatusCode::FAILURE;
    }
    
    rootfile->Close();

    ATH_MSG_INFO( "StandardFieldSvc::createFieldMap: Initialized the field map from " << resolvedMapFile );
  return StatusCode::SUCCESS;
}
