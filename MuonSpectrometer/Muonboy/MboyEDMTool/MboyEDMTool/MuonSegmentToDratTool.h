/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonSegmentToDratTool_H
#define MuonSegmentToDratTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyEDMToolInterfaces/IMuonSegmentToDratTool.h"

namespace Muon {
  class  MuonSegment;
}

class MuonRecDigiStoreAth;

namespace Trk {
  class IMeasurementBaseToDratTool ;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

namespace Trk {

  /**
   @class MuonSegmentToDratTool
   
  @author samusog@cern.ch
  
  */

class MuonSegmentToDratTool:public AthAlgTool, virtual public IMuonSegmentToDratTool{
public:
    MuonSegmentToDratTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonSegmentToDratTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** Convert single object */
    virtual MbDRAT* convert (
                    MuonRecDigiStoreAth*     pMuonRecDigiStoreAth ,
                    const Muon::MuonSegment* pMuonSegment
                               );

private:
///////////////////////////////////

   ToolHandle< Trk::IMeasurementBaseToDratTool > p_IMeasurementBaseToDratTool ; //!< Pointer On IMeasurementBaseToDratTool

};
}

#endif

