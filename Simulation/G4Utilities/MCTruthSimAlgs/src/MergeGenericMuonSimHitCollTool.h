/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGEGENERICMUONSIMHITCOLLTOOL_H
#define MCTRUTHSIMALGS_MERGEGENERICMUONSIMHITCOLLTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "MuonSimEvent/GenericMuonSimHitCollection.h"

#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>

class PileUpMergeSvc;
/** @class MergeGenericMuonSimHitCollTool
 *  @brief an algorithm to merge MC track record collTool in the overlay store
 *
 *  $Id:
 *  @author DCostanzo@bnl.gov (from Paolo's MergeMcEventCollTool)
 *
 */
class MergeGenericMuonSimHitCollTool : public PileUpToolBase {
public:
  MergeGenericMuonSimHitCollTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);
  ///called before the event loop
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
  // virtual bool toProcess(int bunchXing) const;
  /// Merge the GenericMuonSimHitColl
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;
private:
  /// share code between two approaches
  virtual void processGenericMuonSimHitColl(const GenericMuonSimHitCollection *inputCollection, GenericMuonSimHitCollection *outputCollection, const double& timeOfBCID);
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc;
  bool m_firstSubEvent;
  std::vector<GenericMuonSimHitCollection*> m_outputContainers;
  StringArrayProperty m_SimHitContainerNamesProp;
  std::vector<std::string> m_SimHitContainerNames;
};
#endif //MCTRUTHSIMALGS_MERGEGENERICMUONSIMHITCOLLTOOL_H
