/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Function to handle 2D maps of modules, one for each region
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMon2DLumiProfiles.h"
#include <string.h>
#include "GaudiKernel/StatusCode.h"
#include "InDetIdentifier/PixelID.h"
#include "LWHists/TProfile2D_LW.h"
#include "PixelMonitoring/Components.h"

PixelMon2DLumiProfiles::PixelMon2DLumiProfiles(std::string name, std::string title, std::string zlabel, const PixMon::HistConf& config)
    : HolderTemplate<TProfile2D_LW>(config) {
  const int lbRange = 3000;
  int num_modules;
  if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kIBL)) {
    num_modules = PixMon::kNumStavesIBL * (PixMon::kNumModulesIBL2D + PixMon::kNumModulesIBL3D);
    m_histograms.at(0).reset(TProfile2D_LW::create((name + "_2D_Profile_IBL").c_str(), (title + ", IBL " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                                   lbRange, -0.5, -0.5 + lbRange,
                                                   num_modules, -0.5, -0.5 + num_modules));
  }
  if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kB0)) {
    num_modules = PixMon::kNumStavesL0 * PixMon::kNumModulesBarrel;
    m_histograms.at(3).reset(TProfile2D_LW::create((name + "_2D_Profile_B0").c_str(), (title + ", B0 " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                                   lbRange, -0.5, -0.5 + lbRange,
                                                   num_modules, -0.5, -0.5 + num_modules));
  }
  if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kB1)) {
    num_modules = PixMon::kNumStavesL1 * PixMon::kNumModulesBarrel;
    m_histograms.at(4).reset(TProfile2D_LW::create((name + "_2D_Profile_B1").c_str(), (title + ", B1 " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                                   lbRange, -0.5, -0.5 + lbRange,
                                                   num_modules, -0.5, -0.5 + num_modules));
  }
  if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kB2)) {
    num_modules = PixMon::kNumStavesL2 * PixMon::kNumModulesBarrel;
    m_histograms.at(5).reset(TProfile2D_LW::create((name + "_2D_Profile_B2").c_str(), (title + ", B2 " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                                   lbRange, -0.5, -0.5 + lbRange,
                                                   num_modules, -0.5, -0.5 + num_modules));
  }
  if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kECA)) {
    num_modules = PixMon::kNumLayersDisk * PixMon::kNumModulesDisk;
    m_histograms.at(6).reset(TProfile2D_LW::create((name + "_2D_Profile_ECA").c_str(), (title + ", ECA " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                                   lbRange, -0.5, -0.5 + lbRange,
                                                   num_modules, -0.5, -0.5 + num_modules));
  }
  if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kECC)) {
    num_modules = PixMon::kNumLayersDisk * PixMon::kNumModulesDisk;
    m_histograms.at(7).reset(TProfile2D_LW::create((name + "_2D_Profile_ECC").c_str(), (title + ", ECC " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                                   lbRange, -0.5, -0.5 + lbRange,
                                                   num_modules, -0.5, -0.5 + num_modules));
  }
  if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kDBMA)) {
    num_modules = PixMon::kNumLayersDBM * PixMon::kNumModulesDBM;
    m_histograms.at(8).reset(TProfile2D_LW::create((name + "_2D_Profile_DBMA").c_str(), (title + ", DBMA " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                                   lbRange, -0.5, -0.5 + lbRange,
                                                   num_modules, -0.5, -0.5 + num_modules));
  }
  if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kDBMC)) {
    num_modules = PixMon::kNumLayersDBM * PixMon::kNumModulesDBM;
    m_histograms.at(9).reset(TProfile2D_LW::create((name + "_2D_Profile_DBMC").c_str(), (title + ", DBMC " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                                   lbRange, -0.5, -0.5 + lbRange,
                                                   num_modules, -0.5, -0.5 + num_modules));
  }

  setHistogramPointers();
  formatHist();
}

