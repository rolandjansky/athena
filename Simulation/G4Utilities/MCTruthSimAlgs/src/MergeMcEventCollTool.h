/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGEMCEVENTCOLLTOOL_H
#define MCTRUTHSIMALGS_MERGEMCEVENTCOLLTOOL_H

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
/** @class MergeMcEventCollTool
 *  @brief a PileUpTool to merge MC truth collection in the overlay store
 *
 *  $Id:
 *  @author jchapman@cern.ch
 *
 */
class MergeMcEventCollTool : public PileUpToolBase {
public:
  MergeMcEventCollTool(const std::string& type,
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
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;

  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //  virtual bool toProcess(int bunchXing) const;

  virtual StatusCode processAllSubEvents() override final;

private:
  //** Add the required information from the current GenEvent to the output McEventCollection
  StatusCode processEvent(const McEventCollection *pMcEvtColl, const double currentEventTime, const int currentBkgEventIndex);
  //** Special case of processEvent for the first (signal) GenEvent
  StatusCode processFirstSubEvent(const McEventCollection *pMcEvtColl);
  //** Add the required sub-set of the information from the current GenEvent to the output McEventCollection
  StatusCode processUnfilteredEvent(const McEventCollection *pMcEvtColl, const double currentEventTime, const int currentBkgEventIndex);
  //** This is for events which have already been truth-filtered. Add the whole GenEvent to the output McEventCollection
  StatusCode processTruthFilteredEvent(const McEventCollection *pMcEvtColl, const double currentEventTime, const int currentBkgEventIndex);
  //** Remove any empty background GenEvents from the output McEventCollection
  StatusCode compressOutputMcEventCollection();
  //** Print out detailed debug info if required.
  void printDetailsOfMergedMcEventCollection() const;
  //** Ensure that any GenEvent::HeavyIon info is stored in the signal GenEvent.
  StatusCode saveHeavyIonInfo(const McEventCollection *pMcEvtColl);

  //** Handle for the PileUpMergeSvc (provides input McEventCollections)
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc;
  //** New McEventCollection to be written out to file
  McEventCollection* m_pOvrlMcEvColl;
  //** Name of input McEventCollection
  StringProperty m_truthCollKey;
  //** Should unstable particles should be kept?
  BooleanProperty m_keepUnstable;
  //** Eta cut-off for INTIME GenParticles
  DoubleProperty m_absEtaMax;
  //** Eta cut-off for OUTOFTIME GenParticles
  DoubleProperty m_absEtaMax_outOfTime;
  //** Define time range for OUTOFTIME GenParticles
  DoubleProperty m_lowTimeToKeep, m_highTimeToKeep;
  //** Radius acceptance cut on production vertex of all stable particles (in mm)
  DoubleProperty m_rRange;
  //** Radius acceptance squared to speed up cut
  double m_r2Range;
  //** Cut on Z coordinate of production vertex in mm, to distinguish between minbias and cavern background categories
  DoubleProperty m_zRange;
  //** Minimum threshold for pT for selected pileup particles.
  DoubleProperty m_ptMin;
  //** Minimum threshold for Kinetic Energy of selected cavern events
  DoubleProperty m_minKinE;
  //** save minimum bias events in the t0 bunch xing
  BooleanProperty m_saveInTimePileup;
  //** save minbias events which are "close" by (within [ m_lowTimeToKeep, m_highTimeToKeep ])
  BooleanProperty m_saveOutOfTimePileup;
  //** save as well the rest of minbias
  BooleanProperty m_saveRestOfPileup;
  //** save as well the cavern background
  BooleanProperty m_saveCavernBackground;
  //** do the slimming - flag to do slimming or not
  BooleanProperty m_doSlimming;
  //** The types used to classify the events. NOPUTYPE is used both as the number of types, and to flag a reject (no type)
  typedef enum puType { INTIME, OUTOFTIME, RESTOFMB, CAVERN, NOPUTYPE } puType;
  //** Should any details of GenEvents corresponding to each puType be saved?
  bool m_saveType[NOPUTYPE];
  //** The index (before sorting) within the McEventCollection where the background events start
  unsigned int m_startingIndexForBackground;
  //** Bool to indicate that the next GenEvent is a new signal event
  bool m_newevent;
  //** The total number of GenEvents that will be passed for the current signal event
  unsigned int m_nInputMcEventColls;
  //** How many background events have been read so far for this signal event
  unsigned int m_nBkgEventsReadSoFar;
  //** Ensure that the collision GenVertex objects of minbias background events are saved.
  BooleanProperty m_addBackgroundCollisionVertices;
  //** Should empty GenEvents be removed from the output McEventCollection?
  BooleanProperty m_compressOutputCollection;
  //** Just save the Signal GenEvent
  BooleanProperty m_onlySaveSignalTruth;
  //** Temporary store for the true signal event number
  int m_signal_event_number;
  //** Classify the current GenParticle according to the MC Truth Taskforce requirements
  MergeMcEventCollTool::puType classifyVertex(const HepMC::GenParticle *pCurrentVertexParticle, const HepMC::GenVertex *pCurrentParticleProductionVertex, double currentEventTime);
  //** Check if the current GenVertex contains beam particles
  bool isInitialCollisionVertex(const HepMC::GenVertex *pCurrentVertex) const;
  //** Check whether the current McEventCollection has already been truth-filtered
  bool isTruthFiltertedMcEventCollection(const McEventCollection *pMcEvtColl) const;
  //** Map from GenEvent to puType FIXME: Simpler to key the map on GenEvent* ?
  typedef std::map<std::pair<int,int>, int> PileUpBackgroundMap;
  PileUpBackgroundMap m_backgroundClassificationMap;
  //** Update the map of GenEvent->puType
  void updateClassificationMap(int signal_process_id, int event_number, int hack, int classification, bool firstUpdateForThisEvent);
};
#endif //MCTRUTHSIMALGS_MERGEMCEVENTCOLLTOOL_H
