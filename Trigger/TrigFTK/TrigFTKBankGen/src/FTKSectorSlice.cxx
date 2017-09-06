/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigFTKBankGen/FTKSectorSlice.h"
#include "TrigFTKSim/FTKSetup.h"
#include <TROOT.h>
#include <TFile.h>
#include <TClonesArray.h>
#include <TBits.h>
#include <TMath.h>
#include <GaudiKernel/MsgStream.h>
#include <iostream>

using namespace std;

FTKSectorSlice::FTKSectorSlice() : 
  bits_phi(0x0),
  bits_c(0x0),
  bits_d(0x0),
  bits_z0(0x0),
  bits_ctheta(0x0),

  m_phi_min(-TMath::Pi()),
  m_phi_max(TMath::Pi()),
  // m_c_min(-7e-4),
  // m_c_max(7e-4),
  m_c_min(-1e-3),
  m_c_max(1e-3),
  m_d0_min(-2.2),
  m_d0_max(2.2),
  m_z0_min(-120),
  m_z0_max(120),
  m_ctheta_min(TMath::SinH(-3)),
  m_ctheta_max(TMath::SinH(3)),
  m_eta_min(-3),
  m_eta_max(3),

  m_autotune_phi_min(999),
  m_autotune_c_min(999),
  m_autotune_d0_min(999),
  m_autotune_z0_min(999),
  m_autotune_ctheta_min(999),
  m_autotune_eta_min(999),
  m_autotune_phi_max(-999),
  m_autotune_c_max(-999),
  m_autotune_d0_max(-999),
  m_autotune_z0_max(-999),
  m_autotune_ctheta_max(-999),
  m_autotune_eta_max(-999),

  m_phi_nbins(0),
  m_c_nbins(0),
  m_d0_nbins(0),
  m_z0_nbins(0),
  m_ctheta_nbins(0),

  m_step_phi(0),
  m_step_c(0),
  m_step_d0(0),
  m_step_z0(0),
  m_step_ctheta(0),

  m_ITkMode(false)
  
{
  // nothing to do
}

void FTKSectorSlice::loadSlices(std::string slice_file_path) {

  // Check if using ITk geometry
  m_ITkMode = FTKSetup::getFTKSetup().getITkMode();

  TFile slice_file(slice_file_path.c_str());
 
  //TClonesArray *bits_phi = (TClonesArray*)slice_file.Get("c_bits_phi");
  bits_phi = (TClonesArray*) (slice_file.Get("c_bits_phi")->Clone("bitsphi"));
  m_phi_nbins = bits_phi->GetEntries();
  m_step_phi = (m_phi_max - m_phi_min)/m_phi_nbins;

  autotune(bits_phi,m_phi_min,m_phi_max,m_autotune_phi_min,m_autotune_phi_max,
           true,"phi");
  //TClonesArray *bits_c = (TClonesArray*) slice_file.Get("c_bits_c");
  bits_c = (TClonesArray*) slice_file.Get("c_bits_c");
  m_c_nbins = bits_c->GetEntries();
  m_step_c = (m_c_max - m_c_min)/m_c_nbins;

  autotune(bits_c,m_c_min,m_c_max,m_autotune_c_min,m_autotune_c_max,false,"c");

  //TClonesArray *bits_d = (TClonesArray*)slice_file.Get("c_bits_d");
  bits_d = (TClonesArray*)slice_file.Get("c_bits_d");
  m_d0_nbins = bits_d->GetEntries();
  m_step_d0 = (m_d0_max - m_d0_min)/m_d0_nbins;

  autotune(bits_d,m_d0_min,m_d0_max,m_autotune_d0_min,m_autotune_d0_max,false,
           "d0");

  //TClonesArray *bits_z0 = (TClonesArray*)slice_file.Get("c_bits_z0");
  bits_z0 = (TClonesArray*)slice_file.Get("c_bits_z0");
  m_z0_nbins = bits_z0->GetEntries();
  m_step_z0 = (m_z0_max - m_z0_min)/m_z0_nbins;

  autotune(bits_z0,m_z0_min,m_z0_max,m_autotune_z0_min,m_autotune_z0_max,false,
           "z0");

  //TClonesArray *bits_ctheta = (TClonesArray*)slice_file.Get("c_bits_ctheta");
  bits_ctheta = (TClonesArray*)slice_file.Get("c_bits_ctheta");
  m_ctheta_nbins = bits_ctheta->GetEntries();
  m_step_ctheta = (m_ctheta_max - m_ctheta_min)/m_ctheta_nbins;

  autotune(bits_ctheta,m_ctheta_min,m_ctheta_max,
           m_autotune_ctheta_min,m_autotune_ctheta_max,false,"cos(theta)");

  
  m_autotune_eta_min=TMath::ASinH(m_autotune_ctheta_min);
  m_autotune_eta_max=TMath::ASinH(m_autotune_ctheta_max);

#if 0
  for (int ibin=0;ibin!=m_phi_nbins;++ibin) {
    TBits *curtbit = (TBits*) bits_phi->At(ibin);
    
  }
#endif
  cout<<"number of sectors in the slide file \""<<slice_file_path
      <<"\" : "<<((TBits const *)bits_ctheta->UncheckedAt(0))->GetNbits()
      <<"\n";
}

