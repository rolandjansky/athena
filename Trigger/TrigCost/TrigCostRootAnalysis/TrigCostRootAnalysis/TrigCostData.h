// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_TrigCostData_H
#define TrigCostRootAnalysis_TrigCostData_H

// STL include(s):
#include <string>
#include <map>
#include <set>
#include <utility>
#include <mutex>

// ROOT include(s):
#include <Rtypes.h>

//D3PD Reader include(s):
#include "TrigCostD3PD/TrigCostD3PDObject.h"

// Local include(s):
#include "Utility.h" //Typedefs
#include "Timer.h"

//Forward declaration
class TTree;

namespace TrigCostRootAnalysis {
  //Forward declaration
  class ProcessEvent;

  /**
   * @class TrigCostData
   * Wrapper class for a TrigCostD3PDObject, provides more sensible access operators and iterators.
   * Can compute additional monitoring variables using the D3PD members. These are calculated
   * transparently on-the-fly when requested, and buffered for the event to be efficient if the quantity
   * is requested from different locations.
   */
  class TrigCostData {
  public:
    TrigCostData(const Long64_t& master, const char* prefix, TTree* tree);
    TrigCostData();
    ~TrigCostData();
    void setup(const Long64_t& master, const char* prefix, TTree* tree);

    Int_t getEventNumber() const;
    Float_t getEBWeight() const;
    UInt_t getEBWeightBG() const;
    Bool_t getEBUnbiased() const;
    Bool_t getIsMonitoringEvent() const;
    Int_t getRunNumber() const;
    Int_t getLumi() const;
    Float_t getLumiLength() const;
    UInt_t getAppId() const;
    Float_t getCostEvent() const;
    UInt_t getCostRunSec() const;
    UInt_t getCostRunNsec() const;
    Float_t getTimerTrigCost() const;
    Float_t getTimerEndSteer() const;
    Float_t getTimerChainProcessed() const;
    Float_t getTimerResultBuilder() const;
    Float_t getTimerMonitoring() const;
    UInt_t getBunchCrossingId() const;

    //
    // CHAIN VARIABLES
    //

    UInt_t getNChains() const;
    Int_t getChainID(UInt_t n) const;
    Int_t getChainLevel(UInt_t n) const;
    Bool_t getIsChainPassed(UInt_t n) const;
    Bool_t getIsChainPassedRaw(UInt_t n) const;
    Bool_t getIsChainPassthrough(UInt_t n) const;
    Bool_t getIsChainResurrected(UInt_t n) const;
    Bool_t getIsChainPrescaled(UInt_t n) const;
    Bool_t getIsChainExpressStream(UInt_t n) const;
    Bool_t getIfChainWasL1AfterVeto(UInt_t n) const;
    Bool_t getIfChainWasL1BeforePrescale(UInt_t n) const;
    Bool_t getIfChainWasL1AfterPrescale(UInt_t n) const;
    Float_t getChainTimer(UInt_t n) const;
    // Cached
    Float_t getChainTimerFromSequences(UInt_t n) const; //TODO make this private and use only if ChainTimer == 0
    UInt_t getChainAlgCalls(UInt_t n) const;
    UInt_t getChainAlgCaches(UInt_t n) const;
    const std::set< std::pair<Int_t, Int_t> >& getChainAlgs(UInt_t n) const;
    UInt_t getChainSeqCalls(UInt_t n) const;
    Bool_t getIsChainPassed(std::string& n) const;
    // Cached ROS
    UInt_t getChainROBRetrievals(UInt_t n) const;
    Float_t getChainROBRetrievalSize(UInt_t n) const;
    UInt_t getChainROBRequests(UInt_t n) const;
    Float_t getChainROBRequestSize(UInt_t n) const;

    //
    // SEQUENCE VARIABLES
    //

