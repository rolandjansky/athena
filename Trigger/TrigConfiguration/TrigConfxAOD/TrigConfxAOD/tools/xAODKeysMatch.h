// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODKeysMatch.h 612011 2014-08-15 13:43:57Z krasznaa $
#ifndef TRIGCONFXAOD_TOOLS_XAODKEYSMATCH_H
#define TRIGCONFXAOD_TOOLS_XAODKEYSMATCH_H

// xAOD include(s):
#include "xAODTrigger/TrigConfKeys.h"
#include "xAODTrigger/TriggerMenu.h"

namespace TrigConf {

   /// Helper function checking if the DB keys match on two objects
   bool xAODKeysMatch( const xAOD::TrigConfKeys* keys,
                       const xAOD::TriggerMenu* menu );

} // namespace TrigConf

#endif // TRIGCONFXAOD_TOOLS_XAODKEYSMATCH_H
