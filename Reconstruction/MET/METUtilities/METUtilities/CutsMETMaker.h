/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _CUTS_METMAKER_
#define _CUTS_METMAKER_
///////////////////////// -*- C++ -*- /////////////////////////////
// CutsMETMaker.h
// Header file for cuts in METMakerAlg and examples
// Author: Russell Smith <rsmith@cern.ch>
///////////////////////////////////////////////////////////////////

#include "AsgMessaging/StatusCode.h"

#include "xAODTau/TauJet.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"

namespace CutsMETMaker{
   StatusCode accept(const xAOD::Muon* mu);
   StatusCode accept(const xAOD::Electron* mu);
   StatusCode accept(const xAOD::Photon* mu);
   StatusCode accept(const xAOD::TauJet* mu);
}
#endif