void FTKSectorSlice::autotune
(const TClonesArray *array,double_t x0,double_t x1,
 double &autoMin,double &autoMax,bool isAzimuth,const char *debug) {
   int nbin=array->GetEntries();
   int i0,i1;
   for(i0=0;(i0<nbin-1) &&
          !((TBits const *)array->UncheckedAt(i0))->CountBits();i0++) { }
   for(i1=nbin-1;(i1>0) &&
          !((TBits const *)array->UncheckedAt(i1))->CountBits();i1--) { }
   if(isAzimuth) {
      int j;
      vector<char> phi(nbin);
      for(j=0;j<nbin;j++) { 
         if(((TBits const *)array->UncheckedAt(j%nbin))->CountBits()) {
            // add safety margin: +/- 1 bin
            phi[(j+nbin-1)%nbin]++;
            phi[(j+nbin)%nbin]++;
            phi[(j+nbin+1)%nbin]++;
         }
      }
      // locate largest continuous region
      int gap0=-1,gapStart=-1,gapSize=0;
      for(j=0;j<2*nbin;j++) { 
         if(phi[j%nbin]) {
            if(gap0<0) {
               // start gap
               gap0=j;
            }
            int size=j+1-gap0;
            if(size>gapSize) {
               gapStart=gap0;
               gapSize=size;
            }
         } else {
            // end gap
            gap0=-1;
         }
      }
      i0=gapStart;
      i1=gapStart+gapSize-1;
   }
   autoMin=x0+((x1-x0)*i0)/nbin;
   autoMax=x0+((x1-x0)*(i1+1))/nbin;
   if(debug) {
      cout<<"FTKSectorSlice::autotune "<<debug
          <<" nbin="<<nbin<<" range=[ "<<x0<<" ; "<<x1
          <<" ] result=[ "<<autoMin<<" ; "<<autoMax<<" ]\n";
   }
}

double FTKSectorSlice::getAutotunePhiMin() {
  return m_autotune_phi_min;
}

double FTKSectorSlice::getAutotuneCMin() {
  return m_autotune_c_min;
}

double FTKSectorSlice::getAutotuneDMin() {
  return m_autotune_d0_min;
}

double FTKSectorSlice::getAutotuneZ0Min() {
  return m_autotune_z0_min;
}

double FTKSectorSlice::getAutotuneCthetaMin() {
  return m_autotune_ctheta_min;
}

double FTKSectorSlice::getAutotuneEtaMin() {
  return m_autotune_eta_min;
}

double FTKSectorSlice::getAutotunePhiMax() {
  return m_autotune_phi_max;
}

double FTKSectorSlice::getAutotuneCMax() {
  return m_autotune_c_max;
}

double FTKSectorSlice::getAutotuneDMax() {
  return m_autotune_d0_max;
}

double FTKSectorSlice::getAutotuneZ0Max() {
  return m_autotune_z0_max;
}

double FTKSectorSlice::getAutotuneCthetaMax() {
  return m_autotune_ctheta_max;
}

double FTKSectorSlice::getAutotuneEtaMax() {
  return m_autotune_eta_max;
}

