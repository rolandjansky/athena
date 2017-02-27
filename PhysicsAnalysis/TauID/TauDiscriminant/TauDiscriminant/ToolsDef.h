/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef THOR_MXAODCREATOR_H

#include "TauDiscriminant/TauJetBDT.h"
#include "TauDiscriminant/TauEleBDT.h"
#include "TauDiscriminant/TauScoreFlatteningTool.h"
#include "TauDiscriminant/TauMuonVeto.h"

REGISTER_TOOL(TauJetBDT)
REGISTER_TOOL(TauEleBDT)
REGISTER_TOOL(TauScoreFlatteningTool)
REGISTER_TOOL(TauMuonVeto)

#endif // THOR_MXAODCREATOR_H
