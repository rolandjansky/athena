// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>
#include <fstream> //TODO remove me
#include <algorithm>

// ROOT include(s):
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TError.h>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterFullEvent.h"
#include "../TrigCostRootAnalysis/CounterAlgorithm.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Utility.h"

namespace TrigCostRootAnalysis {
  /**
   * Trigger Element counter constructor. Sets values of internal variables.
   * @param _name Name of TE counter
   * @param _ID ID number of TE counter.
   */
  CounterFullEvent::CounterFullEvent(const TrigCostData* _costData, const std::string& _name, Int_t _ID,
                                     UInt_t _detailLevel, MonitorBase* _parent)
    : CounterBase(_costData, _name, _ID, _detailLevel, _parent),
    m_isRun(kFALSE),
    m_algCounters() {
  }

  /**
   * Counter destructor. Delete my vector of counters
   */
  CounterFullEvent::~CounterFullEvent() {
    for (std::vector<CounterAlgorithm*>::iterator _it = m_algCounters.begin(); _it != m_algCounters.end(); ++_it) {
      delete (*_it);
    }
    m_algCounters.clear();
  }

  /**
   * Reset per-event counter(s). Not used currently for TriggerElement counter.
   */
  void CounterFullEvent::startEvent() {
  }

  /**
   * Investigate the Trigger Element structure for a single event.
   * Not yet implemented.
   * @param _e Unused.
   * @param _f Unused.
   * @param _weight Event weight.
   */
  void CounterFullEvent::processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight) {
    ++m_calls;

    UNUSED(_e);
    UNUSED(_f);

    if (m_isRun == kTRUE) {
      Error("CounterFullEvent::processEventCounter", "Each FullEvent Counter should only be run on one event");
      return;
    }
    m_isRun = kTRUE;

    // Nest some CounterAlgorithm and CounterChain's to hold this event's data.
    // Note we explicitly turn off histogramming

    // // Get chain data for event
    // Bool_t _first = kTRUE;
    // for (UInt_t _c = 0; _c < m_costData->getNChains(); ++_c) {

    //   Int_t _chainID = m_costData->getChainID(_c);
    //   const std::string _chainName = TrigConfInterface::getHLTNameFromChainID( _chainID,
    // m_costData->getChainLevel(_c) );
    //   counterMapIt _it = m_chainCounters.find( _chainName );
    //   if ( _it == m_chainCounters.end() ) {

    //     _counter =  (CounterBase*) new CounterChain( _chainName,  _chainID, /*detail = */ 0 );
    //     m_chainCounters[_chainName] = _counter;
    //     if (_first == kTRUE) {
    //       _first = kFALSE;
    //       _counter->startEvent();
    //     }
    //   } else {
    //     _counter = (*_it).second;
    //   }
    //   _counter->processEventCounter( m_costData, _c, 0, _weight );
    // }

    // for (counterMapIt _it = m_chainCounters.begin(); _it != m_chainCounters.end(); ++_it) {
    //   _it->second->endEvent();
    // }

    // Get all alg data for event
    for (UInt_t _s = 0; _s < m_costData->getNSequences(); ++_s) {
      //Int_t _chainID = m_costData->getSequenceChannelCounter(_s);
      //const std::string _chainName = TrigConfInterface::getHLTNameFromChainID( _chainID,
      // m_costData->getSequenceLevel(_s) );
      for (UInt_t _a = 0; _a < m_costData->getNSeqAlgs(_s); ++_a) {
        Int_t _seqIndex = m_costData->getSequenceIndex(_s);
        Int_t _seqAlgPos = m_costData->getSeqAlgPosition(_s, _a);

        const std::string _algName = TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos(_seqIndex, _seqAlgPos);
        const std::string _algType = TrigConfInterface::getHLTAlgClassNameFromSeqIDAndAlgPos(_seqIndex, _seqAlgPos);
        Int_t _seqAlgNameHash = TrigConfInterface::getHLTAlgClassNameIDFromSeqIDAndAlgPos(_seqIndex, _seqAlgPos);

        // Exceptionally here we make a new sub-counter for every algorithm to capture the fine details of the execution

        CounterAlgorithm* _counter = new CounterAlgorithm(m_costData, _algName, _seqAlgNameHash, /*detail = */ 0);
        //_counter->decorate( "algClassName", _algType );
        _counter->processEventCounter(_s, _a, _weight);
        m_algCounters.push_back(_counter);
      }
    }

    //Sort into a nice time order
    std::sort(m_algCounters.begin(), m_algCounters.end());

    // Code here. Suppress warnings for now.
    debug(_e);
  }

  /**
   * Perform end-of-event monitoring. Does nothing for FullEvent
   */
  void CounterFullEvent::endEvent(Float_t _weight) {
    UNUSED(_weight);
  }

  /**
   * When running with prescales applied. This function returns how the counter should be scaled for the current call.
   * This is an odd case, the FullEvent counter actually runs Alg counters which can pick this up on their own
   * Hence this function is not used here
   * @return Multiplicative weighting factor
   */
  Double_t CounterFullEvent::getPrescaleFactor(UInt_t _e) {
    UNUSED(_e);
    return 0.;
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterFullEvent::debug(UInt_t _e) {
    UNUSED(_e);
    // Loop over TEs
    std::ofstream fout(std::string("te_test" + getName() + ".dot").c_str());
    fout << "digraph G{" << std::endl;
    for (UInt_t i = 0; i < m_costData->getNTEs(); ++i) {
      fout << "\t" << m_costData->getTEIndex(i) << "[label=\"<f0>" <<

      (m_costData->getIsTERegularTe(i) ? "R|" : "") <<
      (m_costData->getIsTEInitial(i) ? " I|" : "") <<
      (m_costData->getIsTERoITe(i) ? "RoI|" : "") <<
      (m_costData->getIsTEL1Threshold(i) ? "L1|" : "") <<

        "<f1> " <<

      (m_costData->getIsTEActiveState(i) ? "A|" : "") <<
      (m_costData->getIsTEErrorState(i) ? "E|" : "") <<
      (m_costData->getIsTEOutputEFNode(i) ? "OEF|" : "") <<
      (m_costData->getIsTEOutputL2Node(i) ? "OL2|" : "") <<
      (m_costData->getIsTETerminalNode(i) ? "TRM|" : "") <<
      (m_costData->getIsTETopologicalTe(i) ? "TOPO|" : "") <<

        "<f2> ";


      for (UInt_t c = 0; c < m_costData->getTERoIIDSize(i); ++c) {
        Int_t r = m_costData->getRoIIndexFromId(m_costData->getTERoIIDIndex(i, c));
        if (r != -1 && r != 255) {
          fout << "RoIThr:" << m_costData->getRoINL1Thresh(r) << "-";
          if (m_costData->getIsRoIEmTau(r)) fout << "EMTAU";
          if (m_costData->getIsRoIEnergy(r)) fout << "E";
          if (m_costData->getIsRoIJet(r)) fout << "JET";
          if (m_costData->getIsRoIJetEt(r)) fout << "JETET";
          if (m_costData->getIsRoIMuon(r)) fout << "MU";
          if (m_costData->getIsRoINone(r)) fout << "NONE";
          fout << " (" << m_costData->getRoIEta(r) << "," << m_costData->getRoIPhi(r) << ")|";
        }
      }
      fout << "\"";
      fout << std::endl << "\tshape = \"record\"];" << std::endl;

      std::cout << "TE Index:" << m_costData->getTEIndex(i) << " ID:" << m_costData->getTEID(i) << " Children:{";
      for (UInt_t c = 0; c < m_costData->getTEChildSize(i); ++c) {
        std::cout << m_costData->getTEChildIndex(i, c) << ",";
        fout << "\t" << m_costData->getTEIndex(i) << ":f0 -> " << m_costData->getTEChildIndex(i, c) << ";" << std::endl;
      }
      std::cout << "} Parents:{";
      for (UInt_t c = 0; c < m_costData->getTEParentSize(i); ++c) {
        std::cout << m_costData->getTEParentIndex(i, c) << ",";
      }
      std::cout << "} CLID:{";
      for (UInt_t c = 0; c < m_costData->getTECLIDSize(i); ++c) {
        std::cout << m_costData->getTECLIDIndex(i, c) << ",";
      }
      std::cout << "} RoIID:{";
      for (UInt_t c = 0; c < m_costData->getTERoIIDSize(i); ++c) {
        std::cout << m_costData->getTERoIIDIndex(i, c) << "[";
        Int_t r = m_costData->getRoIIndexFromId(m_costData->getTERoIIDIndex(i, c));
        if (r != -1) {
          std::cout << "NL1Thresh:" << m_costData->getRoINL1Thresh(r) << "|";
          if (m_costData->getIsRoIEmTau(r)) std::cout << "EMTAU|";
          if (m_costData->getIsRoIEnergy(r)) std::cout << "ENERGY|";
          if (m_costData->getIsRoIJet(r)) std::cout << "JET|";
          if (m_costData->getIsRoIJetEt(r)) std::cout << "JETET|";
          if (m_costData->getIsRoIMuon(r)) std::cout << "MU|";
          if (m_costData->getIsRoINone(r)) std::cout << "NONE|";
          std::cout << m_costData->getRoIEta(r) << "|" << m_costData->getRoIPhi(r);
        }
        std::cout << "],";
      }
      std::cout << "}";
      if (m_costData->getIsTERegularTe(i)) std::cout << " <isRegularTE>";
      if (m_costData->getIsTEInitial(i)) std::cout << " <isInitialTe>";
      if (m_costData->getIsTERoITe(i)) std::cout << " <isRoITE>";
      if (m_costData->getIsTEL1Threshold(i)) std::cout << " <isL1Thresh>";
      if (m_costData->getIsTEActiveState(i)) std::cout << " isActiveState";
      if (m_costData->getIsTEErrorState(i)) std::cout << " isErrorState";
      if (m_costData->getIsTEOutputEFNode(i)) std::cout << " isOutputEFNode";
      if (m_costData->getIsTEOutputL2Node(i)) std::cout << " isOutputL2Node";
      if (m_costData->getIsTETerminalNode(i)) std::cout << " isTerminalNode";
      if (m_costData->getIsTETopologicalTe(i)) std::cout << " isTopoTE";
      std::cout << std::endl;
    }
    fout << "}";
    fout.close();
  }
} // namespace TrigCostRootAnalysis
