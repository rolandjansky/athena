// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LeptonTriggerSF.h 564936 2013-10-10 17:46:00Z schae $
#ifndef TrigMuonEfficiency_LeptonTriggerSF_H
#define TrigMuonEfficiency_LeptonTriggerSF_H

/** 
 * @author  Junjie Zhu (junjie@umich.edu) [original stand-alone code]
 * @author  Marilyn Marx (marx@cern.ch)   [major changes for the TrigMuonEfficiency package / updates]
 * @author  Takashi Matsushita (takashi@cern.ch)   [addition of functionalities / code refactoring]
 * 
 *
 * How to use:
 *
 * pair<double, double>
 * GetTriggerSF(int runnumber, bool useGeV, vector<TLorentzVector> muons, MuonQuality q, vector<TLorentzVector> electrons, ElectronQuality p, int var)
 *
 * where runnumber is an int associating to a real data runnumber in mc,                                
 * useGeV is a bool you set depending on whether quantities are in MeV (useGeV=false) or GeV (useGeV=true) in your code,
 * vector<TLorentzVector> should be filled (separately for muons and electrons) with all good leptons passing your object selection cuts,
 * (N.B. make sure CLUSTER eta is used in the electron TLorentzVector)
 * MuonQuality is an enum, for q enter either loose or combined,
 * ElectronQuality is an enum, for p enter either loosepp, mediumpp, tightpp or ML (multilepton).
 * var is an int for systematic studies, leave out or set to 0 for central SF, set to 1 or 2 for +1 or -1 sigma variation on the SF
 *
 * GetTriggerSF().first returns the event SF
 * GetTriggerSF().second returns the uncertainty (stat and syst) on the event SF
 *
 * Note for analyses that use different lepton qualities for different leptons, use:
 * pair<double, double>
 * GetTriggerSF(int runnumber, bool useGeV, vector<TLorentzVector> muons, vector<MuonQuality> vecq, vector<TLorentzVector> electrons, vector<ElectronQuality> vecp, int var)
 */

// STL include(s):
#include <vector>
#include <map>
#include <string>

// Forward declaration(s):
class TLorentzVector;
class TH2;
class TFile;
class TDirectory;

namespace Root {
  class TElectronEfficiencyCorrectionTool;
}


namespace TrigMuonEff {

  /**
   *  strcuture to hold variables to configure behaviour of the tool
   */
  struct Configuration {
    mutable bool isData;
    bool isAFII;          /**<  use AFII scale factor for electron trigger or not */
    bool useGeV;          /**<  use a unit of GeV or not */
    bool setByUser;       /**<  true if the structure is set by user */
    int replicaIndex;     /**<  index of replicated histograms to access */
    int runNumber;        /**<  run number */
    int systematics;      /**<  flag for systematics */
    std::string trigger;  /**<  trigger */
    std::string period;   /**<  data taking period */
    std::string binning;  /**<  binning of the scale factor map */

    Configuration(const bool isData_ = true,
                  const bool isAFII_ = false,
                  const bool useGeV_ = false,
                  const bool setByUser_ = false,
                  const int replicaIndex_ = -1,
                  const int runNumber_ = 0,
                  const int systematics_ = 0,
                  const std::string& trigger_ = "",
                  const std::string& period_ = "",
                  const std::string& binning_ = "") :
      isData(isData_),
      isAFII(isAFII_),
      useGeV(useGeV_),
      setByUser(setByUser_),
      replicaIndex(replicaIndex_),
      runNumber(runNumber_),
      systematics(systematics_),
      trigger(trigger_), 
      period(period_),
      binning(binning_) {}
  };


  /**
   *  Helper enumeration defining the data periods understood by the tool
   */
  typedef enum DataPeriod {
    period_undefined = -1,            perUnDefined      = -1,
    period_2011_BtoI =  0,            per2011B_I        = 0, 
    period_2011_JtoMwithoutL3L4 = 1,  per2011J_MwoL3_L4 = 1, 
    period_2011_L3toL4 = 2,           per2011L3_L4      = 2,
    
    //for HSG3 specific use
    per2011J   = 3,
    per2011K   = 4,
    per2011J_K = 5,
    per2011J_M = 6,
    per2011L_M = 7,
    
    // 2012
    period_2012_A = 8,      per2012A = 8,
    period_2012_B = 9,      per2012B = 9,
    period_2012_CtoE = 10,  per2012CtoE = 10,
    period_2012_G = 11,  per2012G = 11,
    period_2012_HtoL = 12,  per2012HtoL = 12
  } SFDataPeriod;
}



/**
 * Helper enumeration defining the muon quality types
 */
