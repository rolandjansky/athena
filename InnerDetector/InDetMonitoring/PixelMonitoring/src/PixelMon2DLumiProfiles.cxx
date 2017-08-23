/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Function to handle 2D maps of modules, one for each region
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/Components.h"
#include "PixelMonitoring/PixelMon2DLumiProfiles.h"
#include "InDetIdentifier/PixelID.h"
#include "LWHists/TProfile2D_LW.h"
#include "GaudiKernel/StatusCode.h"     
#include <string.h>

PixelMon2DLumiProfiles::PixelMon2DLumiProfiles(std::string name, std::string title, std::string zlabel, const PixMon::HistConf& config)
    : IBLlbp(nullptr),
      B0lbp(nullptr),
      B1lbp(nullptr),
      B2lbp(nullptr),
      Albp(nullptr),
      Clbp(nullptr),
      DBMAlbp(nullptr),
      DBMClbp(nullptr),
      m_config(config)
{
   const int lbRange = 3000;
   int num_modules;
   if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kIBL)) {
      num_modules = PixMon::kNumStavesIBL * (PixMon::kNumModulesIBL2D + PixMon::kNumModulesIBL3D);
      IBLlbp= TProfile2D_LW::create((name+"_2D_Profile_IBL").c_str(), (title + ", IBL " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                    lbRange, -0.5, -0.5 + lbRange,
                                    num_modules, -0.5, -0.5 + num_modules);
   }
   if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kB0)) {
      num_modules = PixMon::kNumStavesL0 * PixMon::kNumModulesBarrel;
      B0lbp = TProfile2D_LW::create((name+"_2D_Profile_B0").c_str(),  (title + ", B0 " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                    lbRange, -0.5, -0.5 + lbRange,
                                    num_modules, -0.5, -0.5 + num_modules);
   }
   if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kB1)) {
      num_modules = PixMon::kNumStavesL1 * PixMon::kNumModulesBarrel;
      B1lbp = TProfile2D_LW::create((name+"_2D_Profile_B1").c_str(),  (title + ", B1 " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                    lbRange, -0.5, -0.5 + lbRange,
                                    num_modules, -0.5, -0.5 + num_modules);
   }
   if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kB2)) {
      num_modules = PixMon::kNumStavesL2 * PixMon::kNumModulesBarrel;;
      B2lbp = TProfile2D_LW::create((name+"_2D_Profile_B2").c_str(),  (title + ", B2 " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                    lbRange, -0.5, -0.5 + lbRange,
                                    num_modules, -0.5, -0.5 + num_modules);
   }
   if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kECA)) {
      num_modules = PixMon::kNumLayersDisk * PixMon::kNumModulesDisk;
      Albp = TProfile2D_LW::create((name+"_2D_Profile_ECA" ).c_str(),(title + ", ECA " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                   lbRange, -0.5, -0.5 + lbRange,
                                   num_modules, -0.5, -0.5 + num_modules);
   }
   if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kECC)) {
      num_modules = PixMon::kNumLayersDisk * PixMon::kNumModulesDisk;
      Clbp = TProfile2D_LW::create((name+"_2D_Profile_ECC" ).c_str(),(title + ", ECC " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                   lbRange, -0.5, -0.5 + lbRange,
                                   num_modules, -0.5, -0.5 + num_modules);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kDBMA)) {
      num_modules = PixMon::kNumLayersDBM * PixMon::kNumModulesDBM;
      DBMAlbp = TProfile2D_LW::create((name+"_2D_Profile_DBMA" ).c_str(),(title + ", DBMA " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                      lbRange, -0.5, -0.5 + lbRange,
                                      num_modules, -0.5, -0.5 + num_modules);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kDBMC)) {
      num_modules = PixMon::kNumLayersDBM * PixMon::kNumModulesDBM;
      DBMClbp = TProfile2D_LW::create((name+"_2D_Profile_DBMC" ).c_str(),(title + ", DBMC " + title + " (Profile);LB;Module;" + zlabel).c_str(),
                                      lbRange, -0.5, -0.5 + lbRange,
                                      num_modules, -0.5, -0.5 + num_modules);
   }

   // We do not have support for IBL2D/3D in this class, but the histogram
   // array should look the same for all classes. Therefore, we fill the
   // blanks for IBL2D/3d with null pointers.
   m_histograms = {IBLlbp, nullptr, nullptr, B0lbp, B1lbp, B2lbp, Albp, Clbp, DBMAlbp, DBMClbp};

   formatHist();
}

PixelMon2DLumiProfiles::~PixelMon2DLumiProfiles()
{
   for (auto& hist : m_histograms) {
      if (hist) LWHist::safeDelete(hist);
   }
}

