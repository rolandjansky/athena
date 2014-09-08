// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>
#include <fstream>
#include <assert.h>
#include <algorithm> //std::replace

// ROOT include(s):
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TError.h>

// Local include(s):
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"

// RootCore TrigRootAnalysis include(s):
#include "TrigRootAnalysis/TrigDecisionToolD3PD.h"
#include "TrigRootAnalysis/TrigConfigSvcD3PD.h"

namespace TrigCostRootAnalysis {

  // Initialise statics
  D3PD::TrigDecisionToolD3PD* TrigConfInterface::m_tdt;
  D3PD::TrigConfigSvcD3PD* TrigConfInterface::m_tct;
  
  Bool_t TrigConfInterface::m_isConfigured = kFALSE;
  
  Bool_t TrigConfInterface::m_usingNtupleMetadata = kTRUE;
  Bool_t TrigConfInterface::m_usingNtuplePrescales = kTRUE;
    
  DBKey TrigConfInterface::m_key(0, 0, 0);
  
  /**
   * Link tool to trigger meta data. Call once per chain.
   * @param _inputChain TChain pointer to input files
   * @return Returns kTRUE upon successful configuration.
   */
  Bool_t TrigConfInterface::configure( TChain* _inputChain ) {
  
    TFile* _file = 0;
    TChain* _chain = dynamic_cast< TChain* >( _inputChain );
    if( _chain ) {
      // We are running locally...
      Info("TrigConfInterface::configure", "Opening a new file locally...");
      _file = _chain->GetFile();
    } else {
      // We are running on PROOF:
      Info("TrigConfInterface::configure", "Opening a new file on PROOF...");
      _file = _inputChain->GetCurrentFile();
    }
    
    TTree* _confTree = dynamic_cast< TTree* >( _file->Get( Config::config().getStr(kConfigPrefix).c_str() ) );
    if( ! _confTree ) {
      Error("TrigConfInterface::configure", "Couldn't retrieve configuration metadata tree!");
      m_isConfigured = kFALSE;
      return kFALSE;
    }
    
    if (m_tdt) delete m_tdt;
    m_tdt = new D3PD::TrigDecisionToolD3PD();
    
    if( ! m_tdt->SetEventTree( _inputChain ) ) {
      Error("TrigConfInterface::configure", "Problems with setting the event tree to the TrigDecisionTool.");
      m_isConfigured = kFALSE;
      return kFALSE;
    }
    if( ! m_tdt->SetConfigTree( _confTree ) ) {
      Error("TrigConfInterface::configure", "Problems setting the trigger config tree to the TrigDecisionTool.");
      m_isConfigured = kFALSE;
      return kFALSE;
    }
    
    m_isConfigured = kTRUE;
    return m_isConfigured;
  }
  
  /**
   * @return When configured, returns the super-master key for the currently processing event.
   */
  Int_t TrigConfInterface::getCurrentSMK() {
    // XXX this is temporary
    Int_t _smk = getTDT()->GetSMK();
    //if (_smk == 65535) return 0; 
    return _smk;
  }
  
  /**
   * @return When configured, returns the level 1 prescale key for the currently processing event.
   */
  Int_t TrigConfInterface::getCurrentL1PSK() {
    return getTDT()->GetL1PSK();
  }
  
  /**
   * @return When configured, returns the HLT prescale key for the currently processing event.
   */
  Int_t TrigConfInterface::getCurrentHLTPSK() {
    return getTDT()->GetHLTPSK();
  }
  
  /**
   * @return When configured, returns the current HLT keyset in small wrapper object.
   */
  const DBKey& TrigConfInterface::getCurrentDBKey() {
    m_key.set( getCurrentSMK(), getCurrentL1PSK(), getCurrentHLTPSK() );
    return m_key;
  }
  
  /**
   * Fetch the ID of L1 item from name.
   * @param _name Const reference to name of L1 chain.
   * @return Integer CTPID for L1 chain.
   */
  Int_t TrigConfInterface::getCtpId( const std::string& _name ) {
    if (_name == "NO_SEED") return -1;
    return getTCT()->GetCTPId( _name );
  }
  
  /**
   * Fetch the name of a L1 item from its ID
   * @param _name CPT ID of L1 chain.
   * @return Name of the L1 chain.
   */
  const std::string& TrigConfInterface::getNameFromCtpId( Int_t _ctpId ) {
    return getTCT()->GetNameFromCTPId( _ctpId );
  }
  
