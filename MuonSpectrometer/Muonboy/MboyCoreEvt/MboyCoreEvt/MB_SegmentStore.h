/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_SegmentStore_H
#define MB_SegmentStore_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MB_Segment.h"

  /**
   @class MB_SegmentStore

   This class is managing collection of segments

  @author samusog@cern.ch
  
  */

class MB_SegmentStore{
public:
    MB_SegmentStore();
    virtual ~MB_SegmentStore();

public:
///////////////////////////////////

   /** Reset collection */
   void ResetMB_SegmentStore();

   /**Print collection summary */
   void PrintSummary() const;

   /**Print collection data */
   void PrintSegments(std::ostream* out,
                      int EventNber) const;

   /**Add a segment */
   void Add(const MB_Segment& ToBeAdded);

   /**Get number of segments */
   int NberOfMB_Segment() const;

   /**Get a segment */
   const MB_Segment* GetMB_Segment(int MB_SegmentNber) const;

private:
///////////////////////////////////
//Data

   std::vector<MB_Segment>  MB_SegmentSet ; //!< vector of segments

};

#endif
