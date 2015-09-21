/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class FlexErrArrayGroup                   //
//                                                            //
//  Description: Covering up to 128 bins, the class uses up   //
//               to 8 FlexBinChunk's and up to 8 double[16]   //
//               internally for bin contents and sumw.        //
//               Alternatively it uses just one long array    //
//               if the overhead from the subgroups becomes   //
//               too high (when almost all bins are fully     //
//               used).                                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef FLEXERRARRAYGROUP_H
#define FLEXERRARRAYGROUP_H

#define FLEXERRARRAYGROUP_NCHUNKS 8
#define FLEXERRARRAYGROUP_MAXBINS (FLEXBINCHUNK_NBINS*FLEXERRARRAYGROUP_NCHUNKS)

#ifdef LW_STRICT_ROOT_BEHAVIOUR
#define STRICT_ROOT_PAR(x) , x
#else
#define STRICT_ROOT_PAR(x)
#endif

#ifndef NDEBUG
template <class T> inline bool isNaN(const T&t) { return t!=t; }
template <class T> inline bool bothNaN(const T&t1, const T&t2) { return isNaN(t1)&&isNaN(t2); }

#endif

#include "FlexBinChunk.h"
#include "LWHistBitUtils.h"
#include <cmath>
#include <cstdlib>
#include <cassert>

template <class T>
class FlexErrArrayGroup {
public:

  FlexErrArrayGroup(unsigned nbins = FLEXERRARRAYGROUP_MAXBINS);
  ~FlexErrArrayGroup();

  void fill(unsigned bin STRICT_ROOT_PAR(bool pretendSumWMode));
  void fill(unsigned bin, const double& weight STRICT_ROOT_PAR(bool pretendSumWMode));

  T getBinContent(unsigned bin) const;
  double getBinError(unsigned bin) const;
  void getBinContentAndError(unsigned bin, T& content, double& error ) const;

  //  void setBinContent(unsigned bin, const T& val);
  void setBinContent(unsigned bin, const T& val STRICT_ROOT_PAR(bool pretendSumWMode));
  void setBinError(unsigned bin, const double& error);
  void setBinContentAndError(unsigned bin, const T& content, const double& error);

  bool holdsSeparateSumW2Info() const { return m_sumw2allocated; }
  void copyContents(T*cont, double*err=0) const;//Won't copy errors if err==0.

  unsigned getNBins() const { return m_nbins; }

  double Integral() const;
  void resetActiveBinLoop();
  bool getNextActiveBin(unsigned& bin, T& content, double& error);

private:

  FlexErrArrayGroup( const FlexErrArrayGroup & );
  FlexErrArrayGroup & operator= ( const FlexErrArrayGroup & );

  void** m_indices;//Pointer(s) to data. First pointer(s) to bin
		   //content (chunks or one T array), then follows
		   //pointer(s) to sumw2.
  uint8_t m_nbins;//since we might have less than the full 8*16 bins... Actually only needed in copycontents (could be passed as arg)
  uint8_t m_chunksallocated;//8 bits telling which chunks are allocated
  uint8_t m_sumw2allocated;//8 bits telling which sumw2 are allocated
  uint8_t m_fastloop_nextbin2check;//fixme: do it by chunk instead!

  //Fixme: We have 1 byte (2 if we get rid of m_nbins) available here due to alignment... use it for something? cache nindicesusedbychunks?
  void addIndexPointer(unsigned position, void* newval);

  unsigned nIndicesUsedByChunks() const { return m_chunksallocated ? LWHistBitUtils::countSetBits<uint8_t>(m_chunksallocated) : 0; }//FIXME: CACHE
  unsigned nIndicesUsedByErrors() const { return m_sumw2allocated ? LWHistBitUtils::countSetBits<uint8_t>(m_sumw2allocated) : 0; }


  //The following bit-handling help could be outside the class to avoid template duplication:
  unsigned getGroupIndex(unsigned bin) const { assert(bin/FLEXBINCHUNK_NBINS<FLEXERRARRAYGROUP_NCHUNKS);
                                             return bin/FLEXBINCHUNK_NBINS; }
  unsigned getChunkBin(unsigned bin) const { return bin%FLEXBINCHUNK_NBINS; }

  FlexBinChunk<T>* getChunk(unsigned igroup);
  const FlexBinChunk<T>* getChunkNoAlloc(unsigned igroup) const;//returns 0 if no allocated
  double* getErrChunk(unsigned igroup);
  const double* getErrChunkNoAlloc(unsigned igroup) const;//returns 0 if no allocated
};

#include "FlexErrArrayGroup.icc"

#endif
