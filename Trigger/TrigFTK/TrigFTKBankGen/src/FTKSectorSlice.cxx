/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigFTKBankGen/FTKSectorSlice.h"
#include "TrigFTKSim/FTKSetup.h"
#include <TROOT.h>
#include <TFile.h>
#include <TMatrixD.h>
#include <TClonesArray.h>
#include <TMath.h>
#include <GaudiKernel/MsgStream.h>
#include <iostream>

using namespace std;

FTKSectorSlice::FTKSectorSlice() {
}

FTKSectorSlice::~FTKSectorSlice() {
}

void FTKSectorSlice::getSliceRanges
(std::pair<double,double> &phiRange,
 std::pair<double,double> &cRange,
 std::pair<double,double> &d0Range,
 std::pair<double,double> &z0Range,
 std::pair<double,double> &cotRange) {
   if(m_slicePhi) phiRange=m_slicePhi->GetRangeWithSectors();
   if(m_sliceC) cRange=m_sliceC->GetRangeWithSectors();
   if(m_sliceD0) d0Range=m_sliceD0->GetRangeWithSectors();
   if(m_sliceZ0) z0Range=m_sliceZ0->GetRangeWithSectors();
   if(m_sliceCot) cotRange=m_sliceCot->GetRangeWithSectors();
}

bool FTKSectorSlice::loadSlices(std::string slice_file_path,
                                std::pair<double,double> &phiRange,
                                std::pair<double,double> &cRange,
                                std::pair<double,double> &d0Range,
                                std::pair<double,double> &z0Range,
                                std::pair<double,double> &cotRange) {
   // decide which boundaries to use
   //
   // these numbers should agree with those used for the S&C generation
   //
   //
   // in old slice files (pre-2019),
   // this information was not saved in the slice file
   //
   // this creates the problem of consistency
   //
   // there are three sources of slice boundaries
   // (1) from slice file
   // (2) from job options
   // (3) hard-coded in this method
   //
   // use (1) if possible
   // else use (2) if possible
   // else use (3)


   // delete old slices (if any)

   // open slice file
   TFile *slice_file=TFile::Open(slice_file_path.c_str());
   if(!slice_file) return false;

   TMatrixD *boundaries;
   slice_file->GetObject("slice_boundaries",boundaries);
   if(boundaries) {
      cRange.first=(*boundaries)(0,0);
      cRange.second=(*boundaries)(0,1);
      d0Range.first=(*boundaries)(1,0);
      d0Range.second=(*boundaries)(1,1);
      phiRange.first=(*boundaries)(2,0);
      phiRange.second=(*boundaries)(2,1);
      z0Range.first=(*boundaries)(3,0);
      z0Range.second=(*boundaries)(3,1);
      cotRange.first=(*boundaries)(4,0);
      cotRange.second=(*boundaries)(4,1);
      delete boundaries;
   }

   if(!(phiRange.first<phiRange.second)) {
      phiRange.first=-TMath::Pi();
      phiRange.second=TMath::Pi();
   }
   if(!(cRange.first<cRange.second)) {
      cRange.first=-0.0007;
      cRange.second=0.0007;
   }
   if(!(d0Range.first<d0Range.second)) {
      d0Range.first=-2.2;
      d0Range.second=2.2;
   }
   if(!(z0Range.first<z0Range.second)) {
      z0Range.first=-120.;
      z0Range.second=120.;
   }
   if(!(cotRange.first<cotRange.second)) {
      cotRange.first=TMath::SinH(-3);
      cotRange.second=TMath::SinH(3);
   }

   m_slicePhi=std::unique_ptr<FTKSectorSlice_oneVar>
      (new FTKSectorSlice_oneVar
       (slice_file,"c_bits_phi",phiRange.first,phiRange.second,true));
   m_sliceC=std::unique_ptr<FTKSectorSlice_oneVar>
      (new FTKSectorSlice_oneVar
       (slice_file,"c_bits_c",cRange.first,cRange.second,false));
   m_sliceD0=std::unique_ptr<FTKSectorSlice_oneVar>
      (new FTKSectorSlice_oneVar
       (slice_file,"c_bits_d",d0Range.first,d0Range.second,false));
   m_sliceZ0=std::unique_ptr<FTKSectorSlice_oneVar>
      (new FTKSectorSlice_oneVar
       (slice_file,"c_bits_z0",z0Range.first,z0Range.second,false));
   m_sliceCot=std::unique_ptr<FTKSectorSlice_oneVar>
      (new FTKSectorSlice_oneVar
       (slice_file,"c_bits_ctheta",cotRange.first,cotRange.second,false));

   selectSlices();

   delete slice_file;
   return true;
}

void FTKSectorSlice::selectSlices(bool usePhi,
                                  bool useC,
                                  bool useD0,
                                  bool useZ0,
                                  bool useCot) {
   m_usePhi= usePhi && m_slicePhi;
   m_useC = useC && m_sliceC;
   m_useD0 = useD0 && m_sliceD0;
   m_useZ0 = useZ0 && m_sliceZ0;
   m_useCot = useCot && m_sliceCot;
}

