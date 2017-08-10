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
  void AutoMonControl::parseTestXml(TXMLEngine* _xml, XMLNodePointer_t _listNode) {
    XMLNodePointer_t _testNode = _xml->GetChild(_listNode);

    while (_testNode != 0) {
      bool doNotInstantiateTest = false;
      std::string testName, varName, varOpt, doFrac, alert, warn, advise;
      ConfKey_t confVar;
      VariableOption_t confVarOpts;

      if (_xml->HasAttr(_testNode, "name")) {
        testName = _xml->GetAttr(_testNode, "name");
      } else {
        doNotInstantiateTest = true;
      }
      if (_xml->HasAttr(_testNode, "var")) {
        varName = _xml->GetAttr(_testNode, "var");
      } else {
        doNotInstantiateTest = true;
      }
      if (_xml->HasAttr(_testNode, "opt")) {
        varOpt = _xml->GetAttr(_testNode, "opt");
      } else {
        doNotInstantiateTest = true;
      }
      if (_xml->HasAttr(_testNode, "alert")) {
        alert = _xml->GetAttr(_testNode, "alert");
      } else {
        doNotInstantiateTest = true;
      }
      if (_xml->HasAttr(_testNode, "dofrac")) {
        doFrac = _xml->GetAttr(_testNode, "dofrac");
      } else {
        doFrac = "false";
      }
      if (_xml->HasAttr(_testNode, "warn")) {
        warn = _xml->GetAttr(_testNode, "warn");
      }
      if (_xml->HasAttr(_testNode, "advise")) {
        advise = _xml->GetAttr(_testNode, "advise");
      }

      if (!Config::config().getConfKeyNameFromString(varName, confVar)) {
        doNotInstantiateTest = true;
        const std::string varNameError = "varName not recognised : " + varName;
        Error("AutoMonControl::parseTestXml", varNameError.c_str());
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
        const std::string varOptError = "Unkown Variable Option, please use Call, Event, EventFraction or Period";
        Error("AutoMonControl::parseTestXml", varOptError.c_str());
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
        const std::string testInfoString = "Making Test " + testName;
        Info("AutoMonControl::parseTestXml", testInfoString.c_str());

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
        const std::string errorString = "Error when making test, XML parsing failed. Name (if able to retrieve) : " +
                                        testName;
        const std::string infoString = "Continuing without adding Test";
        Error("AutoMonControl::parseTestXml", errorString.c_str());
        Info("infoString", infoString.c_str());
      }

      _testNode = _xml->GetNext(_testNode); //iterate to next test node
    }//end loop over test Nodes
  }//end parseExamXml

  /**
   * Parse the control options, currently only the interest level
   */
  void AutoMonControl::parseControlOptionsXml(TXMLEngine* _xml, XMLNodePointer_t _listNode) {
    std::string levelString = "alert";
    if (_xml->GetAttr(_listNode, "level")) {
      levelString = _xml->GetAttr(_listNode, "level");
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
  void AutoMonControl::parseExamXml(TXMLEngine* _xml, XMLNodePointer_t _listNode) {
    XMLNodePointer_t _examNode = _xml->GetChild(_listNode);
    MonitorBase* monitor;

    while (_examNode != 0) {
      bool doNotInstantiateExam = false;
      bool userDefinedRange = false;
      std::string nameString, monitorString, excludeString, specifyString, testsString, rangeString;
      monitor = nullptr;

      if (_xml->HasAttr(_examNode, "name")) {
        nameString = _xml->GetAttr(_examNode, "name");
      } else {
        doNotInstantiateExam = true;
      }
      if (_xml->HasAttr(_examNode, "monitor")) {
        monitorString = _xml->GetAttr(_examNode, "monitor");
      } else {
        doNotInstantiateExam = true;
      }
      if (_xml->HasAttr(_examNode, "tests")) {
        testsString = _xml->GetAttr(_examNode, "tests");
      } else {
        doNotInstantiateExam = true;
      }
      if (_xml->HasAttr(_examNode, "exclude")) {
        excludeString = _xml->GetAttr(_examNode, "exclude");
      }
      if (_xml->HasAttr(_examNode, "specify")) {
        specifyString = _xml->GetAttr(_examNode, "specify");
      }
      if (_xml->HasAttr(_examNode, "range")) {
        rangeString = _xml->GetAttr(_examNode, "range");
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
      _examNode = _xml->GetNext(_examNode);   //Move on to next exam
    } // end of loop over exam nodes
  } // end parseExamXml

  /**
   * Iterate through the XML and parse the different options.
   */
  void AutoMonControl::loadXml() {
    TXMLEngine* _xml = new TXMLEngine();
    XMLDocPointer_t _xmlDoc = _xml->ParseFile(m_xml_location.c_str());
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);

    if (_xml->GetNodeName(_mainNode) != std::string("AutoCostMon")) {
      std::cout << "XML Head node not correct (AutoCostMon)" << '\n';
    }
    XMLNodePointer_t _listNode = _xml->GetChild(_mainNode);
    while (_listNode != 0) {
      const std::string _listName = _xml->GetNodeName(_listNode);
      if (_listName == "ControlOptions") {
        parseControlOptionsXml(_xml, _listNode);
      }

      if (_listName == "TestDef") {
        parseTestXml(_xml, _listNode);
      }

      if (_listName == "ExamDef") {
        parseExamXml(_xml, _listNode);
      }
      _listNode = _xml->GetNext(_listNode); //go to next node
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