void PixelMon2DLumiProfiles::fill(double LB, Identifier& id, const PixelID* pixID, double weight) {
  const int bec = pixID->barrel_ec(id);
  const int pm = pixID->phi_module(id);
  int ld = pixID->layer_disk(id);

  if (bec == 2 && A) {
    A->Fill(LB, ld * 48 + pm, weight);
  } else if (bec == -2 && C) {
    C->Fill(LB, ld * 48 + pm, weight);
  } else if (bec == 4 && DBMA) {
    DBMA->Fill(LB, ld * 4 + pm, weight);
  } else if (bec == -4 && DBMC) {
    DBMC->Fill(LB, ld * 4 + pm, weight);
  } else if (bec == 0) {
    if (m_doIBL) ld--;
    const int em = pixID->eta_module(id) + 6;
    if (ld == 0 && B0) {
      B0->Fill(LB, em + 13 * pm, weight);
    } else if (ld == 1 && B1) {
      B1->Fill(LB, em + 13 * pm, weight);
    } else if (ld == 2 && B2) {
      B2->Fill(LB, em + 13 * pm, weight);
    } else if (ld == -1 && IBL) {
      IBL->Fill(LB, em + 4 + 20 * pm, weight);
    }
  }
}

void PixelMon2DLumiProfiles::formatHist() {
  std::string label;
  unsigned int count = 1;
  if (A && C) {
    for (unsigned int j = 0; j < PixMon::kNumLayersDisk; ++j) {
      for (unsigned int i = 0; i < PixMon::kNumModulesDisk; ++i) {
        label = PixMon::LayersDisk.at(j) + "A_" + PixMon::ModulesECA.at(i);
        A->GetYaxis()->SetBinLabel(count, label.c_str());
        label = PixMon::LayersDisk.at(j) + "C_" + PixMon::ModulesECC.at(i);
        C->GetYaxis()->SetBinLabel(count, label.c_str());
        count++;
      }
    }
    count = 1;
  }
  if (DBMA && DBMC) {
    for (unsigned int j = 0; j < PixMon::kNumLayersDBM; ++j) {
      for (unsigned int i = 0; i < PixMon::kNumModulesDBM; ++i) {
        label = PixMon::LayersDBM.at(j) + "A_" + PixMon::ModulesDBM.at(i);
        DBMA->GetYaxis()->SetBinLabel(count, label.c_str());
        label = PixMon::LayersDBM.at(j) + "C_" + PixMon::ModulesDBM.at(i);
        DBMC->GetYaxis()->SetBinLabel(count, label.c_str());
        count++;
      }
    }
    count = 1;
  }
  if (B0 && B1 && B2) {
    for (unsigned int i = 0; i < PixMon::kNumStavesL0; ++i) {
      for (unsigned int j = 0; j < PixMon::kNumModulesBarrel; ++j) {
        label = "L0_" + PixMon::StavesL0.at(i) + "_" + PixMon::ModulesBarrel.at(j);
        B0->GetYaxis()->SetBinLabel(count, label.c_str());
        count++;
      }
    }
    count = 1;
    for (unsigned int i = 0; i < PixMon::kNumStavesL1; ++i) {
      for (unsigned int j = 0; j < PixMon::kNumModulesBarrel; ++j) {
        label = "L1_" + PixMon::StavesL1.at(i) + "_" + PixMon::ModulesBarrel.at(j);
        B1->GetYaxis()->SetBinLabel(count, label.c_str());
        count++;
      }
    }
    count = 1;
    for (unsigned int i = 0; i < PixMon::kNumStavesL2; ++i) {
      for (unsigned int j = 0; j < PixMon::kNumModulesBarrel; ++j) {
        label = "L2_" + PixMon::StavesL2.at(i) + "_" + PixMon::ModulesBarrel.at(j);
        B2->GetYaxis()->SetBinLabel(count, label.c_str());
        count++;
      }
    }
    count = 1;
  }
  if (IBL) {
    for (unsigned int i = 0; i < PixMon::kNumStavesIBL; ++i) {
      for (unsigned int j = 0; j < PixMon::kNumModulesIBL; ++j) {
        label = "IBL_" + PixMon::StavesIBL.at(i) + "_" + PixMon::ModulesIBL.at(j);
        IBL->GetYaxis()->SetBinLabel(count, label.c_str());
        count++;
      }
    }
  }

  for (auto& hist : m_histograms) {
    if (!hist) continue;
    if (hist.get() == A || hist.get() == C) {
      hist->GetYaxis()->SetLabelSize(0.02);
    } else {
      hist->GetYaxis()->SetLabelSize(0.03);
    }
    hist->SetOption("colz");
  }
}
