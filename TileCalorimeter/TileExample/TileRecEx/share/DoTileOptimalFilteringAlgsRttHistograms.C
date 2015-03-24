// This ROOT MACRO produces histograms representing distribution of output
// quantities (energy, time, pedestal, chi2) from Optimal Filtering Algorithms
// (TileOptATLAS, TileOpt2, TileFit) in the case of existence of corresponding
// branches in tree "h2000". The histograms are intended to be used
// for testing these algorithms in RTT framework.
// Following types of histograms are produced for every tile partition
// (LBA, LBC, EBA, EBC):
//   1. Two dimensional histograms representing average quantities
//      (energy, time, pedestal, chi2) distribution vis channel and module;
//   2. One dimensional histograms representing average quantities
//      (energy, time, pedestal, chi2) distribution vis module;
//   3. One dimensional histograms representing average quantities distribution
//      (energy, time, pedestal, chi2) distribution vis channel;
// There is possibility to use only channels with energy more than given energy
// threshold for producing these histograms.
//
// Usage example:
//  root -b -q 'DoTileOptimalFilteringAlgsRttHistograms.C(
//           [--outputFile] [--lowGainEThr] [--highGainEThr] [--uniGainEThr]
//           [--badModules] [--useBadChannelStatus]
//           [--usePedestalToMaskChannel]'
//
// Options:
//  --outputFile=FILE
//      Output file name with result histograms.
//  --lowGainEThr=ENERGY
//      Number of float type  representing energy threshold for low gain (i.e.
//      only channels with energy more than energy threshold will be processed).
//  --highGainEThr=ENERGY
//      Number of float type  representing energy threshold for high gain (i.e.
//      only channels with energy more than energy threshold will be processed).
//  --uniGainEThr=ENERGY
//      Number of float type  representing energy threshold for uni gain (i.e.
//      only channels with energy more than energy threshold will be processed).
//  --badModules=MODULES
//      Bad modules separated by comma (e.g. --badModules=LBC17,EBA20);
//  --useBadChannelStatus=(True|False) default: True
//  --usePedestalToMaskChannel=(True|False) default: False
//      Mask channels which have pedestal > 5000 (have mask info in pedestal)

// Input file name is determined automatically by pattern in order to allow
// names from different run number. All histograms are saved in output file
// in directories named after the Optimal Filtering Algorithm name and partition
// name. Input file name pattern and default output file name are determined
// by macros further in this file.

#define INPUT_FILE_NAME_PATTERN "^tile_[0-9]*_RTT.aan.root$"
#define OUTPUT_FILE_NAME "TileOptimalFilteringAlgsRttHistograms.root"
// Log level of message to be logged: DEBUG = 0, INFO = 1, ERROR = 2.
#define LOG_LEVEL 1
#define ROOT_MACRO_NAME "DoTileOptimalFilteringAlgRttHistograms.C"
#define BAD_SIGNAL_CHI2 100000

#include <stdlib.h>
#include <vector>
#include <math.h>
#include <algorithm>

#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile2D.h"
#include "TF1.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TString.h"
#include "TRegexp.h"

//  Output quantities (energy, time, pedestal, chi2) from Optimal Filtering
//  Algorithms are stored in output ntuples tree "h2000" as arrays
//  [i][j][k] of type Float_t, where:
//     i - partition number from 0 to 3 in order: LBA, LBC, EBA, EBC;
//     j - module number from 0 to 63;
//     k - channel number from 0 to 47.
typedef Float_t Quantity_t[4][64][48];
// For MF
typedef Float_t QuantityMF_t[4][64][48][7];

// Type of exit result of this ROOT Macro.
enum ExitResult {
  FAILLURE = -1, SUCCESS = 0
};

// Level or severity of log messages.
enum LogLevel {
  DEBUG = 0, INFO = 1, ERROR = 2
};

// Type of run.
enum RunType {
  PHY = 1, LAS = 2, PED = 4, MONOCIS = 8, CIS = 16
};

enum EnergyUnits {
  ADCCOUNTS = 0, PC = 1, MEV = 3
};

enum GainCode {
  LOW = 0, HIGH = 1, UNI = 2
};

enum ChannelStatus {
  GOOD = 0, BAD = 5
};

enum Partition {
  LBA = 0, LBC = 1, EBA = 2, EBC = 3
};

//*****************************************************************************
//                     Description of class "SimpleLogger"
//*****************************************************************************

// Simple logger class that log only message with severity level more or equal
// than effective log level associated with logger (defined by macro
// LOG_LEVEL).This class may be instantiated only by method GetLogger().
// Logger logs message in standard output stream.
class SimpleLogger {
  public:
    // Only one legal method to get instance of this class.
    static SimpleLogger* GetLogger(void);
    // Logs INFO message.
    void Info(TString message);
    // Logs  ERROR message.
    void Error(TString message);
    // Logs DEBUG message.
    void Debug(TString message);
    // Returns effective log level associated with logger.
    // Logger does not care about messages with severity level less than
    // this level.
    LogLevel GetLogLevel(void);
    // Sets effective log level associated with logger.
    // Logger does not care about messages with severity level less than
    // this level.
    void SetLogLevel(LogLevel level);
  protected:
    // Default constructor is made protected to allow instantiation
    // only one logger by method GetLogger.
    SimpleLogger(): logLevel_(INFO) { };

    // Copy constructor is made protected to allow instantiation
    // only one logger by method GetLogger.
    // Copy constructor  is not defined intentionally.
    SimpleLogger(const SimpleLogger&);
    // Assignment operator is made protected to allow instantiation
    // only one logger by method GetLogger.
    // Assignment operator is not defined intentionally.
    SimpleLogger& operator=(SimpleLogger const&);
    // Logs only messages with severity level more or equal
    // than effective log level associated with logger (defined by macro
    // LOG_LEVEL)
    void Log(LogLevel messageSeverity, TString message);
    // Returns string representation of message type (severity).
    TString getMessageType(LogLevel level);

  private:
    static SimpleLogger* instance_;
    // Log level associated with logger. Logger does not care about messages
    // with severity level less than this level.
    LogLevel logLevel_;
};

//*****************************************************************************
//                     Description of class "TileOfaDescription"
//*****************************************************************************

// Class to hold description of particular Optimal Filtering Algorithm (OFA)
// and present such characteristic of OFA for particular gain as name of
// algorithm, name of gain, name of energy branch in tree "h2000", name of
// time branch in tree "h2000", name of pedestal branch in tree "h2000",
// and name of chi2 branch in tree "h2000". By default object of this
// class is constructed for uni gain. Objects describing Optimal Filtering
// Algorithm for low and high gains may be get by following methods
// "GetForLowGain" and "GetForHighGain" from initially constructed object
// for uni gain.
class TileOfaDescription {
  public:
    // Constructs object describing characteristics of particular Optimal
    // Filtering  Algorithm. Used input parameters:
    // name of Optimal Filtering Algorithm; names of branches corresponding
    // to output quantity (energy, time, pedestal,chi2) from the algorithm
    //in input file in tree "h2000" for this algorithm.
    TileOfaDescription(TString algorithmName, TString energyBranchName,
        TString timeBranchName, TString pedestalBranchName,
        TString chi2BranchName);
    TileOfaDescription(const TileOfaDescription& description);
    TString GetAlgorithmName(void);
    TString GetEnergyBranchName(void);
    TString GetTimeBranchName(void);
    TString GetPedestalBranchName(void);
    TString GetChi2BranchName(void);
    TString GetGainName(void);
    GainCode GetGainCode(void);
    // Gets different object of type TileOfaDicription which describes
    // the same Tile Optimal Filtering Algorithm as initial object but
    // for uni gain.
    TileOfaDescription GetForUniGain(void);
    // Gets different object of type TileOfaDicription which describes
    // the same Tile Optimal Filtering Algorithm as initial object but
    // for high gain.
    TileOfaDescription GetForHighGain(void);
    // Gets different object of type TileOfaDicription which describes
    // the same Tile Optimal Filtering Algorithm as initial object but
    // for low gain.
    TileOfaDescription GetForLowGain(void);
  private:
    TString gainName_;
    GainCode gainCode_;
    TString gainAwareSuffix_;
    TString algorithmName_;
    TString energyBranchName_;
    TString timeBranchName_;
    TString pedestalBranchName_;
    TString chi2BranchName_;
};

//*****************************************************************************
//                     Description of class "TileOfaInterface"
//*****************************************************************************

class TileOfaInterface {
  public:
    virtual ~TileOfaInterface() {
    }
    ;
    // Set up energy threshold if particular object has been created
    // for uni gain and nothing will be done in the other case.
    virtual void SetEnergyUniGainThreshold(Float_t energyUniGainThreshold) = 0;
    // Set up energy threshold if particular object has been created
    // for low gain and nothing will be done in the other case.
    virtual void SetEnergyLowGainThreshold(Float_t energyLowGainThreshold) = 0;
    // Set up energy threshold if particular object has been created
    // for high gain and nothing will be done in the other case.
    virtual void SetEnergyHighGainThreshold(Float_t energyHighGainThreshold) = 0;
    // Set up to use or don't channel bad status from branch bad of 
    // h200_map tree.
    virtual void UseBadChannelStatus(Bool_t useChStatus) = 0;
    virtual void UsePedestalToMaskChannel(Bool_t usePedToMaskCh) = 0;
    virtual void SetBadModules(TObjArray* modules) = 0;
    // Makes and fills all RTT histograms from input root file with tree
    // "h2000" for particular combination of Optimal Filtering Algorithm
    // and gain.
    virtual void MakeAndFillRttHistogramsFromFile(TFile* inputFile) = 0;
    // Saves all produced histograms for particular combination of Optimal
    // Filtering Algorithm and gain in output file in directory named after
    // name of Optimal Filtering Algorithm.
    virtual void SaveRttHistogramsToFile(TFile* outputFile) = 0;
};

//*****************************************************************************
//                     Description of class "TileOFA"
//*****************************************************************************

