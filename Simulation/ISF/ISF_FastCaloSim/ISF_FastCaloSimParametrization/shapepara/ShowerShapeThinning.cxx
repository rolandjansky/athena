/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************
 * Class to implement:
 *      - looping over events
 *      - create flat ntuple with hits
 *      - create another ntuple with alpha and r
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

#include "ShowerShapeThinning.h"
#include "ISF_FastCaloSimParametrization/CaloGeometryFromFile.h"
#include "ISF_FastCaloSimParametrization/CaloGeoDetDescrElement.h"

using namespace std;


ShowerShapeThinning::ShowerShapeThinning()
{
   m_debug = 1;

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

   m_chain = 0;
   m_tree  = 0;


   cellDeta = 0;
   cellDphi = 0;
   cellr    = 0;
   cellz    = 0;
   celleta  = 0;
}


ShowerShapeThinning::ShowerShapeThinning(TChain *chain, TTree *tree)
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

   m_chain = chain;
   m_tree  = tree;

   cellDeta = 0;
   cellDphi = 0;
   cellr    = 0;
   cellz    = 0;
   celleta  = 0;
}


ShowerShapeThinning::~ShowerShapeThinning()
{
   if (m_cellVector)
   {
      delete m_cellVector;
   }
   if (m_truthCollection)
   {
      delete m_truthCollection;
   }
   if (m_truthPx)
   {
      delete m_truthPx;
   }
   if (m_truthPy)
   {
      delete m_truthPy;
   }
   if (m_truthPz)
   {
      delete m_truthPz;
   }
   if (m_truthE)
   {
      delete m_truthE;
   }
}


