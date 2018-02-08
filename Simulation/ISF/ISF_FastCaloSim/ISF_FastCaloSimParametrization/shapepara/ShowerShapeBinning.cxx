/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Class to implement:
 *
 *      - calculate optimal binning
 *      - produce distributions
 *      - create TTree for NN regression
 *
 *    <a.hasib@cern.ch>
 *****************************************************/
#include <iostream>
#include <tuple>
#include <algorithm>

#include "TString.h"
#include "TMath.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

#include "ShowerShapeBinning.h"



using namespace std;


ShowerShapeBinning::ShowerShapeBinning()
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
}


ShowerShapeBinning::~ShowerShapeBinning()
{
   if (hEnergyDensity)
   {
      delete hEnergyDensity;
   }
   if (hEnergy)
   {
      delete hEnergy;
   }
   if (hLnEnergy)
   {
      delete hLnEnergy;
   }
   if (hLnEnergyDensity)
   {
      delete hLnEnergyDensity;
   }
}


void ShowerShapeBinning::CreateHitsAlphaDrNtuple(std::string inputHits)
{
   // * get the input hits file

   cout << "* Reading input hits file = " << inputHits.c_str() << endl;

   TFile *inputHitsFile = TFile::Open(inputHits.c_str());

   // * get the TTrees from hitsFile

   TTree *hitsTree   = (TTree *)inputHitsFile->Get("hits");
   TTree *globalTree = (TTree *)inputHitsFile->Get("global");

   TH1F* hEventEnergy = (TH1F*)inputHitsFile->Get("hEventEnergy");
   TH1F* hEventHit = (TH1F*)inputHitsFile->Get("hEventHit");


   // * initialize the hits and global TTrees
   InitHitsTree(hitsTree, globalTree);

   // * Get the global variables
   globalTree->GetEntry(0);


   float etaCenter = m_etaCenter;
   float phiCenter = m_phiCenter;
   float etaShift  = m_etaShift;
   float phiShift  = m_phiShift;

   float cell_r    = m_cellr;
   float cell_z    = m_cellz;
   float cell_eta  = m_celleta;
   float cell_deta = m_cellDeta;
   float cell_dphi = m_cellDphi;

   cout << " ==> eta, phi shifts = " << m_etaShift << " , " << m_phiShift << endl;

   if (m_debug)
   {
      cout << " eta center = " << etaCenter << ", phi center = " << phiCenter << " , eta shift = " << etaShift << " , phi shift = " << phiShift << ", cell r = " << cell_r << ", cell z = " << cell_z << ", cell eta = " << cell_eta << ", cell deta = " << cell_deta << ", cell dpi = " << cell_dphi << endl;
   }

   // * define alpha and dr histograms needed to determine binning

   float alphaMin = TMath::Pi() / 8; // * to cover the crack region in a single bin
   float alphaMax = 2 * TMath::Pi() + alphaMin;
   float rMax     = 10000;

   // * use fine binning for the initial histograms
   TH1F *h_alpha = new TH1F("h_alpha", "h_alpha", 630000, alphaMin, alphaMax);
   TH1F *h_alphaE = new TH1F("h_alphaE", "h_alphaE", 630000, alphaMin, alphaMax);

   TH1F *h_dr_init    = new TH1F("h_dr_init", "h_dr_init", 75000000, 0, rMax);
   TH1F *h_drE_init    = new TH1F("h_drE_init", "h_dr_init", 75000000, 0, rMax);



   TH1F *hdeta = new TH1F("hdeta", "hdeta", 100, -0.2, 0.2);
   TH1F *hdphi = new TH1F("hdphi", "hdphi", 100, -0.2, 0.2);

   TH1F *hdetaE = new TH1F("hdetaE", "hdetaE", 100, -0.2, 0.2);
   TH1F *hdphiE = new TH1F("hdphiE", "hdphiE", 100, -0.2, 0.2);

   TH1F *hdeta_corr = new TH1F("hdeta_corr", "hdeta_corr", 100, -0.2, 0.2);
   TH1F *hdphi_corr = new TH1F("hdphi_corr", "hdphi_corr", 100, -0.2, 0.2);

   //* plot energy heatmap before binning optimization
   TH2F *hDetaDphi  = new TH2F("hDetaDphi", "hDetaDphi", 100, -0.02, 0.02, 100, -0.02, 0.02);
   TH2F *hDetaDphiE = new TH2F("hDetaDphiE", "hDetaDphiE", 100, -0.02, 0.02, 100, -0.02, 0.02);

   TH2F *hDetaDphi_corr  = new TH2F("hDetaDphi_corr", "hDetaDphi_corr", 100, -0.02, 0.02, 100, -0.02, 0.02);
   TH2F *hDetaDphiE_corr = new TH2F("hDetaDphiE_corr", "hDetaDphiE_corr", 100, -0.02, 0.02, 100, -0.02, 0.02);


   // * define output file to save h_alpha h_dr and TTree with alpha, dr and energy

   std::string HitsAlphaDr = m_topDir + m_particle + "/HitsAlphaDr_" + m_fileName + ".root";
   if (m_debug)
   {
      cout << " hits alpha dr = " << HitsAlphaDr << endl;
   }


   TFile *hitsAlphaDrFile     = new TFile(HitsAlphaDr.c_str(), "recreate");
   TTree *m_hitsAlphaDrNtuple = new TTree("hitsAlphaDr", "hitsAlphaDr");
   BookHitsAlphaDrTree(m_hitsAlphaDrNtuple);




   // * loop through all hits

   int eventNumber = -1;
   bool newEvent = false;
   float eventEnergy = 0.;
   int eventHit = 0;
   float energy_hit = 0.;
   float total_energy = 0.;
   int nSkipHits = 0;
   float eta_hit = 0.;
   float phi_hit = 0.;
   float eta_entrance = 0.;
   float phi_entrance = 0.;
   float deta = 0.;
   float dphi = 0.;

   float cutoff = 0.95; // cutoff energy fraction

   cout << " Looping over nhits = " << hitsTree->GetEntries() << endl;

   int nSkpHitsTotal = 0;

   for (int ihit = 0; ihit < hitsTree->GetEntries(); ihit++)
   {
      hitsTree->GetEntry(ihit);


      // read the TTree variables
      eventNumber = m_eventNumber;
      newEvent = m_newEvent;

      if (newEvent) {

         // cout << " total energy, skipped hits in last event = " << total_energy << " , " << nSkipHits << endl;
         eventEnergy = hEventEnergy->GetBinContent(eventNumber);
         eventHit = hEventHit->GetBinContent(eventNumber);
         total_energy = 0.; // set the total energy to zero for each event
         nSkipHits = 0; // reset the number of hits skipped
         // cout << " event no = " << eventNumber << " event energy = " << eventEnergy << "event hit =" << eventHit << "new event? =" << newEvent << " cutoff = " << cutoff << endl;


      }

      energy_hit = m_energy;
      total_energy += energy_hit;
      eta_hit    = m_etaHit;
      phi_hit    = m_phiHit;

      eta_entrance = m_etaEntrance;
      phi_entrance = m_phiEntrance;

      deta = m_deta;
      dphi = m_dphi;


      float deta_mm, dphi_mm;
      tie(deta_mm, dphi_mm) = GetUnitsmm(eta_hit, deta, dphi, cell_r, cell_z);

      hdeta->Fill(deta_mm);
      hdphi->Fill(dphi_mm);

      hdetaE->Fill(deta_mm, energy_hit);
      hdphiE->Fill(dphi_mm, energy_hit);



      // * calculate the corrected deta, dphi


      float deta_corr = deta; // - etaShift;
      float dphi_corr = dphi; // - phiShift;

      if (m_debug)
      {
         cout << "energy hit = " << energy_hit << ", eta hit = " << eta_hit << ", deta = " << deta << ", dphi = " << dphi << ", deta corr = " << deta_corr << ", dphi corr = " << dphi_corr << endl;
      }


      // * get the co-ordinates in mm

      float deta_corr_mm, dphi_corr_mm;
      tie(deta_corr_mm, dphi_corr_mm) = GetUnitsmm(eta_hit, deta_corr, dphi_corr, cell_r, cell_z);

      hdeta_corr->Fill(deta_corr);
      hdphi_corr->Fill(dphi_corr);



      if (m_debug)
      {
         cout << "in mm deta corr = " << deta_corr_mm << " dphi corr = " << dphi_corr_mm << endl;
      }


      // * save TH2 of eta-phi w/ and w/o energy weighted
      hDetaDphi->Fill(deta, dphi);
      hDetaDphiE->Fill(deta, dphi, energy_hit);

      hDetaDphi_corr->Fill(deta_corr, dphi_corr);
      hDetaDphiE_corr->Fill(deta_corr, dphi_corr, energy_hit);


      // * calculate r and alpha co-ordiantes

      float delta_r = TMath::Sqrt(dphi_corr_mm * dphi_corr_mm + deta_corr_mm * deta_corr_mm);
      float alpha   = TMath::ATan2(dphi_corr_mm, deta_corr_mm);

      if (alpha < (TMath::Pi() / 8.))
      {
         alpha = 2 * TMath::Pi() + alpha;
      }

      if (m_debug)
      {
         cout << " delta r = " << delta_r << " mm, alpha = " << alpha << " mm" << endl;
      }

      // if (total_energy <= eventEnergy * cutoff) {
      h_alpha->Fill(alpha);
      h_alphaE->Fill(alpha, energy_hit);
      h_dr_init->Fill(delta_r);
      h_drE_init->Fill(delta_r, energy_hit);

      // } else {
      //    nSkipHits++;
      //    nSkpHitsTotal++;

      // cout << " energy reached = " << cutoff << " of total event energy.. Skipping hits = " << ihit << endl;
      //}

      // cout << " Total number of hits skipped = " << nSkpHitsTotal << endl;
      // * fill the TTree
      m_eventNumber = eventNumber;
      m_alpha       = alpha;
      m_dr          = delta_r;
      m_energy      = energy_hit;
      m_hitsAlphaDrNtuple->Fill();


   }//end loop over hits

   int bins;
   double binmax;

   tie(bins, binmax) = GetBinUpEdge(h_drE_init);
   cout.precision(8);
   cout << " bins = " << bins << " binmax = " << binmax << endl;




   TH1F *h_dr = new TH1F("h_dr", "h_dr", bins + 1, 0, binmax);
   TH1F *h_drE = new TH1F("h_drE", "h_drE", bins + 1, 0, binmax);


   for (int i = 0; i < h_dr->GetNbinsX(); i ++) {

      h_dr->SetBinContent(i, h_dr_init->GetBinContent(i));
      h_drE->SetBinContent(i, h_drE_init->GetBinContent(i));
   }



   hitsAlphaDrFile->cd();
   m_hitsAlphaDrNtuple->Write();
   globalTree->CloneTree()->Write();
   h_alpha->Write();
   h_alphaE->Write();
   h_dr_init->Write();
   h_drE_init->Write();
   h_dr->Write();
   h_drE->Write();


//
//  hdeta_corr->Write();
//  hdphi_corr->Write();
//

   hdeta->Write();
   hdetaE->Write();
   hdeta_corr->Write();

   hdphi->Write();
   hdphiE->Write();
   hdphi_corr->Write();

   hDetaDphi->Write();
   hDetaDphiE->Write();
   hDetaDphi_corr->Write();
   hDetaDphiE_corr->Write();

   hitsAlphaDrFile->Close();
   inputHitsFile->Close();
}


