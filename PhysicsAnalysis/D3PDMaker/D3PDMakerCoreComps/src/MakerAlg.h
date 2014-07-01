// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MakerAlg.h 487245 2012-03-08 09:00:36Z krasznaa $
/**
 * @file D3PDMakerCoreComps/src/MakerAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Algorithm to create a D3PD tree.
 */


#ifndef D3PDMAKERCORECOMPS_MAKERALG_H
#define D3PDMAKERCORECOMPS_MAKERALG_H


#include "D3PDMakerInterfaces/ID3PD.h"
#include "D3PDMakerInterfaces/ID3PDSvc.h"
#include "D3PDMakerInterfaces/IObjFillerTool.h"
#include "D3PDMakerInterfaces/IMetadataTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/FilteredAlgorithm.h"
#include <vector>


namespace D3PD {


/**
 * @brief Algorithm to create a D3PD tree.
 *
 * This algorithm creates one D3PD tree.
 * It's given a list of @c IObjFillerTool instances to then run
 * on each event.
 * Most of the actual work is done by the separate D3PDSvc.
 *
 * Properties:
 *   D3PDSvc   - The D3PD creation service.
 *   Tools     - List of IObjFillerTool instances to run.
 *   TuplePath - The name of the tuple.  The interpretation of this
 *               depends on the D3PDSvc.
 */
class MakerAlg
  : public FilteredAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  MakerAlg (const std::string& name,
            ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c finalize method.
  virtual StatusCode finalize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  /// Call fill() on a tool with an auditor
  StatusCode fillToolWithAuditor( IObjFillerTool& tool );

  /// Cakk fill on a tool without an auditor
  StatusCode fillToolWithoutAuditor( IObjFillerTool& tool );


  /// Property: The D3PD creation service.
  ServiceHandle<ID3PDSvc> m_d3pdSvc;

  /// Property: List of object filler tools to run.
  ToolHandleArray<IObjFillerTool> m_tools;

  /// Property: List of metadata tools to run.
  ToolHandleArray<IMetadataTool> m_metadataTools;

  /// Property: Audit the tools with PerfMon or not?
  bool m_audit;

  /// Property: The tuple name.
  std::string m_tuplePath;

  /// The created tuple.  Note: we don't take ownership.
  ID3PD* m_d3pd;

  /// Flag that we've called book().
  bool m_booked;

  /// Pointer to the fill function to be used
  StatusCode ( D3PD::MakerAlg::*m_fillFunction )( IObjFillerTool& );
};


} // namespace D3PD


#endif //not D3PDMAKERCORECOMPS_MAKERALG_H
