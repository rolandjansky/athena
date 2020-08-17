/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGETRACKRECORDCOLLTOOL_H
#define MCTRUTHSIMALGS_MERGETRACKRECORDCOLLTOOL_H

#include "PileUpTools/PileUpToolBase.h"
#include "TrackRecord/TrackRecordCollection.h"

#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include <string>

/** @class MergeTrackRecordCollTool
 *  @brief an algorithm to merge MC track record collTool in the overlay store
 *
 *  $Id:
 *  @author DCostanzo@bnl.gov (from Paolo's MergeMcEventCollTool)
 *
 */
class MergeTrackRecordCollTool : public PileUpToolBase {
public:
  MergeTrackRecordCollTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);
  /// Initialize tool
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
     /**  @brief Propagate the TrackRecordCollections to the output
          StoreGate
     */
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;
private:
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc", ""};
  StringProperty m_trRecCollKey{this, "TrackRecordCollKey", "MuonEntryLayer", ""};
  SG::WriteHandleKey<TrackRecordCollection> m_outputKey{ this, "TrackRecordCollOutputKey", "MuonExitLayer", "" };
  bool m_firstSubEvent{true};
};
#endif //MCTRUTHSIMALGS_MERGETRACKRECORDCOLLTOOL_H
