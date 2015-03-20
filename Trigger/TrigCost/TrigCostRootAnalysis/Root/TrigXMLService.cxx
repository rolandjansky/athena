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
#include <algorithm> //find

// Local include(s):
#include "../TrigCostRootAnalysis/TrigXMLService.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/WriteXML_JSON.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"

// ROOT includes
#include <TSystem.h>
#include <TXMLEngine.h>
#include <TObjString.h>
#include <TDOMParser.h>
#include <TXMLNode.h>
#include <TXMLDocument.h>
#include <TXMLAttr.h>
#include <TError.h>

// ATHENA Includes
#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif // not ROOTCORE

namespace TrigCostRootAnalysis {

  /**
   * Construct singleton, 
   * This class handles all loading of custom prescale sets to be applied at rate/cost post-processing time
   * It also handles the distribution of these data around the application
   */
  TrigXMLService::TrigXMLService() : 
      m_menuName(),
      m_prescalSetName(),
      m_serviceEnabled(kFALSE),
      m_weightsServiceEnabled(kFALSE) {
	  
    if ( Config::config().getIsSet(kPrescaleXMLPath1) == kTRUE) parseXML(1);
    if ( Config::config().getIsSet(kPrescaleXMLPath2) == kTRUE) parseXML(2);
	  if (m_serviceEnabled == kFALSE) {
      Warning("TrigXMLService::TrigXMLService", "User did not supply any prescale XML files. L1 PS=%f and HLT PS=%f will be used.",
        Config::config().getFloat(kRateFallbackPrescaleL1), Config::config().getFloat(kRateFallbackPrescaleHLT));
    }
  }

  /**
   * Singleton class retrieval call.
   * @return Reference to the singleton class object.
   */
  TrigXMLService& TrigXMLService::trigXMLService() { // Instance getter
    static TrigXMLService instance; // Guaranteed to be destroyed.
    return instance;        // Instantiated on first use.
  }

  Double_t TrigXMLService::getPrescale(const std::string& _name) {
    if (m_serviceEnabled == kFALSE) {
      if (_name.substr(0,3) == "L1_") {
        return (Double_t)Config::config().getFloat(kRateFallbackPrescaleL1);
      } else {
        return (Double_t)Config::config().getFloat(kRateFallbackPrescaleHLT);
      }
    }

    StringDoubleMapIt_t _it = m_chainPS.find( _name );
    if ( _it == m_chainPS.end() ) {
      if (_name.substr(0,3) == "L1_") {
        if (Config::config().getDisplayMsg(kMsgXMLPrescale) == kTRUE) {
          Warning("TrigXMLService::getPrescale", "Cannot find rates prescale for chain %s in the supplied XML. Will return fallback L1 PS:%f.",
            _name.c_str(),
            Config::config().getFloat(kRateFallbackPrescaleL1));
        }
        return (Double_t)Config::config().getFloat(kRateFallbackPrescaleL1);
      } else {
        if (Config::config().getDisplayMsg(kMsgXMLPrescale) == kTRUE) {
          Warning("TrigXMLService::getPrescale", "Cannot find rates prescale for chain %s in the supplied XML. Will return fallback HLT PS:%f.",
            _name.c_str(),
            Config::config().getFloat(kRateFallbackPrescaleHLT));
        }
        return (Double_t)Config::config().getFloat(kRateFallbackPrescaleHLT);
      }
    }

    return _it->second;
  }

  /**
   * When running cost analysis in the HLT and looking at the effect of prescales the following should be taken into account:
   * - All data should be scaled down by 1/PSL1 * 1/PSHLT
   * - If the HLT trigger or L1 trigger is disabled, the weight should be 0.
   * @param _chainName Name of the chain to get the weighting factor for. Only makes sense to this fn for HLT chains
   * @return the calculated multiplicitive weighting factor
   */
  Double_t TrigXMLService::getHLTCostWeightingFactor(const std::string& _chainName ) {
    Double_t _prescaleHLT = getPrescale( _chainName );
    if (_prescaleHLT < 0.) return 0.;
    const std::string _myLowerChain = TrigConfInterface::getLowerChainName( _chainName );
    // Check we're not multi-seeded
    if ( _myLowerChain.find(",") != std::string::npos ) return 1./_prescaleHLT; //What else could we do here?
    Double_t _lowerPS = getPrescale( _myLowerChain );
    if (_lowerPS < 0.) return 0.;
    return (1./_prescaleHLT)*(1./_lowerPS);  
  } 

