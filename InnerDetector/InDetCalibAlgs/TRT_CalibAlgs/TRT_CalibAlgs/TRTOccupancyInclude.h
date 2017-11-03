/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBALGS_TRTOCCUPANCYINCLUDE_H
#define TRT_CALIBALGS_TRTOCCUPANCYINCLUDE_H 

#include <string>
#include <vector>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TRT_ElectronPidTools/ITRT_LocalOccupancy.h"

#include "StoreGate/ReadHandleKey.h"

#include "xAODEventInfo/EventInfo.h"

/**

      @class TRTOccupancyInclude

      This algorithm includes the Occupancy inside event info....

Documentation being updated (Jan 2009)
      @Alex

*/


namespace InDet{
        class ITRT_LocalOccupancy;
}

class TRTOccupancyInclude: public AthAlgorithm
{

public:

  TRTOccupancyInclude(const std::string& name, ISvcLocator* pSvcLocator);
  ~TRTOccupancyInclude(void);

  StatusCode initialize(void);    
  StatusCode execute(void);
  StatusCode finalize(void);  

private:


  ToolHandle<InDet::ITRT_LocalOccupancy> m_LocalOccTool;     //!< the track selector tool 
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfoKey", "EventInfo", "RHK to retrieve xAOD::EventInfo"}; //!
};

#endif // TRT_CALIBALGS_TRTOCCUPANCYINCLUDE_H 
