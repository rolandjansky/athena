/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_SegmentStoreAth_H
#define MB_SegmentStoreAth_H

#include "MboyAthEvt/MboyAthEvtMisc.h"

#include "MboyCoreEvt/MB_SegmentStore.h"

#include "MboyAthEvt/MB_SegmentAth.h"

class MuonRecDigiStoreAth;

  /**
   @class MB_SegmentStoreAth

   This class is managing collection of MB_SegmentAth. It derives from a MB_SegmentStore. 
   
   The method Consistentify permits the creation of a set of MB_SegmentAth 
   from the  already stored MB_Segment using a MuonRecDigiStoreAth.
   
   The MB_SegmentAth are accessed by GetMB_SegmentAth method
   
  @author samusog@cern.ch
  
  */

class MB_SegmentStoreAth: public MB_SegmentStore{
public:
    MB_SegmentStoreAth();
    virtual ~MB_SegmentStoreAth();

public:
///////////////////////////////////

   /** Reset collection */
   void ResetMB_SegmentStoreAth();

   /** Consistentify  */
   void Consistentify(const MuonRecDigiStoreAth* pMuonRecDigiStoreAth);

   /**Get a segment */
   const MB_SegmentAth* GetMB_SegmentAth(int MB_SegmentNber) const;

private:
///////////////////////////////////
//Data

   std::vector<MB_SegmentAth>  MB_SegmentAthSet ; //!< vector of segments

};


#endif
