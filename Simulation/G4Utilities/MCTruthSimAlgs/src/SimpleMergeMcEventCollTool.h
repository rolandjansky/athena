/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_SIMPLEMERGEMCEVENTCOLLTOOL_H
#define MCTRUTHSIMALGS_SIMPLEMERGEMCEVENTCOLLTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include <utility> /* pair */

class McEventCollection;
class StoreGateSvc;
class PileUpMergeSvc;

namespace HepMC {
  class GenParticle;
  class GenVertex;
}
/** @class SimpleMergeMcEventCollTool
 *  @brief a PileUpTool to merge MC truth collection in the overlay store
 *
 *  $Id:
 *  @author jchapman@cern.ch
 *
 */
class SimpleMergeMcEventCollTool : public PileUpToolBase {
public:
  SimpleMergeMcEventCollTool(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);
  virtual StatusCode initialize() override final;
  ///called before the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode prepareEvent(unsigned int nInputEvents) override final;
  ///called at the end of the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode mergeEvent() override final;
  ///called for each active bunch-crossing to process current SubEvents
  /// bunchXing is in ns
  virtual StatusCode
    processBunchXing(int /*bunchXing*/,
                     SubEventIterator bSubEvents,
                     SubEventIterator eSubEvents) override final;
  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //  virtual bool toProcess(int bunchXing) const;

  virtual StatusCode processAllSubEvents() override final;

private:
  //** Add the required information from the current GenEvent to the output McEventCollection
  StatusCode processEvent(const McEventCollection *pMcEvtColl, McEventCollection *outputMcEventCollection);
  //** Special case of processEvent for the first (signal) GenEvent
  StatusCode processFirstSubEvent(const McEventCollection *pMcEvtColl, McEventCollection *outputMcEventCollection);
  //** Print out detailed debug info if required.
  void printDetailsOfMergedMcEventCollection(McEventCollection *outputMcEventCollection) const;
  //** Handle for the PileUpMergeSvc (provides input McEventCollections)
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc", ""};
  //** Writing to StoreGate safely in MT
  SG::WriteHandle<McEventCollection> m_outputMcEventCollection{};
  //** Writing to StoreGate safely in MT
  SG::WriteHandleKey<McEventCollection> m_truthCollOutputKey{this, "TruthCollOutputKey","TruthEvent",""};
  //** Name of input McEventCollection
  StringProperty m_truthCollInputKey{"TruthEvent"};
  //** The total number of GenEvents that will be passed for the current signal event
  unsigned int m_nInputMcEventColls{0};
  //** How many background events have been read so far for this signal event
  unsigned int m_nBkgEventsReadSoFar{0};
};
#endif //MCTRUTHSIMALGS_SIMPLEMERGEMCEVENTCOLLTOOL_H
