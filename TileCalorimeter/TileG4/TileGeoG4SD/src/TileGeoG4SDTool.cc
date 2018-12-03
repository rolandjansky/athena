/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class TileGeoG4SDTool
// AthTool class for holding the Tile sensitive detector
//
//************************************************************

#include "TileGeoG4SDTool.h"
#include "TileGeoG4SD.hh"
#include "TileG4Interfaces/ITileCalculator.h"

TileGeoG4SDTool::TileGeoG4SDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase(type,name,parent)
  , m_tileCalculator("TileGeoG4SDCalc", name)
{
  declareProperty( "TileCalculator", m_tileCalculator);
  declareProperty( "DeltaTHit" , m_options.deltaTHit );
  declareProperty( "TimeCut" , m_options.timeCut );
  declareProperty( "TileTB" , m_options.tileTB );
  declareProperty( "PlateToCell" , m_options.plateToCell );
  declareProperty( "Ushape" , m_options.Ushape );
  declareProperty( "DoBirk" , m_options.doBirk );
  declareProperty( "DoTileRow" , m_options.doTileRow );
  declareProperty( "DoTOFCorrection" , m_options.doTOFCorrection );
  declareProperty( "DoCalibHitParticleID" , m_options.doCalibHitParticleID );

}

StatusCode TileGeoG4SDTool::initialize()
{
  ATH_CHECK(m_tileCalculator.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode TileGeoG4SDTool::Gather()
{
  ATH_MSG_VERBOSE( "TileGeoG4SDTool::Gather()" );
  if(!getSD()) {
    ATH_MSG_ERROR ("Gather: TileGeoG4SD never created!");
    return StatusCode::FAILURE;
  } else {
    TileGeoG4SD *localSD = dynamic_cast<TileGeoG4SD*>(getSD());
    if(!localSD){
      ATH_MSG_ERROR ("Gather: Failed to cast m_SD into TileGeoG4SD.");
      return StatusCode::FAILURE;
    }
    localSD->EndOfAthenaEvent();
  }
  return StatusCode::SUCCESS;
}


G4VSensitiveDetector* TileGeoG4SDTool::makeSD()
{
  // Make sure the job has been set up properly
  ATH_MSG_DEBUG( "Initializing SD" );

  // Create a fresh SD
  if(msgLvl(MSG::VERBOSE))    { m_options.verboseLevel = 10; }
  else if(msgLvl(MSG::DEBUG)) { m_options.verboseLevel = 5;  }
  return new TileGeoG4SD(name(), m_outputCollectionNames[0], &*m_tileCalculator, m_options);
}