void ShowerShapeBinning::RunBinning()
{
   std::string HitsAlphaDr = m_topDir + m_particle + "/HitsAlphaDr_" + m_fileName + ".root";
   if (m_debug)
   {
      cout << " hits alpha dr = " << HitsAlphaDr << endl;
   }

   // * identify the file
   TFile *hitsAlphaDrFile = TFile::Open(HitsAlphaDr.c_str());

   // * read the histograms needed
   h_alpha = (TH1F *)hitsAlphaDrFile->Get("h_alpha");
   h_dr    = (TH1F *)hitsAlphaDrFile->Get("h_dr");

   // * initialize the TTrees needed
   TTree *hitsAlphaDrTree = (TTree *)hitsAlphaDrFile->Get("hitsAlphaDr");
   TTree *globalTree      = (TTree *)hitsAlphaDrFile->Get("global");
   InitHitsAlphaDrTree(hitsAlphaDrTree, globalTree);


   // * get bin alpha bin boundaries

   int nAlphaBins       = m_nbinsAlpha;
   int nAlphaBoundaries = nAlphaBins + 1;
   xbinsAlpha.clear();
   xbinsAlpha = Getxbins(h_alpha, nAlphaBins);

   float xAlphaBins[nAlphaBoundaries];
   for (int i = 0; i < nAlphaBoundaries; i++)
   {
      xAlphaBins[i] = xbinsAlpha.at(i);

      // if (m_debug)
      // {
      cout << " BIN ALPHA  " << i << ":  " << xAlphaBins[i] << endl;
      // }
   }

   // * r binning is iterative to ensure no empty bins

   bool isEmptyBin = true;
   int  incr       = 0;

   // * hack to know the final nbinsR reached

   int final_nbinsR = -1;

   while (isEmptyBin)
   {
      if (incr == m_nbinsR - 2)
      {
         cout << " * ERROR: No empty bin requirement cannot be reached. Decrease tolerance! " << endl;
         exit(EXIT_FAILURE);
      }

      int nRbinsMax    = m_nbinsR;
      int nRBins       = nRbinsMax - incr;
      int nRBoundaries = nRBins + 1;

      cout << " Starting r binning with nbins = " << nRBins << endl;

      std::vector<float> ybinsr = Getxbins(h_dr, nRBins);

      // * first combine the up to last 5 bins with bin width calculated with nRbinsMax
      // * if there is still empty bin, calculate bin width with nRbinsMax-1 and repeat



      for (int ibin = 0; ibin < 5; ibin++)
      {
         if (ibin > 0)
         {
            cout << " * trying to combine bin = " << ibin << endl;
         }

         if (ibin > 0)
         {
            nRBoundaries = nRBoundaries - 1;
         }

         float yRBins[nRBoundaries];

         for (int i = 0; i < nRBoundaries; i++)
         {
            yRBins[i] = ybinsr.at(i);

            if (ibin > 0)
            {
               yRBins[nRBoundaries - 1] = ybinsr.at(ybinsr.size() - 1);
            }
            if (m_debug)
            {
               cout << " BIN R " << i << ": " << yRBins[i] << endl;
            }
         }

         // * save the final r binning selected
         ybinsR.clear();
         for (unsigned int i = 0; i < sizeof(yRBins); i++)
         {
            ybinsR.push_back(yRBins[i]);

            final_nbinsR = sizeof(yRBins);
         }


         // * define histograms needed

         hHits            = new TH2F("hHits", "hHits", nAlphaBins, xAlphaBins, nRBoundaries - 1, yRBins);
         hEnergyDensity   = new TH2F("hEnergyDensity", "hEnergyDensity", nAlphaBins, xAlphaBins, nRBoundaries - 1, yRBins);
         hEnergy          = new TH2F("hEnergy", "hEnergy", nAlphaBins, xAlphaBins, nRBoundaries - 1, yRBins);
         hDrEnergy          = new TH1F("hDrEnergy", "hDrEnergy", nRBoundaries - 1, yRBins);
         hLnEnergy        = new TH2F("hLnEnergy", "hLnEnergy", nAlphaBins, xAlphaBins, nRBoundaries - 1, yRBins);
         hLnEnergyDensity = new TH2F("hLnEnergyDensity", "hLnEnergyDensity", nAlphaBins, xAlphaBins, nRBoundaries - 1, yRBins);

         hGradient = new TH1F("hGradient", "hGradient", nRBoundaries - 1, yRBins);


         // * loop over events to fill histograms with this binning
         cout << " * Start looping over " << hitsAlphaDrTree->GetEntries() << " hits ..." << endl;

         //double nevent = 0.0;


         for (int ihit = 0; ihit < hitsAlphaDrTree->GetEntries(); ihit++)
         {
            if (ihit % (hitsAlphaDrTree->GetEntries() / 10) == 0)
            {
               cout << " Hit: " << ihit << endl;
            }

            bool isNewEvent      = false;
            int  NextEventNumber = -1;

            if (ihit != hitsAlphaDrTree->GetEntries() - 1)
            {
               hitsAlphaDrTree->GetEntry(ihit + 1);
               NextEventNumber = m_eventNumber;
            }

            hitsAlphaDrTree->GetEntry(ihit);

            int eventNumber = m_eventNumber;


            if (ihit == 0)
            {
               // * define the histogram for the first hit
               halphadrE = new TH2F(Form("halphadrE_%i", eventNumber), Form("halphadrE_%i", eventNumber), nAlphaBins, xAlphaBins, nRBoundaries - 1, yRBins);
               halphadrE->Sumw2();

               hDrE = new TH1F(Form("hDrE_%i", eventNumber), Form("hDrE_%i", eventNumber), nRBoundaries - 1, yRBins);
               hDrE->Sumw2();
            }

            float alpha  = m_alpha;
            float dr     = m_dr;
            float energy = m_energy;

            if (m_debug)
            {
               cout << " alpha = " << alpha << " dr = " << dr << " energy = " << energy << endl;
            }


            hHits->Fill(alpha, dr, 1);
            halphadrE->Fill(alpha, dr, energy);
            hDrE->Fill(dr, energy);


            if (NextEventNumber != eventNumber or ihit == hitsAlphaDrTree->GetEntries() - 1)
            {
               // if the next event has a different eventNumber then normalize

               nevent += 1.0;
               isNewEvent = true;

               if (m_debug)
               {
                  cout << " integral = " << halphadrE->Integral() << endl;
               }

               if (halphadrE->Integral() > 0)
               {
                  //halphadrE->Scale(1 / (halphadrE->Integral()));
                  hEnergy->Add(halphadrE);
               }

               //hDrE->Scale(1 / (hDrE->Integral()));
               hDrEnergy->Add(hDrE);

               if (halphadrE)
               {
                  delete halphadrE;
               }

               if (hDrE) {

                  delete hDrE;
               }
            }

            if (isNewEvent and ihit != hitsAlphaDrTree->GetEntries() - 1)
            {
               halphadrE = new TH2F(Form("halphadrE_%i", eventNumber), Form("halphadrE_%i", eventNumber), nAlphaBins, xAlphaBins, nRBoundaries - 1, yRBins);
               halphadrE->Sumw2();

               hDrE = new TH1F(Form("hDrE_%i", eventNumber), Form("hDrE_%i", eventNumber), nRBoundaries - 1, yRBins);
               hDrE->Sumw2();
            }
         }

         cout << " *** Total No of events = " << nevent << endl;
         cout << " *Checking if there are empty bins with tolerance = " << m_tolerance << endl;
         isEmptyBin = CheckEmptyBin();

         cout << " isEmptyBin = " << isEmptyBin << endl;

         if (!isEmptyBin)
         {
            break;
         }

         if (hHits)
         {
            delete hHits;
         }
         if (hEnergyDensity)
         {
            delete hEnergyDensity;
         }
         if (hEnergy)
         {
            delete hEnergy;
         }
         if (hLnEnergy)
         {
            delete hLnEnergy;
         }
         if (hLnEnergyDensity)
         {
            delete hLnEnergyDensity;
         }
         if (hGradient)
         {
            delete hGradient;
         }
         if (hDrEnergy) {
            delete hDrEnergy;
         }

         cout << " end loop to combine bins" << endl;
      }// end loop to combine up to 5 bins

      incr++;
   } // end of while loop

   // * set the nbsinR

   set_nbinsR(final_nbinsR);


   // * Define validation histogram

   hValidation = new TH2F("hValidation", "hValidation", 10, -0.5, 9.5, 14, 0.5, 14.5);

   // * check granularity requirement
   bool isGranularityOK = CheckGranularity(globalTree);

   // * check gradient requirement

   bool isGradientOK = CheckGradient();

   if (isGranularityOK and isGradientOK)
   {
      hValidation->Fill(m_PCAbin, m_calolayer, 1);
   }
   else if (isGranularityOK and !isGradientOK)
   {
      hValidation->Fill(m_PCAbin, m_calolayer, 2);
   }
   else if (!isGranularityOK and isGradientOK)
   {
      hValidation->Fill(m_PCAbin, m_calolayer, 3);
   }
   else if (!isGranularityOK and !isGradientOK)
   {
      hValidation->Fill(m_PCAbin, m_calolayer, 4);
   }
}


