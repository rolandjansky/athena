/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************
* Uses the flat Ntuples created by ShowerShapeThinning.cxx
* Studies different aspects of the shower
* Studies reweighting scheme to have the shower center at (0,0)
* <a.hasib@cern.ch>
**********************************************************/

#include <iostream>

#include "ShowerShapeStudy.h"
#include "TString.h"
#include "TMath.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"


using namespace std;

ShowerShapeStudy::ShowerShapeStudy()
{
   m_debug = 0;

   m_calolayer      = -1;
   m_PCAbin         = 0;
   m_nbinsR         = 0;
   m_nbinsAlpha     = 0;
   m_mincalosize    = 0.;
   m_particle       = "";
   m_particleEnergy = 0.;
   m_etamin         = 0.;
   m_etamax         = 0.;
   m_tolerance      = 0.;

   cellDeta = 0;
   cellDphi = 0;
   cellr    = 0;
   cellz    = 0;
   celleta  = 0;
}


ShowerShapeStudy::~ShowerShapeStudy()
{
}


std::tuple<float, float> ShowerShapeStudy::GetShift(int ievent)
{
   cout << "* Reading input hits file = " << m_hitsNtupleName.c_str() << endl;

   TFile *inputHitsFile = TFile::Open(m_hitsNtupleName.c_str());

   // * get the TTrees from hitsFile

   TTree *hits   = (TTree *)inputHitsFile->Get("hits");
   TTree *global = (TTree *)inputHitsFile->Get("global");

   InitHitsTree(hits, global);

   float sumDetaEnergy = 0;
   float sumDphiEnergy = 0;
   float Etotal        = 0;



   for (int ihit = 0; ihit < hits->GetEntries(); ihit++)
   {
      hits->GetEntry(ihit);

      float eventNumber = m_eventNumber;
      float energy_hit  = m_energy;
      float deta        = m_deta;
      float dphi        = m_dphi;

      if (eventNumber == ievent)
      {
         sumDetaEnergy += deta * energy_hit;
         sumDphiEnergy += dphi * energy_hit;
         Etotal        += energy_hit;
      }
   }

   float etaShift = sumDetaEnergy / Etotal;
   float phiShift = sumDphiEnergy / Etotal;

   return (std::make_tuple(etaShift, phiShift));
}


