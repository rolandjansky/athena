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
   * @param _Id ID number of ROB (hexdecimal identifier stored in UInt_t)
   * @return Const reference to name of the ROBIN this ROB is attached to, or "UNKNOWN" on fail.
   */
  const std::string& ROSConfService::getRobinNameFromId(UInt_t _Id) {
    if (m_serviceEnabled == kFALSE) {
      return Config::config().getStr(kUnknownString);
    }

    IntStringMapIt_t _location = m_RobToRobinMap.find(_Id);
    if (_location == m_RobToRobinMap.end()) {
      return Config::config().getStr(kUnknownString);
    }
    return _location->second;
  }

  /**
   * Get the name of a ROS from a ROBIN. Assumes that parseRosXml has been called.
   * Note: High call rate function.
   * @param _robinName Name of the ROBIN
   * @return Const reference to name of the ROS the ROBIN is associated with, or "UNKNOWN" on fail.
   */
  const std::string& ROSConfService::getRosNameFromFromRobinName(const std::string& _robinName) {
    if (m_serviceEnabled == kFALSE) {
      return Config::config().getStr(kUnknownString);
    }

    StringStringMapIt_t _location = m_RobinToRosMap.find(_robinName);
    if (_location == m_RobinToRosMap.end()) {
      return Config::config().getStr(kUnknownString);
    }
    return _location->second;
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
    TXMLEngine* _xml = new TXMLEngine();
    XMLDocPointer_t _xmlDoc = _xml->ParseFile(Config::config().getStr(kROSXMLPath).c_str());
    if (_xmlDoc == 0) {
      Error("ROSConfService::parseRosXml", "Unable to parse ROS mapping: %s. No named ROS data.",
            Config::config().getStr(kROSXMLPath).c_str());
      delete _xml;
      return;
    }

    // Get access to main node
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    assert(_xml->GetNodeName(_mainNode) == std::string("ros-rob-data"));

    XMLNodePointer_t _RosNode = _xml->GetChild(_mainNode);
    while (_RosNode != 0) { // Loop over all ROS'
      const std::string _RosName = _xml->GetAttrValue(_xml->GetFirstAttr(_RosNode));
      XMLNodePointer_t _RobinNode = _xml->GetChild(_RosNode);
      while (_RobinNode != 0) { // Loop over all ROS' ROBINs
        const std::string _RobinName = _xml->GetAttrValue(_xml->GetFirstAttr(_RobinNode));
        m_RobinToRosMap[_RobinName] = _RosName;
        XMLNodePointer_t _RobNode = _xml->GetChild(_RobinNode);
        while (_RobNode != 0) { // Loop over all ROBIN's RODs
          const std::string _RobIdStr = _xml->GetAttrValue(_xml->GetFirstAttr(_RobNode));
          std::stringstream _ss;
          _ss << std::hex << _RobIdStr;
          UInt_t _RobId = 0;
          _ss >> _RobId;
          m_RobToRobinMap[_RobId] = _RobinName;
          _RobNode = _xml->GetNext(_RobNode);
        }
        _RobinNode = _xml->GetNext(_RobinNode);
      }
      _RosNode = _xml->GetNext(_RosNode);
    }

    if (Config::config().debug()) {
      Info("ROSConfService::parseRosXml", "Using ROS <-> ROBIN and ROBIN <-> ROB Maps from %s",
           Config::config().getStr(kROSXMLName).c_str());
      for (StringStringMapIt_t _it = m_RobinToRosMap.begin(); _it != m_RobinToRosMap.end(); ++_it) {
        Info("ROSConfService::parseRosXml", "\t%s -> %s", _it->first.c_str(), _it->second.c_str());
      }
      for (IntStringMapIt_t _it = m_RobToRobinMap.begin(); _it != m_RobToRobinMap.end(); ++_it) {
        Info("ROSConfService::parseRosXml", "\t%x -> %s", _it->first, _it->second.c_str());
      }
    } else {
      Info("ROSConfService::parseRosXml", "Parsed ROS configuration %s. Read %i ROS nodes, %i ROBINs.",
           Config::config().getStr(kROSXMLName).c_str(),
           (Int_t) m_RobinToRosMap.size(),
           (Int_t) m_RobToRobinMap.size());
    }

    m_serviceEnabled = kTRUE;
    delete _xml;
  }
} // namespace TrigCostRootAnalysis