void ShowerShapeBinning::CreateNNinput()
{
   // * Create NNinput TTrees
   std::string NNinput      = m_topDir + m_particle + "/NNinput_"/*nbinsR" + std::to_string(m_nbinsR) + "_"*/ + m_fileName + ".root";
   TFile       *NNinputFile = new TFile(NNinput.c_str(), "recreate");

   TTree *NNbinCenter = new TTree("NNbinCenter", "NNbinCenter");
   TTree *NNprob      = new TTree("NNprob", "NNprob");

   BookNNinputTree(NNbinCenter, NNprob);

   // * Normalize the energy weighted histogram
   TH2F *hEnergyNorm = (TH2F *)hEnergy->Clone("hEnergyNorm");
   hEnergyNorm->Scale(1 / hEnergyNorm->Integral());

   for (int ibinx = 0; ibinx < hEnergy->GetNbinsX() + 1; ibinx++)
   {
      float Alphamin = hEnergy->GetXaxis()->GetBinLowEdge(ibinx);
      float Alphamax = hEnergy->GetXaxis()->GetBinUpEdge(ibinx);

      for (int ibiny = 0; ibiny < hEnergy->GetNbinsY() + 1; ibiny++)
      {
         float Rmin = hEnergy->GetXaxis()->GetBinLowEdge(ibiny);
         float Rmax = hEnergy->GetYaxis()->GetBinUpEdge(ibiny);

         float area = TMath::Pi() * (Rmax * Rmax - Rmin * Rmin) * ((Alphamax - Alphamin) / (2 * TMath::Pi()));
         // * for energy tree
         if (ibinx > 0 and ibiny > 0)
         {
            m_alphaBinCenter  = hEnergy->GetXaxis()->GetBinCenter(ibinx);
            m_rBinCenter      = hEnergy->GetYaxis()->GetBinCenter(ibiny);
            m_Hits            = hHits->GetBinContent(ibinx, ibiny);
            m_Energy          = hEnergy->GetBinContent(ibinx, ibiny);
            m_EnergyDensity   = hEnergyDensity->GetBinContent(ibinx, ibiny);
            m_LnEnergy        = TMath::Log(hEnergy->GetBinContent(ibinx, ibiny));
            m_LnEnergyDensity = TMath::Log(hEnergyDensity->GetBinContent(ibinx, ibiny));

            if (m_debug)
            {
               cout << " i = " << ibinx << " j = " << ibiny << endl;
               cout << " alpha = " << m_alphaBinCenter << endl;
               cout << " r = " << m_rBinCenter << endl;
               cout << " LN energy density = " << m_LnEnergyDensity << endl;
               cout << "LN energy = " << m_LnEnergy << endl;
               cout << "====================================" << endl;
            }

            NNbinCenter->Fill();
         }


         // * for probability tree: uses bin boundary instead of center

         m_alphaBin = xbinsAlpha.at(ibinx);
         if (ibiny == 0)
         {
            m_LnrBin = TMath::Log(ybinsR.at(1)) - 1;
         }
         else
         {
            m_LnrBin = TMath::Log(ybinsR.at(ibiny));
         }

         // * calculate r prob
         if (ibiny == 0)
         {
            m_rProb = 0;
         }
         else if (ibiny > 0)
         {
            m_rProb = hEnergyNorm->Integral(1, hEnergyNorm->GetNbinsX(), 1, ibiny);
         }

         // * calculate alpha prob

         float alpha_tot = 0;

         if (ibinx == 0 and ibiny == 0)
         {
            m_alphaProb = 0;
         }
         else if (ibiny == 0 and ibinx > 0)
         {
            alpha_tot   = hEnergyNorm->Integral(1, hEnergyNorm->GetNbinsX(), 1, 1);
            m_alphaProb = hEnergyNorm->Integral(1, ibinx, 1, 1) / alpha_tot;
         }
         else if (ibinx > 0 and ibiny > 0)
         {
            alpha_tot   = hEnergyNorm->Integral(1, hEnergyNorm->GetNbinsX(), ibiny, ibiny);
            m_alphaProb = hEnergyNorm->Integral(1, ibinx, ibiny, ibiny) / alpha_tot;
         }

         // * weight: don't understand this? needs to be reviewed!

         if (ibiny == 0)
         {
            m_weight = 1. / 15;
         }
         else if (ibiny == hEnergyNorm->GetNbinsY())
         {
            m_weight = 1.;
         }
         else
         {
            m_weight = area;
         }

         NNprob->Fill();
      }
   }

   // * write the TTrees and relevant histograms

   cout << " ****** Nevents = " << nevent << endl;

   NNinputFile->cd();
   NNbinCenter->Write();
   NNprob->Write();
   hHits->Write();
   hEnergy->Scale(1.0 / nevent);
   hEnergy->Write();

   hEnergyNorm->Write();
   hEnergyDensity->Write();
   hLnEnergy->Write();
   hLnEnergyDensity->Write();
   hGradient->Write();
   hValidation->Write();
   h_alpha->Write();
   h_dr->Write();
   hDrEnergy->Scale(1.0 / nevent);
   hDrEnergy->Write();
   NNinputFile->Close();

   //  std::string inputDistrFile = m_topDir + m_particle + "/InputDistribution_" + m_particle + ".root";
   //  TFile       *fout          = TFile::Open(inputDistrFile.c_str(), "update");
   //  TH2F        *hClone        = (TH2F *)hLnEnergyDensity->Clone(Form("hLnEnergyDensity_layer%i_pca%i", m_calolayer, m_PCAbin));
   //  fout->cd();
   //  hClone->Write();
   //  fout->Close();
}


