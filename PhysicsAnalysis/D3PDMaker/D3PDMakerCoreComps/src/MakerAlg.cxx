/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MakerAlg.cxx 510585 2012-07-18 09:42:05Z krasznaa $
/**
 * @file D3PDMakerCoreComps/src/MakerAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Algorithm to create a D3PD tree.
 */


#include "MakerAlg.h"
#include "D3PDMakerInterfaces/ID3PD.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/Guards.h"


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
MakerAlg::MakerAlg (const std::string& name,
                    ISvcLocator* svcloc)
  : FilteredAlgorithm (name, svcloc),
    m_d3pdSvc ("D3PD::RootD3PDSvc", name),
    m_tools (this),
    m_metadataTools (this),
    m_audit (false),
    m_d3pd (0),
    m_booked (false),
    m_fillFunction (0)
{
  declareProperty ("D3PDSvc", m_d3pdSvc,
                   "The D3PD creation service.");
  declareProperty ("Tools", m_tools,
                   "List of IObjFillerTool instances to run.");
  declareProperty ("MetadataTools", m_metadataTools,
                   "List of IMetadataTool instances to run.");
  declareProperty ("TuplePath", m_tuplePath,
                   "The name of the tuple.  The interpretation of this "
                   "depends on the D3PDSvc.");
  declareProperty ("Audit", m_audit,
                   "Audit the tools using PerfMon?");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode MakerAlg::initialize()
{
  CHECK( FilteredAlgorithm::initialize() );
  CHECK( m_d3pdSvc.retrieve() );
  CHECK( m_tools.retrieve() );
  CHECK( m_metadataTools.retrieve() );

  // Create the tuple.
  CHECK( m_d3pdSvc->make (m_tuplePath, m_d3pd) );

  // Configure each tool. Since the check on m_audit only has to be
  // done once in the job, it's enough to implement it like this.
  if (m_audit) {
     for (size_t i = 0; i < m_tools.size(); i++) {
        Gaudi::Guards::AuditorGuard auditor( m_tools[ i ]->name() +
                                             ":cfg", auditorSvc(), "ini" );
        CHECK( m_tools[i]->configureD3PD (m_d3pd) );
     }
  } else {
     for (size_t i = 0; i < m_tools.size(); i++) {
        CHECK( m_tools[i]->configureD3PD (m_d3pd) );
     }
  }

  // Decide which fill function is to be used later on. This way the
  // if statement is only executed once, and not in every execute()
  // function.
  if (m_audit) {
     m_fillFunction = &MakerAlg::fillToolWithAuditor;
  } else {
     m_fillFunction = &MakerAlg::fillToolWithoutAuditor;
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c finalize method.
 */
StatusCode MakerAlg::finalize()
{
  // Run metadata tools.
  for (size_t i = 0; i < m_metadataTools.size(); i++)
    CHECK( m_metadataTools[i]->writeMetadata (m_d3pd) );

  CHECK( FilteredAlgorithm::finalize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode MakerAlg::execute()
{
  if (!m_booked) {
    m_booked = true;
    // This check also only has to be done once. So no reason to do it
    // in a more complicated way.
    if (m_audit) {
       for (size_t i = 0; i < m_tools.size(); i++) {
          Gaudi::Guards::AuditorGuard auditor( m_tools[ i ]->name() +
                                               ":book", auditorSvc(), "evt" );
          CHECK( m_tools[i]->book() );
       }
    } else {
       for (size_t i = 0; i < m_tools.size(); i++) {
          CHECK( m_tools[i]->book() );
       }
    }
  }

  // Only write out "accepted" events:
  if( ! isEventAccepted() ) return StatusCode::SUCCESS;

  CHECK( m_d3pd->clear() );

  // Call fill() on the tools with the pre-configured function. Notice that
  // no if statements are put into the code here...
  for (size_t i = 0; i < m_tools.size(); i++) {
     CHECK( ( this->*m_fillFunction )( *m_tools[i] ) );
  }

  CHECK( m_d3pd->capture() );

  return StatusCode::SUCCESS;
}


/**
 * This function is used to call <code>fill()</code> on the object filler
 * tools when PerfMon auditing is turned on.
 *
 * @param tool The tool to call the <code>fill()</code> function on
 * @returns The result of calling the <code>fill()</code> function
 */
StatusCode MakerAlg::fillToolWithAuditor( IObjFillerTool& tool )
{
   Gaudi::Guards::AuditorGuard auditor( tool.name() +
                                        ":fill", auditorSvc(), "evt" );
   return tool.fill();
}


/**
 * This function is used to call <code>fill()</code> on the object filler
 * tools when PerfMon auditing is turned off.
 *
 * @param tool The tool to call the <code>fill()</code> function on
 * @returns The result of calling the <code>fill()</code> function
 */
StatusCode MakerAlg::fillToolWithoutAuditor( IObjFillerTool& tool )
{
   return tool.fill();
}


} // namespace D3PD
