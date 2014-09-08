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
#include <stdlib.h>

// Local include(s):
#include "../TrigCostRootAnalysis/TrigXMLService.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"

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
   * Construct singleton, 
   */
    TrigXMLService::TrigXMLService() : m_menuName(),
      m_prescalSetName(),
      m_serviceEnabled(kFALSE) {
	  
	  //Only one (or none) should be configured
	  assert( Config::config().getIsSet(kMenuXMLPath) + Config::config().getIsSet(kPrescaleXMLPath) <= 1);
	  
	  if ( Config::config().getIsSet(kMenuXMLPath) == kTRUE ) parseMenuXML();
	  else if ( Config::config().getIsSet(kPrescaleXMLPath) == kTRUE ) parsePrescaleXML();
	  else  Warning("TrigXMLService::TrigXMLService", "No prescale XML supplied, if running rates then PS=1 will be used throughout.");
    }

  /**
   * Singleton class retrieval call.
   * @return Reference to the singleton class object.
   */
  TrigXMLService& TrigXMLService::trigXMLService() { // Instance getter
    static TrigXMLService instance; // Guaranteed to be destroyed.
    return instance;        // Instantiated on first use.
  }

  Int_t TrigXMLService::getPrescale(const std::string& _name) {
    if (m_serviceEnabled == kFALSE) return 1;

    StringIntMapIt_t _it = m_chainPS.find( _name );
    if ( _it == m_chainPS.end() ) {
      Warning("TrigXMLService::getPrescale", "Cannot find rates prescale for chain %s in the supplied XML. Will return PS=1.", _name.c_str());
      return 1;
    }
    return _it->second;
  }

  /**
   * Read details from a menu XML file. This allows the user to supply their own prescale set.
   * However this method does not supply L1 prescales so supplying prescale XMLs is recommended.
   */
  void TrigXMLService::parseMenuXML() {
  

    // Now try to parse xml file
    // Only file with restricted xml syntax are supported
    TXMLEngine* _xml = new TXMLEngine();
    XMLDocPointer_t _xmlDoc = _xml->ParseFile( Config::config().getStr(kMenuXMLPath).c_str() );
    if (_xmlDoc == 0) {
      Error("TrigXMLService::parseMenuXML", "Unable to parse trigger menu: %s. PS=1 will be used for rate calculations.", Config::config().getStr(kMenuXMLPath).c_str());
      delete _xml;
      return;
    }
    
    // Get access to main node
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    assert( _xml->GetNodeName(_mainNode) == std::string("HLT_MENU") );
    XMLNodePointer_t _listNode = _xml->GetChild( _mainNode );
    m_menuName =  _xml->GetAttr(_mainNode, "menu_name");
    m_prescalSetName =  _xml->GetAttr(_mainNode, "prescale_set_name");

    while ( _listNode != 0 ) { // Loop over all menu elements
      const std::string _listName = _xml->GetNodeName(_listNode);

      if (_listName != "CHAIN_LIST") {
        _listNode = _xml->GetNext(_listNode);
        continue;
      }

      XMLNodePointer_t _chainNode = _xml->GetChild( _listNode );
      while( _chainNode != 0) {
        assert( _xml->GetNodeName(_chainNode) == std::string("CHAIN"));
        const std::string _chainName = _xml->GetAttr(_chainNode, "chain_name");
        m_chainCounter[_chainName]    = stringToInt( _xml->GetAttr(_chainNode, "chain_counter") );
        m_chainLowerLvl[_chainName]   = _xml->GetAttr(_chainNode, "lower_chain_name");
        m_chainPS[_chainName]         = stringToInt( _xml->GetAttr(_chainNode, "prescale") );     
        m_chainPT[_chainName]         = stringToInt( _xml->GetAttr(_chainNode, "pass_through") );     
        m_chainRerunPS[_chainName]    = stringToInt( _xml->GetAttr(_chainNode, "rerun_prescale") );     

        if (Config::config().debug()) {
          Info("TrigXMLService::parseMenuXML","Parsed Chain:%s, Counter:%i, LowerChain:%s, PS:%i, PT:%i RerunPS:%i",
            _chainName.c_str(),
            m_chainCounter[_chainName],
            m_chainLowerLvl[_chainName].c_str(),
            m_chainPS[_chainName],
            m_chainPT[_chainName],    
            m_chainRerunPS[_chainName]);
        }
        _chainNode = _xml->GetNext(_chainNode);
      }

      _listNode = _xml->GetNext(_listNode);
    }

    Info("TrigXMLService::parseMenuXML","Parsed prescale set %s of menu %s from %s. Read %i chains.", 
      m_prescalSetName.c_str(), 
      m_menuName.c_str(), 
      Config::config().getStr(kMenuXMLName).c_str(),
      (Int_t)m_chainCounter.size());

    m_serviceEnabled = kTRUE;
    delete _xml;
  }

  /**
   * Alternate parser for prescale XML files, including the output of TrigCostPython
   * @see TrigXMLService::parseMenuXML
   */
  void TrigXMLService::parsePrescaleXML() {

    // Now try to parse xml file
    // Only file with restricted xml syntax are supported
    TXMLEngine* _xml = new TXMLEngine();
    XMLDocPointer_t _xmlDoc = _xml->ParseFile( Config::config().getStr(kPrescaleXMLPath).c_str() );
    if (_xmlDoc == 0) {
      Error("TrigXMLService::parsePrescaleXML", "Unable to parse trigger prescale XML: %s. PS=1 will be used for rate calculations.", Config::config().getStr(kPrescaleXMLPath).c_str());
      delete _xml;
      return;
    }
    
    // Get access to main node
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    assert( _xml->GetNodeName(_mainNode) == std::string("trigger") );
    XMLNodePointer_t _listNode = _xml->GetChild( _mainNode );
    //m_menuName =  _xml->GetAttr(_mainNode, "menu_name");
    //m_prescalSetName =  _xml->GetAttr(_mainNode, "prescale_set_name");

    while ( _listNode != 0 ) { // Loop over all menu elements
      const std::string _listName = _xml->GetNodeName(_listNode);

      if (_listName != "level") { // Find the "level" item
        _listNode = _xml->GetNext(_listNode);
        continue;
      }

      XMLNodePointer_t _sigNode = _xml->GetChild( _listNode );
      while( _sigNode != 0) {

        if (_xml->GetNodeName(_sigNode) != std::string("signature")) { // Find the "signature" items
          _sigNode = _xml->GetNext(_sigNode);
          continue;
        }

        XMLNodePointer_t _sigDetailsNode = _xml->GetChild( _sigNode );

        std::string _chainName;
        while( _sigDetailsNode != 0) {

          if (_xml->GetNodeContent(_sigDetailsNode) == NULL) {
            _sigDetailsNode = _xml->GetNext(_sigDetailsNode);
            continue;
          }

          const std::string _detail = _xml->GetNodeName(_sigDetailsNode);
          if (_detail == "sig_name") {
            _chainName = _xml->GetNodeContent(_sigDetailsNode);
          } else if (_detail == "sig_counter") {
            m_chainCounter[_chainName] = stringToInt( _xml->GetNodeContent(_sigDetailsNode) );
          } else if (_detail == "prescale") {
            m_chainPS[_chainName] = stringToInt( _xml->GetNodeContent(_sigDetailsNode) );
          } else if (_detail == "lower_chain_name") {
            // Later processing here does not expect any spaces, so remove them now. Pure comma separated list
            std::string _lower = _xml->GetNodeContent(_sigDetailsNode);
            while (_lower.find(" ") != std::string::npos) {
              _lower.replace( _lower.find(" "), 1, Config::config().getStr(kBlankString));
            }
            m_chainLowerLvl[_chainName] = _lower;
          } else if (_detail == "evts_passed") {
            m_chainEvtPassed[_chainName] = stringToInt( _xml->GetNodeContent(_sigDetailsNode) );
          } else if (_detail == "evts_passed_weighted") {
            m_chainEvtPassedWeighted[_chainName] = stringToFloat( _xml->GetNodeContent(_sigDetailsNode) );
          } else if (_detail == "rate") {
            m_chainRate[_chainName] = stringToFloat( _xml->GetNodeContent(_sigDetailsNode) );
          } else if (_detail == "rate_err") {
            m_chainRateErr[_chainName] = stringToFloat( _xml->GetNodeContent(_sigDetailsNode) );
          } else if (_detail == "passthrough") {
            m_chainPT[_chainName] = stringToInt( _xml->GetNodeContent(_sigDetailsNode) );
          } else if (_detail == "efficiency") {
            m_chainEff[_chainName] = stringToFloat( _xml->GetNodeContent(_sigDetailsNode) );
          } else if (_detail == "efficiency_err") {
            m_chainEffErr[_chainName] = stringToFloat( _xml->GetNodeContent(_sigDetailsNode) );
          } else if (_detail == "prescaled_efficiency") {
            m_chainPSEff[_chainName] = stringToFloat( _xml->GetNodeContent(_sigDetailsNode) );
          } else if (_detail == "prescaled_efficiency_error") {
            m_chainPSEffErr[_chainName] = stringToFloat( _xml->GetNodeContent(_sigDetailsNode) );
          }
        
          _sigDetailsNode = _xml->GetNext(_sigDetailsNode);
        }   

        if (Config::config().debug()) {
          Info("TrigXMLService::parsePrescaleXML", "Parsed Chain:%s, "
            "Counter:%i, "
            "EvtPass:%i, "
            "EvtPassWeight:%.2f, "
            "Rate:%.2f, "
            "RateErr:%.2f, "
            "PS:%i, "
            "PT:%i, "
            "Lower:%s, "
            "Eff:%.2f, "
            "EffErr%.2f, "
            "PSEff:%.2f, "
            "PSEffErr:%.2f",
            _chainName.c_str(),
            m_chainCounter[_chainName],
            m_chainEvtPassed[_chainName],
            m_chainEvtPassedWeighted[_chainName],
            m_chainRate[_chainName],    
            m_chainRateErr[_chainName],
            m_chainPS[_chainName],
            m_chainPT[_chainName],
            m_chainLowerLvl[_chainName].c_str(),
            m_chainEff[_chainName],
            m_chainEffErr[_chainName],
            m_chainPSEff[_chainName],
            m_chainPSEffErr[_chainName]);
        }
        _sigNode = _xml->GetNext(_sigNode);
      }

      _listNode = _xml->GetNext(_listNode);
    }

    Info("TrigXMLService::parsePrescaleXML","Parsed prescale set %s. Read %i chains.", 
      Config::config().getStr(kPrescaleXMLName).c_str(),
      (Int_t)m_chainCounter.size());

    m_serviceEnabled = kTRUE;
    delete _xml;
  }

  void TrigXMLService::writePrescaleXML() {

  }

} // namespace TrigCostRootAnalysis

