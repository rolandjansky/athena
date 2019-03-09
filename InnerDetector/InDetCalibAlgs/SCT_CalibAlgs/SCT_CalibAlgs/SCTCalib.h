/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCTCalib.h
 *
 * Header file for the SCTCalib class
 *
 * @author Jose E. Garcia, jose.enrique.garcia@cern.ch
 * @author Peter Vankov,   peter.vankov@cern.ch
 * @author Kazu Hanagaki,  kazunori.hanagaki@cern.ch
 * @author Minoru Hirose,  minoru.hirose@cern.ch
 * @author Tim Andeen,     timothy.robert.andeen@cern.ch
 * @author Junji Tojo,     junji.tojo@cern.ch
 **/

#ifndef SCTCalib_H
#define SCTCalib_H

// Local
#include "SCT_CalibAlgs/ISCT_CalibHistoTool.h"
#include "SCT_CalibAlgs/ISCT_CalibEvtInfo.h"
#include "SCT_CalibAlgs/ISCT_CalibModuleListTool.h"
#include "SCT_CalibAlgs/SCTCalibWriteTool.h" //template parameter

//Athena
#include "AthenaBaseComps/AthAlgorithm.h"  //baseclass
#include "AthenaKernel/IOVTime.h" //member
#include "Identifier/Identifier.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/StoreGateSvc.h"

//InnerDetector
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

// SCT Conditions
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "SCT_ConditionsTools/ISCT_ReadCalibDataTool.h"
#include "SCT_ConditionsTools/ISCT_DetectorLevelConditionsTool.h"

// SCT Cabling
#include "SCT_Cabling/ISCT_CablingTool.h"

// Gaudi
#include "GaudiKernel/ToolHandle.h" //member

// STL and boost headers
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>

//Forward declarations
class TFile;
class TProfile;
class TProfile2D;
class ISvcLocator;
class StatusCode;

class SCTCalib : public AthAlgorithm {

 public:
  SCTCalib( const std::string &name, ISvcLocator* pSvcLocator ) ;
  ~SCTCalib() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode beginRun() override;
  virtual StatusCode execute() override;
  virtual StatusCode endRun() override;
  virtual StatusCode finalize() override;

 private:
  // SCT specific numbers
  enum { nbins       = 6*128, firstStrip     =  0, lastStrip     = nbins-1,
         ntimeBins   =     3, firstTimeBin   =  0, lastTimeBin   =  2,
         n_hitBins   =    50, first_hitBin   =  0, last_hitBin   = 50,
         n_etaBins   =    13, first_etaBin   = -6, last_etaBin   =  6,
         n_phiBins   =    56, first_phiBin   =  0, last_phiBin   = 55,
         n_barrels   =     4, first_barrel   =  0, last_barrel   =  3,
         n_disks     =     9, first_disk     =  0, last_disk     =  8,
         n_etaBinsEC =     3, first_etaBinEC =  0, last_etaBinEC =  2,
         n_phiBinsEC =    52, first_phiBinEC =  0, last_phiBinEC = 51
  };

  // Additional info on SCT specific numbers
  enum { n_chipPerSide = 6, n_chipPerModule = 12, n_stripPerChip = 128, n_etaInBarrel = 12,
         n_phiBinsB0 = 32, n_phiBinsB1 = 40, n_phiBinsB2 = 48, n_phiBinsB3 = 56,
         n_phiBinsECShort = 40, n_phiBinsECMiddle = 40, n_phiBinsECOuter = 52, n_elements=8176,
         n_BSErrorType = 15, firstBSErrorType = 0, lastBSErrorType = 14
  };

  const SCT_ID*                                               m_pSCTHelper;
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  SG::ReadHandleKey<EventInfo>                                m_eventInfoKey{this, "EventInfoKey", "ByteStreamEventInfo"};

