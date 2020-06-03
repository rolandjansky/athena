// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorAlgorithmCommon_H
#define TrigCostRootAnalysis_MonitorAlgorithmCommon_H

// STL include(s):
#include <map>
#include <string>
#include <vector>

// Local include(s):
#include "MonitorBase.h"

namespace TrigCostRootAnalysis {
  //Forward declaration
  class TableColumnFormatter;

  /**
   * @class MonitorAlgorithmCommon
   * Some common code between different monitors which use Algorithm counters
   */
  class MonitorAlgorithmCommon {
  public:
    static void getAllAlgsInEvent(UInt_t level, const TrigCostData* costData);
  protected:
    void addCommonTableEntries(std::vector<MonitorBase::TableColumnFormatter>& toSaveTable);

    struct AlgsInEvent {
      AlgsInEvent(const std::string& algName,
                  const std::string& algClassName,
                  const std::string& seqName,
                  const std::string& chainName,
                  const std::string& chainGroup,
                  Int_t algNameID,
                  Int_t seqD3PDIndex,
                  Int_t algD3PDIndex);
      std::string m_algName; //!< Buffered algorithm name
      std::string m_algClassName; //!< Buffered algorithm class name
      std::string m_seqName; //!< Buffered algorithm sequence name
      std::string m_chainName; //!< Buffered algorithm chain name
      std::string m_chainGroup; //!< Buffered algorithm chain first group, or "" if no group
      Int_t m_algNameID; //!< Buffered hash of algorithm name
      Int_t m_seqD3PDIndex; //!< Buffered D3PD index of alg's sequence
      Int_t m_algD3PDIndex; //!< Buffered D3PD index of alg within sequence
    };

    static std::vector<AlgsInEvent> m_algsInEvent; //!< Static vector of buffered alg primary data for this event
    static Int_t m_eventNumber; //!< Static event number which is buffered
    static UInt_t m_level; //!< Static level which is buffered
  }; //class MonitorAlgorithmCommon
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorAlgorithmCommon_H
