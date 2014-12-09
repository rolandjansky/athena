/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/Geometry.h"

#include "TH2.h"
#include "TProfile2D.h"
#include "TMath.h"
#include <vector>

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;

TH2D* Geo::partitionHist(PartitionId part, TString name, TString title)
{
  TH2D* h = new TH2D(name, title, nFEBs(part), -0.5, nFEBs(part) - 0.5, nPartitionChannels(part), -0.5, nPartitionChannels(part) - 0.5);
  setBinLabels(h, part);
  return h;
}


TProfile2D* Geo::partitionProfileHist(PartitionId part, TString name, TString title)
{
  TProfile2D* h = new TProfile2D(name, title, nFEBs(part), -0.5, nFEBs(part) - 0.5, nPartitionChannels(part), -0.5, nPartitionChannels(part) - 0.5);
  setBinLabels(h, part);
  return h;
}


bool Geo::setBinLabels(TH2* hist, PartitionId part)
{
  if (hist->GetXaxis()->GetNbins() != nFEBs(part)) return false;
  for (int i = 0; i < nFeedThroughs(part); i++)
    hist->GetXaxis()->SetBinLabel(1 + i*nSlots(part), feedThroughName(part, i));

  short nTicks = 16;
  if (hist->GetYaxis()->GetNbins() != nPartitionChannels(part)) return false;
  for (int i = 0; i < nTicks; i++)
    hist->GetYaxis()->SetBinLabel(1 + nPartitionChannels(part)/nTicks*i, Form("%d", nPartitionChannels(part)/nTicks*i));
  
  return true;
}


short Geo::nFeedThroughs(PartitionId part)
{
  if (Id::summary(part) == EMB)  return 32;
  if (Id::summary(part) == EMEC) return 25;
  if (Id::summary(part) == HEC)  return 25;
  if (Id::summary(part) == FCAL) return 25;
  return 0;
}


short Geo::nSlots(PartitionId part)
{
  if (Id::summary(part) == EMB)  return 14;
  if (Id::summary(part) == EMEC) return 15;
  if (Id::summary(part) == HEC)  return 15;
  if (Id::summary(part) == FCAL) return 15;
  return 0;
}
 
 
short Geo::nFEBs()
{
  short n = 0;
  n += nFEBs(EMB_A_PARTITION);
  n += nFEBs(EMB_C_PARTITION);
  n += nFEBs(EMEC_A_PARTITION);
  n += nFEBs(EMEC_C_PARTITION);
  n += nFEBs(HEC_A_PARTITION);
  n += nFEBs(HEC_C_PARTITION);
  n += nFEBs(FCAL_A_PARTITION);
  n += nFEBs(FCAL_C_PARTITION);
  return n;
}


short Geo::nPhiRings()
{
  short n = 0;  
  CaloId cal[10] = { EMB_A, EMB_C, EMEC_OUTER_A, EMEC_OUTER_C, EMEC_INNER_A, EMEC_INNER_C, HEC_A, HEC_C, FCAL_A, FCAL_C };  
  for (unsigned int c = 0; c < 10; c++)
    for (unsigned short l = Geo::firstLayer(cal[c]); l < Geo::firstLayer(cal[c]) + Geo::nLayers(cal[c]); l++) 
      for (unsigned short r = 0; r < Geo::nRegions(cal[c], l); r++) {
        n += Geo::nEta(cal[c], l);
      }

  return n;
}


short Geo::nPartitionChannels(PartitionId /*part*/)
{
  return 128;
}
   
   
short Geo::firstLayer(CaloId calo)
{
  if (Id::matchCalo(calo, EMB))  return 0;
  if (Id::matchCalo(calo, EMEC)) return 0;
  if (Id::matchCalo(calo, HEC))  return 0;
  if (Id::matchCalo(calo, FCAL)) return 1;
  return -1;
}


short Geo::nLayers(CaloId calo)
{
  if (Id::matchCalo(calo, EMB))  return 4;
  if (Id::matchCalo(calo, EMEC)) return 4;
  if (Id::matchCalo(calo, HEC))  return 4;
  if (Id::matchCalo(calo, FCAL)) return 3;
  return -1;
}


