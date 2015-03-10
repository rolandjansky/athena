/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBPATTERNUNITSTREAMERTOOL_H
#define TBREC_TBPATTERNUNITSTREAMERTOOL_H

/*! \brief compares pattern unit with client defined masks 
 *
 * The \c TBPatternUnitStreamerTool compares the event pattern unit with client
 * defined masks for acceptance and rejection. Pattern unit bits are addressed
 * by name. The \c AcceptPattern property names a lit of bits required for
 * acceptance, while \c RejectPattern names a list of bits required to 
 * reject the event.
 *
 * Properties:
 *
 * \param PatternUnitName - object key for the pattern unit 
 * (defaults to \c TBTrigPat )
 * \param AcceptPattern   - list of named bits required for acceptance
 * \param RejectPattern   - list of named bits required for rejection
 *
 * \Author Peter Loch
 * \Date   June 20, 2004 - first implementation
 */

#include "TBRec/TBEventStreamerTool.h"

#include <string>
#include <map>
#include <vector>

class TBPatternUnitStreamerTool : public TBEventStreamerTool
{
 public:

  typedef unsigned int                      bit_mask;
  typedef std::map< std::string, bit_mask > bit_mask_store;
  typedef std::string                       pattern;
  typedef std::vector<pattern>              pattern_store;

  TBPatternUnitStreamerTool(const std::string& type,
			    const std::string& name,
			    const IInterface* parent);
  virtual ~TBPatternUnitStreamerTool();

  virtual StatusCode initializeTool();

  virtual StatusCode accept();

 protected:

  ////////////////
  // Properties //
  ////////////////

  std::string    m_patternUnitKey;
  pattern_store  m_acceptPatternNames;
  pattern_store  m_rejectPatternNames;

  /////////////////////
  // Internal Stores //
  /////////////////////

  bit_mask       m_acceptPattern;
  bit_mask       m_rejectPattern;
  bit_mask_store m_triggerBitMap;

  //////////////////////
  // Internal Helpers //
  //////////////////////

  std::string decodeBinary(bit_mask& theTrigger);
  StatusCode  findPattern(const pattern_store&  thePatterns,
			  const bit_mask_store& theBitMasks,
			  bit_mask&             theMask);
};
#endif
