/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyExtROALoader_H
#define MuonboyExtROALoader_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MboyROAStore;

  /**
   @class MuonboyExtROALoader

  @author samusog@cern.ch
  
  */

class MuonboyExtROALoader{
public:
    MuonboyExtROALoader();
   ~MuonboyExtROALoader();

public:
///////////////////////////////////

   /**Do initialisation */
   void initialize();

   /**Do reset */
   void Reset();

   /**Do execution */
   void execute(MboyROAStore* pMboyROAExternalStore);

   /**Do finalisation */
   void finalize();

};

#endif