short Geo::nRegions(CaloId calo, short layer)
{
  if (Id::matchCalo(calo, EMB)) {
    if (layer == 1) return 2;
    if (layer == 2) return 2;
    return 1;
  }
  
  if (Id::matchCalo(calo, EMEC_OUTER)) {
    if (layer == 1) return 6;
    if (layer == 2) return 2;
    return 1;
  }
  
  if (Id::matchCalo(calo, EMEC_INNER)) return 1;
  if (Id::matchCalo(calo, HEC))  return 2;
  if (Id::matchCalo(calo, FCAL)) return 1;

  return 0;
}


short Geo::firstEta(CaloId calo, short layer, short region)
{
  if (Id::matchCalo(calo, EMB) && layer == 1 && region == 0) return 1;
  if (Id::matchCalo(calo, HEC) && layer == 2 && region == 0) return 1;
  if (Id::matchCalo(calo, HEC) && layer == 3 && region == 0) return 1;
  return 0;
}


short Geo::nEta(CaloId calo, short layer)
{
  if (calo == EMB) return 2*nEta(EMB_A, layer);    
  if (calo == EMEC) return 2*nEta(EMEC_A, layer);    
  if (calo == EMEC_A || calo == EMEC_C) return nEta(EMEC_OUTER_A, layer) + nEta(EMEC_INNER_A, layer);  
  if (calo == HEC) return 2*nEta(HEC_A, layer);    
  if (calo == FCAL) return 2*nEta(FCAL_A, layer);    

  short total = 0;
  for (short region = 0; region < nRegions(calo, layer); region++) total += nEta(calo, layer, region);
  return total;
}


short Geo::nEta(CaloId calo, short layer, short region, short iPhi)
{
  if (region >= nRegions(calo, layer)) return 0;
  if (Id::matchCalo(calo, EMB)) {
    if (layer == 0) return 61;
    if (layer == 1) {
      if (region == 0) return 447; // should be 448, but first strip in each 1/2 barrel is missing
      if (region == 1) return 3;
    }
    if (layer == 2) {
      if (region == 0) return 56;
      if (region == 1) return 1;
    }
    if (layer == 3) return 27;
    return 0;
  }
  if (Id::matchCalo(calo, EMEC_OUTER)) {
    if (layer == 0) return 12;
    if (layer == 1) {
      if (region == 0) return 1;
      if (region == 1) return 3;
      if (region == 2) return 96;
      if (region == 3) return 48;
      if (region == 4) return 64;
      if (region == 5) return 4;
    }
    if (layer == 2) {
      if (region == 0) return 1;
      if (region == 1) return 43;
    }
    if (layer == 3) return 20;
    return 0;
  }
  if (Id::matchCalo(calo, EMEC_INNER)) {
    if (layer == 0) return 0;
    if (layer == 1) return 7;
    if (layer == 2) return 7;
    if (layer == 3) return 0;
    return 0;
  }
  if (Id::matchCalo(calo, HEC)) {
    if (layer >= 4) return 0;
    if (region == 0) { short n[4] = { 10, 10, 9, 8 }; return n[layer]; }
    if (region == 1) { short n[4] = {  4,  3, 3, 4 }; return n[layer]; }
    return 0;
  }
  if (Id::matchCalo(calo, FCAL)) {
    if (layer > 3) return 0;
    if (layer == 1) return 63;
    if (layer == 2) {
      short n[16] = { 30, 32, 32, 31, 31, 32, 32, 30, 30, 32, 32, 31, 31, 32, 32, 30 };
      return n[iPhi];
    }
    if (layer == 3) return (iPhi == 5 || iPhi == 13 ? 15 : 16);
  }
  return 0;
}


