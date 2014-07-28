/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MUFAST_EPTESTIMATEPOL2_H
#define TRIG_MUFAST_EPTESTIMATEPOL2_H

#include "TrigmuFast/PtEstimateStd.h"
#include "TrigmuFast/ProcessBase.h"
#include <string>

class MuFeature;
class PtLUTSvc;

class EPtEstimatePol2: public PtEstimateStd
{
   public:
      EPtEstimatePol2(MuMessageInterface& msg,
		      bool tilt_field,
		      const PtLUTSvc* LUTsvc,
		      MuFeature& feature,
		      const MuLUT* mu_lut);
      virtual ~EPtEstimatePol2(void);
      
   public:
      std::string name(void) const { return "EPtEstimatePol2"; };

   public:
      virtual bool run(MuonFeatureDetails* det=0);
      
   private:
      bool m_tilt_field;
      const PtLUTSvc* pPtLUTSvc;
      static const std::string s_name;
      bool m_doDebug;
};

#endif // TRIG_MUFAST_EPTESTIMATEPOL2_H