    UInt_t getNSequences() const;
    Int_t getSequenceIndex(UInt_t n) const;
    Int_t getSequenceChannelCounter(UInt_t n) const;
    Int_t getSequenceLevel(UInt_t n) const;
    Float_t getSequenceAlgTotalTime(UInt_t n) const;
    Float_t getSequenceTime(UInt_t n) const;
    Bool_t getIsSequenceAlreadyExecuted(UInt_t n) const;
    Bool_t getIsSequenceExecuted(UInt_t n) const;
    Bool_t getIsSequenceInitial(UInt_t n) const;
    Bool_t getIsSequencePrevious(UInt_t n) const;
    // Cached
    UInt_t getSequenceAlgCalls(UInt_t n) const;
    UInt_t getSequenceAlgCaches(UInt_t n) const;
    //
    UInt_t getSeqROSCalls(UInt_t n) const;
    Float_t getSeqROSTime(UInt_t n) const;
    UInt_t getSeqROBRequests(UInt_t n) const;
    Float_t getSeqROBRequestSize(UInt_t n) const;
    UInt_t getSeqROBRetrievals(UInt_t n) const;
    Float_t getSeqROBRetrievalSize(UInt_t n) const;
    UInt_t getSeqROBOthers(UInt_t n) const;
    //
    Bool_t getSeqIsRerun(UInt_t n) const;


    //
    // SEQUENCE ---> ALGORITHM VARIABLES
    //

    UInt_t getNSeqAlgs(UInt_t n) const;
    Float_t getSeqAlgTimer(UInt_t n, UInt_t a) const;
    Float_t getSeqAlgTimeStart(UInt_t n, UInt_t a) const;
    UInt_t getSeqAlgTimeStartSec(UInt_t n, UInt_t a) const;
    UInt_t getSeqAlgTimeStartMicroSec(UInt_t n, UInt_t a) const;
    Float_t getSeqAlgTimeStop(UInt_t n, UInt_t a) const;
    UInt_t getSeqAlgTimeStopSec(UInt_t n, UInt_t a) const;
    UInt_t getSeqAlgTimeStopMicroSec(UInt_t n, UInt_t a) const;
    Int_t getSeqAlgPosition(UInt_t n, UInt_t a) const;
    Int_t getSeqAlgNRoI(UInt_t n, UInt_t a) const;
    Bool_t getSeqAlgIsCached(UInt_t n, UInt_t a) const;
    Bool_t getSeqAlgIsCalled(UInt_t n, UInt_t a) const;
    Int_t getSeqAlgRoIID(UInt_t n, UInt_t a, UInt_t roi) const;
    Int_t getSeqAlgRoILocation(UInt_t n, UInt_t a, UInt_t roi) const;
    // Cached
    std::set<Int_t> getSeqAlgROBLocations(UInt_t n, UInt_t a) const;
    std::set<Int_t> getSeqAlgROBLocations(const std::pair<Int_t, Int_t>& algLocation) const;
    UInt_t getSeqAlgROSCalls(UInt_t n, UInt_t a) const;
    Float_t getSeqAlgROSTime(UInt_t n, UInt_t a) const;
    UInt_t getSeqAlgROBRequests(UInt_t n, UInt_t a) const;
    Float_t getSeqAlgROBRequestSize(UInt_t n, UInt_t a) const;
    UInt_t getSeqAlgROBRetrievals(UInt_t n, UInt_t a) const;
    Float_t getSeqAlgROBRetrievalSize(UInt_t n, UInt_t a) const;
    UInt_t getSeqAlgROBOthers(UInt_t n, UInt_t a) const;

    //
    // LEVEL 1 VARIABLES
    //

    UInt_t getNL1() const;
    UInt_t getL1CtpId(UInt_t n) const;
    Bool_t getIsL1Prescaled(UInt_t n) const;
    Bool_t getIsL1Vetoed(UInt_t n) const;
    Bool_t getIsL1Passed(UInt_t n) const;
    Bool_t getIsL1PassedAfterPrescale(UInt_t n) const;
    Bool_t getIsL1PassedBeforePrescale(UInt_t n) const;
    Bool_t getIsL1PassedAfterVeto(UInt_t n) const;
    // Cached
    Bool_t getIsL1PassedBeforePrescale(std::string& n) const;
    Int_t getL1Location(const std::string& n) const;

