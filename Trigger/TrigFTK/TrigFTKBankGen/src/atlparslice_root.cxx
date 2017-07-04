/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigFTKBankGen/atlparslice_root.h"
#include <cstdlib>

TSlices **s;

TSlices::TSlices(int *nslices, double *minvals, double *maxvals) {
  curPos=0;
  result_bits=0;
  ofile=0;
  par_c_slices = nslices[0];
  par_c_max = maxvals[0];
  par_c_min = minvals[0];
  
  par_d_slices = nslices[1];
  par_d_max = maxvals[1];
  par_d_min = minvals[1];

  par_phi_slices = nslices[2];
  par_phi_max = maxvals[2];
  par_phi_min = minvals[2];
  
  par_z0_slices = nslices[3];
  par_z0_max = maxvals[3];
  par_z0_min = minvals[3];
  
  par_ctheta_slices = nslices[4];
  par_ctheta_max = maxvals[4];
  par_ctheta_min = minvals[4];
}

TSlices::~TSlices() {
  if(ofile) if(ofile->IsOpen()) ofile->Close();
  return;
}

void initNSlices(int N, int *nslices, double *minvals, double *maxvals) {
  s = new TSlices*[N];
  if(!s) {
    std::cout << "Failed to allocate TSlices*[] of size " << N << "; exiting..." << std::endl;
    exit(-1);
  }
  for(int i=0;i<N;i++) {
    s[i] = new TSlices(nslices,minvals,maxvals);
    if(!s) {
      std::cout << "Failed to allocate TSlices[" << i << "]; exiting..." << std::endl;
      exit(-1);
    }
  }
}

void initSlices(int index, int nsectors) {
  s[index]->c_bits_c = new TClonesArray("TBits",s[index]->par_c_slices);
  for(Int_t i = 0; i < s[index]->par_c_slices; ++i) {
    new((*s[index]->c_bits_c)[i]) TBits(nsectors);
  }
  s[index]->c_bits_d = new TClonesArray("TBits",s[index]->par_d_slices);
  for(Int_t i = 0; i < s[index]->par_d_slices; ++i) {
    new((*s[index]->c_bits_d)[i]) TBits(nsectors);
  }
  s[index]->c_bits_phi = new TClonesArray("TBits",s[index]->par_phi_slices);
  for(Int_t i = 0; i < s[index]->par_phi_slices; ++i) {
    new((*s[index]->c_bits_phi)[i]) TBits(nsectors);
  }
  s[index]->c_bits_z0 = new TClonesArray("TBits",s[index]->par_z0_slices);
  for(Int_t i = 0; i < s[index]->par_z0_slices; ++i) {
    new((*s[index]->c_bits_z0)[i]) TBits(nsectors);
  }
  s[index]->c_bits_ctheta = new TClonesArray("TBits",s[index]->par_ctheta_slices);
  for(Int_t i = 0; i < s[index]->par_ctheta_slices; ++i) {
    new((*s[index]->c_bits_ctheta)[i]) TBits(nsectors);
  }
}
void addSectorToSlice(int index, int sector, int c, int d, int phi, int z0, int ctheta) {
  Int_t bin;
  TBits *t;

#ifdef DEBUG_ADDSECTORTOSLICE
  Int_t bin_c,bin_d,bin_phi,bin_z0,bin_ctheta;
#endif
    bin =c;
    bin = (bin < 0) ? 0 : bin;
    bin = (bin > s[index]->par_c_slices-1) ? s[index]->par_c_slices-1 : bin;
#ifdef DEBUG_ADDSECTORTOSLICE
    bin_c = bin;
#endif
    t = (TBits *)(s[index]->c_bits_c->UncheckedAt(bin));
    t->SetBitNumber(sector);
        
    bin = d;
    bin = (bin < 0) ? 0 : bin;
    bin = (bin > s[index]->par_d_slices-1) ? s[index]->par_d_slices-1 : bin;
#ifdef DEBUG_ADDSECTORTOSLICE
    bin_d = bin;
#endif
    t = (TBits *)(s[index]->c_bits_d->UncheckedAt(bin));
    t->SetBitNumber(sector);
    
    bin= phi;
    bin = (bin < 0) ? 0 : bin;
    bin = (bin > s[index]->par_phi_slices-1) ? s[index]->par_phi_slices-1 : bin;
#ifdef DEBUG_ADDSECTORTOSLICE
    bin_phi = bin;
#endif
    t = (TBits *)(s[index]->c_bits_phi->UncheckedAt(bin));
    t->SetBitNumber(sector);

    
    bin =z0;
    bin = (bin < 0) ? 0 : bin;
    bin = (bin > s[index]->par_z0_slices-1) ? s[index]->par_z0_slices-1 : bin;
#ifdef DEBUG_ADDSECTORTOSLICE
    bin_z0 = bin;
#endif
    t = (TBits *)(s[index]->c_bits_z0->UncheckedAt(bin));
    t->SetBitNumber(sector);
      
    bin = ctheta;
    bin = (bin < 0) ? 0 : bin;
    bin = (bin > s[index]->par_ctheta_slices-1) ? s[index]->par_ctheta_slices-1 : bin;
#ifdef DEBUG_ADDSECTORTOSLICE
    bin_ctheta = bin;
#endif
    t = (TBits *)(s[index]->c_bits_ctheta->UncheckedAt(bin));
    t->SetBitNumber(sector);

#ifdef DEBUG_ADDSECTORTOSLICE
    printf("AK %d %d %f %d %f %d %f %d %f %d %f\n",sector,bin_c,1/2.0/c,bin_d,d,bin_phi,phi,bin_z0,z0,bin_ctheta,ctheta);
#endif
}

