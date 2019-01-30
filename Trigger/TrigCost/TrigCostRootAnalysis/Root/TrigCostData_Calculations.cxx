// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <map>
#include <utility> //std::pair
#include <iostream>
#include <assert.h>
#include <float.h>

// Local include(s):
#include "../TrigCostRootAnalysis/ProcessEvent.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"

// D3PD Reader include(s):
#include "TrigCostD3PD/TrigCostD3PDObject.h"

// ROOT include(s):
#include <TTree.h>

namespace TrigCostRootAnalysis {
  /**
   * @file TricCostData_Calculations.cxx
   * Some useful quantities are not present in the ntuple and need to be calculated on the fly.
   * The functions in this class do more than simply fetch the requested data straight from the ntuple,
   * rather they calculate and buffer the quantity for the event such that it can be quickly returned to
   * other clients of the data. There is a performance hit to using any of these calls.
   */

  /**
   * We used to buffer-as-we-went. But with multi threaded event execution that just does not work.
   * This function lets us buffer the whole event in one go - as a bonus we get to calculate how long this takes
   **/
  void TrigCostData::bufferEvent() const {
#ifdef MTHREAD
    m_trigCostObject->ReadAllActive();
#endif

    checkBuffers(); // Out with the old

    if (m_rosMatching) mapRosToAlgs();
    mapChainAndSeqToAlg();

    bufferSequenceAlgCallsCaches(); // Needs mapChainAndSeqToAlg
    bufferChainSeqTime();
    bufferChainAlgCalls(); // This ALSO buffers chain Rerun status. Needs bufferSequenceAlgCallsCaches

    //bufferChainPassed(); // Is anything using this?
    //bufferIsL1PassedBeforePrescale(); // Is anything using this?

    if (m_rosMatching) {
      bufferAlgRosInformation(); // needs  mapRosToAlgs
      bufferSeqROSInformation(); // needs bufferAlgRosInformation
      bufferChainRosInformation(); // Needs mapChainAndSeqToAlg
    }
  }

  /**
   * Figure out if the alg is part of a chain which was resurrected
   */
  Bool_t TrigCostData::getSeqIsRerun(UInt_t n) const {
    Int_t chainID = getSequenceChannelCounter(n);

    return m_chainRerunStatus[ chainID ];
  }

  /**
   * Used when full chain (L1+HLT) prescale weights are needed outside of the Rates monitor.
   * Use the data store to distribute the information
   * @param name Const reference to the chain name.
   * @param value 1/total chain prescale
   */
  void TrigCostData::setChainPrescaleWeight(const std::string& name, Double_t value) const {
    m_chainPSWeight[name] = value;
  }

  /**
   * Get the prescale weight for a chain - is filled by the Rates monitor
   * @param name Const reference to the chain name.
   * @return value 1/total chain prescale
   */
  Double_t TrigCostData::getChainPrescaleWeight(const std::string& name) const {
    StringDoubleMapIt_t it = m_chainPSWeight.find(name);

    if (it == m_chainPSWeight.end()) {
      Error("TrigCostData::getChainPrescaleWeight", "Cannot find chain prescale weight for %s", name.c_str());
      return DBL_MIN;
    }
    return it->second;
  }

  /**
   * For times when chain execution time is not stored in the D3PD, this function computes it
   * by looping over the sequences for the event.
   * Result is buffered internally for the event.
   * In addition, calculates the number of sequence calls and buffers this too.
   **/
  void TrigCostData::bufferChainSeqTime() const {
    for (UInt_t c = 0; c < this->getNChains(); ++c) {
      Int_t chainID = this->getChainID(c);

      Float_t timer = 0.;
      UInt_t nSeq = 0;
      for (UInt_t s = 0; s < this->getNSequences(); ++s) {
        if (this->getSequenceChannelCounter(s) != chainID) continue; // If sequence does not belong to correct chain

        if (this->getSequenceTime(s) > 0.) {
          timer += this->getSequenceTime(s);
        } else {
          timer += this->getSequenceAlgTotalTime(s);
        }

        ++nSeq;
      }

      // Buffer answer
      m_chainSeqTimeMap[ chainID ] = timer;
      m_chainSeqCallMap[ chainID ] = nSeq; // Also buffer this
    }
  }

  /**
   * @param n Chain index in D3PD.
   * @return Total chain execution time in the event as sum of all algorithm time associated to the chain.
   */
  Float_t TrigCostData::getChainTimerFromSequences(UInt_t n) const {
    return m_chainSeqTimeMap[ this->getChainID(n) ];
  }

  /**
   * Calculate the number of sequences which are called by a chain. This is buffered.
   * @param n Chain index in D3PD.
   * @return Total number of sequences called by the chain in the event.
   */
  UInt_t TrigCostData::getChainSeqCalls(UInt_t n) const {
    return m_chainSeqCallMap[ this->getChainID(n) ];
  }

  /**
   * Calculate the number of algorithms which are called by a chain. This is buffered, along with caches.
   * Also buffers the rerun status flag
   * @param n Chain index in D3PD.
   * @return Total number of algorithm calls by the chain in the event.
   **/
  void TrigCostData::bufferChainAlgCalls() const {
    for (UInt_t c = 0; c < this->getNChains(); ++c) {
      Int_t chainID = this->getChainID(c);

      m_chainRerunStatus[ chainID ] = (Bool_t) getIsChainResurrected(c);

      UInt_t called = 0;
      UInt_t cached = 0;
      for (UInt_t s = 0; s < this->getNSequences(); ++s) {
        if (this->getSequenceChannelCounter(s) != chainID) continue; // If sequence does not belong to correct chain
        called += getSequenceAlgCalls(s);
        cached += getSequenceAlgCaches(s);
      }

      m_chainAlgCallMap [ chainID ] = called;
      m_chainAlgCacheMap[ chainID ] = cached; // Also buffer this
    }
  }