vector<int>* FTKSectorSlice::searchSectors(FTKTrack& track) {
   // sector candidates are found by checking slices in the coordinates
   // ther are up to five coordinates:
   //   phi, C, D0, Z0, Cot
   // for each coordinate there is a flag m_useXXX
   // on top of that, the coordinate onlyu contributes if the
   // track's coordinate is in range (find() method is non-null)

   vector<TBits const *> check; // set of Tbits to be checked
   check.reserve(5);

   TBits const *bits; // temporary pointer to the coordinate of interest

   // locate phi slice
   if(m_usePhi &&((bits=m_slicePhi->find(track.getPhi()))) )
      check.push_back(bits);

   // locate curvature slide
   if(m_useC && ((bits=m_sliceC->find(2.*track.getHalfInvPt()))) ) 
      check.push_back(bits);

   // locate D0 slice
   if(m_useD0 && ((bits=m_sliceD0->find(track.getIP()))) )
      check.push_back(bits);

   // locate Z0 slice
   if(m_useZ0 && ((bits=m_sliceZ0->find(track.getZ0()))) )
      check.push_back(bits);

   // locate cot(theta) slice
   if(m_useCot && ((bits=m_sliceCot->find(track.getCotTheta()))) )
      check.push_back(bits);

   // result: a new list of sector numbers
   vector<int> *sectors = new vector<int>();

   if(check.size()) {
      TBits result_bits(*check[0]);
      for(size_t i=1;i<check.size();i++) {
         // AND of all TBits
         result_bits &= *check[i];
      }
      // extract list of sectors
      // sector numbers : all TBit positions whcih are true
      unsigned int curPos = result_bits.FirstSetBit(0);
      unsigned int nMax=result_bits.GetNbits();
      while (curPos != nMax) {
         sectors->push_back((int) curPos);
         curPos = result_bits.FirstSetBit(curPos+1);
      }
   }
   return sectors;
}


//================================ FTKSectorSlice_oneVar =================

FTKSectorSlice_oneVar::~FTKSectorSlice_oneVar() {
   if(m_bits) delete m_bits;
}

FTKSectorSlice_oneVar::FTKSectorSlice_oneVar
(TDirectory *dir,std::string slice_file_name,
 double xmin,double xmax,bool isPhi) {
   m_maxRange.first=xmin;
   m_maxRange.second=xmax;
   TString name(slice_file_name.c_str());
   m_bits=(TClonesArray*) (dir->Get(name)->Clone(name+"_clone"));
   m_nbins=0;
   m_step=0.;
   m_wrapAround=isPhi;
   if(m_bits) {
      m_nbins=m_bits->GetEntries();
      m_step=(m_maxRange.second-m_maxRange.first)/m_nbins;
      int i0,i1;
      if(!m_wrapAround) {
         for(i0=0;(i0<m_nbins-1) &&
                !((TBits const *)m_bits->UncheckedAt(i0))->CountBits();i0++);
         for(i1=m_nbins-1;(i1>0) &&
                !((TBits const *)m_bits->UncheckedAt(i1))->CountBits();i1--) { }
      } else {
         int j;
         vector<char> phi(m_nbins);
         for(j=0;j<m_nbins;j++) { 
            if(((TBits const *)m_bits->UncheckedAt(j))->CountBits()) {
               // add safety margin: +/- 1 bin
               phi[(j+m_nbins-1)%m_nbins]++;
               phi[(j+m_nbins)%m_nbins]++;
               phi[(j+m_nbins+1)%m_nbins]++;
            }
         }
         // locate largest continuous region
         int gap0=-1,gapStart=-1,gapSize=0;
         for(j=0;j<2*m_nbins;j++) { 
            if(phi[j%m_nbins]) {
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
      m_rangeWithSectors.first=xmin+((xmax-xmin)*i0)/m_nbins;
      m_rangeWithSectors.second=xmin+((xmax-xmin)*(i1+1))/m_nbins;
   }
}

TBits const *FTKSectorSlice_oneVar::find(double x) {
   int i;
   if(!m_wrapAround) {
      // truncate floats
      if(x<m_maxRange.first) {
         i=-1;
      } else if(x>=m_maxRange.second) {
         i=m_nbins;
      } else {
         i=(int) ((x-m_maxRange.first)/m_step);
         // truncate int
         if(i>=m_nbins) i=m_nbins-1;
         if(i<0) i=0;
      }
   } else {
      i=(int)((x-m_maxRange.first)/m_step);
      i %= m_nbins; // range   -m_nbins<i<m_nbins
      i = (i+m_nbins)%m_nbins; // range 0 <= i < m_nbins
   }
   TBits const *r=0;
   if((i>=0) &&(i<m_nbins)) {
      r=(TBits*)(m_bits->UncheckedAt(i));
   }
   return r;
}
