/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "EventLoop/Worker.h"

#include "TopEventSelectionTools/PlotManager.h"

#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TDirectoryFile.h"

#include <iostream>

namespace top {
  PlotManager::PlotManager(const std::string& name, TFile* outputFile, EL::Worker* wk) :
    m_wk(wk),
    m_name(name),
    m_plotDir(nullptr) {
    if (!m_wk) {
      m_plotDir = outputFile->GetDirectory(m_name.c_str());
      if (!m_plotDir) m_plotDir = outputFile->mkdir(m_name.c_str());
      m_plotDir = outputFile->GetDirectory(m_name.c_str());// this is needed if the name contains a sub-directory

      m_plotDir->cd();
    }
  }

  PlotManager::PlotManager(const std::string& sample, const std::string& channel, const std::string& syst,
                           TFile* outputFile) :
    m_wk(nullptr),
    m_name("SetMe"),
    m_plotDir(nullptr) {
    outputFile->cd();
    gDirectory->cd("/");

    if (!gDirectory->GetKey(sample.c_str())) {
      gDirectory->mkdir(sample.c_str());
    }
    gDirectory->cd(sample.c_str());

    if (!gDirectory->GetKey(channel.c_str())) {
      gDirectory->mkdir(channel.c_str());
    }
    gDirectory->cd(channel.c_str());

    if (!gDirectory->GetKey(syst.c_str())) {
      gDirectory->mkdir(syst.c_str());
    }
    gDirectory->cd(syst.c_str());

    TString plotDirName = sample + "/" + channel + "/" + syst;

    m_plotDir = outputFile->GetDirectory(plotDirName);
    m_plotDir->cd();
  }

  void PlotManager::addHist(const std::string& hname, const std::string& title, int bins, double start,
                            double end) const {
    TH1D* h = nullptr;

    if (!m_wk) {
      m_plotDir->cd();
      h = new TH1D(hname.c_str(), title.c_str(), bins, start, end);
    } else {
      std::string pathname = m_name + "/" + hname;
      h = new TH1D(pathname.c_str(), title.c_str(), bins, start, end);
      m_wk->addOutput(h);
    }

    checkDuplicate(hname);

    h->Sumw2();
    m_histograms.insert(make_pair(hname, h));
  }

  void PlotManager::addHist(const std::string& hname, const std::string& title, int bins, double* binArray) const {
    TH1D* h = nullptr;

    if (!m_wk) {
      m_plotDir->cd();
      h = new TH1D(hname.c_str(), title.c_str(), bins, binArray);
    } else {
      std::string pathname = m_name + "/" + hname;
      h = new TH1D(pathname.c_str(), title.c_str(), bins, binArray);
      m_wk->addOutput(h);
    }

    checkDuplicate(hname);

    h->Sumw2();
    m_histograms.insert(make_pair(hname, h));
  }

  void PlotManager::addHist(const std::string& hname, const std::string& title,
                            int xbins, double xstart, double xend,
                            int ybins, double ystart, double yend) const {
    TH2D* h = nullptr;

    if (!m_wk) {
      m_plotDir->cd();
      h = new TH2D(hname.c_str(), title.c_str(),
                   xbins, xstart, xend,
                   ybins, ystart, yend);
    } else {
      std::string pathname = m_name + "/" + hname;
      h = new TH2D(pathname.c_str(), title.c_str(),
                   xbins, xstart, xend,
                   ybins, ystart, yend);
      m_wk->addOutput(h);
    }

    checkDuplicate(hname);

    h->Sumw2();
    m_histograms.insert(make_pair(hname, h));
  }

  void PlotManager::addHist(const std::string& hname, const std::string& title,
                            int xbins, double* xbinArray,
                            int ybins, double* ybinArray) const {
    TH2D* h = nullptr;

    if (!m_wk) {
      m_plotDir->cd();
      h = new TH2D(hname.c_str(), title.c_str(),
                   xbins, xbinArray,
                   ybins, ybinArray);
    } else {
      std::string pathname = m_name + "/" + hname;
      h = new TH2D(pathname.c_str(), title.c_str(),
                   xbins, xbinArray,
                   ybins, ybinArray);
      m_wk->addOutput(h);
    }

    checkDuplicate(hname);

    h->Sumw2();
    m_histograms.insert(make_pair(hname, h));
  }

  void PlotManager::addHist(const std::string& hname, const std::string& title,
                            int xbins, double* xbinArray,
                            int ybins, double ystart, double yend) const {
    TH2D* h = nullptr;

    if (!m_wk) {
      m_plotDir->cd();
      h = new TH2D(hname.c_str(), title.c_str(),
                   xbins, xbinArray,
                   ybins, ystart, yend);
    } else {
      std::string pathname = m_name + "/" + hname;
      h = new TH2D(pathname.c_str(), title.c_str(),
                   xbins, xbinArray,
                   ybins, ystart, yend);
      m_wk->addOutput(h);
    }

    checkDuplicate(hname);

    h->Sumw2();
    m_histograms.insert(make_pair(hname, h));
  }

  TH1* PlotManager::hist(const std::string& name) const {
    if (m_histograms.find(name) == m_histograms.end()) {
      throw std::runtime_error("PlotManager::hist: Histogram " + name + " does not exist.");
    }

    return m_histograms[name];
  }

  void PlotManager::scaleHistograms(double sf) const {
    for (std::unordered_map<std::string, TH1*>::iterator it = begin(m_histograms); it != end(m_histograms); ++it)
      it->second->Scale(sf);
  }

  void PlotManager::checkDuplicate(const std::string& hname) const {
    if (m_histograms.find(hname) != m_histograms.end()) {
      throw std::runtime_error("PlotManager::addHist: Histogram " + hname + " to add already exists.");
    }
  }
}
