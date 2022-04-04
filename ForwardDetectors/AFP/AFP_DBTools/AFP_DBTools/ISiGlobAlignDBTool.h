/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @brief  Header file for interface of SiGlobAlignDBTool used to read global alignment for database
#ifndef AFP_DBTOOLS_ISIGLOBALIGNDBTOOL_H
#define AFP_DBTOOLS_ISIGLOBALIGNDBTOOL_H


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

namespace AFP
{
  // forward declarations
  class SiGlobAlignData;
  
  /// Interface to tool providing local alignment of silicon detectors from the conditions database.
  class ISiGlobAlignDBTool : virtual public IAlgTool
  {
  public:
    DeclareInterfaceID(ISiGlobAlignDBTool, 1, 0);

    /// Provide alignment parameters for a given station. Returns zeros if no data available.
    virtual const SiGlobAlignData alignment (const EventContext& ctx, const int stationID) const = 0;
  };

}      // namespace AFP

#endif // > ! AFP_DBTOOLS_ISIGLOBALIGNDBTOOL_H
