/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EventShapeEvent/EventEtDensity.h"

#include "EventShapeEvent/StaticConstants.h"

bool EventEtDensity::copyData(const std::vector<double>& lparm,
			      size_t                     lindx)
{
  // check consistency
  static size_t nn((size_t)MAXINDEX);
  if ( (lindx+1) * nn > lparm.size() )
    {
      this->clear();
      return false;
    }

  // pick up 
  rhoValue     = lparm[lindx+(size_t)RHOVALUE];
  rhoSigma     = lparm[lindx+(size_t)RHOSIGMA];
  areaValue    = lparm[lindx+(size_t)AREAVALUE];
  etaRangeLow  = lparm[lindx+(size_t)ETARANGELOW];
  etaRangeHigh = lparm[lindx+(size_t)ETARANGEHIGH];
  return true;
}

bool EventEtDensity::extractData(std::vector<double>& lparm,size_t lindx) const
{
  static size_t nn((size_t)MAXINDEX);
  if ( nn*(lindx+1) > lparm.size() ) lparm.resize(nn*(lindx+1));
  lparm[lindx+(size_t)RHOVALUE]     = rhoValue;
  lparm[lindx+(size_t)RHOSIGMA]     = rhoSigma;
  lparm[lindx+(size_t)AREAVALUE]    = areaValue;
  lparm[lindx+(size_t)ETARANGELOW]  = etaRangeLow;
  lparm[lindx+(size_t)ETARANGEHIGH] = etaRangeHigh;
  return true;
}

void EventEtDensity::clear()
{
  double invVal(EventShapeConstants::invalidValue());
  rhoValue     = invVal;
  rhoSigma     = invVal;
  areaValue    = invVal;
  etaRangeLow  = invVal;
  etaRangeHigh = invVal;
}

void EventEtDensity::reset()
{
  double nullVal(EventShapeConstants::nullValue());
  rhoValue     = nullVal;
  rhoSigma     = nullVal;
  areaValue    = nullVal;
  etaRangeLow  = nullVal;
  etaRangeHigh = nullVal;
}
