/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_TrackStoreAth_H
#define MB_TrackStoreAth_H

#include "MboyAthEvt/MboyAthEvtMisc.h"

#include "MboyCoreEvt/MB_TrackStore.h"

#include "MboyAthEvt/MB_TrackAth.h"

class MuonRecDigiStoreAth;

  /**
   @class MB_TrackStoreAth

   This class is managing collection of MB_TrackAth. It derives from a MB_TrackStore. 
   
   The method Consistentify permits the creation of a set of MB_TrackAth 
   from the  already stored MB_Track using a MuonRecDigiStoreAth.
   
   The MB_TrackAth are accessed by GetMB_TrackAth method
   
  @author samusog@cern.ch
  
  */

class MB_TrackStoreAth: public MB_TrackStore {
public:
    MB_TrackStoreAth();
    virtual ~MB_TrackStoreAth();

public:
///////////////////////////////////

   /** Reset collection */
   void ResetMB_TrackStoreAth();

   /** Consistentify  */
   void Consistentify(const MuonRecDigiStoreAth* pMuonRecDigiStoreAth);

   /**Get a track */
   const MB_TrackAth* GetMB_TrackAth(int MB_TrackNber) const;

private:
///////////////////////////////////
//Data

   std::vector<MB_TrackAth>  MB_TrackAthSet ; //!< vector of tracks

};


#endif
