/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityDoubleHitsTool.h            #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#                                                   #
//# Last Modified: Feb 7/2005                         #
//#####################################################

#ifndef TBREC_TBBEAMQUALITYDOUBLEHITSTOOL_H
#define TBREC_TBBEAMQUALITYDOUBLEHITSTOOL_H

#include "TBBeamQualityTool.h"

#include <string>
#include <vector>

class TBBeamQualityDoubleHitsTool : public TBBeamQualityTool
{
 public:
  TBBeamQualityDoubleHitsTool(const std::string& type,
			      const std::string& name,
			      const IInterface* parent);
  
  virtual ~TBBeamQualityDoubleHitsTool();
  
  virtual StatusCode initializeTool() override;
  
  virtual StatusCode accept(const std::vector<std::string>&) override;
  
 protected:
  
  std::string m_SGScintkey;
  
  //Properties
  std::vector<std::string> m_scint_names; 

  bool m_h6cuts; 
  
  int m_ScintCut1;
  int m_ScintCut2;
  
  float m_ScintCut1ADC;
  float m_ScintCut2ADC;
  
  float m_Scint1ADC;
  float m_Scint2ADC;

};

#endif
