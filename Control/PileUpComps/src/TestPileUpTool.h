/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_TESTPILEUPTOOL_H
#define PILEUPCOMPS_TESTPILEUPTOOL_H
/** @file TestPileUpTool.h
 * @brief a sample implementation of IPileUpTool to test the framework
 * $Id: PileUpStream.h,v 1.18 2008-10-31 18:34:42 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */
#include <vector>
#include <utility> /* pair */

#include "PileUpTools/PileUpToolBase.h"
class TestPileUpTool : public PileUpToolBase
{
public:
  /// \name Constructor
  //@{
  TestPileUpTool(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);
  //@}
  /// \name IPileUpTool methods
  //@{
  ///called before the bunchXing loop
  virtual StatusCode prepareEvent(unsigned int nInputEvents);
  ///called at the end of the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode mergeEvent();
  ///called for each active bunch-crossing to process current SubEvents
  /// bunchXing is in ns
  virtual StatusCode
  processBunchXing(int bunchXing,
                   SubEventIterator bSubEvents,
                   SubEventIterator eSubEvents);
  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //@}
private:
  std::vector<std::pair<unsigned int, int> > m_seen;
  bool m_doFiltering;
  unsigned int m_maxInTimePileUp;
};
#endif
