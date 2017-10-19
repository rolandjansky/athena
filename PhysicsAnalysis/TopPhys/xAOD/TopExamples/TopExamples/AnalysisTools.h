/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOOLS_H
#define ANALYSISTOOLS_H

#include <string>
#include <memory>

#include "TopExamples/AnalysisBase.h"
#include "TopExamples/ContainerNames.h"

class TH1D;
class TFile;

namespace EL {
  class Worker;
}

/**
 * @file TopExamples/AnalysisTools.h
 *
 * @brief A few functions for analysis code in the final stages.
 */

namespace top {

///Sometimes you want things in GeV, no.  Do by multiplication.
const double toGeV = 0.001;

/**
 * @brief Load one of the three analyses in a factory like way.
 *
 * Allows you to specify a string name which is mapped to the analysis code
 * using this function.  Note that if you add some new analysis code and want
 * to actually run it then you also need to modify this function in a hopefully
 * trivial way.
 *
 * @param name The name of the analysis to load (Dilepton, LJets, NoSelection etc)
 *
 * @return a unique ptr (these things are magic, they delete themselves when they
 * are no longer needed) to the analysis code.
 */
std::unique_ptr<top::AnalysisBase> loadAnalysis(const std::string& name, TFile* outputFile, EL::Worker* wk=nullptr);

/**
 * @brief Open an xAOD and return the sample number as a string.
 *
 * For data this is the run number but for MC simulation this returns the
 * MC channel number.  Not that for data we also pad it to 8 characters long
 * with a zero or two.  Because this is how the grid datasets are named to. For
 * MC simulation datasets are not padded out to 8 characters long.
 *
 * @param filename Name of an xAOD file to extract the information from.
 * @param containerNames Name of the containers to load.  In theory only the
 * EventInfo is important, but this seemed easier for now.
 *
 * @return run number for data, MC channel number for MC simulation.
 */
const std::string getSampleNumberAsString(const std::string& filename, const std::string& infoContainerName);

/**
 * @brief Calculate a scale factor to one inverse fb given the sum of weights
 * and the mc channel number.
 *
 * This loads the TopDataPreparation code and text file to get the cross-section
 * for the MC sample.
 *
 * @param mcChannelNumber The MC channel number, so we can look up the cross-
 * section using TopDataPreparation
 * @param mcWeightsInSample The sum of the MC weights in the MC sample
 */
double sfToOneInversefb(unsigned int mcChannelNumber, double mcWeightsInSample);

/**
 * @brief We want to add a histogram with name "name" from file "inputFile" to
 * the histogram "histogram".  Note that if "histogram" is nullptr this function
 * will create it using the first one it finds in a file.  Otherwise it just adds
 * the new histogram to this one.
 *
 * @param name The name of the cutflow histogram in the input file (e.g.
 * ee/cutflow).
 * @param histogram The histogram in our code that we want to add to. This is
 * modified by this function!
 * @param inputFile The inputFile to read the histogram from.
 */
void updateCutflow(const std::string& name, TH1D*& histogram, TFile* inputFile);

/**
 * @brief Messy.
 *
 * Given a path /this/is/a/path/user.simon.117050.e1727_s1933_s1911_r5591.MC_31_07_1/input.txt.
 * Strip off the input.txt and all the stuff before the very last directory.
 * Then remove user.simon.117050 (i.e. the first three items between dots).
 * Then strip off the end bit to leave e1727_s1933_s1911_r5591.
 *
 * @param inputFile A path with a specific naming convention for the last directory.
 * @return The e<blah>_s<blah>_r<blah> or equivalent.
 */
std::string gridTagFromDirectoryName(const std::string inputFile);

/**
 * @brief The top-xaod code is decorating events with a integer (not bool!)
 * flag for every event that passes/fails the pre-selection.  The name of the
 * decoration is the analysis channel (e.g. ee, mumu, emu).  So we can check
 * if the event passes or fails the selection done on the grid by looking at
 * this.
 *
 * @param topEvent The top event we want to know about.
 * @param name The channel we're interested in. e.g. ee
 * @return True if the event passed the grid selection, false otherwise.
 */
bool passesPreSelection(const top::Event& topEvent, const std::string& name);

}

#endif
