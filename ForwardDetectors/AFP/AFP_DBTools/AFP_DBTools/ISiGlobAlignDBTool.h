/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @file   ISiGlobAlignDBTool.h
/// @author Petr Balek <petr.balek@cern.ch>
/// @date   2021-02-23
/// 
/// @brief  Header file for interface of SiGlobAlignDBTool used to read global alignment for database
#ifndef AFP_DBTOOLS_ISIGLOBALIGNDBTOOL_H
#define AFP_DBTOOLS_ISIGLOBALIGNDBTOOL_H


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

// database access
#include "AthenaKernel/IOVSvcDefs.h"

// general includes
#include <vector>
#include <memory>

namespace AFP
{
  // forward declarations
  class SiGlobAlignData;

  static const InterfaceID IID_ISiGlobAlignDBTool ("ISiGlobAlignDBTool", 1, 0);

  
  /// Interface to tool providing local alignment of silicon detectors from the conditions database.
  class ISiGlobAlignDBTool : virtual public IAlgTool
  {
  public:
    static const InterfaceID &interfaceID()
    {return IID_ISiGlobAlignDBTool;}

    /// Does nothing
    virtual ~ISiGlobAlignDBTool() override {}

    /// Initilise tool
    virtual StatusCode initialize() override = 0;

    /// Finalise tool
    virtual StatusCode finalize() override = 0;

    /// Provide alignment parameters for a given station. Returns nullptr if no data available.
    virtual const SiGlobAlignData* alignment (const int stationID) const = 0;


  protected:
    /// @brief Method called when new conditions are loaded
    ///
    /// The method copies information from #m_conditionsData to 
    virtual StatusCode update (IOVSVC_CALLBACK_ARGS) = 0;
  };


}      // namespace AFP

#endif // > ! AFP_DBTOOLS_SIGLOBALIGNDBTOOL_H

