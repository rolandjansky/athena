/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPTOOLS_H__
#define TOPTOOLS_H__

#include <vector>
#include <string>

#include "xAODRootAccess/TEvent.h"

#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

#include "TopConfiguration/TopConfig.h"

/**
 * @file Tools.h
 *
 * Some tools to make life in the xAOD world a bit easier.  Including ways
 * to initialise, guess the best "mode" to open an xAOD with, load cuts, list
 * files, check yields of files, and load some "tools".
 */

namespace top {
  class TopObjectSelection;
  class EventSaverBase;

/**
 * @brief A little wrapper for the xAOD tools.  Also turns enabled the failure
 * on unchecked status codes.
 *
 * @param failOnUnchecked Terminate the job on unchecked status codes by
 * default.
 */
  void xAODInit(bool failOnUnchecked = true);

/**
 * @brief Function to determine the derivation type using string manipulation
 *
 * @param inputFile A pointer to the input file
 *
 * @return string with the derivation stream name
 */
  std::string getDerivationStream(TFile* inputFile);

/**
 * @brief Search bookkeepers for ones matching AllExecutedEvents, and which
 * originate from AOD before skimming.
 *
 * @param TEvent to retrieve the cutbookkeepers
 * @param number of the generator weights
 * @param names Vector of names of the selected bookkeepers
 * @param sumW Vector of sum of weights of the selected bookkeepers
 * @param isHLLHC Whether this is a HLLHC ugprade sample, it uses different stream for bookkeepers
 */
  void parseCutBookkeepers(xAOD::TEvent& event, const std::size_t size,
      std::vector<std::string> &names, std::vector<float>& sumW, const bool isHLLHC=false);

/**
 * @brief Get raw number of events before skimming from "AllExecutedEvents" bookkeeper
 * 
 * @param cutBookKeepers Container with CutBookkeepers
 * @param isHLLHC Whether this is a HLLHC ugprade sample, it uses different stream for bookkeepers
 *
 * @return number of raw events before skimming
 */
  ULong64_t getRawEventsBookkeeper(const xAOD::CutBookkeeperContainer *cutBookKeepers,
      const bool isHLLHC=false);

/**
 * @brief Rename CutBookkeeper names according to MC generator weight names reported by PMGTruthWeightTool
 *
 * @param bookkeeper_names Vector of original AllExecutedEvents* bookkeeper names
 * @param pmg_weight_names Vector with MC generator weight names from PMGTruthWeightTool
 */
  void renameCutBookkeepers(std::vector<std::string>& bookkepeer_names,
      const std::vector<std::string>& pmg_weight_names);

/**
 * @brief Load the file and make a vector of the cuts to pass on to the
 * selection tool.
 *
 * @param filename The text file to load and read the cut information from.  The
 * idea is that the user will supply this at the command line.
 *
 * @return a vector with one cut-tool and configuration per line.
 */
  std::vector<std::string> loadCuts(const std::string& filename);

/**
 * @brief Given a filename for a text file, parse it and extract a list of root
 * files.
 *
 * This should open the text file and split it into a vector of strings.  It
 * should handle filenames that are separated by a newline or a comma.
 *
 * @param filename The text file that we want to parse.
 * @return A vector of filenames.
 */
  std::vector<std::string> fileList(const std::string& filename);

/**
 * @brief Open each file in the vector and get the number of events in it.
 *
 * Useful to make sure all the files are readable before jumping into an event
 * loop.  Also returns the total yield for all files.  If the file can't be
 * opened or the CollectionTree is missing then it will print an error message
 * and exit.
 *
 * @param filenames A vector of filenames to check.
 * @return The total yield for all files.
 */
  size_t checkFiles(const std::vector<std::string>& filenames);

/**
 * @brief So that we can load external libraries with (1) extra event selection
 * tools in, (2) user defined object selection and (3) user defined output file
 * formats.  For all those things the user supplies the class name of the class
 * to load.  But we also need the library that they're in so that root can load
 * it.
 *
 * @param libraryNames The name of the library, for exmample
 * libTopEventSelectionTools
 */
  void loadLibraries(const std::string& libraryNames);

/**
 * @brief Users may want to define (and load!) their own object selection. Once
 * the library is loaded we also need to know the class that they want to use.
 * Root then loads that class and attempts to convert it to a
 * top::TopObjectSelection pointer.  Only if that works can the program
 * continue.  If it doesn't work, the program should end (not much point running
 * if you didn't manage to configure the objects to run on, is there?)
 *
 * @param config The object selection setup needs access to the config object, which
 * should know the name of the object selection to use
 *
 * @return The object selection pointer.
 */
  top::TopObjectSelection* loadObjectSelection(std::shared_ptr<top::TopConfig> config);

/**
 * @brief Users may also want to write out custom ntuples / xAODs.  This too
 * can be loaded from the name of the class supplied in the configuration file.
 *
 * @param outputFormat Name of the class to load for the output format.  For
 * example top::EventSaverxAOD, or top::EventSaverFlatNtuple.  Note that user
 * defined ones are also allowed. Due to popular demand.
 *
 * @return If everything works, then a pointer to the event saver object.
 * Otherwise the program should end and tell people why.
 */
  top::EventSaverBase* loadEventSaver(std::shared_ptr<top::TopConfig> config);

/**
 * @brief This function will be used to load the metadata object and pull information
 * from it. We will then need to look at storing the info in TopConfig
 *
 * @param inputFile Include a single TFile which we load and read
 * @param config Pass in the config, before it is fixed, so that we can add info
 *
 * @return If successful, return true, else return false
 */

  bool readMetaData(TFile* inputFile, std::shared_ptr<top::TopConfig> config);
}

#endif
