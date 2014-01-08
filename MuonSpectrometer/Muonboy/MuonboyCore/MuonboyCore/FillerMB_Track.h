/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FillerMB_Track_H
#define FillerMB_Track_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MB_Track_SpecEntr;
class MB_Track_CaloEntr;
class MB_Track_Perigee;

  /**
   @class FillerMB_Track

   this class builds tracks collection

  @author samusog@cern.ch
  
  */

class FillerMB_Track{
public:
    FillerMB_Track();
   ~FillerMB_Track();

public:
///////////////////////////////////

   /**Do execution */
   void FillMB_Track_SpecEntr (MB_Track_SpecEntr* pMB_Track_SpecEntr,int TrackNber);
   void FillMB_Track_CaloEntr (MB_Track_CaloEntr* pMB_Track_CaloEntr,int TrackNber);
   void FillMB_Track_Perigee  (MB_Track_Perigee*  pMB_Track_Perigee ,int TrackNber);

};
#endif

