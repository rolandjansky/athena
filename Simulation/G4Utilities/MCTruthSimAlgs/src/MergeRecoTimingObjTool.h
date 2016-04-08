/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGERECTIMINGOBJTOOL_H
#define MCTRUTHSIMALGS_MERGERECTIMINGOBJTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>

class PileUpMergeSvc;
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
  ///called before the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode prepareEvent(unsigned int nInputEvents) override final;
  ///called at the end of the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode mergeEvent() override final;
  ///called for each active bunch-crossing to process current SubEvents
  /// bunchXing is in ns
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;
  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  virtual bool toProcess(int bunchXing) const override final;
  /// Merge the RecoTimingObj
  virtual StatusCode processAllSubEvents() override final;
private:
  /// share code between two approaches
  virtual StatusCode processRecoTimingObj(const RecoTimingObj *inputObj);
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc;
  StringProperty m_recTimingObjKey;
  bool m_firstSubEvent;
};
#endif