  /**
   * Fetch the low chain name of a given chain, EF->L2, L2->L1, HLT->L1.
   * @param _name Const reference to higher chain name.
   * @return Const reference to lower chain name.
   */
  const std::string& TrigConfInterface::getLowerChainName( const std::string& _name ) {
    return getTCT()->GetLowerChainName( _name );
  }
  
  /**
   * Explicitly load given entry in tree. This should not be needed.
   * @param _entry Entry in tree to load.
   */
  void TrigConfInterface::getEntry( Long64_t _entry ) {
    assert( m_isConfigured );
    // This is not, it appears, needed.
    Warning("TrigConfInterface::getEntry", "This does not need to be called"); //TODO Remove this func
    return;
    m_tdt->GetEntry( _entry );
  }
  
  /**
   * Fetch HLT string name using currently configured access method. Supplying level helps but is not needed.
   * @param _chainID HLT ID number of chain
   * @param _level Chain level (2 for L2 or HLT, 3 for EF)
   * @return Const reference to chain name. Empty string if invalid ID or if using unsupported access mode.
   */
  const std::string TrigConfInterface::getHLTNameFromChainID( Int_t _chainID, Int_t _level ) {
    if ( getUsingNtupleMetadata() == kTRUE ) {
      // Currently split into L2 and EF
      if (_level == 2 || _level == 0) { // If L2 (HLT responds as L2) or unspecified
        if ( getTCT()->GetL2NameFromChainId( _chainID ) != "" && getTCT()->GetL2NameFromChainId( _chainID ) != "0") {
          //std::cout << _chainID << ", THE NAME IS " << getTCT()->GetL2NameFromChainId( _chainID ) << std::endl;
          return getTCT()->GetL2NameFromChainId( _chainID );
        }
      }
      // Else try EF, unless we're running on HLT as there is no EF any more, then we can just return cannot find.
      if (Config::config().getInt(kDoHLT) == kTRUE) return Config::config().getStr(kBlankString);
      return getTCT()->GetEFNameFromChainId( _chainID );
    } else {
      Error("TrigConfInterface::getHLTNameFromChainID", "XML based menu navigation not yet included.");
    }
    return Config::config().getStr(kBlankString);
  }

  /**
   * Fetch the number of groups a chain is registered under.
   * TODO check that we don't need to add extra code for L2 and EF here?
   * @param _chainID HLT ID number of chain
   * @return Number of groups this chain belongs to
   */
  UInt_t TrigConfInterface::getNHLTGroupNamesFromChainID( Int_t _chainID ) {
    if ( getUsingNtupleMetadata() == kTRUE ) {
      UInt_t _index = getTCT()->GetChainIndexFromCounter(_chainID);
      if (_index == UINT_MAX) {
        Warning("TrigConfInterface::getNHLTGroupNamesFromChainID","Chain with Counter %i is not in the configuration.", _chainID);
        return 0;
      }
      return getTCT()->GetChainGroupNameSize(_index);
    } else {
      Error("TrigConfInterface::getNHLTGroupNamesFromChainID", "XML based menu navigation not yet included.");
    }
    return 0;
  }

  /**
   * Fetch the name of one of the groups this chain is registered under.
   * TODO check that we don't need to add extra code for L2 and EF here?
   * @param _chainID HLT ID number of chain
   * @param _group the index of the group to fetch (0 to getNHLTGroupNamesFromChainID() -1)
   * @return New'd string of the group name.
   */
  const std::string TrigConfInterface::getHLTGroupNameFromChainID( Int_t _chainID, UInt_t _group ) {
    if ( getUsingNtupleMetadata() == kTRUE ) {
      UInt_t _index = getTCT()->GetChainIndexFromCounter(_chainID);
      if (_index == UINT_MAX) {
        Warning("TrigConfInterface::getHLTGroupNameFromChainID","Chain with Counter %i is not in the configuration.", _chainID);
        return Config::config().getStr(kBlankString);
      }
      if ( _group >= getNHLTGroupNamesFromChainID(_chainID)) {
        Warning("TrigConfInterface::getHLTGroupNameFromChainID","Requested group %i out of range, size is %i.", _group, getNHLTGroupNamesFromChainID(_chainID));
        return Config::config().getStr(kBlankString);
      }
      return getTCT()->GetChainGroupName(_index, _group);
    } else {
      Error("TrigConfInterface::getHLTGroupNameFromChainID", "XML based menu navigation not yet included.");
    }
    return Config::config().getStr(kBlankString);
  }
  
