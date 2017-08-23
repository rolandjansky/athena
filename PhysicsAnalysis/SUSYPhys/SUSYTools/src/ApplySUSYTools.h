/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ApplySUSYTools.h
// Header file for class ApplySUSYTools
// Author: Frank Paige <paige@bnl.gov>
//
// ApplySUSYTools is algorithm to apply SUSYTools in AthAnalysisBase,
// producing shallow copies with nominal calibration and systematic
// variations for jets, electrons, muons, and MET.
//
// Also produces vector<bool>, true if nominal or any systematic pass
// "baseline" selection, and uses it to thin objects.
//
///////////////////////////////////////////////////////////////////

#ifndef ST_APPLYSUSYTOOLS_H
#define ST_APPLYSUSYTOOLS_H 1

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

// Handles on the tools
#include "AsgTools/AnaToolHandle.h"

// Handle on the thinning service
#include "GaudiKernel/ServiceHandle.h"

// SUSYTools interface
// Include needed for SystInfo
#include "SUSYTools/ISUSYObjDef_xAODTool.h"

// STL includes
#include <vector>
#include <string>

// For the thinning service
class IThinningSvc;

// Need truth matching for TauJet CP tools
namespace TauAnalysisTools {
  class ITauTruthMatchingTool;
}

namespace ST {

class ApplySUSYTools
  : public ::AthAlgorithm
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Copy constructor:

  /// Constructor with parameters:
  ApplySUSYTools( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor:
  virtual ~ApplySUSYTools();

  // Assignment operator:
  //ApplySUSYTools &operator=(const ApplySUSYTools &alg);

  // Athena algorithm Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:


  bool m_doFatJets;


  // Parameters read by SUSYTools from SUSYTools_Default.conf
  // We repeat here those passed through GetElectrons, etc.
  double m_eleBaselinePt;
  double m_elePt;
  double m_eleEta;
  double m_eled0sig;
  double m_elez0;
  
  double m_muBaselinePt;
  double m_muPt;
  double m_muEta;
  double m_mud0sig;
  double m_muz0;
  double m_muCosmicz0;
  double m_muCosmicd0;
  
  double m_photonBaselinePt;
  double m_photonEta;
  double m_photonPt;
  
  double m_tauPt;
  double m_tauEta;
  
  double m_jetPt;
  double m_jetEta;
  double m_jetJvt;
  double m_badjetJvt;

  // Counters
  int m_evtCount; //!
  int m_maxCount;

  // Cuts
  float m_jetPtCut; // Hard coded >=20GeV in FillJet
  float m_inDetPtCut;

  // Flags
  bool m_doTST;
  int m_isData;
 


  // StoreGate keys
  std::string m_MuonsName;
  std::string m_MuonSpecTPName;
  std::string m_ElectronsName;
  std::string m_JetsName;
  std::string m_FatJetsName;
  std::string m_BTagName;
  std::string m_TruthJetsName;
  std::string m_METsName;
  std::string m_MuonCutMaskName;
  std::string m_MuonSpecCutMaskName;
  std::string m_ElectronCutMaskName;
  std::string m_JetCutMaskName;
  std::string m_TruthJetCutMaskName;
  std::string m_InDetTracksName;
  std::string m_InDetCutMaskName;
  std::string m_GSFTracksName;
  std::string m_GSFTrackCutMaskName;
  std::string m_GSFVerticesName;
  std::string m_GSFVertexCutMaskName;
  std::string m_TauJetsName;
  std::string m_TauJetCutMaskName;
  std::string m_PhotonsName;
  std::string m_PhotonCutMaskName;

  std::vector< std::string > m_ElectronTrigs;
  std::vector< std::string > m_MuonTrigs;
  std::vector< std::string > m_PhotonTrigs;

  std::string m_configFile;

  // SUSYTools object
  asg::AnaToolHandle< ISUSYObjDef_xAODTool > m_objTool;
  // Tau tools that we need direct handles on
  asg::AnaToolHandle< TauAnalysisTools::ITauTruthMatchingTool > m_tauTruthTool;

  // SUSYTools systematics
  std::vector< SystInfo > m_systInfoList; //!
  std::vector< const SystInfo* > m_systInfoJET; //!
  std::vector< const SystInfo* > m_systInfoELE; //!
  std::vector< const SystInfo* > m_systInfoMUON; //!
  std::vector< const SystInfo* > m_systInfoTST; //!
  std::vector< const SystInfo* > m_systInfoCST; //!
  std::vector< const SystInfo* > m_systInfoTAU; //!
  std::vector< const SystInfo* > m_systInfoPHO; //!

  /// Pointer to IThinningSvc
  ServiceHandle<IThinningSvc> m_thinningSvc;

};


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

} //end namespace ST

#endif //ST_APPLYSUSYTOOLS_H

