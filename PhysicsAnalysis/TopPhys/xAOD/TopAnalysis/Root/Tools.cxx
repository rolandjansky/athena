/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopAnalysis/Tools.h"

#include "TopAnalysis/ObjectLoaderBase.h"
#include "TopAnalysis/EventSaverBase.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"

#include "TopEventSelectionTools/ToolLoaderBase.h"
#include "xAODMetaData/FileMetaData.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <memory>

#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/LoadDictionaries.h"

#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/CorrectionCode.h"

#include "AsgTools/AsgMetadataTool.h"
#include "xAODTruth/TruthMetaData.h"
#include "xAODTruth/TruthMetaDataContainer.h"

#ifndef ROOTCORE
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

#include "TopAnalysis/MsgCategory.h"
using namespace TopAnalysis;

namespace top {
  void xAODInit(bool failOnUnchecked) {
    if (!xAOD::Init()) {
      throw std::runtime_error("Failed xAOD::Init - no idea what to do, exiting");
    }
    if (xAOD::LoadDictionaries().isFailure()) {
      ATH_MSG_WARNING("Failiure in xAOD::LoadDictionaries(). The job will "
                      "likely not run correctly.");
    }

    //fail on unchecked error codes
    if (failOnUnchecked) {
      xAOD::TReturnCode::enableFailure();
      CP::SystematicCode::enableFailure();
      CP::CorrectionCode::enableFailure();
      //StatusCode::enableFailure();
    }
  }

  bool isFilePrimaryxAOD(TFile* inputFile) {
    TTree* metaData = dynamic_cast<TTree*> (inputFile->Get("MetaData"));

    if (metaData) {
      metaData->LoadTree(0);

      TObjArray* ar = metaData->GetListOfBranches();
      for (int i = 0; i < ar->GetEntries(); ++i) {
        TBranch* b = (TBranch*) ar->At(i);
        std::string name = std::string(b->GetName());
        if (name == "StreamAOD") return true;
      }
    } else {
      throw std::runtime_error("Tools::isFilePrimaryxAOD: MetaData tree missing from input file.");
    }

    return false;
  }

  bool isFileSimulation(TFile* inputFile, const std::string& eventInfoName) {
    xAOD::TEvent xaodEvent(xAOD::TEvent::kClassAccess);
    top::check(xaodEvent.readFrom(inputFile), "Tools::isFileSimulation Failed to read file in");

    xaodEvent.getEntry(0);
    const xAOD::EventInfo* eventInfo(0);
    top::check(xaodEvent.retrieve(eventInfo, eventInfoName), "Tools::isFileSimulation Failed to get " + eventInfoName);

    return eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  }

  size_t MCweightsSize(TFile* inputFile, const std::string& eventInfoName) {
    xAOD::TEvent xaodEvent(xAOD::TEvent::kClassAccess);
    top::check(xaodEvent.readFrom(inputFile), "Tools::MCweightsSize Failed to read file in");

    xaodEvent.getEntry(0);
    const xAOD::EventInfo* eventInfo(0);
    top::check(xaodEvent.retrieve(eventInfo, eventInfoName), "Tools::isFileSimulation Failed to get " + eventInfoName);

    return eventInfo->mcEventWeights().size();
  }

  bool isTruthDxAOD(TFile* inputFile) {
    TTree* metaData = dynamic_cast<TTree*> (inputFile->Get("MetaData"));

    if (metaData) {
      metaData->LoadTree(0);

      TObjArray* ar = metaData->GetListOfBranches();
      for (int i = 0; i < ar->GetEntries(); ++i) {
        TBranch* b = (TBranch*) ar->At(i);
        std::string name = std::string(b->GetName());
        if (name.find("DAOD_TRUTH") != std::string::npos) return true;
      }
    } else {
      throw std::runtime_error("isTruthDxAOD says MetaData tree"
                               " missing from input file.\n Weird");
    }

    return false;
  }

  unsigned int getDSID(TFile* inputFile, const std::string& eventInfoName) {
    xAOD::TEvent xaodEvent(xAOD::TEvent::kClassAccess);
    top::check(xaodEvent.readFrom(inputFile), "Tools::isFileSimulation Failed to read file in");

    xaodEvent.getEntry(0);
    const xAOD::EventInfo* eventInfo(0);
    top::check(xaodEvent.retrieve(eventInfo, eventInfoName), "Tools::isFileSimulation Failed to get " + eventInfoName);

    return eventInfo->mcChannelNumber();
  }

