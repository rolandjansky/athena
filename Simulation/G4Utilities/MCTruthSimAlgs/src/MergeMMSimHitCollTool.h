/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGEMMSIMHITCOLLTOOL_H
#define MCTRUTHSIMALGS_MERGEMMSIMHITCOLLTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "MuonSimEvent/MMSimHitCollection.h"

#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>

class PileUpMergeSvc;
/** @class MergeMMSimHitCollTool
 *  @brief an algorithm to merge MC track record collTool in the overlay store
 *
 *  $Id:
 *  @author DCostanzo@bnl.gov (from Paolo's MergeMcEventCollTool)
 *
 */
class MergeMMSimHitCollTool : public PileUpToolBase {
public:
  MergeMMSimHitCollTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);
  virtual ~MergeMMSimHitCollTool();
  ///called before the event loop
  virtual StatusCode initialize() override final;
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
  // virtual bool toProcess(int bunchXing) const;
  /// Merge the MMSimHitColl
  virtual StatusCode processAllSubEvents() override final;
private:
  /// share code between two approaches
  virtual void processMMSimHitColl(const MMSimHitCollection *inputCollection, MMSimHitCollection *outputCollection, const double& timeOfBCID);
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc;
  bool m_firstSubEvent;
  std::vector<MMSimHitCollection*> m_outputContainers;
  StringArrayProperty m_SimHitContainerNamesProp;
  std::vector<std::string> m_SimHitContainerNames;
};
#endif //MCTRUTHSIMALGS_MERGEGENERICMUONSIMHITCOLLTOOL_H