    //
    // READ OUT BUFFER VARIABLES
    //

    UInt_t getNROBs() const;
    UInt_t getROBReqID(UInt_t n) const;
    Float_t getROBTimer(UInt_t n) const;
    UInt_t getROBTimeStartSec(UInt_t n) const;
    UInt_t getROBTimeStartMicroSec(UInt_t n) const;
    UInt_t getROBTimeStopSec(UInt_t n) const;
    UInt_t getROBTimeStopMicroSec(UInt_t n) const;
    UInt_t getROBDataN(UInt_t n) const;
    UInt_t getROBDataID(UInt_t n, UInt_t r) const;
    Float_t getROBDataSize(UInt_t n, UInt_t r) const;
    Bool_t getIsROBDataCached(UInt_t n, UInt_t r) const;
    Bool_t getIsROBDataDisabled(UInt_t n, UInt_t r) const;
    Bool_t getIsROBDataIgnored(UInt_t n, UInt_t r) const;
    Bool_t getIsROBDataRetrieved(UInt_t n, UInt_t r) const;
    Bool_t getIsROBDataStatusOK(UInt_t n, UInt_t r) const;
    Bool_t getIsROBDataStatusPrefetched(UInt_t n, UInt_t r) const;
    Bool_t getIsROBDataUnclassified(UInt_t n, UInt_t r) const;
    UInt_t getROBSumN(UInt_t n) const;
    UInt_t getROBSumDetID(UInt_t n, UInt_t r) const;
    UInt_t getROBSumNROBs(UInt_t n, UInt_t r) const;
    Float_t getROBSumSize(UInt_t n, UInt_t r) const;
    Bool_t getIsROBSumCached(UInt_t n, UInt_t r) const;
    Bool_t getIsROBSumDisabled(UInt_t n, UInt_t r) const;
    Bool_t getIsROBSumIgnored(UInt_t n, UInt_t r) const;
    Bool_t getIsROBSumRetrieved(UInt_t n, UInt_t r) const;
    Bool_t getIsROBSumUnclassified(UInt_t n, UInt_t r) const;
    // Cached
    const std::pair< Int_t, Int_t >& getROBAlgLocation(UInt_t n) const;

    //
    // RoI VARIABLES
    //

    UInt_t getNRoIs() const;
    Int_t getRoIIndexFromId(Int_t id) const;
    Int_t getRoIID(Int_t n) const;
    Float_t getRoIArea(UInt_t n) const;
    Float_t getRoIEta(UInt_t n) const;
    Float_t getRoIPhi(UInt_t n) const;
    Float_t getRoIEt(UInt_t n) const;
    Int_t getRoINL1Thresh(UInt_t n) const;
    Bool_t getIsRoIEmTau(UInt_t n) const;
    Bool_t getIsRoITau(UInt_t n) const;
    Bool_t getIsRoIEnergy(UInt_t n) const;
    Bool_t getIsRoIJet(UInt_t n) const;
    Bool_t getIsRoIJetEt(UInt_t n) const;
    Bool_t getIsRoIMuon(UInt_t n) const;
    Bool_t getIsRoINone(UInt_t n) const;
    Float_t getRoIEtLarge(UInt_t n) const;
    Int_t getRoIMuonCharge(UInt_t n) const;
    Int_t getRoIEmTauIsoBits(UInt_t n) const;
    Float_t getRoIVectorEX(UInt_t n) const;
    Float_t getRoIVectorEY(UInt_t n) const;
    Bool_t getRoIOverflowEX(UInt_t n) const;
    Bool_t getRoIOverflowEY(UInt_t n) const;
    Bool_t getRoIOverflowET(UInt_t n) const;
    const std::string& getRoITypeString(Int_t n) const;

    //
    // TRIGGER ELEMENT VARIABLES
    //