  bool isFileFastSim(TFile* /*inputFile*/) {
    return false;
  }

  std::string getDerivationStream(TFile* inputFile) {
    TTree* metaData = dynamic_cast<TTree*> (inputFile->Get("MetaData"));

    if (metaData) {
      metaData->LoadTree(0);

      TObjArray* ar = metaData->GetListOfBranches();
      for (int i = 0; i < ar->GetEntries(); ++i) {
        TBranch* b = (TBranch*) ar->At(i);
        std::string name = std::string(b->GetName());
        // Find a variable name with Stream, then split off from DAOD
        // If we only need TOPQX, we just find "_", add 1 and substring from there instead
        if (name.find("Stream") != std::string::npos) {
          auto cursor = name.find("_");
          std::string stream = name.substr(cursor + 1);
          return stream;
        }
      }
    } else {
      throw std::runtime_error("getDerivationStream says MetaData tree"
                               " missing from input file.\n Weird");
    }

    // If we don't already return, something unexpected has happened and we need to fix it!
    throw std::runtime_error("Cannot determine the derivation stream. Please report.");
  }

  void parseCutBookkeepers(const xAOD::CutBookkeeperContainer *cutBookKeepers,
      std::vector<std::string> &names, std::vector<float>& sumW, const bool isHLLHC) {
    std::vector<int> maxCycle;
    for (const xAOD::CutBookkeeper *cbk : *cutBookKeepers) {
      // skip RDO and ESD numbers, which are nonsense; and
      // skip the derivation number, which is the one after skimming
      // we want the primary xAOD numbers
      if ((cbk->inputStream() != "StreamAOD") && !(isHLLHC && cbk->inputStream() == "StreamDAOD_TRUTH1"))
        continue;
      // only accept "AllExecutedEvents" bookkeeper (0th MC weight)
      // or "AllExecutedEvents_NonNominalMCWeight_XYZ" where XYZ is the MC weight index
      if (!((cbk->name() == "AllExecutedEvents")
            || (cbk->name().find("AllExecutedEvents_NonNominalMCWeight_") != std::string::npos)))
        continue;
      const std::string name = cbk->name();
      auto pos_name = std::find(names.begin(), names.end(), name);
      // is it a previously unencountered bookkeeper? If yes append its name to the vector of names
      // if not no need, but we must check the corresponding entry for the sum of weights exist
      if (pos_name == names.end()) {
        names.push_back(name);
        maxCycle.push_back(cbk->cycle());
        sumW.push_back(cbk->sumOfEventWeights());
      } else if (cbk->cycle() > maxCycle.at(pos_name - names.begin())) {
        maxCycle.at(pos_name - names.begin()) = cbk->cycle();
        sumW.at(pos_name - names.begin()) = cbk->sumOfEventWeights();
      } else {
        continue;
      }
    }
  }

  ULong64_t getRawEventsBookkeeper(const xAOD::CutBookkeeperContainer *cutBookKeepers, const bool isHLLHC) {
    int maxCycle = -1;
    ULong64_t rawEntries = 0;
    // search for "AllExecutedEvents" bookkeeper -- this one should always exist
    for (const xAOD::CutBookkeeper *cbk : *cutBookKeepers) {
      if ((cbk->inputStream() != "StreamAOD") && !(isHLLHC && cbk->inputStream() == "StreamDAOD_TRUTH1"))
        continue;
      if (cbk->name() != "AllExecutedEvents")
        continue;
      if (cbk->cycle() > maxCycle) {
        rawEntries = cbk->nAcceptedEvents();
        maxCycle = cbk->cycle();
      }
    }

    return rawEntries;
  }