  /**
   * Fetch HLT sequence name from sequence index number. Uses advanced trigger configuration information.
   * @param _index Index number of sequence.
   * @return Const reference to sequence name. Empty string if invalid ID or if using unsupported access mode.
   */
  const std::string& TrigConfInterface::getHLTSeqNameFromIndex( UInt_t _index ) {
    if ( getUsingNtupleMetadata() == kTRUE ) {
      return getTCT()->GetSequenceNameFromIndex( _index );
    } else {
      Error("TrigConfInterface::getHLTSeqNameFromIndex", "XML based menu navigation not yet included.");
    }
    return Config::config().getStr(kBlankString);
  }
  
  /**
   * Fetch HLT algorithm name using the parents sequence index and position of algorithm within sequence.
   * @param _index Index number of parent sequence.
   * @param _position Position of algorithm within parent sequence.
   * @return Const reference to algorithm name. Empty string if invalid ID or if using unsupported access mode.
   */
  const std::string& TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos( UInt_t _index, UInt_t _position ) {
    if ( getUsingNtupleMetadata() == kTRUE ) {
      return getTCT()->GetAlgNameFromSeqIDAndAlgPos( (int) _index, (int) _position );
    } else {
      Error("TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos", "XML based menu navigation not yet included.");
    }
    return Config::config().getStr(kBlankString);
  }
  
  /**
   * Fetch hash of HLT algorithm name using the parents sequence index and position of algorithm within sequence.
   * @param _index Index number of parent sequence.
   * @param _position Position of algorithm within parent sequence.
   * @return Const reference to algorithm name. Empty string if invalid ID or if using unsupported access mode.
   */
  UInt_t TrigConfInterface::getHLTAlgNameIDFromSeqIDAndAlgPos( UInt_t _index, UInt_t _position ) {
    if ( getUsingNtupleMetadata() == kTRUE ) {
      return getTCT()->GetAlgNameIDFromSeqIDAndAlgPos( (int) _index, (int) _position );
    } else {
      Error("TrigConfInterface::getHLTAlgNameIDFromSeqIDAndAlgPos", "XML based menu navigation not yet included.");
    }
    return 0;
  }
  
  /**
   * Fetch HLT algorithm class type name using the parents sequence index and position of algorithm within sequence.
   * @param _index Index number of parent sequence.
   * @param _position Position of algorithm within parent sequence.
   * @return Const reference to algorithm class type name. Empty string if invalid ID or if using unsupported access mode.
   */
  const std::string& TrigConfInterface::getHLTAlgClassNameFromSeqIDAndAlgPos( UInt_t _index, UInt_t _position ) {
    if ( getUsingNtupleMetadata() == kTRUE ) {
      return getTCT()->GetAlgClassNameFromSeqIDAndAlgPos( (Int_t) _index, (Int_t) _position );
    } else {
      Error("TrigConfInterface::getHLTAlgClassNameFromSeqIDAndAlgPos", "XML based menu navigation not yet included.");
    }
    return Config::config().getStr(kBlankString);
  }
  
  /**
   * Fetch hash of HLT algorithm class type name using the parents sequence index and position of algorithm within sequence.
   * @param _index Index number of parent sequence.
   * @param _position Position of algorithm within parent sequence.
   * @return Hash of algorithm class type name. 0 if invalid ID or if using unsupported access mode.
   */
  UInt_t TrigConfInterface::getHLTAlgClassNameIDFromSeqIDAndAlgPos( UInt_t _index, UInt_t _position ) {
    if ( getUsingNtupleMetadata() == kTRUE ) {
      return getTCT()->GetAlgClassNameIDFromSeqIDAndAlgPos( (Int_t) _index, (Int_t) _position );
    } else {
      Error("TrigConfInterface::getHLTAlgClassNameIDFromSeqIDAndAlgPos", "XML based menu navigation not yet included.");
    }
    return 0;
  }
  
  /**
   * Fetch prescale factor for given chain name.
   * @param _chainName Name of chain to fetch prescale for.
   * @return Floating point prescale factor for chain in current menu. 0 on fail.
   */
  Float_t TrigConfInterface::getPrescale( std::string _chainName ) {
    if ( getUsingNtuplePrescales() == kTRUE ) {
      return getTCT()->GetPrescale( _chainName );
    } else {
      Error("TrigConfInterface::getPrescale", "XML based menu navigation not yet included.");
    }
    return 0;
  }
  
