/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCSensitiveDetectorTool.h"
#include "RPCSensitiveDetector.h"
#include <TString.h> // for Form

RPCSensitiveDetectorTool::RPCSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
}

G4VSensitiveDetector* RPCSensitiveDetectorTool::makeSD() const
{
  ATH_MSG_DEBUG( "Initializing SD" );
  StatusCode sc = m_idHelperSvc.retrieve();
  if (sc.isFailure()) throw std::runtime_error(Form("File: %s, Line: %d\nRPCSensitiveDetectorCosmicsTool::makeSD() - could not retrieve MuonIdHelperSvc", __FILE__, __LINE__));
  return new RPCSensitiveDetector(name(), m_outputCollectionNames[0], m_idHelperSvc->rpcIdHelper().gasGapMax());
}
