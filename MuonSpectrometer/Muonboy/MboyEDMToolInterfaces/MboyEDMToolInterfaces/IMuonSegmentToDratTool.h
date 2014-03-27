/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuonSegmentToDratTool_H
#define IMuonSegmentToDratTool_H

#include "GaudiKernel/IAlgTool.h" 

namespace Muon {
  class  MuonSegment;
}

class MbDRAT ;
class MuonRecDigiStoreAth;

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

namespace Trk {

  /**
   @class IMuonSegmentToDratTool
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMuonSegmentToDratTool("IMuonSegmentToDratTool", 1, 0);

class IMuonSegmentToDratTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Convert single object */
   virtual MbDRAT* convert (
                    MuonRecDigiStoreAth*     pMuonRecDigiStoreAth ,
                    const Muon::MuonSegment* pMuonSegment
                               ) = 0;
};

inline const InterfaceID& Trk::IMuonSegmentToDratTool::interfaceID(){return IID_IIMuonSegmentToDratTool;}

}

#endif