    UInt_t getNTEs() const;
    UInt_t getTEIndex(UInt_t n) const;
    UInt_t getTEID(UInt_t n) const;
    UInt_t getTEChildIndex(UInt_t n, UInt_t c) const;
    UInt_t getTEParentIndex(UInt_t n, UInt_t c) const;
    UInt_t getTECLIDIndex(UInt_t n, UInt_t c) const;
    UInt_t getTERoIIDIndex(UInt_t n, UInt_t c) const;
    UInt_t getTEChildSize(UInt_t n) const;
    UInt_t getTEParentSize(UInt_t n) const;
    UInt_t getTECLIDSize(UInt_t n) const;
    UInt_t getTERoIIDSize(UInt_t n) const;
    Int_t getTEPositionFromTEIndex(UInt_t n) const;
    Bool_t getIsTEActiveState(UInt_t n) const;
    Bool_t getIsTEErrorState(UInt_t n) const;
    Bool_t getIsTEInitial(UInt_t n) const;
    Bool_t getIsTEL1Threshold(UInt_t n) const;
    Bool_t getIsTEOutputEFNode(UInt_t n) const;
    Bool_t getIsTEOutputL2Node(UInt_t n) const;
    Bool_t getIsTERegularTe(UInt_t n) const;
    Bool_t getIsTERoITe(UInt_t n) const;
    Bool_t getIsTETerminalNode(UInt_t n) const;
    Bool_t getIsTETopologicalTe(UInt_t n) const;

    //
    // RATES ANALYSIS CHAIN PRESCALE WEIGHTS
    //
    void setChainPrescaleWeight(const std::string& name, Double_t value) const;
    Double_t getChainPrescaleWeight(const std::string& name) const;

    // Debug
    ProcessEvent* getParent() const;
    void setParent(ProcessEvent* parent) const;

    void bufferEvent() const;
  private:
    // Forbid copying
    TrigCostData& operator = (const TrigCostData&);
    TrigCostData(const TrigCostData&);

    // These variables are used to buffer information which is calculated on request from the D3PD
    // Buffer based on event number
    void checkBuffers() const;
    void mapRosToAlgs() const;
    void mapChainAndSeqToAlg() const;
    void bufferSequenceAlgCallsCaches() const;
    void bufferChainSeqTime() const;
    void bufferChainAlgCalls() const;
    void bufferChainPassed() const;
    void bufferIsL1PassedBeforePrescale() const;
    void bufferChainRosInformation() const;
    void bufferSeqROSInformation() const;
    void bufferAlgRosInformation() const;
    Bool_t getRosReqBelongsToAlg(UInt_t seq, UInt_t alg, UInt_t ros) const;

    mutable ProcessEvent* m_parent;
    mutable std::mutex m_mutex;

    mutable Int_t m_bufferEventNumber; //!< The event being buffered

    mutable Bool_t m_hasLumiData; //!< Temp flag for if file has lumi block data

    mutable std::map< Int_t, Float_t > m_chainSeqTimeMap; //!< Chain time for all sequences
    mutable std::map< Int_t, UInt_t > m_chainSeqCallMap; //!< Chain Sequence calls
    mutable std::map< std::string, Bool_t > m_chainPassMap; //!< Chain pass map
    mutable std::map< std::string, Bool_t > m_L1BeforePrescalePassMap; //!< Chain pass map

    mutable std::map< Int_t, UInt_t > m_chainAlgCallMap; //!< Algorithm calls per chain
    mutable std::map< Int_t, UInt_t > m_chainAlgCacheMap; //!< Algorithm caches per chain
    mutable std::map< Int_t, Float_t > m_chainAlgTimeMap; //!< Algorithm time per chain

    mutable std::map< Int_t, Bool_t > m_chainRerunStatus; //!< Rerun bit for a chain ID

    mutable std::map< Int_t, std::set< std::pair<Int_t, Int_t> > > m_chainAlgMap; //!< Chain ID to set of algorithm
                                                                                  // locations in current event
    mutable std::map< Int_t, std::set< std::pair<Int_t, Int_t> > > m_seqAlgMap; //!< Sequence ID to set of algorithm
                                                                                // locations in current event