void ShowerShapeThinning::CreateHitsNtuple(TFile *file)
{
   // *define TTree for per hits and global variables

   TTree *m_hitsNtuple   = new TTree("hits", "hits");
   TTree *m_globalNtuple = new TTree("global", "global");

   // * book the hits and global TTree
   BookHitsTree(m_hitsNtuple, m_globalNtuple);


   // * get the PCA bins
   auto vbinsPCA = GetPCAbins();

   // * initialize the input TTree
   InitInputTree();


   // * intialize variables that needs to be saved in hits TTree

   float sumDetaEnergy = 0.;
   float sumDphiEnergy = 0.;
   float sumEta        = 0.;
   float sumPhi        = 0.;
   float Etotal        = 0.;
   int   nhits         = 0;

   // * pca file requires special counter since some are run over inclusive eta with a particular eta selection;

   int pcaEvent = -1;

   int nevent = 0;

   // * loop over events


   int previousEvent = -1;

   cout << " No of events = " << m_chain->GetEntries() << endl;

   TH1F *hEventEnergy = new TH1F("hEventEnergy", "hEventEnergy", m_chain->GetEntries(), 0, m_chain->GetEntries());

   TH1F *hEventHit = new TH1F("hEventHit", "hEventHit", m_chain->GetEntries(), 0, m_chain->GetEntries());


   for (int ievent = 0; ievent < m_chain->GetEntries(); ievent++)
   {
      if (ievent % 200 == 0)
      {
         cout << " Event: " << ievent << endl;
      }

      m_chain->GetEntry(ievent);

      TLorentzVector TruthParticle;
      TruthParticle.SetPxPyPzE(m_truthPx->at(0), m_truthPy->at(0), m_truthPz->at(0), m_truthE->at(0));



      if (fabs(TruthParticle.Eta()) < m_etamin or fabs(TruthParticle.Eta()) > m_etamax)
      {
         continue;
      }

      pcaEvent++;

      if (vbinsPCA.at(pcaEvent) != m_PCAbin)
      {
         continue;
      }

      nevent++;

      // * loop over the cells and hits in each cell

      if (m_debug) cout << " event number = " << ievent << endl;
      // cout << " no of cells = " << (*m_cellVector).size() << endl;

      int hitCount = 0;
      float energyEvent = 0.;


      for (unsigned int icell = 0; icell < (*m_cellVector).size(); icell++)
      {

         for (unsigned int ihit = 0; ihit < ((FCS_matchedcell)(*m_cellVector)[icell]).hit.size(); ihit++)
         {
            hitCount++;

            float energy = ((FCS_matchedcell)(*m_cellVector)[icell]).hit[ihit].hit_energy;
            energyEvent += energy;
            float posx   = ((FCS_matchedcell)(*m_cellVector)[icell]).hit[ihit].hit_x;
            float posy   = ((FCS_matchedcell)(*m_cellVector)[icell]).hit[ihit].hit_y;
            float posz   = ((FCS_matchedcell)(*m_cellVector)[icell]).hit[ihit].hit_z;

            if (m_debug) std::cout << " event = " << ievent << " cell = " << icell << " hit = " << ihit << " energy = " << energy << " pos x = " << posx << " pos y = " << posy << " pos z = " << posz << std::endl;

            TVector3 *pos = new TVector3(posx, posy, posz);

            // * hit eta, phi
            float etaHit = pos->PseudoRapidity();
            float phiHit = pos->Phi();

            if (m_debug) std::cout << " event = " << ievent << " cell = " << icell << " hit = " << ihit << " energy = " << energy << " eta hit = " << etaHit << " phi hit = " << phiHit << std::endl;

            // * entrance eta
            float etaEntrance = (*m_truthCollection)[0].newTTC_entrance_eta[0];
            float phiEntrance = (*m_truthCollection)[0].newTTC_entrance_phi[0];

            if (m_debug) std::cout << " event No = " << ievent << " eta entrance = " << etaEntrance << " phi entrance = " << phiEntrance << std::endl;

            float deta = etaHit - etaEntrance;


            // * calculate dphi
            float y_entrance = ((*m_truthCollection)[0].newTTC_entrance_r[0]) * TMath::Sin((*m_truthCollection)[0].newTTC_entrance_phi[0]);
            float x_entrance = ((*m_truthCollection)[0].newTTC_entrance_r[0]) * TMath::Cos((*m_truthCollection)[0].newTTC_entrance_phi[0]);

            TVector2 *entrance = new TVector2(x_entrance, y_entrance);
            TVector2 *hit      = new TVector2(pos->X(), pos->Y());

            float dphi = hit->DeltaPhi(*entrance);


            // * to calculate eta and phi shift

            sumDetaEnergy += deta * energy;
            sumDphiEnergy += dphi * energy;
            Etotal        += energy;

            // * to calculate eta center and phi center

            sumEta += etaHit;
            sumPhi += phiHit;
            nhits++;

            bool newEvent = false;
            if ((ievent == 0 and icell == 0 and ihit == 0 ) or ievent != previousEvent)
            {
               //cout << " --> Start of a new event " << endl;
               newEvent = true;

            }

            // * fill the hits TTree
            m_eventNumber = ievent; // save the event number;
            m_newEvent    = newEvent;
            m_energy      = energy;
            m_etaHit      = etaHit;
            m_phiHit      = phiHit;
            m_etaEntrance = etaEntrance;
            m_phiEntrance = phiEntrance;
            m_deta        = deta;
            m_dphi        = dphi;
            m_PCA         = vbinsPCA.at(pcaEvent);
            m_hitsNtuple->Fill();
            previousEvent = ievent;
         } // end loop over cells
      }    // end loop over hits

      if (m_debug)
      {
         cout << " ==> hitCount = " << hitCount << endl;
         cout << " ==> event energy = " << energyEvent << endl;
         cout << " ==> total_hit_energy = " << m_total_hit_energy << endl;
         cout << " ==> total_g4hit_energy = " << m_total_g4hit_energy << endl;
      }

      hEventEnergy->SetBinContent(ievent, energyEvent);
      hEventHit->SetBinContent(ievent, hitCount);

   }       // end loop over events

   if (m_debug)
   {
      cout << " sum eta = " << sumEta << endl;
      cout << " sum phi = " << sumPhi << endl;

      cout << " sumDetaEnergy = " << sumDetaEnergy << endl;
      cout << " sumDphiEnergy = " << sumDphiEnergy << endl;
      cout << " Etotal = " << Etotal << endl;
   }

   float etaCenter = sumEta / nhits;
   float phiCenter = sumPhi / nhits;

   float etaShift = sumDetaEnergy / Etotal;
   float phiShift = sumDphiEnergy / Etotal;

   if (m_debug)
   {
      cout << " etaCenter = " << etaCenter << endl;
      cout << " phiCenter = " << phiCenter << endl;
      cout << " nhits = " << nhits << endl;

      cout << " etaShift = " << etaShift << endl;
      cout << " phiShift = " << phiShift << endl;
   }

   // * get cell geometry

   GetCellGeom(etaCenter, phiCenter);

   if (m_debug)
   {
      cout << " cell r = " << cellr << ", cell z = " << cellz << ", cell eta = " << celleta << ", cell deta= " << cellDeta << ", cell dpi = " << cellDphi << endl;
   }

   cout << " Number of events = " << nevent << endl;

   //* fill global TTree

   m_etaCenter = etaCenter;
   m_phiCenter = phiCenter;
   m_etaShift  = etaShift;
   m_phiShift  = phiShift;
   m_cellr     = cellr;
   m_cellz     = cellz;
   m_celleta   = celleta;
   m_cellDeta  = cellDeta;
   m_cellDphi  = cellDphi;
   m_globalNtuple->Fill();


   // * write the TTree

   file->cd();
   m_hitsNtuple->Write();
   m_globalNtuple->Write();
   hEventEnergy->Write();
   hEventHit->Write();
   file->Close();
}


