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
#include <iomanip>
#include <sstream>
#include <utility> //std::pair
#include <cmath>  //HUGE_VAL
#include <assert.h>

// Local include(s):
#include "../TrigCostRootAnalysis/LumiCollector.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"


// ROOT include(s):
#include <TError.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TSystem.h>
#include <TDirectory.h>

namespace TrigCostRootAnalysis {

  /**
   * Default lumi collector constructor
   */
  LumiCollector::LumiCollector() :
    m_lumiLength(),
    m_eventsProcessedPerLB(),
    m_totalLumiLength(0),
    m_useDefault(kFALSE),
    m_dataStore() {
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
      if (Config::config().getDisplayMsg(kMsgNoLumiInfo) == kTRUE) {
        Warning("LumiCollector::recordEventLumi", "No lumi block length info in file. Using default length %i s.",
          Config::config().getInt(kDefaultLBLength) );
      }
      m_useDefault = kTRUE;
    }

    if (m_useDefault == kTRUE) {
      _length = Config::config().getInt(kDefaultLBLength);
    }

    assert(_lumiBlock >= 0);
    assert(_length > 0 && _length == _length && _length < HUGE_VAL); //Check -ve, NaN, Inf

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
   * Get the time for a supplied lumiblock from internal map. Should the service be disabled, this will return the
   * default lumi block length.
   * @see m_defaultLumiBlockLength
   * @param _lumiBlock Luminosity block to fetch time for.
   * @return Length of luminosity block in seconds if present in map, 0 if not present or m_defaultLumiBlockLength if disabled.
   */
  Float_t LumiCollector::getLumiBlockTime(Int_t _lumiBlock) {

    assert(_lumiBlock >= 0);

    if (TrigXMLService::trigXMLService().getParsedRunXML() == kFALSE || Config::config().getInt(kNoLBRescaling) == kTRUE) {

      // Basic mode. No partial-run scaling
      if ( m_lumiLength.count(_lumiBlock) == 1 ) {
        return m_lumiLength[_lumiBlock];
      }

    } else {

      // Advanced mode, use partial-run scaling
      Float_t _eventsSeen = m_eventsProcessedPerLB[_lumiBlock];
      Float_t _eventsTotal = TrigXMLService::trigXMLService().getOnlineEventsInLB( _lumiBlock );
      Float_t _fractionSeen = _eventsSeen / _eventsTotal;
      if (isZero(_eventsTotal) == kTRUE) _fractionSeen = 0.;
      if ( m_lumiLength.count(_lumiBlock) == 1 ) {
        return m_lumiLength[_lumiBlock] * _fractionSeen;
      }

    }

    Error("LumiCollector::getLumiBlockTime", "Requested length of unknown lumi block %i returning 0.", _lumiBlock);
    return 0;

  }

  Float_t LumiCollector::getTotalLumiBlockTime() {
    if (TrigXMLService::trigXMLService().getParsedRunXML() == kFALSE || Config::config().getInt(kNoLBRescaling) == kTRUE) {
      // Basic mode?
      return m_totalLumiLength;
    } else {
      //Advanced mode // TODO CACHE THIS INFO!
      Float_t _totalTime = 0.;
      static Bool_t _doStatistics = kTRUE;
      std::stringstream _stats;
      for (IntIntMapIt_t _it = m_eventsProcessedPerLB.begin(); _it != m_eventsProcessedPerLB.end(); ++_it) {
        Int_t _lb = (*_it).first;
        Float_t _eventsSeen = m_eventsProcessedPerLB[_lb];
        Float_t _eventsTotal = TrigXMLService::trigXMLService().getOnlineEventsInLB( _lb );
        Float_t _fractionSeen = _eventsSeen / _eventsTotal;
        if (isZero(_eventsTotal) == kTRUE) _fractionSeen = 0.;
        Float_t _effectiveLBTime = m_lumiLength[_lb] * _fractionSeen;
        if (_doStatistics) _stats << "[LB:" << _lb << ",Len:"<< m_lumiLength[_lb]<<"s,Saw:"<< std::setprecision(2) << _fractionSeen*100. <<"%]";
        _totalTime += _effectiveLBTime;
      }
      if (_doStatistics) {
        Info("LumiCollector::getTotalLumiBlockTime", "Overall, %.2f%% of %i lumi blocks were processed. Effective time %.2fs", (_totalTime/m_totalLumiLength)*100., (Int_t)m_eventsProcessedPerLB.size(), _totalTime);
        if (Config::config().debug()) Info("LumiCollector::getTotalLumiBlockTime", "Per LB breakdown: %s", _stats.str().c_str());
        _doStatistics = kFALSE;
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
