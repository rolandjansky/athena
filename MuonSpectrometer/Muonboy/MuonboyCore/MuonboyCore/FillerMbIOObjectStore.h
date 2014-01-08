/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FillerMbIOObjectStore_H
#define FillerMbIOObjectStore_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MuonRecDigiStore;
class MbIOObjectStore;

  /**
   @class FillerMbIOObjectStore

  @author samusog@cern.ch
  
  */

class FillerMbIOObjectStore{
public:
    FillerMbIOObjectStore();
    ~FillerMbIOObjectStore();


public:
///////////////////////////////////

   void FillMbIOObjectStore(
     const MuonRecDigiStore* pMuonRecDigiStore,
     MbIOObjectStore* pMbIOObjectStore,
     int TrackNber
   );

};
#endif

