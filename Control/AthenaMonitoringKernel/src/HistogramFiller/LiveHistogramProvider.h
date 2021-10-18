/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_LiveHistogramProvider_h
#define AthenaMonitoringKernel_HistogramFiller_LiveHistogramProvider_h

#include <memory>

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/HistogramDef.h"
#include "AthenaMonitoringKernel/IHistogramProvider.h"

#include "HistogramFactory.h"

namespace Monitored {
  /**
   * @brief Provides latest-N-lumiblock histograms to be filled
   *
   * Provides histograms which shows only information from the last N lumiblocks. The
   * highest encountered lumiblock is calculated, and only data from Max-N to N is shown.
   * Note: kLBNLive must be defined in histogram definition options.
   */
  class LiveHistogramProvider : public IHistogramProvider {
  public:

    /**
     * @brief Constructor
     *
     * @param gmTool Source of the lumi block info.
     * @param factory ROOT object factory.
     * @param def Definition of the histogram.
     */
    LiveHistogramProvider(
      GenericMonitoringTool* const gmTool,
      std::shared_ptr<HistogramFactory> factory,
      const HistogramDef& histDef)
      : IHistogramProvider()
      , m_gmTool(gmTool)
      , m_factory(factory)
      , m_histDef(new HistogramDef(histDef))
      {}

    /**
     * @brief Getter of ROOT histogram
     *
     * Each time the method is called, factory produces ROOT object based on the current
     * lumi block. Note: ROOT objects are cached at the factory. Nevertheless, it is
     * recommended to call this method as rarely as possible.
     *
     * @return ROOT object
     */
    TNamed* histogram() override {
      // Get the LB for this event.
      const int lumiBlock = m_gmTool->lumiBlock();

      if (lumiBlock > m_currentLumiBlock) {
        // Update the variable keeping track of the highest LB.
        m_currentLumiBlock = std::max((float) lumiBlock, m_histDef->xmax);

        if (!m_currentHistogram) {
          // The histogram does not exist yet and must be created.
          updateHistDef();
          m_currentHistogram = m_factory->create(*m_histDef);
        } else {
          // The histogram exists and needs to be rolled
          if (m_histDef->type=="TEfficiency") {
            // Roll a TEfficiency (same process as the codeblock immediately above)
            TH1* totalClone = ((TEfficiency*) m_currentHistogram)->GetCopyTotalHisto();
            TH1* passedClone = ((TEfficiency*) m_currentHistogram)->GetCopyPassedHisto();
            m_factory->remove(*m_histDef);
            updateHistDef();
            TEfficiency* eNew = (TEfficiency*) m_factory->create(*m_histDef);
            TH1* totalNew = eNew->GetCopyTotalHisto();
            TH1* passedNew = eNew->GetCopyPassedHisto();
            copyDataToNewHistogram(totalClone, totalNew);
            copyDataToNewHistogram(passedClone, passedNew);
            eNew->SetTotalHistogram(*totalNew, "");
            eNew->SetPassedHistogram(*passedNew, "");
            m_currentHistogram = eNew;
            delete totalClone;
            delete totalNew;
            delete passedClone;
            delete passedNew;
	  } else if (m_histDef->type=="TProfile") {
	    // Store the data and deregister the old histogram.
            TProfile* hClone = (TProfile*) m_currentHistogram->Clone();
            m_factory->remove(*m_histDef);
            // Update the bin ranges and register the new histogram.
            updateHistDef();
            TProfile* hNew = (TProfile*) m_factory->create(*m_histDef);
            // Fill it with the old histogram's data and update pointer.
            copyDataToNewHistogram(hClone, hNew);
            m_currentHistogram = hNew;
            // Free the memory used by the clone
            delete hClone;
          } else {
            // Store the data and deregister the old histogram.
            TH1* hClone = (TH1*) m_currentHistogram->Clone();
            m_factory->remove(*m_histDef);
            // Update the bin ranges and register the new histogram.
            updateHistDef();
            TH1* hNew = (TH1*) m_factory->create(*m_histDef);
            // Fill it with the old histogram's data and update pointer.
            copyDataToNewHistogram(hClone, hNew);
            m_currentHistogram = hNew;
            // Free the memory used by the clone
            delete hClone;
          }
        }
      }
      return m_currentHistogram;
    }

