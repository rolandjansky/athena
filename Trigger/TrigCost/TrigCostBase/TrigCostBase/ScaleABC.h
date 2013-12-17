/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_SCALEABC_H
#define ANP_SCALEABC_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : ScaleABC
 * @Author : Rustem Ospanov
 *
 * @Brief  : Interface class for accessing event weights
 * 
 **********************************************************************************/

// Local
#include "TrigCostBase/Handle.h"

namespace Anp
{
  namespace Scale {
    
    struct ScaleBGInfo {
      ScaleBGInfo () : scale_lumi(1.0), scale_rd0(1.0), scale_rd1(1.0) {}
      
      long double scale_lumi;
      long double scale_rd0;
      long double scale_rd1;
    };
  }
  
  class ScaleABC: public virtual Base {
  public:
    ScaleABC() {}
    virtual ~ScaleABC() {}
    
    virtual long double GetScaleCorr()   const = 0;
    virtual long double GetScalePileup() const = 0;
    virtual Scale::ScaleBGInfo GetScaleBCID() const { return fScaleBG; };
    virtual long double GetPredLumi()    const = 0;
    virtual std::vector<int> GetPredBunchGroupNum() const {return fBGtmp; };

  protected:

    Scale::ScaleBGInfo    fScaleBG; // dummy for declaration
    std::vector<int>      fBGtmp;

  };
}

#endif
