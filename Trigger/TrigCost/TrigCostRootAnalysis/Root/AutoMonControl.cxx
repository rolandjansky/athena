// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Chris McNicol <christopher.john.mcnicol@cern.ch>
// -------------------------------------------------------------


#include <sstream>

#include "TrigCostRootAnalysis/AutoMonExam.h"
#include "TrigCostRootAnalysis/AutoMonControl.h"
#include "TrigCostRootAnalysis/MonitorBase.h"
#include "TrigCostRootAnalysis/CounterBase.h"
#include "TrigCostRootAnalysis/Config.h"


namespace TrigCostRootAnalysis {
  AutoMonControl::AutoMonControl(const std::string& xmlLoc, ProcessEvent* procEvent) :
    m_xml_location(xmlLoc),
    m_process_event(procEvent) {
    m_test_results = AutoMonResult(AlertStatus::ALERT);
  }

  AutoMonControl::AutoMonControl(const std::string& xmlLoc, ProcessEvent* procEvent, const AlertStatus level) :
    m_xml_location(xmlLoc),
    m_process_event(procEvent) {
    m_test_results = AutoMonResult(level);
  }

  /**
   * Split string by delimiter.
   */
  template<typename Out>
  void AutoMonControl::split(const std::string& str, char delim, Out resultString) const {
    std::stringstream ss;
    ss.str(str);
    std::string item;
    while (std::getline(ss, item, delim)) {
      *(resultString++) = item;
    }
  }

  /**
   * Get vector of strings from single string split by delimiter.
   * @param str String to be decoded.
   * @param delim Delimiter to use.
   * @returns Vector of strings from one string, split by delimiter.
   */
  std::vector<std::string> AutoMonControl::split(const std::string& str, char delim) const {
    std::vector<std::string> elems;
    split(str, delim, std::back_inserter(elems));
    return elems;
  } //end split

  /**
   * Get list of pointers to tests specified in argument.
   * @param testList List of test names to search.
   * @returns vector of pointers to tests found that match requested names.
   */
  std::vector< const AutoMonTest* > AutoMonControl::getTestList(const std::vector< std::string > testList) const {
    std::vector< const AutoMonTest* > resultList;
    for (auto iter = testList.begin(); iter != testList.end(); ++iter) {
      const AutoMonTest* resultTest = getTest(*iter);
      if (resultTest) {
        resultList.push_back(resultTest);
      }
    }
    return resultList;
  }// end getTestList

  /**
   * Get pointer to test that matches TestName
   * @param TestName Name of test to return.
   * @returns Pointer to test that matches TestName.
   */
  const AutoMonTest* AutoMonControl::getTest(const std::string& TestName) const {
    for (auto iter = m_test_collection.begin(); iter != m_test_collection.end(); ++iter) {
      if (iter->first == TestName) {
        return &(iter->second);
      }
    }
    return nullptr;
  }

  /**
   * Add test to internal collection of tests.
   * @param test Test to be added.
   */
  void AutoMonControl::bankTest(const AutoMonTest test) {
    const std::string testName = test.getTestName();

    m_test_collection.insert(std::make_pair(testName, test));
  }

  /**
   * Add exam to internal collection of exams.
   * @param exam Exam to be added.
   */
  void AutoMonControl::bankExam(const AutoMonExam exam) {
    const std::string examName = exam.getExamName();

    m_exam_collection.insert(std::make_pair(examName, exam));
  }

  /**
   * Get a Float_t to use as a threshold from the input string.
   * @param str String to be decoded as threshold.
   */
  Float_t AutoMonControl::thresholdFromString(const std::string& str) const {
    return static_cast<Float_t>(std::stod(str));
  }// end thresholdFromString