short Geo::nPhi(CaloId calo, short layer, short region)
{
  if (Id::matchCalo(calo, EMB)) {
    if (layer > 3) return 0;
    if (region == 1) return (layer == 1 || layer == 2 ? 256 : 0); // barrel-end region
    short n[4] = { 64, 64, 256, 256 };
    return n[layer];
  }
  if (Id::matchCalo(calo, EMEC_OUTER)) {
    if (layer > 3) return 0;
    short n[4] = { 64, 64, 256, 256 };     
    return n[layer];
  }
  if (Id::matchCalo(calo, EMEC_INNER)) {
    if (layer > 2 || layer == 0) return 0;
    short n[3] = { 0, 64, 64 };     
    return n[layer];
  }
  if (Id::matchCalo(calo, EMEC)) return nPhi(EMEC_OUTER, layer);
  if (Id::matchCalo(calo, PRECISION_EM)) return nPhi(EMB, layer);
  if (Id::matchCalo(calo, EM)) return nPhi(EMB, layer);
  if (Id::matchCalo(calo, HEC)) {
    if (layer > 3) return 0;
    if (region == 0) return 64;
    if (region == 1) return 32;
    return 0;
  }
  if (Id::matchCalo(calo, FCAL)) {
    if (layer > 3) return 0;
    short n[4] = { 0, 16, 16, 16 };     
    return n[layer];
  }
  return 0;
}


double Geo::etaSize(CaloId calo, short layer, short region, short iEta)
{
  if (Id::matchCalo(calo, EMB)) {
    if (layer > 3) return 0;
    double w[4] = { 0.025, 0.0249724/8, 0.025, 0.050 };
    double nominal = w[layer];
    if (iEta == -1) return nominal;
    // iEta == 0 strip is not there! (well, strip is there, but not connected...)
/*    if (layer == 1 && iEta >= 1 && iEta <= 447) return nominal;
    if (layer == 1 && iEta >= 448)              return 8*nominal;
    if (layer == 2 && iEta >= 0 && iEta <= 56)  return nominal;
    if (layer == 2 && iEta == 57)               return 3*nominal;*/
    if (layer == 0) return nominal;
    if (calo == EMB_A && layer == 1 && region == 0 && iEta == 0) return nominal*1.5;
    if (calo == EMB_C && layer == 1 && region == 0 && iEta == nEta(calo, layer, region)) return nominal*1.5;
    if (layer == 1 && region == 0) return nominal;
    if (layer == 1 && region == 1) return 8*nominal;
    if (layer == 2 && region == 0) return nominal;
    if (layer == 2 && region == 1) return 3*nominal;
    if (layer == 3) return nominal;
    return 0;
  }
  
  if (Id::matchCalo(calo, EMEC_OUTER)) {
    if (layer > 3) return 0;
    double w[4] = { 0.025, 0.025/8, 0.025, 0.050 };
    double nominal = w[layer];
    if (iEta == -1) return nominal;
/*    if (layer == 1 && iEta ==   0)                return 16*nominal;
    if (layer == 1 && iEta >=   1 && iEta <=   3) return 8*nominal;
    if (layer == 1 && iEta >=   4 && iEta <=  99) return nominal;
    if (layer == 1 && iEta >= 100 && iEta <= 147) return 4/3.*nominal;
    if (layer == 1 && iEta >= 148 && iEta <= 211) return 2*nominal;
    if (layer == 1 && iEta >= 212 && iEta <= 215) return 8*nominal;
    if (layer == 2 && iEta ==   1)                return 2*nominal;
    if (layer == 2 && iEta >=   2 && iEta <=  44) return nominal;*/
    if (layer == 0) return nominal;
    if (layer == 1 && region == 0) return 16*nominal;
    if (layer == 1 && region == 1) return 8*nominal;
    if (layer == 1 && region == 2) return nominal;
    if (layer == 1 && region == 3) return 4/3.*nominal;
    if (layer == 1 && region == 4) return 2*nominal;
    if (layer == 1 && region == 5) return 8*nominal;
    if (layer == 2 && region == 0) return 2*nominal;
    if (layer == 2 && region == 1) return nominal;
    if (layer == 3) return nominal;
    return 0;
  }
  
  if (Id::matchCalo(calo, EMEC_INNER)) {
    if (layer > 3) return 0;
    double nominal = 0.1;
    if (iEta == -1) return nominal;
/*  if (layer == 1 && iEta >= 216 && iEta <= 222) return 32*nominal;
    if (layer == 2 && iEta >=  45 && iEta <=  51) return 4*nominal;*/
    if (layer == 1) return nominal;
    if (layer == 2) return nominal;
    if (layer == 3) return nominal;
    return 0;
  }

  if (Id::matchCalo(calo, HEC)) {
    if (layer > 3) return 0;
    if (region == 0) return 0.1;
    if (region == 1) return 0.2;
    return 0;
  }
  
  if (Id::matchCalo(calo, FCAL)) {
    double nominal = 1.8/64;
    if (layer == 1) return nominal;
    if (layer == 2) return 2*nominal;
    if (layer == 3) return 4*nominal;
    return 0;
  }
  
  return 0;
}