// Class for producing histograms representing distribution of output
// quantities (energy, time, pedestal, chi2)  from particular Optimal Filtering
// Algorithm in the case of existence of corresponding branches in input file
// and saving them in output file. Following types of histograms are produced:
//   1. Two dimensional histograms representing average quantities distribution
//       vis channel and module;
//   2. One dimensional histograms representing average quantities distribution
//       vis channel;
//   3. One dimensional histograms representing average quantities distribution
//       vis module.
// If set up energy threshold for particular gain only channels with energy
// more than energy threshold will be used for producing these histograms.
// Tree "h2000" in input ntuple is used.
class TileOFA: public TileOfaInterface {
  public:
    // Constructs object to produce histograms for particular Optimal
    // Filtering  Algorithm. Used input parameters: object of type
    // "TileOfaDescription" which describes characteristics particular
    // Optimal Filtering Algorithm for particular gain.
    TileOFA(TileOfaDescription tileOfaDescr);
    virtual ~TileOFA();
    virtual void SetEnergyUniGainThreshold(Float_t energyUniGainThreshold);
    virtual void SetEnergyLowGainThreshold(Float_t energyLowGainThreshold);
    virtual void SetEnergyHighGainThreshold(Float_t energyHighGainThreshold);
    virtual void UseBadChannelStatus(Bool_t useChStatus);
    virtual void UsePedestalToMaskChannel(Bool_t usePedToMaskCh);
    virtual void SetBadModules(TObjArray* modules);
    virtual void MakeAndFillRttHistogramsFromFile(TFile* inputFile);
    virtual void SaveRttHistogramsToFile(TFile* outputFile);

  private:
    // Copy constructor are declared as private and have no definitions
    // intentionally. This functionalities seems to be useless.
    TileOFA(const TileOFA& tileOfaAlgorithm);
    // Assignment operator are declared as private and have no definitions
    // intentionally. This functionalities seems to be useless
    TileOFA& operator=(const TileOFA& tileOfaAlgorithm);
    // Initializes channel status using branch "bad" in tree "h2000_map".
    void InitializeChannelStatus(void);
    // Corrects channel status in order to switch off all really absent channels
    // and modules.
    void CorrectChannelStatus(void);
    // Switches off given channels in particular partition.
    // Channels and partitions start from 0.
    void SwitchOffAllModulesFor(TArrayI& channels, Int_t partition);
    // Switches off given module in particular partition.
    // Channels and partitions start from 0. Modules starts from 1.
    void SwitchOffAllChannelsFor(Int_t module, Int_t partition);
    // Switches off given modules in particular partition.
    // Partitions start from 0. Modules start from 1.
    void SwitchOffAllChannelsFor(TArrayI& modules, Int_t partition);
    // // Channels and partitions start from 0. Modules start from 1.
    void SwitchOffChannels(TArrayI& channels, Int_t module, Int_t partition);
    // Switches off modules given in input command of this macro as
    // bad modules. Modules start from 1.
    void SwitchOffAdditionalModules();
    // Initializes type of run using branch "cispar" in tree "h2000".
    void InitializeRunType(void);
    // Initialize run number using branch "Run" in tree "h2000".
    void InitializeRunNumber(void);
    // Initializes energy units using branch "OFLunits" in tree "h2000".
    void InitializeEnergyUnits(void);
    // Makes histograms for all quantities (energy, time, pedestal,
    // chi2) available in input tree h2000 for particular combination of
    // Optimal Filtering Algorithm and gain.
    void MakeHistogramsForAllQuantities(void);
    // Tries to book two dimensional histogram representing distribution of
    // average time vis module and channel for particular combination of
    // Optimal Filtering Algorithm and gain. In the case of success the
    // method returns "true" in the other case it returns "false".
    Bool_t TryBookEnergyVsModuleAndChannel(void);
    // Tries to book two dimensional histogram representing distribution of
    // average time vis module and channel for particular combination of
    // Optimal Filtering Algorithm and gain. In the case of success the
    // method returns "true" in the other case it returns "false".
    Bool_t TryBookTimeVsModuleAndChannel(void);
    // Tries to book two dimensional histogram representing distribution of
    // average pedestal vis module and channel for particular combination of
    // Optimal Filtering Algorithm and gain. In the case of success the
    // method returns "true" in the other case it returns "false".
    Bool_t TryBookPedestalVsModuleAndChannel(void);
    // Tries to book two dimensional histogram representing distribution of
    // average chi2 vis module and channel for particular combination of
    // Optimal Filtering Algorithm and gain. In the case of success the
    // method returns "true" in the other case it returns "false".
    Bool_t TryBookChi2VsModuleAndChannel(void);
    // Tries to book two dimensional histogram representing distribution of
    // average quantity vis module and channel for particular Optimal Filtering
    // Algorithm and gain. In the case of success the method returns "true"
    // in the other case it returns "false".
    Bool_t TryBookQuantityVsModuleAndChannel(Quantity_t quantity, TString quantityName, TString quantityBranchName,
        TProfile2D* quantityVsModuleAndChannel[4]);
    Bool_t TryBookQuantityVsModuleAndChannel(QuantityMF_t quantity, TString quantityName, TString quantityBranchName,
        TProfile2D* quantityVsModuleAndChannel[4]);
    // Books TProfile2D histogram representing average quantity distribution
    // vis module and channel.
    TProfile2D* BookQuantityVsModuleAndChannel(TString quantityName, TString partitionName);
    // Fills all two dimensional histograms booked earlier successively and
    // representing distribution of average quantities vis module and
    // channel for particular combination of Optimal Filtering Algorithm
    // and gain.
    void FillAllQuantitiesVsModuleAndChannel(void);
    // Tries to fill two dimensional histogram representing distribution of
    // average energy vis module and channel for particular combination of
    // Optimal Filtering Algorithm and gain if it has been booked earlier
    // successively.
    Bool_t TryFillEnergyVsModuleAndChannel(void);
    // Tries to fill two dimensional histogram representing distribution of
    // average time vis module and channel for particular combination of
    // Optimal Filtering Algorithm and gain if it has been booked earlier
    // successively.
    Bool_t TryFillTimeVsModuleAndChannel(void);
    // Tries to fill two dimensional histogram representing distribution of
    // average pedestal vis module and channel for particular combination of
    // Optimal Filtering Algorithm and gain if it has been booked earlier
    // successively.
    Bool_t TryFillPedestalVsModuleAndChannel(void);
    // Tries to fill two dimensional histogram representing distribution of
    // average chi2 vis module and channel for particular combination of
    // Optimal Filtering Algorithm and gain if it has been booked earlier
    // successively.
    Bool_t TryFillChi2VsModuleAndChannel(void);
    // Fills two dimensional histogram booked earlier successively and
    // representing distribution of average quantity vis module and
    // channel for particular combination of Optimal Filtering Algorithm
    // and gain.
    void FillQuantityVsModuleAndChannel(TProfile2D* quantityVsModuleAndChannel[4], Quantity_t quantity);
    void FillQuantityVsModuleAndChannel(TProfile2D* quantityVsModuleAndChannel[4], QuantityMF_t quantity);
    // Return "true" if channel is to be processed;
    // Channels and partitions start from 0. Modules start from 1.
    Bool_t IsChannelToBeProcessed(Int_t partition, Int_t module, Int_t channel);
    // Returns "true" if channel is good.
    // Channels and partitions start from 0. Modules start from 1.
    Bool_t IsChannelGood(Int_t partition, Int_t module, Int_t channel);
    // Returns "true" if energy in particular channel is more than
    // energy threshold set up earlier.
    // Channels and partitions start from 0. Modules start from 1.
    Bool_t IsChannelEnergyMoreThanThreshold(Int_t partition, Int_t module,
        Int_t channel);
    // Returns "true" if chi2 in particular channel is ok.
    // Channels and partitions start from 0. Modules start from 1.
    Bool_t IsChannelChi2Ok(Int_t partition, Int_t module, Int_t channel);
    // Returns "true" if pedestal in particular channel has mask information.
    // i.e. pedestal > 5000.
    // Channels and partitions start from 0. Modules start from 1.
    Bool_t HasPedestalMaskInfo(Int_t partition, Int_t module, Int_t channel);
    Bool_t HasPedestalMaskInfo(Float_t value);
    Float_t RemoveMaskInfoFromPedestal(Float_t value);
    // Makes one dimensional histograms representing average quantities
    // distribution vis channel from all two dimensional histograms
    // representing average quantities distribution vis channel and module
    // that have been gotten earlier.
    void MakeAllQuantitiesVsChannel(void);
    // Makes one dimensional histogram representing average quantity
    // distribution vis channel from two dimensional histogram representing
    // average quantity distribution vis channel and module that has been
    // gotten earlier.
    TH1* MakeQuantityVsChannelFrom(TProfile2D* quantityVsModuleAndChannel);
    // Makes one dimensional histograms representing average quantities
    // distribution vis module from all two dimensional histograms
    // representing average quantities distribution vis channel and module
    // that have been gotten earlier.
    void MakeAllQuantitiesVsModule(void);
    // Makes one dimensional histogram representing average quantity
    // distribution vis module from two dimensional histogram representing
    // average quantity distribution vis channel and module that has been
    // gotten earlier.
    TH1* MakeQuantityVsModuleFrom(TProfile2D* quantityVsModuleAndChannel);
    // Sets channels names for every fifth virtual channel number in
    // histograms representing distribution average quantity vis channel.
    // Virtual channel number defined as (partition number * 48) + channel
    // number and used to place on the same histogram data from all partitions.
    // Virtual channel number has range from 0  to 192.
    void MakeAllQuantitiesAsTH2D(void);
    void SetQuantityYTitleFor(TH1* histogram);
    TString GetQuantityNameFrom(TH1* histogram);
    void LogMessage(LogLevel messageType, TString message);
    // Forms and logs string of symbol. Used to decorate titles in log.
    static const char* partitionsNames_[];
    // All one dimensional histograms produced for particular Optimal Filtering
    // Algorithm and gain.
    TObjArray* histsTH1D_[4];
    // All two dimensional histograms produced for particular Optimal Filtering
    // Algorithm and gain.
    TObjArray* histsProf2D_[4];
    TObjArray* histsTH2D_[4];
    // Name of Optimal Filtering Algorithm.
    TString algorithmName_;
    // Name of branch for energy in tree h2000 for particular Optimal Filtering
    // Algorithm.
    TString energyBranchName_;
    // Name of branch for time in tree h2000 for particular Optimal Filtering
    // Algorithm.
    TString timeBranchName_;
    // Name of branch for pedestal in tree h2000 for particular Optimal
    // Filtering Algorithm.
    TString pedestalBranchName_;
    // Name of branch for chi2 in tree h2000 for particular Optimal Filtering
    // Algorithm.
    TString chi2BranchName_;
    SimpleLogger* logger_;
    // Function used for finding average quantity over channels and
    // modules.
    // Tree in input ntuple with name h2000.
    TTree* h2000_tree_;
    // Represents energy for particular event.
    Quantity_t energy_;
    // Represents time for particular event.
    Quantity_t time_;
    // Represents pedestal for particular event.
    Quantity_t pedestal_;
    // Represents chi2 for particular event.
    Quantity_t chi2_;

