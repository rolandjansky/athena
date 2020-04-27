/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGECALIBHITSTOOL_H
#define MCTRUTHSIMALGS_MERGECALIBHITSTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "GaudiKernel/ServiceHandle.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include <vector>
#include <string>

class CaloCalibrationHitContainer;

/** @class MergeCalibHitsTool
 *  @brief an algorithm to copy in the overlayed store the calib hits from the signal event
 *
 *  $Id:
 *  @author Guillaume Unal (from Paolo's MergeMcEventCollection)
 *
 */
class MergeCalibHitsTool : public PileUpToolBase {
public:
  MergeCalibHitsTool(const std::string& type,
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

  /**  @brief Propagate the Calib Hit Containers to the output
       StoreGate
  */
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

private:
  Gaudi::Property<bool> m_oldFormat{this, "OldFormat", false, ""};
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc", ""};
  bool m_firstSubEvent{true};
  std::vector<CaloCalibrationHitContainer*> m_outputContainers;//FIXME hardcoded size
  std::vector<std::string> m_CalibrationHitContainer;
};
#endif //MCTRUTHSIMALGS_MERGECALIBHITSTOOL_H
