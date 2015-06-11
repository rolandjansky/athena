/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGL2RESIDUALCALCULATOR_H__
#define __ITRIGL2RESIDUALCALCULATOR_H__ 

#include "GaudiKernel/IAlgTool.h"
#include <vector>

class TrigInDetTrack;
namespace Trk {
  class Track;
}
class TrigL2HitResidual;

static const InterfaceID IID_ITrigL2ResidualCalculator("ITrigL2ResidualCalculator",1,0);


  /** @class ITrigL2ResidualCalculator

      provides the abstract interface for the LVL2 ID tool that calculates smoothed residuals
      of Pixel and SCT hits

      @author D.Emeliyanov <http://consult.cern.ch/xwho>
  */

  class ITrigL2ResidualCalculator : virtual public IAlgTool { 

  public:
    /** other standard AlgTool methods */

    static const InterfaceID& interfaceID ()   //!< the Tool's interface
      {  return IID_ITrigL2ResidualCalculator; }  	

    virtual StatusCode getResiduals(const TrigInDetTrack*, std::vector<TrigL2HitResidual>&) = 0;
    virtual StatusCode getUnbiassedResiduals(const TrigInDetTrack*, std::vector<TrigL2HitResidual>&) = 0;
    virtual StatusCode getUnbiassedResiduals(const Trk::Track&, std::vector<TrigL2HitResidual>&) = 0;
  };

#endif
