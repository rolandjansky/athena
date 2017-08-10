/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ATLPARSLICE_ROOT_H
#define ATLPARSLICE_ROOT_H

#include <TMath.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TClonesArray.h>
#include <TBits.h>
#include <vector>
#include <iostream>
class TSlices {
 public:
  TFile *ofile; // output file
  TNtuple *n_slicedef; // ntuple with all the slice definitions
  TClonesArray *c_bits_c; // c slices
  TClonesArray *c_bits_d; // d slices
  TClonesArray *c_bits_phi; // phi slices
  TClonesArray *c_bits_z0; // z0 slices
  TClonesArray *c_bits_ctheta; // ctheta slices
  TBits *result_bits;
  //    std::vector<int> finalSectors;
  unsigned int curPos; // popSector current position pointer
  
 public:
  // slice parameters
  Int_t par_c_slices;
  Double_t par_c_max;
  Double_t par_c_min;
  
  Int_t par_d_slices;
  Double_t par_d_max;
  Double_t par_d_min;
  
  Int_t par_phi_slices;
  Double_t par_phi_max;
  Double_t par_phi_min;
  
  Int_t par_z0_slices;
  Double_t par_z0_max;
  Double_t par_z0_min;
  
  Int_t par_ctheta_slices;
  Double_t par_ctheta_max;
  Double_t par_ctheta_min;
 public:
  TSlices(int *nslices, double *min_vals, double *max_vals);
  ~TSlices();
};

  
// FlagAK: allow to work with multiple slices simultaneously, indexed by the index variable
void initNSlices(int N, int *nslices, double *min_vals, double *max_vals);
void initSlices(int index, int nsectors);
void addSectorToSlice(int index, int sector, int c, int d, int phi, int z0, int ctheta);
void saveSlices(int index, char *slice_file);
//int popSector(int index);
//void printSectors(int index);
void searchSectors(int index, double c, double d, double phi, double z0, double ctheta);
int checkSector(int index, int sector,double c, double d, double phi, double z0, double ctheta);
void loadSlices(int index, char *slice_file);

#endif // ATLPARSLICE_ROOT_H
