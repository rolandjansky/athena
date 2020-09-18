/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */

/**
 * @file AthenaServices/CompressionInfo.h
 * @author Alaettin Serhan Mete <amete@cern.ch>
 * @date Aug, 2020
 * @brief Communicate between AthenaOuptutStream and ThinningCacheTool.
 *
 * AthenaOutputStream processes ItemList entries, including the selections
 * of which static or dynamic variables to be lossy float compressed.
 * This last information is later used by the ThinningCacheTool to
 * populate the ThinningCache. To convey this information,
 * AthenaOutputStream records a CompressionInfo object in the events store
 * with a key of `CompressionInfo_STREAM`, for stream STREAM.
 * CompressionInfo then contains a map from object names to
 * a set of variables to be lossy float compressed and the associated 
 * compression levels (nmantissa).
 */

#ifndef ATHENASERVICES_COMPRESSIONINFO_H
#define ATHENASERVICES_COMPRESSIONINFO_H

#include "AthenaKernel/ThinningInfo.h"

#include <string>
#include <unordered_map>


namespace SG {

  /**
   * @brief Map of compressed variables and their compression levels
   */

  using CompressionInfo = std::unordered_map<std::string, SG::ThinningInfo::compression_map_t>;

} // namespace SG

CLASS_DEF( SG::CompressionInfo, 39950991, 1)

#endif