double Geo::phiSize(CaloId calo, short layer, short /*region*/, short /*iPhi*/)
{
  if (layer > 3) return 0;
  if (Id::matchCalo(calo, FCAL) && layer == 0) return 0;
  short nphi=nPhi(calo, layer);
  double nominal = 0;
  if (nphi!=0) nominal=TMath::TwoPi()/nphi;//nPhi(calo, layer);
  return nominal;
}


double Geo::etaMin(CaloId calo, short layer, short region)
{
  if (Id::sign(calo) < 0) return -etaMax(Id::mirror(calo), layer, region);
  
  if (region < 0) {
    if (calo == EMB)    return -etaMax(EMB_A, layer);
    if (calo == EMEC_A) return etaMin(EMEC_OUTER_A, layer);
    region = 0;
  }
    
  if (calo == EMB_A) {
    if (layer == 1 && region == 0) return 0;
    if (region == 1) return 1.40; // where applicable, i.e. layers 1,2
    return 0;
  }
  if (calo == EMEC_OUTER_A) {
    if (layer == 0) return 1.5;
    if (layer == 1) {
      if (region == 0) return 1.385; // njpb
      if (region == 1) return 1.435; // njpb
      if (region == 2) return 1.51; // njpb
      if (region == 3) return 1.81; // njpb
      if (region == 4) return 2.01; // njpb
      if (region == 5) return 2.41; // njpb
    }
    if (layer == 2) {
      if (region == 0) return 1.385; // njpb
      if (region == 1) return 1.435; // njpb
    }
    if (layer == 3) return 1.5;
  }
  if (calo == EMEC_INNER_A) 
    if (layer == 1 || layer == 2) return 2.51;
  if (calo == HEC_A) {
    if (region == 0) return 1.5;
    if (region == 1) return 2.5;
  }
  if (calo == FCAL_A) return 3.1; 
  return -999;
}


double Geo::etaMax(CaloId calo, short layer, short region)
{
  if (Id::sign(calo) < 0) return -etaMin(Id::mirror(calo), layer, region);

  if (region < 0) {
    if (calo == EMB)    return etaMax(EMB_A, layer);
    if (calo == EMEC_A) return etaMax(EMEC_INNER_A, layer);
    region = nRegions(calo, layer) - 1;
  }
  
  if (calo == EMB_A) {
    if (layer == 0) return 1.52;
    if (layer == 1 || layer == 2) {
      if (region == 0) return 1.40;
      if (region == 1) return 1.475;
    }
    if (layer == 3) return 1.35;
  }
  if (calo == EMEC_OUTER_A) {
    if (layer == 0) return 1.8;
    if (layer == 1) {
      if (region == 0) return 1.435; // njpb
      if (region == 1) return 1.51; // njpb
      if (region == 2) return 1.81; // njpb
      if (region == 3) return 2.01; // njpb
      if (region == 4) return 2.41; // njpb
      if (region == 5) return 2.51; // njpb
    }
    if (layer == 2) {
      if (region == 0) return 1.435; // njpb
      if (region == 1) return 2.51; // njpb
    }
    if (layer == 3) return 2.5;
  }
  if (calo == EMEC_INNER_A) 
    if (layer == 1 || layer == 2) return 3.21;
  if (calo == HEC_A) {
    if (region == 0) return 2.5;
    if (region == 1) return 3.3;
  }
  if (calo == FCAL_A) return 4.9; 
  return -999;
}


