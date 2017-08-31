/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTAGTOOL_H 
#define MUONTAGTOOL_H 

/*****************************************************************************
Name    : MuonTagTool.h
Package : offline/PhysicsAnalysis/MuonID/MuonTagTools
Purpose : build the Muon Tag objects - MuonTagCollection.h. 
	  The Muon Tag fragment is built here
*****************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "GaudiKernel/ToolHandle.h"
#include <inttypes.h>
#include "xAODMuon/MuonContainer.h"
#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "xAODEventInfo/EventInfo.h"
#include <map>


namespace CP {
  class IIsolationSelectionTool;
}

/** Interface ID for MuonTagTool*/  
static const InterfaceID IID_MuonTagTool("MuonTagTool", 1, 0);

class MuonTagTool : public AthAlgTool {
  
 public:
  
  /** Standard Constructor */
  MuonTagTool(const std::string& type, const std::string& name, const IInterface* parent);
  
  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_MuonTagTool; };
  
  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& muonTagCol, const int max);
  virtual StatusCode finalize();
  
 protected:
  
  /** Standard destructor */
  virtual ~MuonTagTool( );
  
 private:
  
  inline int bit2int(int b) const { return 1<<b; };
  
  /** private function to get impact parameter */
  void getMuonImpactParameter (const xAOD::Muon*,double& d0, double& z0, double& d0_significance);
  
  /** Properties */
  std::vector<std::string> m_containerNames;
  std::string m_muon_met_container_name;
  double m_cut_Pt;
  double m_DRcut;
  std::vector<float> m_etconeisocutvalues;
  std::vector<float> m_etconeisorelcutvalues;
  std::vector<float> m_ptconeisocutvalues;
  std::vector<float> m_ptconeisorelcutvalues;
  double m_maxD0preselection;
  
  bool m_cosmics;
  bool m_doInDet;
  double m_maxD0tight, m_maxZ0tight, m_maxZ0loose, m_maxD0signLoose, m_maxD0signTight;
  
  /** the attribute names */
  std::vector<std::string> m_ptStr;
  std::vector<std::string> m_etaStr;
  std::vector<std::string> m_phiStr;
  std::vector<std::string> m_tightStr;
  std::vector<std::string> m_momBalStr;
  std::vector<std::string> m_nprecLayStr;
  std::vector<std::string> m_nprecHoleLayStr;
  std::vector<std::string> m_nphiLayStr;
  std::vector<std::string> m_isoStr;
  
  /** we need to fix the momentum */
  ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_muon_calibration_tool;

  /** Since release 20.1.6 we use the analysis muon selector tool */
  ToolHandle<CP::IMuonSelectionTool> m_muon_selection_tool;
  
  /** Muon isolation tool */
  ToolHandle<CP::IIsolationSelectionTool> m_loose_trackonly_isolation;
  ToolHandle<CP::IIsolationSelectionTool> m_loose_isolation;
  ToolHandle<CP::IIsolationSelectionTool> m_tight_isolation;
  ToolHandle<CP::IIsolationSelectionTool> m_gradient_loose_isolation;
  ToolHandle<CP::IIsolationSelectionTool> m_gradient_isolation;
  ToolHandle<CP::IIsolationSelectionTool> m_fixedcut_tight_trackonly_isolation;
  ToolHandle<CP::IIsolationSelectionTool> m_fixedcut_loose_isolation;

  /** PV StoreGate key */
  std::string m_vxCandidate;

  bool m_doIso;
};

#endif // MUONTAGTOOL_H