typedef enum MuonQuality {
  loose    = 0,
  combined = 1,
  CaloMuon = 2
} muon_quality;

/**
 * Helper enumeration defining the electron quality types
 */
typedef enum ElectronQuality {
  loosepp = 0,      /**< Loose++ */
  mediumpp = 1,     /**< Medium++ */
  tightpp = 2,      /**< Tight++ */
  MultiLepton = 3,  /**< MultiLepton */
  ML = 3,           /**< alias for MultiLepton */
  LooseLLH = 4,     /**< Likelihood loose  */
  VeryTightLLH = 5  /**< Likelihood very tight */
} electron_quality;

/**
 * Helper enumeration defining the muon quality types
 */
typedef enum DileptonTriggerName {
  kNoTrig    = 0,
  EF_e12Tvh_medium1_mu8    = 1,
  EF_2e12Tvh_loose1 = 2,
  EF_mu18_mu8_EFFS = 3
} dilepton_trigger_name;

/**
 * Helper enumeration for systematics study
 */
enum SystematicVariation {
  noVariation = 0,            /**< no variation */
  plusOneSigma = 1,           /**< +1 sigma: where sigma is the full error on the SF */
  minusOneSigma = 2,          /**< -1 sigma: where sigma is the full error on the SF */
  plusOneSigmaElectron = 3,   /**< +1 sigma: where sigma is the error on the SF only due to electron trigger contributions */
  minusOneSigmaElectron = 4,  /**< -1 sigma: where sigma is the error on the SF only due to electron trigger contributions */
  plusOneSigmaMuon = 5,       /**< +1 sigma: where sigma is the error on the SF only due to muon trigger contributions */
  minusOneSigmaMuon = 6,      /**< -1 sigma: where sigma is the error on the SF only due to muon trigger contributions */
  statisticalOnlyMuon = 7,    /**< statistical component of uncertainty from muons are used for total uncertainty */
  systematicOnlyMuon = 8      /**< systematic component of uncertainty from muons are used for total uncertainty */
};



/**
 *  @short Class providing trigger scale factors for analyses using electrons and muons
 *
 *         This tool can be used to get the scale factors and their uncertainties in
 *         events with multiple electrons/muons.
 *
 * $Revision: 564936 $
 * $Date: 2013-10-10 19:46:00 +0200 (Thu, 10 Oct 2013) $
 */
class LeptonTriggerSF {
public:
  struct Storage {
    const std::vector<std::string> *type;
    const std::vector<std::string> *region;
    const std::vector<std::string> *quality;
    const std::vector<std::string> *bins;
    const std::vector<std::string> *trigger;
    const std::vector<std::string> *period;

    Storage(const std::vector<std::string> *type_=0,
            const std::vector<std::string> *region_=0,
            const std::vector<std::string> *quality_=0,
            const std::vector<std::string> *bins_=0,
            const std::vector<std::string> *trigger_=0,
            const std::vector<std::string> *period_=0) :
      type(type_),
      region(region_),
      quality(quality_),
      bins(bins_),
      trigger(trigger_),
      period(period_) {}
  };

  /** constructor
   *  The constructor expects the directory name under which the efficiency file(s) can
   *  be found, when running in stand-alone ROOT mode. When running in an Athena job it
   *  can try to guess the correct location, but it may need some help even in that case.
   *
   *  The directory can be any directory that ROOT understands, so you can point the tool to a
   *  DCAP, XRootD or even a web server if you want to.
   *
   *  @param directory [in] path to the directory that contains efficiency file(s)
   *  @param directoryElectron [in] path to the directory that contains efficiency files for ElectronEfficiencyCorrection
   *  @param electronFileVersion [in] version string of the efficiency files for ElectronEfficiencyCorrection
   */
  LeptonTriggerSF(const std::string& directory = "",
                  const std::string& directoryElectron = "",
                  const std::string& electronFileVersion = "rel17p2.v02");

  /** constructor
   *  @param year [in] year in 4 digits
   *  @param directory [in] path to the directory that contains efficiency file(s)
   *  @param fileName [in] name of the ROOT file
   *  @param directoryElectron [in] path to the directory that contains efficiency files for ElectronEfficiencyCorrection
   *  @param electronFileVersion [in] version string of the efficiency files for ElectronEfficiencyCorrection
   */
  LeptonTriggerSF(const int year,
                  const std::string& directory = "",
                  const std::string& fileName = "",
                  const std::string& directoryElectron = "",
                  const std::string& electronFileVersion = "rel17p2.v02");

