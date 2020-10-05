/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "TrackFastSimSDTool.h"

// Pacakge includes
#include "TrackWriteFastSim/TrackFastSimSD.h"

TrackFastSimSDTool::TrackFastSimSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
  m_outputCollectionNames.setValue({"NeutronBG"});
  m_noVolumes.setValue(true);
}

G4VSensitiveDetector* TrackFastSimSDTool::makeSD() const
{
  ATH_MSG_DEBUG( "Initializing SD" );

  // Create a fresh SD
  return new TrackFastSimSD(name(), m_outputCollectionNames.value()[0]);
}