double Geo::eta(CaloId calo, short layer, short region, short iEta)
{
  double pos = etaMin(calo, layer, region);
  for (short i = 0; i < iEta; i++) pos += etaSize(calo, layer, region, i);
  return Id::sign(calo)*pos;
}


double Geo::phiMin(CaloId calo, short layer, short region)
{
  if (Id::matchCalo(calo, EMEC_OUTER) && layer == 2) return phiSize(calo, layer, region)/2;
  return 0;
}


double Geo::phi(CaloId calo, short layer, short region, short iPhi)
{
  double pos = phiMin(calo, layer, region);
  for (short i = 0; i < iPhi; i++) pos += phiSize(calo, layer, region, i);
  return pos;
}


short Geo::etaIndex(CaloId calo, short layer, short region, short iEta)
{
  if (Id::matchCalo(calo, EMB)) {
    if (layer == 1 && region == 1) iEta += nEta(calo, layer, 0);
    if (layer == 2 && region == 1) iEta += nEta(calo, layer, 0);
  }
  if (Id::matchCalo(calo, EMEC_OUTER)) {
    if (layer == 1)
      for (short r = 0; r < region; r++) iEta += nEta(calo, layer, r);
    if (layer == 2 && region >= 1) iEta += 1;
  }
  if (Id::matchCalo(calo, HEC))
    if (region == 1) iEta += 10;
  return iEta;
}


TArrayD Geo::etaBins(CaloId calo, short layer)
{
  if (Id::sign(calo) < 0) return mirror(etaBins(Id::mirror(calo), layer));
  
  if (calo == EMB) {
    TArrayD binsA = etaBins(EMB_A, layer);
    TArrayD binsC = etaBins(EMB_C, layer);
    std::vector<TArrayD*> v; v.push_back(&binsC); v.push_back(&binsA); 
    return merge(v);
  }
  if (calo == EMEC_A) {
    TArrayD binsO = etaBins(EMEC_OUTER_A, layer);
    TArrayD binsI = etaBins(EMEC_INNER_A, layer);
    std::vector<TArrayD*> v; v.push_back(&binsO); v.push_back(&binsI); 
    return merge(v);
  }
  if (calo == PRECISION_EM) {
    TArrayD binsBA = etaBins(EMB_A, layer);
    TArrayD binsOA = etaBins(EMEC_OUTER_A, layer);
    if (layer == 1) {
      binsBA = remove(binsBA, -1);
      binsBA = remove(binsBA, -1);
      binsOA = remove(binsOA, 0);
    } else if (layer == 2) {
      binsBA = remove(binsBA, -1);
      binsOA[0] = 1.40;
    }
    TArrayD binsBC = mirror(binsBA);
    TArrayD binsOC = mirror(binsOA);
    std::vector<TArrayD*> v; 
    v.push_back(&binsOC); v.push_back(&binsBC); 
    v.push_back(&binsBA); v.push_back(&binsOA); 
    return merge(v);
  }
  if (calo == EM) {
    TArrayD binsIA = etaBins(EMEC_INNER_A, layer);
    TArrayD binsIC = etaBins(EMEC_INNER_C, layer);
    TArrayD binsPE = etaBins(PRECISION_EM, layer);
    std::vector<TArrayD*> v; 
    v.push_back(&binsIC); v.push_back(&binsPE); v.push_back(&binsIA); 
    return merge(v);
  }

  if (calo == HEC) {
    TArrayD binsHA = etaBins(HEC_A, layer);
    TArrayD binsHC = etaBins(HEC_C, layer);
    TArrayD trans(2); trans[0] = -1.5; trans[1] = 1.5;
    std::vector<TArrayD*> v; 
    v.push_back(&binsHC); v.push_back(&trans); v.push_back(&binsHA);
    return merge(v);
  }
  
  if (nEta(calo, layer) == 0) return TArrayD();

  TArrayD bins(nEta(calo, layer) + 1);
  short nRegs = nRegions(calo, layer);
  
  int i = 0;
  for (short k = 0; k < nRegs; k++) {
    for (short ii = 0; ii < nEta(calo, layer, k); ii++, i++) 
      bins[i] = eta(calo, layer, k, ii);
  }
  bins[i] = etaHigh(calo, layer, nRegs - 1, nEta(calo, layer, nRegs - 1) - 1);
  return bins;
}


