/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPTOOLS_H__
#define TOPTOOLS_H__

#include <vector>
#include <string>

#include "xAODRootAccess/TEvent.h"

#include "TopConfiguration/TopConfig.h"

/**
 * @file TopAnalysis/Tools.h
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
 * @brief Sometimes we need to know if we're running on a primary xAOD or a
 * derivation.
 *
 * To figure this out (while we wait for a proper solution) we can open up
 * the MetaData tree.  As far as I can tell primary xAODs have a branch called
 * StreamAOD and derivations have something a bit like StreamDAOD_TOPQ1.
 * So if we see StreamAOD, we know it's a primary xAOD. I hope.
 *
 * @param inputFile A pointer to the input file
 * @return True if this is a primary xAOD
 */
bool isFilePrimaryxAOD(TFile* inputFile);

/**
 * @brief Some of our things need to know if the events are MC or data before
 * we're in the event loop.  In the future this should be in the file metadata
 * but right now we can't access it easily in RootCore.
 *
 * @param inputFile A pointer to the input file
 * @param eventInfoName Name of the EventInfo container in the xAOD
 * @return True if MC simulation, false for data
 */
bool isFileSimulation(TFile* inputFile, const std::string& eventInfoName);

bool isTruthDxAOD(TFile* inputFile);

unsigned int getDSID(TFile* inputFile, const std::string& eventInfoName);

/**                                                                                                                                                                                                           
 * @brief Some of our things need to know if the events are AFII or FullSim
 * for certain systematics.                                                                        
 *                                                                                                                                   
 * @param inputFile A pointer to the input file                                                                                                                                                              
 * @param eventInfoName Name of the EventInfo container in the xAOD                                                                                                                                          
 * @return True if fast simulation, false for FullSim                                                                                                                                                         
 */
bool isFileFastSim(TFile* inputFile);

/**
 * @brief Function to determine the derivation type using string manipulation
 *
 * @param inputFile A pointer to the input file
 *
 * @return string with the derivation stream name
 */

std::string getDerivationStream(TFile* inputFile);


/**
 * @brief xAODs can be accessed in class or branch access mode. Guess 'the best'
 * by trying branch access first and falling back to class access if that fails.
 *
 * Try to guess the 'best' mode to open the files with.  The way we do this is
 * first try with branch access (this is what is preferred because it'll allow
 * us to make our mini xAODs smaller by kicking-out branches).  The problem is
 * the first xAODs for 8 TeV DC14 can't be read in this mode (actually only the
 * egamma objects).  This is fixed for 13 TeV, and will be for the 8 TeV
 * derivations and the xAOD->xAOD reprocessing.
 *
 * This function opens the file, loads the first event and attempts to read the
 * electron collection.  If that works in branch mode it returns branch  mode.
 * If it fails, it returns class mode.
 *
 * @param filename A file that you're going to run on, so we can attempt to
 * figure out which mode to use.
 *
 * @param electronCollectionName Name of the electron collection - no really!
 *
 * @return BranchAccess (preferred) or ClassAccess if the test fails.
 */
xAOD::TEvent::EAuxMode guessAccessMode(const std::string& filename, const std::string& electronCollectionName);

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
unsigned int checkFiles(const std::vector<std::string>& filenames);

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

}

#endif
