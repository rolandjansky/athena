/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#ifndef EVGENPRODTOOLS_TESTHEPMC_H
#define EVGENPRODTOOLS_TESTHEPMC_H

#include "GeneratorModules/GenBase.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TruthHelper/IsGenNonInteracting.h"
#include "TFile.h"
#include "TH1.h"
#include "HepMC/GenEvent.h"
#include<cmath>

#include<fstream>


/// Filtering algorithm to sanity check HepMC event features.
///
/// The TestHepMC algorithm is used in MC production to ensure that
/// only events with valid final state particles, properly balanced
/// initial-final state momentum and energy, etc. are accepted for
/// further processing. A too-high failure rate results in a warning
/// or algorithm failure to flag up to production that there is a
/// fundamental problem and that the sanity check may be unphysically
/// biasing the resulting events.
///
/// @todo Inherit from GenFilter? It *is* a filter
class TestHepMC : public GenBase {
public:

  TestHepMC(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  int         m_pdg;
  double      m_cm_energy, m_cme_diff;
  double      m_energy_diff, m_max_energy_diff;
  bool        m_dumpEvent;
  double      m_max_dist_trans, m_max_dist, m_min_tau, m_nonG4_energy_threshold;
  double      m_eff_warn_threshold, m_eff_fail_threshold, m_tau_eff_threshold;
  double      m_accur_margin;
  bool m_doHist;

  std::vector<int> m_vertexStatuses;

  int m_nPass;
  int m_nFail;

  int m_TotalTaus;
  int m_FastDecayedTau;

  int m_invalidBeamParticlesCheckRate;
  int m_beamParticleswithStatusNotFourCheckRate;
  int m_beamEnergyCheckRate;
  int m_vtxNANandINFCheckRate;
  int m_vtxDisplacedstatuscode12CheckRate;
  int m_vtxDisplacedstatuscodenot12CheckRate;
  int m_vtxDisplacedMoreThan_1m_CheckRateCnt;
  int m_partMomentumNANandINFCheckRate;
  int m_undecayedPi0statuscode12CheckRate;
  int m_unstableNoEndVtxCheckRate;
  int m_negativeEnergyTachyonicCheckRate;
  int m_decayCheckRate;
  int m_undisplacedLLHdaughtersCheckRate;
  int m_nonZeroPhotonMassCheckRate;
  int m_energyBalanceCheckRate;
  int m_momentumBalanceCheckRate;
  int m_negativeEnergyCheckRate;
  int m_tachyonCheckRate;
  int m_stableUnstableNoParentCheckRate;
  int m_unstablePartNoDecayVtxCheckRate;
  int m_undecayedPi0CheckRate;
  int m_Status1ShortLifetime;
  int m_undisplacedDecayDaughtersOfDisplacedVtxCheckRate;
  int m_nonG4_energyCheckRate;

  std::string m_paramFile;
  
  std::ifstream m_G4file;
  std::ifstream m_susyFile;
  std::vector<int> m_G4pdgID_tab;
  std::vector<int> m_SusyPdgID_tab;
  TruthHelper::IsGenNonInteracting m_nonint;

  /// @todo Can we use the GenAnalysis / AthHistoAlg methods for histo management?
  ServiceHandle<ITHistSvc> m_thistSvc;

  TH1F* m_h_energy_dispVtxCheck;
  TH1F* m_h_energy_dispVtxCheck_lt10;
  TH1F* m_h_pdgid_dispVtxCheck;
  TH1F* m_h_status_dispVtxCheck;
  TH1F* m_h_px_dispVtxCheck;
  TH1F* m_h_py_dispVtxCheck;
  TH1F* m_h_pz_dispVtxCheck;
  TH1F* m_h_vx_dispVtxCheck;
  TH1F* m_h_vy_dispVtxCheck;
  TH1F* m_h_vz_dispVtxCheck;
  TH1F* m_h_vxprod_dispVtxCheck;
  TH1F* m_h_vyprod_dispVtxCheck;
  TH1F* m_h_vzprod_dispVtxCheck;
  TH1F* m_h_vtxend_dispVtxCheck;
  TH1F* m_h_vtxprod_dispVtxCheck;

  TH1F* m_h_photon_mass;
  TH1F* m_h_photon_energy;
  TH1F* m_h_photon_e2_p2_e2;

  TH1F* m_h_energyImbalance;
  TH1F* m_h_momentumImbalance_px;
  TH1F* m_h_momentumImbalance_py;
  TH1F* m_h_momentumImbalance_pz;

  TH1F* m_h_beamparticle1_Energy;
  TH1F* m_h_beamparticle2_Energy;
  TH1F* m_h_cmEnergyDiff;

};


#endif

#endif