bool ShowerShapeBinning::CheckEmptyBin()
{
   bool isEmpty = false;

   for (int ibinx = 0; ibinx < hEnergy->GetNbinsX() + 1; ibinx++)
   {
      float Alphamin = hEnergy->GetXaxis()->GetBinLowEdge(ibinx);
      float Alphamax = hEnergy->GetXaxis()->GetBinUpEdge(ibinx);

      for (int ibiny = 0; ibiny < hEnergy->GetNbinsY() + 1; ibiny++)
      {
         float Rmin = hEnergy->GetYaxis()->GetBinLowEdge(ibiny);
         float Rmax = hEnergy->GetYaxis()->GetBinUpEdge(ibiny);

         float area = TMath::Pi() * (Rmax * Rmax - Rmin * Rmin) * (Alphamax - Alphamin) / (2 * TMath::Pi());

         if (ibinx > 0 and ibiny > 0)
         {
            float energy = hEnergy->GetBinContent(ibinx, ibiny);
            hLnEnergy->SetBinContent(ibinx, ibiny, TMath::Log(energy));
            hEnergyDensity->SetBinContent(ibinx, ibiny, energy / area);
            float density = hEnergyDensity->GetBinContent(ibinx, ibiny);
            hLnEnergyDensity->SetBinContent(ibinx, ibiny, TMath::Log(density));
            if (m_debug)
            {
               cout << " BIN (" << ibinx << "," << ibiny << ") = " << density << endl;
            }
            if (density < m_tolerance)
            {
               cout << " EMPTY BIN (" << ibinx << "," << ibiny << ") = " << density << endl;
               isEmpty = true;
               return isEmpty;
            }
         }
      } // end loop over ybins
   }    //end loop over xbins

   return isEmpty;
}


