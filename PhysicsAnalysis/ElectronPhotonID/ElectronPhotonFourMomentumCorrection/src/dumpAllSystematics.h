// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DUMPALLSYSTEMATICS_H
#define DUMPALLSYSTEMATICS_H

#include <vector>
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

#include "TTree.h"

// Local include(s):
#include "EgammaAnalysisInterfaces/IEgammaCalibrationAndSmearingTool.h"

class DumpAllSystematics : public AthAlgorithm {

public:
  /// Regular Algorithm constructor
  DumpAllSystematics( const std::string& name, ISvcLocator* svcLoc );

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode  finalize();

  std::string m_particle_name;
  std::string m_reco_container_name;
  bool m_keep_one;

private:
  enum class ParticleType {ELECTRON, PHOTON};
  ParticleType m_particle_type;

  TTree* m_tree;

  StatusCode do_truth(const xAOD::Egamma& particle);
  StatusCode do_egamma(const xAOD::Egamma& particle);
  StatusCode do_energy(xAOD::Egamma& particle, int itool);

  unsigned long long m_EventNumber;
  uint32_t m_RunNumber;
  int m_instance_index;
  float m_actualIntPerXing;
  float m_averageIntPerXing;
  float m_truth_pt;
  float m_truth_phi;
  float m_truth_eta;
  float m_truth_E;
  int m_truth_pdgId;
  int m_truth_parent_pdgId;
  bool m_truth_matched;
  bool m_truth_isConv;
  float m_truth_Rconv;
  int m_npv;

  float m_cl_phi;
  float m_cl_eta;
  float m_cl_etaCalo;
  float m_cl_rawcl_Es0;
  float m_cl_rawcl_Es1;
  float m_cl_rawcl_Es2;
  float m_cl_rawcl_Es3;
  float m_cl_E;
  float m_ph_Rconv;
  int m_ph_convFlag;
  float m_wstot;

  std::vector<float> m_nominal_E;
  std::vector<float> m_energy_variations_sum_up;
  std::vector<float> m_energy_variations_sum_down;
  std::vector<std::vector<float> > m_energy_variations;


  ToolHandleArray<CP::IEgammaCalibrationAndSmearingTool> m_EgammaCalibrationAndSmearingTools;

}; // class testAthenaEgammaCalibTool

#endif // DUMPALLSYSTEMATICS_H