TH2D* Geo::etaPhiHist(CaloId calo, short layer, TString name, TString title)
{
  TArrayD etaBinArray = etaBins(calo, layer);
  short nPhiBins = nPhi(calo, layer);
  double eps = 1E-6; // This is needed for changes in granularity: e.g. in the HEC, at larger eta the phi granularity is twice less => the center of 
                     // the coarse phi bins fall on a histogram boundary, and floating-point uncertainties mean that some bins fall on one side and some on the other...
  TH2D* h = new TH2D(name, title, etaBinArray.GetSize() - 1, etaBinArray.GetArray(), 
                     nPhiBins, -TMath::Pi() + eps + phiMin(calo, layer), TMath::Pi() + eps + phiMin(calo, layer));
  h->GetXaxis()->SetTitle("#eta");
  h->GetYaxis()->SetTitle("#phi");
  return h;
}


double Geo::etaCenter(CaloId calo, short layer, short region, short iEta)
{ 
  return eta(calo, layer, region, iEta) + Id::sign(calo)*etaSize(calo, layer, region, iEta)/2; 
}


double Geo::phiCenter(CaloId calo, short layer, short region, short iPhi)
{ 
  return phi(calo, layer, region, iPhi) + phiSize(calo, layer, region, iPhi)/2; 
}


double Geo::etaHigh(CaloId calo, short layer, short region, short iEta)
{ 
  return eta(calo, layer, region, iEta) + Id::sign(calo)*etaSize(calo, layer, region, iEta); 
}


double Geo::phiHigh(CaloId calo, short layer, short region, short iPhi)
{ 
  return phi(calo, layer, region, iPhi) + phiSize(calo, layer, region, iPhi); 
}


int Geo::nChannels(CaloId calo, short layer, short region)
{
  if (region >= Geo::nRegions(calo, layer)) return 0;
  int n = Geo::nEta(calo, layer, region)*Geo::nPhi(calo, layer, region);
  if (Id::matchCalo(calo, FCAL) && layer == 2) n -= 12; // account for non-phi-symmetric disconnected channels
  if (Id::matchCalo(calo, FCAL) && layer == 3) n -= 2; // account for non-phi-symmetric disconnected channels
  return n;
}


int Geo::nChannels(CaloId calo, short layer)
{
  if (calo == EMB)    return nChannels(EMB_A, layer) + nChannels(EMB_C, layer);
  if (calo == EMEC_A) return nChannels(EMEC_OUTER_A, layer) + nChannels(EMEC_INNER_A, layer);
  if (calo == EMEC_C) return nChannels(EMEC_OUTER_C, layer) + nChannels(EMEC_INNER_C, layer);
  if (calo == EMEC)   return nChannels(EMEC_A, layer) + nChannels(EMEC_C, layer);
  if (calo == HEC)    return nChannels(HEC_A, layer) + nChannels(HEC_C, layer);
  if (calo == FCAL)   return nChannels(FCAL_A, layer) + nChannels(FCAL_C, layer);

  int n = 0;
  for (unsigned short r = 0; r < Geo::nRegions(calo, layer); r++) 
    n += Geo::nChannels(calo, layer, r);
  return n;
}


int Geo::nChannels(CaloId calo)
{
  int n = 0;
  for (unsigned short l = Geo::firstLayer(calo); l < Geo::firstLayer(calo) + Geo::nLayers(calo); l++) 
    n += Geo::nChannels(calo, l);
  return n;
}


