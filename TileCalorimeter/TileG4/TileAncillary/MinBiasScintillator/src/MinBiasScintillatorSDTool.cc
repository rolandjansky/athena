/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class MinBiasScintillatorSDTool
// Tool for configuring the Sensitive detector for the Minimum Bias Scintillator
//
//************************************************************

#include "MinBiasScintillatorSDTool.h"
#include "MinBiasScintillatorSD.h"

MinBiasScintillatorSDTool::MinBiasScintillatorSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : SensitiveDetectorBase( type , name , parent )
{
  declareProperty( "DeltaTHit" , m_options.deltaTHit );
  declareProperty( "TimeCut" , m_options.timeCut );
  declareProperty( "TileTB" , m_options.tileTB );
  declareProperty( "DoBirk" , m_options.doBirk );
  declareProperty( "Birk1", m_options.birk1 );
  declareProperty( "Birk2", m_options.birk2 );
  declareProperty( "DoTOFCorrection" , m_options.doTOFCorrection );

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* MinBiasScintillatorSDTool::makeSD()
{
  ATH_MSG_VERBOSE( "Creating a copy of the MinBiasScintillatorSD!" );

  return new MinBiasScintillatorSD(name(), m_outputCollectionNames[0], m_options);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode MinBiasScintillatorSDTool::Gather()
{
  ATH_MSG_VERBOSE( "Gather()" );
  if(!getSD()) {
    ATH_MSG_ERROR ("Gather: MinBiasScintillatorSD never created!");
    return StatusCode::FAILURE;
  } else {
    MinBiasScintillatorSD *localSD = dynamic_cast<MinBiasScintillatorSD*>(getSD());
    if(!localSD){
      ATH_MSG_ERROR ("Gather: Failed to cast m_SD into MinBiasScintillatorSD.");
      return StatusCode::FAILURE;
    }
    localSD->EndOfAthenaEvent();
  }
  return StatusCode::SUCCESS;
}

