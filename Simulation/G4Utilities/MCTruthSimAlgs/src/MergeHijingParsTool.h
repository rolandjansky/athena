/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGEHIJINGPARSTOOL_H
#define MCTRUTHSIMALGS_MERGEHIJINGPARSTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>

class PileUpMergeSvc;

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
  ///called before the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode prepareEvent(unsigned int nInputEvents);
  ///called at the end of the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode mergeEvent();
  ///called for each active bunch-crossing to process current SubEvents
  /// bunchXing is in ns
  virtual StatusCode
    processBunchXing(int bunchXing,
                     PileUpEventInfo::SubEvent::const_iterator bSubEvents,
                     PileUpEventInfo::SubEvent::const_iterator eSubEvents);
  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  virtual bool toProcess(int bunchXing) const;
     /**  @brief Propagate the HijingParsections to the output
          StoreGate
     */
  virtual StatusCode processAllSubEvents();
private:
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc;
  StringProperty m_key;
  bool m_firstSubEvent;
};
#endif //MCTRUTHSIMALGS_MERGEHIJINGPARSTOOL_H