  // /**
  //  * Get if given chain was passed in the event by decoding the trigger bits.
  //  * @deprecated This is currently unsupported due to change in source of trigger information.
  //  * @see TrigCostData::getIsChainPassed(std::string& _n)
  //  * @param _chainName Name of chain to fetch decision for.
  //  * @return Boolean decision.
  //  */
  // Bool_t TrigConfInterface::getIsPassedPhysics( const std::string _chainName ) {
  //   assert( m_isConfigured );
  //   Error("TrigConfInterface::getIsPassedPhysics", "GETTING 'IS PASSED' IS BROKEN FOR NOW."); //TODO remove this, it's not coming back any tme soon
  //   return getTDT()->IsPassed( _chainName );
  // }
  
  // /**
  //  * Get list of triggers which passed at L2.
  //  * @deprecated This is currently unsupported due to change in source of trigger information.
  //  * @return Vector of string chain names for chains which passed this event.
  //  */ 
  // std::vector< std::string > TrigConfInterface::getPassedL2Triggers() {
  //   assert( m_isConfigured );
  //   Error("TrigConfInterface::getPassedL2Triggers", "GETTING 'IS PASSED' IS BROKEN FOR NOW."); //TODO remove this, it's not coming back any tme soon
  //   return getTDT()->GetPassedL2Triggers();
  // }
  
  // /**
  //  * Get list of triggers which passed at EF.
  //  * @deprecated This is currently unsupported due to change in source of trigger information.
  //  * @return Vector of string chain names for chains which passed this event.
  //  */
  // std::vector< std::string > TrigConfInterface::getPassedEFTriggers() {
  //   assert( m_isConfigured );
  //   Error("TrigConfInterface::getPassedEFTriggers", "GETTING 'IS PASSED' IS BROKEN FOR NOW."); //TODO remove this, it's not coming back any tme soon
  //   return getTDT()->GetPassedEFTriggers();
  // }
  
  /**
   * Designed for internal use, get a pointer to the TriggerDescisionTool object.
   * @return Pointer to configured D3PD::TrigDecisionToolD3PD.
   */
  D3PD::TrigDecisionToolD3PD* TrigConfInterface::getTDT() {
    assert( m_isConfigured );
    return m_tdt;
  }
  
  /**
   * Designed for internal use, get a pointer to the TriggerDescisionTool object.
   * @return Const pointer to configured D3PD::TrigConfigSvcD3PD.
   */
  const D3PD::TrigConfigSvcD3PD* TrigConfInterface::getTCT() {
    assert( m_isConfigured );
    if ( !m_tct) {
      m_tct = dynamic_cast<D3PD::TrigConfigSvcD3PD*>( &(m_tdt->GetConfigSvc()) );
    }
    return m_tct;
  }
  
