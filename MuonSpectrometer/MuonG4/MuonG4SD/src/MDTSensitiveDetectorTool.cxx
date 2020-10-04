/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTSensitiveDetectorTool.h"
#include "MDTSensitiveDetector.h"
#include <TString.h> // for Form

MDTSensitiveDetectorTool::MDTSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
}

G4VSensitiveDetector* MDTSensitiveDetectorTool::makeSD() const
{
  ATH_MSG_DEBUG( "Initializing SD" );
  StatusCode sc = m_idHelperSvc.retrieve();
  if (sc.isFailure()) throw std::runtime_error(Form("File: %s, Line: %d\nMDTSensitiveDetectorTool::makeSD() - could not retrieve MuonIdHelperSvc", __FILE__, __LINE__));
  return new MDTSensitiveDetector(name(), m_outputCollectionNames[0], m_idHelperSvc->mdtIdHelper().tubeMax());
}
