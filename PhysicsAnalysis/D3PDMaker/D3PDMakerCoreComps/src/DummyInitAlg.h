// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DummyInitAlg.h 486128 2012-03-02 08:54:20Z krasznaa $
/**
 * @file D3PDMakerCoreComps/src/DummyInitAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Work around initialization ordering problems.
 */


#ifndef D3PDMAKERCORECOMPS_DUMMYINITALG_H
#define D3PDMAKERCORECOMPS_DUMMYINITALG_H



#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"


namespace D3PD {


/**
 * @brief Work around initialization ordering problems.
 *
 * If the event store is initialized before the detector store,
 * then the detector store won't get its providers set correctly
 * (because initializing the event store ends up initializing
 * the detector store while the ProxyProviderSvc is initializing).
 * FIXME: Make this more robust in core, so we don't need to do this!
 */
class DummyInitAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  DummyInitAlg (const std::string& name,
                ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  ServiceHandle<ITHistSvc> m_histSvc;
};


} // namespace D3PD


#endif // D3PDMAKERCORECOMPS_DUMMYINITALG_H
