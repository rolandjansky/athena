/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class FlexProfileArray                    //
//                                                            //
//  Description: Container for the information needed by      //
//               the bins in profile histograms. Using float  //
//               internally, but double in all interfaces.    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2009                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef FLEXPROFILEARRAY_H
#define FLEXPROFILEARRAY_H

#include <cstring>//memset
#include <cmath>
#include <cassert>
#include <iostream>
#include "LWPools.h"

#define FLEXPROFILEARRAY_NBINSPERGROUP 8

class FlexProfileArray {
public:

  static unsigned extraAllocSize(unsigned nbins) { return nGroups(nbins)*sizeof(float*); }

  FlexProfileArray( unsigned nbins );
  ~FlexProfileArray();

  unsigned getNBins() const { return m_nbins; }

  //Filling:
  void fill(unsigned bin, const double& profiledpar);
  void fill(unsigned bin, const double& profiledpar, const double& weight);

  //Getters:
  double getBinEntries(unsigned bin) const;
  double getBinContent(unsigned bin) const;
  double getBinError(unsigned bin) const;
  void getBinInfo(unsigned bin, double& entries, double& content, double& error ) const;

  //Setters:
  void setBinEntries(unsigned bin, const double&);
  void setBinContent(unsigned bin, const double&);
  void setBinError(unsigned bin, const double&);
  void setBinInfo(unsigned bin, const double& entries, const double& content, const double& error );

  //get all contents in flat arrays:
  void copyContents(double* entries, double*contents, double*errors) const;

  //Options to change error calculations:
  enum ERROROPTION{ ERRORMEAN, ERRORSPREAD, ERRORSPREADI, ERRORSPREADG };
  void setErrorOption(ERROROPTION e) { m_errOpt=e; }
  void setErrorOptionFromString(const char *option);
  const char* getErrorOptionAsString() const;

  //Misc:
  void reset();
  double integral() const;

private:
  //Simple arrays for now:
  const unsigned m_nbins;
  ERROROPTION m_errOpt;
  float ** m_groups;
  float * getPtr(unsigned ibin);//returns f -> f[0]: entries, f[1]: contents, f[2]: error
  float * getPtrNoAlloc(unsigned ibin) const;//returns 0 or pointer as above
  static unsigned nGroups(unsigned nbins) { return nbins/FLEXPROFILEARRAY_NBINSPERGROUP + (nbins%FLEXPROFILEARRAY_NBINSPERGROUP?1:0); }
  static unsigned groupIndex(unsigned ibin) { return ibin/FLEXPROFILEARRAY_NBINSPERGROUP; }
  static unsigned indexInGroup(unsigned ibin) { return (ibin%FLEXPROFILEARRAY_NBINSPERGROUP)*3; }//3 because there are three fields per bin
  static float* allocateGroup();
  static void deallocateGroup(float*);
  unsigned nbinsInLastGroup() const
  {
    unsigned n(m_nbins%FLEXPROFILEARRAY_NBINSPERGROUP);
    return n ? n : FLEXPROFILEARRAY_NBINSPERGROUP;
  }

  //For now we keep a float for each of contents, errors, and entries,
  //and we keep those 12 bytes next to each other. This already gives
  //a factor of 2 reduction compared to ROOT (NB: internally the float
  //used for "errors" might be used for a different, more numerically
  //robust, quantity).
  //
  //We also allocate those 12 bytes in groups of X bins (X=16?), meaning that we need an index
  //array (overhead of unused groups sizeof(void*)).
  //
  //Footprint in bytes per group/per bin given X bins-per-group is:
  //
  //------|--------------------|------------------------|
  //  X   |   <unused groups>  |      <used groups>     |
  //      | 32bit     64bit    | 32bit       64bit      |
  //------|--------------------|------------------------|
  //  1   |  4/4       8/8     | 16/16       20/20      |
  //  2   |  4/2       8/4     | 28/14       32/16      |
  //  4   |  4/1       8/2     | 52/13       56/14      |
  //  8   |  4/0.5     8/1     | 100/12.5    104/13     |
  // 16   |  4/0.25    8/0.5   | 196/12.25   200/12.5   |
  // 32   |  4/0.125   8/0.25  | 388/12.125  392/12.25  |
  // 64   |  4/0.0625  8/0.125 | 772/12.0625 776/12.125 |
  //------|--------------------|------------------------|
  //
  //The optimal size is difficult to guess in advance, as it will
  //depend on how the histograms are used. But from looking at it, X=8
  //seems to be a good guess to get started.
  //
  //At a cost in cpu, this can be improved upon. For instance, entries
  //could follow the uchar->ushrt->float cycle, at a potential gain of
  //up to 25% (12bytes->9bytes, excluding pointer-to-group overhead),
  //in the case where no weighted fills are used, giving an average
  //gain of perhaps O(10%)? This is probably not worth it.

  double computeBinError(float*f) const;

};

#include "FlexProfileArray.icc"

#endif