  ToolHandle<SCTCalibWriteTool>                   m_pCalibWriteTool{this, "SCTCalibWriteTool", "SCTCalibWriteTool", "Tool to write out data from calibration loop"};
  ToolHandle<ISCT_ConfigurationConditionsTool>    m_ConfigurationConditionsTool{this, "SCT_ConfigurationConditionsTool", "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration"};
  ToolHandle<ISCT_ReadCalibDataTool>              m_ReadCalibDataTool{this, "SCT_ReadCalibDataTool", "SCT_ReadCalibDataTool/InDetSCT_ReadCalibDataTool", "Tool to retrieve SCT calibration data"};
  ToolHandle<ISCT_DetectorLevelConditionsTool>    m_MajorityConditionsTool{this, "SCT_MajorityConditionsTool", "SCT_MajorityConditionsTool", "Tool to retrieve the majority conditions of SCT"};
  ToolHandle<ISCT_CablingTool>                    m_CablingTool{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};
  ToolHandle<ISCT_CalibHistoTool>                 m_calibHitmapTool{this, "SCT_CalibHitMapTool", "SCT_CalibHitMapTool", "Tool for reading and writing HitMap histograms"};
  ToolHandle<ISCT_CalibHistoTool>                 m_calibLbTool{this, "SCT_CalibLbTool", "SCT_CalibLbTool", "Tool for reading and writing Lb dependent HitMap histograms"};
  ToolHandle<ISCT_CalibHistoTool>                 m_calibBsErrTool{this, "SCT_CalibBsErrTool", "SCT_CalibBsErrTool", "Tool for reading and writing BS error histograms"};
  ToolHandle<ISCT_CalibModuleListTool>            m_calibModuleListTool{this, "SCT_CalibModuleListTool", "SCT_CalibModuleListTool", "Tool for handling list of modules"};
  ToolHandle<ISCT_CalibEvtInfo>                   m_calibEvtInfoTool{this, "SCT_CalibEvtInfo", "SCT_CalibEvtInfo", "Tool for handling event info"};

  // For ntuple sorting
  // For HV trips
  std::vector< std::vector< std::pair<int, int> > > m_summarytrips;
  std::vector< std::vector< std::pair<int, int> > > m_summarytripslb;

  std::ofstream m_gofile;

  // output files (for dead search & MONP200 related things )
  std::ofstream m_outDeadStrips;
  std::ofstream m_outDeadChips;
  std::ofstream m_outDeadSummary;
  std::ofstream m_outNOSummary;
  std::ofstream m_outROSummary;
  std::ofstream m_outEffSummary;
  std::ofstream m_outBSErrSummary;
  std::ofstream m_outBSErrModule;
  std::ofstream m_outLASummary;

  // Properties configurable from jobO
  IntegerProperty m_runNumber{this, "RunNumber", -1};

  StringProperty m_runStartTime{this, "RunStartTime", "", "from runInfo.txt"};
  StringProperty m_runEndTime{this, "RunEndTime", "", "from runInfo.txt"};
  StringProperty m_LBMax{this, "LBMax", "1000", "from runInfo.txt"};

  BooleanProperty m_useConfiguration{this, "UseConfiguration", true};
  BooleanProperty m_useCalibration{this, "UseCalibration", true};
  BooleanProperty m_useMajority{this, "UseMajority", true};
  BooleanProperty m_useBSError{this, "UseBSError", false};

  StringArrayProperty m_input_hist{this, "InputHist", {}, "list of HIST (assuming 1 file)"};

  BooleanProperty m_readBS{this, "ReadBS", false, "True if BS is used"};
  BooleanProperty m_histBefore2010{this, "HistBefore2010", false, "True if HIST is from 2009 or earlier"};

