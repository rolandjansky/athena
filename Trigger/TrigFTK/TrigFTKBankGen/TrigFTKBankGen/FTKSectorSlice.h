/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef FTKSectorSlice_h
#define FTKSectorSlice_h

#include <utility>
#include <TROOT.h>
#include <TBits.h>
#include <TrigFTKSim/FTKTrack.h>
#include <memory>

class FTKSectorSlice_oneVar {
 public:
   // a slice is a table
   // there is a range in a given variable
   // the range is binned (number of bins =  TClonesArray size)
   // for each bin there is bit vector TBits
   // the bit index is the sector number
   // it the bit is set, that sector may produce tracks in the given bin
   FTKSectorSlice_oneVar(TDirectory *dir,std::string slice_file_name,
                         double xmin,double xmax,bool isPhi);
   ~FTKSectorSlice_oneVar();
   std::pair<double,double> const &GetRangeWithSectors(void) const { return m_rangeWithSectors; }
   TBits const *find(double x);
 protected:
   bool m_wrapAround;
   int m_nbins;
   double m_step;
   TClonesArray *m_bits;
   std::pair<double,double> m_maxRange; // range of this slice 
   std::pair<double,double> m_rangeWithSectors; // parameter range in which there are non-empty bins
};

class FTKSectorSlice {

 public:
  FTKSectorSlice();
  ~FTKSectorSlice();

  bool loadSlices(std::string slice_file_path,
                  std::pair<double,double> &phiRange,
                  std::pair<double,double> &cRange,
                  std::pair<double,double> &d0Range,
                  std::pair<double,double> &z0Range,
                  std::pair<double,double> &cotRange);

  void selectSlices(bool usePhi=true,
                    bool useC=false,
                    bool useD0=true,
                    bool useZ0=true,
                    bool useCot=true);

  std::vector<int> *searchSectors(FTKTrack& track);

  void getSliceRanges( std::pair<double,double> &phiRange,
                       std::pair<double,double> &cRange,
                       std::pair<double,double> &d0Range,
                       std::pair<double,double> &z0Range,
                       std::pair<double,double> &cotRange);

 private:

  std::unique_ptr<FTKSectorSlice_oneVar>
     m_slicePhi,m_sliceC,m_sliceD0,m_sliceZ0,m_sliceCot;

  bool m_usePhi,m_useC,m_useD0,m_useZ0,m_useCot;
};

#endif // FTKSectorSlice_h