  void renameCutBookkeepers(std::vector<std::string>& bookkeeper_names,
      const std::vector<std::string>& pmg_weight_names) {

    // prefix in the bookkeeper names to remove
    static const std::string name_prefix = "AllExecutedEvents_NonNominalMCWeight_";

    // check if we have more than one MC generator weight, in that case we have to do the renaming
    if (pmg_weight_names.size() > 1) {
      if (bookkeeper_names.size() != pmg_weight_names.size()) {
        // The number of AllExecutedEvents_ bookkeepers does not match the number of weights retrieved by PMGTool
        // we cannot match the bookkeepers to weights by indices in this case
        throw std::runtime_error("ERROR: The number of CutBookkeepers does not match the number of MC generator weights in metadata! Cannot match nominal MC weight to nominal sum of weights!");
      }

      // rename the bookkeepers based on the weight names from PMGTool
      // this names are then also written into the sumWeights TTree in output files
      for (std::string &name : bookkeeper_names) {
        if (name == "AllExecutedEvents") {
          name = pmg_weight_names.at(0);
        } else {
          // erase "AllExecutedEvents_NonNominalMCWeight_" prefix
          int index = std::stoi(name.erase(0, name_prefix.size()));
          name = pmg_weight_names.at(index);
        }
      }
    } else {
      // expect only one MC weight in this sample, hence only one AllExecutedEvents* bookeeeper
      if (bookkeeper_names.size() == 1) {
        bookkeeper_names[0] = "nominal";
      } else {
        ATH_MSG_INFO("WARNING: PMGTruthWeightTool reports no extra MC generator weight variations, "
          << "but this sample does not have exactly one AllExecutedEvents* bookkeeper!\n"
          << "Expect trouble, because this means we can't guarantee that proper CutBookkeeper "
          << "is used for the sum of weights!");
      }
    }
  }

  xAOD::TEvent::EAuxMode guessAccessMode(const std::string& filename, const std::string& electronCollectionName) {
    //there must be a better way than this
    std::unique_ptr<TFile> inputFile(TFile::Open(filename.c_str()));

    xAOD::TEvent xaodEvent(xAOD::TEvent::kBranchAccess);
    top::check(xaodEvent.readFrom(inputFile.get()), "Failed to guessAccessMode");

    //if we're unlucky and none of the events have electrons we can't
    //tell which mode we need -> use ClassAccess
    bool fallback = true;
    xAOD::TEvent::EAuxMode mode = xAOD::TEvent::kBranchAccess;

    if (electronCollectionName != "None") {
      //look at the first n events
      unsigned int entries = xaodEvent.getEntries();

      for (unsigned int i = 0; i < entries; ++i) {
        xaodEvent.getEntry(i);

        const xAOD::ElectronContainer* electrons(0);
        top::check(xaodEvent.retrieve(electrons, electronCollectionName), "Failed to guessAccessMode");

        try {
          //loading the electron pt only works for class access for the first
          //round of xAODs made for DC14 8 TeV
          //class access doesn't let us make out mini-xAOD super small though

          //branch access is available for the xAOD->xAOD reprocessing,
          //derivations and 13 TeV DC14 MC samples
          if (!electrons->empty()) {
            fallback = false;
            electrons->at(0)->pt();
            break;
          }
        } catch (...) {
          mode = xAOD::TEvent::kClassAccess;
          break;
        }
      }
    }

    if (fallback) {
      ATH_MSG_WARNING("Falling back to kClassAccess");
      mode = xAOD::TEvent::kClassAccess;
    }

    //useful message
    if (mode == xAOD::TEvent::kClassAccess)
      ATH_MSG_INFO("guessAccessMode: Using kClassAccess");
    else if (mode == xAOD::TEvent::kBranchAccess)
      ATH_MSG_INFO("guessAccessMode: Using kBranchAccess");

    return mode;
  }

  std::vector<std::string> loadCuts(const std::string& filename) {
    std::vector<std::string> v;

    std::fstream in(filename.c_str());

    if (!in.is_open()) {
      throw std::runtime_error("Problem opening " + filename);
    }

    std::string str;
    while (std::getline(in, str)) {
      std::string newstring(str);

      if (str.find("#") != std::string::npos) newstring = str.substr(0, str.find("#"));

      //leading and trailing white space removal
      while (std::isspace(*newstring.begin()))
        newstring.erase(newstring.begin());

      while (std::isspace(*newstring.rbegin()))
        newstring.erase(newstring.length() - 1);

      //make sure it contains something
      if (newstring.size() > 0) v.push_back(newstring);
    }

    return v;
  }