  /**
   * Get the number of algorithms which are called by a chain. T Result is buffered.
   * @param n Chain index in D3PD.
   * @return Total number of algorithm calls by the chain in the event.
   */
  UInt_t TrigCostData::getChainAlgCalls(UInt_t n) const {
    return m_chainAlgCallMap[ this->getChainID(n) ];
  }

  /**
   * Get the number of algorithms whose cached results were used by a chain. Result is buffered.
   * @param n Chain index in D3PD.
   * @return Total number of cached algorithm calls by the chain in the event.
   */
  UInt_t TrigCostData::getChainAlgCaches(UInt_t n) const {
    return m_chainAlgCacheMap[ this->getChainID(n) ];
  }

  /**
   * Get a set of D3PD locations of algorithms associated to a chain. Result is buffered.
   * @param n Chain index in D3PD.
   * @return Cont reference to set of pairs of <seqLocation,algLocation> identifiers of algorithms in the event.
   */
  const std::set< std::pair<Int_t, Int_t> >& TrigCostData::getChainAlgs(UInt_t n) const {
    return m_chainAlgMap[ getChainID(n) ];
  }

  /**
   * Helper function to map a chain or sequence ID to a list of algs which ran.
   */
  void TrigCostData::mapChainAndSeqToAlg() const {
    for (UInt_t s = 0; s < getNSequences(); ++s) {
      for (UInt_t a = 0; a < getNSeqAlgs(s); ++a) {
        // Seq ID and chain ID, alg location
        const UInt_t seqId = getSequenceIndex(s);
        const UInt_t chainId = getSequenceChannelCounter(s);
        const std::pair<Int_t, Int_t> myAlgLocation(s, a);

        m_chainAlgMap[ chainId ].insert(myAlgLocation);
        m_seqAlgMap[ seqId ].insert(myAlgLocation);

        if (Config::config().debug()) {
          Info("TrigCostData::mapChainAndSeqToAlg", "Mapping Chain ID %i and sequence ID %i to alg location %i,%i",
               chainId, seqId, myAlgLocation.first, myAlgLocation.second);
        }
      }
    }
  }

  /**
   * A map of this information is formed if needed and buffered for the event.
   */
  void TrigCostData::bufferChainPassed() const {
    if (this->getNChains() == 0) {
      Error("TrigCostData::bufferChainPassed", "No chain data in this cost object.");
      return;
    }
    for (UInt_t c = 0; c < this->getNChains(); ++c) {
      const Int_t id = this->getChainID(c);
      const Int_t level = this->getChainLevel(c);
      this->m_chainPassMap[ TrigConfInterface::getHLTNameFromChainID(id, level) ] = this->getIsChainPassed(c);
    }
  }

  /**
   * Allows for a query if a particular chain passed the event where the chain is supplied by name.
   * A map of this information is formed on the first call and buffered for the event.
   * @param n Name of the chain.
   * @return If the chain passed the physics requirements in the event.
   */
  Bool_t TrigCostData::getIsChainPassed(std::string& n) const {
    if (this->m_chainPassMap.count(n) == 1) {
      // If we have a record of this chain, return its reported PASS or FAIL
      return this->m_chainPassMap[ n ];
    } else {
      // If the chain asked for was not in the event, it cannot have passed, return FAIL
      return kFALSE;
    }
  }

  /**
   * Get the number of ROBs requested (cached) by all algorithms run by a chain
   * @param n Chain index in D3PD.
   * @return Number of cached ROB requests
   */
  UInt_t TrigCostData::getChainROBRequests(UInt_t n) const {
    return m_chainROBRequests[ n ];
  }

  /**
   * Get the size of all requested (cached) ROBs by all algorithms run by a chain
   * @param n Chain index in D3PD.
   * @return Size in kB of cached ROB requests
   */
  Float_t TrigCostData::getChainROBRequestSize(UInt_t n) const {
    return m_chainROBRequestSize[ n ];
  }

  /**
   * Get the number of ROBs retrieved by by all algorithms run by a chain
   * @param n Chain index in D3PD.
   * @return Number of ROB retrievals
   */
  UInt_t TrigCostData::getChainROBRetrievals(UInt_t n) const {
    return m_chainROBRetrievals[ n ];
  }

  /**
   * Get the size of all ROBs retrieved by by all algorithms run by a chain
   * @param n Chain index in D3PD.
   * @return Size in kB of ROB retrievals
   */
  Float_t TrigCostData::getChainROBRetrievalSize(UInt_t n) const {
    return m_chainROBRetrievalSize[ n ];
  }

  /**
   * Loop over the ROS info of all algs run under a given chain. Buffer the data.
   * @param n Chain index in D3PD.
   */
  void TrigCostData::bufferChainRosInformation() const {
    for (UInt_t n = 0; n < this->getNChains(); ++n) {
      std::set< std::pair<Int_t, Int_t> > algs = getChainAlgs(n);

      UInt_t req = 0, ret = 0;
      Float_t reqSize = 0., retSize = 0.;
      // Loop over all algorithms called by this chain
      std::set< std::pair<Int_t, Int_t> >::iterator it = algs.begin();
      for (; it != algs.end(); ++it) {
        req += getSeqAlgROBRequests(it->first, it->second);
        ret += getSeqAlgROBRetrievals(it->first, it->second);
        reqSize += getSeqAlgROBRequestSize(it->first, it->second);
        retSize += getSeqAlgROBRetrievalSize(it->first, it->second);
      }

      m_chainROBRequests[ n ] = req;
      m_chainROBRetrievals[ n ] = ret;
      m_chainROBRequestSize[ n ] = reqSize;
      m_chainROBRetrievalSize[ n ] = retSize;
    }
  }