  BooleanProperty m_doHitMaps{this, "DoHitMaps", true};
  IntegerProperty m_nLbsMerged{this, "LbsPerWindow", 20};
  BooleanProperty m_readHitMaps{this, "ReadHitMaps", false};
  BooleanProperty m_doBSErrors{this, "DoBSErrors", false};
  BooleanProperty m_doNoisyStrip{this, "DoNoisyStrip", true};
  BooleanProperty m_doNoisyLB{this, "DoNoisyLB", true};
  BooleanProperty m_doHV{this, "DoHV", false};
  BooleanProperty m_doDeadStrip{this, "DoDeadStrip", false};
  BooleanProperty m_doDeadChip{this, "DoDeadChip", false};
  BooleanProperty m_doNoiseOccupancy{this, "DoNoiseOccupancy", false};
  BooleanProperty m_doRawOccupancy{this, "DoRawOccupancy", false};
  BooleanProperty m_doEfficiency{this, "DoEfficiency", false};
  BooleanProperty m_doBSErrorDB{this, "DoBSErrorDB", false};
  BooleanProperty m_doLorentzAngle{this, "DoLorentzAngle", false};
  BooleanProperty m_writeToCool{this, "WriteToCool", true};

  BooleanProperty m_noisyUpdate{this, "NoisyUpdate", true};
  BooleanProperty m_noisyUploadTest{this, "NoisyUploadTest", true};
  FloatProperty   m_noisyModuleAverageInDB{this, "NoisyModuleAverageInDB", -1., "Average number of modules with at least 1 noisy strip in COOL"};
  IntegerProperty m_noisyStripLastRunInDB{this, "NoisyStripLastRunInDB", -999, "Number of noisy strips of the last run uploaded to COOL"};
  FloatProperty   m_noisyStripAverageInDB{this, "NoisyStripAverageInDB", -999., "Number of noisy strips of the last run uploaded to COOL"};
  IntegerProperty m_noisyModuleList{this, "NoisyModuleList", 200};
  FloatProperty   m_noisyModuleDiff{this, "NoisyModuleDiff", 0.200};
  FloatProperty   m_noisyStripDiff{this, "NoisyStripDiff", 128.};

  UnsignedIntegerProperty m_noisyMinStat{this, "NoisyMinStat", 50000};
  BooleanProperty         m_noisyStripAll{this, "NoisyStripAll", true};
  BooleanProperty         m_noisyStripThrDef{this, "NoisyStripThrDef", true};
  FloatProperty           m_noisyStripThrOffline{this, "NoisyStripThrOffline", 1.500E-2};
  FloatProperty           m_noisyStripThrOnline{this, "NoisyStripThrOnline", 1.500E-3};
  BooleanProperty         m_noisyWaferFinder{this, "NoisyWaferFinder", true};
  BooleanProperty         m_noisyWaferWrite{this, "NoisyWaferWrite", true};
  BooleanProperty         m_noisyWaferAllStrips{this, "NoisyWaferAllStrips", true};
  FloatProperty           m_noisyWaferThrBarrel{this, "NoisyWaferThrBarrel", 1.000E-4};
  FloatProperty           m_noisyWaferThrECA{this, "NoisyWaferThrECA", 1.000E-4};
  FloatProperty           m_noisyWaferThrECC{this, "NoisyWaferThrECC", 1.000E-4};
  FloatProperty           m_noisyWaferFraction{this, "NoisyWaferFraction", 0.500};
  FloatProperty           m_noisyChipFraction{this, "NoisyChipFraction", 0.500};

  IntegerProperty m_maxtbins{this, "HVBinWidth", 5};

  UnsignedIntegerProperty m_deadStripMinStat{this, "DeadStripMinStat", 200000};
  UnsignedIntegerProperty m_deadStripMinStatBusy{this, "DeadStripMinStatBusy", 1600000};
  UnsignedIntegerProperty m_deadChipMinStat{this, "DeadChipMinStat", 200000};
  UnsignedIntegerProperty m_deadStripSignificance{this, "DeadStripSignificance", 10};
  UnsignedIntegerProperty m_deadChipSignificance{this, "DeadChipSignificance", 10};
  FloatProperty           m_busyThr4DeadFinding{this, "BusyThr4DeadFinding", 1E-4};
  FloatProperty           m_noisyThr4DeadFinding{this, "NoisyThr4DeadFinding", 1.500E-3};
  BooleanProperty         m_deadChipUploadTest{this, "DeadChipUploadTest", true};
  BooleanProperty         m_deadStripUploadTest{this, "DeadStripUploadTest", true};