  /** constructor
   *  @param year [in] year in 4 digits
   *  @param quality [in] year in 4 digits
   *  @param directory [in] path to the directory that contains efficiency file(s)
   *  @param fileName [in] name of the ROOT file
   *  @param directoryElectron [in] path to the directory that contains efficiency files for ElectronEfficiencyCorrection
   *  @param electronFileVersion [in] version string of the efficiency files for ElectronEfficiencyCorrection
   */
  LeptonTriggerSF(const int year,
                  const std::vector<std::string>& qualities,
                  const std::string& directory = "",
                  const std::string& fileName = "",
                  const std::string& directoryElectron = "",
                  const std::string& electronFileVersion = "rel17p2.v02");

  /** Destructor */
  ~LeptonTriggerSF();



  /** get the trigger scale factor for muons of uniform quality
    *
    * @param runNumber [in] run number
    * @param useGeV [in] use GeV if true, otherwise use MeV
    * @param muons [in] TLorentzVector array of muons
    * @param muonQuality [in] muon quality
    * @param systematicVariation [in] systematic variation
    * @param configuration [in] configuration
    * @return pair->first : scale factor, pair->second : uncertainty on the scale factor
    */
  std::pair<double, double>
  GetTriggerSF(const int runNumber,
               const bool useGeV,
               const std::vector<TLorentzVector>& muons,
               const MuonQuality muonQuality,
               const int systematicVariation = 0,
               TrigMuonEff::Configuration* configuration = 0) const;



  /** get the trigger scale factor for muons with non-uniform quality
    *
    * @param runNumber [in] run number
    * @param useGeV [in] use GeV if true, otherwise use MeV
    * @param muons [in] TLorentzVector array of muons
    * @param muonsQuality [in] array of muon qualities
    * @param systematicVariation [in] systematic variation
    * @param configuration [in] configuration
    * @return pair->first : scale factor, pair->second : uncertainty on the scale factor
    */
  std::pair<double, double>
  GetTriggerSF(const int runNumber,
               const bool useGeV,
               const std::vector<TLorentzVector>& muons,
               const std::vector<MuonQuality>& muonsQuality,
               const int systematicVariation = 0,
               TrigMuonEff::Configuration* configuration = 0) const;



  /** get the trigger scale factor for muons with non-uniform quality for given trigger
    *
    * @param runNumber [in] run number
    * @param useGeV [in] use GeV if true, otherwise use MeV
    * @param muons [in] TLorentzVector array of muons
    * @param muonsQuality [in] array of muon qualities
    * @param trigger [in] trigger chain
    * @param systematicVariation [in] systematic variation
    * @param configuration [in] configuration
    * @return pair->first : scale factor, pair->second : uncertainty on the scale factor
    */
  std::pair<double, double>
  GetTriggerSF(const int runNumber,
               const bool useGeV,
               const std::vector<TLorentzVector>& muons,
               const std::vector<MuonQuality>& muonsQuality,
               const std::string& trigger,
               const int systematicVariation = 0,
               TrigMuonEff::Configuration* configuration = 0) const;

/** get the trigger scale factor for electrons with non-uniform quality for given trigger
    *
    * @param runNumber [in] run number
    * @param useGeV [in] use GeV if true, otherwise use MeV
    * @param electrons [in] TLorentzVector array of electrons
    * @param electronsQuality [in] array of electron qualities
    * @param trigger [in] trigger chain
    * @param systematicVariation [in] systematic variation
    * @param configuration [in] configuration
    * @return pair->first : scale factor, pair->second : uncertainty on the scale factor
    */
  std::pair<double, double>
  GetTriggerSF(const int runNumber,
               const bool useGeV,
               const std::vector<TLorentzVector>& electrons,
               const std::vector<ElectronQuality>& electronsQuality,
               const std::string& trigger,
               const int systematicVariation = 0,
               TrigMuonEff::Configuration* configuration = 0) const;

  /** get the trigger scale factor for leptons of uniform quality
    *
    * @param runNumber [in] run number
    * @param useGeV [in] use GeV if true, otherwise use MeV
    * @param muons [in] TLorentzVector array of muons
    * @param muonQuality [in] muon quality
    * @param electrons [in] TLorentzVector array of electrons
    * @param electronQuality [in] electron quality
    * @param systematicVariation [in] systematic variation
    * @param configuration [in] configuration
    * @return pair->first : scale factor, pair->second : uncertainty on the scale factor
    */
  std::pair<double, double>
  GetTriggerSF(const int runNumber,
               const bool useGeV,
               const std::vector<TLorentzVector>& muons,
               const MuonQuality muonQuality,
               const std::vector<TLorentzVector>& electrons,
               const ElectronQuality electronQuality,
               const int systematicVariation = 0,
               TrigMuonEff::Configuration* configuration = 0) const;



