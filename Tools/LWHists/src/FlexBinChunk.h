/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class FlexBinChunk                        //
//                                                            //
//  Description: The basic chunk, holding bin contents for    //
//               16-bin.                                      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef FLEXBINCHUNK_H
#define FLEXBINCHUNK_H

#define FLEXBINCHUNK_NBINS 16

#include <cassert>
#include <limits>
#include "LWPools.h"
#include "LWHistGlobalSettings.h"
#include "LWHistBitUtils.h"

template <class T>
class FlexBinChunk {
public:

  FlexBinChunk();
  ~FlexBinChunk();

  //Todo: fill(..) and setBinContent(..) methods should return bool,
  //      indicating if the number of (fully?) allocated bins changed
  //      (so group can consider going to reverted mode).
  void fill(unsigned bin);
  void fill(unsigned bin, const double& weight);

  T getBinContent(unsigned bin) const;
  void setBinContent(unsigned bin, const T& val);

  void copyContents(T*cont) const;
  double Integral() const;

private:

  FlexBinChunk( const FlexBinChunk & );
  FlexBinChunk & operator= ( const FlexBinChunk & );

  char * m_data;
  uint32_t m_stages;//2 bits per bin (the two left-most for bin 0, the two right-most for bin 15)

  //Helpers for working with m_stages:
  unsigned getStage(unsigned bin) const { return (m_stages >> (2*FLEXBINCHUNK_NBINS-2*(bin+1))) & 0x3; }
  bool isEmpty(unsigned bin) const { return getStage(bin)==0x0; }
  bool isChar(unsigned bin) const { return getStage(bin)==0x1; }
  bool isShort(unsigned bin) const { return getStage(bin)==0x2; }
  bool isFull(unsigned bin) const { return getStage(bin)==0x3; }

  //NB: We could put some of these methods outside of the template (to avoid multiple instantations)
  bool stageIsEmpty(unsigned stage/*0x0, 0x1, 0x2 or 0x3*/) const { return stage==0x0; }
  bool stageIsChar(unsigned stage/*0x0, 0x1, 0x2 or 0x3*/) const { return stage==0x1; }
  bool stageIsShort(unsigned stage/*0x0, 0x1, 0x2 or 0x3*/) const { return stage==0x2; }
  bool stageIsFull(unsigned stage/*0x0, 0x1, 0x2 or 0x3*/) const { return stage==0x3; }

  void setStage(unsigned bin, unsigned stage/*0x0, 0x1, 0x2 or 0x3*/);

  unsigned calcOffset(unsigned bin ) const;

  template <class Told, class Tnew>
  unsigned changeBinStage(unsigned bin, unsigned offset);//returns new length of m_data
  template <class T2>
  T2& getBinValRef(unsigned offset);
  template <class T2>
  T2 getBinVal(unsigned offset) const;

  unsigned moveToFullStage(unsigned bin, unsigned currentstage,unsigned offset);//returns new length of m_data

#if FLEXBINCHUNK_CONVERTALLTHRESHOLD != FLEXBINCHUNK_NBINS
  void possibleMoveAllBins(unsigned newarrsize);//do nothing if newarrsize==0
  template <class T2>
  void moveAllBinsToStage(uint16_t oldallocsize);
#define FLEXBINCHUNK_TESTMOVEALLBINS(newarrsize) if (newarrsize) possibleMoveAllBins(newarrsize)
#else
#define FLEXBINCHUNK_TESTMOVEALLBINS(newarrsize) if (newarrsize);
//(not empty to avoid compiler warnings)
#endif
};

#include "FlexBinChunk.icc"

#endif