    Bool_t isMF;
    QuantityMF_t energyMF_;
    // Represents time for particular event.
    QuantityMF_t timeMF_;

    TString gainName_;
    GainCode gainCode_;
    TString energyUnitsName_;
    TString runTypeName_;
    Int_t runNumber_;
    // Two dimensional histogram representing average energy
    // distribution vis channel and module
    TProfile2D* energyVsModuleAndChannel_[4];
    // Two dimensional histogram representing average time
    // distribution vis channel and module
    TProfile2D* timeVsModuleAndChannel_[4];
    // Two dimensional histogram representing average pedestal
    // distribution vis channel and module
    TProfile2D* pedestalVsModuleAndChannel_[4];
    // Two dimensional histogram representing average chi2
    // distribution vis channel and module
    TProfile2D* chi2VsModuleAndChannel_[4];
    // Tree in input ntuple with name h2000_map.
    TTree* h2000_map_tree_;
    // Represents channel status from branch "bad" in tree "h2000_map" and
    // corrected by method "CorrectChannelStatus" in the form of array
    //  [i][j][k][l], where:
    //     i - partition number from 0 to 3 in order: LBA, LBC, EBA, EBC;
    //     j - module number from 0 to 63;
    //     k - channel number from 0 to 47.
    //     l - gain from 0 to 1.
    Short_t channelStatus_[4][64][48][2];
    // Represents channel gain from branch "gain" in tree "h2000"
    // in the form of array [i][j][k], where:
    //     i - partition number from 0 to 3 in order: LBA, LBC, EBA, EBC;
    //     j - module number from 0 to 63;
    //     k - channel number from 0 to 47.
    Short_t gain_[4][64][48];
    // Represents if only channels with energy more than energy threshold
    // will be processed.
    Bool_t energyThresholdIsUsed_;
    Float_t energyThreshold_;
    Bool_t useBadChStatus_;
    Bool_t usePedToMaskCh_;
    TObjArray* badModules_;
};

//*****************************************************************************
//                     Description of class "TileOfaCollection"
//*****************************************************************************

// Collection of objects of type "TileOfaInterface". Collection expand
// automatically when objects are added. "TileOfaCollection owns the
// objects it points to and deletion of collection itself lead to the deletion
// of its contents.
class TileOfaCollection: public TileOfaInterface {
  public:
    // Constructs collection of objects of type "TileOfaInterface". Input
    // parameter is  initial capacity of collection.
    TileOfaCollection(Int_t initialSize);
    virtual ~TileOfaCollection();
    // Adds to collection the object of type "TileOfaInterface".
    void Add(TileOfaInterface* algorithm);
    virtual void SetEnergyUniGainThreshold(Float_t energyUniGainThreshold);
    virtual void SetEnergyLowGainThreshold(Float_t energyLowGainThreshold);
    virtual void SetEnergyHighGainThreshold(Float_t energyHighGainThreshold);
    virtual void UseBadChannelStatus(Bool_t useChStatus);
    virtual void UsePedestalToMaskChannel(Bool_t usePedToMaskCh);
    virtual void SetBadModules(TObjArray* modules);
    virtual void MakeAndFillRttHistogramsFromFile(TFile* inputFile);
    virtual void SaveRttHistogramsToFile(TFile* outputFile);
  private:
    // Copy constructor are declared as private and have no definitions
    // intentionally. This functionalities seems to be useless.
    TileOfaCollection(const TileOFA& TileOfaCollection);
    // Assignment operator are declared as private and have no definitions
    // intentionally. This functionalities seems to be useless
    TileOfaCollection& operator=(const TileOFA& TileOfaCollection);
    TileOfaInterface** algorithms_;
    Int_t capacity_;
    Int_t size_;
    SimpleLogger* logger_;
};

//-----------------------------------------------------------------------------
//   Definition and description of function "FindInputFilesInCurrentDirectory"
//-----------------------------------------------------------------------------

// Finds all files in current directory that contains input
// file name pattern.
Bool_t FindInputFilesInCurrentDirectory(TString fileNamePattern, std::vector<
    TString>& fileNames) {
  Bool_t inputFileIsFound = false;
  void* directory = gSystem->OpenDirectory("./");
  if (directory != 0) {
    const char* file = 0;
    while ((file = gSystem->GetDirEntry(directory))) {
      TString fileName(file);
      if (fileName.Contains(TRegexp(fileNamePattern))) {
        fileNames.push_back(fileName);
        inputFileIsFound = true;
      }
    }
  }
  return inputFileIsFound;
}

//-----------------------------------------------------------------------------
//         Definition and description of function "FindInputFileName"
//-----------------------------------------------------------------------------

// Finds input file name. Must be  found only one input file.
Bool_t FindInputFileName(TString& inputFileName) {
  Bool_t inputFileIsFound = false;
  SimpleLogger* logger = SimpleLogger::GetLogger();
  std::vector<TString> inputFileNames;
  Bool_t inputFileFoundInCurrentDirectory = FindInputFilesInCurrentDirectory(
      INPUT_FILE_NAME_PATTERN, inputFileNames);
  if (inputFileFoundInCurrentDirectory) {
    if ((Int_t) inputFileNames.size() == 1) {
      inputFileName = inputFileNames[0];
      inputFileIsFound = true;
    } else {
      // Form all found input file names in one string.
      TString inputFileNamesAsString = inputFileNames[0];
      for (Int_t iFileName = 1; iFileName < (Int_t) inputFileNames.size(); iFileName++) {
        inputFileNamesAsString.Append(", ");
        inputFileNamesAsString.Append(inputFileNames[iFileName]);
      }
      logger->Error("Too many input file names found in current directory: "
          + inputFileNamesAsString + "!");
    }
  }
  return inputFileIsFound;
}


void AddAllGainsOfaToCollection(TileOfaCollection& tileOfaAlgorithms, TileOfaDescription& ofaDescription) {

  TileOFA* tileOfaUniGain = new TileOFA(ofaDescription.GetForUniGain());
  tileOfaAlgorithms.Add(tileOfaUniGain);
  TileOFA* tileOfaHighGain = new TileOFA(ofaDescription.GetForHighGain());
  tileOfaAlgorithms.Add(tileOfaHighGain);
  TileOFA* tileOfaLowGain = new TileOFA(ofaDescription.GetForLowGain());
  tileOfaAlgorithms.Add(tileOfaLowGain);
}

//-----------------------------------------------------------------------------
//   Definition and description of function "PrepareTileOfaRttHistograms"
//-----------------------------------------------------------------------------

// Prepares RTT histograms for all Tile Optimal Filtering Algorithm and
// gains.
void PrepareTileOfaAlgorithms(TileOfaCollection& tileOfaAlgorithms) {
  // Preparing rtt histograms for TileOptATLAS for all gains.
  TileOfaDescription tileOptAtlasDescr("TileOptATLAS", "ene", "time", "ped", "chi2");
  AddAllGainsOfaToCollection(tileOfaAlgorithms, tileOptAtlasDescr);

  // Preparing rtt histograms for TileOpt2 for all gains.
  TileOfaDescription tileOpt2Descr("TileOpt2", "eOpt", "tOpt", "pedOpt", "chi2Opt");
  AddAllGainsOfaToCollection(tileOfaAlgorithms, tileOpt2Descr);

  // Preparing rtt histograms for TileFit for all gains..
  TileOfaDescription tileFitDescr("TileFit", "eFit", "tFit", "pedFit", "chi2Fit");
  AddAllGainsOfaToCollection(tileOfaAlgorithms, tileFitDescr);

  // Preparing rtt histograms for TileOF1 for all gains..
  TileOfaDescription tileOF1Descr("TileOF1", "eOF1", "tOF1", "pedOF1", "chi2OF1");
  AddAllGainsOfaToCollection(tileOfaAlgorithms, tileOF1Descr);

  // Preparing rtt histograms for TileMF for all gains..
  TileOfaDescription tileMFDescr("TileMF", "eMF", "tMF", "pedMF", "chi2MF");
  AddAllGainsOfaToCollection(tileOfaAlgorithms, tileMFDescr);

}

//-----------------------------------------------------------------------------
//     Definition and description of function "InitializeEnergyThresholds"
//-----------------------------------------------------------------------------

// Initializes energy thresholds for all gains if any are given.
void InitializeThresholds(TileOfaCollection& allAlgorithmsRttHistograms, TMap& parameters) {
  if (parameters.GetValue("uniGainEThr") != 0) {
    TObjString* uniGainEThr = (TObjString*) parameters.GetValue("uniGainEThr");
    Float_t energyUniGainThreshold = uniGainEThr->String().Atof();
    allAlgorithmsRttHistograms.SetEnergyUniGainThreshold(energyUniGainThreshold);
  }
  if (parameters.GetValue("lowGainEThr") != 0) {
    TObjString* lowGainEThr = (TObjString*) parameters.GetValue("lowGainEThr");
    Float_t energyLowGainThreshold = lowGainEThr->String().Atof();
    allAlgorithmsRttHistograms.SetEnergyLowGainThreshold(energyLowGainThreshold);
  }
  if (parameters.GetValue("highGainEThr") != 0) {
    TObjString* highGainEThr = (TObjString*) parameters.GetValue("highGainEThr");
    Float_t energyHighGainThreshold = highGainEThr->String().Atof();
    allAlgorithmsRttHistograms.SetEnergyHighGainThreshold( energyHighGainThreshold);
  }
}

//-----------------------------------------------------------------------------
//     Definition and description of function "InitializeEnergyThresholds"
//-----------------------------------------------------------------------------