  size_t checkFiles(const std::vector<std::string>& filenames) {
    ATH_MSG_INFO("Input filenames:\n");
    size_t i = 0;
    size_t totalYield = 0;

    for (const auto& filename : filenames) {
      std::unique_ptr<TFile> f(TFile::Open(filename.c_str()));
      ATH_MSG_INFO(i + 1 << "/" << filenames.size() << " File: " << filename);

      if (!f.get()) {
        throw std::runtime_error("Did not manage to open " + filename);
      }

      const TTree* const t = dynamic_cast<TTree* > (f->Get("CollectionTree"));

      //some files made by the derivation framework have no events passing the
      //cuts -> which means no collection tree.
      //the old behaviour was to end execution on this problem
      //not any more!
      int entries = 0;
      std::string note;
      if (t) entries = t->GetEntries();
      else note = " (No CollectionTree)";

      ATH_MSG_INFO("Entries: " << entries << note);
      totalYield += entries;

      ++i;
    }

    return totalYield;
  }

  std::vector<std::string> fileList(const std::string& filename) {
    std::ifstream ifs(filename.c_str());

    if (!ifs) {
      throw std::runtime_error("File does not exist " + filename
          + "\nThis should contain a list - comma separated list of input files.");
    }

    //loop over the lines in the file
    std::stringstream wholefile;

    //read the file, and replace new lines with commas
    //if the line already has comma separated filenames (a la the grid)
    //these will be included automagically.
    std::string line;
    while (!ifs.eof()) {
      std::getline(ifs, line);
      wholefile << line << ',';
    }

    //somewhere to save the filenames
    std::vector<std::string> fileList;

    //now zoom through the whole file string and split at the comma
    std::string to;
    while (std::getline(wholefile, to, ',')) {
      if (to.size() > 0) fileList.push_back(to);
    }

    if (fileList.size() == 0) {
      throw std::runtime_error("Could not get a list of input files from " + filename);
    }

    return fileList;
  }

  void loadLibraries(const std::string& libraryNames) {
    std::vector<std::string> tokens;

    std::stringstream ss(libraryNames);
    std::string item;
    char delim = ' ';
    while (std::getline(ss, item, delim))
      tokens.push_back(item);

    std::vector<std::unique_ptr<top::ToolLoaderBase> > toolLoaders;
    for (const auto& toolLoaderName : tokens) {
      ATH_MSG_INFO("Attempting to load library: " << toolLoaderName << ".so");
      gSystem->Load((toolLoaderName + ".so").c_str());
    }
  }

  top::TopObjectSelection* loadObjectSelection(std::shared_ptr<top::TopConfig> config) {
    ATH_MSG_INFO("Attempting to load ObjectSelection: " << config->objectSelectionName());
    TClass* c = ::TClass::GetClass(config->objectSelectionName().c_str());

    if (c == nullptr) {
      throw std::runtime_error("Didn't manage to load " + config->objectSelectionName());
    }

    top::ObjectLoaderBase* bc = static_cast<top::ObjectLoaderBase*> (c->New());

    if (bc == nullptr) {
      throw std::runtime_error("Didn't manage to cast it to top::ObjectLoaderBase");
    }

    top::TopObjectSelection* objectSelection = bc->init(config);

    if (objectSelection == nullptr) {
      throw std::runtime_error("Didn't manage to make a top::ObjectSelection class");
    }

    return objectSelection;
  }

  top::EventSaverBase* loadEventSaver(std::shared_ptr<top::TopConfig> config) {
    ATH_MSG_INFO("Attempting to load OutputFormat: " << config->outputFormat());
    TClass* c = ::TClass::GetClass(config->outputFormat().c_str());

    if (c == nullptr) {
      throw std::runtime_error("Didn't manage to load " + config->outputFormat());
    }

    top::EventSaverBase* bc = static_cast<top::EventSaverBase*> (c->New());

    if (bc == nullptr) {
      throw std::runtime_error("Didn't manage to cast it to top::EventSaverBase ");
    }

    return bc;
  }

