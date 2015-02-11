/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityTrackingTool.h              #
//# Package: TBRec                                    #
//#                                                   #
//# Author : P.Krieger <krieger@physics.utoronto.ca>  #
//#                                                   #
//# Last Modified: Jun 30/2005                        #
//##################################################### 

#ifndef TBBEAMQUALITYTRACKINGTOOL_H
#define TBBEAMQUALITYTRACKINGTOOL_H

#include "TBRec/TBBeamQualityTool.h"

#include <string>
#include <vector>

class TBBeamQualityTrackingTool : public TBBeamQualityTool
{
 public:
  TBBeamQualityTrackingTool(const std::string& type,
			    const std::string& name,
			    const IInterface* parent);
  
  virtual ~TBBeamQualityTrackingTool();
  
  virtual StatusCode initializeTool();
  
  virtual StatusCode accept(std::vector<std::string>);
  
 protected:
  
  StoreGateSvc* m_StoreGate;
  
  // Keys
  std::string m_SGTrigkey;
  
  // TRIGGERING
  std::vector<unsigned int> m_triggflag;
  
  float  m_chi2cut_u;
  float  m_chi2cut_v;
};
#endif