void ShowerShapeStudy::EachParticleShower()
{


   // * get the input hits file

   cout << "* Reading input hits file = " << m_hitsNtupleName.c_str() << endl;

   TFile *inputHitsFile = TFile::Open(m_hitsNtupleName.c_str());

   // * get the TTrees from hitsFile

   TTree *hitsTree   = (TTree *)inputHitsFile->Get("hits");
   TTree *globalTree = (TTree *)inputHitsFile->Get("global");


   // * initialize the hits and global TTrees
   InitHitsTree(hitsTree, globalTree);

   // * Get the global variables
   globalTree->GetEntry(0);

   float etaCenter = m_etaCenter;
   float phiCenter = m_phiCenter;
   float etaShift  = m_etaShift;
   float phiShift  = m_phiShift;

   //cout << " 0th eta , phi shifts = " << etaShift << " , " << phiShift << endl;


   float cell_r    = m_cellr;
   float cell_z    = m_cellz;
   float cell_eta  = m_celleta;
   float cell_deta = m_cellDeta;
   float cell_dphi = m_cellDphi;

   float sumDetaEnergy = 0;
   float sumDphiEnergy = 0;
   float Etotal        = 0;

   float sumEtaEnergy = 0;
   float sumPhiEnergy = 0;


   TH2F *hdetadphi0  = nullptr;
   TH2F *hdetadphiE0 = nullptr;
   TH1F *hdetaE0     = nullptr;

   TH2F *hEnergy = nullptr;
   TH2F *hEnergyAvg = nullptr;

   TH1F *hDrEnergy = nullptr;
   TH1F *hDrEnergyAvg = nullptr;

   std::string file     = m_topDir + m_particle + "/EachParticleShower_" + m_fileName + ".root";
   TFile       *outfile = new TFile(file.c_str(), "recreate");

   std::string GetBinningFile = m_topDir + m_particle + "/InputDistribution_" + m_particle + "_" + std::to_string(m_particleEnergy) + "GeV" + "_eta_" + std::to_string(m_etamin) + "_" + std::to_string(m_etamax) + ".root";

   cout << " GetBinningFile = " << GetBinningFile << endl;

   TFile *BinFile = TFile::Open(GetBinningFile.c_str());

   std::string BinHistoName = "hEnergy_layer" + std::to_string(m_calolayer) + "_pca" + std::to_string(m_PCAbin);

   cout << " BinHistoName = " << BinHistoName << endl;

   TH2F* BinHisto = (TH2F*)BinFile->Get(BinHistoName.c_str());

   int nBinsAlpha = BinHisto->GetNbinsX();
   int nBinsR = BinHisto->GetNbinsY();




   int particleCount = 0;
   int passEvent     = 0;

   float etaShift2, phiShift2 = 0;

   cout << " nbins alpha, nbins r = " << nBinsAlpha << " , " << nBinsR << endl;


   double Alphabinsx[nBinsAlpha + 1], Rbinsy[nBinsR + 1];




   for (int i = 1; i <= BinHisto->GetNbinsX() + 1; i++) {
      //cout << " x bins ..." << endl;
      cout << BinHisto->GetXaxis()->GetBinLowEdge(i) << endl;
      Alphabinsx[i - 1] = BinHisto->GetXaxis()->GetBinLowEdge(i);
   }

   for (int i = 1; i <= BinHisto->GetNbinsY() + 1; i++) {
      //cout << " y bins..." << endl;
      cout << BinHisto->GetYaxis()->GetBinLowEdge(i) << endl;
      Rbinsy[i - 1] = BinHisto->GetYaxis()->GetBinLowEdge(i);
   }



   for (int ihit = 0; ihit < hitsTree->GetEntries(); ihit++)
   {

      hitsTree->GetEntry(ihit);

      if (m_PCA != m_PCAbin)
      {
         continue;
      }
      passEvent++;



      bool isNewEvent      = false;
      int  NextEventNumber = -1;



      if (ihit != hitsTree->GetEntries() - 1)
      {
         hitsTree->GetEntry(ihit + 1);
         NextEventNumber = m_eventNumber;
      }


      hitsTree->GetEntry(ihit);



      int eventNumber = m_eventNumber;

      float energy_hit = m_energy;
      float eta_hit    = m_etaHit;
      float phi_hit    = m_phiHit;

      float eta_entrance = m_etaEntrance;
      float phi_entrance = m_phiEntrance;

      // float etaShift2 = 0, phiShift2 = 0;
      // if (passEvent == 1 or NextEventNumber != eventNumber)
      // {
      //    tie(etaShift2, phiShift2) = GetShift(eventNumber);
      // }

      // * calculate the corrected deta, dphi

      float deta0 = m_deta - etaShift;
      float dphi0 = m_dphi - phiShift;

      float deta0_mm, dphi0_mm;
      tie(deta0_mm, dphi0_mm) = GetUnitsmm(eta_hit, deta0, dphi0, cell_r, cell_z);

      if (m_debug)
      {
         cout << "deta0 = " << deta0 << ", " << "dphi0 = " << dphi0 << ", deta0_mm =" << deta0_mm << ", dphi0_mm =" << dphi0_mm << endl;
      }

      // * calculate r and alpha co-ordiantes

      float delta_r = TMath::Sqrt(dphi0_mm * dphi0_mm + deta0_mm * deta0_mm);
      float alpha   = TMath::ATan2(dphi0_mm, deta0_mm);

      if (alpha < (TMath::Pi() / 8.))
      {
         alpha = 2 * TMath::Pi() + alpha;
      }

      if (m_debug)
      {
         cout << " delta r = " << delta_r << " mm, alpha = " << alpha << " mm" << endl;
      }


      int nbins = 400;
      int low   = -100;
      int high  = 100;


      cout << "particleCount = " << particleCount << endl;

      if (passEvent == 1)
      {
         cout << " Defining histogram for each particle..." << endl;
         // * define the histogram for the first hit
         hdetaE0     = new TH1F(Form("hdetaE0_%i", eventNumber), Form("hdetaE0_%i", eventNumber), nbins, low, high);
         hdetadphi0  = new TH2F(Form("hdetadphi0_%i", eventNumber), Form("hdetadphi0_%i", eventNumber), nbins, low, high, nbins, low, high);
         hdetadphiE0 = new TH2F(Form("hdetadphiE0_%i", eventNumber), Form("hdetadphiE0_%i", eventNumber), nbins, low, high, nbins, low, high);

         hEnergy = new TH2F(Form("hEnergy_%i", particleCount), Form("hEnergy_%i", particleCount), nBinsAlpha, Alphabinsx, nBinsR, Rbinsy);
         hEnergy->Sumw2();

         hEnergyAvg = new TH2F("hEnergyAvg", "hEnergyAvg", nBinsAlpha, Alphabinsx, nBinsR, Rbinsy);
         hEnergyAvg->Sumw2();


         hDrEnergy = new TH1F(Form("hDrEnergy_%i", particleCount), Form("hDrEnergy_%i", particleCount), nBinsR, Rbinsy);
         hDrEnergy->Sumw2();

         hDrEnergyAvg = new TH1F("hDrEnergyAvg", "hDrEnergyAvg", nBinsR, Rbinsy);
         hDrEnergyAvg->Sumw2();
      }

      hdetaE0->Fill(deta0_mm, energy_hit);
      hdetadphi0->Fill(deta0_mm, dphi0_mm);
      hdetadphiE0->Fill(deta0_mm, dphi0_mm, energy_hit);
      hEnergy->Fill(alpha, delta_r, energy_hit);
      hDrEnergy->Fill(delta_r, energy_hit);
      hDrEnergyAvg->Fill(delta_r, energy_hit);
      hEnergyAvg->Fill(alpha, delta_r, energy_hit);


      if (NextEventNumber != eventNumber or ihit == hitsTree->GetEntries() - 1)
      {
         // if the next event has a different eventNumber then write

         isNewEvent = true;
         particleCount++;

         outfile->cd();
         //detaE0->Write();
         // hdetadphi0->Write();
         // hdetadphiE0->Write();

         hEnergy->Scale(1.0 / (hEnergy->Integral()));
         //hEnergyAvg->Add(hEnergy);

         hDrEnergy->Scale(1.0 / (hDrEnergy->Integral()));
         //hDrEnergyAvg->Add(hDrEnergy);

         hEnergy->Write();
         hDrEnergy->Write();


         delete hdetaE0;
         delete hdetadphi0;
         delete hdetadphiE0;
         delete hEnergy;
      }



      if (isNewEvent and ihit != hitsTree->GetEntries())
      {
         hdetaE0     = new TH1F(Form("hdetaE0_%i", eventNumber), Form("hdetaE0_%i", eventNumber), nbins, low, high);
         hdetadphi0  = new TH2F(Form("hdetadphi0_%i", eventNumber), Form("hdetadphi0_%i", eventNumber), nbins, low, high, nbins, low, high);
         hdetadphiE0 = new TH2F(Form("hdetadphiE0_%i", eventNumber), Form("hdetadphiE0_%i", eventNumber), nbins, low, high, nbins, low, high);
         hEnergy = new TH2F(Form("hEnergy_%i", particleCount), Form("hEnergy_%i", particleCount), nBinsAlpha, Alphabinsx, nBinsR, Rbinsy);

         hDrEnergy = new TH1F(Form("hDrEnergy_%i", particleCount), Form("hDrEnergy_%i", particleCount), nBinsR, Rbinsy);
      }


      if (/*particleCount == 200 or*/ ihit == hitsTree->GetEntries() - 1)
      {
         cout << " Reach number of particles = " << particleCount << " exiting..." << endl;
         outfile->cd();
         hEnergyAvg->Scale(1.0 / particleCount);
         hEnergyAvg->Scale(1.0 / hEnergyAvg->Integral());
         hEnergyAvg->Write();
         hDrEnergyAvg->Scale(1.0 / particleCount);
         hDrEnergyAvg->Scale(1.0 / hDrEnergyAvg->Integral());
         hDrEnergyAvg->Write();
         outfile->Close();
         break;
      }
   }
}