  /**
   * Read XML structure, decide on format using (usually) the head node and pass to parser function
   * @param _xmlID This program can take in two XMLs, if the L1 and HLT PS are split over them. This int says which we want to process.
   */
  void TrigXMLService::parseXML(UInt_t _xmlID) {

    assert( _xmlID == 1 || _xmlID == 2);
    std::string _psFilePath;
    std::string _psFileName;
    if (_xmlID == 1) {
      _psFilePath = Config::config().getStr(kPrescaleXMLPath1);
      _psFileName = Config::config().getStr(kPrescaleXMLName1);
    } else if (_xmlID == 2) {
      _psFilePath = Config::config().getStr(kPrescaleXMLPath2);
      _psFileName = Config::config().getStr(kPrescaleXMLName2);
    }

    // Now try to parse xml file
    // Only file with restricted xml syntax are supported
    TXMLEngine* _xml = new TXMLEngine();
    XMLDocPointer_t _xmlDoc = _xml->ParseFile( _psFilePath.c_str() );
    if (_xmlDoc == 0) {
      Error("TrigXMLService::parseXML", "Unable to parse prescale XML %s!!! Fallback PS=%f L1, PS=%fi HLT will be used for rate calculations.",
        _psFilePath.c_str(),
        Config::config().getFloat(kRateFallbackPrescaleL1),
        Config::config().getFloat(kRateFallbackPrescaleHLT));
      delete _xml;
      return;
    }
    
    // Get access to main node
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    if ( _xml->GetNodeName(_mainNode) == std::string("trigger") ) {
      parsePrescaleXML(_xml, _xmlDoc);
    } else if ( _xml->GetNodeName(_mainNode) == std::string("HLT_MENU") ) {
      parseMenuXML(_xml, _xmlDoc);
    } else if ( _xml->GetNodeName(_mainNode) == std::string("LVL1Config") ) {
      parseL1MenuXML(_xml, _xmlDoc);
    } else {
      Error("TrigXMLService::parseXML", "Supplied a prescale XML with root node '%s'. I do not know how to decode this!!! Fallback PS=%f L1, PS=%fi HLT will be used for rate calculations.",
        _xml->GetNodeName(_mainNode),
        Config::config().getFloat(kRateFallbackPrescaleL1),
        Config::config().getFloat(kRateFallbackPrescaleHLT));
    }

    // Check for PS=0
    for (StringDoubleMapNonConstIt_t _it = m_chainPS.begin(); _it != m_chainPS.end(); ++_it) {
      if ( isZero(_it->second) == kTRUE ) _it->second = -1.0;
    }
  
    delete _xml;
  }