    mutable std::map< UInt_t, Int_t > m_chainROBRequests; //!< Chain ID to number of cached ROB requests by chain
                                                          // algorithms
    mutable std::map< UInt_t, Int_t > m_chainROBRetrievals; //!< Chain ID to number of ROB retrievals by chain
                                                            // algorithms
    mutable std::map< UInt_t, Float_t > m_chainROBRequestSize; //!< Chain ID to size of cached ROB requests by chain
                                                               // algorithms
    mutable std::map< UInt_t, Float_t > m_chainROBRetrievalSize; //!< Chain ID to size of ROB retrievals by chain
                                                                 // algorithms

    mutable IntIntMap_t m_seqAlgCallMap; //!< Algorithm calls per sequence
    mutable IntIntMap_t m_seqAlgCacheMap; //!< Algorithm caches per sequence

    mutable std::map< Int_t, std::pair< Int_t, Int_t > > m_rosAlgMap; //!< ROS location to algorithm location
                                                                      // (one-to-one)
    mutable std::map< std::pair< Int_t, Int_t >, std::set<Int_t> > m_algRosMap; //!< Reverse algorithm location to ROS
                                                                                // location (one-to-many)
    mutable std::map< UInt_t, std::set< std::pair<UInt_t, UInt_t> > > m_algExecMap; //!< Map of alg name hash to D3PD
                                                                                    // location of run instances in an
                                                                                    // event (internal).

    mutable std::map< std::pair< Int_t, Int_t >, Int_t >   m_algRosCalls; //!< Number of calls an algorithm makes to the
                                                                          // ROS
    mutable std::map< std::pair< Int_t, Int_t >, Float_t > m_algRosTime; //!< Time taken for all ROS calls requested by
                                                                         // an alg
    mutable std::map< std::pair< Int_t, Int_t >, Int_t >   m_algROBRequests; //!< Number of ROBs requested by an alg
                                                                             // (cached)
    mutable std::map< std::pair< Int_t, Int_t >, Int_t >   m_algROBRetrievals; //!< Number of ROBs retrieved by an alg
    mutable std::map< std::pair< Int_t, Int_t >, Int_t >   m_algROBOther; //!< Number of 'other' (unknown, ignored,
                                                                          // disabled) ROBs requested by an alg
    mutable std::map< std::pair< Int_t, Int_t >, Float_t > m_algROBRequestSize; //!< Size in kB of all ROB fragments
                                                                                // requested by an alg (cached)
    mutable std::map< std::pair< Int_t, Int_t >, Float_t > m_algROBRetrievalSize; //!< Size in kB of all ROB fragments
                                                                                  // retrieved by an alg

    mutable IntIntMap_t m_seqROSCallMap; //!< Number of calls to the ROS by chains in a seq
    mutable IntFloatMap_t m_seqROSTimeMap; //!< Time to get all ROBs of algs in a seq
    mutable IntIntMap_t m_seqROBRets; //<! Number of ROB retrievals by algs in a seq
    mutable IntIntMap_t m_seqROBReqs; //!< Nuber of cached ROBs used by algs in a seq
    mutable IntFloatMap_t m_seqROBRetSize; //!< Size in kB of ROB retrievals by algs in a seq
    mutable IntFloatMap_t m_seqROBReqSize; //!< Size ib kB of cached ROBs used by algs in a seq
    mutable IntIntMap_t m_seqROBOther; //<! Number of [unknown, disabled, ignored] ROBs requested by algs in a seq

    mutable StringDoubleMap_t m_chainPSWeight; //!< Holds chain prescale weights for use in other monitors

    const std::set<Int_t> m_emptySet; //!< Returned for cases where an alg has no ROBs
    mutable std::pair< Int_t, Int_t > m_recyclablePair; // Re-usable pair object

    mutable Bool_t m_rosMatching; //!< Match ROS to algorithms? Time consuming, only use when needed

    D3PDReader::TrigCostD3PDObject* m_trigCostObject; //!< The wrapped D3PDReader object
  }; //class TrigCostData
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_TrigCostData_H