void ShowerShapeThinning::CreateHitsAlphaDrNtuple(std::string inputHits)
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

   float cell_r    = m_cellr;
   float cell_z    = m_cellz;
   float cell_eta  = m_celleta;
   float cell_deta = m_cellDeta;
   float cell_dphi = m_cellDphi;


   if (m_debug)
   {
      cout << " eta center = " << etaCenter << ", phi center = " << phiCenter << " , eta shift = " << etaShift << " , phi shift = " << phiShift << ", cell r = " << cell_r << ", cell z = " << cell_z << ", cell eta = " << cell_eta << ", cell deta = " << cell_deta << ", cell dpi = " << cell_dphi << endl;
   }

   // * define alpha and dr histograms needed to determine binning

   float alphaMin = TMath::Pi() / 8;    // * to cover the crack region in a single bin
   float alphaMax = 2 * TMath::Pi() + alphaMin;
   float rMax     = 10000;

   // * use fine binning for the initial histograms
   TH1F *h_alpha = new TH1F("h_alpha", "h_alpha", 630000, alphaMin, alphaMax);
   TH1F *h_dr    = new TH1F("h_dr", "h_dr", 75000000, 0, rMax);

   TH1F *h_alphaE = new TH1F("h_alphaE", "h_alphaE", 630000, alphaMin, alphaMax);

   TH1F *hdeta_corr = new TH1F("hdeta_corr", "hdeta_corr", 100, -0.2, 0.2);
   TH1F *hdphi_corr = new TH1F("hdphi_corr", "hdphi_corr", 100, -0.2, 0.2);

   // plot energy heatmap before binning optimization
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

   cout << " Looping over nhits = " << hitsTree->GetEntries() << endl;

   for (int ihit = 0; ihit < hitsTree->GetEntries(); ihit++)
   {
      hitsTree->GetEntry(ihit);

      int eventNumber = m_eventNumber;

      float energy_hit = m_energy;
      float eta_hit    = m_etaHit;
      float phi_hit    = m_phiHit;

      float eta_entrance = m_etaEntrance;
      float phi_entrance = m_phiEntrance;

      float deta = m_deta;
      float dphi = m_dphi;

      // * calculate the corrected deta, dphi

      float deta_corr = deta - etaShift;
      float dphi_corr = dphi - phiShift;

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

      h_alpha->Fill(alpha);
      h_alphaE->Fill(alpha, energy_hit);

      // * reweight r with hit energy,  if energy > 1 MeV weigher = energy, otherwise weight = 1
      if (energy_hit > 1.)
      {
         h_dr->Fill(delta_r, energy_hit);
      }
      else
      {
         h_dr->Fill(delta_r, 1);
      }


      // * fill the TTree
      m_eventNumber = eventNumber;
      m_alpha       = alpha;
      m_dr          = delta_r;
      m_energy      = energy_hit;
      m_hitsAlphaDrNtuple->Fill();
   }   //end loop over hits



   hitsAlphaDrFile->cd();
   m_hitsAlphaDrNtuple->Write();
   globalTree->CloneTree()->Write();
   h_alpha->Write();
   h_dr->Write();

   h_alphaE->Write();

   hdeta_corr->Write();
   hdphi_corr->Write();

   hDetaDphi->Write();
   hDetaDphiE->Write();
   hDetaDphi_corr->Write();
   hDetaDphiE_corr->Write();

   hitsAlphaDrFile->Close();
   inputHitsFile->Close();
}


std::vector<float> ShowerShapeThinning::Getxbins(TH1F *histo, int nbins)
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