  /**
   * Parse information about Tests from XML and create an AutoMonTest object.
   */
  void AutoMonControl::parseTestXml(TXMLEngine* xml, XMLNodePointer_t listNode) {
    XMLNodePointer_t testNode = xml->GetChild(listNode);

    while (testNode != 0) {
      bool doNotInstantiateTest = false;
      std::string testName, varName, varOpt, doFrac, alert, warn, advise;
      ConfKey_t confVar;
      VariableOption_t confVarOpts;

      if (xml->HasAttr(testNode, "name")) {
        testName = xml->GetAttr(testNode, "name");
      } else {
        doNotInstantiateTest = true;
      }
      if (xml->HasAttr(testNode, "var")) {
        varName = xml->GetAttr(testNode, "var");
      } else {
        doNotInstantiateTest = true;
      }
      if (xml->HasAttr(testNode, "opt")) {
        varOpt = xml->GetAttr(testNode, "opt");
      } else {
        doNotInstantiateTest = true;
      }
      if (xml->HasAttr(testNode, "alert")) {
        alert = xml->GetAttr(testNode, "alert");
      } else {
        doNotInstantiateTest = true;
      }
      if (xml->HasAttr(testNode, "dofrac")) {
        doFrac = xml->GetAttr(testNode, "dofrac");
      } else {
        doFrac = "false";
      }
      if (xml->HasAttr(testNode, "warn")) {
        warn = xml->GetAttr(testNode, "warn");
      }
      if (xml->HasAttr(testNode, "advise")) {
        advise = xml->GetAttr(testNode, "advise");
      }

      if (!Config::config().getConfKeyNameFromString(varName, confVar)) {
        doNotInstantiateTest = true;
        Error("AutoMonControl::parseTestXml", "varName not recognised : %s",
              varName.c_str());
      }

      if (varOpt == "call" || varOpt == "Call") {
        confVarOpts = kSavePerCall;
      } else if (varOpt == "Event" || varOpt == "event") {
        confVarOpts = kSavePerEvent;
      } else if (varOpt == "EventFraction" || varOpt == "fraction") {
        confVarOpts = kSavePerEventFraction;
      } else if (varOpt == "period" || varOpt == "Period") {
        confVarOpts = kSavePerPeriod;
      } else {
        Error("AutoMonControl::parseTestXml","Unkown Variable Option, please use Call, Event, EventFraction or Period");
        doNotInstantiateTest = true;
      }

      if (!doNotInstantiateTest) {
        bool fracArg = false;
        if (doFrac == "False" || doFrac == "false" || doFrac == "f" || doFrac == "F") {
          fracArg = false;
        } else if (doFrac == "True" || doFrac == "true" || doFrac == "t" || doFrac == "T") {
          fracArg = true;
        } else {
          Error("AutoMonControl::parseTestXml", "Unknown entry for doFrac, using 'false'");
        }

        AutoMonTest test = AutoMonTest(testName, confVar, confVarOpts, fracArg, &m_test_results);
        Info("AutoMonControl::parseTestXml", "Making Test %s", testName.c_str());

        if (advise == "") {
          if (warn == "") {
            test.setThresholds(thresholdFromString(alert));
          } else {
            test.setThresholds(thresholdFromString(alert), thresholdFromString(warn));
          }
        } else {
          test.setThresholds(thresholdFromString(alert), thresholdFromString(warn), thresholdFromString(advise));
        }
        bankTest(test);
      } else {
        Error("AutoMonControl::parseTestXml", "Error when making test, XML parsing failed. Name (if able to retrieve) : %s",
              testName.c_str());
        Info("infoString", "Continuing without adding Test");
      }

      testNode = xml->GetNext(testNode); //iterate to next test node
    }//end loop over test Nodes
  }//end parseExamXml

  /**
   * Parse the control options, currently only the interest level
   */
  void AutoMonControl::parseControlOptionsXml(TXMLEngine* xml, XMLNodePointer_t listNode) {
    std::string levelString = "alert";
    if (xml->GetAttr(listNode, "level")) {
      levelString = xml->GetAttr(listNode, "level");
    }

    if (levelString == "warn" || levelString == "warning" || levelString == "Warn" || levelString == "Warning" ||
        levelString == "w" || levelString == "W") {
      m_test_results.setInterestLevel(AlertStatus::WARNING);
    }
    if (levelString == "advise" || levelString == "advisory" || levelString == "Advise" || levelString == "Advisory") {
      m_test_results.setInterestLevel(AlertStatus::ADVISORY);
    }
  }// end parseControloptionsXml