bool ShowerShapeBinning::CheckGranularity(TTree *tree)
{
   bool isGoodGranularity = false;

   // * get the r granularity for the first 3 bins

   float rgranularity = ybinsR.at(3) - ybinsR.at(0);



   // * read the variables needed from the TTree
   tree->GetEntry(0);

   float cell_r    = m_cellr;
   float cell_z    = m_cellz;
   float cell_eta  = m_celleta;
   float cell_deta = m_cellDeta;
   float cell_dphi = m_cellDphi;

   if (m_debug)
   {
      cout << "cell r = " << cell_r << ", cell z = " << cell_z << ", cell eta = " << cell_eta << ", cell deta = " << cell_deta << ", cell dpi = " << cell_dphi << endl;
   }


   // * get the cell dimensions in mm
   float cell_deta_mm, cell_dphi_mm;

   tie(cell_deta_mm, cell_dphi_mm) = GetUnitsmm(cell_eta, cell_deta, cell_dphi, cell_r, cell_z);

   float minCellGranularity = TMath::Min(cell_deta_mm, cell_dphi_mm);

   // * calculate the max and min alpha bin width, already in mm

   std::vector<float> alphaBinWidth;

   for (unsigned int i = 0; i < xbinsAlpha.size() - 1; i++)
   {
      alphaBinWidth.push_back(xbinsAlpha.at(i + 1) - xbinsAlpha.at(i));
   }

   float alphaMax = *std::max_element(alphaBinWidth.begin(), alphaBinWidth.end());
   float alphaMin = *std::min_element(alphaBinWidth.begin(), alphaBinWidth.end());

   // * calculate eta/phi max & min from alpha max & min and r

   float etaMax = TMath::Sqrt((rgranularity * rgranularity) / (1 + TMath::Tan(alphaMax) * TMath::Tan(alphaMax)));
   float etaMin = TMath::Sqrt((rgranularity * rgranularity) / (1 + TMath::Tan(alphaMax) * TMath::Tan(alphaMin)));
   float phiMax = fabs(etaMax * TMath::Tan(alphaMax));
   float phiMin = fabs(etaMin * TMath::Tan(alphaMin));

   if (m_debug)
   {
      cout << " alphaMin = " << alphaMin << " alphaMax = " << alphaMax << " etaMin = " << etaMin << " etaMax = " << etaMax << " phiMin = " << phiMin << " phiMax = " << phiMax << endl;
   }

   // * check if the granularity of the parametrization is smaller than cell granularity

   float minParamGranularity = 0;

   if (minCellGranularity == cell_deta_mm)
   {
      minParamGranularity = TMath::Max(etaMin, etaMax);
   }
   else if (minCellGranularity == cell_dphi_mm)
   {
      minParamGranularity = TMath::Max(phiMin, phiMax);
   }

   if (minCellGranularity >= minParamGranularity)
   {
      isGoodGranularity = true;
      cout << " * Granularity is OK! " << endl;
   }
   else
   {
      cout << " * Granularity Problem " << endl;
      cout << " Cell min granularity  = " << minCellGranularity << endl;
      cout << " Parametrization min granularity = " << minParamGranularity << endl;
   }


   return isGoodGranularity;
}