  BooleanProperty         m_noiseOccupancyTriggerAware{this, "NoiseOccupancyTriggerAware", true};
  UnsignedIntegerProperty m_noiseOccupancyMinStat{this, "NoiseOccupancyMinStat", 50000};
  UnsignedIntegerProperty m_rawOccupancyMinStat{this, "RawOccupancyMinStat", 50000};
  UnsignedIntegerProperty m_efficiencyMinStat{this, "EfficiencyMinStat", 50000};
  UnsignedIntegerProperty m_BSErrorDBMinStat{this, "BSErrorDBMinStat", 50000};
  UnsignedIntegerProperty m_LorentzAngleMinStat{this, "LorentzAngleMinStat", 50000};

  BooleanProperty  m_LorentzAngleDebugMode{this, "LorentzAngleDebugMode", true};

  StringProperty m_tagID4NoisyStrips{this, "TagID4NoisyStrips", "SctDerivedMonitoring-001-00"};
  StringProperty m_tagID4DeadStrips{this, "TagID4DeadStrips", "SctDerivedDeadStrips-001-00"};
  StringProperty m_tagID4DeadChips{this, "TagID4DeadChips", "SctDerivedDeadChips-001-00"};
  StringProperty m_tagID4NoiseOccupancy{this,  "TagID4NoiseOccupancy", "SctDerivedNoiseOccupancy-001-00"};

  StringProperty m_badStripsAllFile{this, "BadStripsAllFile", "BadStripsAllFile.xml", "Output XML for all noisy strips"};
  StringProperty m_badStripsNewFile{this, "BadStripsNewFile", "BadStripsNewFile.xml", "Output XML for newly found noisy strips (i.e. not in config/calib data)"};
  StringProperty m_badStripsSummaryFile{this, "BadStripsSummaryFile", "BadStripsSummaryFile.xml", "Output XML for summary info from noisy strip search"};

  StringProperty m_badModulesFile{this, "BadModulesFile", "BadModulesFile.xml", "Output XML for HV trips"};

  StringProperty m_deadStripsFile{this, "DeadStripsFile", "DeadStripsFile.xml", "Output XML for dead strips"};
  StringProperty m_deadChipsFile{this, "DeadChipsFile", "DeadChipsFile.xml", "Output XML for dead chips"};
  StringProperty m_deadSummaryFile{this, "DeadSummaryFile", "DeadSummaryFile.xml", "Output XML for summary of Dead Finding"};

  StringProperty m_noiseOccupancyFile{this, "NoiseOccupancyFile", "NoiseOccupancyFile.xml", "Output XML for noise occupancy"};
  StringProperty m_noiseOccupancySummaryFile{this, "NoiseOccupancySummaryFile", "NoiseOccupancySummaryFile.xml", "Output XML for summary of noise occupancy"};
  StringProperty m_rawOccupancySummaryFile{this,"RawOccupancySummaryFile", "RawOccupancySummaryFile.xml", "Output XML for summary of raw occupancy"};
  StringProperty m_efficiencySummaryFile{this, "EfficiencySummaryFile", "EfficiencySummaryFile.xml", "Output XML for summary of efficiency"};
  StringProperty m_efficiencyModuleFile{this, "EfficiencyModuleFile", "EfficiencyModuleSummary.xml", "Output XML for efficiency"};
  StringProperty m_BSErrorSummaryFile{this, "BSErrorSummaryFile", "BSErrorSummaryFile.xml", "Output XML for summary of BS Errors"};
  StringProperty m_BSErrorModuleFile{this, "BSErrorModuleFile", "BSErrorModuleSummary.xml", "Output XML for summary of BS Errors"};
  StringProperty m_LorentzAngleFile{this, "LorentzAngleFile", "LorentzAngleFile.xml", "Output XML for noise occupancy"};
  StringProperty m_LorentzAngleSummaryFile{this, "LorentzAngleSummaryFile", "LorentzAngleSummaryFile.xml", "Output XML for summary of lorentz angle"};