vector<int>* FTKSectorSlice::searchSectors(FTKTrack& track) {
  
  //std::vector<int> sectors;
  Int_t bin;
  //Int_t bin_phi, bin_c, bin_d0, bin_z0, bin_ctheta;
  TBits *phi_bits;
  TBits *c_bits;
  TBits *d0_bits;
  TBits *z0_bits;
  TBits *ctheta_bits;

  int error=0;

  if ((track.getPhi()<m_phi_min) || (track.getPhi()>m_phi_max)) {
    cout << "Error: wrong phi parameter " << track.getPhi() << '\n';
    error++;
  }
  if ((2.*track.getHalfInvPt()<m_c_min) ||
      (2.*track.getHalfInvPt()>m_c_max)) {
    cout << "Error: wrong c parameter " << 2.*track.getHalfInvPt() << '\n';
    error++;
  }
  if ((track.getIP()<m_d0_min) || (track.getIP()>m_d0_max)) {
    cout << "Error: wrong d0 parameter " << track.getIP() << '\n';
    error++;
  }
  if ((track.getZ0()<m_z0_min) || (track.getZ0()>m_z0_max)) {
    cout << "Error: wrong z0 parameter " << track.getZ0() << '\n';
    error++;
  }
  if ((track.getCotTheta()<m_ctheta_min) || (track.getCotTheta()>m_ctheta_max)) {
    cout << "Error: wrong ctheta parameter " << track.getCotTheta() << '\n';
    error++;
  }
  //if(error) exit(0);


  bin = (Int_t)((track.getPhi()-m_phi_min)/m_step_phi);
  bin = (bin+ m_phi_nbins)% m_phi_nbins; // modulo operator: phi wraps around
  //bin_phi = bin;
  phi_bits = (TBits*)(bits_phi->UncheckedAt(bin));
  //phi_bits = (TBits*)(bits_phi->At(bin));
  
  bin = (Int_t)((2.*track.getHalfInvPt()-m_c_min)/m_step_c);
  // If the bin underrun or overrun set to min or max bin
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > m_c_nbins-1) ? m_c_nbins-1 : bin;
  //bin_c = bin;
  c_bits = (TBits*)(bits_c->UncheckedAt(bin));
  //c_bits = (TBits*)(bits_c->At(bin));

  bin = (Int_t)((track.getIP()-m_d0_min)/m_step_d0);
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > m_d0_nbins-1) ? m_d0_nbins-1 : bin;
  //bin_d0 = bin;
  d0_bits = (TBits*)(bits_d->UncheckedAt(bin));
  //d0_bits = (TBits*)(bits_d->At(bin));

  bin = (Int_t)((track.getZ0()-m_z0_min)/m_step_z0);
  bin = (bin < 0) ? 0 : bin;
  //bin = (bin > m_z0_nbins) ? m_z0_nbins-1 : bin;
  bin = (bin > m_z0_nbins-1) ? m_z0_nbins-1 : bin;
  //bin_z0 = bin;
  z0_bits = (TBits*)(bits_z0->UncheckedAt(bin));
  //z0_bits = (TBits*)(bits_z0->At(bin));

  bin = (Int_t)((track.getCotTheta()-m_ctheta_min)/m_step_ctheta);
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > m_ctheta_nbins-1) ? m_ctheta_nbins-1 : bin;
  //bin_ctheta = bin;
  ctheta_bits = (TBits*)(bits_ctheta->UncheckedAt(bin));
  //ctheta_bits = (TBits*)(bits_ctheta->At(bin));

  TBits result_bits(c_bits->GetNbits());

  // AND of all TBits
  /// c-slices are ill-defined ???
  /// remove 29.11.2016, STS
  ///  result_bits = *c_bits;
  ///  result_bits &= *d0_bits;
  result_bits = *d0_bits;
  result_bits &= *phi_bits;
  result_bits &= *z0_bits;
  result_bits &= *ctheta_bits;
  
  vector<int> *sectors = new vector<int>();
  unsigned int curPos = result_bits.FirstSetBit(0);
  while (curPos != result_bits.GetNbits()) {
    sectors->push_back((int) curPos);

    curPos = result_bits.FirstSetBit(curPos+1);
  }
  
  return sectors;
}