bool ShowerShapeBinning::CheckGradient()
{
   cout << " * Checking gradient of energy density ..." << endl;

   bool isGradientOK = true;

   for (int ibiny = 2; ibiny < hEnergyDensity->GetNbinsY(); ibiny++)
   {
      float relvar1     = 0;
      float relvar2     = 0;
      float errGradient = 0;

      float Lnrelvar1     = 0;
      float Lnrelvar2     = 0;
      float LnerrGradient = 0;


      for (int ibinx = 1; ibinx < hEnergyDensity->GetNbinsX() + 1; ibinx++)
      {
         relvar1 += (hEnergyDensity->GetBinContent(ibinx, ibiny) - hEnergyDensity->GetBinContent(ibinx, ibiny - 1)) / (hEnergyDensity->GetBinContent(ibinx, ibiny));

         relvar2 += (hEnergyDensity->GetBinContent(ibinx, ibiny + 1) - hEnergyDensity->GetBinContent(ibinx, ibiny)) / (hEnergyDensity->GetBinContent(ibinx, ibiny));

         errGradient += (hEnergyDensity->GetBinError(ibinx, ibiny) - hEnergyDensity->GetBinError(ibinx, ibiny - 1)) / (hEnergyDensity->GetBinError(ibinx, ibiny - 1));


         Lnrelvar1     += (TMath::Log(hEnergyDensity->GetBinContent(ibinx, ibiny)) - TMath::Log(hEnergyDensity->GetBinContent(ibinx, ibiny - 1)));
         Lnrelvar2     += (TMath::Log(hEnergyDensity->GetBinContent(ibinx, ibiny + 1)) - TMath::Log(hEnergyDensity->GetBinContent(ibinx, ibiny)));
         LnerrGradient += (TMath::Log(hEnergyDensity->GetBinError(ibinx, ibiny)) - TMath::Log(hEnergyDensity->GetBinError(ibinx, ibiny - 1)));
      }
      relvar1 = relvar1 / hEnergyDensity->GetNbinsX();
      relvar2 = relvar2 / hEnergyDensity->GetNbinsX();

      Lnrelvar1 = Lnrelvar1 / hEnergyDensity->GetNbinsX();
      Lnrelvar2 = Lnrelvar2 / hEnergyDensity->GetNbinsX();


      hGradient->Fill(hEnergyDensity->GetYaxis()->GetBinCenter(ibiny), Lnrelvar1);
      hGradient->SetBinError(ibiny, LnerrGradient);

      float grad_tolerance = 0.;

      if (ibiny < int(hEnergyDensity->GetNbinsY() / 3.))
      {
         grad_tolerance = 0.1;
      }
      else if (ibiny >= int(hEnergyDensity->GetNbinsY() / 3.) and ibiny < 2 * (int(hEnergyDensity->GetNbinsY() / 3.)))
      {
         grad_tolerance = 0.3;
      }
      else if (ibiny >= 2 * (int(hEnergyDensity->GetNbinsY() / 3.)))
      {
         grad_tolerance = 0.8;
      }

      if (fabs(Lnrelvar1) > grad_tolerance and fabs(Lnrelvar2) > grad_tolerance)
      {
         isGradientOK = false;

         cout << " * Gradient Problem ....." << endl;
         cout << " (ibiny, Ln(DelRho(E)_j), Ln(DelRho(E)_j+1) ) = (" << ibiny << ", " << Lnrelvar1 << ", " << Lnrelvar2 << " )" << endl;
         cout << " (ibiny, DelRho(E)_j, DelRho(E)_j+1 ) = (" << ibiny << ", " << relvar1 << ", " << relvar2 << " )" << endl;
      }
   }

   return isGradientOK;
}