  /**
   * Parse information about exam and create AutoMonExam object.
   */
  void AutoMonControl::parseExamXml(TXMLEngine* xml, XMLNodePointer_t listNode) {
    XMLNodePointer_t examNode = xml->GetChild(listNode);
    MonitorBase* monitor;

    while (examNode != 0) {
      bool doNotInstantiateExam = false;
      bool userDefinedRange = false;
      std::string nameString, monitorString, excludeString, specifyString, testsString, rangeString;
      monitor = nullptr;

      if (xml->HasAttr(examNode, "name")) {
        nameString = xml->GetAttr(examNode, "name");
      } else {
        doNotInstantiateExam = true;
      }
      if (xml->HasAttr(examNode, "monitor")) {
        monitorString = xml->GetAttr(examNode, "monitor");
      } else {
        doNotInstantiateExam = true;
      }
      if (xml->HasAttr(examNode, "tests")) {
        testsString = xml->GetAttr(examNode, "tests");
      } else {
        doNotInstantiateExam = true;
      }
      if (xml->HasAttr(examNode, "exclude")) {
        excludeString = xml->GetAttr(examNode, "exclude");
      }
      if (xml->HasAttr(examNode, "specify")) {
        specifyString = xml->GetAttr(examNode, "specify");
      }
      if (xml->HasAttr(examNode, "range")) {
        rangeString = xml->GetAttr(examNode, "range");
        userDefinedRange = true;
      }

      ConfKey_t monitorName;
      if (Config::config().getConfKeyNameFromString(monitorString, monitorName)) {
        monitor = m_process_event->getMonitor(monitorName);
      } else {
        std::cout << "Can't find Conf Key for monitor Name : " << monitorName << '\n';
      }

      //Instantiate the exam and add it to exam List. Need to choose correct constructor.
      if (monitor && !doNotInstantiateExam) {
        std::vector<std::string > excludeList = split(excludeString, ',');
        std::vector<std::string > specifyList = split(specifyString, ',');
        std::vector< const AutoMonTest* > testList = getTestList(split(testsString, ','));

        // If the user defines counters to be included or excluded
        if (specifyList.size() != 0 || excludeList.size() != 0) {
          //If the user defines a particular range, if not, default "All" is used
          if (userDefinedRange) {
            AutoMonExam exam = AutoMonExam(nameString, this, monitor, excludeList, specifyList, testList, rangeString);
            bankExam(exam);
          } else {
            AutoMonExam exam = AutoMonExam(nameString, this, monitor, excludeList, specifyList, testList);
            bankExam(exam);
          }
        } // end if exclude or specify are set
        else {
          if (userDefinedRange) {
            AutoMonExam exam = AutoMonExam(nameString, this, monitor, testList, rangeString);
            bankExam(exam);
          } else {
            AutoMonExam exam = AutoMonExam(nameString, this, monitor, testList);
            bankExam(exam);
          }
        }
      } //end if exam is to be instantiated
      else {
        if (!monitor) {
          std::cout << "Failed to get Monitor with name : " << monitorString << '\n';
        } else {
          std::cout << "Please check you have specified a valid exam name, monitor and list of tests " << '\n';
          std::cout << "Recovered values   name = " << nameString << " monitor = " << monitorString << " tests = " <<
          testsString << '\n';
        }
      }
      examNode = xml->GetNext(examNode);   //Move on to next exam
    } // end of loop over exam nodes
  } // end parseExamXml

  /**
   * Iterate through the XML and parse the different options.
   */
  void AutoMonControl::loadXml() {
    TXMLEngine* xml = new TXMLEngine();
    XMLDocPointer_t xmlDoc = xml->ParseFile(m_xml_location.c_str());
    XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);

    if (xml->GetNodeName(mainNode) != std::string("AutoCostMon")) {
      std::cout << "XML Head node not correct (AutoCostMon)" << '\n';
    }
    XMLNodePointer_t listNode = xml->GetChild(mainNode);
    while (listNode != 0) {
      const std::string listName = xml->GetNodeName(listNode);
      if (listName == "ControlOptions") {
        parseControlOptionsXml(xml, listNode);
      }

      if (listName == "TestDef") {
        parseTestXml(xml, listNode);
      }

      if (listName == "ExamDef") {
        parseExamXml(xml, listNode);
      }
      listNode = xml->GetNext(listNode); //go to next node
    } // end loop over nodes
  }// end loadxml

  /**
   * Run each exam in the collection, save the output and print the results.
   */
  void AutoMonControl::runAutoMonitoring() {
    for (auto exam_iter = m_exam_collection.begin(); exam_iter != m_exam_collection.end(); ++exam_iter) {
      (exam_iter->second).runExam();
    }// end loop over exam collection
    m_test_results.saveOutput();
    m_test_results.printResults();
  }// end runAutoMonitoring
}// namespace TrigCostRootAnalysis
