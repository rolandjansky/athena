/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RecToolInterfaces/IHGTD_PadClusterizationTool.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 *
 * @brief To use hits found in HGTD in reconstruction, the raw data objects
 * (RDOs) need to be transformed into "HGTD_PrepRawData" objects. In general,
 * adjacent hits on an LGAD could originate from the same energy deposit. This
 * charge sharing is expected to be negligible, but via the interfaces different
 * ways of handling the transformation and applying clustering algorithms can be
 * implemented.
 */

#ifndef IHGTD_CLUSTERMAKERTOOL_H
#define IHGTD_CLUSTERMAKERTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "HGTD_Identifier/HGTD_ID.h"
#include "HGTD_PrepRawData/HGTD_ClusterCollection.h"
#include "HGTD_RawData/HGTD_RDOCollection.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include <memory>

class IHGTD_PadClusterizationTool : virtual public IAlgTool {

public:
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IHGTD_PadClusterizationTool, 1, 0);

  /**
   * @brief Processes raw data objects on a HGTD module into tracking input.
   *
   * @param [in] rdo_collection RDOs found on a given detector element.
   *
   * @return The collection of clusters built from the RDOs.
   */
  virtual std::unique_ptr<HGTD_ClusterCollection>
  clusterize(const HGTD_RDOCollection& rdo_collection) const = 0;
};

#endif // IHGTD_CLUSTERMAKERTOOL_H