// Initializes energy thresholds for all gains if any are given.
void InitializeBadModules(TileOfaCollection& allAlgorithmsRttHistograms, TMap& parameters) {
  if (parameters.GetValue("badModules") != 0) {
    TObjString* badModules = (TObjString*) parameters.GetValue("badModules");
    TString mod(badModules->GetName());
    TObjArray* modules = mod.Tokenize(",");
    allAlgorithmsRttHistograms.SetBadModules(modules);
    delete modules;
  }
}

//-----------------------------------------------------------------------------
//     Definition and description of function "InitializeBadChannelStatus"
//-----------------------------------------------------------------------------

// Initializes property use or don't useBadChannelStatus.
void InitializeBadChannelStatus(TileOfaCollection& allAlgorithmsRttHistograms, TMap& parameters) {
  if (parameters.GetValue("useBadChannelStatus") != 0) {
    TObjString* useChStatus = (TObjString*) parameters.GetValue("useBadChannelStatus");
    if (useChStatus->String().EqualTo("False")){
      allAlgorithmsRttHistograms.UseBadChannelStatus(false);
    }
  }
}

//-----------------------------------------------------------------------------
//     Definition and description of function "InitializeBadChannelStatus"
//-----------------------------------------------------------------------------

// Initializes property usePedestalToMaskChannels to mask channels.
void InitializeUsePedestalToMaskChannel(TileOfaCollection& allAlgorithmsRttHistograms, TMap& parameters) {
  if (parameters.GetValue("usePedestalToMaskChannel") != 0) {
    TObjString* useChStatus = (TObjString*) parameters.GetValue("usePedestalToMaskChannel");
    if (useChStatus->String().EqualTo("True")){
      allAlgorithmsRttHistograms.UsePedestalToMaskChannel(true);
    }
  }
}


//-----------------------------------------------------------------------------
// Definition and description of "DoTileOptimalFilteringAlgsRttHistograms"
//-----------------------------------------------------------------------------

// ROOT MACRO to be invoked in RTT framework.
ExitResult DoTileOptimalFilteringAlgsRttHistograms(TMap& parameters) {
  SimpleLogger* logger = SimpleLogger::GetLogger();
  // Initialize effective log level of logger by Macro LOG_LEVEL.
  logger->SetLogLevel(static_cast<LogLevel> (LOG_LEVEL));
  logger->Info(TString("ROOT macro ") + ROOT_MACRO_NAME + " is starting ...");
  // Container for all Optimal Filtering Algorithms histograms to be used in
  // RTT framework.
  TileOfaCollection tileOfaAlgorithms(9);
  PrepareTileOfaAlgorithms(tileOfaAlgorithms);
  InitializeThresholds(tileOfaAlgorithms, parameters);
  InitializeBadChannelStatus(tileOfaAlgorithms, parameters);
  InitializeUsePedestalToMaskChannel(tileOfaAlgorithms, parameters);
  InitializeBadModules(tileOfaAlgorithms, parameters);
  TString inputFileName;
  Bool_t inputFileIsFound = FindInputFileName(inputFileName);
  if (!inputFileIsFound) {
    logger->Error("No input file is found!");
    return FAILLURE;
  } else {
    logger->Info("Found input file name is " + inputFileName);
  }
  TFile* inputFile = new TFile(inputFileName);
  if (inputFile->IsZombie()) { logger->Error("Input file " + inputFileName + " is incorrect root file or does not exist!");
    return FAILLURE;
  }
  TString outputFilename;
  if (parameters.GetValue("outputFile") != 0) {
    outputFilename = (parameters.GetValue("outputFile"))->GetName();
  } else {
    outputFilename = OUTPUT_FILE_NAME;
  }
  logger->Info("Output file name is " + outputFilename + ".");
  TFile* outputFile = new TFile(outputFilename, "recreate");
  tileOfaAlgorithms.MakeAndFillRttHistogramsFromFile(inputFile);
  tileOfaAlgorithms.SaveRttHistogramsToFile(outputFile);
  outputFile->Close();
  inputFile->Close();
  logger->Info(TString("ROOT macro ") + ROOT_MACRO_NAME + " is finished successful.");
  return SUCCESS;
}


ExitResult DoTileOptimalFilteringAlgsRttHistograms(const TString& argument) {
  SimpleLogger* logger = SimpleLogger::GetLogger();
  TMap parameters;
  parameters.SetOwnerKeyValue(true, true);
  TObjArray* arguments = argument.Tokenize("--");
  Int_t nParameters = arguments->GetEntries();
  TString debugMessage;
  debugMessage.Form("Number of input parameters is: %i", nParameters);
  logger->Debug(debugMessage);
  for (Int_t iParameter = 0; iParameter < nParameters; iParameter++) {
    TString parameter = arguments->At(iParameter)->GetName();
    TObjArray* keyValue = parameter.Tokenize("=");
    TObjString* key = (TObjString*) keyValue->At(0);
    TObjString* value = (TObjString*) keyValue->At(1);
    parameters.Add(key, value);
  }
  return DoTileOptimalFilteringAlgsRttHistograms(parameters);
}


//-----------------------------------------------------------------------------
//               Definition and description of function main
//-----------------------------------------------------------------------------

// Used only for testing purposes.
Int_t main(int argc, char* argv[]) {
  TString argument;
  for (Int_t iParameter = 1; iParameter < argc; iParameter++) {
    argument.Append(argv[iParameter]);
  }
  return DoTileOptimalFilteringAlgsRttHistograms(argument);
}

//*****************************************************************************
//                     Definition of class "SimpleLogger"
//*****************************************************************************

SimpleLogger* SimpleLogger::instance_ = 0;

inline SimpleLogger* SimpleLogger::GetLogger(void) {
  if (instance_ == 0) {
    instance_ = new SimpleLogger();
  }
  return instance_;
}

inline void SimpleLogger::Info(TString message) {
  Log(INFO, message);
}

inline void SimpleLogger::Error(TString message) {
  Log(ERROR, message);
}

inline void SimpleLogger::Debug(TString message) {
  Log(DEBUG, message);
}

inline LogLevel SimpleLogger::GetLogLevel(void) {
  return logLevel_;
}

inline void SimpleLogger::SetLogLevel(LogLevel level) {
  if (level >= DEBUG) {
    if (level <= ERROR) {
      logLevel_ = level;
    }
  } else {
    instance_->Error("Unknown log level! Log level is unchanged: "
        + getMessageType(logLevel_) + "!");
  }
}

inline TString SimpleLogger::getMessageType(LogLevel messageSeverity) {
  TString messageType;
  switch (messageSeverity) {
    case DEBUG:
      messageType = "DEBUG";
      break;
    case INFO:
      messageType = "INFO";
      break;
    case ERROR:
      messageType = "ERROR";
      break;
    default:
      messageType = "UNKNOWN";
      break;
  }
  return messageType;
}

inline void SimpleLogger::Log(LogLevel messageSeverity, TString message) {
  if (logLevel_ <= messageSeverity) {
    TString messageType = getMessageType(messageSeverity);
    std::cout << messageType << ":\t" << message << std::endl;
  }
}

//*****************************************************************************
//                     Definition of class "TileOfaDescription"
//*****************************************************************************

TileOfaDescription::TileOfaDescription(TString algorithmName,
    TString energyBranchName, TString timeBranchName,
    TString pedestalBranchName, TString chi2BranchName)
  : gainName_("uni")
  , gainCode_(UNI)
  , algorithmName_(algorithmName)
  , energyBranchName_(energyBranchName)
  , timeBranchName_(timeBranchName)
  , pedestalBranchName_(pedestalBranchName)
  , chi2BranchName_(chi2BranchName) {
}

TileOfaDescription::TileOfaDescription(const TileOfaDescription& description) {
  algorithmName_ = description.algorithmName_;
  gainName_ = description.gainName_;
  gainCode_ = description.gainCode_;
  gainAwareSuffix_ = description.gainAwareSuffix_;
  energyBranchName_ = description.energyBranchName_;
  timeBranchName_ = description.timeBranchName_;
  pedestalBranchName_ = description.pedestalBranchName_;
  chi2BranchName_ = description.chi2BranchName_;
}

TString TileOfaDescription::GetAlgorithmName(void) {
  return algorithmName_;
}

TString TileOfaDescription::GetEnergyBranchName(void) {
  return energyBranchName_ + gainAwareSuffix_;
}

TString TileOfaDescription::GetTimeBranchName(void) {
  return timeBranchName_ + gainAwareSuffix_;
}

TString TileOfaDescription::GetPedestalBranchName(void) {
  return pedestalBranchName_ + gainAwareSuffix_;
}

TString TileOfaDescription::GetChi2BranchName(void) {
  return chi2BranchName_ + gainAwareSuffix_;
}

TString TileOfaDescription::GetGainName(void) {
  return gainName_;
}

GainCode TileOfaDescription::GetGainCode(void) {
  return gainCode_;
}

TileOfaDescription TileOfaDescription::GetForUniGain(void) {
  TileOfaDescription tileOfaDiscription_(*this);
  tileOfaDiscription_.gainAwareSuffix_ = "";
  tileOfaDiscription_.gainName_ = "Uni";
  tileOfaDiscription_.gainCode_ = UNI;
  return tileOfaDiscription_;
}

TileOfaDescription TileOfaDescription::GetForHighGain(void) {
  TileOfaDescription tileOfaDiscription_(*this);
  tileOfaDiscription_.gainAwareSuffix_ = "_hi";
  tileOfaDiscription_.gainName_ = "High";
  tileOfaDiscription_.gainCode_ = HIGH;
  return tileOfaDiscription_;
}

TileOfaDescription TileOfaDescription::GetForLowGain(void) {
  TileOfaDescription tileOfaDiscription_(*this);
  tileOfaDiscription_.gainAwareSuffix_ = "_lo";
  tileOfaDiscription_.gainName_ = "Low";
  tileOfaDiscription_.gainCode_ = LOW;
  return tileOfaDiscription_;
}

//*****************************************************************************
//                     Definition of class "TileOFA"
//*****************************************************************************

// Partitions names are initialized in order corresponding to order of
// partitions in arrays where  quantities are stored.
const char* TileOFA::partitionsNames_[] = { "LBA", "LBC", "EBA", "EBC" };

