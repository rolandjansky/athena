/* -*- C++ -*- */

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    virtual StatusCode prepareEvent(const EventContext& ctx, unsigned int nInputEvents) override final;
  ///called at the end of the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode mergeEvent(const EventContext& ctx) override final;
  ///called for each active bunch-crossing to process current SubEvents
  /// bunchXing is in ns
  virtual StatusCode
  processBunchXing(int bunchXing,
                   SubEventIterator bSubEvents,
                   SubEventIterator eSubEvents) override final;
  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //@}
private:
  std::vector<std::pair<unsigned int, int> > m_seen;
  Gaudi::Property<bool> m_doFiltering{this, "DoFiltering", false, ""};
  Gaudi::Property<unsigned int> m_maxInTimePileUp{this, "MaxInTimePileUp", 4, ""};
};
#endif
