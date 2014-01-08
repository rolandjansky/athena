/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_SegmentStoreMaker_H
#define MB_SegmentStoreMaker_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MuonRecDigiStore;
class MB_SegmentStore;

  /**
   @class MB_SegmentStoreMaker

   this class builds segments collection

  @author samusog@cern.ch
  
  */

class MB_SegmentStoreMaker{
public:
    MB_SegmentStoreMaker();
   ~MB_SegmentStoreMaker();

public:
///////////////////////////////////

   /**Do initialisation */
   void initialize();

   /**Do execution */
   void execute(const MuonRecDigiStore* pMuonRecDigiStore,
                      MB_SegmentStore*  pMB_SegmentStore);

   /**Do finalisation */
   void finalize();

};
#endif
