/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_TrackStoreMaker_H
#define MB_TrackStoreMaker_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MuonRecDigiStore;
class MB_TrackStore;

  /**
   @class MB_TrackStoreMaker

   this class builds tracks collection

  @author samusog@cern.ch
  
  */

class MB_TrackStoreMaker{
public:
    MB_TrackStoreMaker();
   ~MB_TrackStoreMaker();

public:
///////////////////////////////////

   /**Do initialisation */
   void initialize();

   /**Do execution */
   void execute(const MuonRecDigiStore* pMuonRecDigiStore,
                      MB_TrackStore* pMB_TrackStore);

   /**Do finalisation */
   void finalize();

};
#endif