void ShowerShapeStudy::InvesitageShowerCenter(std::string inputHits)
{
   // * get the input hits file

   cout << "* Reading input hits file = " << inputHits.c_str() << endl;

   TFile *inputHitsFile = TFile::Open(inputHits.c_str());

   // * get the TTrees from hitsFile

   TTree *hitsTree   = (TTree *)inputHitsFile->Get("hits");
   TTree *globalTree = (TTree *)inputHitsFile->Get("global");


   // * initialize the hits and global TTrees
   InitHitsTree(hitsTree, globalTree);

   // * Get the global variables
   globalTree->GetEntry(0);

   float etaCenter = m_etaCenter;
   float phiCenter = m_phiCenter;
   float etaShift  = m_etaShift;
   float phiShift  = m_phiShift;

   //cout << " 0th eta , phi shifts = " << etaShift << " , " << phiShift << endl;


   float cell_r    = m_cellr;
   float cell_z    = m_cellz;
   float cell_eta  = m_celleta;
   float cell_deta = m_cellDeta;
   float cell_dphi = m_cellDphi;

   float sumDetaEnergy = 0;
   float sumDphiEnergy = 0;
   float Etotal        = 0;

   float sumEtaEnergy = 0;
   float sumPhiEnergy = 0;


   float alphaMin = TMath::Pi() / 8; // * to cover the crack region in a single bin
   float alphaMax = 2 * TMath::Pi() + alphaMin;
   float rMax     = 10000;

   TH1F *h_dr2_orig  = new TH1F("h_dr2_orig", "h_dr2_orig", 75000000, 0, rMax * rMax);
   TH1F *h_dr2E_orig = new TH1F("h_dr2E_orig", "h_dr2E_orig", 75000000, 0, rMax * rMax);

   //TH1F *h_dr2  = new TH1F("h_dr2", "h_dr2", 75000000, 0, rMax * rMax);
   //TH1F *h_dr2E = new TH1F("h_dr2E", "h_dr2E", 75000000, 0, rMax * rMax);

   //TH1F *hdeta0 = new TH1F("hdeta0", "hdeta0", 100, -0.2, 0.2);
   //TH1F *hdphi0 = new TH1F("hdphi0", "hdphi0", 100, -0.2, 0.2);

   //TH1F *hdeta0_mm = new TH1F("hdeta0_mm", "hdeta0_mm", 10000, -500, 500);
   //TH1F *hdphi0_mm = new TH1F("hdphi0_mm", "hdphi0_mm", 10000, -500, 500);

   //TH1F *hdeta1 = new TH1F("hdeta1", "hdeta1", 100, -0.2, 0.2);
   //TH1F *hdphi1 = new TH1F("hdphi1", "hdphi1", 100, -0.2, 0.2);


   TH1F *hdeta0_mm = new TH1F("hdeta0_mm", "hdeta0_mm", 1000, -500, 500);
   TH1F *hdphi0_mm = new TH1F("hdphi0_mm", "hdphi0_mm", 1000, -500, 500);

   TH1F *hdeta1_mm = new TH1F("hdeta1_mm", "hdeta1_mm", 1000, -500, 500);
   TH1F *hdphi1_mm = new TH1F("hdphi1_mm", "hdphi1_mm", 1000, -500, 500);

   //TH1F *hdeta2 = new TH1F("hdeta2", "hdeta2", 100, -0.2, 0.2);
   //TH1F *hdphi2 = new TH1F("hdphi2", "hdphi2", 100, -0.2, 0.2);

   //TH1F *hdeta2_mm = new TH1F("hdeta2_mm", "hdeta2_mm", 10000, -500, 500);
   //TH1F *hdphi2_mm = new TH1F("hdphi2_mm", "hdphi2_mm", 10000, -500, 500);

   //TH1F *hdeta0E = new TH1F("hdeta0E", "hdeta0E", 100, -0.2, 0.2);
   //TH1F *hdphi0E = new TH1F("hdphi0E", "hdphi0E", 100, -0.2, 0.2);

   //TH1F *hdeta0E_mm = new TH1F("hdeta0E_mm", "hdeta0E_mm", 10000, -500, 500);
   //TH1F *hdphi0E_mm = new TH1F("hdphi0E_mm", "hdphi0E_mm", 10000, -500, 500);

   //TH1F *hdeta1E = new TH1F("hdeta1E", "hdeta1E", 100, -0.2, 0.2);
   //TH1F *hdphi1E = new TH1F("hdphi1E", "hdphi1E", 100, -0.2, 0.2);

   TH1F *hdeta0E_mm = new TH1F("hdeta0E_mm", "hdeta0E_mm", 1000, -500, 500);
   TH1F *hdphi0E_mm = new TH1F("hdphi0E_mm", "hdphi0E_mm", 1000, -500, 500);

   TH1F *hdeta1E_mm = new TH1F("hdeta1E_mm", "hdeta1E_mm", 1000, -500, 500);
   TH1F *hdphi1E_mm = new TH1F("hdphi1E_mm", "hdphi1E_mm", 1000, -500, 500);

   TH2F *hdeta0dphi0E_mm = new TH2F("hdeta0dphi0E_mm", "hdeta0dphi0E_mm", 1000, -500, 500, 1000, -500, 500);
   TH2F *hdeta0dphi0_mm  = new TH2F("hdeta0dphi0_mm", "hdeta0dphi0_mm", 1000, -500, 500, 1000, -500, 500);


   TH2F *hdeta1dphi1E_mm = new TH2F("hdeta1dphi1E_mm", "hdeta1dphi1E_mm", 1000, -500, 500, 1000, -500, 500);
   TH2F *hdeta1dphi1_mm  = new TH2F("hdeta1dphi1_mm", "hdeta1dphi1_mm", 1000, -500, 500, 1000, -500, 500);


   // TH1F *hdeta2E = new TH1F("hdeta2E", "hdeta2E", 100, -0.2, 0.2);
   //TH1F *hdphi2E = new TH1F("hdphi2E", "hdphi2E", 100, -0.2, 0.2);

   //TH1F *hdeta2E_mm = new TH1F("hdeta2E_mm", "hdeta2E_mm", 10000, -500, 500);
   //TH1F *hdphi2E_mm = new TH1F("hdphi2E_mm", "hdphi2E_mm", 10000, -500, 500);

   TH1F *h_alpha0  = new TH1F("h_alpha0", "h_alpha0", 630000, alphaMin, alphaMax);
   TH1F *h_alpha0E = new TH1F("h_alpha0E", "h_alpha0E", 630000, alphaMin, alphaMax);

   TH1F *h_alpha1  = new TH1F("h_alpha1", "h_alpha1", 630000, alphaMin, alphaMax);
   TH1F *h_alpha1E = new TH1F("h_alpha1E", "h_alpha1E", 630000, alphaMin, alphaMax);


   cout << " Running over nhits = " << hitsTree->GetEntries() << endl;

   for (int ihit = 0; ihit < hitsTree->GetEntries(); ihit++)
   {
      hitsTree->GetEntry(ihit);

      if (m_PCA != m_PCAbin)
      {
         continue;
      }

      int eventNumber = m_eventNumber;

      float energy_hit = m_energy;
      float eta_hit    = m_etaHit;
      float phi_hit    = m_phiHit;

      float eta_entrance = m_etaEntrance;
      float phi_entrance = m_phiEntrance;

      // float deta = m_deta;
      // float dphi = m_dphi;


      // * calculate the corrected deta, dphi

      float deta0 = m_deta;
      float dphi0 = m_dphi;

      float deta0_mm, dphi0_mm;
      tie(deta0_mm, dphi0_mm) = GetUnitsmm(eta_hit, deta0, dphi0, cell_r, cell_z);

      hdeta0_mm->Fill(deta0_mm);
      hdeta0E_mm->Fill(deta0_mm, energy_hit);

      hdphi0_mm->Fill(dphi0_mm);
      hdphi0E_mm->Fill(dphi0_mm, energy_hit);

      hdeta0dphi0_mm->Fill(deta0_mm, dphi0_mm);
      hdeta0dphi0E_mm->Fill(deta0_mm, dphi0_mm, energy_hit);

      float delta_r0 = TMath::Sqrt(dphi0_mm * dphi0_mm + deta0_mm * deta0_mm);
      float alpha0   = TMath::ATan2(dphi0_mm, deta0_mm);

      if (alpha0 < (TMath::Pi() / 8.))
      {
         alpha0 = 2 * TMath::Pi() + alpha0;
      }

      h_alpha0->Fill(alpha0);
      h_alpha0E->Fill(alpha0, energy_hit);


      float deta = m_deta - etaShift;
      float dphi = DeltaPhi(m_dphi, phiShift);

      //  if (m_debug)
      //{
      //cout << " deta = " << m_deta << ", dphi = " << m_dphi << ", deta corr = " << deta << ", dphi corr = " << dphi << endl;
      //}

      //hdeta1->Fill(deta);
      //hdeta1E->Fill(deta, energy_hit);

      //  hdphi1->Fill(dphi);
      //  hdphi1E->Fill(dphi, energy_hit);



      // * get the uncorrected co-ordinate in mm
      float deta_mm, dphi_mm;
      tie(deta_mm, dphi_mm) = GetUnitsmm(eta_hit, deta, dphi, cell_r, cell_z);

      hdeta1_mm->Fill(deta_mm);
      hdeta1E_mm->Fill(deta_mm, energy_hit);

      hdphi1_mm->Fill(dphi_mm);
      hdphi1E_mm->Fill(dphi_mm, energy_hit);

      hdeta1dphi1_mm->Fill(deta_mm, dphi_mm);
      hdeta1dphi1E_mm->Fill(deta_mm, dphi_mm, energy_hit);

      float alpha1 = TMath::ATan2(dphi_mm, deta_mm);

      if (alpha1 < (TMath::Pi() / 8.))
      {
         alpha1 = 2 * TMath::Pi() + alpha1;
      }

      h_alpha1->Fill(alpha1);
      h_alpha1E->Fill(alpha1, energy_hit);



      //cout << " deta = " << deta << " deta_mm =" << deta_mm << "dphi = " << dphi << "dph_mm =" << dphi_mm << endl;

      float delta_r2 = dphi_mm * dphi_mm + deta_mm * deta_mm;

      h_dr2_orig->Fill(delta_r2);
      h_dr2E_orig->Fill(delta_r2, energy_hit);


      if (delta_r2 < 100)
      {
         sumDetaEnergy += deta * energy_hit;
         sumDphiEnergy += dphi * energy_hit;
         sumEtaEnergy  += eta_hit * energy_hit;
         sumPhiEnergy  += phi_hit * energy_hit;

         Etotal += energy_hit;
      }
   }

   float etaShift2 = sumDetaEnergy / Etotal;
   float phiShift2 = sumDphiEnergy / Etotal;

   float etaCenter2 = sumEtaEnergy / Etotal;
   float phiCenter2 = sumPhiEnergy / Etotal;

   cout << " 1st eta center, phi center =" << etaCenter2 << ", " << phiCenter2 << endl;
   cout << " 1st eta , phi shifts = " << etaShift2 << " , " << phiShift2 << endl;

   //float alphaMin = TMath::Pi() / 8; // * to cover the crack region in a single bin
   //float alphaMax = 2 * TMath::Pi() + alphaMin;
   //float rMax     = 10000;


   TH1F *h_alpha  = new TH1F("h_alpha", "h_alpha", 630000, alphaMin, alphaMax);
   TH1F *h_alphaE = new TH1F("h_alphaE", "h_alphaE", 630000, alphaMin, alphaMax);

   TH2F *hdeta2dphi2E_mm = new TH2F("hdeta2dphi2E_mm", "hdeta2dphi2E_mm", 1000, -500, 500, 1000, -500, 500);
   TH2F *hdeta2dphi2_mm  = new TH2F("hdeta2dphi2_mm", "hdeta2dphi2_mm", 1000, -500, 500, 1000, -500, 500);


   for (int ihit = 0; ihit < hitsTree->GetEntries(); ihit++)
   {
      hitsTree->GetEntry(ihit);

      //* apply pca bin choice
      if (m_PCA != m_PCAbin)
      {
         continue;
      }


      int eventNumber = m_eventNumber;

      float energy_hit = m_energy;
      float eta_hit    = m_etaHit;
      float phi_hit    = m_phiHit;

      float eta_entrance = m_etaEntrance;
      float phi_entrance = m_phiEntrance;

      //float deta = m_deta;
      //float dphi = m_dphi;

      // * calculate the corrected deta, dphi

      float deta = m_deta - etaShift;
      float dphi = DeltaPhi(m_dphi, phiShift);

      //if (m_debug)
      //{
      //cout << " deta = " << m_deta << ", dphi = " << m_dphi << ", deta corr = " << deta << ", dphi corr = " << dphi << endl;
      //}


      // * get the uncorrected co-ordinate in mm
      float deta_mm, dphi_mm;
      tie(deta_mm, dphi_mm) = GetUnitsmm(eta_hit, deta, dphi, cell_r, cell_z);

      //cout<<" deta mm = "<<deta_mm<<endl;
      //cout<<" dphi mm = "<<dphi_mm<<endl;

      float delta_r2 = (dphi_mm * dphi_mm + deta_mm * deta_mm);

      // if (delta_r2 > 1000)
      // {
      //    continue;
      // }
      // float hit_eta_corr = eta_hit - etaCenter2;
      // float hit_phi_corr = phi_hit - phiCenter2;


      float deta_corr = deta - etaShift2;
      float dphi_corr = DeltaPhi(dphi, phiShift2);


      float deta_corr_mm, dphi_corr_mm;
      tie(deta_corr_mm, dphi_corr_mm) = GetUnitsmm(eta_hit, deta_corr, dphi_corr, cell_r, cell_z);


      hdeta2dphi2_mm->Fill(deta_corr_mm, dphi_corr_mm);
      hdeta2dphi2E_mm->Fill(deta_corr_mm, dphi_corr_mm, energy_hit);


      // * calculate r and alpha co-ordiantes

      float delta_r = TMath::Sqrt(dphi_corr_mm * dphi_corr_mm + deta_corr_mm * deta_corr_mm);
      float alpha   = TMath::ATan2(dphi_corr_mm, deta_corr_mm);

      // h_dr2->Fill(delta_r * delta_r);
      // h_dr2E->Fill(delta_r * delta_r, energy_hit);


      if (alpha < (TMath::Pi() / 8.))
      {
         alpha = 2 * TMath::Pi() + alpha;
      }

      h_alpha->Fill(alpha);
      h_alphaE->Fill(alpha, energy_hit);
   }



   std::string showercenter = m_topDir + m_particle + "/InvesitageShowerCenter_" + m_fileName + ".root";
   TFile       *outfile     = new TFile(showercenter.c_str(), "recreate");
   outfile->cd();
   //hdeta1->Write();
   //hdeta1E->Write();
   hdeta0_mm->Write();
   hdeta0E_mm->Write();

   hdeta1_mm->Write();
   hdeta1E_mm->Write();
   //hdphi1->Write();
   //hdphi1E->Write();
   hdphi0_mm->Write();
   hdphi0E_mm->Write();

   hdphi1_mm->Write();
   hdphi1E_mm->Write();
   //h_dr2->Write();
   //h_dr2E->Write();
   h_dr2_orig->Write();
   h_dr2E_orig->Write();


   hdeta0dphi0E_mm->Write();
   hdeta0dphi0_mm->Write();

   hdeta1dphi1E_mm->Write();
   hdeta1dphi1_mm->Write();

   h_alpha0->Write();
   h_alpha0E->Write();

   h_alpha1->Write();
   h_alpha1E->Write();

   h_alpha->Write();
   h_alphaE->Write();

   hdeta2dphi2_mm->Write();
   hdeta2dphi2E_mm->Write();


   outfile->Close();
}


