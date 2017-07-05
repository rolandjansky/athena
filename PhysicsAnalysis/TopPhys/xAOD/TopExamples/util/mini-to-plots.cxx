/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <memory>
#include <cstdlib>
#include <sstream>

#include "TFile.h"

#include "xAODRootAccess/TEvent.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"

#include "TopAnalysis/Tools.h"

#include "TopExamples/EventLoaderDirect.h"
#include "TopExamples/AnalysisTools.h"

/**
 * @file mini-to-plots.cxx
 *
 * @brief Executable for the 'local' part of the analysis.
 *
 */

/**
 * @brief The main executable
 *
 * Does the xAOD stuff, then makes an output file (root) based on the name of
 * the input file directory (hopefully useful for grid running).  Then goes on
 * to loop over all the files in that directory and make some plots / do some
 * analysis.
 *
 * The output plots are normalised to 1fb-1.
 */
int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Use like:\n";
        std::cout << argv[0] << " Dilepton /path/with/user.simon.117050.e1727_s1933_s1911_r5591.MC_31_07_1/input.txt\n";
        std::cout << "\n";
        std::cout << "note that the directory has user.simon.117050.GRIDTAG.stuff/input.txt\n";
        std::cout << "i.e. something.something.something.GRIDTAG.something\n";
        exit(1);
    }

    //magic xAOD stuff
    top::xAODInit();

    //attempt to extract the tag from the directory that the input files lives in
    std::vector<std::string> filenames = top::fileList(argv[2]);

    //attempt to extract the tag from the directory name
    const std::string gridTag = top::gridTagFromDirectoryName(argv[2]);

    //get the sample number (run or mc-sample number depending on data / MC sim)
    //todo should this be in a function instead?
    //padded to 8 char for data, not padded for MC to match dataset naming convention
    std::string sampleNumberString;
    size_t firstIdx = 0;
    while (firstIdx < filenames.size()) {
        try {
            sampleNumberString = top::getSampleNumberAsString(filenames[firstIdx], "EventInfo");
            break;
        } catch(std::string &e) { // can fail if no events passed selection in a specific job
            std::cout << "Error getting MC sample number: " << e.c_str() << std::endl;
            ++firstIdx;
        }
    }
    if (firstIdx >= filenames.size()) {
        std::cout << "No events passed in any of the jobs. No entries available. Abort!" << std::endl;
        exit(1);
    }

    //generate the output filename
    //pad the run number to 8 digits
    std::stringstream s;
    s << sampleNumberString << "." << gridTag << ".root";
    std::cout << "Writing plots to: " << s.str() << "\n";

    //output file
    std::unique_ptr<TFile> plotsFile(TFile::Open(s.str().c_str(), "RECREATE"));

    //user analysis code to make histograms
    std::unique_ptr<top::AnalysisBase> analy(top::loadAnalysis(argv[1], plotsFile.get()));
    const unsigned int totalYield = top::checkFiles(filenames);

    //event loop
    xAOD::TEvent xaodEvent(xAOD::TEvent::kBranchAccess); //don't use kClassAccess on non-primary xAOD, obviously.

    top::EventLoaderDirect loader(analy->containerNames());

    //loop over input files
    unsigned int totalYieldSoFar = 0;
    unsigned int fileCounter = 0;
    for (const auto& filename : filenames) {
        std::cout << "Opening " << fileCounter << " of " << filenames.size() << ": " << filename << std::endl;
        std::unique_ptr<TFile> inputFile(TFile::Open(filename.c_str()));
        top::check(xaodEvent.readFrom(inputFile.get()), "xAOD::TEvent readFrom failed");

        //update any cutflow / meta stuff we might need to get per-file
        analy->newFile(inputFile.get());

        //loop over events in the file
        const unsigned int entries = xaodEvent.getEntries();
        for (unsigned int entry = 0; entry < entries; ++entry, ++totalYieldSoFar) {
            if (entry % 100 == 0)
                std::cout << "    Processing event " << totalYieldSoFar << " / " << totalYield << " (this file: " << entry << " / " << entries << ")" << std::endl;

            //read the event and do stuff
            xaodEvent.getEntry(entry);
            top::Event topEvent = loader.loadEvent(xaodEvent);
            analy->event(topEvent);
        }

        ++fileCounter;
    }

    //save output histograms
    analy->finalise(plotsFile.get());


    return 0;
}
