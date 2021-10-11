/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONCHARGECORRECTION__ELECTRONCHARGECORRECTIONTOOL__H
#define ELECTRONCHARGECORRECTION__ELECTRONCHARGECORRECTIONTOOL__H

#include "AsgTools/AsgTool.h"
//#include
//"ElectronChargeEfficiencyCorrectionTool/IElectronChargeEfficiencyCorrectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "TH2.h"
// #include "xAODTruth/TruthParticle.h"
#include "AthContainers/AuxElement.h"
#include "xAODEgamma/ElectronFwd.h"

#include <map>
#include <string>
#include <vector>

// Forward declaration(s):
namespace xAOD {
class IParticle;
}
// class TH2D;

namespace CP {

class ElectronChargeEfficiencyCorrectionTool
  : virtual public IAsgElectronEfficiencyCorrectionTool
  , public asg::AsgTool
{
  ASG_TOOL_CLASS(ElectronChargeEfficiencyCorrectionTool,
                 IAsgElectronEfficiencyCorrectionTool)

public:
  /// Standard constructor
  ElectronChargeEfficiencyCorrectionTool(const std::string& name);

  /// Standard destructor
  virtual ~ElectronChargeEfficiencyCorrectionTool();

  // METADATA  // TO DO
  //    virtual StatusCode beginInputFile();
  //    virtual StatusCode beginEvent();
  // virtual StatusCode endInputFile();

public:
  /// Gaudi Service Interface method implementations
  virtual StatusCode initialize();

  /// Gaudi Service Interface method implementations
  virtual StatusCode finalize();

  /// Retrieve the Scale factor
  virtual CP::CorrectionCode getEfficiencyScaleFactor(
    const xAOD::Electron& inputObject,
    double& sf) const;

  /// Decorate the electron
  virtual CP::CorrectionCode applyEfficiencyScaleFactor(
    const xAOD::Electron& inputObject) const;

  /// Systematics
  //  void applySysVariation();

  /// Returns whether this tool is affected by the given systematics
  virtual bool isAffectedBySystematic(
    const SystematicVariation& systematic) const;

  /// Returns the list of all systematics this tool can be affected by
  virtual SystematicSet affectingSystematics() const;

  /// Returns the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const;

  virtual StatusCode applySystematicVariation(const SystematicSet& systConfig);

  StatusCode registerSystematics();

  /// returns: the currently applied systematics
  const CP::SystematicSet& appliedSystematics() const
  {
    return *m_appliedSystematics;
  }

  int systUncorrVariationIndex(const xAOD::Electron&) const
  {
    ATH_MSG_WARNING("systUncorrVariationIndex is not implemented in "
                    "ElectronChargeEfficiencyCorrectionTool");
    return -999;
  }

  //

private:
  /// Get the charge flip rate rate given pt, eta, histogram
  float getChargeFlipRate(double eta,
                          double pt,
                          TH2* hrates,
                          double& flipRate) const;

  /// Get the charge of the original electron
  CP::CorrectionCode getEleTruthCharge(const xAOD::Electron& ele,
                                       int& truthcharge) const;

  /// Return true if it's prompt ele
  CP::CorrectionCode isGoodEle(const xAOD::Electron& ele, bool& goodele) const;

  /// Force the data type to a given value
  int m_dataTypeOverwrite;
  //
  //
  /// The Event info collection name
  std::string m_eventInfoCollectionName;

  /// Histogram that holds the correction rates for Monte Carlo
  std::map<std::string, std::vector<TH2*>>
    m_SF_SS; // keys (e.g. RunNumber223333_319200_Nvtx0_10_Phi1.5_1.6) mapping
             // to vector of SF histograms --> vector m_SF: 0=nominal, 1=stat,
             // 2,3,4...n=syst
  std::map<std::string, std::vector<TH2*>>
    m_SF_OS; // keys (e.g. RunNumber223333_319200_Nvtx0_10_Phi1.5_1.6) mapping
             // to vector of SF histograms --> vector m_SF: 0=nominal, 1=stat,
             // 2,3,4...n=syst

  // cuts // further variables to bin in
  std::vector<unsigned int> m_RunNumbers;
  bool m_useRandomRunNumber;
  unsigned int m_defaultRandomRunNumber;

  /// The name of the input file that contains the histograms
  std::string m_filename;

  /// The name of the folder defining the LH/iso working point
  std::string m_workingPoint;

  /// Lower limit of eta range where corrections are available; taken from
  /// histogram
  double m_eta_lowlimit;

  /// Upper limit of eta range where corrections are available; taken from
  /// histogram
  double m_eta_uplimit;

  /// Lower limit of pt range where corrections are available; taken from
  /// histogram
  double m_pt_lowlimit;

  /// Upper limit of pt range where corrections are available; taken from
  /// histogram
  double m_pt_uplimit;

  /// Factor for GeV <-> MeV switching
  float m_gevmev;

  /// Truth charge
  int m_truthCharge;

  // const xAOD::TruthParticle *m_truthparticle;

  /// Random number generator for throwing the dice
  //    TRandom3 *m_Rndm;

  // Systematics
  std::vector<std::string> m_systematics;

  std::map<CP::SystematicSet, CP::SystematicSet> m_filtered_sys_sets;
  // boost::unordered_map<SystematicSet,EffiCollection*> m_sf_sets;
  CP::SystematicSet m_mySysConf;
  CP::SystematicSet m_affectingSys;

  /// Currently applied systematics
  CP::SystematicSet* m_appliedSystematics;

  /// Decorator
  std::string m_sf_decoration_name;
  SG::AuxElement::Decorator<float>* m_sfDec;
};

} // End namespace CP

#endif

/// Apply the correction on a modifyable object
//......virtual CP::CorrectionCode applyCorrection( xAOD::Electron& ele );

/// Create a corrected copy from a constant muon
//...virtual CP::CorrectionCode correctedCopy( const xAOD::Electron& input,
//...					      xAOD::Electron*& output );
