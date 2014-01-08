/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboySetBFRT_H
#define MuonboySetBFRT_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MuonboyBFAbstract;
class MuonboyRTAbstract;

  /**
   @class MuonboySetBFRT

  @author samusog@cern.ch
  
  */

class MuonboySetBFRT{
public:
    MuonboySetBFRT();
   ~MuonboySetBFRT();

public:
///////////////////////////////////

 void initialize() ;
 void finalize() ;
 
  /**Set */
   void Set(
     MuonboyBFAbstract*       pMuonboyBFAbstract,
     MuonboyRTAbstract*       pMuonboyRTAbstract
     );

};

#endif