  /**
   * Export current trigger configuration to basic HTML page.
   */
  void TrigConfInterface::dump() {
  
    const std::string _sm = intToString( getCurrentSMK() );
    
    const std::string _fileName = std::string( Config::config().getStr(kOutputDirectory) 
      + "/TriggerConfiguration_" 
      + getCurrentDBKey().name() + ".json" );
    Info("TrigConfInterface::dump", "Saving trigger configuration to %s", _fileName.c_str() );
    
    //std::ofstream _foutHtml( std::string(_fileName + ".htm").c_str() );
    std::ofstream _foutJson( _fileName.c_str() );

    JsonExport* _json = new JsonExport();

    //_foutHtml << "<!DOCTYPE html><html><head><title>Menu Configuration</title></head><body>" << std::endl;
    //_foutHtml << "<h1>Trigger Menu Configuration: " << getCurrentDBKey().name() << "</h1><p>" << std::endl;
    //_foutHtml << "<ul>" << std::endl;

    _json->addNode(_foutJson, "Trigger", "TRG");
    _json->addNode(_foutJson, "LVL1", "LV1");     // BEGIN 
    Bool_t _jsonDoingL1 = kTRUE;
    // CHAIN
    for (UInt_t _c = 0; _c < getTCT()->GetChainN(); ++_c) {
      Bool_t _isL1 = kFALSE;
      //std::string _counter = "</b>, Counter:<i>";
      if ( getTCT()->GetChainLevel(_c) == 1) {
        //_counter = "</b>, CTPID:<i>";
        _isL1 = kTRUE;
      } else if (_jsonDoingL1 == kTRUE) {
        //switch over to HLT
        _json->endNode(_foutJson); //END L1
        _json->addNode(_foutJson, "HLT", "HLT");  // BEGINHLT
        _jsonDoingL1 = kFALSE;
      }
      //_foutHtml << "<hr>" << std::endl;
      //_foutHtml << "<li>Trigger Chain: Name:<b>" << getTCT()->GetChainName(_c) << _counter << getTCT()->GetChainCounter(_c) << "</i></li>" << std::endl;
      _json->addNode(_foutJson, getTCT()->GetChainName(_c), "C");
      // _foutHtml << "<li>Prescale:<i>" << getPrescale( getTCT()->GetChainName(_c) ) << "</i></li>" << std::endl;
      // if (getTCT()->GetChainGroupNameSize(_c)) {
      //   _foutHtml << "<li>Groups:<i>";
      //   for (UInt_t _g = 0; _g < getTCT()->GetChainGroupNameSize(_c); ++_g) {
      //     _foutHtml << getTCT()->GetChainGroupName( _c, _g );
      //     if (_g != getTCT()->GetChainGroupNameSize(_c) - 1) _foutHtml << ",";
      //   }
      //   _foutHtml << "</i></li>" << std::endl;
      // }
      // if ( getTCT()->GetChainLevel(_c) > 1) {
      //   _foutHtml << "<li>Lower Chain Name:" << getTCT()->GetLowerChainName( getTCT()->GetChainName(_c) ) << "</li>" << std::endl;
      // }
      // if ( getTCT()->GetChainEBHypoNameSize(_c) > 0) {
      //   std::cout << "<li>Enhanced Bias Hypo Items:<i>";
      //   _foutHtml << "<li>Enhanced Bias Hypo Items:<i>";
      //   for (UInt_t _g = 0; _g < getTCT()->GetChainEBHypoNameSize(_c); ++_g) {
      //     _foutHtml << getTCT()->GetChainEBHypoName( _c, _g );
      //     if (_g != getTCT()->GetChainEBHypoNameSize(_c) - 1) _foutHtml << ",";
      //   }
      //   _foutHtml << "</i></li>" << std::endl;
      // }
      //_foutHtml << " <ol>" << std::endl;
      // SIGNATURE
      for (UInt_t _s = 0; _s < getTCT()->GetSigN(_c); ++_s) {
        UInt_t _nOutputTEs = getTCT()->GetSigNOutputTE(_c, _s);
        if (!_isL1) _json->addNode(_foutJson, getTCT()->GetSigLabel(_c, _s), "SI"); // SIG STEP
        //_foutHtml << " <li>Signature Step: Label:<b>" << getTCT()->GetSigLabel(_c, _s) << "</b>, Logic:<i>" << getTCT()->GetSigLogic(_c, _s) << "</i></li>" << std::endl;
        //_foutHtml << "  <ul>" << std::endl;
        // SIG -> SEQ
        for (Int_t _t = _nOutputTEs - 1; _t >= 0; --_t) {
          UInt_t _outputTE = getTCT()->GetSigOutputTE(_c, _s, _t);
          UInt_t _seq = getTCT()->GetSequenceIndex( _outputTE );
          if (_seq != UINT_MAX) {
            //_foutHtml << "  <li> Sequence: Name:<b>" << getTCT()->GetSeqName(_seq) << "</b>, Index:<i>" << getTCT()->GetSeqIndex(_seq) << "</i></li>" << std::endl;
            if (_isL1) _json->addLeaf(_foutJson, getTCT()->GetSeqName(_seq), "L1"); // L1 BG (does not need closing)
            else _json->addNode(_foutJson, getTCT()->GetSeqName(_seq), "SE"); // SEQ STEP
            //_foutHtml << "   <ol>" << std::endl;
            // ALG
            for (UInt_t _a = 0; _a < getTCT()->GetAlgN(_seq); ++_a) {
              _json->addLeaf(_foutJson, getTCT()->GetAlgName(_seq, _a), "A"); //ALG LEAF (does not need closing)
              //_foutHtml << "   <li>Algorithm Instance/Class:<b>" << getTCT()->GetAlgName(_seq, _a) << " </b>/<b> " << getTCT()->GetAlgTypeName(_seq, _a) << "</b></li>" << std::endl;
            }
            //_foutHtml << "   </ol>" << std::endl;
            if (!_isL1) _json->endNode(_foutJson); // SEQ STEP 
          }
        }
        //_foutHtml << "  </ul>" << std::endl;
        if (!_isL1) _json->endNode(_foutJson); // SIG STEP
      }
      //_foutHtml << " </ol>" << std::endl;
      _json->endNode(_foutJson); // CHAIN
    }
    _json->endNode(_foutJson); //END HLT
    _json->endNode(_foutJson); //END TRIGGER
    _foutJson.close();
    //_foutHtml << "</ul>" << std::endl;
    //_foutHtml << "</p></body></html>" << std::endl;
    //_foutHtml.close();

  }
  
