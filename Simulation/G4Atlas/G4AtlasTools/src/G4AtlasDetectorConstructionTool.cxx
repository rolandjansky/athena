/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files

// local
#include "G4AtlasTools/G4AtlasDetectorConstructionTool.h"

#include "G4AtlasInterfaces/IDetectorConstructionTool.h"

#include "G4VUserDetectorConstruction.hh"
#include "G4RunManager.hh"

//-----------------------------------------------------------------------------
// Implementation file for class : G4AtlasDetectorConstructionTool
//
// 2014-10-03: Andrea Dell'Acqua
//-----------------------------------------------------------------------------


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
G4AtlasDetectorConstructionTool::G4AtlasDetectorConstructionTool( const std::string& type,
                                                                  const std::string& nam,const IInterface* parent )
  : AthAlgTool( type, nam , parent )
  , m_world(nullptr)
{
  ATH_MSG_INFO("G4AtlasDetectorConstructionTool "<<type<<" "<<nam);
  declareInterface< IDetectorConstructionTool >( this ) ;
}

//=============================================================================
// Destructor
//=============================================================================

G4AtlasDetectorConstructionTool::~G4AtlasDetectorConstructionTool()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode G4AtlasDetectorConstructionTool::initialize( )
{
  ATH_MSG_INFO("this is G4AtlasDetectorConstructionTool::initialize( )!!!");
  return StatusCode::SUCCESS;
}

void G4AtlasDetectorConstructionTool::SetWorld(G4VPhysicalVolume *w)
{
  m_world=w;
}
G4VPhysicalVolume* G4AtlasDetectorConstructionTool::Construct()
{
  return m_world;
}

StatusCode G4AtlasDetectorConstructionTool::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if ( riid == IDetectorConstructionTool::interfaceID() ) {
    *ppvIf = (IDetectorConstructionTool*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return AlgTool::queryInterface( riid, ppvIf );
}