    /**
     * @brief Updates HistogramDef xmin, xmax and xbins members.
     */
    void updateHistDef() {
        m_histDef->xmax = std::max(m_currentLumiBlock + 0.5f, m_histDef->xmax);
        m_histDef->xmin = std::max(m_currentLumiBlock + 0.5f - m_histDef->kLive, 0.5f);
        m_histDef->xbins = m_histDef->xmax - m_histDef->xmin;
    }

    /**
     * @brief Copies bin contents from an old to a new histogram.
     */
    void copyDataToNewHistogram(TH1* hOld, TH1* hNew) {
      int offset = hNew->GetXaxis()->GetXmax() - hOld->GetXaxis()->GetXmax();
      int nNewEntries(0);
      bool sumw2Filled = (hOld->GetSumw2N()>0);

      // Loop through the old histogram bins
      for (int oldBin=0; oldBin < hOld->GetNcells(); oldBin++) {
        // Convert global bin number into x-y-z bin number
        int oldBinX, oldBinY, oldBinZ;
        hOld->GetBinXYZ(oldBin, oldBinX, oldBinY, oldBinZ);
        if ((oldBinX-offset < 1) || hOld->IsBinUnderflow(oldBin, 1) || hOld->IsBinOverflow(oldBin, 1)) {
          // Overflow bins are ignored since their meaning has changed.
          continue;
        } else {
          // Get the global bin coordinate of this (x, y, z) bin coordinates.
          int newBin = hNew->GetBin(oldBinX-offset, oldBinY, oldBinZ);
	  if (hOld->GetBinContent(oldBin)) hNew->SetBinContent(newBin, hOld->GetBinContent(oldBin));
	  if (sumw2Filled) {
	    hNew->SetBinError(newBin, hOld->GetBinError(oldBin));
	    nNewEntries+=(*hOld->GetSumw2())[oldBin]; // works correctly only for weight=1
	  }
        }
      }
      // Update the total number of entries member.
      if (sumw2Filled) hNew->SetEntries(nNewEntries);
      else hNew->SetEntries(hOld->GetEntries()); // a choice since there is no way to get it right.
    }  

    void copyDataToNewHistogram(TProfile* hOld, TProfile* hNew) {
      int offset = hNew->GetXaxis()->GetXmax() - hOld->GetXaxis()->GetXmax();
      int nNewEntries(0);
      bool sumw2Filled = (hOld->GetSumw2N()>0);
      
      // Loop through the old histogram bins
      for (int oldBin=0; oldBin < hOld->GetNcells(); oldBin++) {
	// Convert global bin number into x-y-z bin number
	int oldBinX, oldBinY, oldBinZ;
	hOld->GetBinXYZ(oldBin, oldBinX, oldBinY, oldBinZ);
	if ((oldBinX-offset < 1) || hOld->IsBinUnderflow(oldBin, 1) || hOld->IsBinOverflow(oldBin, 1)) {
	  // Overflow bins are ignored since their meaning has changed.
	  continue;
	} else {
	  // Get the global bin coordinate of this (x, y, z) bin coordinates.
	  int newBin = hNew->GetBin(oldBinX-offset, oldBinY, oldBinZ);
	  int oldBinEntries = hOld->GetBinEntries(oldBin);
	  if (oldBinEntries>0) {
	    nNewEntries += oldBinEntries;
	    hNew->SetBinEntries(newBin, oldBinEntries);
	    (*hNew)[newBin] = (*hOld)[oldBin];
	    (*hNew->GetSumw2())[newBin] = (*hOld->GetSumw2())[oldBin];
	  }
	}
      }
      // Update the total number of entries member.
      if (sumw2Filled) hNew->SetEntries(nNewEntries);
      else hNew->SetEntries(hOld->GetEntries()); // a choice since there is no way to get it right.
    } 

  private:
    GenericMonitoringTool* const m_gmTool;
    std::shared_ptr<HistogramFactory> m_factory;
    std::shared_ptr<HistogramDef> m_histDef;
    TNamed *m_currentHistogram = nullptr;
    int m_currentLumiBlock = 0;
  };  
}

#endif /* AthenaMonitoringKernel_HistogramFiller_LiveHistogramProvider_h */
