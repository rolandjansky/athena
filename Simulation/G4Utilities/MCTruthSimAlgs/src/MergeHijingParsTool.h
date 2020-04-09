/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGEHIJINGPARSTOOL_H
#define MCTRUTHSIMALGS_MERGEHIJINGPARSTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "GeneratorObjects/HijingEventParams.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include "GaudiKernel/ServiceHandle.h"

#include <string>

/** @class MergeHijingParsTool
 * @brief an algorithm to propagate HijingPars to the overlay store
 *
 *  $Id:
 *  @author DCostanzo@bnl.gov (from Paolo's MergeMcEventCollTool)
 *
 */
class MergeHijingParsTool : public PileUpToolBase {
public:
  MergeHijingParsTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);
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
     /**  @brief Propagate the HijingParsections to the output
          StoreGate
     */
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;
private:
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc", ""};
  SG::WriteHandleKey<HijingEventParams> m_outputObjectKey{this, "HijingParamsKey", "Hijing_event_params", ""};
  SG::WriteHandle<HijingEventParams> m_outputObject;
  bool m_firstSubEvent{true};
};
#endif //MCTRUTHSIMALGS_MERGEHIJINGPARSTOOL_H
