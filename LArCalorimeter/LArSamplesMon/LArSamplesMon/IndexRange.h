/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::IndexRange
   @brief storage of the time histories of all the cells
 */

#ifndef LArSamples_IndexRange_H
#define LArSamples_IndexRange_H

#include "TString.h"
#include "TVectorD.h"

namespace LArSamples {
  
  class IndexRange
  {
    public:

      virtual ~IndexRange() { }
      
      virtual int lwb() const = 0;
      virtual int upb() const = 0;

      bool isInRange(int i) const { return (i >= lwb() && i <= upb()); }

      bool hasSameRange(int lw, int up) const { return (lwb() == lw && upb() == up); }
      bool hasSameRange(const IndexRange& other) const { return hasSameRange(other.lwb(), other.upb()); }
      bool hasSameRange(const TVectorD& v) const { return hasSameRange(v.GetLwb(), v.GetUpb()); }

      bool providesRange(int lw, int up) const { return (lwb() <= lw && upb() >= up); }
      bool providesRange(const IndexRange& other) const { return providesRange(other.lwb(), other.upb()); }
      bool providesRange(const TVectorD& v) const { return providesRange(v.GetLwb(), v.GetUpb()); }

      TString rangeStr() const { return Form("[%d, %d]", lwb(), upb()); }
      
      int commonLwb(const IndexRange& other) const { return (other.lwb() > lwb() ? other.lwb() : lwb()); }
      int commonUpb(const IndexRange& other) const { return (other.upb() < upb() ? other.upb() : upb()); }
      
      bool checkRange(int& l, int& h) const;
  };
}
  
#endif
