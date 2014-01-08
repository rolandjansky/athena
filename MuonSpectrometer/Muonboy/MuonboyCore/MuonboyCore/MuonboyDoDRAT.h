/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyDoDRAT_H
#define MuonboyDoDRAT_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MbDRAT;

  /**
   @class MuonboyDoDRAT

  @author samusog@cern.ch
  
  */

class MuonboyDoDRAT{
public:
    MuonboyDoDRAT();
   ~MuonboyDoDRAT();

public:
///////////////////////////////////

   /**Do initialisation */
   void initialize();

   /**Do execution */
   void execute(
        std::vector<MbDRAT*>*    pVectorOfMbDRAT
     );
   void execute(
        MbDRAT*        pMbDRAT
     );

   /**Do finalisation */
   void finalize();

};

#endif

