/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Function to handle 2D maps of modules, one for each region
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMon2DLumiMaps.h"
#include "PixelMonitoring/Components.h"
#include "InDetIdentifier/PixelID.h"
#include "LWHists/TH2F_LW.h"
#include "GaudiKernel/StatusCode.h"     
#include <string.h>

PixelMon2DLumiMaps::PixelMon2DLumiMaps(std::string name, std::string title, std::string zlabel, const PixMon::HistConf& config)
    : IBLlbm(nullptr),
      B0lbm(nullptr),
      B1lbm(nullptr),
      B2lbm(nullptr),
      Albm(nullptr),
      Clbm(nullptr),
      DBMAlbm(nullptr),
      DBMClbm(nullptr),
      m_config(config)
{
   const int lbRange = 3000;
   int num_modules;
   if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kIBL)) {
      num_modules = PixMon::kNumStavesIBL * (PixMon::kNumModulesIBL2D + PixMon::kNumModulesIBL3D);
      IBLlbm = TH2F_LW::create((name+"_2D_Map_IBL").c_str(),(title + ", IBL " + " (Map);LB;Module;" + zlabel).c_str(),
                               lbRange, -0.5, -0.5 + lbRange,
                               num_modules, -0.5, -0.5 + num_modules);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kB0)) {
      num_modules = PixMon::kNumStavesL0 * PixMon::kNumModulesBarrel;
      B0lbm = TH2F_LW::create((name+"_2D_Map_B0").c_str(), (title + ", B0 " + " (Map);LB;Module;" + zlabel).c_str(),
                              lbRange, -0.5, -0.5 + lbRange,
                              num_modules, -0.5, -0.5 + num_modules);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kB1)) {
      num_modules = PixMon::kNumStavesL1 * PixMon::kNumModulesBarrel;
      B1lbm = TH2F_LW::create((name+"_2D_Map_B1").c_str(), (title + ", B1 " + " (Map);LB;Module;" + zlabel).c_str(),
                              lbRange, -0.5, -0.5 + lbRange,
                              num_modules, -0.5, -0.5 + num_modules);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kB2)) {
      num_modules = PixMon::kNumStavesL2 * PixMon::kNumModulesBarrel;
      B2lbm = TH2F_LW::create((name+"_2D_Map_B2").c_str(), (title + ", B2 " + " (Map);LB;Module;" + zlabel).c_str(),
                              lbRange, -0.5, -0.5 + lbRange,
                              num_modules, -0.5, -0.5 + num_modules);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kECA)) {
      num_modules = PixMon::kNumLayersDisk * PixMon::kNumModulesDisk;
      Albm = TH2F_LW::create((name+"_2D_Map_ECA" ).c_str(),(title + ", ECA " + " (Map);LB;Module;" + zlabel).c_str(),
                             lbRange, -0.5, -0.5 + lbRange,
                             num_modules, -0.5, -0.5 + num_modules);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kECC)) {
      num_modules = PixMon::kNumLayersDisk * PixMon::kNumModulesDisk;
      Clbm = TH2F_LW::create((name+"_2D_Map_ECC" ).c_str(),(title + ", ECC " + " (Map);LB;Module;" + zlabel).c_str(),
                             lbRange, -0.5, -0.5 + lbRange,
                             num_modules, -0.5, -0.5 + num_modules);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kDBMA)) {
      num_modules = PixMon::kNumLayersDBM * PixMon::kNumModulesDBM;
      DBMAlbm = TH2F_LW::create((name+"_2D_Map_DBMA").c_str(),(title + ", DBMA " + " (Map);LB;Module;" + zlabel).c_str(),
                                lbRange, -0.5, -0.5 + lbRange,
                                num_modules, -0.5, -0.5 + num_modules);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kDBMC)) {
      num_modules = PixMon::kNumLayersDBM * PixMon::kNumModulesDBM;
      DBMClbm = TH2F_LW::create((name+"_2D_Map_DBMC").c_str(),(title + ", DBMC " + " (Map);LB;Module;" + zlabel).c_str(),
                                lbRange, -0.5, -0.5 + lbRange,
                                num_modules, -0.5, -0.5 + num_modules);
   }

   // We do not have support for IBL2D/3D in this class, but the histogram
   // array should look the same for all classes. Therefore, we fill the
   // blanks for IBL2D/3d with null pointers.
   m_histograms = {IBLlbm, nullptr, nullptr, B0lbm, B1lbm, B2lbm, Albm, Clbm, DBMAlbm, DBMClbm};

   formatHist();
}

PixelMon2DLumiMaps::~PixelMon2DLumiMaps()
{
   for (auto& hist : m_histograms) {
      if (hist) LWHist::safeDelete(hist);
   }
}

