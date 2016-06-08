/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class FlexErrArray                        //
//                                                            //
//  Description: Array of bins and possible associated errors //
//               used internally in THXY_LW classes.          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef FLEXERRARRAY_H
#define FLEXERRARRAY_H

//A few global typedefs:

#include "LWHistGlobalSettings.h"
#include "FlexErrArrayGroup.h"

#include <cmath>
#include <cassert>

#define FLEX1DERRARRAY_NGROUPSPERINDEX 8

//T should be a builtin numeric type with sizeof(T) > sizeof(short)

template <class T>
class FlexErrArray {
public:

  //extraAllocSize indicates the size of the memory immediately following the object which will be used for supergroups indices.
  static unsigned extraAllocSize(unsigned nbins) { return (small(nbins) ? nGroupsNeeded(nbins) : nSuperGroups(nbins))*sizeof(void*); }
  static bool small(unsigned nbins) { return nbins<FLEXERRARRAYGROUP_MAXBINS*FLEX1DERRARRAY_NGROUPSPERINDEX; }
  bool small() const { return small(m_nbins); }

  FlexErrArray( unsigned nbins );
  ~FlexErrArray();

  unsigned getNBins() const;

  void fill(unsigned bin);
  void fill(unsigned bin, const double& weight);
  T getBinContent(unsigned bin) const;
  double getBinError(unsigned bin) const;
  void getBinContentAndError(unsigned bin, T& content, double& error ) const;

  void setBinContent(unsigned bin, const T& content);
  void setBinError(unsigned bin, const double& error );
  void setBinContentAndError(unsigned bin, const T& content, const double& error );

  bool holdsSeparateSumW2Info() const;
  void copyContents(T*cont, double*err=0) const;//Won't copy errors if err==0.

  double Integral() const;

  //For fast looping through bins, automatically skipping bins where
  //both content and error are 0:
  void resetActiveBinLoop();
  bool getNextActiveBin(unsigned& bin, T& content, double& error);

  void scaleContentsAndErrors( const double& fact );

#ifdef LW_STRICT_ROOT_BEHAVIOUR
  void forcePretendSumWMode() { m_pretendSumWMode = true; }
  bool pretendSumWMode() const { return m_pretendSumWMode; }
#endif

private:

  FlexErrArray( const FlexErrArray & );
  FlexErrArray & operator= ( const FlexErrArray & );

  //If we are "small()" (i.e. <=1024 bins), we just keep an array of
  //the group pointers. If larger than that, we keep arrays of arrays
  //of groups (super-groups).

  //Data members:
  const FlexErrArrayGroup<T> *** superGroups() const
  {
    assert(!small());
    return reinterpret_cast<const FlexErrArrayGroup<T> ***>(const_cast<void*>(m_indices));
  }
  FlexErrArrayGroup<T> *** superGroups()
  {
    assert(!small());
    return reinterpret_cast<FlexErrArrayGroup<T> ***>(m_indices);
  }
  const FlexErrArrayGroup<T> ** groups() const
  {
    assert(small());
    return reinterpret_cast<const FlexErrArrayGroup<T> **>(const_cast<void*>(m_indices));
  }
  FlexErrArrayGroup<T> ** groups()
  {
    assert(small());
    return reinterpret_cast<FlexErrArrayGroup<T> **>(m_indices);
  }

  void* m_indices;
  FlexErrArrayGroup<T> * m_fastloop_group2check;
  const unsigned m_nbins;
  unsigned m_fastloop_isuper2check;
  unsigned m_fastloop_igr2check;
#ifdef LW_STRICT_ROOT_BEHAVIOUR
  bool m_pretendSumWMode;
#endif

  void fastLoop_findAndResetNextGroup( );

  unsigned iSuperGroup(unsigned bin) const { assert(bin<m_nbins); return bin/(FLEXERRARRAYGROUP_MAXBINS*FLEX1DERRARRAY_NGROUPSPERINDEX); }
  unsigned superGroupBin(unsigned bin) const { assert(bin<m_nbins); return (bin/FLEXERRARRAYGROUP_MAXBINS)%FLEX1DERRARRAY_NGROUPSPERINDEX; }
  //  unsigned superGroupBin(unsigned bin) const { assert(bin<m_nbins); return bin%(FLEXERRARRAYGROUP_MAXBINS*FLEX1DERRARRAY_NGROUPSPERINDEX); }
  unsigned groupBin(unsigned bin) const { assert(bin<m_nbins); return bin%FLEXERRARRAYGROUP_MAXBINS; }
  unsigned iGroup(unsigned bin) const { assert(bin<m_nbins); return bin/FLEXERRARRAYGROUP_MAXBINS; }
  //This example should show how to use m_supergroups to use the individual groups:
  //   m_supergroups[iSuperGroup(bin)][superGroupBin(bin)]->fill(groupBin(bin))

  const FlexErrArrayGroup<T> * getGroupNoAlloc(unsigned bin) const
  {
    assert(bin<m_nbins);
    if (small()) {
      assert(iGroup(bin)<nGroupsNeeded(m_nbins));
      return groups()[iGroup(bin)];
    } else {
      assert(iSuperGroup(bin)<nSuperGroups(m_nbins));
      const FlexErrArrayGroup<T> ** supergroup = superGroups()[iSuperGroup(bin)];
      return supergroup ? supergroup[superGroupBin(bin)] : 0;
    }
  }
  FlexErrArrayGroup<T> * getGroup(unsigned bin);

  static unsigned nGroupsNeeded(unsigned nbins) { return nbins/FLEXERRARRAYGROUP_MAXBINS+(nbins%FLEXERRARRAYGROUP_MAXBINS?1:0); }
  static unsigned nSuperGroups(unsigned nbins) { unsigned u=nGroupsNeeded(nbins); return u/FLEX1DERRARRAY_NGROUPSPERINDEX+(u%FLEX1DERRARRAY_NGROUPSPERINDEX?1:0); }
  //NB: The last index array is allocated/released shorter than FLEX1DERRARRAY_NGROUPSPERINDEX

   unsigned entriesInSuperGroup(unsigned iSuperGroup) const {
     unsigned l(nSuperGroups(m_nbins));
     if (iSuperGroup+1<l) {
       return FLEX1DERRARRAY_NGROUPSPERINDEX;
     } else {
       //Fixme: nGroupsNeeded calculated twice!
       unsigned n(nGroupsNeeded(m_nbins)%FLEX1DERRARRAY_NGROUPSPERINDEX);
       return n ? n : FLEX1DERRARRAY_NGROUPSPERINDEX;
     }
   }
};

#include "FlexErrArray.icc"

#endif
