// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaServices/src/ThinningCacheTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Create ThinningCache for a given stream.
 */


#ifndef ATHENASERVICES_THINNINGCACHETOOL_H
#define ATHENASERVICES_THINNINGCACHETOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/ThinningCache.h"
#include "AthenaKernel/IAthenaOutputTool.h"
#include "AthenaKernel/ITrigNavigationThinningSvc.h"
#include "GaudiKernel/ServiceHandle.h"


namespace Athena {


/**
 * @brief Create ThinningCache for a given stream.
 *
 * Hooks in this class are called before and after output is written for a given
 * stream.  The @c preExecute hook will query the event store for any thinning
 * to be done for the stream, build a ThinningCache object for all of these,
 * and install it in the current EventContext.  The @c postExecute hook
 * will then clear the cache from the EventContext.
 */
class ThinningCacheTool : public extends<AthAlgTool, IAthenaOutputTool>
{
public:
  using base_class::base_class;


  /**
   * @brief Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Called at the end of initialize.  A no-op here.
   */
  virtual StatusCode postInitialize() override;


  /**
   * @brief Called at the beginning of execute.  A no-op for now.
   */
  virtual StatusCode preExecute() override;


  /**
   * @brief Called before actually streaming objects.
   *        Find all thinning requests for this stream, build the @c ThinningCache,
   *        and install it in the current @c EventContext.
   */
  virtual StatusCode preStream() override;


  /**
   * @brief Called at the end of execute.
   *        Remove ThinningCache from the EventContext and clear the cache instance.
   */
  virtual StatusCode postExecute() override;


  /**
   * @brief Called at the beginning of finalize.  A no-op here.
   */
  virtual StatusCode preFinalize() override;


private:
  /**
   * @brief Test if a thinning decision object is for our stream.
   * @param key The SG key of the @c ThinningDecision object.
   *
   * If this thinning decision is for our stream, then return the SG key
   * of the object being thinned.  Otherwise, return an empty string.
   */
  std::string wantedKey (const std::string& key) const;


  /// Name of the stream for this tool.
  StringProperty m_streamName
  { this, "StreamName", "", "Name of the stream being written." };


  /// Optional TrigNavigation thinning service to use.
  ServiceHandle<ITrigNavigationThinningSvc> m_trigNavigationThinningSvc
  { this, "TrigNavigationThinningSvc", "", "Service to use for TrigNavigation thinning (optional)." };

  
  /// Thinning cache instance for this stream.
  SG::ThinningCache m_cache;
};


} // namespace Athena


#endif // not ATHENASERVICES_THINNINGCACHETOOL_H
