/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_NEWMERGEMCEVENTCOLLTOOL_H
#define MCTRUTHSIMALGS_NEWMERGEMCEVENTCOLLTOOL_H

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
/** @class NewMergeMcEventCollTool
 *  @brief a PileUpTool to merge MC truth collection in the overlay store
 *
 *  $Id:
 *  @author jchapman@cern.ch
 *
 */
class NewMergeMcEventCollTool : public PileUpToolBase {
public:
  NewMergeMcEventCollTool(const std::string& type,
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
  //** Prepare the required output McEventCollections
  StatusCode prepareOutputMcEventCollections(const int pileUpType);
  //** Add the required information from the current GenEvent to the output McEventCollection
  StatusCode processEvent(const McEventCollection *pMcEvtColl, const int& dsid, PileUpTimeEventIndex::time_type timeOffset=0);
  //** Print out detailed debug info if required.
  void printDetailsOfMergedMcEventCollection() const;
  //** Handle for the PileUpMergeSvc (provides input McEventCollections)
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc;
  //** New McEventCollections to be written out to file
  std::map<int,McEventCollection*> m_outputMcEventCollectionMap; // FIXME avoid hard-coding in General
  //** Do we expect to need a McEventCollection for low-pT pileup events ?
  bool m_expectLowPtMinBiasBackgroundCollection;
  //** Do we expect to need a McEventCollection for high-pT pileup events ?
  bool m_expectHighPtMinBiasBackgroundCollection;
  //** Name of input McEventCollection
  StringProperty m_truthCollKey;
  //** Bool to indicate that the next GenEvent is a new signal event
  bool m_newevent;
  //** The total number of GenEvents that will be passed for the current signal event
  unsigned int m_nInputMcEventColls;
  //** How many background events have been read so far for this signal event
  unsigned int m_nBkgEventsReadSoFar;
  //** Depends on PileUpTimeEventIndex::PileUpType; provide one instance of this tool for each type
  unsigned int m_pileUpType;
  //** Writing to StoreGate safely in MT
  SG::WriteHandleKey<McEventCollection> m_wrhMcEventCollection;
};
#endif //MCTRUTHSIMALGS_NEWMERGEMCEVENTCOLLTOOL_H