  /** get the trigger scale factor for leptons with non-uniform quality
    *
    * @param runNumber [in] run number
    * @param useGeV [in] use GeV if true, otherwise use MeV
    * @param muons [in] TLorentzVector array of muons
    * @param muonsQuality [in] array of muon qualities
    * @param electrons [in] TLorentzVector array of electron
    * @param electronsQuality [in] array of electron qualities
    * @param systematicVariation [in] systematic variation
    * @param configuration [in] configuration
    * @return pair->first : scale factor, pair->second : uncertainty on the scale factor
    */
  std::pair<double, double>
  GetTriggerSF(const int runNumber,
               const bool useGeV,
               const std::vector<TLorentzVector>& muons,
               const std::vector<MuonQuality>& muonsQuality,
               const std::vector<TLorentzVector>& electrons,
               const std::vector<ElectronQuality>& electronsQuality,
               const int systematicVariation = 0,
               TrigMuonEff::Configuration* configuration = 0) const;


  /** get the trigger scale factor for leptons with non-uniform quality
    *
    * @param runNumber [in] run number
    * @param useGeV [in] use GeV if true, otherwise use MeV
    * @param muons [in] TLorentzVector array of muons
    * @param muonsQuality [in] array of muon qualities
    * @param electrons [in] TLorentzVector array of electron
    * @param electronsQuality [in] array of electron qualities
    * @param trigger [in] trigger chain
    * @param systematicVariation [in] systematic variation
    * @param configuration [in] configuration
    * @return pair->first : scale factor, pair->second : uncertainty on the scale factor
    */
  std::pair<double, double>
  GetTriggerSF(const int runNumber,
               const bool useGeV,
               const std::vector<TLorentzVector>& muons,
               const std::vector<MuonQuality>& muonsQuality,
               const std::vector<TLorentzVector>& electrons,
               const std::vector<ElectronQuality>& electronsQuality,
	       const std::string& trigger,
               const int systematicVariation = 0,
               TrigMuonEff::Configuration* configuration = 0) const;

  /** get the trigger scale factor for leptons with non-uniform quality
    *
    * @param configuration [in] configuration
    * @param muons [in] TLorentzVector array of muons
    * @param muonsQuality [in] array of muon qualities
    * @param electrons [in] TLorentzVector array of electron
    * @param electronsQuality [in] array of electron qualities
    * @return pair->first : scale factor, pair->second : uncertainty on the scale factor
    */
  std::pair<double, double>
  GetTriggerSF(const TrigMuonEff::Configuration& configuration,
               const std::vector< TLorentzVector >& muons,
               const std::vector< MuonQuality >& muonsQuality,
               const std::vector< TLorentzVector >& electrons,
               const std::vector< ElectronQuality >& electronsQuality) const;



  /** get the di-muon trigger scale factor for di-muons with non-uniform quality
    *
    * @param configuration [in] configuration
    * @param muons [in] TLorentzVector array of muons
    * @param muonsQuality [in] array of muon qualities
    * @param trigger [in] trigger chain
    * @return pair->first : scale factor, pair->second : uncertainty on the scale factor
    */
  std::pair<double, double>
  GetTriggerSF(const TrigMuonEff::Configuration& configuration,
               const std::vector<TLorentzVector>& muons,
               const std::vector<MuonQuality>& muonsQuality,
               const std::string& trigger) const;

  /** get the di-electron trigger scale factor for di-electrons with non-uniform quality
    *
    * @param configuration [in] configuration
    * @param electrons [in] TLorentzVector array of electrons
    * @param electronsQuality [in] array of electron qualities
    * @param trigger [in] trigger chain
    * @return pair->first : scale factor, pair->second : uncertainty on the scale factor
    */
  std::pair<double, double>
  GetTriggerSF(const TrigMuonEff::Configuration& configuration,
               const std::vector<TLorentzVector>& electrons,
               const std::vector<ElectronQuality>& electronsQuality,
               const std::string& trigger) const;

  /** get the trigger efficiency for a single muon
    *
    * @param configuration [in] configuration
    * @param muon [in] TLorentzVector of muon
    * @param muonQuality [in] muon quality
    * @param trigger [in] trigger
    * @return pair->first : efficiency, pair->second : uncertainty on the efficiency
    */
  std::pair<double, double>
  getMuonEfficiency(const TrigMuonEff::Configuration& configuration,
                    const TLorentzVector& muon,
                    const MuonQuality muonQuality,
                    const std::string& trigger = "") const;

  std::pair<double, double>
  MuEff(const TrigMuonEff::Configuration& configuration,
        const TLorentzVector& muon,
        const MuonQuality muonQuality,
        const std::string& trigger = "") const
  {return getMuonEfficiency(configuration, muon, muonQuality, trigger);}



