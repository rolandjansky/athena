/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyMuonBackTracker_H
#define MboyMuonBackTracker_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MB_Track;

  /**
   @class MboyMuonBackTracker

  @author samusog@cern.ch
  
  */

class MboyMuonBackTracker{
public:
    MboyMuonBackTracker();
   ~MboyMuonBackTracker();

public:
///////////////////////////////////

   /**Do initialisation */
   void initialize();

   /**Do execution */
   void execute(
     MB_Track*  pMB_Track,
     MB_Track*& pMB_Track_New
     );

   /**Do finalisation */
   void finalize();

};

#endif