std::tuple<float, float> ShowerShapeStudy::GetUnitsmm(float eta_hit, float d_eta, float d_phi, float cell_r, float cell_z)
{
   float phi_dist2r = 1.0;
   float dist000    = TMath::Sqrt(cell_r * cell_r + cell_z * cell_z);

   float eta_jakobi = TMath::Abs(2.0 * TMath::Exp(-eta_hit) / (1.0 + TMath::Exp(-2 * eta_hit)));

   d_eta = d_eta * eta_jakobi * dist000;
   d_phi = d_phi * cell_r * phi_dist2r;

   if (m_debug)
   {
      cout << " in function deta = " << d_eta << " dphi = " << d_phi << endl;
   }
   return std::make_tuple(d_eta, d_phi);
}


float ShowerShapeStudy::DeltaPhi(float phi1, float phi2)
{
   double result = phi1 - phi2;

   while (result > TMath::Pi())
   {
      result -= 2 * TMath::Pi();
   }
   while (result <= -TMath::Pi())
   {
      result += 2 * TMath::Pi();
   }
   return result;
}


void ShowerShapeStudy::InitHitsTree(TTree *hitsNtuple, TTree *globalNtuple)
{
   m_eventNumber = -1;
   m_energy      = 0.;
   m_etaHit      = 0.;
   m_phiHit      = 0.;
   m_etaEntrance = 0.;
   m_phiEntrance = 0.;
   m_deta        = 0.;
   m_dphi        = 0.;
   m_PCA         = 0;

   m_etaCenter = 0.;
   m_phiCenter = 0.;
   m_etaShift  = 0.;
   m_phiShift  = 0.;
   m_cellr     = 0.;
   m_cellz     = 0.;
   m_celleta   = 0.;
   m_cellDeta  = 0.;
   m_cellDphi  = 0.;

   hitsNtuple->SetBranchAddress("eventNumber", &m_eventNumber);
   hitsNtuple->SetBranchAddress("energy_hit", &m_energy);
   hitsNtuple->SetBranchAddress("eta_hit", &m_etaHit);
   hitsNtuple->SetBranchAddress("phi_hit", &m_phiHit);
   hitsNtuple->SetBranchAddress("eta_entrance", &m_etaEntrance);
   hitsNtuple->SetBranchAddress("phi_entrance", &m_phiEntrance);
   hitsNtuple->SetBranchAddress("delta_eta", &m_deta);
   hitsNtuple->SetBranchAddress("delta_phi", &m_dphi);
   hitsNtuple->SetBranchAddress("delta_eta", &m_deta);
   hitsNtuple->SetBranchAddress("pca_bin", &m_PCA);

   globalNtuple->SetBranchAddress("eta_center", &m_etaCenter);
   globalNtuple->SetBranchAddress("phi_center", &m_phiCenter);
   globalNtuple->SetBranchAddress("eta_shift", &m_etaShift);
   globalNtuple->SetBranchAddress("phi_shift", &m_phiShift);
   globalNtuple->SetBranchAddress("cell_r", &m_cellr);
   globalNtuple->SetBranchAddress("cell_z", &m_cellz);
   globalNtuple->SetBranchAddress("cell_eta", &m_celleta);
   globalNtuple->SetBranchAddress("cell_deta", &m_cellDeta);
   globalNtuple->SetBranchAddress("cell_dphi", &m_cellDphi);
}
