/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBPATTERNUNITSTREAMERTOOLH8_H
#define TBREC_TBPATTERNUNITSTREAMERTOOLH8_H

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
 * \author Monika Wielers
 * \date   July 28, 2004 - copy of TBPatternUnitStreamerTool adapted for H8
 */

#include "TBEventStreamerTool.h"

#include <string>
#include <map>
#include <vector>

class TBPatternUnitStreamerToolH8 : public TBEventStreamerTool
{
 public:

  typedef unsigned int                      bit_mask;
  typedef std::map< std::string, bit_mask > bit_mask_store;
  typedef std::string                       pattern;
  typedef std::vector<pattern>              pattern_store;

  TBPatternUnitStreamerToolH8(const std::string& type,
			    const std::string& name,
			    const IInterface* parent);
  virtual ~TBPatternUnitStreamerToolH8();

  virtual StatusCode initializeTool() override;

  virtual StatusCode accept() override;

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