  /**
   * On request, this is buffered for the event.
   */
  void TrigCostData::bufferIsL1PassedBeforePrescale() const {
    if (this->getNL1() == 0) {
      Error("TrigCostData::getIsL1PassedBeforePrescale", "No L1 data in this cost object.");
      return;
    }
    for (UInt_t c = 0; c < this->getNL1(); ++c) {
      const Int_t id = this->getL1CtpId(c);
      this->m_L1BeforePrescalePassMap[ TrigConfInterface::getNameFromCtpId(id) ] =
        this->getIsL1PassedBeforePrescale(c);
    }
  }

  /**
   * String based check for an event passing L1. On request, this is buffered for the event.
   * @param n Name of a chain in the current menu.
   * @return If chain passed before L1 prescale was applied.
   */
  Bool_t TrigCostData::getIsL1PassedBeforePrescale(std::string& n) const {
    if (this->m_L1BeforePrescalePassMap.count(n) == 1) {
      // If we have a record of this chain, return its reported PASS or FAIL
      return this->m_L1BeforePrescalePassMap[ n ];
    } else {
      // If the chain asked for was not in the event, it cannot have passed, return FAIL
      return kFALSE;
    }
  }

  /**
   * Find the location of a L1 item in the D3PD
   * @return D3PD index or -1 if cannot be found
   */
  Int_t TrigCostData::getL1Location(const std::string& n) const {
    const Int_t CTIPD = TrigConfInterface::getCtpId(n);

    for (UInt_t c = 0; c < this->getNL1(); ++c) {
      if (CTIPD == (Int_t) this->getL1CtpId(c)) return c;
    }
    return -1;
  }

  /////////////////////////////////

  /**
   * Gets the number of algorithms called by a sequence. Result is buffered.
   * @param n Sequence index in D3PD.
   * @return Total number of algorithm calls by the sequence.
   */
  UInt_t TrigCostData::getSequenceAlgCalls(UInt_t n) const {
    return m_seqAlgCallMap[ n ];
  }

  /**
   * Get the number of cached algorithm calls by a sequence. Result is buffered.
   * @param n Sequence index in D3PD.
   * @return Total number of cached algorithm calls by the sequence.
   */
  UInt_t TrigCostData::getSequenceAlgCaches(UInt_t n) const {
    return m_seqAlgCacheMap[ n ];
  }

  /**
   * Calculate the number of algorithms called by a sequence.
   */
  void TrigCostData::bufferSequenceAlgCallsCaches() const {
    for (UInt_t n = 0; n < this->getNSequences(); ++n) {
      UInt_t called = 0;
      UInt_t cached = 0;
      for (UInt_t a = 0; a < getNSeqAlgs(n); ++a) {
        if (this->getSeqAlgIsCalled(n, a) == kTRUE) {
          ++called;
        } else if (this->getSeqAlgIsCached(n, a) == kTRUE) {
          ++cached;
        }
        //Assert that I cannot be both cached and called
        assert((this->getSeqAlgIsCached(n, a) == kTRUE && this->getSeqAlgIsCalled(n, a) == kTRUE) == kFALSE);
      }

      m_seqAlgCallMap [ n ] = called;
      m_seqAlgCacheMap[ n ] = cached; // Also buffer this
    }
  }

  /////////////////////////////////

  UInt_t TrigCostData::getSeqROSCalls(UInt_t n) const {
    return m_seqROSCallMap[n];
  }

  Float_t TrigCostData::getSeqROSTime(UInt_t n) const {
    return m_seqROSTimeMap[n];
  }

  UInt_t TrigCostData::getSeqROBRequests(UInt_t n) const {
    return m_seqROBReqs[n];
  }

  Float_t TrigCostData::getSeqROBRequestSize(UInt_t n) const {
    return m_seqROBReqSize[n];
  }

  UInt_t TrigCostData::getSeqROBRetrievals(UInt_t n) const {
    return m_seqROBRets[n];
  }

  Float_t TrigCostData::getSeqROBRetrievalSize(UInt_t n) const {
    return m_seqROBRetSize[n];
  }

  UInt_t TrigCostData::getSeqROBOthers(UInt_t n) const {
    return m_seqROBOther[n];
  }

  /**
   * Internal call to buffer the ROB data for a sequence from its algorithms
   * @param n Sequnce D3PD index to buffer
   */
  void TrigCostData::bufferSeqROSInformation() const {
    for (UInt_t n = 0; n < this->getNSequences(); ++n) {
      Float_t rosTime = 0., robReqSize = 0., robRetSize = 0.;
      Int_t robReq = 0, robRet = 0, robOther = 0, rosCalls = 0;
      // Loop over all algorithms in sequence
      for (UInt_t a = 0; a < this->getNSeqAlgs(n); ++a) {
        rosCalls += this->getSeqAlgROSCalls(n, a);
        rosTime += this->getSeqAlgROSTime(n, a);
        robRet += this->getSeqAlgROBRetrievals(n, a);
        robReq += this->getSeqAlgROBRequests(n, a);
        robRetSize += this->getSeqAlgROBRetrievalSize(n, a);
        robReqSize += this->getSeqAlgROBRequestSize(n, a);
        robOther += this->getSeqAlgROBOthers(n, a);
      }

      m_seqROSCallMap[n] = rosCalls;
      m_seqROSTimeMap[n] = rosTime;
      m_seqROBRets[n] = robRet;
      m_seqROBReqs[n] = robReq;
      m_seqROBRetSize[n] = robRetSize;
      m_seqROBReqSize[n] = robReqSize;
      m_seqROBOther[n] = robOther;
    }
  }