  /**
   * Read details from a menu XML file. This allows the user to supply their own prescale set.
   * However this method does not supply L1 prescales.
   */
  void TrigXMLService::parseMenuXML(TXMLEngine* _xml, XMLDocPointer_t _xmlDoc) {
      
    // Get access to main node
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    assert( _xml->GetNodeName(_mainNode) == std::string("HLT_MENU") );
    XMLNodePointer_t _listNode = _xml->GetChild( _mainNode );
    m_menuName =  _xml->GetAttr(_mainNode, "menu_name");
    m_prescalSetName =  _xml->GetAttr(_mainNode, "prescale_set_name");
    Int_t _chainsRead = 0;

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
        if (_xml->GetAttr(_chainNode, "chain_counter"))    m_chainCounter[_chainName]  = stringToInt( _xml->GetAttr(_chainNode, "chain_counter") );
        if (_xml->GetAttr(_chainNode, "lower_chain_name")) m_chainLowerLvl[_chainName] = _xml->GetAttr(_chainNode, "lower_chain_name");
        if (_xml->GetAttr(_chainNode, "prescale"))         m_chainPS[_chainName]       = stringToDouble( _xml->GetAttr(_chainNode, "prescale") );     
        if (_xml->GetAttr(_chainNode, "pass_through"))     m_chainPT[_chainName]       = stringToInt( _xml->GetAttr(_chainNode, "pass_through") );     
        if (_xml->GetAttr(_chainNode, "rerun_prescale"))   m_chainRerunPS[_chainName]  = stringToDouble( _xml->GetAttr(_chainNode, "rerun_prescale") );    
        ++_chainsRead; 

        if (Config::config().debug()) {
          Info("TrigXMLService::parseMenuXML","Parsed Chain:%s, Counter:%i, LowerChain:%s, PS:%f, PT:%i RerunPS:%f",
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

    Info("TrigXMLService::parseMenuXML","Parsed prescale set %s of menu %s. Read %i chains.", 
      m_prescalSetName.c_str(), 
      m_menuName.c_str(), 
      _chainsRead);

    Config::config().set(kPrescaleSetName, m_prescalSetName, "PrescaleSetName", kUnlocked);
    Config::config().set(kMenuName, m_menuName, "MenuName", kUnlocked);

    m_serviceEnabled = kTRUE;
  }

  /**
   * Read details from a L1 menu XML file. This allows the user to supply their own prescale set.
   * We first need to read the name-CTPID mappings, then can read in the PS
   */
  void TrigXMLService::parseL1MenuXML(TXMLEngine* _xml, XMLDocPointer_t _xmlDoc) {
      
    // Get access to main node
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    assert( _xml->GetNodeName(_mainNode) == std::string("LVL1Config") );
    XMLNodePointer_t _listNode = _xml->GetChild( _mainNode );
    Int_t _chainsRead = 0;

    while ( _listNode != 0 ) { // Loop over all menu elements
      const std::string _listName = _xml->GetNodeName(_listNode);

      // TriggerMenu contains TriggerItems which hold the CTPID and names
      if (_listName == "TriggerMenu") { 

        XMLNodePointer_t _chainNode = _xml->GetChild( _listNode );
        while( _chainNode != 0) {
          assert( _xml->GetNodeName(_chainNode) == std::string("TriggerItem"));
          Int_t _ctpid = stringToInt( _xml->GetAttr(_chainNode, "ctpid"));
          m_CTPIDToL1Name[_ctpid] = _xml->GetAttr(_chainNode, "name");
          if (Config::config().debug()) {
            Info("TrigXMLService::parseL1MenuXML","Step 1: CTPID %i = %s", _ctpid, m_CTPIDToL1Name[_ctpid].c_str());
          }
          _chainNode = _xml->GetNext(_chainNode);
        }

      // PrescaleSet contains Prescale items which map these CTPIDs to their PS
      // The CTPID, and "n", "m" and "d" parameters are attributes, the prescale is content
      } else if (_listName == "PrescaleSet") {

        XMLNodePointer_t _chainNode = _xml->GetChild( _listNode );
        while( _chainNode != 0) {
          assert( _xml->GetNodeName(_chainNode) == std::string("Prescale"));
          Int_t _ctpid = stringToInt( _xml->GetAttr(_chainNode, "ctpid"));
          const std::string _L1Name = m_CTPIDToL1Name[ _ctpid ];
          if (_L1Name == Config::config().getStr(kBlankString)) {
            Error("TrigXMLService::parseL1MenuXML","XML trying to set PS for CTPID %i, but no chain name was supplied by the file", _ctpid);
            _chainNode = _xml->GetNext(_chainNode);
            continue;
          }
          if (_xml->GetNodeContent(_chainNode) == NULL) {
            Error("TrigXMLService::parseL1MenuXML","Unable to extract prescale for %s", _L1Name.c_str());
            _chainNode = _xml->GetNext(_chainNode);
            continue;
          }
          Double_t _prescale = stringToDouble( _xml->GetNodeContent(_chainNode) );
          m_chainCounter[_L1Name] = _ctpid;
          m_chainPS[_L1Name] = _prescale;
          ++_chainsRead;
          if (Config::config().debug()) {
            Info("TrigXMLService::parseL1MenuXML","Step 2: L1 %s = PS %f", _L1Name.c_str(), _prescale);
          }
          _chainNode = _xml->GetNext(_chainNode);
        }

      } else {
        Error("TrigXMLService::parseL1MenuXML","Found an unknown XML element %s", _listName.c_str());
      }

      _listNode = _xml->GetNext(_listNode);
    }

    Info("TrigXMLService::parseL1MenuXML","Parsed L1 prescale set. Read %i chains.", _chainsRead);
    m_serviceEnabled = kTRUE;
  }

  /**
   * Alternate parser for prescale XML files, including the output of TrigCostPython
   * @param _xml Pointer to XML document
   * @see TrigXMLService::parseMenuXML
   */
  void TrigXMLService::parsePrescaleXML(TXMLEngine* _xml, XMLDocPointer_t _xmlDoc) {

    // Get access to main node
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    assert( _xml->GetNodeName(_mainNode) == std::string("trigger") );
    XMLNodePointer_t _listNode = _xml->GetChild( _mainNode );

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
          } else if (_detail == "prescale" || _detail == "chain_prescale") { // This is an alternate name
            m_chainPS[_chainName] = stringToDouble( _xml->GetNodeContent(_sigDetailsNode) );
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

        // Sanity check here. If the user is not doing EB weighting, and is not scaling up by the L1 PS, then != 1 PS at L1 make no sense.
        if (Config::config().getInt(kDoEBWeighting) == 0 
          && Config::config().getInt(kRatesScaleByPS) == 0
          && _chainName.substr(0,3) == "L1_" 
          && isZero(m_chainPS[_chainName] - 1.) == kFALSE) {
          Error("TrigXMLService::parsePrescaleXML","You are setting a prescale PS=%f for an L1 item %s, but you are NOT using EnhancedBias data. "
            "Nor are you using the --scaleRatesByPS option to weight up chains by their L1 prescale. "
            "In order to simulate changes to the L1 prescale, you should be using EB weighting. Otherwise the correlations between L1 items in the CTP "
            "will have been lost.", m_chainPS[_chainName], _chainName.c_str());
        }  

        if (Config::config().debug()) {
          Info("TrigXMLService::parsePrescaleXML", "Parsed Chain:%s, "
            "Counter:%i, "
            "EvtPass:%i, "
            "EvtPassWeight:%.2f, "
            "Rate:%.2f, "
            "RateErr:%.2f, "
            "PS:%f, "
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

    Info("TrigXMLService::parsePrescaleXML","Parsed prescale set from XML. Now know PS values for %i chains.", (Int_t)m_chainPS.size());
    m_serviceEnabled = kTRUE;
  }

  /**
   * Function to load into memory the enhanced bias weighting factors for a run.
   * These are calculated in advance for each EB run and stored in XML format with one entry for each event.
   * They are loaded into memory in a map which is used to fetch the weight.
   */
  void TrigXMLService::parseEnhancedBiasXML() {
    TXMLEngine* _xml = new TXMLEngine();

    Info("TrigXMLService::parseEnhancedBiasXML","Loading Enhanced Bias Weighting XML. This could take a little time.");

    // Try one. Use the external (AFS) data path
    Config::config().set(kEBXMLName, "/EnhancedBiasWeights_" + intToString(Config::config().getInt(kRunNumber)) + ".xml", "EBXMLName");
    Config::config().set(kEBXMLPath, Config::config().getStr(kAFSDataDir) + "/" + Config::config().getStr(kEBXMLName), "EBXMLPath", kUnlocked);

    XMLDocPointer_t _xmlDoc = _xml->ParseFile( Config::config().getStr(kEBXMLPath).c_str() );
    // Could we load from AFS?

    if (_xmlDoc == 0) {
      // No - then try loading locally
      if (Config::config().getInt(kIsRootCore) == kTRUE) {
        Config::config().set(kEBXMLPath, Config::config().getStr(kDataDir) + "/" + Config::config().getStr(kEBXMLName), "EBXMLPath");
      } else {
// CAUTION - "ATHENA ONLY" CODE
#ifndef ROOTCORE
        std::string _locAthena = PathResolverFindDataFile( Config::config().getStr(kEBXMLName) );
        if (_locAthena == Config::config().getStr(kBlankString)) {
          Error("TrigXMLService::parseEnhancedBiasXML","Athena cannot find Enhanced Bias weighting file %s. Critical error.", Config::config().getStr(kEBXMLName).c_str());
          abort();
        } else {
          Config::config().set(kEBXMLName, _locAthena);
          Info("TrigXMLService::parseEnhancedBiasXML","Athena has found the file: %s", Config::config().getStr(kEBXMLName).c_str());
        }
#endif // not ROOTCORE
      }

      // Load the file - try 2
      _xmlDoc = _xml->ParseFile( Config::config().getStr(kEBXMLPath).c_str() );
      if (_xmlDoc == 0) {
        Error("TrigXMLService::parseEnhancedBiasXML","Unable to load EnhancedBias weighting factors from %s. Critical error.", Config::config().getStr(kEBXMLName).c_str());
        abort();
      }
    }

    if (Config::config().debug()) Info("TrigXMLService::parseEnhancedBiasXML","Loading into memory the Enhanced Bias Weighting factors for run %i", Config::config().getInt(kRunNumber));


    // Navigate XML
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    assert( _xml->GetNodeName(_mainNode) == std::string("run") );
    XMLNodePointer_t _weightsNode = _xml->GetChild( _mainNode );
    XMLNodePointer_t _eventsNode = _xml->GetNext( _weightsNode );

    XMLNodePointer_t _weightNode = _xml->GetChild( _weightsNode );
    XMLNodePointer_t _eventNode = _xml->GetChild( _eventsNode );

    while ( _weightNode != 0 ) { // Loop over all menu elements
      assert( _xml->GetNodeName(_weightNode) == std::string("weight")); //Event

      UInt_t _id = stringToInt(_xml->GetAttr(_weightNode, "id")); //Number
      Float_t _weight = stringToFloat(_xml->GetAttr(_weightNode, "value")); //Weight
      std::string _bunchgroup = _xml->GetAttr(_weightNode, "bunchgroup"); //BG

      UInt_t _bunchgroupID = (UInt_t) kBG_UNSET;
      for (UInt_t _i = 0; _i < kEBBunchGroupType_SIZE; ++_i) {
        if ( _bunchgroup == BunchGroupNameStr[_i] ) {
          _bunchgroupID = _i;
          break;
        }
      }

      if (_bunchgroupID == (UInt_t) kBG_UNSET) {
        Error("TrigXMLService::parseEnhancedBiasXML", "I don't know about this bunchgroup: %s", _bunchgroup.c_str());
      }

      m_idToWeightMap[_id] = _weight;
      m_idToBGMap[_id] = _bunchgroupID;

      if (Config::config().debug()) {
        Info("TrigXMLService::parseEnhancedBiasXML", "Enhanced bias weight %.4f has ID %i and bunchgroup %s (%i)",
          _weight, _id, _bunchgroup.c_str(), _bunchgroupID);
      }

      _weightNode = _xml->GetNext(_weightNode);
    }

    while ( _eventNode != 0 ) { // Loop over all menu elements
      assert( _xml->GetNodeName(_eventNode) == std::string("e")); //Event
      UInt_t _eventNumber = stringToInt(_xml->GetAttr(_eventNode, "n")); //Number
      UInt_t _eventWeightID = stringToInt(_xml->GetAttr(_eventNode, "w")); //Weight ID

      m_ebWeightingMap[_eventNumber] = _eventWeightID;
      _eventNode = _xml->GetNext(_eventNode);
    }

    Info("TrigXMLService::parseEnhancedBiasXML","Loaded %i event weights for run %i.", (Int_t) m_ebWeightingMap.size(), Config::config().getInt(kRunNumber));
    m_weightsServiceEnabled = kTRUE;
    return;
  }

  /**
   * Read (if not already imported) the XML rates and bunch groups for this run.
   * Return the weight for this event and store the event's  bunch group setting using the config service.
   * @return The event weight from the EnhancedBias XML.
   */
  Float_t TrigXMLService::getEventWeight(UInt_t _eventNumber) {
    if (Config::config().getInt(kDoEBWeighting) == kFALSE) return 1.;
    if (m_weightsServiceEnabled == kFALSE) parseEnhancedBiasXML();

    IntIntMapIt_t _ebIt = m_ebWeightingMap.find( _eventNumber );
    Int_t _weightID = 0;
    if ( _ebIt == m_ebWeightingMap.end() ) {
      if (Config::config().getDisplayMsg(kMsgXMLWeight) == kTRUE) { 
        Warning("TrigXMLService::getEventWeight","Cannot find the weight for event %i, will use weight = 0. !!", _eventNumber);
      }
      Config::config().set(kCurrentEventBunchGroupID, (Int_t) kBG_UNSET, "BunchGroup", kUnlocked);
      return 0.;
    } else {
      _weightID = _ebIt->second;
    }

    Float_t _eventWeight = m_idToWeightMap[_weightID];
    UInt_t  _eventBG     = m_idToBGMap[_weightID];

    // Store the event BG
    Config::config().set(kCurrentEventBunchGroupID, _eventBG, "BunchGroup", kUnlocked);
    return _eventWeight;
  }

  /**
   * Export enhanced bias weighting values to XML for use by everyone in this application.
   * This function is called on every event when using the expert option --writeEBXML.
   * Ehanced bias weights, calculated in Athena, are read from the D3PD. Each floating point
   * weight is assigned a numeric ID and event numbers are matched to these IDs.
   * The XML should be coppied to a central accessible location, this is currently 
   * /afs/cern.ch/user/a/attradm/public
   * @param _eventNumber The current event number
   * @param _weight The enhanced bias (un)weighting value for this event, based on the online prescale set and L1 pass before prescale bits.
   */
  void TrigXMLService::exportEnhancedBiasXML(UInt_t _eventNumber, Float_t _weight, UInt_t _bunchGroup) {

    static std::pair<std::string, Float_t> _recyclablePair;

    _recyclablePair.first = BunchGroupNameStr[_bunchGroup];
    _recyclablePair.second = _weight;

    m_ebWeightTypesCount[_recyclablePair] += 1;

    Int_t _location = -1;
    std::vector< std::pair<std::string, Float_t> >::iterator _weightTypeIt = std::find(m_ebWeightTypes.begin(), m_ebWeightTypes.end(), _recyclablePair);
    if (_weightTypeIt == m_ebWeightTypes.end() ) {
      //Add me
      m_ebWeightTypes.push_back( _recyclablePair );
      _location = m_ebWeightTypes.size() - 1;
    } else {
      _location = _weightTypeIt - m_ebWeightTypes.begin();
    }
    m_ebWeightingMapSave[_eventNumber] = _location;
  }

  /**
   * Produce the actual XML output
   * @see TrigXMLService::exportEnhancedBiasXML(UInt_t _eventNumber, Float_t _weight)
   */
  void TrigXMLService::saveExportedEnhancedBiasXML() {
    std::ofstream _ebOut;
    const std::string _outputEBWeight = Config::config().getStr(kOutputDirectory) 
      + "/EnhancedBiasWeights_" 
      + intToString(Config::config().getInt(kRunNumber)) 
      + ".xml";            
    _ebOut.open( _outputEBWeight.c_str() );
    XMLExport _ebXML(_ebOut);
    _ebXML.setSpaces(0);
    _ebXML.addNode(_ebOut, "run");
    _ebXML.addNode(_ebOut, "weights");
    for (UInt_t _i = 0; _i < m_ebWeightTypes.size(); ++_i) {
      _ebXML.addNodeWProperty(_ebOut,"weight","id",intToString(_i),"value",floatToString(m_ebWeightTypes.at(_i).second,7),"bunchgroup",m_ebWeightTypes.at(_i).first);
      Info("TrigXMLService::saveExportedEnhancedBiasXML","Weight %f of BunchGroup %s has ID %i", m_ebWeightTypes.at(_i).second, m_ebWeightTypes.at(_i).first.c_str(), _i );
    }
    _ebXML.endNode(_ebOut);//weights
    _ebXML.addNode(_ebOut, "events");
    for (IntIntMapIt_t _it = m_ebWeightingMapSave.begin(); _it != m_ebWeightingMapSave.end(); ++_it) {
      _ebXML.addNodeWProperty(_ebOut,"e","n",intToString(_it->first),"w",intToString(_it->second));
    }
    _ebXML.endNode(_ebOut);//events
    _ebXML.endNode(_ebOut);//run
    _ebOut.close();
    for (PairStringFloat_Float_MapIt_t _it = m_ebWeightTypesCount.begin(); _it != m_ebWeightTypesCount.end(); ++_it) {
      Info("TrigXMLService::saveExportedEnhancedBiasXML","%i events with enhanced bias weight %f and BG %s", _it->second, _it->first.second, _it->first.first.c_str() );
    }
  }

  /**
   * Write a simple dummy file Prescales.xml which contains the structure of the trigger menu, and the default prescales used when
   * the trigger was run to produce the ntuple.
   * This file can then be modified by hand etc. by the end user and used to apply a custom prescale set to the trigger in
   * subsiquent rate prediction campaigns.
   * 
   * This is an alternative to getting the initial prescale set using the Rule Book.
   */ 
  void TrigXMLService::writePrescaleXML() {

    const std::string _xmlName = Config::config().getStr(kOutputRootDirectory) + "/" + "Prescales.xml";
    std::ofstream _fout;
    _fout.open( _xmlName.c_str() );
    _fout << std::fixed; // Use fixed width output

    Info("TrigXMLService::writePrescaleXML","Writing a dummy file Prescales.xml with current menu.");

    XMLExport _xml(_fout);
    _xml.setSpaces(2);
    _xml.addNode(_fout, "trigger");

    // Loop over all chains in metadata trigger config folder
    Int_t _level = -1;
    for (UInt_t _c = 0; _c < TrigConfInterface::getChainN(); ++_c) {

      if ( TrigConfInterface::getChainLevel(_c) == 1 && _level == -1) {
        _level = 1;
        _xml.addNode(_fout, "level");
        _xml.addNode(_fout, "lvl_name", "L1");
      } else if ( TrigConfInterface::getChainLevel(_c) == 2 && _level == 1 ) {
        //switch over to HLT
        _level = 2;
        _xml.endNode(_fout); //level
        _xml.addNode(_fout, "level");
        _xml.addNode(_fout, "lvl_name", "HLT");
      }

      _xml.addNode(_fout, "signature");
      std::string _name = TrigConfInterface::getChainName(_c);
      _xml.addNode(_fout, "sig_name", _name);
      _xml.addNode(_fout, "chain_prescale", floatToString( TrigConfInterface::getPrescale(_name) ));
      _xml.endNode(_fout); //signature

    }
    _xml.endNode(_fout); //level
    _xml.endNode(_fout); //Trigger
    _fout.close();

    Info("TrigXMLService::writePrescaleXML","Exported XML with %i chains.", TrigConfInterface::getChainN());

  }

} // namespace TrigCostRootAnalysis

