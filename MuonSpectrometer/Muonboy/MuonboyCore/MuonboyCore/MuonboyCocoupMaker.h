/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyCocoupMaker_H
#define MuonboyCocoupMaker_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MboyPosDigiChoice;
class MuonRecDigiStore;
class MboyGenTrackStore;

  /**
   @class MuonboyCocoupMaker

   this class loads digits

  @author samusog@cern.ch
  
  */

class MuonboyCocoupMaker{
public:
    MuonboyCocoupMaker();
   ~MuonboyCocoupMaker();

public:
///////////////////////////////////

   /**Do initialisation */
   void initialize();

   /**Do execution */
   void execute(
     const MboyPosDigiChoice* pMboyPosDigiChoice ,
     MuonRecDigiStore*        pMuonRecDigiStore,
     const MboyGenTrackStore* pMboyGenTrackStore
     );

   /**Do finalisation */
   void finalize();

};

#endif