  /////////////////////////////////

  /**
   * Get the location in the D3PD of the ROS request from an algorithm.
   * Take a pair as input as this is the standard format from chain->alg lookup
   * Regular method also available,
   * @see TrigCostData::getSeqAlgROBLocations(UInt_t n, UInt_t a) const
   * @param algLocation std::pair of <seqLocation,algLocation>
   * @return Location of ROS request, or -1 if none found.
   */
  std::set<Int_t> TrigCostData::getSeqAlgROBLocations(const std::pair<Int_t, Int_t>& algLocation) const {
    if (m_algRosMap.count(algLocation) == 0) return m_emptySet;

    return m_algRosMap[ algLocation ];
  }

  /**
   * Get the location in the D3PD of the ROS request from an algorithm.
   * @see TrigCostData::getSeqAlgROBLocations(const std::pair<Int_t,Int_t>& algLocation) const
   * @param n Sequence index in D3PD.
   * @param a Algorithm index in sequence.
   * @return Location of ROS request, or -1 if none found.
   */
  std::set<Int_t> TrigCostData::getSeqAlgROBLocations(UInt_t n, UInt_t a) const {
    m_recyclablePair.first = (Int_t) n;
    m_recyclablePair.second = (Int_t) a;
    return getSeqAlgROBLocations(m_recyclablePair);
  }

  /**
   * Calculates and buffers for the event statistics on ROS information for all algorithms
   * @see TrigCostData::getSeqAlgROSCalls(UInt_t n, UInt_t a)
   * @see TrigCostData::getSeqAlgROSTime(UInt_t n, UInt_t a)
   * @see TrigCostData::getSeqAlgROBRequests(UInt_t n, UInt_t a)
   * @see TrigCostData::getSeqAlgROBRequestSize(UInt_t n, UInt_t a)
   * @see TrigCostData::getSeqAlgROBRetrievals(UInt_t n, UInt_t a)
   * @see TrigCostData::getSeqAlgROBRetrievalSize(UInt_t n, UInt_t a)
   * @see TrigCostData::getSeqAlgROBOthers(UInt_t n, UInt_t a)
   */
  void TrigCostData::bufferAlgRosInformation() const {
    for (UInt_t s = 0; s < getNSequences(); ++s) {
      for (UInt_t a = 0; a < getNSeqAlgs(s); ++a) {
        // For this function - recyclable pair holds this algorithm's execution
        m_recyclablePair.first = (Int_t) s;
        m_recyclablePair.second = (Int_t) a;

        // Get the ROS requests I made
        std::set<Int_t> ros = getSeqAlgROBLocations(m_recyclablePair);

        Float_t rosTime = 0., robReqSize = 0., robRetSize = 0.;
        Int_t robReq = 0, robRet = 0, robOther = 0;

        // Loop over the request's ROS call's data
        for (std::set<Int_t>::iterator robIt = ros.begin(); robIt != ros.end(); ++robIt) {
          Int_t rob = (*robIt);
          rosTime += getROBTimer(rob);

          Int_t TEMP_REQ = 0, TEMP_RET = 0;
          for (UInt_t robData = 0; robData < getROBDataN(rob); ++robData) {
            if (getIsROBDataRetrieved(rob, robData)) {
              ++robRet;
              ++TEMP_RET;
              robRetSize += getROBDataSize(rob, robData);
            }
            if (getIsROBDataCached(rob, robData)) {
              ++robReq;
              ++TEMP_REQ;
              robReqSize += getROBDataSize(rob, robData);
            }
            if (getIsROBDataUnclassified(rob, robData)
                || getIsROBDataDisabled(rob, robData)
                || getIsROBDataIgnored(rob, robData)) {
              ++robOther;
            }
          }
          // This can come out at some point.
          // as the CACHE flag applies only to a ROB, not the ROS request - I am checking here that there is never a
          // ROS request which is listed as both
          assert((TEMP_REQ == 0 && TEMP_RET == 0) || (TEMP_REQ > 0 && TEMP_RET == 0) ||
                 (TEMP_REQ == 0 && TEMP_RET > 0));
        }

        m_algRosCalls[ m_recyclablePair ] = ros.size();
        m_algRosTime[ m_recyclablePair ] = rosTime;
        m_algROBRequests[ m_recyclablePair ] = robReq;
        m_algROBRetrievals[ m_recyclablePair ] = robRet;
        m_algROBOther[ m_recyclablePair ] = robOther;
        m_algROBRequestSize[ m_recyclablePair ] = robReqSize;
        m_algROBRetrievalSize[ m_recyclablePair ] = robRetSize;
      }
    }
  }

  /**
   * Get the number of calls made to the ROS from an algorithm.
   * @param n Sequence index in D3PD.
   * @param a Algorithm index in sequence.
   */
  UInt_t TrigCostData::getSeqAlgROSCalls(UInt_t n, UInt_t a) const {
    m_recyclablePair.first = (Int_t) n;
    m_recyclablePair.second = (Int_t) a;
    return m_algRosCalls[ m_recyclablePair ];
  }

