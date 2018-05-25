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
   * @param lumiBlock The luminosity block
   * @param length The length in seconds.
   */
  void LumiCollector::recordEventLumi(Int_t lumiBlock, Float_t length) {
    if (isZero(length) && m_useDefault == kFALSE) {
      if (Config::config().getDisplayMsg(kMsgNoLumiInfo) == kTRUE) {
        Warning("LumiCollector::recordEventLumi", "No lumi block length info in file. Using default length %i s.",
                Config::config().getInt(kDefaultLBLength));
      }
      m_useDefault = kTRUE;
    }

    if (m_useDefault == kTRUE) {
      length = Config::config().getInt(kDefaultLBLength);
    }

    assert(lumiBlock >= 0);
    assert(length > 0 && length == length && length < HUGE_VAL); //Check -ve, NaN, Inf

    m_dataStore.store(kVarEventsPerLumiblock, lumiBlock, 1.);

    // Look to see if we've seen this LB yet
    if (m_lumiLength.count(lumiBlock) == 0) {
      m_totalLumiLength += length;
      m_lumiLength[ lumiBlock ] = length;
    }

    m_eventsProcessedPerLB[ lumiBlock ]++;
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
   * @param lumiBlock Luminosity block to fetch time for.
   * @return Length of luminosity block in seconds if present in map, 0 if not present or m_defaultLumiBlockLength if
   *disabled.
   */
  Float_t LumiCollector::getLumiBlockTime(Int_t lumiBlock) {
    assert(lumiBlock >= 0);

    if (TrigXMLService::trigXMLService().getParsedRunXML() == kFALSE ||
        Config::config().getInt(kNoLBRescaling) == kTRUE) {
      // Basic mode. No partial-run scaling
      if (m_lumiLength.count(lumiBlock) == 1) {
        return m_lumiLength[lumiBlock];
      }
    } else {
      // Advanced mode, use partial-run scaling
      Float_t eventsSeen = m_eventsProcessedPerLB[lumiBlock];
      Float_t eventsTotal = TrigXMLService::trigXMLService().getOnlineEventsInLB(lumiBlock);
      Float_t fractionSeen = eventsSeen / eventsTotal;
      if (isZero(eventsTotal) == kTRUE) fractionSeen = 0.;
      if (m_lumiLength.count(lumiBlock) == 1) {
        return m_lumiLength[lumiBlock] * fractionSeen;
      }
    }

    Error("LumiCollector::getLumiBlockTime", "Requested length of unknown lumi block %i returning 0.", lumiBlock);
    return 0;
  }

  Float_t LumiCollector::getTotalLumiBlockTime() {
    if (TrigXMLService::trigXMLService().getParsedRunXML() == kFALSE ||
        Config::config().getInt(kNoLBRescaling) == kTRUE) {
      // Basic mode?
      return m_totalLumiLength;
    } else {
      //Advanced mode // TODO CACHE THIS INFO!
      Float_t totalTime = 0.;
      static Bool_t doStatistics = kTRUE;
      std::stringstream stats;
      for (IntIntMapIt_t it = m_eventsProcessedPerLB.begin(); it != m_eventsProcessedPerLB.end(); ++it) {
        Int_t lb = (*it).first;
        Float_t eventsSeen = m_eventsProcessedPerLB[lb];
        Float_t eventsTotal = TrigXMLService::trigXMLService().getOnlineEventsInLB(lb);
        Float_t fractionSeen = eventsSeen / eventsTotal;
        if (isZero(eventsTotal) == kTRUE) fractionSeen = 0.;
        Float_t effectiveLBTime = m_lumiLength[lb] * fractionSeen;
        if (doStatistics) stats << "[LB:" << lb << ",Len:" << m_lumiLength[lb] << "s,Saw:" <<
            std::setprecision(2) << fractionSeen * 100. << "%]";
        totalTime += effectiveLBTime;
      }
      if (doStatistics) {
        Info("LumiCollector::getTotalLumiBlockTime",
             "Overall, %.2f%% of %i lumi blocks were processed. Effective time %.2fs",
             (totalTime / m_totalLumiLength) * 100., (Int_t) m_eventsProcessedPerLB.size(), totalTime);
        if (Config::config().debug()) Info("LumiCollector::getTotalLumiBlockTime", "Per LB breakdown: %s",
                                           stats.str().c_str());
        doStatistics = kFALSE;
      }
      return totalTime;
    }
  }

  void LumiCollector::saveOutput() {
    Bool_t doRoot = Config::config().getInt(kOutputRoot);
    Bool_t doCanv = Config::config().getInt(kOutputCanvas);
    Bool_t doHist = Config::config().getInt(kOutputHist);
    Bool_t doImage = Config::config().getInt(kOutputImage);
    Bool_t doPng = Config::config().getInt(kOutputPng);
    Bool_t doPdf = Config::config().getInt(kOutputPdf);

    if (doRoot == kFALSE && doImage == kFALSE) return;

    //disableROOTMsg();

    TCanvas* c = new TCanvas();
    c->SetLogy(kTRUE);
    c->SetBatch(kTRUE);

    const std::string outputFolder = Config::config().getStr(kOutputDirectory);
    const std::string imgFolder = Config::config().getStr(kOutputImageDirectory);
    const std::string summaryFolder = Config::config().getStr(kOutputSummaryDirectory);
    ConfKey_t plotName = kVarEventsPerLumiblock;

    if (doRoot) gDirectory->mkdir(summaryFolder.c_str());
    if (doRoot) gDirectory->cd(summaryFolder.c_str());

    const std::string outputPath = outputFolder + "/" + imgFolder + "/" + summaryFolder;
    if (doImage) gSystem->mkdir(outputPath.c_str(), kTRUE);

    TH1F* h = m_dataStore.getHist(plotName, kSavePerCall);

    if (h != 0) {
      plotHistogram(h);

      if (doPng) c->Print(std::string(outputPath + "/" + Config::config().getStr(plotName) + ".png").c_str());
      if (doPdf) c->Print(std::string(outputPath + "/" + Config::config().getStr(plotName) + ".pdf").c_str());
      if (doCanv) c->Write(std::string("c_" + Config::config().getStr(plotName)).c_str());
      h->GetYaxis()->SetRangeUser(0.1, h->GetBinContent(h->GetMaximumBin()) * 1.1);
      if (doHist) h->Write(std::string("h_" + Config::config().getStr(plotName)).c_str());
    }

    if (doRoot) gDirectory->cd("..");

    delete c;

    //enableROOTMsg();
  }

  void LumiCollector::print() {
    Info("LumiCollector::print", "Total time of all LBs: %f", m_totalLumiLength);
    for (IntFloatMapIt_t it = m_lumiLength.begin(); it != m_lumiLength.end(); ++it) {
      Info("LumiCollector::print", "LB %i has time %f", it->first, it->second);
    }
  }
} // namespace TrigCostRootAnalysis