  /** get the trigger efficiency for a single muon
    *
    * @param period [in] data period
    * @param isData [in] set it to true if data, false for MC
    * @param muonQuality [in] muon quality
    * @return pair->first : efficiency, pair->second : uncertainty on the efficiency
    */
  std::pair<double, double>
  getMuonEfficiency(const TrigMuonEff::DataPeriod period,
                    const bool isData,
                    const TLorentzVector& muon,
                    const MuonQuality muonQuality) const
  {
    TrigMuonEff::Configuration config;
    config.isData = isData;

    setConfiguration(config, period);
    return getMuonEfficiency(config, muon, muonQuality);
  };

  std::pair<double, double>
  MuEff(const TrigMuonEff::DataPeriod period,
        const bool isData,
        const TLorentzVector& muon,
        const MuonQuality muonQuality) const
  {return getMuonEfficiency(period, isData, muon, muonQuality);}



  /** get the trigger efficiency for a single muon
    *
    * @param runNumber [in] run number
    * @param isData [in] true for data, false for MC
    * @param useGeV [in] use GeV if true, otherwise use MeV
    * @param muon [in] TLorentzVector of muon
    * @param muonQuality [in] muon quality
    * @param uncertainty [in] optional: statisticalOnlyMuon or systematicOnlyMuon
    * @param configuration [in] configuration
    * @return pair->first : muon trigger efficiency, pair->second : uncertainty on the trigger efficiency
    */
  std::pair<double, double>
  getMuonEfficiency(const int runNumber,
                    const bool isData,
                    const bool useGeV,
                    const TLorentzVector& muon,
                    const MuonQuality muonQuality,
                    const int uncertainty = 0,
                    TrigMuonEff::Configuration* configuration = 0) const
  {
    TrigMuonEff::Configuration config;
    config.runNumber = runNumber;
    config.isData = isData;
    config.useGeV = useGeV;
    config.setByUser = false;
    config.isAFII = m_IsAFII;
    config.systematics = uncertainty;

    if (configuration) {
      config.setByUser = true;
      config.trigger = configuration->trigger;
      config.period = configuration->period;
      config.binning = configuration->binning;

    } else {
      setConfiguration(config);
    }

    return getMuonEfficiency(config, muon, muonQuality);
  }



  /** get the di-muon trigger efficiency
    *
    * @param config [in] configuration
    * @param muons [in] array of TLorentzVector for muons
    * @param muonsQuality [in] array of muon quality
    * @param chain [in] trigger chain name
    * @return pair->first : dimuon trigger efficiency, pair->second : uncertainty on the trigger efficiency
    */
  std::pair<double, double>
  getDimuonEfficiency(const TrigMuonEff::Configuration& config,
                      const std::vector<TLorentzVector>& muons,
                      const std::vector<MuonQuality>& muonsQuality,
                      const std::string& chain) const;

 /** get the di-electron trigger efficiency
    *
    * @param config [in] configuration
    * @param muons [in] array of TLorentzVector for electrons
    * @param muonsQuality [in] array of electron quality
    * @param chain [in] trigger chain name
    * @return pair->first : dielectron trigger efficiency, pair->second : uncertainty on the trigger efficiency
    */
  std::pair<double, double>
  getDielectronEfficiency(const TrigMuonEff::Configuration& config,
                      const std::vector<TLorentzVector>& electrons,
                      const std::vector<ElectronQuality>& electronsQuality,
                      const std::string& chain) const;



  /** get the OR'ed efficiency of single and dimuon triggers
    *
    * @param config [in] configuration
    * @param muons [in] array of TLorentzVector for muons
    * @param muonsQuality [in] array of muon quality
    * @param chain [in] trigger chain name
    * @return pair->first : single or dimuon trigger efficiency, pair->second : uncertainty on the trigger efficiency
    */
  std::pair<double, double>
  getSingleOrDimuonEfficiency(const TrigMuonEff::Configuration& config,
                              const std::vector<TLorentzVector>& muons,
                              const std::vector<MuonQuality>& muonsQuality,
                              const std::string& chain) const;

  /** get the OR'ed efficiency of single and dielectron triggers
    *
    * @param config [in] configuration
    * @param electrons [in] array of TLorentzVector for electrons
    * @param electronsQuality [in] array of electron quality
    * @param chain [in] trigger chain name
    * @return pair->first : single or dielectron trigger efficiency, pair->second : uncertainty on the trigger efficiency
    */
  std::pair<double, double>
  getSingleOrDielectronEfficiency(const TrigMuonEff::Configuration& config,
                              const std::vector<TLorentzVector>& electrons,
                              const std::vector<ElectronQuality>& electronsQuality,
                              const std::string& chain) const;