void ShowerShapeThinning::GetCellGeom(float etacentre, float phicentre)
{
   int layer = m_calolayer;

   CaloGeometryFromFile *geo = new CaloGeometryFromFile();

   geo->LoadGeometryFromFile("/afs/cern.ch/atlas/groups/Simulation/FastCaloSim/ATLAS-GEO-20-00-01.root", "ATLAS-GEO-20-00-01");


   const CaloGeoDetDescrElement *cell;

   // const CaloGeoGeometry *cell;
   //



   cell = geo->getDDE(layer, etacentre, phicentre);

   cellr    = cell->r();
   cellz    = cell->z();
   cellDphi = cell->dphi();
   cellDeta = cell->deta();
   celleta  = cell->eta();
}


std::tuple<float, float> ShowerShapeThinning::GetUnitsmm(float eta_hit, float d_eta, float d_phi, float cell_r, float cell_z)
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


vector<int> ShowerShapeThinning::GetPCAbins()
{
   vector<int> binsPCA;

   int bin = 0;
   m_tree->SetBranchAddress("firstPCAbin", &bin);

   for (int ientry = 0; ientry < m_tree->GetEntries(); ientry++)
   {
      m_tree->GetEntry(ientry);
      if (m_debug)
      {
         cout << " bin = " << bin << endl;
      }
      binsPCA.push_back(bin);
   }

   return binsPCA;
}


void ShowerShapeThinning::InitInputTree()
{
   m_cellVector      = nullptr;
   m_truthCollection = nullptr;
   m_truthPx         = nullptr;
   m_truthPy         = nullptr;
   m_truthPz         = nullptr;
   m_truthE          = nullptr;

   m_total_hit_energy = 0.;
   m_total_g4hit_energy = 0.;


   TString layer = Form("Sampling_%i", m_calolayer);
   if (m_debug)
   {
      cout << " layer = " << layer << endl;
   }


   m_chain->SetBranchAddress(layer, &m_cellVector);
   m_chain->SetBranchAddress("TruthCollection", &m_truthCollection);
   m_chain->SetBranchAddress("TruthPx", &m_truthPx);
   m_chain->SetBranchAddress("TruthPy", &m_truthPy);
   m_chain->SetBranchAddress("TruthPz", &m_truthPz);
   m_chain->SetBranchAddress("TruthE", &m_truthE);
   m_chain->SetBranchAddress("total_hit_energy", &m_total_hit_energy);
   m_chain->SetBranchAddress("total_g4hit_energy", &m_total_g4hit_energy);
}


void ShowerShapeThinning::BookHitsTree(TTree *hitsNtuple, TTree *globalNtuple)
{
   m_eventNumber = -1;
   m_newEvent = false;
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

   hitsNtuple->Branch("eventNumber", &m_eventNumber);
   hitsNtuple->Branch("newEvent", &m_newEvent);
   hitsNtuple->Branch("energy_hit", &m_energy);
   hitsNtuple->Branch("eta_hit", &m_etaHit);
   hitsNtuple->Branch("phi_hit", &m_phiHit);
   hitsNtuple->Branch("eta_entrance", &m_etaEntrance);
   hitsNtuple->Branch("phi_entrance", &m_phiEntrance);
   hitsNtuple->Branch("delta_eta", &m_deta);
   hitsNtuple->Branch("delta_phi", &m_dphi);
   hitsNtuple->Branch("delta_eta", &m_deta);
   hitsNtuple->Branch("pca_bin", &m_PCA);

   globalNtuple->Branch("eta_center", &m_etaCenter);
   globalNtuple->Branch("phi_center", &m_phiCenter);
   globalNtuple->Branch("eta_shift", &m_etaShift);
   globalNtuple->Branch("phi_shift", &m_phiShift);
   globalNtuple->Branch("cell_r", &m_cellr);
   globalNtuple->Branch("cell_z", &m_cellz);
   globalNtuple->Branch("cell_eta", &m_celleta);
   globalNtuple->Branch("cell_deta", &m_cellDeta);
   globalNtuple->Branch("cell_dphi", &m_cellDphi);
}


void ShowerShapeThinning::InitHitsTree(TTree *hitsNtuple, TTree *globalNtuple)
{
   m_eventNumber = -1;
   m_newEvent = false;
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


void ShowerShapeThinning::BookHitsAlphaDrTree(TTree *hitsNtuple)
{
   m_eventNumber = -1;
   m_alpha       = 0.;
   m_dr          = 0.;


   hitsNtuple->Branch("eventNumber", &m_eventNumber);
   hitsNtuple->Branch("energy", &m_energy);
   hitsNtuple->Branch("alpha", &m_alpha);
   hitsNtuple->Branch("dr", &m_dr);
}


float ShowerShapeThinning::DeltaPhi(float phi1, float phi2)
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