  /**
   * Get the total time spent on ROS access for all ROS requests from an algorithm
   * @param n Sequence index in D3PD.
   * @param a Algorithm index in sequence.
   */
  Float_t TrigCostData::getSeqAlgROSTime(UInt_t n, UInt_t a) const {
    m_recyclablePair.first = (Int_t) n;
    m_recyclablePair.second = (Int_t) a;
    return m_algRosTime[ m_recyclablePair ];
  }

  /**
   * Get the number of ROBs requested (cached) by an algorithm
   * @param n Sequence index in D3PD.
   * @param a Algorithm index in sequence.
   */
  UInt_t TrigCostData::getSeqAlgROBRequests(UInt_t n, UInt_t a) const {
    m_recyclablePair.first = (Int_t) n;
    m_recyclablePair.second = (Int_t) a;
    return m_algROBRequests[ m_recyclablePair ];
  }

  /**
   * Get the size of all requested (cached) ROBs by an algorithm
   * @param n Sequence index in D3PD.
   * @param a Algorithm index in sequence.
   */
  Float_t TrigCostData::getSeqAlgROBRequestSize(UInt_t n, UInt_t a) const {
    m_recyclablePair.first = (Int_t) n;
    m_recyclablePair.second = (Int_t) a;
    return m_algROBRequestSize[ m_recyclablePair ];
  }

  /**
   * Get the number of ROBs retrieved by an algorithm
   * @param n Sequence index in D3PD.
   * @param a Algorithm index in sequence.
   */
  UInt_t TrigCostData::getSeqAlgROBRetrievals(UInt_t n, UInt_t a) const {
    m_recyclablePair.first = (Int_t) n;
    m_recyclablePair.second = (Int_t) a;
    return m_algROBRetrievals[ m_recyclablePair ];
  }

  /**
   * Get the size of all ROBs retrieved by an algorithm
   * @param n Sequence index in D3PD.
   * @param a Algorithm index in sequence.
   */
  Float_t TrigCostData::getSeqAlgROBRetrievalSize(UInt_t n, UInt_t a) const {
    m_recyclablePair.first = (Int_t) n;
    m_recyclablePair.second = (Int_t) a;
    return m_algROBRetrievalSize[ m_recyclablePair ];
  }

  /**
   * Get the number of ROBs requested by an algorithm which are marked as 'Ignored', 'Disabled' or 'Unclassified'
   * @param n Sequence index in D3PD.
   * @param a Algorithm index in sequence.
   */
  UInt_t TrigCostData::getSeqAlgROBOthers(UInt_t n, UInt_t a) const {
    m_recyclablePair.first = (Int_t) n;
    m_recyclablePair.second = (Int_t) a;
    return m_algROBOther[ m_recyclablePair ];
  }

  /////////////////////////////////

  /**
   * As algs are already stored under sequences (hence - vector<vector<stuff>>) we cannot go deeper.
   * But algs can have many ROI. So we use a linking index to connect seq_alg_roi_index() with a location in seq_roi().
   * At this location we find a vector with size getSeqAlgNRoI(seq, alg) which contains a list of ROI ID
   * These can be accessed in the D3PD by calling getRoIIndexFromId( roiID ) and then looking at the returned D3PD
   *index location.
   *
   * Given this complexity, it is probably better to look at the example below.
   *
   * @see getRoIIndexFromId( roiID )
   * @param n Sequence index in D3PD.
   * @param a Algorithm index in sequence.
   * @param roi Which ROI to get the ID for - this still needs to be mapped to the location in the D3PD using
   *getRoIIndexFromId
   * @return ID of roi'th attached region of interest.
   */
  Int_t TrigCostData::getSeqAlgRoIID(UInt_t n, UInt_t a, UInt_t roi) const {
    // Info("TrigCostData::getSeqAlgRoIID"," ---   ---   --- " );
    // Info("TrigCostData::getSeqAlgRoIID","Requested ROI #%i from alg. ", roi );

    UInt_t nRoi = getSeqAlgNRoI(n, a);

    // Info("TrigCostData::getSeqAlgRoIID","This alg has %i ROIs. ", nRoi );

    if (roi >= nRoi) {
      Warning("TrigCostData::getSeqAlgRoIID", "Requested RoI ID out-of-range");
      return 0;
    }

    UInt_t sizeOuter = m_trigCostObject->seq_alg_roi_index()->size(); // Range check
    if (sizeOuter <= n) {
      Warning("TrigCostData::getSeqAlgRoIID", "Cannot find any RoI lists for alg (out of range).");
      return 0;
    }

    UInt_t sizeInner = m_trigCostObject->seq_alg_roi_index()->at(n).size(); // Range check
    if (sizeInner <= a) {
      Warning("TrigCostData::getSeqAlgRoIID", "Cannot find location of RoI list for alg (out of range).");
      return 0;
    }

    UInt_t algRoIs = (Int_t) m_trigCostObject->seq_alg_roi_index()->at(n).at(a);
    // Info("TrigCostData::getSeqAlgRoIID"," The requested ROIs have index in the ALG-ROI container %i.", algRoIs );

    // Info("TrigCostData::getSeqAlgRoIID"," The ALG-ROI container is size %i",  (Int_t)
    // m_trigCostObject->seq_roi()->size() );

    sizeOuter = m_trigCostObject->seq_roi()->size(); // Range check
    if (sizeOuter <= algRoIs) {
      Warning("TrigCostData::getSeqAlgRoIID", "Cannot find RoI list for alg (out of range).");
      return 0;
    }

    sizeInner = m_trigCostObject->seq_roi()->at(algRoIs).size(); // Range check
    if (sizeInner <= roi) {
      if (Config::config().getDisplayMsg(kMsgRoISize) == kTRUE) {
        Warning("TrigCostData::getSeqAlgRoIID",
                "Cannot find RoI #%u within the list for this alg (out of range [size:%u]).", roi, sizeInner);
      }
      return 0;
    }

    UInt_t roiID = m_trigCostObject->seq_roi()->at(algRoIs).at(roi);
    // Info("TrigCostData::getSeqAlgRoIID","   At location %i of the ALG-ROI container there are %i entries, the %i'th
    // has ROI-ID %i",
    //  algRoIs, (Int_t) m_trigCostObject->seq_roi()->at(algRoIs).size(), roi, roiID  );

    //UInt_t roiLocation = getRoIIndexFromId( roiID );
    // Info("TrigCostData::getSeqAlgRoIID","    The location of ROI with index %i is %i ", roiID, roiLocation );

    // const std::string type = getRoITypeString( roiLocation );
    // Info("TrigCostData::getSeqAlgRoIID","     This ROI at location %i is type %s", roiLocation, type.c_str() );
    return roiID;
  }

