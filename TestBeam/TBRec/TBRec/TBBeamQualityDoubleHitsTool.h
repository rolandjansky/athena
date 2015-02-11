/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#ifndef TBBEAMQUALITYDOUBLEHITSTOOL_H
#define TBBEAMQUALITYDOUBLEHITSTOOL_H

#include "TBRec/TBBeamQualityTool.h"

#include <string>
#include <vector>

class TBBeamQualityDoubleHitsTool : public TBBeamQualityTool
{
 public:
  TBBeamQualityDoubleHitsTool(const std::string& type,
			      const std::string& name,
			      const IInterface* parent);
  
  virtual ~TBBeamQualityDoubleHitsTool();
  
  virtual StatusCode initializeTool();
  
  virtual StatusCode accept(std::vector<std::string>);
  
 protected:
  
  StoreGateSvc* m_StoreGate;
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