void saveSlices(int index, char *slice_file) {
  s[index]->ofile = new TFile(slice_file,"recreate");
  s[index]->c_bits_c->Write("c_bits_c",TObject::kSingleKey);
  s[index]->c_bits_d->Write("c_bits_d",TObject::kSingleKey);
  s[index]->c_bits_phi->Write("c_bits_phi",TObject::kSingleKey);
  s[index]->c_bits_z0->Write("c_bits_z0",TObject::kSingleKey);
  s[index]->c_bits_ctheta->Write("c_bits_ctheta",TObject::kSingleKey);
  s[index]->ofile->Close();
}

void loadSlices(int index, char *slice_file) {
  s[index]->ofile = new TFile(slice_file);
  s[index]->c_bits_c = (TClonesArray*)s[index]->ofile->Get("c_bits_c");
  s[index]->c_bits_d = (TClonesArray*)s[index]->ofile->Get("c_bits_d");
  s[index]->c_bits_phi = (TClonesArray*)s[index]->ofile->Get("c_bits_phi");
  s[index]->c_bits_z0 = (TClonesArray*)s[index]->ofile->Get("c_bits_z0");
  s[index]->c_bits_ctheta = (TClonesArray*)s[index]->ofile->Get("c_bits_ctheta");
}

void searchSectors(int index, double c, double d, double phi, double z0, double ctheta) {
  TBits *c_bits;
  TBits *d_bits;
  TBits *phi_bits;
  TBits *z0_bits;
  TBits *ctheta_bits;
  Int_t bin;
  Double_t step;

#ifdef DEBUG_SEARCH_SECTORS
  Int_t bin1,bin2,bin3,bin4,bin5;
#endif
  
  // Find bit arrays
  // Compute form parameters the step size
  step = (s[index]->par_c_max - s[index]->par_c_min)/s[index]->par_c_slices;
  // Compute the bin
  bin = (Int_t)((c - s[index]->par_c_min)/step);
  // If the bin underrun or overrun set to min or max bin
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > s[index]->par_c_slices-1) ? s[index]->par_c_slices-1 : bin;
#ifdef DEBUG_SEARCH_SECTORS
  bin1=bin;
#endif
  // Fetch TBits for the bin, the use of UncheckedAt is fast but risky
  c_bits = (TBits *)(s[index]->c_bits_c->UncheckedAt(bin));
  // Same for other parameters
  step = (s[index]->par_d_max - s[index]->par_d_min)/s[index]->par_d_slices;
  bin = (Int_t)((d - s[index]->par_d_min)/step);
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > s[index]->par_d_slices-1) ? s[index]->par_d_slices-1 : bin;
#ifdef DEBUG_SEARCH_SECTORS
  bin2=bin;
#endif
  d_bits = (TBits *)(s[index]->c_bits_d->UncheckedAt(bin));
  step = (s[index]->par_phi_max - s[index]->par_phi_min)/s[index]->par_phi_slices;
  bin = (Int_t)((phi - s[index]->par_phi_min)/step);
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > s[index]->par_phi_slices-1) ? s[index]->par_phi_slices-1 : bin;
#ifdef DEBUG_SEARCH_SECTORS
  bin3=bin;
