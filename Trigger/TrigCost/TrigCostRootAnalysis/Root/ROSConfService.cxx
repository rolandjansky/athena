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
#include <sstream>
#include "cstdlib" //std::getenv

// Local include(s):
#include "../TrigCostRootAnalysis/ROSConfService.h"
#include "../TrigCostRootAnalysis/Config.h"

// ROOT includes
#include <TSystem.h>
#include <TXMLEngine.h>
#include <TObjString.h>
#include <TDOMParser.h>
#include <TXMLNode.h>
#include <TXMLDocument.h>
#include <TXMLAttr.h>
#include <TError.h>


namespace TrigCostRootAnalysis {
  /**
   * Construct singleton, register histograms
   */
  ROSConfService::ROSConfService() : m_serviceEnabled(kFALSE) {
    parseRosXml();
  }

  /**
   * Singleton class retrieval call.
   * @return Reference to the singleton class object.
   */
  ROSConfService& ROSConfService::rosConfService() { // Instance getter
    static ROSConfService instance; // Guaranteed to be destroyed.

    return instance;        // Instantiated on first use.
  }

  /**
   * Get the name of a ROBIN from its ID within the ntuple. Assumes that parseRosXml has been called.
   * @param Id ID number of ROB (hexdecimal identifier stored in UInt_t)
   * @return Const reference to name of the ROBIN this ROB is attached to, or "UNKNOWN" on fail.
   */
  const std::string& ROSConfService::getRobinNameFromId(UInt_t Id) {
    if (m_serviceEnabled == kFALSE) {
      return Config::config().getStr(kUnknownString);
    }

    IntStringMapIt_t location = m_RobToRobinMap.find(Id);
    if (location == m_RobToRobinMap.end()) {
      return Config::config().getStr(kUnknownString);
    }
    return location->second;
  }

  /**
   * Get the name of a ROS from a ROBIN. Assumes that parseRosXml has been called.
   * Note: High call rate function.
   * @param robinName Name of the ROBIN
   * @return Const reference to name of the ROS the ROBIN is associated with, or "UNKNOWN" on fail.
   */
  const std::string& ROSConfService::getRosNameFromFromRobinName(const std::string& robinName) {
    if (m_serviceEnabled == kFALSE) {
      return Config::config().getStr(kUnknownString);
    }

    StringStringMapIt_t location = m_RobinToRosMap.find(robinName);
    if (location == m_RobinToRosMap.end()) {
      return Config::config().getStr(kUnknownString);
    }
    return location->second;
  }

  /**
   * Read ROS-SubDetector name mappings from XML file. Does not change often. To be kept up to date in /data.
   */
  void ROSConfService::parseRosXml() {
    if (Config::config().getIsSet(kROSXMLPath) == kFALSE) {
      Error("ROSConfService::parseRosXml", "Unable to find ROS mapping: %s. No named ROS data.",
            Config::config().getStr(kROSXMLName).c_str());
      return;
    }

    m_RobinToRosMap.clear();
    m_RobToRobinMap.clear();

    // Now try to parse xml file
    // Only file with restricted xml syntax are supported
    TXMLEngine* xml = new TXMLEngine();
    XMLDocPointer_t xmlDoc = xml->ParseFile(Config::config().getStr(kROSXMLPath).c_str());
    if (xmlDoc == 0) {
      Error("ROSConfService::parseRosXml", "Unable to parse ROS mapping: %s. No named ROS data.",
            Config::config().getStr(kROSXMLPath).c_str());
      delete xml;
      return;
    }

    // Get access to main node
    XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);
    assert(xml->GetNodeName(mainNode) == std::string("ros-rob-data"));

    XMLNodePointer_t RosNode = xml->GetChild(mainNode);
    while (RosNode != 0) { // Loop over all ROS'
      const std::string RosName = xml->GetAttrValue(xml->GetFirstAttr(RosNode));
      XMLNodePointer_t RobinNode = xml->GetChild(RosNode);
      while (RobinNode != 0) { // Loop over all ROS' ROBINs
        const std::string RobinName = xml->GetAttrValue(xml->GetFirstAttr(RobinNode));
        m_RobinToRosMap[RobinName] = RosName;
        XMLNodePointer_t RobNode = xml->GetChild(RobinNode);
        while (RobNode != 0) { // Loop over all ROBIN's RODs
          const std::string RobIdStr = xml->GetAttrValue(xml->GetFirstAttr(RobNode));
          std::stringstream ss;
          ss << std::hex << RobIdStr;
          UInt_t RobId = 0;
          ss >> RobId;
          m_RobToRobinMap[RobId] = RobinName;
          RobNode = xml->GetNext(RobNode);
        }
        RobinNode = xml->GetNext(RobinNode);
      }
      RosNode = xml->GetNext(RosNode);
    }

    if (Config::config().debug()) {
      Info("ROSConfService::parseRosXml", "Using ROS <-> ROBIN and ROBIN <-> ROB Maps from %s",
           Config::config().getStr(kROSXMLName).c_str());
      for (StringStringMapIt_t it = m_RobinToRosMap.begin(); it != m_RobinToRosMap.end(); ++it) {
        Info("ROSConfService::parseRosXml", "\t%s -> %s", it->first.c_str(), it->second.c_str());
      }
      for (IntStringMapIt_t it = m_RobToRobinMap.begin(); it != m_RobToRobinMap.end(); ++it) {
        Info("ROSConfService::parseRosXml", "\t%x -> %s", it->first, it->second.c_str());
      }
    } else {
      Info("ROSConfService::parseRosXml", "Parsed ROS configuration %s. Read %i ROS nodes, %i ROBINs.",
           Config::config().getStr(kROSXMLName).c_str(),
           (Int_t) m_RobinToRosMap.size(),
           (Int_t) m_RobToRobinMap.size());
    }

    m_serviceEnabled = kTRUE;
    delete xml;
  }
} // namespace TrigCostRootAnalysis
