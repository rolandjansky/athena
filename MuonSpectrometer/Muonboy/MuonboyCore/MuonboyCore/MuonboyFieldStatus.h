/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyFieldStatus_H
#define MuonboyFieldStatus_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyFieldStatus

   this class manages update of Field Status

  @author samusog@cern.ch
  
  */

class MuonboyFieldStatus{
public:
    MuonboyFieldStatus();
   ~MuonboyFieldStatus();

public:
///////////////////////////////////

   /** Set Field Status data */
   void SetFieldStatus(
                       int getToroidBarrelOn ,
                       int getToroidECTAOn   ,
                       int getToroidECTCOn   ,
                       int getSolenoidOn     ,
                       int getAllToroidOn    ,
                       int getFieldStatusOn 
                      ) ;

};

#endif
