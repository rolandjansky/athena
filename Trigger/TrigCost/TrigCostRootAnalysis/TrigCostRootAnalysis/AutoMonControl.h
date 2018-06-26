// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Chris McNicol <christopher.john.mcnicol@cern.ch>
// -------------------------------------------------------------

#ifndef TrigCostRootAnalysis_AutoMonControl_H
#define TrigCostRootAnalysis_AutoMonControl_H

#include "TrigCostRootAnalysis/ProcessEvent.h"
#include "TrigCostRootAnalysis/AutoMonResult.h"
#include "TrigCostRootAnalysis/AutoMonTest.h"
#include "TrigCostRootAnalysis/AutoMonExam.h"

#include <TXMLEngine.h>
#include <TObjString.h>
#include <TDOMParser.h>
#include <TXMLNode.h>
#include <TXMLDocument.h>
#include <TXMLAttr.h>

namespace TrigCostRootAnalysis {
  class AutoMonControl {
  public:
    AutoMonControl(const std::string& xmlLoc, ProcessEvent* procEvent);
    AutoMonControl(const std::string& xmlLoc, ProcessEvent* procEvent, const AlertStatus level);

    template<typename Out>
    void split(const std::string& s, char delim, Out result) const;
    std::vector<std::string> split(const std::string& s, char delim) const;

    void parseExamXml(TXMLEngine* xml, XMLNodePointer_t examNode);
    void parseTestXml(TXMLEngine* xml, XMLNodePointer_t node);
    void parseControlOptionsXml(TXMLEngine* xml, XMLNodePointer_t listNode);
    void loadXml();

    void bankTest(const AutoMonTest test);
    void bankExam(const AutoMonExam exam);
    const AutoMonTest* getTest(const std::string& TestName) const;
    std::vector<const AutoMonTest* > getTestList(const std::vector< std::string > testList) const;

    std::map< const std::string, AutoMonTest > m_test_collection;
    std::map< const std::string, AutoMonExam > m_exam_collection;

    Float_t thresholdFromString(const std::string& s) const;
    void runAutoMonitoring();
  private:
    std::string m_xml_location;
    ProcessEvent* m_process_event;
    AutoMonResult m_test_results;
  };
} //namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_AutoMonControl_H