TileOFA::TileOFA(TileOfaDescription tileOfaDescr)
  : algorithmName_(tileOfaDescr.GetAlgorithmName())
  , energyBranchName_(tileOfaDescr.GetEnergyBranchName())
  , timeBranchName_(tileOfaDescr.GetTimeBranchName())
  , pedestalBranchName_(tileOfaDescr.GetPedestalBranchName())
  , chi2BranchName_(tileOfaDescr.GetChi2BranchName())
  , logger_(SimpleLogger::GetLogger())
  , h2000_tree_(0)
  , isMF(tileOfaDescr.GetAlgorithmName().EqualTo("TileMF"))
  , gainName_(tileOfaDescr.GetGainName())
  , gainCode_(tileOfaDescr.GetGainCode())
  , runNumber_(-1)
  , h2000_map_tree_(0)
  , energyThresholdIsUsed_(false)
  , energyThreshold_(-999999999)
  , useBadChStatus_(true)
  , usePedToMaskCh_(false)
  , badModules_(0)

 {
  for (Int_t iPartition = LBA; iPartition <= EBC; iPartition++) {
    histsProf2D_[iPartition] = new TObjArray();
    histsTH2D_[iPartition] = new TObjArray();
    histsTH1D_[iPartition] = new TObjArray();
  }
  memset(channelStatus_, 0, sizeof(channelStatus_));
}

// Deletes all produced histograms for particular Optimal Filtering Algorithm
// and deletes function used for finding average quantity over channel and
// modules.
TileOFA::~TileOFA() {
  for (Int_t iPartition = LBA; iPartition <= EBC; iPartition++) {
    histsProf2D_[iPartition]->Delete();
    histsTH2D_[iPartition]->Delete();
    histsTH1D_[iPartition]->Delete();
  }
  if (badModules_ != 0) {
    delete badModules_;
  }
}

void TileOFA::SetEnergyUniGainThreshold(Float_t energyUniGainThreshold) {
  if (gainCode_ == UNI) {
    energyThresholdIsUsed_ = true;
    energyThreshold_ = energyUniGainThreshold;
  }
}

void TileOFA::SetEnergyLowGainThreshold(Float_t energyLowGainThreshold) {
  if (gainCode_ == LOW) {
    energyThresholdIsUsed_ = true;
    energyThreshold_ = energyLowGainThreshold;
  }
}

void TileOFA::SetEnergyHighGainThreshold(Float_t energyHighGainThreshold) {
  if (gainCode_ == HIGH) {
    energyThresholdIsUsed_ = true;
    energyThreshold_ = energyHighGainThreshold;
  }
}

void TileOFA::UseBadChannelStatus(Bool_t useChStatus) {
  useBadChStatus_ = useChStatus;
}

void TileOFA::UsePedestalToMaskChannel(Bool_t usePedToMaskCh) {
  if (usePedToMaskCh)
    LogMessage(INFO, "Channels with pedestal greater than 5000 will be masked");
  else
    LogMessage(INFO, "Channels with pedestal greater than 5000 will not be masked");
  
  usePedToMaskCh_ = usePedToMaskCh;
}

void TileOFA::SetBadModules(TObjArray* modules) {
  badModules_ = (TObjArray*) modules->Clone();
}

void TileOFA::MakeAndFillRttHistogramsFromFile(TFile* inputFile) {
  inputFile->GetObject("h2000_map", h2000_map_tree_);
  inputFile->GetObject("h2000", h2000_tree_);
  if (h2000_map_tree_) {
    if (h2000_tree_) {
      MakeHistogramsForAllQuantities();
    } else {
      TString errorMessage = TString("Can not get tree h2000 from file ") + inputFile->GetName() + "!";
      LogMessage(ERROR, errorMessage);
    }
  } else {
    TString errorMessage = TString("Can not get tree h2000_map from file ") + inputFile->GetName() + "!";
    LogMessage(ERROR, errorMessage);
  }
}

// Saves all produced histograms for particular Optimal Filtering Algorithm
// in file. All histograms are saved in directory named after algorithm name.
void TileOFA::SaveRttHistogramsToFile(TFile *outputFile) {
  if ((!histsProf2D_[1]->IsEmpty())
      || (!histsTH2D_[1]->IsEmpty()) 
      || (!histsTH1D_[1]->IsEmpty())) {
    TDirectory* algorithmDirectory = outputFile->GetDirectory(algorithmName_);
    if (algorithmDirectory == 0) {
      algorithmDirectory = outputFile->mkdir(algorithmName_);
    }
    algorithmDirectory->cd();
    TString gainDirectoryName = gainName_ + "Gain";
    TDirectory* gainDirectory = algorithmDirectory->GetDirectory(
        gainDirectoryName);
    if (gainDirectory == 0) {
      gainDirectory = algorithmDirectory->mkdir(gainDirectoryName);
    }
    gainDirectory->cd();
    for (Int_t iPartition = LBA; iPartition <= EBC; iPartition++) {
      TString partitionName = partitionsNames_[iPartition];
      TDirectory* partitionDirectory = gainDirectory->mkdir(partitionName);
      partitionDirectory->cd();
      histsProf2D_[iPartition]->Write();
      histsTH1D_[iPartition]->Write();
      histsTH2D_[iPartition]->Write();
      gainDirectory->cd();
    }
  }
}

void TileOFA::InitializeChannelStatus(void) {
  TBranch* channelStatusBranch = h2000_map_tree_->GetBranch("bad");
  if (channelStatusBranch != 0) {
    channelStatusBranch->SetAddress(&channelStatus_);
    channelStatusBranch->GetEntry(0);
    channelStatusBranch->ResetAddress();
  }
}


void TileOFA::CorrectChannelStatus(void) {
  Int_t abscentChannelsInLongBarrel[] = { 30, 31, 43 };
  Int_t abscentChannelsInExtendedBarrel[] = { 18, 19, 24, 25, 26, 27, 28, 29,
      33, 34, 42, 43, 44, 45, 46, 47 };

  Int_t nChannelsInLB = sizeof(abscentChannelsInLongBarrel) / sizeof(Int_t);
  TArrayI abscentChannelsInLB(nChannelsInLB, abscentChannelsInLongBarrel);

  Int_t nChannelsInEB = sizeof(abscentChannelsInExtendedBarrel) / sizeof(Int_t);
  TArrayI abscentChannelsInEB(nChannelsInEB, abscentChannelsInExtendedBarrel);

  SwitchOffAllModulesFor(abscentChannelsInLB, LBA);
  SwitchOffAllModulesFor(abscentChannelsInLB, LBC);
  SwitchOffAllModulesFor(abscentChannelsInEB, EBA);
  SwitchOffAllModulesFor(abscentChannelsInEB, EBC);

  // Switch off channels 0, 1, 2, 3 in module EBA15
  // and in module EBC18.
  Int_t badChannels[] = { 0, 1, 2, 3 };
  Int_t nBadChannels = sizeof(badChannels) / sizeof(Int_t);
  TArrayI additionalBadChannels(nBadChannels, badChannels);

  SwitchOffChannels(additionalBadChannels, 15, EBA);
  SwitchOffChannels(additionalBadChannels, 18, EBC);

  SwitchOffAdditionalModules();
}

// Channels and partitions start from 0. Modules start from 1.
void TileOFA::SwitchOffAllModulesFor(TArrayI& channels, Int_t partition) {
  Int_t channel;
  for (Int_t iChannel = 0; iChannel < channels.GetSize(); iChannel++) {
    channel = channels.At(iChannel);
    for (Int_t iGain = 0; iGain < 2; iGain++) {
      for (Int_t iModule = 0; iModule < 64; iModule++) {
        channelStatus_[partition][iModule][channel][iGain] = BAD;
      }
    }
  }
}

// Channels and partitions start from 0. Modules start from 1.
void TileOFA::SwitchOffAllChannelsFor(Int_t module, Int_t partition) {
  for (Int_t iGain = 0; iGain < 2; iGain++) {
    for (Int_t iChannel = 0; iChannel < 48; iChannel++) {
      channelStatus_[partition][module - 1][iChannel][iGain] = BAD;
    }
  }
}

// Channels and partitions start from 0. Modules start from 1.
void TileOFA::SwitchOffAllChannelsFor(TArrayI& modules, Int_t partition) {
  Int_t module;
  for (Int_t iModule = 0; iModule < modules.GetSize(); iModule++) {
    module = modules.At(iModule);
    SwitchOffAllChannelsFor( module, partition);
  }
}

// Channels and partitions start from 0. Modules start from 1.
void TileOFA::SwitchOffChannels(TArrayI& channels, Int_t module,
    Int_t partition) {
  Int_t channel;
  for (Int_t iChannel = 0; iChannel < channels.GetSize(); iChannel++) {
    channel = channels.At(iChannel);
    for (Int_t iGain = 0; iGain < 2; iGain++) {
      channelStatus_[partition][module - 1][channel][iGain] = BAD;
    }
  }
}

void TileOFA::SwitchOffAdditionalModules() {
  if (badModules_ != 0) {
    for (Int_t iModule = 0; iModule < badModules_->GetEntries(); iModule++) {
      TObjString *badModule = (TObjString*) (badModules_->At(iModule));
      TString moduleName = badModule->GetName();
      Int_t partition;
      if (moduleName.BeginsWith("LBA")) {
        partition = LBA;
      } else if (moduleName.BeginsWith("LBC")) {
        partition = LBC;
      } else if (moduleName.BeginsWith("EBA")) {
        partition = EBA;
      } else if (moduleName.BeginsWith("EBC")) {
        partition = EBC;
      } else {
        LogMessage(ERROR, "Unknown partition in: " + moduleName);
        LogMessage(ERROR, "Allowed partitions: LBA, LBC, EBA, EBC");
        LogMessage(ERROR, "Additional module " + moduleName + " will not be marked as bad!");
        continue;
      }

      Int_t module = (moduleName.Remove(0, 3)).Atoi();
      if ((module < 1) || (module > 64)) {
        LogMessage(ERROR, "Unknown module number: " + moduleName);
        LogMessage(ERROR, "Module number must be: 1 - 64!");
        continue;
      }
      TString infoMessage;
      infoMessage.Form("Module %s%i marked as bad!", partitionsNames_[partition], module);
      LogMessage(INFO, infoMessage);
      SwitchOffAllChannelsFor(module, partition);
    }
  }
}