  bool readMetaData(TFile* inputFile, std::shared_ptr<top::TopConfig> config) {
    // Load the file into a TEvent
    xAOD::TEvent xaodEvent(xAOD::TEvent::kClassAccess);
    top::check(xaodEvent.readFrom(inputFile), "Cannot load inputFile");
    xaodEvent.getEntry(0);

    bool gotDSID=false;
    unsigned int mcChannelNumber = ((unsigned int) -1);

    std::string productionRelease="?", amiTag="?", AODFixVersion="?", AODCalibVersion="?", dataType="?", geometryVersion="?", conditionsTag="?",
                  beamType="?", simFlavour="?";
    float beamEnergy = 0, mcProcID = -1;

    // Magical metadata tool to access FileMetaData object
    asg::AsgMetadataTool ATMetaData("OurNewMetaDataObject");
    bool readFMD=false;

    // Check it exists, and if it does we will work with it
    if (ATMetaData.inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData")) {

      ATH_MSG_INFO("Trying to read FileMetaData");

      // Create pointer for FileMetaData which we will load
      const xAOD::FileMetaData* FMD = 0;
      top::check(ATMetaData.inputMetaStore()->retrieve(FMD,
                                                       "FileMetaData"),
                 "Failed to retrieve metadata from AsgMetadataTool");
      // Let's get all the info we can...
      // https://gitlab.cern.ch/atlas/athena/blob/21.2/Event/xAOD/xAODMetaData/xAODMetaData/versions/FileMetaData_v1.h#L47
      /// Release that was used to make the file [string]
      FMD->value(xAOD::FileMetaData::productionRelease, productionRelease);
      /// AMI tag used to process the file the last time [string]
      FMD->value(xAOD::FileMetaData::amiTag, amiTag);
      /// Version of AODFix that was used on the file last [string]
      FMD->value(xAOD::FileMetaData::AODFixVersion, AODFixVersion);
      /// Version of AODCalib that was used on the file last [string]
      FMD->value(xAOD::FileMetaData::AODCalibVersion, AODCalibVersion);
      /// Data type that's in the file [string]
      FMD->value(xAOD::FileMetaData::dataType, dataType);
      /// Geometry version [string]
      FMD->value(xAOD::FileMetaData::geometryVersion, geometryVersion);
      /// Conditions version used for simulation/reconstruction [string]
      FMD->value(xAOD::FileMetaData::conditionsTag, conditionsTag);
      /// Beam energy [float]
      FMD->value(xAOD::FileMetaData::beamEnergy, beamEnergy);
      /// Beam type [string]
      FMD->value(xAOD::FileMetaData::beamType, beamType);
      /// Same as mc_channel_number [float]
      gotDSID = FMD->value(xAOD::FileMetaData::mcProcID, mcProcID);
      mcChannelNumber = mcProcID;
      /// Fast or Full sim [string]
      FMD->value(xAOD::FileMetaData::simFlavour, simFlavour);
      /// It is also possible to access any other info in metadata with
      /// FMD->value("SomeMetaInfo", someObject);
      readFMD=true;
    }
    if (!readFMD || !gotDSID || mcChannelNumber == ((unsigned int) -1)){  // in case FileMetaData is bugged and does not have DSID properly stored happens for example for files with 0 events in CollectionTree after skimming

      ATH_MSG_INFO("FileMetaData not found or not readable, trying to read TruthMetaData");

      bool gotTruthMetaData = true;
      const xAOD::TruthMetaDataContainer *truthMetadata =  nullptr;
      if (ATMetaData.inputMetaStore()->contains<xAOD::TruthMetaDataContainer>("TruthMetaData")) {
        if (ATMetaData.inputMetaStore()->retrieve(truthMetadata, "TruthMetaData")) {
          if (truthMetadata->size() == 1) {
            mcChannelNumber = truthMetadata->at(0)->mcChannelNumber();
          } else {
            ATH_MSG_WARNING("TruthMetaData does not have exactly one entry. Cannot reliably determine DSID");
          }
        } else {
          gotTruthMetaData = false;
        }
      } else {
        gotTruthMetaData = false;
      }
      if (!gotTruthMetaData)
      {
        ATH_MSG_ERROR("We cannot retrieve even TruthMetaData to determine DSID");
        return false;
      }
    }

    /// Print out this information as a cross-check
    ATH_MSG_INFO("Using AsgMetadataTool to access the following information\n"
        << "productionRelease  -> " << productionRelease << "\n"
        << "amiTag             -> " << amiTag << "\n"
        << "AODFixVersion      -> " << AODFixVersion << "\n"
        << "AODCalibVersion    -> " << AODCalibVersion << "\n"
        << "dataType           -> " << dataType << "\n"
        << "geometryVersion    -> " << geometryVersion << "\n"
        << "conditionsTag      -> " << conditionsTag << "\n"
        << "beamEnergy         -> " << beamEnergy << "\n"
        << "beamType           -> " << beamType << "\n"
        << "mcProcID           -> " << int(mcChannelNumber) << "\n"
        << "simFlavour         -> " << simFlavour << "\n"
        << "Not all of this information is propagated to TopConfig");

    config->setAmiTag(amiTag);
    config->setDSID(mcChannelNumber);

    return true;
  }
}