  /**
   * @see TrigCostData::getSeqAlgRoIID(UInt_t n, UInt_t a, UInt_t roi)
   */
  Int_t TrigCostData::getSeqAlgRoILocation(UInt_t n, UInt_t a, UInt_t roi) const {
    return getRoIIndexFromId(getSeqAlgRoIID(n, a, roi));
  }

  /**
   * Check if this ROB can be associated to an algorithm in the event, if so get the D3PD
   * location of the algorithm.
   * Association is done via checking the algorithm name hash and by time window.
   * @param n ROB index in D3PD.
   * @return D3PD location in this event of associated algorithm, or (-1,-1) if not found.
   */
  const std::pair< Int_t, Int_t >& TrigCostData::getROBAlgLocation(UInt_t n) const {
    const static std::pair<Int_t, Int_t> noMatch(-1, -1);

    if (m_rosAlgMap.count(n) == 0) return noMatch;

    return m_rosAlgMap[ n ];
  }

  /**
   * Helper function. Map algs executed in the event to the hash of their name as an ID.
   * Map these IDs to ROS requests - use the timing information to identify the correct alg.
   *
   * This function will only perform the mapping if ROS monitoring is switched on.
   */
  void TrigCostData::mapRosToAlgs() const {
    // Buffer this data for this event

    // First loop over algs - build map of which algs were processed this event, linked to the hash of their name.
    for (UInt_t s = 0; s < getNSequences(); ++s) {
      // Loop over all algorithms in sequence
      for (UInt_t a = 0; a < getNSeqAlgs(s); ++a) {
        // Map this location for easy access later (in next, ROS loop)
        // This prevents a nested loop in a per-event method which would be mighty slow
        Int_t seqIndex = getSequenceIndex(s);
        Int_t seqAlgPos = getSeqAlgPosition(s, a);
        UInt_t algNameHash = TrigConfInterface::getHLTAlgNameIDFromSeqIDAndAlgPos(seqIndex, seqAlgPos);

        m_algExecMap[ algNameHash ].insert(std::make_pair(s, a));

        // if (Config::config().debug()) {
        //   Info("TrigCostData::mapRosToAlgs","Mapped Alg Name Hash %u to Alg %i,%i", algNameHash, s, a);
        // }
      }
    }

    // Second loop over all ROS, map ROS requests to algorithms.
    for (UInt_t r = 0; r < getNROBs(); ++r) {
      // * NOTE * If we have been told NOT to do ROB information. Here is a good place to break this loop and save on
      // execution time
      if (Config::config().getInt(kEnableROSToAlgMatching) == kFALSE) break;

      UInt_t rosAlgId = getROBReqID(r);

      // Check if this alg's exec data is saved
      if (m_algExecMap.count(rosAlgId) == 0) {
        if (Config::config().debug()) Info("TrigCostData::mapRosToAlgs", "No Mapped Alg with name hash %u for ROB # %i",
                                           rosAlgId, r);
        continue;
      }

      if (Config::config().debug()) {
        Info("TrigCostData::mapRosToAlgs", "Finding match for ROB %i", r);
      }

      // Loop over alg exec's and check timing to see if compatible
      std::set< std::pair< UInt_t, UInt_t > >::iterator it = m_algExecMap[ rosAlgId ].begin();
      for (; it != m_algExecMap[ rosAlgId ].end(); ++it) {
        if (getRosReqBelongsToAlg(it->first, it->second, r) == kFALSE) {
          if ((Config::config().debug()) && it == --m_algExecMap[ rosAlgId ].end()) {
            Info("TrigCostData::mapRosToAlgs", "  Temporal match failed for Alg with hash %u and ROB # %u", rosAlgId,
                 r);
          }
          continue;
        }
        // We have a name hash and temporal match! This alg can be associated to this ROS request.
        // Should only get one match...
        if (m_rosAlgMap.count(r) == 1) {
          Warning("TrigCostData::mapRosToAlgs", "Found two matches for a ROS request. (%i->%i,%i and %i->%i,%i)",
                  r, it->first, it->second, r, m_rosAlgMap[r].first, m_rosAlgMap[r].second);
        }

        //if (Config::config().debug()) {
        //  Info("TrigCostData::mapRosToAlgs"," Mapped Alg %i,%i to ROB # %i", it->first, it->second, r);
        //}
        // Each ROS Req is associated to only one Alg (1-to-1)
        m_rosAlgMap[r] = (*it); // This dereferences to the pair
        // Each alg can have many ROS requests
        if (m_algRosMap.count(*it) == 0) {
          m_algRosMap[ (*it) ] = std::set<Int_t>();
        }
        m_algRosMap[ (*it) ].insert(r); // Make the map go both ways (1-to-many)
      }
      if ((Config::config().debug()) && m_rosAlgMap.count(r) == 0) {
        Warning("TrigCostData::mapRosToAlgs", " Found zero matches for a ROS request. (%i)", r);
      }
    }
  }