void TileOFA::InitializeRunType(void) {
  TString cisparBranchName("cispar");
  Int_t cispar[110];
  Int_t runTypeCode;
  h2000_tree_->SetBranchStatus(cisparBranchName, 1);
  TBranch* cisparBranch = h2000_tree_->GetBranch(cisparBranchName);
  if (cisparBranch) {
    cisparBranch->SetAddress(&cispar);
    cisparBranch->GetEntry(0);
    cisparBranch->ResetAddress();
    runTypeCode = cispar[12];
    TString debugMessage;
    debugMessage.Form("%s %d", "Run type code is ", runTypeCode);
    LogMessage(DEBUG, debugMessage);
    switch (runTypeCode) {
      case PHY:
        runTypeName_ = "Physic";
        break;
      case LAS:
        runTypeName_ = "Laser";
        break;
      case PED:
        runTypeName_ = "Pedestal";
        break;
      case MONOCIS:
        runTypeName_ = "MonoCIS";
        break;
      case CIS:
        runTypeName_ = "CIS";
        break;
      default:
        runTypeName_ = "Unknown";
        break;
    }
    LogMessage(INFO, "Run type is " + runTypeName_);
  }
  h2000_tree_->SetBranchStatus(cisparBranchName, 0);
}

void TileOFA::InitializeRunNumber(void) {
  TString runNumberName("Run");
  h2000_tree_->SetBranchStatus(runNumberName, 1);
  TBranch* runNumberBranch = h2000_tree_->GetBranch(runNumberName);
  if (runNumberBranch) {
    runNumberBranch->SetAddress(&runNumber_);
    runNumberBranch->GetEntry(0);
    runNumberBranch->ResetAddress();
    TString infoMessage;
    infoMessage.Form("%s %d", "Run number is ", runNumber_);
    LogMessage(INFO, infoMessage);
  } else {
    LogMessage(ERROR, "There no branch: " + runNumberName + " in tree: " + h2000_tree_->GetName());
  }
  h2000_tree_->SetBranchStatus(runNumberName, 0);
}

void TileOFA::InitializeEnergyUnits(void) {
  TString energyUnitsBranchName("OFLunits");
  Short_t energyUnitsCode;
  h2000_tree_->SetBranchStatus(energyUnitsBranchName, 1);
  TBranch* energyUnitsBranch = h2000_tree_->GetBranch(energyUnitsBranchName);
  if (energyUnitsBranch != 0) {
    energyUnitsBranch->SetAddress(&energyUnitsCode);
    energyUnitsBranch->GetEntry(0);
    energyUnitsBranch->ResetAddress();
    TString debugMessage;
    debugMessage.Form("%s %d", "Energy units code is ", energyUnitsCode);
    LogMessage(DEBUG, debugMessage);
    switch (energyUnitsCode) {
      case ADCCOUNTS:
        energyUnitsName_ = "ADC count";
        break;
      case PC:
        energyUnitsName_ = "pC";
        break;
      case MEV:
        energyUnitsName_ = "Mev";
        break;
      default:
        energyUnitsName_ = "Unknown";
        break;
    }
    LogMessage(INFO, "Energy units are " + energyUnitsName_);
  }
  h2000_tree_->SetBranchStatus(energyUnitsBranchName, 0);
}

// Makes histograms for all output quantities (energy, time, pedestal
// chi2) from particular Optimal Filtering algorithm.
void TileOFA::MakeHistogramsForAllQuantities(void) {
  h2000_tree_->SetBranchStatus("*", 0);
  Bool_t energyVsModuleAndChannelIsBooked = TryBookEnergyVsModuleAndChannel();
  if (energyVsModuleAndChannelIsBooked) {
    if (energyThresholdIsUsed_) {
      TString infoMessage;
      infoMessage.Form("%s %5.1f ", "Energy threshold is ", energyThreshold_);
      LogMessage(INFO, infoMessage + energyUnitsName_);
    }
    if (gainCode_ == UNI) {
      h2000_tree_->SetBranchStatus("gain", 1);
      h2000_tree_->SetBranchAddress("gain", &gain_);
    }
    TryBookTimeVsModuleAndChannel();
    TryBookPedestalVsModuleAndChannel();
    TryBookChi2VsModuleAndChannel();
    if (useBadChStatus_) {
      InitializeChannelStatus();
    }
    CorrectChannelStatus();
    FillAllQuantitiesVsModuleAndChannel();
    MakeAllQuantitiesVsChannel();
    MakeAllQuantitiesVsModule();
    MakeAllQuantitiesAsTH2D();
  }
  h2000_tree_->ResetBranchAddresses();
}

Bool_t TileOFA::TryBookEnergyVsModuleAndChannel(void) {
  Bool_t energyVsModuleAndChannelIsBooked = false;
  if (isMF)   energyVsModuleAndChannelIsBooked = TryBookQuantityVsModuleAndChannel(energyMF_, "Energy", energyBranchName_, energyVsModuleAndChannel_);
  else  energyVsModuleAndChannelIsBooked = TryBookQuantityVsModuleAndChannel(energy_, "Energy", energyBranchName_, energyVsModuleAndChannel_);
  return energyVsModuleAndChannelIsBooked;
}

Bool_t TileOFA::TryBookTimeVsModuleAndChannel(void) {
  Bool_t timeVsModuleAndChannelIsBooked = false;
  if (isMF) timeVsModuleAndChannelIsBooked = TryBookQuantityVsModuleAndChannel(timeMF_, "Time", timeBranchName_, timeVsModuleAndChannel_);
  else TryBookQuantityVsModuleAndChannel(time_, "Time", timeBranchName_, timeVsModuleAndChannel_);
  return timeVsModuleAndChannelIsBooked;
}

Bool_t TileOFA::TryBookPedestalVsModuleAndChannel(void) {
  Bool_t pedestalVsModuleAndChannelIsBooked = false;
  pedestalVsModuleAndChannelIsBooked = TryBookQuantityVsModuleAndChannel( pedestal_, "Pedestal", pedestalBranchName_, pedestalVsModuleAndChannel_);
  return pedestalVsModuleAndChannelIsBooked;
}

Bool_t TileOFA::TryBookChi2VsModuleAndChannel(void) {
  Bool_t chi2VsModuleAndChannelIsBooked = false;
  chi2VsModuleAndChannelIsBooked = TryBookQuantityVsModuleAndChannel(chi2_, "Chi2", chi2BranchName_, chi2VsModuleAndChannel_);
  return chi2VsModuleAndChannelIsBooked;
}

// Checks if exist quantity branch for particular combination of
// Optimal Filtering Algorithms and gain. In the case of absence
// of the branch the method returns "false". In the other case
// the method book two dimensional histogram representing distribution
// of average quantity vis module and channel and returns "true".
Bool_t TileOFA::TryBookQuantityVsModuleAndChannel(Quantity_t quantity,
    TString quantityName, TString quantityBranchName,
    TProfile2D* quantityVsModuleAndChannel[4]) {
  Bool_t quantityVsModuleAndChannelIsBooked = false;
  TBranch* quantityBranch = h2000_tree_->GetBranch(quantityBranchName);
  // Define if exist branch in tree for the given quantity.
  if (quantityBranch != 0) {
    if (quantityName.Contains("Energy")) {
      InitializeRunNumber();
      InitializeRunType();
      InitializeEnergyUnits();
    }
    TString infoMessage("Found branch for " + quantityName + " is "
        + quantityBranch->GetName() + ".");
    LogMessage(INFO, infoMessage);
    for (Int_t iPartition = LBA; iPartition <= EBC; iPartition++) {
      TString partitionName = partitionsNames_[iPartition];
      quantityVsModuleAndChannel[iPartition] = BookQuantityVsModuleAndChannel(
          quantityName, partitionName);
      histsProf2D_[iPartition]->Add(quantityVsModuleAndChannel[iPartition]);
    }
    h2000_tree_->SetBranchStatus(quantityBranchName, 1);
    quantityBranch->SetAddress(quantity);
    quantityVsModuleAndChannelIsBooked = true;
  }
  return quantityVsModuleAndChannelIsBooked;
}


// Checks if exist quantity branch for particular combination of
// Optimal Filtering Algorithms and gain. In the case of absence
// of the branch the method returns "false". In the other case
// the method book two dimensional histogram representing distribution
// of average quantity vis module and channel and returns "true".
Bool_t TileOFA::TryBookQuantityVsModuleAndChannel(QuantityMF_t quantity,
    TString quantityName, TString quantityBranchName,
    TProfile2D* quantityVsModuleAndChannel[4]) {
  Bool_t quantityVsModuleAndChannelIsBooked = false;
  TBranch* quantityBranch = h2000_tree_->GetBranch(quantityBranchName);
  // Define if exist branch in tree for the given quantity.
  if (quantityBranch != 0) {
    if (quantityName.Contains("Energy")) {
      InitializeRunNumber();
      InitializeRunType();
      InitializeEnergyUnits();
    }
    TString infoMessage("Found branch for " + quantityName + " is " + quantityBranch->GetName() + ".");
    LogMessage(INFO, infoMessage);
    for (Int_t iPartition = LBA; iPartition <= EBC; iPartition++) {
      TString partitionName = partitionsNames_[iPartition];
      quantityVsModuleAndChannel[iPartition] = BookQuantityVsModuleAndChannel(quantityName, partitionName);
      histsProf2D_[iPartition]->Add(quantityVsModuleAndChannel[iPartition]);
    }
    h2000_tree_->SetBranchStatus(quantityBranchName, 1);
    quantityBranch->SetAddress(quantity);
    quantityVsModuleAndChannelIsBooked = true;
  }
  return quantityVsModuleAndChannelIsBooked;
}