std::tuple<int, double> ShowerShapeBinning::GetBinUpEdge(TH1F* histo) {

   int nbins = histo->GetNbinsX();

   double energy = histo->Integral();

   double cutoff = 0.95 * energy;

   double tot_energy = 0.;

   double bin_edge = 0.;

   int bin_no = -1;

   for (int i = 0; i < nbins; i++) {

      tot_energy += histo->GetBinContent(i);

      if (tot_energy >= cutoff) {

         bin_edge = histo->GetXaxis()->GetBinUpEdge(i);
         bin_no = i;
         break;
      }

   }

   return std::make_tuple(bin_no, bin_edge);
}

std::vector<float> ShowerShapeBinning::Getxbins(TH1F *histo, int nbins)
{
   // * calculate variable bin width in alpha and dr making sure each bin has almost equal amount of hits.

   bool isAlpha = true;

   std::vector<float> xbins;


   std::string title = histo->GetTitle();

   if (title.compare("h_dr") == 0)
   {
      isAlpha = false;
   }

   if (m_debug)
   {
      cout << "title = " << title.c_str() << " isAlpha = " << isAlpha << endl;
   }

   if (isAlpha)
   {
      xbins.push_back(TMath::Pi() / 8);

      for (int i = 1; i <= nbins; i++ ) {

         xbins.push_back(TMath::Pi() / 8 + i * 2 * TMath::Pi() / nbins);
      }

      return xbins;
   }
   else
   {
      xbins.push_back(0);
   }



   float AvgHitsPerBin = histo->Integral() / nbins;


   float hitCounts = 0;

   for (int ibin = 1; ibin < histo->GetNbinsX() + 1; ibin++)
   {
      if (hitCounts < AvgHitsPerBin)
      {
         hitCounts = hitCounts + histo->GetBinContent(ibin);
      }
      else if (hitCounts >= AvgHitsPerBin)
      {
         xbins.push_back(histo->GetBinLowEdge(ibin) + histo->GetBinWidth(ibin));
         hitCounts = 0;
      }
   }

   int   nRmax = histo->FindLastBinAbove(0);
   float Rmax  = histo->GetBinLowEdge(nRmax) + histo->GetBinWidth(nRmax);

   if (isAlpha)
   {
      xbins.push_back(2 * TMath::Pi() + TMath::Pi() / 8);
   }
   else
   {
      xbins.push_back(Rmax);
   }

   return xbins;
}


