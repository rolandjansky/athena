/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyGenTrackStore_H
#define MboyGenTrackStore_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MboyGenTrack.h"

  /**
   @class MboyGenTrackStore

   This class is for managing a collections of MboyGenTrack

  @author samusog@cern.ch
  
  */

class MboyGenTrackStore{
public:
    MboyGenTrackStore();
    virtual ~MboyGenTrackStore();

public:
///////////////////////////////////

   /**Reset the Store */
   void Reset();

   /**Print summary */
   void PrintSummary() const;

   /**Print data */
   void Print(std::ostream* out) const;

   /**Add a MboyGenTrack */
   void Add(MboyGenTrack ToBeAdded);

   /**Get number of MboyGenTrack */
   int NberOfMboyGenTrack() const;

   /**Get a MboyGenTrack */
   const MboyGenTrack* GetMboyGenTrack(int MboyGenTrackNber) const;

private:
///////////////////////////////////
//Data

   std::vector<MboyGenTrack>  MboyGenTrackSet ; //!< Storage vector for MboyGenTrack

};


#endif