void PixelMon2DLumiProfiles::Fill(double LB,Identifier &id, const PixelID* pixID, double weight)
{
   const int bec = pixID->barrel_ec(id);
   const int pm  = pixID->phi_module(id);
   int ld = pixID->layer_disk(id);

   if (bec == 2 && Albp) {
      Albp->Fill(LB, ld * 48 + pm, weight);
   } else if (bec == -2 && Clbp) {
      Clbp->Fill(LB, ld * 48 + pm, weight);
   } else if (bec == 4 && DBMAlbp) {
      DBMAlbp->Fill(LB, ld * 4 + pm, weight);
   } else if (bec == -4 && DBMClbp) {
      DBMClbp->Fill(LB, ld * 4 + pm, weight);
   } else if (bec == 0) {
      if (m_doIBL) ld--;
      const int em = pixID->eta_module(id) + 6;
      if (ld == 0 && B0lbp) {
         B0lbp->Fill(LB, em + 13 * pm, weight);
      } else if (ld == 1 && B1lbp) {
         B1lbp->Fill(LB, em + 13 * pm, weight);
      } else if (ld == 2 && B2lbp) {
         B2lbp->Fill(LB, em + 13 * pm, weight);
      } else if (ld == -1 && IBLlbp) {
         IBLlbp->Fill(LB, em + 4 + 20 * pm, weight);
      }
   }
}


void PixelMon2DLumiProfiles::formatHist()
{
   std::string label;
   unsigned int count = 1;
   if (Albp && Clbp) {
      for (unsigned int j = 0; j < PixMon::kNumLayersDisk; ++j) {
         for (unsigned int i = 0; i < PixMon::kNumModulesDisk; ++i) {
            label = PixMon::LayersDisk.at(j) + "A_" + PixMon::ModulesECA.at(i);
            Albp->GetYaxis()->SetBinLabel(count, label.c_str());
            label = PixMon::LayersDisk.at(j) + "C_" + PixMon::ModulesECC.at(i);
            Clbp->GetYaxis()->SetBinLabel(count, label.c_str());
            count++;
         }
      }
      count = 1;
   }
   if (DBMAlbp && DBMClbp) {
      for (unsigned int j = 0; j < PixMon::kNumLayersDBM; ++j) {
         for (unsigned int i = 0; i < PixMon::kNumModulesDBM; ++i) {
            label = PixMon::LayersDBM.at(j) + "A_" + PixMon::ModulesDBM.at(i);
            DBMAlbp->GetYaxis()->SetBinLabel(count, label.c_str());
            label = PixMon::LayersDBM.at(j) + "C_" + PixMon::ModulesDBM.at(i);
            DBMClbp->GetYaxis()->SetBinLabel(count, label.c_str());
            count++;
         }
      }
      count = 1;
   }
   if (B0lbp && B1lbp && B2lbp) {
      for (unsigned int i = 0; i < PixMon::kNumStavesL0; ++i) {
         for (unsigned int j = 0; j < PixMon::kNumModulesBarrel; ++j) {
            label = "L0_" + PixMon::StavesL0.at(i) + "_" + PixMon::ModulesBarrel.at(j);
            B0lbp->GetYaxis()->SetBinLabel(count, label.c_str());
            count++;
         }
      }
      count = 1;
      for (unsigned int i = 0; i < PixMon::kNumStavesL1; ++i) {
         for (unsigned int j = 0; j < PixMon::kNumModulesBarrel; ++j) {
            label = "L1_" + PixMon::StavesL1.at(i) + "_" + PixMon::ModulesBarrel.at(j);
            B1lbp->GetYaxis()->SetBinLabel(count, label.c_str());
            count++;
         }
      }
      count = 1;
      for (unsigned int i = 0; i < PixMon::kNumStavesL2; ++i) {
         for (unsigned int j = 0; j < PixMon::kNumModulesBarrel; ++j) {
            label = "L2_" + PixMon::StavesL2.at(i) + "_" + PixMon::ModulesBarrel.at(j);
            B2lbp->GetYaxis()->SetBinLabel(count, label.c_str());
            count++;
         }
      }
      count = 1;
   }
   if (IBLlbp) {
      for (unsigned int i = 0; i < PixMon::kNumStavesIBL; ++i) {
         for (unsigned int j = 0; j < PixMon::kNumModulesIBL; ++j) {
            label = "IBL_" + PixMon::StavesIBL.at(i) + "_" + PixMon::ModulesIBL.at(j);
            IBLlbp->GetYaxis()->SetBinLabel(count, label.c_str());
            count++;
	 }
      }
   }

   for (auto& hist : m_histograms) {
      if (!hist) continue;
      if (hist == Albp || hist == Clbp) {
         hist->GetYaxis()->SetLabelSize(0.02);
      } else {
         hist->GetYaxis()->SetLabelSize(0.03);
      }        
      hist->SetOption("colz");
   }
}

StatusCode PixelMon2DLumiProfiles::regHist(ManagedMonitorToolBase::MonGroup &group)
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

const bool PixelMon2DLumiProfiles::m_doIBL{true};