  /**
   * Reset the configuration interface, removing any TriggerDescisionTool object.
   */
  void TrigConfInterface::reset() {
    Info("TrigConfInterface::reset", "Clearing Memory.");
    delete m_tdt;
    m_tdt = 0;
    m_tct = 0;
    m_isConfigured = false;
  }
  
  /**
   * Perform diagnostic dumping of configuration information to the terminal
   */
  void TrigConfInterface::debug() {
    Info("TrigConfInterface::debug", " Printing Full Trigger Menu");
    for (UInt_t _s = 0; _s < getTCT()->GetSeqN(); ++_s) {
      Info("TrigConfInterface::debug", "Sequence: ID=%i\tIndex=%i\tNAlgs=%i\tNInputTEs=%i\tName=%s",
           getTCT()->GetSeqID(_s),
           getTCT()->GetSeqIndex(_s),
           getTCT()->GetAlgN(_s),
           getTCT()->GetSeqNInputTEs(_s),
           getTCT()->GetSeqName(_s).c_str() );
      for (UInt_t _t = 0; _t < getTCT()->GetSeqNInputTEs(_s); ++_t) {
        Info("TrigConfInterface::debug", "\tSequence Input TE #%i: %u", _t, getTCT()->GetSeqInputTE(_s, _t) );
      }
      // Algorithm
      for (UInt_t _a = 0; _a < getTCT()->GetAlgN(_s); ++_a) {
        Info("TrigConfInterface::debug", "\tAlgorithm: Index=%i\tPosition=%i\tNameHash=%u\tTypeNameHash=%u\tName=%s\tTypeName=%s",
             getTCT()->GetAlgIndex(_s, _a),
             getTCT()->GetAlgPosition(_s, _a),
             getTCT()->GetAlgNameID(_s, _a),
             getTCT()->GetAlgTypeID(_s, _a),
             getTCT()->GetAlgName(_s, _a).c_str(),
             getTCT()->GetAlgTypeName(_s, _a).c_str() );
      }
    }
  }

  UInt_t      TrigConfInterface::getChainN() { 
    return getTCT()->GetChainN(); 
  }

  UInt_t      TrigConfInterface::getChainLevel(UInt_t _c) {
    return getTCT()->GetChainLevel(_c);
  }

  UInt_t      TrigConfInterface::getChainCounter(UInt_t _c) { 
    return getTCT()->GetChainCounter(_c); 
  }

  std::string TrigConfInterface::getChainName(UInt_t _c) { 
    return getTCT()->GetChainName(_c); 
  }

  UInt_t      TrigConfInterface::getChainEBHypoNameSize(UInt_t _c) {
    return getTCT()->GetChainEBHypoNameSize(_c); 
  }

  std::string TrigConfInterface::getChainEBHypoName(UInt_t _c, UInt_t _h) { 
    return getTCT()->GetChainEBHypoName(_c, _h); 
  }  

  UInt_t      TrigConfInterface::getChainGroupsNameSize(UInt_t _c) { 
    return getTCT()->GetChainGroupNameSize(_c); 
  }

  std::string TrigConfInterface::getChainGroupName(UInt_t _c, UInt_t _g) { 
    return getTCT()->GetChainGroupName(_c, _g); 
  }

  std::string TrigConfInterface::getChainRatesGroupName(UInt_t _c) { 
    for (UInt_t _group = 0; _group < getTCT()->GetChainGroupNameSize(_c); ++_group) {
      std::string _groupName = getTCT()->GetChainGroupName(_c, _group);
      if (_groupName.find("Rate:") != std::string::npos || _groupName.find("RATE:") != std::string::npos) {
        std::replace( _groupName.begin(), _groupName.end(), ':', '_'); // A ":" can cause issues in TDirectory naming structure. "_" is safe.
        return _groupName;
      }
    }
    return Config::config().getStr(kBlankString);
  }
  
} // namespace TrigCostRootAnalysis
