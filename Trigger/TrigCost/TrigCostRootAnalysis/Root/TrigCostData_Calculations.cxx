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
  Bool_t TrigCostData::getSeqIsRerun(UInt_t _n) const { 
    Int_t _chainID = getSequenceChannelCounter(_n);
    return m_chainRerunStatus[ _chainID ];
  }

  /**
   * Used when full chain (L1+HLT) prescale weights are needed outside of the Rates monitor. 
   * Use the data store to distribute the information
   * @param _name Const reference to the chain name.
   * @param _value 1/total chain prescale
   */
  void TrigCostData::setChainPrescaleWeight(const std::string& _name, Double_t _value) const {
    m_chainPSWeight[_name] = _value;
  }

  /**
   * Get the prescale weight for a chain - is filled by the Rates monitor
   * @param _name Const reference to the chain name.
   * @return _value 1/total chain prescale
   */
  Double_t TrigCostData::getChainPrescaleWeight(const std::string& _name) const {
    StringDoubleMapIt_t _it = m_chainPSWeight.find( _name );
    if (_it == m_chainPSWeight.end()) {
      Error("TrigCostData::getChainPrescaleWeight", "Cannot find chain prescale weight for %s", _name.c_str());
      return DBL_MIN;
    }
    return _it->second;
  }

  /**
   * For times when chain execution time is not stored in the D3PD, this function computes it
   * by looping over the sequences for the event.
   * Result is buffered internally for the event.
   * In addition, calculates the number of sequence calls and buffers this too.
   **/
  void TrigCostData::bufferChainSeqTime() const {
    for (UInt_t _c = 0; _c < this->getNChains(); ++_c) {
      Int_t _chainID = this->getChainID(_c);

      Float_t _timer = 0.;
      UInt_t _nSeq = 0;
      for (UInt_t _s = 0; _s < this->getNSequences(); ++_s) {

        if (this->getSequenceChannelCounter(_s) != _chainID) continue; // If sequence does not belong to correct chain

        if (this->getSequenceTime(_s) > 0.) {
          _timer += this->getSequenceTime(_s);
        } else {
          _timer += this->getSequenceAlgTotalTime(_s);
        }

        ++_nSeq;
      }

      // Buffer answer
      m_chainSeqTimeMap[ _chainID ] = _timer;
      m_chainSeqCallMap[ _chainID ] = _nSeq; // Also buffer this
    }
  }

  /**
   * @param _n Chain index in D3PD.
   * @return Total chain execution time in the event as sum of all algorithm time associated to the chain.
   */
  Float_t TrigCostData::getChainTimerFromSequences(UInt_t _n) const {
    return m_chainSeqTimeMap[ this->getChainID(_n) ];
  }

  /**
   * Calculate the number of sequences which are called by a chain. This is buffered.
   * @param _n Chain index in D3PD.
   * @return Total number of sequences called by the chain in the event.
   */
  UInt_t TrigCostData::getChainSeqCalls(UInt_t _n) const {
    return m_chainSeqCallMap[ this->getChainID(_n) ];
  }

  /**
   * Calculate the number of algorithms which are called by a chain. This is buffered, along with caches.
   * Also buffers the rerun status flag
   * @param _n Chain index in D3PD.
   * @return Total number of algorithm calls by the chain in the event.
   **/
  void TrigCostData::bufferChainAlgCalls() const {
    for (UInt_t _c = 0; _c < this->getNChains(); ++_c) {
      Int_t _chainID = this->getChainID(_c);

      m_chainRerunStatus[ _chainID ] = (Bool_t) getIsChainResurrected(_c);

      UInt_t _called = 0;
      UInt_t _cached = 0;
      for (UInt_t _s = 0; _s < this->getNSequences(); ++_s) {
        if (this->getSequenceChannelCounter(_s) != _chainID) continue; // If sequence does not belong to correct chain
        _called += getSequenceAlgCalls (_s);
        _cached += getSequenceAlgCaches(_s);
      }

      m_chainAlgCallMap [ _chainID ] = _called;
      m_chainAlgCacheMap[ _chainID ] = _cached; // Also buffer this
    }
  }

  /**
   * Get the number of algorithms which are called by a chain. T Result is buffered.
   * @param _n Chain index in D3PD.
   * @return Total number of algorithm calls by the chain in the event.
   */
  UInt_t TrigCostData::getChainAlgCalls(UInt_t _n) const {
    return m_chainAlgCallMap[ this->getChainID(_n) ];
  }

  /**
   * Get the number of algorithms whose cached results were used by a chain. Result is buffered.
   * @param _n Chain index in D3PD.
   * @return Total number of cached algorithm calls by the chain in the event.
   */
  UInt_t TrigCostData::getChainAlgCaches(UInt_t _n) const {
    return m_chainAlgCacheMap[ this->getChainID(_n) ];
  }

  /**
   * Get a set of D3PD locations of algorithms associated to a chain. Result is buffered.
   * @param _n Chain index in D3PD.
   * @return Cont reference to set of pairs of <seqLocation,algLocation> identifiers of algorithms in the event.
   */
  const std::set< std::pair<Int_t,Int_t> >& TrigCostData::getChainAlgs(UInt_t _n) const {
    return m_chainAlgMap[ getChainID(_n) ];
  }

  /**
   * Helper function to map a chain or sequence ID to a list of algs which ran.
   */
  void TrigCostData::mapChainAndSeqToAlg() const {
    for (UInt_t _s = 0; _s < getNSequences(); ++_s) {
      for (UInt_t _a = 0; _a < getNSeqAlgs(_s); ++_a) {

        // Seq ID and chain ID, alg location
        const UInt_t _seqId = getSequenceIndex(_s);
        const UInt_t _chainId = getSequenceChannelCounter(_s);
        const std::pair<Int_t, Int_t> _myAlgLocation(_s,_a);

        m_chainAlgMap[ _chainId ].insert( _myAlgLocation );
        m_seqAlgMap[ _seqId ].insert( _myAlgLocation );

        if (Config::config().debug()) {
          Info("TrigCostData::mapChainAndSeqToAlg","Mapping Chain ID %i and sequence ID %i to alg location %i,%i",
            _chainId,_seqId,_myAlgLocation.first,_myAlgLocation.second);
        }
      }
    }
  }

  /**
   * A map of this information is formed if needed and buffered for the event.
   */
  void TrigCostData::bufferChainPassed() const {
    if ( this->getNChains() == 0 ) {
      Error("TrigCostData::bufferChainPassed", "No chain data in this cost object.");
      return;
    }
    for (UInt_t _c = 0; _c < this->getNChains(); ++_c) {
      const Int_t _id = this->getChainID(_c);
      const Int_t _level = this->getChainLevel(_c);
      this->m_chainPassMap[ TrigConfInterface::getHLTNameFromChainID( _id,  _level) ] = this->getIsChainPassed(_c);
    }
  }

  /**
   * Allows for a query if a particular chain passed the event where the chain is supplied by name.
   * A map of this information is formed on the first call and buffered for the event.
   * @param _n Name of the chain.
   * @return If the chain passed the physics requirements in the event.
   */
  Bool_t TrigCostData::getIsChainPassed( std::string& _n ) const {
    if ( this->m_chainPassMap.count( _n ) == 1) {
      // If we have a record of this chain, return its reported PASS or FAIL
      return this->m_chainPassMap[ _n ];
    } else {
      // If the chain asked for was not in the event, it cannot have passed, return FAIL
      return kFALSE;
    }
  }

  /**
   * Get the number of ROBs requested (cached) by all algorithms run by a chain
   * @param _n Chain index in D3PD.
   * @return Number of cached ROB requests
   */
  UInt_t TrigCostData::getChainROBRequests(UInt_t _n) const {
    return m_chainROBRequests[ _n ];
  }

  /**
   * Get the size of all requested (cached) ROBs by all algorithms run by a chain
   * @param _n Chain index in D3PD.
   * @return Size in kB of cached ROB requests
   */
  Float_t TrigCostData::getChainROBRequestSize(UInt_t _n) const {
    return m_chainROBRequestSize[ _n ];
  }

  /**
   * Get the number of ROBs retrieved by by all algorithms run by a chain
   * @param _n Chain index in D3PD.
   * @return Number of ROB retrievals
   */
  UInt_t  TrigCostData::getChainROBRetrievals(UInt_t _n) const {
    return m_chainROBRetrievals[ _n ];
  }

  /**
   * Get the size of all ROBs retrieved by by all algorithms run by a chain
   * @param _n Chain index in D3PD.
   * @return Size in kB of ROB retrievals
   */
  Float_t TrigCostData::getChainROBRetrievalSize(UInt_t _n) const {
      return m_chainROBRetrievalSize[ _n ];
  }

  /**
   * Loop over the ROS info of all algs run under a given chain. Buffer the data.
   * @param _n Chain index in D3PD.
   */
  void TrigCostData::bufferChainRosInformation() const {
    for (UInt_t _n = 0; _n < this->getNChains(); ++_n) {

      std::set< std::pair<Int_t, Int_t> > _algs = getChainAlgs(_n);

      UInt_t _req = 0, _ret = 0;
      Float_t _reqSize = 0., _retSize = 0.;
      // Loop over all algorithms called by this chain
      std::set< std::pair<Int_t, Int_t> >::iterator _it = _algs.begin();
      for (; _it != _algs.end(); ++_it) {
        _req += getSeqAlgROBRequests( _it->first, _it->second ); 
        _ret += getSeqAlgROBRetrievals( _it->first, _it->second );
        _reqSize += getSeqAlgROBRequestSize( _it->first, _it->second );
        _retSize += getSeqAlgROBRetrievalSize( _it->first, _it->second );
      }

      m_chainROBRequests[ _n ]      = _req;
      m_chainROBRetrievals[ _n ]    = _ret;
      m_chainROBRequestSize[ _n ]   = _reqSize;
      m_chainROBRetrievalSize[ _n ] = _retSize;

    }
  }

  /**
   * On request, this is buffered for the event.
   */
  void  TrigCostData::bufferIsL1PassedBeforePrescale() const {
    if ( this->getNL1() == 0 ) {
      Error("TrigCostData::getIsL1PassedBeforePrescale", "No L1 data in this cost object.");
      return;
    }
    for (UInt_t _c = 0; _c < this->getNL1(); ++_c) {
      const Int_t _id = this->getL1CtpId(_c);
      this->m_L1BeforePrescalePassMap[ TrigConfInterface::getNameFromCtpId( _id ) ] = this->getIsL1PassedBeforePrescale(_c);
    }
  }

  /**
   * String based check for an event passing L1. On request, this is buffered for the event.
   * @param _n Name of a chain in the current menu.
   * @return If chain passed before L1 prescale was applied.
   */
  Bool_t TrigCostData::getIsL1PassedBeforePrescale(std::string& _n) const {
    if ( this->m_L1BeforePrescalePassMap.count( _n ) == 1) {
      // If we have a record of this chain, return its reported PASS or FAIL
      return this->m_L1BeforePrescalePassMap[ _n ];
    } else {
      // If the chain asked for was not in the event, it cannot have passed, return FAIL
      return kFALSE;
    }
  }

  /**
   * Find the location of a L1 item in the D3PD
   * @return D3PD index or -1 if cannot be found
   */
  Int_t TrigCostData::getL1Location(const std::string& _n) const {
    const Int_t _CTIPD = TrigConfInterface::getCtpId(_n);
    for (UInt_t _c = 0; _c < this->getNL1(); ++_c) {
      if (_CTIPD == (Int_t)this->getL1CtpId(_c)) return _c;
    }
    return -1;
  }

  /////////////////////////////////

  /**
   * Gets the number of algorithms called by a sequence. Result is buffered.
   * @param _n Sequence index in D3PD.
   * @return Total number of algorithm calls by the sequence.
   */
  UInt_t TrigCostData::getSequenceAlgCalls(UInt_t _n) const {
    return m_seqAlgCallMap[ _n ];
  }

  /**
   * Get the number of cached algorithm calls by a sequence. Result is buffered.
   * @param _n Sequence index in D3PD.
   * @return Total number of cached algorithm calls by the sequence.
   */
  UInt_t TrigCostData::getSequenceAlgCaches(UInt_t _n) const {
    return m_seqAlgCacheMap[ _n ];
  }

  /**
   * Calculate the number of algorithms called by a sequence. 
   */
  void TrigCostData::bufferSequenceAlgCallsCaches() const {
    for (UInt_t _n = 0; _n < this->getNSequences(); ++_n) {

      UInt_t _called = 0;
      UInt_t _cached = 0;
      for (UInt_t _a = 0; _a < getNSeqAlgs(_n); ++_a) {
        if (this->getSeqAlgIsCalled(_n, _a) == kTRUE) {
          ++_called;
        } else if (this->getSeqAlgIsCached(_n, _a) == kTRUE) {
          ++_cached;
        }
        //Assert that I cannot be both cached and called
        assert ( (this->getSeqAlgIsCached(_n, _a) == kTRUE && this->getSeqAlgIsCalled(_n, _a) == kTRUE) == kFALSE );
      }

      m_seqAlgCallMap [ _n ] = _called;
      m_seqAlgCacheMap[ _n ] = _cached; // Also buffer this
    }
  }

  /////////////////////////////////

  UInt_t  TrigCostData::getSeqROSCalls(UInt_t _n) const {
    return m_seqROSCallMap[_n];
  }

  Float_t TrigCostData::getSeqROSTime(UInt_t _n) const {
    return m_seqROSTimeMap[_n];
  }

  UInt_t  TrigCostData::getSeqROBRequests(UInt_t _n) const {
    return m_seqROBReqs[_n];
  }

  Float_t TrigCostData::getSeqROBRequestSize(UInt_t _n) const {
    return m_seqROBReqSize[_n];
  }

  UInt_t  TrigCostData::getSeqROBRetrievals(UInt_t _n) const {
    return m_seqROBRets[_n];
  }

  Float_t TrigCostData::getSeqROBRetrievalSize(UInt_t _n) const {
    return m_seqROBRetSize[_n];
  }

  UInt_t  TrigCostData::getSeqROBOthers(UInt_t _n) const {
    return m_seqROBOther[_n];
  }

  /**
   * Internal call to buffer the ROB data for a sequence from its algorithms
   * @param _n Sequnce D3PD index to buffer
   */
  void TrigCostData::bufferSeqROSInformation() const {
    for (UInt_t _n = 0; _n < this->getNSequences(); ++_n) {

      Float_t _rosTime = 0., _robReqSize = 0., _robRetSize = 0.;
      Int_t _robReq = 0, _robRet = 0, _robOther = 0, _rosCalls = 0;
      // Loop over all algorithms in sequence
      for (UInt_t _a = 0; _a < this->getNSeqAlgs(_n); ++_a) {
        _rosCalls   += this->getSeqAlgROSCalls(_n, _a);
        _rosTime    += this->getSeqAlgROSTime(_n, _a);
        _robRet     += this->getSeqAlgROBRetrievals(_n, _a);
        _robReq     += this->getSeqAlgROBRequests(_n, _a);
        _robRetSize += this->getSeqAlgROBRetrievalSize(_n, _a);
        _robReqSize += this->getSeqAlgROBRequestSize(_n, _a);
        _robOther   += this->getSeqAlgROBOthers(_n, _a);
      }

      m_seqROSCallMap[_n] = _rosCalls;
      m_seqROSTimeMap[_n] = _rosTime;
      m_seqROBRets[_n] = _robRet;
      m_seqROBReqs[_n] = _robReq;
      m_seqROBRetSize[_n] = _robRetSize;
      m_seqROBReqSize[_n] = _robReqSize;
      m_seqROBOther[_n] = _robOther;
    }
  }

  /////////////////////////////////

  /**
   * Get the location in the D3PD of the ROS request from an algorithm.
   * Take a pair as input as this is the standard format from chain->alg lookup
   * Regular method also available,
   * @see TrigCostData::getSeqAlgROBLocations(UInt_t _n, UInt_t _a) const
   * @param _algLocation std::pair of <seqLocation,algLocation>
   * @return Location of ROS request, or -1 if none found.
   */
  std::set<Int_t> TrigCostData::getSeqAlgROBLocations(const std::pair<Int_t,Int_t>& _algLocation) const {
    if ( m_algRosMap.count( _algLocation ) == 0) return m_emptySet;
    return m_algRosMap[ _algLocation ];
  }

  /**
   * Get the location in the D3PD of the ROS request from an algorithm.
   * @see TrigCostData::getSeqAlgROBLocations(const std::pair<Int_t,Int_t>& _algLocation) const
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @return Location of ROS request, or -1 if none found.
   */
  std::set<Int_t> TrigCostData::getSeqAlgROBLocations(UInt_t _n, UInt_t _a) const {
    _recyclablePair.first = (Int_t) _n;
    _recyclablePair.second = (Int_t) _a;
    return getSeqAlgROBLocations( _recyclablePair );
  }

  /**
   * Calculates and buffers for the event statistics on ROS information for all algorithms
   * @see TrigCostData::getSeqAlgROSCalls(UInt_t _n, UInt_t _a)
   * @see TrigCostData::getSeqAlgROSTime(UInt_t _n, UInt_t _a)
   * @see TrigCostData::getSeqAlgROBRequests(UInt_t _n, UInt_t _a)
   * @see TrigCostData::getSeqAlgROBRequestSize(UInt_t _n, UInt_t _a)
   * @see TrigCostData::getSeqAlgROBRetrievals(UInt_t _n, UInt_t _a)
   * @see TrigCostData::getSeqAlgROBRetrievalSize(UInt_t _n, UInt_t _a)
   * @see TrigCostData::getSeqAlgROBOthers(UInt_t _n, UInt_t _a)
   */
  void TrigCostData::bufferAlgRosInformation() const {
    for (UInt_t _s = 0; _s < getNSequences(); ++_s) {
      for (UInt_t _a = 0; _a < getNSeqAlgs(_s); ++_a) {

        // For this function - recyclable pair holds this algorithm's execution
        _recyclablePair.first = (Int_t) _s;
        _recyclablePair.second = (Int_t) _a;

        // Get the ROS requests I made
        std::set<Int_t> _ros = getSeqAlgROBLocations( _recyclablePair );

        Float_t _rosTime = 0., _robReqSize = 0., _robRetSize = 0.;
        Int_t _robReq = 0, _robRet = 0, _robOther = 0;

        // Loop over the request's ROS call's data
        for ( std::set<Int_t>::iterator _robIt = _ros.begin(); _robIt != _ros.end(); ++_robIt) {

          Int_t _rob = (*_robIt);
          _rosTime += getROBTimer( _rob );

          Int_t _TEMP_REQ = 0, _TEMP_RET = 0;
          for (UInt_t _robData = 0; _robData < getROBDataN( _rob ); ++_robData) {
            if (getIsROBDataRetrieved(_rob, _robData)) {
              ++_robRet;
              ++_TEMP_RET;
              _robRetSize += getROBDataSize(_rob, _robData);
            }
            if (getIsROBDataCached(_rob, _robData)) {
              ++_robReq;
              ++_TEMP_REQ;
              _robReqSize += getROBDataSize(_rob, _robData);
            }
            if ( getIsROBDataUnclassified(_rob, _robData)
              || getIsROBDataDisabled(_rob, _robData)
              || getIsROBDataIgnored(_rob, _robData) ) {
              ++_robOther;
            }
          }
          // This can come out at some point.
          // as the CACHE flag applies only to a ROB, not the ROS request - I am checking here that there is never a
          // ROS request which is listed as both
          assert( (_TEMP_REQ == 0 && _TEMP_RET == 0) || (_TEMP_REQ > 0 && _TEMP_RET == 0) || (_TEMP_REQ == 0 && _TEMP_RET > 0) );
        }

        m_algRosCalls[ _recyclablePair ]         = _ros.size();
        m_algRosTime[ _recyclablePair ]          = _rosTime;
        m_algROBRequests[ _recyclablePair ]      = _robReq;
        m_algROBRetrievals[ _recyclablePair ]    = _robRet;
        m_algROBOther[ _recyclablePair ]         = _robOther;
        m_algROBRequestSize[ _recyclablePair ]   = _robReqSize;
        m_algROBRetrievalSize[ _recyclablePair ] = _robRetSize;

      }
    }
  }

  /**
   * Get the number of calls made to the ROS from an algorithm.
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   */
  UInt_t TrigCostData::getSeqAlgROSCalls(UInt_t _n, UInt_t _a) const {
    _recyclablePair.first = (Int_t) _n;
    _recyclablePair.second = (Int_t) _a;
    return m_algRosCalls[ _recyclablePair ];
  }


  /**
   * Get the total time spent on ROS access for all ROS requests from an algorithm
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   */
  Float_t TrigCostData::getSeqAlgROSTime(UInt_t _n, UInt_t _a) const {
    _recyclablePair.first = (Int_t) _n;
    _recyclablePair.second = (Int_t) _a;
    return m_algRosTime[ _recyclablePair ];
  }

  /**
   * Get the number of ROBs requested (cached) by an algorithm
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   */
  UInt_t TrigCostData::getSeqAlgROBRequests(UInt_t _n, UInt_t _a) const {
    _recyclablePair.first = (Int_t) _n;
    _recyclablePair.second = (Int_t) _a;
    return m_algROBRequests[ _recyclablePair ];
  }

  /**
   * Get the size of all requested (cached) ROBs by an algorithm
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   */
  Float_t TrigCostData::getSeqAlgROBRequestSize(UInt_t _n, UInt_t _a) const {
    _recyclablePair.first = (Int_t) _n;
    _recyclablePair.second = (Int_t) _a;
    return m_algROBRequestSize[ _recyclablePair ];
  }

  /**
   * Get the number of ROBs retrieved by an algorithm
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   */
  UInt_t TrigCostData::getSeqAlgROBRetrievals(UInt_t _n, UInt_t _a) const {
    _recyclablePair.first = (Int_t) _n;
    _recyclablePair.second = (Int_t) _a;
    return m_algROBRetrievals[ _recyclablePair ];
  }

  /**
   * Get the size of all ROBs retrieved by an algorithm
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   */
  Float_t TrigCostData::getSeqAlgROBRetrievalSize(UInt_t _n, UInt_t _a) const {
    _recyclablePair.first = (Int_t) _n;
    _recyclablePair.second = (Int_t) _a;
    return m_algROBRetrievalSize[ _recyclablePair ];
  }

  /**
   * Get the number of ROBs requested by an algorithm which are marked as 'Ignored', 'Disabled' or 'Unclassified'
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   */
  UInt_t TrigCostData::getSeqAlgROBOthers(UInt_t _n, UInt_t _a) const {
    _recyclablePair.first = (Int_t) _n;
    _recyclablePair.second = (Int_t) _a;
    return m_algROBOther[ _recyclablePair ];
  }

  /////////////////////////////////

  /**
   * As algs are already stored under sequences (hence - vector<vector<stuff>>) we cannot go deeper.
   * But algs can have many ROI. So we use a linking index to connect seq_alg_roi_index() with a location in seq_roi().
   * At this location we find a vector with size getSeqAlgNRoI(_seq, _alg) which contains a list of ROI ID
   * These can be accessed in the D3PD by calling getRoIIndexFromId( _roiID ) and then looking at the returned D3PD index location.
   *
   * Given this complexity, it is probably better to look at the example below.
   *
   * @see getRoIIndexFromId( _roiID )
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @param _roi Which ROI to get the ID for - this still needs to be mapped to the location in the D3PD using getRoIIndexFromId
   * @return ID of _roi'th attached region of interest.
   */
  Int_t TrigCostData::getSeqAlgRoIID(UInt_t _n, UInt_t _a, UInt_t _roi) const {
    // Info("TrigCostData::getSeqAlgRoIID"," ---   ---   --- " );
    // Info("TrigCostData::getSeqAlgRoIID","Requested ROI #%i from alg. ", _roi );

    UInt_t _nRoi = getSeqAlgNRoI(_n, _a);
    // Info("TrigCostData::getSeqAlgRoIID","This alg has %i ROIs. ", _nRoi );

    if (_roi >= _nRoi) {
      Warning("TrigCostData::getSeqAlgRoIID", "Requested RoI ID out-of-range");
      return 0;
    }

    UInt_t _sizeOuter = m_trigCostObject->seq_alg_roi_index()->size(); // Range check
    if (_sizeOuter <= _n) {
      Warning("TrigCostData::getSeqAlgRoIID", "Cannot find any RoI lists for alg (out of range).");
      return 0;
    }

    UInt_t _sizeInner = m_trigCostObject->seq_alg_roi_index()->at(_n).size(); // Range check
    if (_sizeInner <= _a) {
      Warning("TrigCostData::getSeqAlgRoIID", "Cannot find location of RoI list for alg (out of range).");
      return 0;
    }

    UInt_t _algRoIs = (Int_t) m_trigCostObject->seq_alg_roi_index()->at(_n).at(_a);
    // Info("TrigCostData::getSeqAlgRoIID"," The requested ROIs have index in the ALG-ROI container %i.", _algRoIs );

    // Info("TrigCostData::getSeqAlgRoIID"," The ALG-ROI container is size %i",  (Int_t) m_trigCostObject->seq_roi()->size() );

    _sizeOuter = m_trigCostObject->seq_roi()->size(); // Range check
    if (_sizeOuter <= _algRoIs) {
      Warning("TrigCostData::getSeqAlgRoIID", "Cannot find RoI list for alg (out of range).");
      return 0;
    }

    _sizeInner = m_trigCostObject->seq_roi()->at(_algRoIs).size(); // Range check
    if (_sizeInner <= _roi) {
      if (Config::config().getDisplayMsg(kMsgRoISize) == kTRUE) {
        Warning("TrigCostData::getSeqAlgRoIID", "Cannot find RoI #%u within the list for this alg (out of range [size:%u]).", _roi, _sizeInner);
      }
      return 0;
    }

    UInt_t _roiID = m_trigCostObject->seq_roi()->at(_algRoIs).at(_roi);
    // Info("TrigCostData::getSeqAlgRoIID","   At location %i of the ALG-ROI container there are %i entries, the %i'th has ROI-ID %i",
    //  _algRoIs, (Int_t) m_trigCostObject->seq_roi()->at(_algRoIs).size(), _roi, _roiID  );

    //UInt_t _roiLocation = getRoIIndexFromId( _roiID );
    // Info("TrigCostData::getSeqAlgRoIID","    The location of ROI with index %i is %i ", _roiID, _roiLocation );

    // const std::string _type = getRoITypeString( _roiLocation );
    // Info("TrigCostData::getSeqAlgRoIID","     This ROI at location %i is type %s", _roiLocation, _type.c_str() );
    return _roiID;
  }

  /**
   * @see TrigCostData::getSeqAlgRoIID(UInt_t _n, UInt_t _a, UInt_t _roi)
   */
  Int_t TrigCostData::getSeqAlgRoILocation(UInt_t _n, UInt_t _a, UInt_t _roi) const {
    return getRoIIndexFromId( getSeqAlgRoIID(_n, _a, _roi) );
  }

  /**
   * Check if this ROB can be associated to an algorithm in the event, if so get the D3PD
   * location of the algorithm.
   * Association is done via checking the algorithm name hash and by time window.
   * @param _n ROB index in D3PD.
   * @return D3PD location in this event of associated algorithm, or (-1,-1) if not found.
   */
  const std::pair< Int_t, Int_t >& TrigCostData::getROBAlgLocation(UInt_t _n) const {
    const static std::pair<Int_t,Int_t> _noMatch(-1,-1);
    if ( m_rosAlgMap.count( _n ) == 0) return _noMatch;
    return m_rosAlgMap[ _n ];
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
    for (UInt_t _s = 0; _s < getNSequences(); ++_s) {
      // Loop over all algorithms in sequence
      for (UInt_t _a = 0; _a < getNSeqAlgs(_s); ++_a) {

        // Map this location for easy access later (in next, ROS loop)
        // This prevents a nested loop in a per-event method which would be mighty slow
        Int_t _seqIndex = getSequenceIndex(_s);
        Int_t _seqAlgPos = getSeqAlgPosition(_s, _a);
        UInt_t _algNameHash = TrigConfInterface::getHLTAlgNameIDFromSeqIDAndAlgPos(_seqIndex, _seqAlgPos);

        m_algExecMap[ _algNameHash ].insert( std::make_pair(_s, _a) );

        // if (Config::config().debug()) {
        //   Info("TrigCostData::mapRosToAlgs","Mapped Alg Name Hash %u to Alg %i,%i", _algNameHash, _s, _a);
        // }
      }
    }

    // Second loop over all ROS, map ROS requests to algorithms.
    for (UInt_t _r = 0; _r < getNROBs(); ++_r) {

      // * NOTE * If we have been told NOT to do ROB information. Here is a good place to break this loop and save on execution time
      if (Config::config().getInt(kEnableROSToAlgMatching) == kFALSE) break;

      UInt_t _rosAlgId = getROBReqID( _r );

      // Check if this alg's exec data is saved
      if ( m_algExecMap.count( _rosAlgId ) == 0) {
        if (Config::config().debug() ) Info("TrigCostData::mapRosToAlgs","No Mapped Alg with name hash %u for ROB # %i", _rosAlgId, _r);
        continue;
      }

      if (Config::config().debug()) {
        Info("TrigCostData::mapRosToAlgs","Finding match for ROB %i", _r);
      }

      // Loop over alg exec's and check timing to see if compatible
      std::set< std::pair< UInt_t, UInt_t > >::iterator _it = m_algExecMap[ _rosAlgId ].begin();
      for (; _it != m_algExecMap[ _rosAlgId ].end(); ++_it) {
        if ( getRosReqBelongsToAlg(_it->first, _it->second, _r) == kFALSE) {
          if ((Config::config().debug() ) && _it == --m_algExecMap[ _rosAlgId ].end() ) {
            Info("TrigCostData::mapRosToAlgs","  Temporal match failed for Alg with hash %u and ROB # %u",  _rosAlgId, _r);
          }
          continue;
        }
        // We have a name hash and temporal match! This alg can be associated to this ROS request.
        // Should only get one match...
        if ( m_rosAlgMap.count( _r ) == 1 ) {
          Warning("TrigCostData::mapRosToAlgs","Found two matches for a ROS request. (%i->%i,%i and %i->%i,%i)",
            _r, _it->first, _it->second, _r, m_rosAlgMap[_r].first, m_rosAlgMap[_r].second );
        }

        //if (Config::config().debug()) {
        //  Info("TrigCostData::mapRosToAlgs"," Mapped Alg %i,%i to ROB # %i", _it->first, _it->second, _r);
        //}
        // Each ROS Req is associated to only one Alg (1-to-1)
        m_rosAlgMap[_r] = (*_it); // This dereferences to the pair
        // Each alg can have many ROS requests
        if ( m_algRosMap.count( *_it ) == 0 ) {
          m_algRosMap[ (*_it) ] = std::set<Int_t>();
        }
        m_algRosMap[ (*_it) ].insert( _r ); // Make the map go both ways (1-to-many)
      }
      if ( (Config::config().debug() ) && m_rosAlgMap.count( _r ) == 0 ) {
        Warning("TrigCostData::mapRosToAlgs"," Found zero matches for a ROS request. (%i)", _r);
      }
    }

  }

  /**
   * Helper function, returns if an algorithm requested a given ROS req based on the time of the request
   * @param _seq D3PD index of sequence to test
   * @param _alg D3PD index of algorithm in sequence to test
   * @param _Ros D3PD index of ROS to test
   * @returns kTRUE if the ROS request time falls within the algorithm execution time.
   */
  Bool_t TrigCostData::getRosReqBelongsToAlg(UInt_t _seq, UInt_t _alg, UInt_t _ros) const {

    if ( isZero(getROBTimeStartSec(_ros)) || isZero(getSeqAlgTimeStartSec(_seq, _alg)) ) return kFALSE;

    Float_t _startTimeDif = ( (Float_t)getROBTimeStartSec(_ros) - (Float_t)getSeqAlgTimeStartSec(_seq, _alg) );
    //Check that this did not go over an hour boundary
    Bool_t _flagStartSpansHourBoundary = kFALSE;
    if (_startTimeDif > 1800.) { //This is 30m
      _startTimeDif = 3600. - _startTimeDif;
      _flagStartSpansHourBoundary = kTRUE;
      if (Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
        Info("TrigCostData::getRosReqBelongsToAlg","Hour Boundary in START AlgStart:%i.%i ROSStart:%i.%i - ROSEnd:%i.%i AlgEnd:%i.%i. TimeDifStart:%f",
              getSeqAlgTimeStartSec(_seq, _alg),
              getSeqAlgTimeStartMicroSec(_seq, _alg),
              getROBTimeStartSec(_ros),
              getROBTimeStartMicroSec(_ros),
              getROBTimeStopSec(_ros),
              getROBTimeStopMicroSec(_ros),
              getSeqAlgTimeStopSec(_seq, _alg),
              getSeqAlgTimeStopMicroSec(_seq, _alg),
              _startTimeDif );
      }
    }
    _startTimeDif += ( (Float_t)getROBTimeStartMicroSec(_ros) - (Float_t)getSeqAlgTimeStartMicroSec(_seq, _alg) ) / 1e6;
    if ( _startTimeDif < 0) {
      if (_flagStartSpansHourBoundary == kTRUE && Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
        Info("TrigCostData::getRosReqBelongsToAlg","Hour boundary in start, therefore modified _startTimeDif:%f"
          " should be >= 0, but this is not the case it seems. Odd - this should be investigated. Failing this alg-ros match.",
         _startTimeDif );
      }
      return kFALSE;
    }

    // Note we reverse the order here as we want to know if the ROS request is in the middle of the alg
    Float_t _stopTimeDif = (Float_t)getSeqAlgTimeStopSec(_seq, _alg) - (Float_t)getROBTimeStopSec(_ros);
    if (_stopTimeDif < -1800.) { //This is 30m
      if (_flagStartSpansHourBoundary == kTRUE) {
        // If both the start and stop appear to span the hour-boundary then this ROS req cannot be contained within the alg
        // Only if one of the two span the hour boundary is this possible
        if (Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
          Info("TrigCostData::getRosReqBelongsToAlg","Hour Boundary in STOP, therefore ROS cannot be associated to this ALG. TimeDifStop:%f", _stopTimeDif );
        }
        return kFALSE;
      }
      _stopTimeDif = 3600. + _stopTimeDif;
      if (Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
        Info("TrigCostData::getRosReqBelongsToAlg","Hour Boundary in STOP AlgStart:%i.%i ROSStart:%i.%i - ROSEnd:%i.%i AlgEnd:%i.%i. TimeDifStop:%f",
              getSeqAlgTimeStartSec(_seq, _alg),
              getSeqAlgTimeStartMicroSec(_seq, _alg),
              getROBTimeStartSec(_ros),
              getROBTimeStartMicroSec(_ros),
              getROBTimeStopSec(_ros),
              getROBTimeStopMicroSec(_ros),
              getSeqAlgTimeStopSec(_seq, _alg),
              getSeqAlgTimeStopMicroSec(_seq, _alg),
              _stopTimeDif );
      }
    }
    _stopTimeDif += ( (Float_t)getSeqAlgTimeStopMicroSec(_seq, _alg) - (Float_t)getROBTimeStopMicroSec(_ros) ) / 1e6;
    if (_flagStartSpansHourBoundary == kTRUE && Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
      Info("TrigCostData::getRosReqBelongsToAlg","Stop TimeDifStop:%f", _stopTimeDif );
    }

    // One of the checks needs to have a veto on == 0 as we can get a ROS on the temporal border between two ALGS
    // e.g. Alg A: AlgStart:339.450639 ROSStart:339.453074 --- ROSEnd:339.453074 AlgEnd:339.453074
    // e.g. Alg B: AlgStart:339.453074 ROSStart:339.453074 --- ROSEnd:339.453074 AlgEnd:339.458076
    // Without this, this ROS req would be assigned to them both.
    if ( _stopTimeDif < 0 || isZero(_stopTimeDif) ) {
      if (_flagStartSpansHourBoundary == kTRUE && Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
        Info("TrigCostData::getRosReqBelongsToAlg","Match failed due to _stopTimeDif <= 0 (%f)", _stopTimeDif );
      }
      return kFALSE;
    }

    if (_flagStartSpansHourBoundary == kTRUE && Config::config().getDisplayMsg(kMsgHourBoundary) == kTRUE) {
      Info("TrigCostData::getRosReqBelongsToAlg","Match suceeded due to _stopTimeDif > 0 (%f)", _stopTimeDif );
    }
    return kTRUE;
  }

  /**
   * Get D3PD index location of a ROI in an event from its ID number.
   * @param _id Read Out Buffer ID number.
   * @return The index location of the ROI or -1 if it could not be found.
   */
  Int_t TrigCostData::getRoIIndexFromId(Int_t _id) const {
    //TODO buffer me.
    for (UInt_t _i = 0; _i < getNRoIs(); ++_i) {
      if (getRoIID(_i) == _id) return _i;
    }
    // Can't find it? try the hack. Note this is a XXX TODO HACK
    for (UInt_t _i = 0; _i < getNRoIs(); ++_i) {
      if ( (_id == 0 || _id == 255) && getRoIID(_i) == 253) {
        if (Config::config().getDisplayMsg(kMsgRoIHack) == kTRUE) {
          Warning("TrigCostData::getRoIIndexFromId","Using RoI hack with %s RoI. NRoIs:%i. Requested RoI ID:%i, returning RoI #:%i with ID:%i",
            getRoITypeString(_i).c_str(),
            getNRoIs(),
            _id,
            _i,
            getRoIID(_i) );
        }
        return _i;
      }
    }
    if (Config::config().getDisplayMsg(kMsgCannotFindRoI) == kTRUE) {
      Warning("TrigCostData::getRoIIndexFromId","No RoI found with ID %i (going to return an invalid location, -1). The posibilities were:", _id);
      for (UInt_t _i = 0; _i < getNRoIs(); ++_i) {
        Warning("TrigCostData::getRoIIndexFromId"," -- RoI at location %i (%s) has ID %i",
         _i,
         this->getRoITypeString(_i).c_str(),
         this->getRoIID(_i));
      }
    }
    return -1;
  }

  /**
   * Return a string representing this ROI's name
   * @param _n RoI ID number.
   * @return Const reference to ROI type string.
   */
  const std::string& TrigCostData::getRoITypeString(Int_t _n) const {
    if( _n >= 0 && _n < (Int_t) getNRoIs()) {
      if (getIsRoINone(_n) == kTRUE) return Config::config().getStr(kNoneString);
      else if (getIsRoIMuon(_n) == kTRUE) return Config::config().getStr(kMuonString);
      else if (getIsRoIEmTau(_n) == kTRUE) {
        return Config::config().getStr(kEmTauString);
        //if (getIsRoITau(_n))  return Config::config().getStr(kTauString); // For later
        //else return Config::config().getStr(kEmString);
      } else if (getIsRoIJet(_n) == kTRUE) return Config::config().getStr(kJetString);
      else if (getIsRoIJetEt(_n) == kTRUE) return Config::config().getStr(kJetEtString);
      else if (getIsRoIEnergy(_n) == kTRUE) return Config::config().getStr(kEnergyString);
    }
    if (Config::config().getDisplayMsg(kMsgUnknownRoIType) == kTRUE) {
      Warning("TrigCostData::getRoITypeString","Encountered an ROI at location:%i with no type.", _n);
    }
    return Config::config().getStr(kUnknownString);
  }

  /**
   * @param _n TE index in D3PD.
   * @return Find location in D3PD of TE with given index
   */
  Int_t TrigCostData::getTEPositionFromTEIndex(UInt_t _n) const {
    for (UInt_t i = 0; i < getNTEs(); ++i) {
      if ( getTEIndex(i) == _n) return _n;
    }
    return -1;
  }

  /**
   * Uses the event number to decide when a new event is processing. At this point, all buffers from the previous
   * event are emptied such that new information can be filled into them.
   */
  void TrigCostData::checkBuffers() const {
    // Check if we have a new event
    //Info("BUFF","CHECK BUFFER d3pd %i comp local %i from %s",this->getEventNumber(), m_bufferEventNumber, getParent()->getName().c_str());

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