// Books two dimensional histogram representing average quantity
// distribution vis channel and module. Histogram is named after
// quantity name and gain.
TProfile2D* TileOFA::BookQuantityVsModuleAndChannel(TString quantityName, TString partitionName) {
  TString gainName = gainName_;
  TString quantityNameInTitle(quantityName);
  if (quantityName.Contains("Chi2")) {
    quantityNameInTitle = "#Chi^{2}";
  }
  TString histogramName = quantityName;
  TString runInfo;
  TString runTypeName(runTypeName_);
  if (runTypeName.Contains("Unknown")) {
    runTypeName = "";
  }
  runInfo.Form("%s Run %d", runTypeName.Data(), runNumber_);
  TString histogramTitle = runInfo + " / " + algorithmName_ + " / " + gainName
      + " gain / " + partitionName + " / Average " + quantityNameInTitle;
  if (energyThresholdIsUsed_) {
    TString energyThresholdDescription;
    energyThresholdDescription.Form(" (E_{channel} > %6.1f %s)",
        energyThreshold_, energyUnitsName_.Data());
    histogramTitle += energyThresholdDescription;
  }
  TProfile2D* quantityVsModuleAndChannel = new TProfile2D(histogramName, histogramTitle, 64, 0.5, 64.5, 48, -0.5, 47.5);
  quantityVsModuleAndChannel->SetXTitle("Module");
  quantityVsModuleAndChannel->SetYTitle("Channel");
  quantityVsModuleAndChannel->SetDirectory(0);
  quantityVsModuleAndChannel->SetStats(false);
  return quantityVsModuleAndChannel;
}

void TileOFA::FillAllQuantitiesVsModuleAndChannel(void) {
  Long_t nEvents = h2000_tree_->GetEntries();
  Int_t nProcessedEvents = 0;
  for (Int_t eventNumber = 0; eventNumber < nEvents; eventNumber++) {
    h2000_tree_->GetEntry(eventNumber);
    TryFillEnergyVsModuleAndChannel();
    TryFillTimeVsModuleAndChannel();
    TryFillPedestalVsModuleAndChannel();
    TryFillChi2VsModuleAndChannel();
    ++nProcessedEvents;
  }
  TString infoMessage;
  infoMessage.Form("Number of processed events is %i", nProcessedEvents);
  LogMessage(INFO, infoMessage);
}

Bool_t TileOFA::TryFillEnergyVsModuleAndChannel(void) {
  Bool_t energyVsModuleAndChannelIsFilled = false;
  if (energyVsModuleAndChannel_[0]) {
    if (isMF) FillQuantityVsModuleAndChannel(energyVsModuleAndChannel_, energyMF_);
    else FillQuantityVsModuleAndChannel(energyVsModuleAndChannel_, energy_);
    energyVsModuleAndChannelIsFilled = true;
  }
  return energyVsModuleAndChannelIsFilled;
}

Bool_t TileOFA::TryFillTimeVsModuleAndChannel(void) {
  Bool_t timeVsModuleAndChannelIsFilled = false;
  if (timeVsModuleAndChannel_[0]) {
    if (isMF) FillQuantityVsModuleAndChannel(timeVsModuleAndChannel_, timeMF_);
    else FillQuantityVsModuleAndChannel(timeVsModuleAndChannel_, time_);
    timeVsModuleAndChannelIsFilled = true;
  }
  return timeVsModuleAndChannelIsFilled;
}

Bool_t TileOFA::TryFillPedestalVsModuleAndChannel(void) {
  Bool_t pedestalVsModuleAndChannelIsFilled = false;
  if (pedestalVsModuleAndChannel_[0]) {
    FillQuantityVsModuleAndChannel(pedestalVsModuleAndChannel_, pedestal_);
    pedestalVsModuleAndChannelIsFilled = true;
  }
  return pedestalVsModuleAndChannelIsFilled;
}

Bool_t TileOFA::TryFillChi2VsModuleAndChannel(void) {
  Bool_t chi2VsModuleAndChannelIsFilled = false;
  if (chi2VsModuleAndChannel_[0]) {
    FillQuantityVsModuleAndChannel(chi2VsModuleAndChannel_, chi2_);
    chi2VsModuleAndChannelIsFilled = true;
  }
  return chi2VsModuleAndChannelIsFilled;
}

void TileOFA::FillQuantityVsModuleAndChannel( TProfile2D* quantityVsModuleAndChannel[4], Quantity_t quantity) {
  Float_t value;
  // Channels and partitions start from 0. Modules start from 1.
  for (Int_t iPartition = LBA; iPartition <= EBC; iPartition++) {
    for (Int_t iModule = 1; iModule <= 64; iModule++) {
      for (Int_t iChannel = 0; iChannel <= 47; iChannel++) {
        if (IsChannelToBeProcessed(iPartition, iModule, iChannel)) {
	  value = quantity[iPartition][iModule - 1][iChannel];
	  if (quantity == pedestal_ && HasPedestalMaskInfo(value))
	    value = RemoveMaskInfoFromPedestal(value);
          quantityVsModuleAndChannel[iPartition]->Fill(iModule, iChannel, value);
        } 
      }
    }
  }
}

void TileOFA::FillQuantityVsModuleAndChannel( TProfile2D* quantityVsModuleAndChannel[4], QuantityMF_t quantity) {
  Float_t value;
  // Channels and partitions start from 0. Modules start from 1.
  for (Int_t iPartition = LBA; iPartition <= EBC; iPartition++) {
    for (Int_t iModule = 1; iModule <= 64; iModule++) {
      for (Int_t iChannel = 0; iChannel <= 47; iChannel++) {
        if (IsChannelToBeProcessed(iPartition, iModule, iChannel)) {
	  value = quantity[iPartition][iModule - 1][iChannel][0];
          quantityVsModuleAndChannel[iPartition]->Fill(iModule, iChannel, value);
        } 
      }
    }
  }
}


// Channels and partitions start from 0. Modules start from 1.
Bool_t TileOFA::IsChannelToBeProcessed(Int_t partition, Int_t module,
    Int_t channel) {

  if (!IsChannelGood(partition, module, channel)) {
    return false;
  }
  if (!IsChannelChi2Ok(partition, module, channel)) {
    return false;
  }
  if (energyThresholdIsUsed_ && !IsChannelEnergyMoreThanThreshold(partition, module, channel)) {
    return false;
  }
  if (usePedToMaskCh_ && HasPedestalMaskInfo(partition, module, channel)) {
    return false;
  }  
  return true;
}

// Channels and partitions start from 0. Modules start from 1.
Bool_t TileOFA::IsChannelGood(Int_t partition, Int_t module, Int_t channel) {
  Short_t channelGain;
  if (gainCode_ == UNI) {
    channelGain = gain_[partition][module - 1][channel];
  } else {
    channelGain = gainCode_;
  }
  if (channelStatus_[partition][module - 1][channel][channelGain] == GOOD) {
    return true;
  }
  return false;
}

// Channels and partitions start from 0. Modules start from 1.
Bool_t TileOFA::IsChannelEnergyMoreThanThreshold(Int_t partition, Int_t module,
    Int_t channel) {
  if (energy_[partition][module - 1][channel] > energyThreshold_) {
    return true;
  }
  return false;
}

// Channels and partitions start from 0. Modules start from 1.
Bool_t TileOFA::IsChannelChi2Ok(Int_t partition, Int_t module, Int_t channel) {
  if (chi2_[partition][module - 1][channel] != BAD_SIGNAL_CHI2) {
    return true;
  }
  return false;
}

// Channels and partitions start from 0. Modules start from 1.
Bool_t TileOFA::HasPedestalMaskInfo(Int_t partition, Int_t module, Int_t channel) {
  if (pedestal_[partition][module - 1][channel] > 5000.0F) {
    return true;
  }
  return false;
}

Bool_t TileOFA::HasPedestalMaskInfo(Float_t value) {
  if (value > 5000.0F) {
    return true;
  }
  return false;
}

Float_t TileOFA::RemoveMaskInfoFromPedestal(Float_t value) {
  while (value > 5000.0F) value -= 10000.0F;
  return value;
}

void TileOFA::MakeAllQuantitiesVsChannel(void) {
  for (Int_t iPartition = LBA; iPartition <= EBC; iPartition++) {
    TProfile2D* quantityVsModuleAndChannel;
    TH1* quantityVsChannel;
    TObjArray* histsProf2d = histsProf2D_[iPartition];
    Int_t nHistsProf2d = histsProf2d->GetLast() + 1;
    for (Int_t iHistProf2d = 0; iHistProf2d < nHistsProf2d; iHistProf2d++) {
      quantityVsModuleAndChannel = (TProfile2D*) (*histsProf2d)[iHistProf2d];
      quantityVsChannel = MakeQuantityVsChannelFrom(quantityVsModuleAndChannel);
      histsTH1D_[iPartition]->Add(quantityVsChannel);
    }
  }
}

// One dimensional histogram representing average quantity distribution vis
// channel is got from two dimensional histogram representing average quantity
// distribution vis channel and module.
TH1* TileOFA::MakeQuantityVsChannelFrom(
    TProfile2D* quantityVsModuleAndChannel) {
  TString histogramTitle(quantityVsModuleAndChannel->GetTitle());
  histogramTitle.Append(" vs channel");
  TString histogramName(quantityVsModuleAndChannel->GetName());
  TH1* quantityVsChannel = new TH1D(histogramName + "VsChannel",
      histogramTitle, 48, -0.5, 47.5);
  // This histogram will be managed by the class, therefore
  // histogram directory is set to 0.
  quantityVsChannel->SetDirectory(0);
  quantityVsChannel->SetStats(false);
  Double_t nEntries = 0;
  Double_t content = 0;
  Double_t contentsSum = 0;
  Double_t contentError = 0;
  Double_t sumw2 = 0;
  for (Int_t iChannelBin = 1; iChannelBin <= 48; iChannelBin++) {
    contentsSum = 0;
    nEntries = 0;
    sumw2 = 0;
    for (Int_t iModuleBin = 1; iModuleBin <= 64; iModuleBin++) {
      Int_t bin = quantityVsModuleAndChannel->GetBin(iModuleBin, iChannelBin);
      content = quantityVsModuleAndChannel->GetBinContent(bin);
      Double_t entries = quantityVsModuleAndChannel->GetBinEntries(bin);
      contentsSum += (content * entries);
      nEntries += entries;
      contentError = quantityVsModuleAndChannel->GetBinError(bin);
      sumw2 += contentError * contentError * entries;
    }
    if (nEntries != 0) {
      quantityVsChannel->SetBinContent(iChannelBin, (contentsSum / nEntries));
      quantityVsChannel->SetBinError(iChannelBin, (sqrt(sumw2) / nEntries));
    }
  }
  quantityVsChannel->SetXTitle("Channels");
  SetQuantityYTitleFor(quantityVsChannel);
  return quantityVsChannel;
}

