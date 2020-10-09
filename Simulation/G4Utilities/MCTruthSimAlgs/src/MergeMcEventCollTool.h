/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGEMCEVENTCOLLTOOL_H
#define MCTRUTHSIMALGS_MERGEMCEVENTCOLLTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <utility> /* pair */

class McEventCollection;

#include "AtlasHepMC/GenParticle_fwd.h"
#include "AtlasHepMC/GenVertex_fwd.h"
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
  //  virtual bool toProcess(int bunchXing) const;

  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

private:
  //** The types used to classify the events. NOPUTYPE is used both as the number of types, and to flag a reject (no type)
  typedef enum puType { INTIME, OUTOFTIME, RESTOFMB, CAVERN, NOPUTYPE } puType;

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
  //** Classify the current GenParticle according to the MC Truth Taskforce requirements
  MergeMcEventCollTool::puType classifyVertex(const HepMC::GenParticlePtr  pCurrentVertexParticle, const HepMC::GenVertexPtr  pCurrentParticleProductionVertex, double currentEventTime);
  //** Check if the current GenVertex contains beam particles
  bool isInitialCollisionVertex(const HepMC::GenVertexPtr  pCurrentVertex) const;
  //** Check whether the current McEventCollection has already been truth-filtered
  bool isTruthFiltertedMcEventCollection(const McEventCollection *pMcEvtColl) const;
  //** Map from GenEvent to puType FIXME: Simpler to key the map on GenEvent* ?
  typedef std::map<std::pair<int,int>, int> PileUpBackgroundMap;
  PileUpBackgroundMap m_backgroundClassificationMap;
  //** Update the map of GenEvent->puType
  void updateClassificationMap(int signal_process_id, int event_number, int hack, int classification, bool firstUpdateForThisEvent);

  ServiceHandle<PileUpMergeSvc> m_pMergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc", "Handle for the PileUpMergeSvc (provides input McEventCollections)"};
  //** New McEventCollection to be written out to file
  McEventCollection* m_pOvrlMcEvColl{};
  StringProperty m_truthCollInputKey{this, "TruthCollInputKey", "TruthEvent", "Name of input McEventCollection"};
  StringProperty m_truthCollOutputKey{this, "TruthCollOutputKey", "TruthEvent", "Name of output McEventCollection"};
  BooleanProperty m_keepUnstable{this, "KeepUnstable", false, "Do not cut unstable particles"};
  DoubleProperty m_absEtaMax{this, "AbsEtaMax", 5.0, "Eta cut-off for INTIME GenParticles"};
  DoubleProperty m_absEtaMax_outOfTime{this,"OutOfTimeAbsEtaMax", 3.0, "Eta cut-off for OUTOFTIME GenParticles"};
  DoubleProperty m_lowTimeToKeep{this, "LowTimeToKeep", -51.0, "leading edge in ns of the time window to keep if SaveOutOfTimePileUp is true"};
  DoubleProperty m_highTimeToKeep{this, "HighTimeToKeep", 51.0, "trailing edge in ns of the time window to keep if SaveOutOfTimePileUp is true"};
  //** Radius acceptance cut on production vertex of all stable particles (in mm)
  DoubleProperty m_rRange{this, "rRange", 20.0*Gaudi::Units::mm, "rRange of production vertex in mm"};
  //** Radius acceptance squared to speed up cut
  double m_r2Range{400.0};
  //** Cut on Z coordinate of production vertex in mm, to distinguish between minbias and cavern background categories
  DoubleProperty m_zRange{this, "zRange", 200.0*Gaudi::Units::mm, "z range of production vertex in mm"};
  DoubleProperty m_ptMin{this, "ptMin", 0.4*Gaudi::Units::GeV, "Minimum threshold for pT for selected pileup particles in MeV"};
  DoubleProperty m_minKinE{this, "EKinMin", 1.0*Gaudi::Units::MeV, "Minimum threshold for Kinetic Energy of selected cavern events in MeV"};
  //** save minimum bias events in the t0 bunch xing
  BooleanProperty m_saveInTimePileup{this, "SaveInTimeMinBias", true, "save min bias particles in the t0 xing"};
  //** save minbias events which are "close" by (within [ m_lowTimeToKeep, m_highTimeToKeep ])
  BooleanProperty m_saveOutOfTimePileup{this, "SaveOutOfTimeMinBias", true, "save out of time min bias particles in the [LowTimeToKeep:HighTimeToKeep] range"};
  //** save as well the rest of minbias
  BooleanProperty m_saveRestOfPileup{this, "SaveRestOfMinBias", false, "save the rest of out of time min bias particles as well"};
  //** save as well the cavern background
  BooleanProperty m_saveCavernBackground{this, "SaveCavernBackground", true, "save the cavern background as well"};
  //** do the slimming - flag to do slimming or not
  BooleanProperty m_doSlimming{this, "DoSlimming", true, "flag to do the slimming or save everything"};
  //** Should any details of GenEvents corresponding to each puType be saved?
  bool m_saveType[NOPUTYPE];
  //** The index (before sorting) within the McEventCollection where the background events start
  unsigned int m_startingIndexForBackground{0};
  //** Bool to indicate that the next GenEvent is a new signal event
  bool m_newevent{true};
  //** The total number of GenEvents that will be passed for the current signal event
  unsigned int m_nInputMcEventColls{0};
  //** How many background events have been read so far for this signal event
  unsigned int m_nBkgEventsReadSoFar{0};
  //** Ensure that the collision GenVertex objects of minbias background events are saved.
  BooleanProperty m_addBackgroundCollisionVertices{this, "AddBackgroundCollisionVertices", true, "ensure that the collision GenVertex objects of minbias background events are saved."};
  //** Should empty GenEvents be removed from the output McEventCollection?
  BooleanProperty m_compressOutputCollection{this, "CompressOutputCollection", false, "Remove all empty GenEvents from the output McEventCollection"};
  //** Just save the Signal GenEvent
  BooleanProperty m_onlySaveSignalTruth{this, "OnlySaveSignalTruth", false, "Just save the Signal GenEvent"};
  //** Temporary store for the true signal event number
  int m_signal_event_number{0};

};
#endif //MCTRUTHSIMALGS_MERGEMCEVENTCOLLTOOL_H