TString Geo::feedThroughName(PartitionId part, short feedThrough)
{
// feedthrough experts names (based on baseplane numbers) vector
// From ATL-AP-IN-0003, Numberings for the Barrel Feedthroughs
  
// Barrel A and C feedthrough experts names
// indexed by feedthrough number (which starts at 0)
  std::vector<std::string> ftBarrelA, ftBarrelC;
  ftBarrelA.resize(32);     ftBarrelC.resize(32); 
  ftBarrelA[0]  = " 0-01L";    ftBarrelC[0]  = " 0-09L";
  ftBarrelA[1]  = " 1-02R";    ftBarrelC[1]  = " 1-08R";
  ftBarrelA[2]  = " 2-02L";    ftBarrelC[2]  = " 2-08L";
  ftBarrelA[3]  = " 3-03R";    ftBarrelC[3]  = " 3-07R";
  ftBarrelA[4]  = " 4-03L";    ftBarrelC[4]  = " 4-07L";
  ftBarrelA[5]  = " 5-04R";    ftBarrelC[5]  = " 5-06R";
  ftBarrelA[6]  = " 6-04L";    ftBarrelC[6]  = " 6-06L";
  ftBarrelA[7]  = " 7-05R";    ftBarrelC[7]  = " 7-05R";
  ftBarrelA[8]  = " 8-05L";    ftBarrelC[8]  = " 8-05L";
  ftBarrelA[9]  = " 9-06R";    ftBarrelC[9]  = " 9-04R";
  ftBarrelA[10] = "10-06L";    ftBarrelC[10] = "10-04L";
  ftBarrelA[11] = "11-07R";    ftBarrelC[11] = "11-03R";
  ftBarrelA[12] = "12-07L";    ftBarrelC[12] = "12-03L";
  ftBarrelA[13] = "13-08R";    ftBarrelC[13] = "13-02R";
  ftBarrelA[14] = "14-08L";    ftBarrelC[14] = "14-02L";
  ftBarrelA[15] = "15-09R";    ftBarrelC[15] = "15-01R";
  ftBarrelA[16] = "16-09L";    ftBarrelC[16] = "16-01L";
  ftBarrelA[17] = "17-10R";    ftBarrelC[17] = "17-16R";
  ftBarrelA[18] = "18-10L";    ftBarrelC[18] = "18-16L";
  ftBarrelA[19] = "19-11R";    ftBarrelC[19] = "19-15R";
  ftBarrelA[20] = "20-11L";    ftBarrelC[20] = "20-15L";
  ftBarrelA[21] = "21-12R";    ftBarrelC[21] = "21-14R";
  ftBarrelA[22] = "22-12L";    ftBarrelC[22] = "22-14L";
  ftBarrelA[23] = "23-13R";    ftBarrelC[23] = "23-13R";
  ftBarrelA[24] = "24-13L";    ftBarrelC[24] = "24-13L";
  ftBarrelA[25] = "25-14R";    ftBarrelC[25] = "25-12R";
  ftBarrelA[26] = "26-14L";    ftBarrelC[26] = "26-12L";
  ftBarrelA[27] = "27-15R";    ftBarrelC[27] = "27-11R";
  ftBarrelA[28] = "28-15L";    ftBarrelC[28] = "28-11L";
  ftBarrelA[29] = "29-16R";    ftBarrelC[29] = "29-10R";
  ftBarrelA[30] = "30-16L";    ftBarrelC[30] = "30-10L";
  ftBarrelA[31] = "31-01R";    ftBarrelC[31] = "31-09R";
  
// Endcap A and C feedthrough experts names, indexed by feedthrough number
  std::vector<std::string> ftEndcapA, ftEndcapC;
  ftEndcapA.resize(25);     ftEndcapC.resize(25); 
  ftEndcapA[0]  = " 0-01R";    ftEndcapC[0]  = " 0-07R";
  ftEndcapA[1]  = " 1-01L";    ftEndcapC[1]  = " 1-07L";
  ftEndcapA[2]  = " 2-02R";    ftEndcapC[2]  = " 2-06R";
  ftEndcapA[3]  = " 3-02L";    ftEndcapC[3]  = " 3-06L";
  ftEndcapA[4]  = " 4-03R";    ftEndcapC[4]  = " 4-05R";
  ftEndcapA[5]  = " 5-03L";    ftEndcapC[5]  = " 5-05L";
  ftEndcapA[6]  = " 6-04R";    ftEndcapC[6]  = " 6-04L";  // the FCAL baseplane
  ftEndcapA[7]  = " 7-05R";    ftEndcapC[7]  = " 7-03R";
  ftEndcapA[8]  = " 8-05L";    ftEndcapC[8]  = " 8-03L";
  ftEndcapA[9]  = " 9-06R";    ftEndcapC[9]  = " 9-02R";
  ftEndcapA[10] = "10-06L";    ftEndcapC[10] = "10-02L";
  ftEndcapA[11] = "11-07R";    ftEndcapC[11] = "11-01R";
  ftEndcapA[12] = "12-07L";    ftEndcapC[12] = "12-01L";
  ftEndcapA[13] = "13-08R";    ftEndcapC[13] = "13-13R";
  ftEndcapA[14] = "14-08L";    ftEndcapC[14] = "14-13L";
  ftEndcapA[15] = "15-09R";    ftEndcapC[15] = "15-12R";
  ftEndcapA[16] = "16-09L";    ftEndcapC[16] = "16-12L";
  ftEndcapA[17] = "17-10R";    ftEndcapC[17] = "17-11R";
  ftEndcapA[18] = "18-10L";    ftEndcapC[18] = "18-11L";
  ftEndcapA[19] = "19-11R";    ftEndcapC[19] = "19-10R";
  ftEndcapA[20] = "20-11L";    ftEndcapC[20] = "20-10L";
  ftEndcapA[21] = "21-12R";    ftEndcapC[21] = "21-09R";
  ftEndcapA[22] = "22-12L";    ftEndcapC[22] = "22-09L";
  ftEndcapA[23] = "23-13R";    ftEndcapC[23] = "23-08R";
  ftEndcapA[24] = "24-13L";    ftEndcapC[24] = "24-08L";
  
  if (part == EMB_A_PARTITION) return ftBarrelA[feedThrough];
  if (part == EMB_C_PARTITION) return ftBarrelC[feedThrough];
  if (part == EMEC_A_PARTITION) return ftEndcapA[feedThrough];
  if (part == EMEC_C_PARTITION) return ftEndcapC[feedThrough];
  return "UNKNOWN";
}