  /** get electron trigger SF or efficiency
    *
    * @param electron [in] TLorentzVector of electron
    * @param combination [in] trigger/offline combination
    * @param year [in] year
    * @param configuration [in] configuration
    * @return pair->first : efficiency or SF, pair->second : uncertainty
    */
  std::pair<double, double>
  getElectronSF(const TLorentzVector& electron,
                const int combination,
                const int year,
                const TrigMuonEff::Configuration& configuration) const;



  /** get electron efficiency for MC
    *
    * @param electron [in] TLorentzVector of electron
    * @param combination [in] trigger/offline combination
    * @param year [in] year
    * @param configuration [in] configuration
    * @return pair->first : efficiency or SF, pair->second : uncertainty
    */
  std::pair<double, double>
  ElEff_MC(const TLorentzVector& electron,
           const int combination,
           const int year,
           const TrigMuonEff::Configuration& configuration) const
  {return getElectronSF(electron, combination, year, configuration);}



  /** get electron trigger efficiency for MC
    *
    * @param electron [in] TLorentzVector of electron
    * @param combination [in] trigger/offline combination
    * @param useGeV [in] true for GeV, false for MeV
    * @param year [in] year
    * @param runNumber [in] Run number
    * @return pair->first : efficiency, pair->second : uncertainty
    */
  std::pair<double, double>
  ElEff_MC(const TLorentzVector& electron,
           const int combination,
           const bool useGeV = false,
           const int year = 2011,
           const int runNumber = 200804) const
  {
    TrigMuonEff::Configuration configuration;
    configuration.runNumber = runNumber;
    configuration.useGeV = useGeV;
    configuration.setByUser = false;
    configuration.isAFII = m_IsAFII;   
    return getElectronSF(electron, combination, year, configuration);
  }



  /** get electron trigger efficiency for data
    *
    * @param electron [in] TLorentzVector of electron
    * @param combinationSF [in] trigger/offline combination
    * @param combinationEfficiency [in] trigger/offline combination
    * @param year [in] year
    * @param configuration [in] configuration
    * @return pair->first : efficiency, pair->second : uncertainty
    */
  std::pair<double, double>
  ElEff_Data(const TLorentzVector& electron,
             const int combinationSF,
             const int combinationEfficiency,
             const int year,
             const TrigMuonEff::Configuration& config) const;



  /** get electron trigger efficiency for data
    *
    * @param electron [in] TLorentzVector of electron
    * @param combinationSF [in] trigger/offline combination
    * @param combinationEfficiency [in] trigger/offline combination
    * @param useGeV [in] true for GeV, false for MeV
    * @param year [in] year
    * @param runNumber [in] Run number
    * @return pair->first : efficiency, pair->second : uncertainty
    */
  std::pair<double, double>
  ElEff_Data(const TLorentzVector& electron,
             const int combinationSF,
             const int combinationEfficiency,
             const bool useGeV = false,
             const int year = 2011,
             const int runNumber = 200804) const
  {
    TrigMuonEff::Configuration configuration;
    configuration.runNumber = runNumber;
    configuration.useGeV = useGeV;
    configuration.setByUser = false;
    configuration.isAFII = m_IsAFII;   
    return ElEff_Data(electron, combinationSF, combinationEfficiency, year, configuration);
  }



  /** flag for AtlFast II sample
   *
   * @param flag [in] true for AtlFast II sample, false otherwise
   */
  void setAFII(const bool flag=true) {m_IsAFII = flag;};

  void setMaxPeriod(const TrigMuonEff::DataPeriod x) {m_max_period = x;};


  /** Creates MC replica of efficiency maps
   *
   * n Efficiency maps are created. MC replica has eff_ij[n] where eff_ij[n] is generated
   * with Gaussian using parameters, mu = efficiency at bin [ij] with sigma = statistical
   * error of the bin [ij]
   *
   * @param n [in] number of replica to be created
   * @param trigger [in] trigger to be replicated
   */
  void
  createMcReplicaMuon(const int n,
                      const std::string& trigger = std::string());



  /** get the trigger scale factor for muons with non-uniform quality
    *
    * @param runNumber [in] run number
    * @param useGeV [in] use GeV if true, otherwise use MeV
    * @param muons [in] TLorentzVector array of muons
    * @param muonsQuality [in] array of muon qualities
    * @param index [in] index of replicated histograms to access
    * @param configuration [in] configuration
    * @return pair->first : scale factor, pair->second : uncertainty on the scale factor
    */
  std::pair<double, double>
  getSfReplicaMuon(const int runNumber,
                   const bool useGeV,
                   const std::vector<TLorentzVector>& muons,
                   const std::vector<MuonQuality>& muonsQuality,
                   const int index = 0,
                   TrigMuonEff::Configuration* configuration = 0) const;
 
