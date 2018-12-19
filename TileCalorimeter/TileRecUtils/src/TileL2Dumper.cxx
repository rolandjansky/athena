/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileRecUtils/src/TileL2Dumper.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Dump a TileL2Container to a text file.
 */


#include "TileL2Dumper.h"
#include "StoreGate/ReadHandle.h"
#include "CxxUtils/StrFormat.h"
#include <fstream>


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
TileL2Dumper::TileL2Dumper (const std::string& name, ISvcLocator* svcloc)
  : AthReentrantAlgorithm (name, svcloc)
{
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode TileL2Dumper::initialize()
{
  ATH_CHECK( m_contKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 * @param ctx Event Context.
 */
StatusCode TileL2Dumper::execute (const EventContext& ctx) const
{
  std::string fname = CxxUtils::strformat ("%s%s_%08u_%08lu.dump",
                                           m_prefix.value().c_str(),
                                           m_contKey.key().c_str(),
                                           ctx.eventID().run_number(),
                                           ctx.eventID().event_number());
  std::ofstream fout (fname);
  if (!fout) {
    ATH_MSG_ERROR ("Cannot open dump file: " << fname);
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<TileL2Container> cont (m_contKey, ctx);

  fout << "TileL2Container/" << m_contKey.key() << " "
       << "run " << ctx.eventID().run_number() << " "
       << "event " << ctx.eventID().event_number() << std::endl;

  fout << static_cast<std::string> (*cont) << "\n";

  fout.close();
  
  return StatusCode::SUCCESS;
}