void PixelMon2DLumiMaps::Fill(double LB,Identifier &id, const PixelID* pixID,double weight)
{
   const int bec = pixID->barrel_ec(id);
   const int pm  = pixID->phi_module(id);
   int ld = pixID->layer_disk(id);

   if (bec == 2 && Albm) {
      Albm->Fill(LB, ld * 48 + pm, weight);
   } else if (bec == -2 && Clbm) {
      Clbm->Fill(LB, ld * 48 + pm, weight);
   } else if (bec == 4 && DBMAlbm) {
      DBMAlbm->Fill(LB, ld * 4 + pm, weight);
   } else if (bec == -4 && DBMClbm) {
      DBMClbm->Fill(LB, ld * 4 + pm, weight);
   } else if (bec == 0) {
      if (m_doIBL) ld--;
      const int em  = pixID->eta_module(id) + 6;
      if (ld == 0 && B0lbm) {
         B0lbm->Fill(LB, em + 13 * pm, weight);
      }else if (ld == 1 && B1lbm) {
         B1lbm->Fill(LB, em + 13 * pm, weight);
      } else if (ld == 2 && B2lbm) {
         B2lbm->Fill(LB, em + 13 * pm, weight);
      } else if (ld == -1 && IBLlbm) {
         IBLlbm->Fill(LB, em + 4 + 20 * pm, weight);
      }
   }
}

void PixelMon2DLumiMaps::formatHist()
{
   std::string label;
   unsigned int count = 1;
   if (Albm && Clbm) {
      for (unsigned int j = 0; j < PixMon::kNumLayersDisk; ++j) {
         for (unsigned int i = 0; i < PixMon::kNumModulesDisk; ++i) {
            label = PixMon::LayersDisk.at(j) + "A_" + PixMon::ModulesECA.at(i);
            Albm->GetYaxis()->SetBinLabel(count, label.c_str());
            label = PixMon::LayersDisk.at(j) + "C_" + PixMon::ModulesECC.at(i);
            Clbm->GetYaxis()->SetBinLabel(count, label.c_str());
            count++;
         }
      }
      count = 1;
   }
   if (DBMAlbm && DBMClbm) {
      for (unsigned int j = 0; j < PixMon::kNumLayersDBM; ++j) {
         for (unsigned int i = 0; i < PixMon::kNumModulesDBM; ++i) {
            label = PixMon::LayersDBM.at(j) + "A_" + PixMon::ModulesDBM.at(i);
            DBMAlbm->GetYaxis()->SetBinLabel(count, label.c_str());
            label = PixMon::LayersDBM.at(j) + "C_" + PixMon::ModulesDBM.at(i);
            DBMClbm->GetYaxis()->SetBinLabel(count, label.c_str());
            count++;
         }
      }
      count = 1;
   }
   if (B0lbm && B1lbm && B2lbm) {
      for (unsigned int i = 0; i < PixMon::kNumStavesL0; ++i) {
         for (unsigned int j = 0; j < PixMon::kNumModulesBarrel; ++j) {
            label = "L0_" + PixMon::StavesL0.at(i) + "_" + PixMon::ModulesBarrel.at(j);
            B0lbm->GetYaxis()->SetBinLabel(count, label.c_str());
            count++;
         }
      }
      count = 1;
      for (unsigned int i = 0; i < PixMon::kNumStavesL1; ++i) {
         for (unsigned int j = 0; j < PixMon::kNumModulesBarrel; ++j) {
            label = "L1_" + PixMon::StavesL1.at(i) + "_" + PixMon::ModulesBarrel.at(j);
            B1lbm->GetYaxis()->SetBinLabel(count, label.c_str());
            count++;
         }
      }
      count = 1;
      for (unsigned int i = 0; i < PixMon::kNumStavesL2; ++i) {
         for (unsigned int j = 0; j < PixMon::kNumModulesBarrel; ++j) {
            label = "L2_" + PixMon::StavesL2.at(i) + "_" + PixMon::ModulesBarrel.at(j);
            B2lbm->GetYaxis()->SetBinLabel(count, label.c_str());
            count++;
         }
      }
      count = 1;
   }

   if (IBLlbm) {
      for (unsigned int i = 0; i < PixMon::kNumStavesIBL; ++i) {
         for (unsigned int j = 0; j < PixMon::kNumModulesIBL; ++j) {
            label = "IBL_" + PixMon::StavesIBL.at(i) + "_" + PixMon::ModulesIBL.at(j);
            IBLlbm->GetYaxis()->SetBinLabel(count, label.c_str());
            count++;
         }
      }
   }

   for (auto& hist : m_histograms) {
      if (!hist) continue;
      hist->GetYaxis()->SetLabelSize(0.04);
      hist->SetOption("colz");
      hist->SetMinimum(0.);
   }
}

StatusCode PixelMon2DLumiMaps::regHist(ManagedMonitorToolBase::MonGroup &group)
{
   StatusCode sc = StatusCode::SUCCESS;

   for (auto& hist : m_histograms) {
      if (!hist) continue;
      if (group.regHist(hist).isFailure()) {
         sc = StatusCode::FAILURE;
      }
   }

   return sc;
}

const bool PixelMon2DLumiMaps::m_doIBL{true};
