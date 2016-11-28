/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TrigPreFlightCheck.cxx
 * @brief  Does some generic checks to make sure we can run the HLT
 * @author Frank Winklmeier
 *
 * $Id: TrigPreFlightCheck.cxx 5 2013-05-14 10:33:04Z ricab $
 */

#include "TrigPreFlightCheck.h"

#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>

#include <stdlib.h>

using namespace std;
namespace fs = boost::filesystem;

//=========================================================================
// Standard methods
//=========================================================================
TrigPreFlightCheck::TrigPreFlightCheck(const std::string &type,
                           const std::string &name,
                           const IInterface *parent)
  : AthAlgTool(type, name, parent)
{
  // No abstract interface
  declareInterface<TrigPreFlightCheck>(this);

  declareProperty("ReleaseDirs",
                  m_releaseDirs,
                  "Directories within LD_LIBRARY_PATH to check for existence");  
}


TrigPreFlightCheck::~TrigPreFlightCheck()
{}


StatusCode TrigPreFlightCheck::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode TrigPreFlightCheck::check(const MSG::Level& errLvl)
{
  // Only one check so far
  return checkRelease(errLvl);
}


StatusCode TrigPreFlightCheck::checkRelease(const MSG::Level& errLvl)
{
  const char* ld_lib_path = getenv("LD_LIBRARY_PATH");
  if ( ld_lib_path==0 ) {
    msg() << errLvl << "Cannot read LD_LIBRARY_PATH" << endmsg;
    return StatusCode::FAILURE;
  }
  
  typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
  string s(ld_lib_path);
  Tokenizer tok(s, boost::char_separator<char>(":"));

  // Loop over elements of LD_LIBRARY_PATH
  for (Tokenizer::iterator it = tok.begin(); it!=tok.end(); ++it) {
    
    // Loop over directories to check
    for (vector<string>::const_iterator dir = m_releaseDirs.begin();
         dir != m_releaseDirs.end(); ++dir) {
      
      if ( it->find(*dir)!=string::npos ) {
        if ( fs::exists(*it) ) {
          ATH_MSG_DEBUG("Checking " << (*it) << ": OK");
        }
        else {
          msg() << errLvl << (*it) << " does not exist" << endmsg;
          return StatusCode::FAILURE;
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}