  /**
   * Helper function, returns if an algorithm requested a given ROS req based on the time of the request
   * @param seq D3PD index of sequence to test
   * @param alg D3PD index of algorithm in sequence to test
   * @param Ros D3PD index of ROS to test
   * @returns kTRUE if the ROS request time falls within the algorithm execution time.
   */
  Bool_t TrigCostData::getRosReqBelongsToAlg(UInt_t seq, UInt_t alg, UInt_t ros) const {
    if (isZero(getROBTimeStartSec(ros)) || isZero(getSeqAlgTimeStartSec(seq, alg))) return kFALSE;

    Float_t startTimeDif = ((Float_t) getROBTimeStartSec(ros) - (Float_t) getSeqAlgTimeStartSec(seq, alg));
    //Check that this did not go over an hour boundary
    Bool_t flagStartSpansHourBoundary = kFALSE;
    if (startTimeDif > 1800.) { //This is 30m
      startTimeDif = 3600. - startTimeDif;
      flagStartSpansHourBoundary = kTRUE;
      if (Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
        Info("TrigCostData::getRosReqBelongsToAlg",
             "Hour Boundary in START AlgStart:%i.%i ROSStart:%i.%i - ROSEnd:%i.%i AlgEnd:%i.%i. TimeDifStart:%f",
             getSeqAlgTimeStartSec(seq, alg),
             getSeqAlgTimeStartMicroSec(seq, alg),
             getROBTimeStartSec(ros),
             getROBTimeStartMicroSec(ros),
             getROBTimeStopSec(ros),
             getROBTimeStopMicroSec(ros),
             getSeqAlgTimeStopSec(seq, alg),
             getSeqAlgTimeStopMicroSec(seq, alg),
             startTimeDif);
      }
    }
    startTimeDif += ((Float_t) getROBTimeStartMicroSec(ros) - (Float_t) getSeqAlgTimeStartMicroSec(seq, alg)) / 1e6;
    if (startTimeDif < 0) {
      if (flagStartSpansHourBoundary == kTRUE && Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
        Info("TrigCostData::getRosReqBelongsToAlg", "Hour boundary in start, therefore modified startTimeDif:%f"
                                                    " should be >= 0, but this is not the case it seems. Odd - this should be investigated. Failing this alg-ros match.",
             startTimeDif);
      }
      return kFALSE;
    }

    // Note we reverse the order here as we want to know if the ROS request is in the middle of the alg
    Float_t stopTimeDif = (Float_t) getSeqAlgTimeStopSec(seq, alg) - (Float_t) getROBTimeStopSec(ros);
    if (stopTimeDif < -1800.) { //This is 30m
      if (flagStartSpansHourBoundary == kTRUE) {
        // If both the start and stop appear to span the hour-boundary then this ROS req cannot be contained within the
        // alg
        // Only if one of the two span the hour boundary is this possible
        if (Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
          Info("TrigCostData::getRosReqBelongsToAlg",
               "Hour Boundary in STOP, therefore ROS cannot be associated to this ALG. TimeDifStop:%f", stopTimeDif);
        }
        return kFALSE;
      }
      stopTimeDif = 3600. + stopTimeDif;
      if (Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
        Info("TrigCostData::getRosReqBelongsToAlg",
             "Hour Boundary in STOP AlgStart:%i.%i ROSStart:%i.%i - ROSEnd:%i.%i AlgEnd:%i.%i. TimeDifStop:%f",
             getSeqAlgTimeStartSec(seq, alg),
             getSeqAlgTimeStartMicroSec(seq, alg),
             getROBTimeStartSec(ros),
             getROBTimeStartMicroSec(ros),
             getROBTimeStopSec(ros),
             getROBTimeStopMicroSec(ros),
             getSeqAlgTimeStopSec(seq, alg),
             getSeqAlgTimeStopMicroSec(seq, alg),
             stopTimeDif);
      }
    }
    stopTimeDif += ((Float_t) getSeqAlgTimeStopMicroSec(seq, alg) - (Float_t) getROBTimeStopMicroSec(ros)) / 1e6;
    if (flagStartSpansHourBoundary == kTRUE && Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
      Info("TrigCostData::getRosReqBelongsToAlg", "Stop TimeDifStop:%f", stopTimeDif);
    }

    // One of the checks needs to have a veto on == 0 as we can get a ROS on the temporal border between two ALGS
    // e.g. Alg A: AlgStart:339.450639 ROSStart:339.453074 --- ROSEnd:339.453074 AlgEnd:339.453074
    // e.g. Alg B: AlgStart:339.453074 ROSStart:339.453074 --- ROSEnd:339.453074 AlgEnd:339.458076
    // Without this, this ROS req would be assigned to them both.
    if (stopTimeDif < 0 || isZero(stopTimeDif)) {
      if (flagStartSpansHourBoundary == kTRUE && Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
        Info("TrigCostData::getRosReqBelongsToAlg", "Match failed due to stopTimeDif <= 0 (%f)", stopTimeDif);
      }
      return kFALSE;
    }

    if (flagStartSpansHourBoundary == kTRUE && Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
      Info("TrigCostData::getRosReqBelongsToAlg", "Match suceeded due to stopTimeDif > 0 (%f)", stopTimeDif);
    }
    return kTRUE;
  }

