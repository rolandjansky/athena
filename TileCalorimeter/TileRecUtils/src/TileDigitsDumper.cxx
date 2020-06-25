/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileRecUtils/src/TileDigitsDumper.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Dump a TileDigitsContainer to a text file.
 */


#include "TileDigitsDumper.h"
#include "StoreGate/ReadHandle.h"
#include "CxxUtils/StrFormat.h"
#include <fstream>


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
TileDigitsDumper::TileDigitsDumper (const std::string& name, ISvcLocator* svcloc)
  : AthReentrantAlgorithm (name, svcloc)
{
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode TileDigitsDumper::initialize()
{
  ATH_CHECK( m_contKey.initialize() );
  if (!m_altContKey.key().empty()) {
    ATH_CHECK( m_altContKey.initialize() );
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 * @param ctx Event Context.
 */
StatusCode TileDigitsDumper::execute (const EventContext& ctx) const
{
  if (!m_altContKey.key().empty()) {
    SG::ReadHandle<TileDigitsContainer> altCont (m_altContKey, ctx);
    *altCont;
  }

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

  SG::ReadHandle<TileDigitsContainer> cont (m_contKey, ctx);

  fout << "TileDigitsContainer/" << m_contKey.key() << " "
       << "run " << ctx.eventID().run_number() << " "
       << "event " << ctx.eventID().event_number() << std::endl;

  fout << "Type: " << cont->get_type() << " ("
       << TileFragHash::typeToString (cont->get_type()) << ") Units: "
       << cont->get_unit() << " ("
       << TileRawChannelUnit::unitToString (cont->get_unit())
       << ") BSflags: 0x" << std::hex << cont->get_bsflags() << std::dec
       << std::endl;

  for (const TileDigitsCollection* coll : *cont) {
    fout << static_cast<std::string> (*coll);
    coll->printExtra (fout);
    fout << "\n";
  }

  fout.close();
  
  return StatusCode::SUCCESS;
}

