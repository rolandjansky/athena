// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>
#include <map>
#include <utility> //std::pair
#include <cmath>  //HUGE_VAL
#include <assert.h>

// Local include(s):
#include "../TrigCostRootAnalysis/LumiCollector.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"

// ROOT include(s):
#include <TError.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TXMLEngine.h>
#include <TObjString.h>
#include <TDOMParser.h>
#include <TXMLNode.h>
#include <TXMLDocument.h>
#include <TXMLAttr.h>

// ATHENA Includes
#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif // not ROOTCORE

namespace TrigCostRootAnalysis {

  /**
   * Default lumi collector constructor
   */
  LumiCollector::LumiCollector() : m_lumiLength(), m_eventsProcessedPerLB(), m_totalEventsPerLB(), m_totalLumiLength(0), m_useDefault(kFALSE), m_dataStore(), m_triedXMLLoad(kFALSE) {
    //m_dataStore = new DataStore();
    m_dataStore.newVariable(kVarEventsPerLumiblock).setSavePerEvent("Events per Lumi Block;Lumi Block;Events");
  }

  /**
   * lumi collector destructor
   */
  LumiCollector::~LumiCollector() {
  }


  /**
   * Record for later use the length of this luminosity block. Passing a length of 0 will disable the monitor which
   * will fall-back on its default luminosity block time rather than the more accurate ones from COOL.
   * @param _lumiBlock The luminosity block
   * @param _length The length in seconds.
   */
  void LumiCollector::recordEventLumi(Int_t _lumiBlock, Float_t _length) {
  
    if (isZero(_length) && m_useDefault == kFALSE) {
      // This has become spammy
      //Warning("LumiCollector::recordEventLumi", "No lumi block length info in file. Using default length %i s.", Config::config().getInt("defaultLBLength") );
      m_useDefault = kTRUE;
    }
    
    if (m_useDefault == kTRUE) {
      _length = Config::config().getInt(kDefaultLBLength);
    }
    
    assert(_lumiBlock >= 0);
    assert(_length > 0 && _length == _length && _length < HUGE_VAL); //Check -ve, NaN, Inf

    if (m_triedXMLLoad == kFALSE) tryXMLLoad();

    m_dataStore.store(kVarEventsPerLumiblock, _lumiBlock, 1.);

    // Look to see if we've seen this LB yet
    if ( m_lumiLength.count(_lumiBlock) == 0) {
      m_totalLumiLength += _length;
      m_lumiLength[ _lumiBlock ] = _length;
    }

    m_eventsProcessedPerLB[ _lumiBlock ]++;
    
  }

  /**
   * @return The size of the lumi length map - one entry per LB
   */
  UInt_t LumiCollector::getNLumiBlocks() {
    return m_lumiLength.size();
  }

  /**
   * See if we can load how many events to expect for each LB. If we can, then we will know what fraction of events we have actually seen!
   */
  void LumiCollector::tryXMLLoad() {

    m_triedXMLLoad = kTRUE;


    // Try one. Use the external (AFS) data path

    const std::string _path = std::string("events_per_lb_") + intToString(Config::config().getInt(kRunNumber)) + std::string(".xml");
  
    TXMLEngine* _xml = new TXMLEngine();
    XMLDocPointer_t _xmlDoc = 0;

    if (Config::config().getInt(kIsRootCore) == kTRUE) {
      _xmlDoc = _xml->ParseFile( std::string( Config::config().getStr(kDataDir) + _path ).c_str() );
    } else {
// CAUTION - "ATHENA ONLY" CODE
#ifndef ROOTCORE
      std::string _locAthena = PathResolverFindDataFile( _path );
      if (_locAthena != Config::config().getStr(kBlankString)) {
        _xmlDoc = _xml->ParseFile( _locAthena.c_str() );
      }
#endif // not ROOTCORE
    }

    if (_xmlDoc == 0) {
      Warning("LumiCollector::tryXMLLoad","Cannot find lumi block entries %s, hence will not do advanced partial-run rate scaling.", _path.c_str() );
      return;
    }

    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    assert( _xml->GetNodeName(_mainNode) == std::string("LUMIBLOCK") );
    XMLNodePointer_t _listNode = _xml->GetChild( _mainNode );
    Int_t _totalEvents = 0;

    while ( _listNode != 0 ) { // Loop over all menu elements
      const std::string _listName = _xml->GetNodeName(_listNode);

      if (_listName != "LB_LIST") {
        _listNode = _xml->GetNext(_listNode);
        continue;
      }

      XMLNodePointer_t _lbNode = _xml->GetChild( _listNode );
      while( _lbNode != 0) {
        assert( _xml->GetNodeName(_lbNode) == std::string("lb") );
        Int_t _lb      = stringToInt( _xml->GetAttr(_lbNode, "id") );
        Int_t _nEvents = stringToInt( _xml->GetNodeContent(_lbNode) );
        m_totalEventsPerLB[_lb] = _nEvents;
        _totalEvents += _nEvents;
        _lbNode = _xml->GetNext(_lbNode);
      }

      _listNode = _xml->GetNext(_listNode);
    }

    if (Config::config().debug()) {
      Info("LumiCollector::tryXMLLoad","Got breakdown of events-per-lumiblock for %i events over %i LB in run %i.", _totalEvents, (Int_t)m_totalEventsPerLB.size(), Config::config().getInt(kRunNumber));
    }

    delete _xml;
  }
  