  /** Translate the electron quality enumeration into a value used internally by the tool
    *
    * @param runNumber [in] run number
    * @param quality [in] electron quality 
    * @param isSF [in] true for SF, false for efficiency
    * @param isditrig [in] true if not just default single electron triggers are used 
    * @return name
    */
  int
  getElectronQuality(const int runNumber,
                     const ElectronQuality quality,
                     const bool isSF,
                     const bool isditrig,
		     const DileptonTriggerName dilepName = kNoTrig) const;


private:
  void
  prepareForElectronSF(const std::string& directory);



  /** get the path name for a root file
    *
    * @param directory [in] directory name
    * @param fileName [in] file name
    * @return file name
    */
  std::string
  getPathName(const std::string& directory,
              const std::string& fileName) const;



  /** set parameters for reading a root file
    *
    * @param storage [in] storage parameter
    */
  void
  setStorage(Storage& storage) const;



  /** get temporary directory
    *
    */
  TDirectory*
  getTemporaryDirectory(void) const;



  /** Initialisation method
   *
   * The initialisation takes care of reading in all the efficiency histograms from the ROOT
   * file found in the package. It makes a copy of every histogram in memory, and takes
   * ownership of them. It then needs to delete each one of them in its destructor.
   *
   * @param directory [in] path to the directory that contains efficiency file(s)
   * @param fileName [in] file name of the efficiency file
   * @param qualities [in] an array specifing offline muon quality
   */
  void
  initialise(const std::string& directory,
             const std::string& fileName = "muon_trigger_sf.root",
             const std::vector<std::string>& qualities = std::vector<std::string>(),
             const std::string& directoryElectron = "");
  


  /** Translate the muon quality enumeration into a value used internally by the tool
    *
    * @param quality [in] muon quality 
    * @return name
    */
  static std::string
  getMuonQuality(const MuonQuality quality);



  /** Get the minimum offline electron and muon pT thresholds for a given run
    *
    * @param configuration [in] configuration
    * @return pair->first : muon, pair->second : electron
    */
  static std::pair<double, double>
  getThresholds(const TrigMuonEff::Configuration& configuration);



  struct DileptonThresholds {
    double mu18_tight;
    double mu8_EFFS;
    double mu8;
    double mu13;
    double e12Tvh_loose1;
    double e12Tvh_medium1;
    // for backward compatibility
    DileptonThresholds(const double mu18_tight_ = 20,
		       const double mu8_EFFS_ = 10,
		       const double mu13_ = 15,
		       const double e12Tvh_loose1_ = 14) :
      mu18_tight(mu18_tight_), mu8_EFFS(mu8_EFFS_), mu8(mu8_EFFS_), mu13(mu13_), e12Tvh_loose1(e12Tvh_loose1_), e12Tvh_medium1(e12Tvh_loose1_) {}

    //DileptonThresholds(const double mu18_tight_ = 20,
    //		       const double mu8_EFFS_ = 10,
    //		       const double mu13_ = 15,
    //		       const double e12Tvh_loose1_ = 14,
    //		       const double mu8_ = 10,
    //		       const double e12Tvh_medium1_ = 14) :
    //  mu18_tight(mu18_tight_), mu8_EFFS(mu8_EFFS_), mu8(mu8_), mu13(mu13_), e12Tvh_loose1(e12Tvh_loose1_), e12Tvh_medium1(e12Tvh_medium1_) {}
  };

  /** Get the minimum offline muon and electron pT thresholds for dimuon and dielectron chains
    *
    * @param configuration [in] configuration
    * @param thresholds [out] DileptonThresholds structure
    */
  static void
  getDileptonThresholds(const TrigMuonEff::Configuration& config,
			DileptonThresholds& thresholds);



  /** Translate a run number to a data period
    *
    * @param configuration [in] configuration
    * @return pair->first : muon, pair->second : electron
    */
  static TrigMuonEff::DataPeriod
  getDataPeriod(int runNumber);



  /** Function to calculate the uncertainty on the SF
    *
    * @param a [in] parameter
    * @param b [in] parameter
    * @param c [in] parameter
    * @param d [in] parameter
    * @return uncertainty
    */
  static double
  getSfError(const double a,
             const double b,
             const double c,
             const double d);