#endif
  phi_bits = (TBits *)(s[index]->c_bits_phi->UncheckedAt(bin));
  step = (s[index]->par_z0_max - s[index]->par_z0_min)/s[index]->par_z0_slices;
  bin = (Int_t)((z0 - s[index]->par_z0_min)/step);
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > s[index]->par_z0_slices-1) ? s[index]->par_z0_slices-1 : bin;
#ifdef DEBUG_SEARCH_SECTORS
  bin4=bin;
#endif
  z0_bits = (TBits *)(s[index]->c_bits_z0->UncheckedAt(bin));
  step = (s[index]->par_ctheta_max - s[index]->par_ctheta_min)/s[index]->par_ctheta_slices;
  bin = (Int_t)((ctheta - s[index]->par_ctheta_min)/step);
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > s[index]->par_ctheta_slices-1) ? s[index]->par_ctheta_slices-1 : bin;
#ifdef DEBUG_SEARCH_SECTORS
  bin5=bin;
#endif
  ctheta_bits = (TBits *)(s[index]->c_bits_ctheta->UncheckedAt(bin));

  // Initialize result bits
  if(s[index]->result_bits == NULL)
    s[index]->result_bits = new TBits(c_bits->GetNbits());

  // AND of all TBits
  *s[index]->result_bits = *c_bits;
  *s[index]->result_bits &= *d_bits;
  *s[index]->result_bits &= *phi_bits;
  *s[index]->result_bits &= *z0_bits;
  *s[index]->result_bits &= *ctheta_bits;

#ifdef DEBUG_SEARCH_SECTORS
   printf("pt=%.2f\td0=%.2f\tphi=%.2f\tz0=%.2f\tctheta=%.2f\n",1/(2*c),d,phi,z0,ctheta);
   printf("bins = %d\t%d\t%d\t%d\t%d\n",bin1,bin2,bin3,bin4,bin5);
   std::cout << c_bits->CountBits() << std::endl;
   std::cout << d_bits->CountBits() << std::endl;
   std::cout << phi_bits->CountBits() << std::endl;
   std::cout << z0_bits->CountBits() << std::endl;
   std::cout << ctheta_bits->CountBits() << std::endl;
   std::cout << "FINAL: " << (*s[index]->result_bits).CountBits() << std::endl;
#endif

  // Reset position
  s[index]->curPos=0;
  // printf("phi=%f, bins = %d,%d,%d,%d,%d\n",phi,bin1,bin2,bin3,bin4,bin5);

}

int checkSector(int index, int sector, double c, double d, double phi, double z0, double ctheta) {
  TBits *c_bits;
  TBits *d_bits;
  TBits *phi_bits;
  TBits *z0_bits;
  TBits *ctheta_bits;
  Double_t step;
  Int_t bin;

  // Find bit arrays
  step = (s[index]->par_c_max - s[index]->par_c_min)/s[index]->par_c_slices;
  bin = (Int_t)((c - s[index]->par_c_min)/step);
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > s[index]->par_c_slices-1) ? s[index]->par_c_slices-1 : bin;
  c_bits = (TBits *)(s[index]->c_bits_c->UncheckedAt(bin));
  step = (s[index]->par_d_max - s[index]->par_d_min)/s[index]->par_d_slices;
  bin = (Int_t)((d - s[index]->par_d_min)/step);
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > s[index]->par_d_slices-1) ? s[index]->par_d_slices-1 : bin;
  d_bits = (TBits *)(s[index]->c_bits_d->UncheckedAt(bin));
  step = (s[index]->par_phi_max - s[index]->par_phi_min)/s[index]->par_phi_slices;
  bin = (Int_t)((phi - s[index]->par_phi_min)/step);
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > s[index]->par_phi_slices-1) ? s[index]->par_phi_slices-1 : bin;
  phi_bits = (TBits *)(s[index]->c_bits_phi->UncheckedAt(bin));
  step = (s[index]->par_z0_max - s[index]->par_z0_min)/s[index]->par_z0_slices;
  bin = (Int_t)((z0 - s[index]->par_z0_min)/step);
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > s[index]->par_z0_slices-1) ? s[index]->par_z0_slices-1 : bin;
  z0_bits = (TBits *)(s[index]->c_bits_z0->UncheckedAt(bin));
  step = (s[index]->par_ctheta_max - s[index]->par_ctheta_min)/s[index]->par_ctheta_slices;
  bin = (Int_t)((ctheta - s[index]->par_ctheta_min)/step);
  bin = (bin < 0) ? 0 : bin;
  bin = (bin > s[index]->par_ctheta_slices-1) ? s[index]->par_ctheta_slices-1 : bin;
  ctheta_bits = (TBits *)(s[index]->c_bits_ctheta->UncheckedAt(bin));

  if(c_bits->TestBitNumber(sector) &&
     d_bits->TestBitNumber(sector) &&
     phi_bits->TestBitNumber(sector) &&
     z0_bits->TestBitNumber(sector) &&
     ctheta_bits->TestBitNumber(sector))
    return 1;
  return 0;
  // done
}

