/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopExamples/AnalysisTools.h"
#include "TopExamples/NoSelectionAnalysis.h"
#include "TopExamples/LJetsAnalysis.h"
#include "TopExamples/LJetsTtresAnalysis.h"
#include "TopExamples/DileptonAnalysis.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

#include "TopDataPreparation/SampleXsection.h"

#include "xAODRootAccess/TEvent.h"

#include "xAODEventInfo/EventInfo.h"

#include "TH1D.h"
#include "TFile.h"

#include <iostream>
#include <memory>

namespace top {

std::unique_ptr<top::AnalysisBase> loadAnalysis(const std::string& name, TFile* outputFile, EL::Worker* wk) {
    if (name == "Dilepton")
        return std::unique_ptr<top::AnalysisBase>(new top::DileptonAnalysis(outputFile, wk));
    else if (name == "LJets")
        return std::unique_ptr<top::AnalysisBase>(new top::LJetsAnalysis(outputFile, wk));
    else if (name == "NoSelection")
        return std::unique_ptr<top::AnalysisBase>(new top::NoSelectionAnalysis(outputFile, wk));
    else if (name == "LJetsTtres")
        return std::unique_ptr<top::AnalysisBase>(new top::LJetsTtresAnalysis(outputFile));
    else {
        std::cout << "I only know about:\n";
        std::cout << "NoSelection\n";
        std::cout << "LJets\n";
        std::cout << "Dilepton\n";
        std::cout << "LJetsTtres\n";
        exit(1);
    }

    //make sure this never happens with the if statement above
    return nullptr;
}

const std::string getSampleNumberAsString(const std::string& filename, const std::string& infoContainerName) {
    xAOD::TEvent xaodEvent(xAOD::TEvent::kBranchAccess);
    std::unique_ptr<TFile> fileForSampleNumber(TFile::Open(filename.c_str()));
    top::check(xaodEvent.readFrom(fileForSampleNumber.get()), "xAOD::TEvent readFrom failed");
    xaodEvent.getEntry(0);

    const xAOD::EventInfo* info(0);
    if (!xaodEvent.retrieve(info, infoContainerName))
        throw std::string("xAOD::TEvent couldn't get hold of ") + infoContainerName;

    //data is padded with zero until it's 8 char long in ATLAS
    std::stringstream ss;
    ss << std::setw(8) << std::setfill('0') << info->runNumber();

    //but MC simulation is not padded in dataset names :)
    if (info->eventType(xAOD::EventInfo::IS_SIMULATION)) {
        ss.str("");
        ss << info->mcChannelNumber();
    }

    return ss.str();
}

double sfToOneInversefb(unsigned int mcChannelNumber, double mcWeightsInSample) {
    //load the x-section from a file
    SampleXsection tdp;
//     const char* const rc = getenv("ROOTCOREBIN");
//     std::string filename = std::string(rc) + "/data/TopDataPreparation/XSection-MC15-13TeV-fromSusyGrp.data";
    std::string filename = PathResolverFindCalibFile("TopDataPreparation/XSection-MC15-13TeV-fromSusyGrp.data");
    if (!tdp.readFromFile(filename.c_str())) {
        std::cout << "TopDataPreparation - could not read file\n";
        std::cout << filename << "\n";
        std::cout << "Maybe check it exists, first?" << std::endl;
        exit(1);
    }

    //calculate
    double xsection = tdp.getXsection(mcChannelNumber);
    if (xsection < 0) { // try exotics samples
      //      std::string filenameExot = std::string(rc) + "/data/TopDataPreparation/XSection-MC12-8TeV-4gt.data";
      //if (!tdp.readFromFile(filenameExot.c_str())) {
      // do not exit since it is an Exotic sample and the file could not be there
      //}
      // xsection = tdp.getXsection(mcChannelNumber);
    }
    const double integratedluminosity = mcWeightsInSample / xsection;
    const double sf = 1000. / integratedluminosity;

    //useful information
    std::cout << "mcChannelNumber           : " << mcChannelNumber << "\n";
    std::cout << "X-section                 : " << xsection << " pb\n";
    std::cout << "Sum weights before cuts   : " << mcWeightsInSample << "\n";
    std::cout << "Integrated Lumi for sample: " << integratedluminosity << " pb-1\n";
    std::cout << "Scale factor to 1 fb-1    : " << sf << "\n";

    return sf;
}

void updateCutflow(const std::string& name, TH1D*& histogram, TFile* inputFile) {
    const auto* const temp = dynamic_cast<TH1D*> (inputFile->Get(name.c_str()));
    if (histogram == nullptr) {
        histogram = dynamic_cast<TH1D*> (temp->Clone());
        histogram->SetDirectory(0);
    } else
        histogram->Add(temp);
}

std::string gridTagFromDirectoryName(const std::string inputFile) {
    std::string gridTag = inputFile;

    try {
        gridTag.erase(gridTag.rfind("/"));
        gridTag = gridTag.substr(gridTag.rfind("/") + 1);

        for (int i = 0; i < 3; ++i)
            gridTag = gridTag.erase(0, gridTag.find(".") + 1);

        gridTag = gridTag.erase(gridTag.rfind("."));
    } catch (...) {
        gridTag = "unkown";
    }

    return gridTag;
}

bool passesPreSelection(const top::Event& topEvent, const std::string& name) {
    return topEvent.m_info->auxdata<int>(name);
}

}
