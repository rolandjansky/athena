/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/Configuration.h>

//
// method implementations
//

namespace ana
{
  Configuration ::
  Configuration ()
    : eventinfoDef ("none"), eventSelectDef ("none"),
      electronDef ("none"), photonDef ("none"), muonDef ("none"),
      tauDef ("none"), jetDef ("none"), pfJetDef ("none"), fatJetDef ("none"),
      metDef ("none"), met2Def ("none"), cleanDef ("none"), orDef ("none"), triggerDef(""),
      schedulerDef ("basic")
  {}



  void Configuration ::
  setConfig (const Configuration& conf)
  {
    *this = conf;
  }
}