  /**
   * Get the time for a supplied lumiblock from internal map. Should the service be disabled, this will return the
   * default lumi block length.
   * @see m_defaultLumiBlockLength
   * @param _lumiBlock Luminosity block to fetch time for.
   * @return Length of luminosity block in seconds if present in map, 0 if not present or m_defaultLumiBlockLength if disabled.
   */
  Float_t LumiCollector::getLumiBlockTime(Int_t _lumiBlock) {
  
    assert(_lumiBlock >= 0);
    
    if ( m_lumiLength.count(_lumiBlock) == 1 ) {
      return m_lumiLength[_lumiBlock];
    } else {
      Error("LumiCollector::getLumiBlockTime", "Requested length of unknown lumi block %i returning 0.", _lumiBlock);
      return 0;
    }
  }

  Float_t LumiCollector::getTotalLumiBlockTime() {
    if (m_totalEventsPerLB.size() == 0) {
      // Basic mode?
      return m_totalLumiLength;
    } else {
      //Advanced mode // TODO CACHE THIS INFO!
      Float_t _totalTime = 0.;
      for (IntIntMapIt_t _it = m_eventsProcessedPerLB.begin(); _it != m_eventsProcessedPerLB.end(); ++_it) {
        Int_t _lb = (*_it).first;
        Float_t _eventsSeen = m_eventsProcessedPerLB[_lb];
        Float_t _eventsTotal = m_totalEventsPerLB[_lb];
        Float_t _fractionSeen = _eventsSeen / _eventsTotal;
        if (isZero(_eventsTotal) == kTRUE) _fractionSeen = 0.;
        Float_t _effectiveLBTime = m_lumiLength[_lb] * _fractionSeen;
        //Info("LumiCollector::getTotalLumiBlockTime", "In LB %i we saw %.0f of %0.f events, %.2f%%, so the effective LB time is %.2f", _lb, _eventsSeen, _eventsTotal, _fractionSeen*100., _effectiveLBTime);
        _totalTime += _effectiveLBTime;
      }
      return _totalTime;
    }
  }

  void LumiCollector::saveOutput() {
    Bool_t _doRoot = Config::config().getInt(kOutputRoot);
    Bool_t _doCanv = Config::config().getInt(kOutputCanvas);
    Bool_t _doHist = Config::config().getInt(kOutputHist);
    Bool_t _doImage = Config::config().getInt(kOutputImage);
    Bool_t _doPng = Config::config().getInt(kOutputPng);
    Bool_t _doPdf = Config::config().getInt(kOutputPdf);

    if (_doRoot == kFALSE && _doImage == kFALSE) return;

    //disableROOTMsg();
    
    TCanvas* _c = new TCanvas();
    _c->SetLogy(kTRUE);
    _c->SetBatch(kTRUE);
    
    const std::string _outputFolder = Config::config().getStr(kOutputDirectory);
    const std::string _imgFolder = Config::config().getStr(kOutputImageDirectory);
    const std::string _summaryFolder = Config::config().getStr(kOutputSummaryDirectory);
    ConfKey_t _plotName = kVarEventsPerLumiblock;

    if (_doRoot) gDirectory->mkdir( _summaryFolder.c_str() );
    if (_doRoot) gDirectory->cd   ( _summaryFolder.c_str() );
        
    const std::string _outputPath = _outputFolder + "/" + _imgFolder + "/" + _summaryFolder;
    if (_doImage) gSystem->mkdir( _outputPath.c_str(), kTRUE);
        
    TH1F* _h = m_dataStore.getHist(_plotName, kSavePerCall);

    if (_h != 0) {
      plotHistogram( _h );
            
      if (_doPng) _c->Print(std::string(_outputPath + "/" + Config::config().getStr(_plotName) + ".png").c_str());
      if (_doPdf) _c->Print(std::string(_outputPath + "/" + Config::config().getStr(_plotName) + ".pdf").c_str());
      if (_doCanv) _c->Write( std::string( "c_" + Config::config().getStr(_plotName)).c_str() );
      _h->GetYaxis()->SetRangeUser(0.1, _h->GetBinContent(_h->GetMaximumBin()) * 1.1);
      if (_doHist) _h->Write( std::string( "h_" + Config::config().getStr(_plotName)).c_str() );
    }
      
    if (_doRoot) gDirectory->cd( ".." );
       
    delete _c;
    
    //enableROOTMsg();

  }

  void LumiCollector::print() {
    Info("LumiCollector::print","Total time of all LBs: %f", m_totalLumiLength);
    for (IntFloatMapIt_t _it = m_lumiLength.begin(); _it != m_lumiLength.end(); ++_it) {
      Info("LumiCollector::print","LB %i has time %f", _it->first, _it->second);
    }
  }
  
} // namespace TrigCostRootAnalysis