  /** Function to calculate the uncertainty on the dilepton efficiency
    *
    * @param a1 [in] parameter
    * @param a2 [in] parameter
    * @param b1 [in] parameter
    * @param b2 [in] parameter
    * @param c1 [in] parameter
    * @param c2 [in] parameter
    * @param d1 [in] parameter
    * @param d2 [in] parameter
    * @param e1 [in] parameter
    * @param e2 [in] parameter
    * @param f1 [in] parameter
    * @param f2 [in] parameter
    * @return uncertainty
    */
  static double
  getDiError(const double a1, const double a2,
             const double b1, const double b2,
             const double c1, const double c2,
             const double d1, const double d2,
             const double e1, const double e2,
             const double f1, const double f2);

/** Function to calculate the uncertainty on the single or dilepton efficiency for asymmetric dilepton triggers
    *
    * @param a1 [in] parameter
    * @param a2 [in] parameter
    * @param b1 [in] parameter
    * @param b2 [in] parameter
    * @param c1 [in] parameter
    * @param c2 [in] parameter
    * @param d1 [in] parameter
    * @param d2 [in] parameter
    * @param e1 [in] parameter
    * @param e2 [in] parameter
    * @param f1 [in] parameter
    * @param f2 [in] parameter
    * @param g1 [in] parameter
    * @param g2 [in] parameter
    * @param h1 [in] parameter
    * @param h2 [in] parameter
    * @return uncertainty
    */
  static double
  getSingleOrDiError_asym(const double a1, const double a2,
			  const double b1, const double b2,
			  const double c1, const double c2,
			  const double d1, const double d2,
			  const double e1, const double e2,
			  const double f1, const double f2,
			  const double g1, const double g2,
			  const double h1, const double h2);


  /** Function to calculate the uncertainty on the single or dilepton efficiency for symmetric dilepton triggers
    *
    * @param a1 [in] parameter
    * @param a2 [in] parameter
    * @param b1 [in] parameter
    * @param b2 [in] parameter
    * @param c1 [in] parameter
    * @param c2 [in] parameter
    * @param d1 [in] parameter
    * @param d2 [in] parameter
    * @param ispT1above [in] parameter
    * @param ispT2above [in] parameter
    * @return uncertainty
    */
  static double
  getSingleOrDiError_sym(const double a1, const double a2,
			 const double b1, const double b2,
			 const double c1, const double c2,
			 const double d1, const double d2,
			 bool ispT1above, bool ispT2above);


  struct DileptonTrigger {
    std::string leg1;
    std::string leg2;
    std::string bothLegs;
    DileptonTrigger(const std::string& leg1_ = std::string(),
		    const std::string& leg2_ = std::string(),
		    const std::string& bothLegs_ = std::string()) :
      leg1(leg1_), leg2(leg2_), bothLegs(bothLegs_) {}
  };

  /** Function to calculate the uncertainty on the SF
    *
    * @param chain [in] trigger chain name
    * @param legs [out] DileptonTrigger structure
    */
  static void
  getDileptonLegs(const std::string& chain,
                DileptonTrigger& legs);


  bool
  setConfiguration(TrigMuonEff::Configuration& config,
                   TrigMuonEff::DataPeriod period = TrigMuonEff::period_undefined) const;

  void
  setPhiBoundary2011(const std::vector<std::string> *period);

  void
  setPhiBoundary(TFile* file);


  /** fix phi range to match for the detector configuration
    *
    * @param muon [in] TLorentzVector of muon
    * @return phi
    */
  double
  fixPhiRange(const TLorentzVector& muon) const;


  void
  initialiseElectronTool(const int key) const;


  int m_year;
  std::vector<std::string> m_qualities;
  typedef std::map<std::string, TH2*> EfficiencyMap;
  typedef std::pair<std::string, TH2*> EfficiencyPair;
  EfficiencyMap m_efficiencyMap; /**< The main efficiency storage location */
  std::vector<EfficiencyMap> m_efficiencyMapReplicaArray; /**< The replica efficiency storage location */
  
  double m_phiBoundaryBarrel; /**< Muon phi boundary in the barrel region */
  double m_phiBoundaryEndcap; /**< Muon phi boundary in the endcap region */
  
  typedef std::map<int, Root::TElectronEfficiencyCorrectionTool*> ElectronEfficiencyToolMap;
  typedef std::pair<int, Root::TElectronEfficiencyCorrectionTool*> ElectronEfficiencyToolPair;
  mutable ElectronEfficiencyToolMap m_electronSF;
  mutable std::map<int, std::string> m_fileNameMap;

  bool m_IsAFII;
  TrigMuonEff::DataPeriod m_max_period;
  std::string m_electronFileVersion;

}; // class LeptonTriggerSF

#endif // TrigMuonEfficiency_LeptonTriggerSF_H
