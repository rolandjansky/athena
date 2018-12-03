/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file   ISiLocAlignDBTool.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-12-03
/// 
/// @brief  Header file for interface of SiLocAlignDBTool used to read local alignment for database
#ifndef AFP_DBTOOLS_ISILOCALIGNDBTOOL_H
#define AFP_DBTOOLS_ISILOCALIGNDBTOOL_H


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
  class SiLocAlignData;

  static const InterfaceID IID_ISiLocAlignDBTool ("ISiLocAlignDBTool", 1, 0);

  
  /// Interface to tool providing local alignment of silicon detectors from the conditions database.
  class ISiLocAlignDBTool : virtual public IAlgTool
  {
  public:
    static const InterfaceID &interfaceID()
    {return IID_ISiLocAlignDBTool;}

    /// Does nothing
    virtual ~ISiLocAlignDBTool() override {}

    /// Initilise tool
    virtual StatusCode initialize() = 0;

    /// Finalise tool
    virtual StatusCode finalize() = 0;

    /// Provide alignment parameters for a given plane. Returns nullptr if no data available.
    virtual const SiLocAlignData* alignment (const int stationID, const int planeID) const = 0;

    /// Returns reference to a vector of alignments for a given station.
    ///
    /// It is enough to get this reference only once, because the
    /// vector is a member of the class. Whenever new conditions are
    /// loaded the values in the vector will be updated.
    ///
    /// @warning if in database there was no data about the given
    /// layer a nullptr will be stored in the vector.
    virtual const std::vector<std::unique_ptr<const SiLocAlignData> >& alignment (const int stationID) const = 0;


  protected:
    /// @brief Method called when new conditions are loaded
    ///
    /// The method copies information from #m_conditionsData to 
    virtual StatusCode update (IOVSVC_CALLBACK_ARGS) = 0;
  };


}      // namespace AFP

#endif // > ! AFP_DBTOOLS_SILOCALIGNDBTOOL_H