void TileOFA::MakeAllQuantitiesVsModule(void) {
  for (Int_t iPartition = LBA; iPartition <= EBC; iPartition++) {
    TProfile2D* quantityVsModuleAndChannel;
    TH1* quantityVsModule;
    TObjArray* histsProf2d = histsProf2D_[iPartition];
    Int_t nHistsProf2d = histsProf2d->GetLast() + 1;
    for (Int_t iHistProf2d = 0; iHistProf2d < nHistsProf2d; iHistProf2d++) {
      quantityVsModuleAndChannel = (TProfile2D*) (*histsProf2d)[iHistProf2d];
      quantityVsModule = MakeQuantityVsModuleFrom(quantityVsModuleAndChannel);
      histsTH1D_[iPartition]->Add(quantityVsModule);
    }
  }
}

// One dimensional histogram representing average quantity distribution vis
// channel is got from two dimensional histogram representing average quantity
// distribution vis channel and module.
TH1* TileOFA::MakeQuantityVsModuleFrom(
    TProfile2D* quantityVsModuleAndChannel) {
  TString histogramTitle(quantityVsModuleAndChannel->GetTitle());
  histogramTitle.Append(" vs module");
  TString histogramName(quantityVsModuleAndChannel->GetName());
  TH1* quantityVsModule = new TH1D(histogramName + "VsModule", histogramTitle,
      64, 0.5, 64.5);
  // This histogram will be managed by the class, therefore
  // histogram directory is set to 0.
  quantityVsModule->SetDirectory(0);
  quantityVsModule->SetStats(false);
  Double_t nEntries = 0;
  Double_t content = 0;
  Double_t contentsSum = 0;
  Double_t contentError = 0;
  Double_t sumw2 = 0;
  for (Int_t iModuleBin = 1; iModuleBin <= 64; iModuleBin++) {
    contentsSum = 0;
    nEntries = 0;
    sumw2 = 0;
    for (Int_t iChannelBin = 1; iChannelBin <= 48; iChannelBin++) {
      Int_t bin = quantityVsModuleAndChannel->GetBin(iModuleBin, iChannelBin);
      content = quantityVsModuleAndChannel->GetBinContent(bin);
      Double_t entries = quantityVsModuleAndChannel->GetBinEntries(bin);
      contentsSum += (content * entries);
      nEntries += entries;
      contentError = quantityVsModuleAndChannel->GetBinError(bin);
      sumw2 += contentError * contentError * entries;
    }
    if (nEntries != 0) {
      quantityVsModule->SetBinContent(iModuleBin, (contentsSum / nEntries));
      quantityVsModule->SetBinError(iModuleBin, (sqrt(sumw2) / nEntries));
    }
  }
  quantityVsModule->SetXTitle("Modules");
  SetQuantityYTitleFor(quantityVsModule);
  return quantityVsModule;
}

void TileOFA::MakeAllQuantitiesAsTH2D(void) {
  for (Int_t iPartition = LBA; iPartition <= EBC; iPartition++) {
    TProfile2D* quantityAsProfile2D;
    TH2* quantityAsTH2D;
    TObjArray* histsProf2d = histsProf2D_[iPartition];
    TObjArray* histsTH2D = new TObjArray(4);
    Int_t nHistsProf2d = histsProf2d->GetLast() + 1;
    for (Int_t iHistProf2d = 0; iHistProf2d < nHistsProf2d; iHistProf2d++) {
      quantityAsProfile2D = (TProfile2D*) (*histsProf2d)[iHistProf2d];
      TString* name = new TString(quantityAsProfile2D->GetName());
      name->Append("_pxy");
      quantityAsTH2D = quantityAsProfile2D->ProjectionXY(name->Data());
      quantityAsTH2D->SetDirectory(0);
      quantityAsTH2D->SetXTitle("Module");
      quantityAsTH2D->SetYTitle("Channel");
      quantityAsTH2D->SetStats(false);
      histsTH2D->Add(quantityAsTH2D);
    }
    histsTH2D_[iPartition]->AddAll(histsTH2D);
    histsTH2D->Clear();
  }
}



void TileOFA::SetQuantityYTitleFor(TH1* histogram) {
  TString histogramName = histogram->GetName();
  if (histogramName.Contains("Energy")) {
    TString quantityYTitle("Energy, ");
    quantityYTitle.Append(energyUnitsName_);
    histogram->SetYTitle(quantityYTitle);
  } else if (histogramName.Contains("Time")) {
    histogram->SetYTitle("Time, ns");
  } else if (histogramName.Contains("Pedestal")) {
    histogram->SetYTitle("Pedestal, ADC count");
  } else if (histogramName.Contains("Chi2")) {
    histogram->SetYTitle("#Chi^{2}");
  } else {
    histogram->SetYTitle("Unknown");
  }
  histogram->GetYaxis()->SetTitleOffset(1.3);
}

TString TileOFA::GetQuantityNameFrom(TH1* histogram) {
  TString quantityName;
  TString histogramName = histogram->GetName();
  if (histogramName.Contains("Energy")) {
    quantityName = "Energy";
  } else if (histogramName.Contains("Time")) {
    quantityName = "Time";
  } else if (histogramName.Contains("Pedestal")) {
    quantityName = "Pedestal";
  } else if (histogramName.Contains("Chi2")) {
    quantityName = "Chi2";
  } else {
    quantityName = "Unknown";
  }
  return quantityName;
}

void TileOFA::LogMessage(LogLevel messageType, TString message) {
  TString algorithmName(algorithmName_);
  while (algorithmName.Sizeof() < 12) {
    algorithmName.Prepend(' ');
  }
  TString resultMessage = algorithmName + ":\t" + gainName_ + " gain" + ":\t"
      + message;
  switch (messageType) {
    case DEBUG:
      logger_->Debug(resultMessage);
      break;
    case INFO:
      logger_->Info(resultMessage);
      break;
    case ERROR:
      logger_->Error(resultMessage);
      break;
  }
}

//*****************************************************************************
//                     Definition of class "TileOfaCollection"
//*****************************************************************************


TileOfaCollection::TileOfaCollection(Int_t initialSize) {
  capacity_ = initialSize;
  algorithms_ = new TileOfaInterface*[initialSize];
  size_ = 0;
  logger_ = SimpleLogger::GetLogger();
}

TileOfaCollection::~TileOfaCollection(void) {
  Int_t histogramsNumber = size_;
  for (Int_t iAlgorithm = 0; iAlgorithm < histogramsNumber; iAlgorithm++) {
    delete algorithms_[iAlgorithm];
    --size_;
  }
  delete[] algorithms_;
}

void TileOfaCollection::Add(TileOfaInterface* algorithm) {
  if (size_ == capacity_) {
    Int_t capacity = capacity_ * 2;
    TileOfaInterface** algorithms = new TileOfaInterface*[capacity];
    for (Int_t i = 0; i < size_; i++) {
      algorithms[i] = algorithms_[i];
    }
    delete[] algorithms_;
    algorithms_ = algorithms;
    capacity_ = capacity;
    TString debugMessage;
    debugMessage.Form(
        "Capacity of collection has been expanded, capacity: %i, size: %i",
        capacity_, size_);
    logger_->Debug(debugMessage);
  }
  if (size_ < capacity_) {
    algorithms_[size_] = algorithm;
    ++size_;
  } else {
    TString debugMessage;
    debugMessage.Form(
        "Collection is too small to add element, capacity: %i, size: %i",
        capacity_, size_);
    logger_->Debug(debugMessage);
  }
}

void TileOfaCollection::SetEnergyUniGainThreshold(
    Float_t energyUniGainThreshold) {
  for (Int_t iAlgorithm = 0; iAlgorithm < size_; iAlgorithm++) {
    algorithms_[iAlgorithm]->SetEnergyUniGainThreshold(energyUniGainThreshold);
  }
}

void TileOfaCollection::SetEnergyLowGainThreshold(
    Float_t energyLowGainThreshold) {
  for (Int_t iAlgorithm = 0; iAlgorithm < size_; iAlgorithm++) {
    algorithms_[iAlgorithm]->SetEnergyLowGainThreshold(energyLowGainThreshold);
  }
}

void TileOfaCollection::SetEnergyHighGainThreshold(
    Float_t energyHighGainThreshold) {
  for (Int_t iAlgorithm = 0; iAlgorithm < size_; iAlgorithm++) {
    algorithms_[iAlgorithm]->SetEnergyHighGainThreshold(energyHighGainThreshold);
  }
}

void TileOfaCollection::UseBadChannelStatus(Bool_t useChStatus) {
  for (Int_t iAlgorithm = 0; iAlgorithm < size_; iAlgorithm++) {
    algorithms_[iAlgorithm]->UseBadChannelStatus(useChStatus);
  }
}

void TileOfaCollection::UsePedestalToMaskChannel(Bool_t usePedToMaskCh) {
  for (Int_t iAlgorithm = 0; iAlgorithm < size_; iAlgorithm++) {
    algorithms_[iAlgorithm]->UsePedestalToMaskChannel(usePedToMaskCh);
  }
}

void TileOfaCollection::SetBadModules(TObjArray* modules) {
  for (Int_t iAlgorithm = 0; iAlgorithm < size_; iAlgorithm++) {
    algorithms_[iAlgorithm]->SetBadModules(modules);
  }
}

void TileOfaCollection::MakeAndFillRttHistogramsFromFile(TFile* inputFiel) {
  for (Int_t iAlgorithm = 0; iAlgorithm < size_; iAlgorithm++) {
    algorithms_[iAlgorithm]->MakeAndFillRttHistogramsFromFile(inputFiel);
  }
}

void TileOfaCollection::SaveRttHistogramsToFile(TFile* outputFile) {
  for (Int_t iAlgorithm = 0; iAlgorithm < size_; iAlgorithm++) {
    algorithms_[iAlgorithm]->SaveRttHistogramsToFile(outputFile);
  }
}