  // Event related
  int             m_numOfLBsProcessed;
  unsigned long long m_numberOfEvents;
  unsigned long long m_numberOfEventsHist; // For number of events from HIST
  std::string        m_utcBegin;
  std::string        m_utcEnd;
  int                m_LBRange;
  IOVTime            m_iovStart;
  IOVTime            m_iovStop;

  // Input
  TFile*  m_inputHist;    // Monitoring histograms
  bool    m_readHIST;   // True if HIST is used

  // Histograms
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVector;
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorECp;
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorECm;
  std::vector<TProfile*>   m_h_phiVsNstripsSideHistoVector;

  // Book histograms

  // Methods implemented
  StatusCode prepareHV();
  StatusCode doHVPrintXML(const std::pair<int, int>& timeInterval, const std::pair<int, int>& lbRange, Identifier);

  bool       notEnoughStatistics(const int required, const int obtained, const std::string& histogramName="HIST") const;

  StatusCode getNoisyStrip();
  StatusCode getDeadStrip();
  StatusCode getNoiseOccupancy();
  StatusCode getRawOccupancy();
  StatusCode getEfficiency();
  StatusCode getBSErrors();
  StatusCode getLorentzAngle();

  // To handle XML file for DB
  StatusCode openXML4DB( std::ofstream&, const char*, const char*, IOVTime, IOVTime ) const;
  StatusCode closeXML4DB( std::ofstream& ) const;
  StatusCode addToXML4DB( std::ofstream&, const Identifier&, const char*, float, const char* ) const;

  // To handle XML file for Summary
  StatusCode openXML4MonSummary( std::ofstream&, const char* ) const;
  StatusCode openXML4DeadSummary( std::ofstream& file, const char* type, int n_Module=0, int n_Link=0, int n_Chip=0, int n_Strip=0 ) const;
  StatusCode wrapUpXML4Summary( std::ofstream&, const char*, std::ostringstream& ) const;
  StatusCode addToSummaryStr( std::ostringstream&, const Identifier&, const char*, const char*, const char* ) const;

  template<class S>
    bool retrievedService(S& service) {
    if (service.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Unable to retrieve "<<service.name());
      return false;
    }
    return true;
  }
  std::string
    xmlChannelNoiseOccDataString(const Identifier& waferId,  const float occupancy, const SCT_SerialNumber& serial) const;

  std::string
    xmlChannelEfficiencyDataString(const Identifier& waferId,  const float efficiency, const SCT_SerialNumber& serial) const;

  std::pair<int, bool>
    getNumNoisyStrips( const Identifier& waferId ) const;

  StatusCode
    addStripsToList( Identifier& waferId, std::set<Identifier>& stripIdList, bool isNoisy, bool isNew );

  StatusCode
    writeModuleListToCool( const std::map< Identifier, std::set<Identifier> >& moduleListAll,
                           const std::map< Identifier, std::set<Identifier> >& moduleListNew,
                           const std::map< Identifier, std::set<Identifier> >& moduleListRef );
  std::string
    getStripList( const std::set<Identifier>& stripIdList ) const;

  StatusCode
    noisyStripsToXml( const std::map< Identifier, std::set<Identifier> >& moduleList, const std::string& badStripsFile ) const;

  StatusCode
    noisyStripsToSummaryXml( const std::map< Identifier, std::set<Identifier> >& moduleListAll,
                             const std::map< Identifier, std::set<Identifier> >& moduleListNew,
                             const std::map< Identifier, std::set<Identifier> >& moduleListRef,
                             const std::string& badStripsFile) const;

  std::set<int>
    getNoisyChips( const std::set<Identifier>& stripIdList ) const;

  std::pair< std::string, float >
    getNoisyLB( const Identifier& moduleId, int& chipId ) const;

  std::string
    getLBList( const std::set<int>& LBList ) const;
}; // end of class

#endif // SCTCalib_H