TArrayD Geo::merge(const std::vector<TArrayD*>& arrays)
{
  std::vector<TArrayD*> nonEmptyArrays;
  unsigned int n = 0;
  for (unsigned int k = 0; k < arrays.size(); k++) {
    if (arrays[k]->GetSize() == 0) continue;
    n += arrays[k]->GetSize();
    nonEmptyArrays.push_back(arrays[k]);
  }
  
  TArrayD bins(n - nonEmptyArrays.size() + 1); // remove final bins, except the last one 
  int i = 0;
  for (unsigned int k = 0; k < nonEmptyArrays.size(); k++) 
      for (int ii = 0; ii < nonEmptyArrays[k]->GetSize() - 1; ii++, i++) 
	bins[i] = (*nonEmptyArrays[k])[ii];
  bins[i] = (*nonEmptyArrays[nonEmptyArrays.size() - 1])[nonEmptyArrays[nonEmptyArrays.size() - 1]->GetSize() - 1];
  return bins;
}


TArrayD Geo::mirror(const TArrayD& bins)
{
  int n = bins.GetSize();
  TArrayD mirrorBins(n);
  for (int k = 0; k < n; k++)
    mirrorBins[k] = -bins[n - k - 1];
  return mirrorBins;
}

TArrayD Geo::remove(const TArrayD& bins, int index)
{
  int n = bins.GetSize();
  if (index < 0) index = n + index;
  if (index >= n) return bins;
  
  TArrayD newBins(n - 1);
  int k = 0;
  for (int kk = 0; kk < n; kk++) {
    if (kk == index) continue;
    newBins[k] = bins[kk];
    k++;
  }
  return newBins;
}
