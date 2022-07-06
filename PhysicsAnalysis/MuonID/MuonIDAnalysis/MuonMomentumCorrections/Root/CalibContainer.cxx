/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// c++
#include <iostream>
#include <stdexcept>

// Local include(s):
#include <MuonMomentumCorrections/CalibContainer.h>
#include "PathResolver/PathResolver.h"

// Root
#include <TFile.h>

namespace MCP {

    CalibContainer::CalibContainer(const std::string& inFileName, const std::string& histName)
    {
        if (inFileName.empty()) throw std::invalid_argument("fileName arguments must be non empty");
        if (histName.empty()) throw std::invalid_argument("histName arguments must be non empty");
        
        auto fileName = PathResolverFindCalibFile(inFileName);

        std::unique_ptr<TFile> fmc{TFile::Open(fileName.c_str(), "READ")};
        if (!fmc || !fmc->IsOpen()) 
        {
            throw std::invalid_argument("Cannot open file " + fileName);
        }

        TH2* hist = nullptr;
        fmc->GetObject(histName.c_str(), hist);
        if (!hist) 
        {
            throw std::invalid_argument("Cannot find hist ("+histName+") in file " + fileName);
        }
        hist->SetDirectory(nullptr);
        m_calibConstantHist.reset(hist);
    }

    double CalibContainer::getCalibConstant(const TrackCalibObj& trk) const
    {
        int binEta = m_calibConstantHist->GetXaxis()->FindFixBin(trk.eta);
        int binPhi = m_calibConstantHist->GetYaxis()->FindFixBin(trk.phi);
        int gbin = m_calibConstantHist->GetBin(binEta, binPhi);

        return m_calibConstantHist->GetBinContent(gbin);
    }

}
