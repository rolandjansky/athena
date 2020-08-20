/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGERECTIMINGOBJTOOL_H
#define MCTRUTHSIMALGS_MERGERECTIMINGOBJTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include <string>

class RecoTimingObj;
/** @class MergeRecoTimingObjTool
 *  @brief an algorithm to merge MC track record collTool in the overlay store
 *
 *  $Id:
 *  @author DCostanzo@bnl.gov (from Paolo's MergeMcEventCollTool)
 *
 */
class MergeRecoTimingObjTool : public PileUpToolBase {
public:
  MergeRecoTimingObjTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);
  ///
  virtual StatusCode initialize() override final;
  ///called before the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode prepareEvent(const EventContext& ctx, unsigned int nInputEvents) override final;
  ///called at the end of the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode mergeEvent(const EventContext& ctx) override final;
  ///called for each active bunch-crossing to process current SubEvents
  /// bunchXing is in ns
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;
  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  virtual bool toProcess(int bunchXing) const override final;
  /// Merge the RecoTimingObj
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;
private:
  /// share code between two approaches
  virtual StatusCode processRecoTimingObj(const RecoTimingObj *inputObj);
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc", ""};
  StringProperty m_recTimingObjInputKey{this, "RecoTimingObjInputKey", "EVNTtoHITS_timings", ""};
  StringProperty m_recTimingObjOutputKey{this, "RecoTimingObjOutputKey", "EVNTtoHITS_timings", ""};
  bool m_firstSubEvent{true};
};
#endif