  /**
   * Get D3PD index location of a ROI in an event from its ID number.
   * @param id Read Out Buffer ID number.
   * @return The index location of the ROI or -1 if it could not be found.
   */
  Int_t TrigCostData::getRoIIndexFromId(Int_t id) const {
    //TODO buffer me.
    for (UInt_t i = 0; i < getNRoIs(); ++i) {
      if (getRoIID(i) == id) return i;
    }
    // Can't find it? try the hack. Note this is a XXX TODO HACK
    for (UInt_t i = 0; i < getNRoIs(); ++i) {
      if ((id == 0 || id == 255) && getRoIID(i) == 253) {
        if (Config::config().getDisplayMsg(kMsgRoIHack) == kTRUE) {
          Warning("TrigCostData::getRoIIndexFromId",
                  "Using RoI hack with %s RoI. NRoIs:%i. Requested RoI ID:%i, returning RoI #:%i with ID:%i",
                  getRoITypeString(i).c_str(),
                  getNRoIs(),
                  id,
                  i,
                  getRoIID(i));
        }
        return i;
      }
    }
    if (Config::config().getDisplayMsg(kMsgCannotFindRoI) == kTRUE) {
      Warning("TrigCostData::getRoIIndexFromId",
              "No RoI found with ID %i (going to return an invalid location, -1). The posibilities were:", id);
      for (UInt_t i = 0; i < getNRoIs(); ++i) {
        Warning("TrigCostData::getRoIIndexFromId", " -- RoI at location %i (%s) has ID %i",
                i,
                this->getRoITypeString(i).c_str(),
                this->getRoIID(i));
      }
    }
    return -1;
  }

  /**
   * Return a string representing this ROI's name
   * @param n RoI ID number.
   * @return Const reference to ROI type string.
   */
  const std::string& TrigCostData::getRoITypeString(Int_t n) const {
    if (n >= 0 && n < (Int_t) getNRoIs()) {
      if (getIsRoINone(n) == kTRUE) return Config::config().getStr(kNoneString);
      else if (getIsRoIMuon(n) == kTRUE) return Config::config().getStr(kMuonString);
      else if (getIsRoIEmTau(n) == kTRUE) {
        return Config::config().getStr(kEmTauString);
        //if (getIsRoITau(n))  return Config::config().getStr(kTauString); // For later
        //else return Config::config().getStr(kEmString);
      } else if (getIsRoIJet(n) == kTRUE) return Config::config().getStr(kJetString);
      else if (getIsRoIJetEt(n) == kTRUE) return Config::config().getStr(kJetEtString);
      else if (getIsRoIEnergy(n) == kTRUE) return Config::config().getStr(kEnergyString);
    }
    if (Config::config().getDisplayMsg(kMsgUnknownRoIType) == kTRUE) {
      Warning("TrigCostData::getRoITypeString", "Encountered an ROI at location:%i with no type.", n);
    }
    return Config::config().getStr(kUnknownString);
  }

  /**
   * @param n TE index in D3PD.
   * @return Find location in D3PD of TE with given index
   */
  Int_t TrigCostData::getTEPositionFromTEIndex(UInt_t n) const {
    for (UInt_t i = 0; i < getNTEs(); ++i) {
      if (getTEIndex(i) == n) return n;
    }
    return -1;
  }

  /**
   * Uses the event number to decide when a new event is processing. At this point, all buffers from the previous
   * event are emptied such that new information can be filled into them.
   */
  void TrigCostData::checkBuffers() const {
    // Check if we have a new event
    //Info("BUFF","CHECK BUFFER d3pd %i comp local %i from %s",this->getEventNumber(), m_bufferEventNumber,
    // getParent()->getName().c_str());

    if (this->getEventNumber() != m_bufferEventNumber) {
      //Info("BUFF","CLEANING BUFFER d3pd %i != local %i",this->getEventNumber(), m_bufferEventNumber);
      // New event, clear buffer
      m_bufferEventNumber = this->getEventNumber();
      m_chainSeqTimeMap.clear();
      m_chainAlgTimeMap.clear();
      m_chainPassMap.clear();
      m_L1BeforePrescalePassMap.clear();
      m_chainSeqCallMap.clear();
      m_chainAlgCallMap.clear();
      m_chainAlgCacheMap.clear();
      m_chainAlgMap.clear();
      m_chainROBRequests.clear();
      m_chainROBRetrievals.clear();
      m_chainROBRequestSize.clear();
      m_chainROBRetrievalSize.clear();
      m_seqAlgMap.clear();
      m_seqAlgCallMap.clear();
      m_seqAlgCacheMap.clear();
      m_algRosMap.clear();
      m_rosAlgMap.clear();
      m_algExecMap.clear();
      m_algRosCalls.clear();
      m_algRosTime.clear();
      m_algROBRequests.clear();
      m_algROBRetrievals.clear();
      m_algROBOther.clear();
      m_algROBRequestSize.clear();
      m_algROBRetrievalSize.clear();
      m_seqROSCallMap.clear();
      m_seqROSTimeMap.clear();
      m_seqROBRets.clear();
      m_seqROBReqs.clear();
      m_seqROBRetSize.clear();
      m_seqROBReqSize.clear();
      m_seqROBOther.clear();
      m_chainRerunStatus.clear();
    }
  }
} // namespace TrigCostRootAnalysis