std::tuple<float, float> ShowerShapeBinning::GetUnitsmm(float eta_hit, float d_eta, float d_phi, float cell_r, float cell_z)
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


void ShowerShapeBinning::InitHitsTree(TTree *hitsNtuple, TTree *globalNtuple)
{
   m_eventNumber = -1;
   m_newEvent    = false;
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
   hitsNtuple->SetBranchAddress("newEvent", &m_newEvent);
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


void ShowerShapeBinning::BookHitsAlphaDrTree(TTree *hitsNtuple)
{
   m_eventNumber = -1;
   m_alpha       = 0.;
   m_dr          = 0.;


   hitsNtuple->Branch("eventNumber", &m_eventNumber);
   hitsNtuple->Branch("energy", &m_energy);
   hitsNtuple->Branch("alpha", &m_alpha);
   hitsNtuple->Branch("dr", &m_dr);
}


void ShowerShapeBinning::InitHitsAlphaDrTree(TTree *hitsNtuple, TTree *globalNtuple)
{
   m_eventNumber = -1;
   m_energy      = 0;
   m_alpha       = 0.;
   m_dr          = 0.;


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
   hitsNtuple->SetBranchAddress("energy", &m_energy);
   hitsNtuple->SetBranchAddress("alpha", &m_alpha);
   hitsNtuple->SetBranchAddress("dr", &m_dr);

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


void ShowerShapeBinning::BookNNinputTree(TTree *energyNtuple, TTree *probNtuple)
{
   m_alphaBinCenter  = 0.;
   m_rBinCenter      = 0.;
   m_Hits            = 0.;
   m_Energy          = 0.;
   m_EnergyDensity   = 0.;
   m_LnEnergy        = 0.;
   m_LnEnergyDensity = 0.;


   m_alphaProb = 0.;
   m_rProb     = 0.;
   m_alphaBin  = 0.;
   m_rBin      = 0.;
   m_LnrBin    = 0;
   m_weight    = 0.;

   energyNtuple->Branch("alpha", &m_alphaBinCenter);
   energyNtuple->Branch("r", &m_rBinCenter);
   energyNtuple->Branch("Hits", &m_Hits);
   energyNtuple->Branch("Energy", &m_Energy);
   energyNtuple->Branch("EnergyDensity", &m_EnergyDensity);
   energyNtuple->Branch("LnEnergy", &m_LnEnergy);
   energyNtuple->Branch("LnEnergyDensity", &m_LnEnergyDensity);

   probNtuple->Branch("alpha_prob", &m_alphaProb);
   probNtuple->Branch("r_prob", &m_rProb);
   probNtuple->Branch("alpha_bin", &m_alphaBin);
   probNtuple->Branch("r_bin", &m_rBin);
   probNtuple->Branch("Lnr_bin", &m_LnrBin);
   probNtuple->Branch("weight", &m_weight);
}
