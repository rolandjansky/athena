/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_TrackStore_H
#define MB_TrackStore_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MB_Track.h"

//class MuonRecDigiStore;

  /**
   @class MB_TrackStore

   This class is managing collection of tracks

  @author samusog@cern.ch
  
  */

class MB_TrackStore{
public:
    MB_TrackStore();
    virtual ~MB_TrackStore();

public:
///////////////////////////////////

   /** Reset collection */
   void ResetMB_TrackStore();

   /**Print collection summary */
   void PrintSummary() const;

   /**Print collection data */
   void PrintTracks(std::ostream* out,
                    int EventNber) const;

   /**Add a track */
   void Add(const MB_Track& ToBeAdded);

   /**Get number of tracks */
   int NberOfMB_Track() const;

   /**Get a track */
   const MB_Track* GetMB_Track(int MB_TrackNber) const;

private:
///////////////////////////////////
//Data
// MB_Track
   std::vector<MB_Track>  MB_TrackSet ; //!< vector of tracks

};


#endif
