/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AandBlinesLoader_H
#define AandBlinesLoader_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MboyAlineStore;
class MboyBlineStore;
class MboyIlineStore;

  /**
   @class AandBlinesLoader

    this class loads the stored A and B lines

  @author samusog@cern.ch
  
  */

class AandBlinesLoader{
public:
    AandBlinesLoader();
   ~AandBlinesLoader();

public:
///////////////////////////////////

   /**Do initialisation */
   void initialize();

   /**Do execution */
   int execute(
               MboyAlineStore* pMboyAlineStore,
               MboyBlineStore* pMboyBlineStore,
               MboyIlineStore* pMboyIlineStore 
	      );

   /**Do finalisation */
   void finalize();

};

#endif

