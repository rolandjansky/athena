// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: prepareTriggerMenu.h 631651 2014-11-27 18:33:16Z lheinric $
#ifndef TRIGCONFXAOD_TOOLS_PREPARETRIGGERMENU_H
#define TRIGCONFXAOD_TOOLS_PREPARETRIGGERMENU_H

// Infrastructure include(s):
#include "AsgMessaging/StatusCode.h"

// xAOD include(s):
#include "xAODTrigger/TriggerMenu.h"

// Forward declaration(s):
class MsgStream;

namespace TrigConf {

   // Forward declaration(s):
   class CTPConfig;
   class HLTChainList;
   class HLTSequenceList;
   class BunchGroupSet;

   class HLTMenu;
   class L1Menu;
   class HLTPrescalesSet;
   class L1PrescalesSet;
   class L1BunchGroupSet;

   /// Function providing translation for the transient configuration from the R2 AOD format
   StatusCode prepareTriggerMenu( const xAOD::TriggerMenu* menu,
                                  CTPConfig& ctpConfig,
                                  HLTChainList& chainList,
                                  HLTSequenceList& sequenceList,
                                  BunchGroupSet& bgSet,
                                  MsgStream& msg );

   StatusCode prepareTriggerMenu(const HLTMenu& loadedHlt,
                                 const L1Menu& loadedL1,
                                 const HLTPrescalesSet& loadedHltps,
                                 const L1PrescalesSet& loadedL1ps,
                                 const L1BunchGroupSet& loadedBgSet,
                                 CTPConfig& ctpConfig,
                                 HLTChainList& chainList,
                                 HLTSequenceList& sequenceList,
                                 BunchGroupSet& bgSet,
                                 MsgStream& msg );

} // namespace TrigConf

#endif // TRIGCONFXAOD_TOOLS_PREPARETRIGGERMENU_H
