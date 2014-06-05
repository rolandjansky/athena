/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetTagFinder.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2012
 * @brief Common code for retrieving jet tag information from a jet.
 */


#include "JetTagFinder.h"
#include "JetEvent/Jet.h"
#include "GaudiKernel/AlgTool.h"
#include "boost/tokenizer.hpp"
#include "boost/foreach.hpp"


namespace D3PD { 


/**
 * @brief Constructor.
 * @param filler The tool of which we are a part.
 * @param infoTypeDefault Default tag info key string.
 * @param allowMissingDefault Default value for the AllowMissing property.
 */
JetTagFinder::JetTagFinder (AlgTool& filler,
                            const std::string& infoTypeDefault,
                            bool allowMissingDefault /*=false*/)
  : m_filler (filler)
{
  filler.declareProperty ("InfoType",
                          m_infoType = infoTypeDefault,
                          "Name of the tagger for which information should be filled.  "
                          "May contain several names separated by commas; "
                          "the first one found is used.");
  filler.declareProperty ("AllowMissing",
                          m_allowMissing = allowMissingDefault,
                          "Don't issue warnings if the tag is missing.");
}


/**
 * @brief Retrieve a taginfo object.
 * @param j Jet from which the object is to be retrieved.
 * @return The taginfo object or 0.
 *
 * If the object can't be found, we return 0; we also issue
 * a warning unless AllowMissing is set.
 */
const JetTagInfoBase* JetTagFinder::getTagInfoBase (const Jet& j) const
{
  if (m_keys.empty()) {
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(",");
    tokenizer tokens (m_infoType, sep);
    m_keys.assign (tokens.begin(), tokens.end());
  }

  BOOST_FOREACH (const std::string& key, m_keys) {
    const JetTagInfoBase* ti = j.tagInfo (key);
    if (ti) return ti;
  }

  if (!m_allowMissing) {
    REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, m_filler.name())
      << "Can't find tag info " << m_infoType;
  }

  return 0;
}


} // namespace D3PD
