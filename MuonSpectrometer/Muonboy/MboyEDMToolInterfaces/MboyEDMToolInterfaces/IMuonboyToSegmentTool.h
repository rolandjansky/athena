/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuonboyToSegmentTool_H
#define IMuonboyToSegmentTool_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"

namespace Muon {
  class  MuonSegment;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

class MB_SegmentStoreAth;

namespace Trk {

  /**
   @class IMuonboyToSegmentTool

   This tools converts Muonboy segments into Trk::Segment
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMuonboyToSegmentTool("IMuonboyToSegmentTool", 1, 0);

class IMuonboyToSegmentTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Convert collection */
   virtual StatusCode convertCollection(
                                const MB_SegmentStoreAth* pMB_SegmentStoreAth,
			        Trk::SegmentCollection* pSegmentCollection
                                 , int Clean = 1
                               ) = 0 ;


   /** Convert collection */
   virtual std::vector<const Muon::MuonSegment*>* convertCollection(
                                const MB_SegmentStoreAth* pMB_SegmentStoreAth
                                 , int Clean = 1
                               ) = 0 ;
};

inline const InterfaceID& Trk::IMuonboyToSegmentTool::interfaceID(){return IID_IIMuonboyToSegmentTool;}

}

#endif