/*
double autotuneMinC(int index) {
  Double_t step;
  TBits *tb;
  for(Int_t i = 0; i < s[index]->c_bits_c->GetEntries(); ++i) {
    tb = (TBits *)(s[index]->c_bits_c->UncheckedAt(i));
    if(tb->FirstSetBit() != tb->GetNbits()) {
        step = (s[index]->par_c_max - s[index]->par_c_min)/s[index]->par_c_slices;
	return s[index]->par_c_min + step*i;
    }
  }
}
double autotuneMinD(int index) {
  Double_t step;
  TBits *tb;
  for(Int_t i = 0; i < s[index]->c_bits_d->GetEntries(); ++i) {
    tb = (TBits *)(s[index]->c_bits_d->UncheckedAt(i));
    if(tb->FirstSetBit() != tb->GetNbits()) {
        step = (s[index]->par_d_max - s[index]->par_d_min)/s[index]->par_d_slices;
	return s[index]->par_d_min + step*i;
    }
  }
}
double autotuneMinPhi(int index) {
  Double_t step;
  Double_t val;
  bool ignore_neg = false;
  TBits *tb;
  for(Int_t i = 0; i < s[index]->c_bits_phi->GetEntries(); ++i) {
    tb = (TBits *)(s[index]->c_bits_phi->UncheckedAt(i));
    if(tb->FirstSetBit() != tb->GetNbits()) {
      if(i==0) ignore_neg=true; // Special fix at the boundary -pi
      step = (s[index]->par_phi_max - s[index]->par_phi_min)/s[index]->par_phi_slices;
      val = (double)s[index]->par_phi_min + (double)step*i;
      if(ignore_neg && val<0) continue;
      return val;
    }
  }
}
double autotuneMinZ0(int index) {
  Double_t step;
  TBits *tb;
  for(Int_t i = 0; i < s[index]->c_bits_z0->GetEntries(); ++i) {
    tb = (TBits *)(s[index]->c_bits_z0->UncheckedAt(i));
    if(tb->FirstSetBit() != tb->GetNbits()) {
      step = (s[index]->par_z0_max - s[index]->par_z0_min)/s[index]->par_z0_slices;
      return s[index]->par_z0_min + step*i;
    }
  }
}
double autotuneMinCtheta(int index) {
  Double_t step;
  TBits *tb;
  for(Int_t i = 0; i < s[index]->c_bits_ctheta->GetEntries(); ++i) {
    tb = (TBits *)(s[index]->c_bits_ctheta->UncheckedAt(i));
    if(tb->FirstSetBit() != tb->GetNbits()) {
        step = (s[index]->par_ctheta_max - s[index]->par_ctheta_min)/s[index]->par_ctheta_slices;
	return s[index]->par_ctheta_min + step*i;
    }
  }
}
double autotuneMinEta(int index) {
  Double_t step;
  TBits *tb;
  for(Int_t i = 0; i < s[index]->c_bits_ctheta->GetEntries(); ++i) {
    tb = (TBits *)(s[index]->c_bits_ctheta->UncheckedAt(i));
    if(tb->FirstSetBit() != tb->GetNbits()) {
        step = (s[index]->par_ctheta_max - s[index]->par_ctheta_min)/s[index]->par_ctheta_slices;
	return TMath::ASinH(s[index]->par_ctheta_min + step*i);
    }
  }
}

double autotuneMaxC(int index) {
  Double_t step;
  TBits *tb;
  for(Int_t i = s[index]->c_bits_c->GetEntries(); i > 0; --i) {
    tb = (TBits *)(s[index]->c_bits_c->UncheckedAt(i-1));
    if(tb->FirstSetBit() != tb->GetNbits()) {
        step = (s[index]->par_c_max - s[index]->par_c_min)/s[index]->par_c_slices;
	return s[index]->par_c_max - step*(s[index]->c_bits_c->GetEntries() - i);
    }
  }
}
double autotuneMaxD(int index) {
  Double_t step;
  TBits *tb;
  for(Int_t i = s[index]->c_bits_d->GetEntries(); i > 0; --i) {
    tb = (TBits *)(s[index]->c_bits_d->UncheckedAt(i-1));
    if(tb->FirstSetBit() != tb->GetNbits()) {
        step = (s[index]->par_d_max - s[index]->par_d_min)/s[index]->par_d_slices;
	return s[index]->par_d_max - step*(s[index]->c_bits_d->GetEntries() - i);
    }
  }
}
// this magic is needed to properly handle +/- Pi boundary. I won't work with <=4 regions
double autotuneMaxPhi(int index) {
  Double_t step;
  Double_t val;
  bool ignore_pos = false;
  Int_t entries = s[index]->c_bits_phi->GetEntries();
  TBits *tb;
  step = (s[index]->par_phi_max - s[index]->par_phi_min)/s[index]->par_phi_slices;
  for(Int_t i = entries; i > 0; --i) {
    tb = (TBits *)(s[index]->c_bits_phi->UncheckedAt(i-1));
    if(tb->FirstSetBit() != tb->GetNbits()) {
      if(i==entries) ignore_pos=true; // Special fix at the boundary +pi
      val = s[index]->par_phi_max - step*(s[index]->c_bits_phi->GetEntries() - i);
      if(ignore_pos && val>0) continue;
      return (ignore_pos ? val + 2*TMath::Pi() : val);
    }
  }
}
double autotuneMaxZ0(int index) {
  Double_t step;
  TBits *tb;
  for(Int_t i = s[index]->c_bits_z0->GetEntries(); i > 0; --i) {
    tb = (TBits *)(s[index]->c_bits_z0->UncheckedAt(i-1));
    if(tb->FirstSetBit() != tb->GetNbits()) {
        step = (s[index]->par_z0_max - s[index]->par_z0_min)/s[index]->par_z0_slices;
	return s[index]->par_z0_max - step*(s[index]->c_bits_z0->GetEntries() - i);
    }
  }
}
double autotuneMaxCtheta(int index) {
  Double_t step;
  TBits *tb;
  for(Int_t i = s[index]->c_bits_ctheta->GetEntries(); i > 0; --i) {
    tb = (TBits *)(s[index]->c_bits_ctheta->UncheckedAt(i-1));
    if(tb->FirstSetBit() != tb->GetNbits()) {
        step = (s[index]->par_ctheta_max - s[index]->par_ctheta_min)/s[index]->par_ctheta_slices;
	return s[index]->par_ctheta_max - step*(s[index]->c_bits_ctheta->GetEntries() - i);
    }
  }
}
double autotuneMaxEta(int index) {
  Double_t step;
  TBits *tb;
  for(Int_t i = s[index]->c_bits_ctheta->GetEntries(); i > 0; --i) {
    tb = (TBits *)(s[index]->c_bits_ctheta->UncheckedAt(i-1));
    if(tb->FirstSetBit() != tb->GetNbits()) {
        step = (s[index]->par_ctheta_max - s[index]->par_ctheta_min)/s[index]->par_ctheta_slices;
	return TMath::ASinH(s[index]->par_ctheta_max - step*(s[index]->c_bits_ctheta->GetEntries() - i));
    }
  }
}


void printSectors(int index) {
  unsigned int pos = 0;
  printf("Matched these sectors: ");
  while(1) {
    pos = s[index]->result_bits->FirstSetBit(pos+1);
    if(pos == s[index]->result_bits->GetNbits()) {
      pos = 0;
      break;
    }
    printf("%d ",pos);
  }
  printf("\n");
}

int popSector(int index) {
  unsigned int pbup = s[index]->curPos;
  s[index]->curPos = s[index]->result_bits->FirstSetBit(s[index]->curPos+1);
  if(s[index]->curPos == s[index]->result_bits->GetNbits()) {
    s[index]->curPos = 0;
    return -1;
  } else {
    return s[index]->curPos;
  }
}
*/
