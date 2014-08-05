/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityMuonToolH6.cxx              #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#                                                   #
//# Last Modified: Nov 18/2004                        #
//##################################################### 

#ifndef TBBEAMQUALITYMUONTOOLH6_H
#define TBBEAMQUALITYMUONTOOLH6_H

#include "TBRec/TBBeamQualityTool.h"
#include "GaudiKernel/AlgTool.h"

#include <string>
#include <vector>

class TBBeamQualityMuonToolH6 : public TBBeamQualityTool
{
 public:
  TBBeamQualityMuonToolH6(const std::string& type,
			  const std::string& name,
			  const IInterface* parent);
  
  virtual ~TBBeamQualityMuonToolH6();
  
  virtual StatusCode initializeTool();
  
  virtual StatusCode accept(std::vector<std::string>);
  
 protected:
  
  StoreGateSvc* m_StoreGate;
  
  // Keys
  std::string m_SGTrigkey;
  
  // TRIGGERING
  std::vector<unsigned int> m_triggflag;
  
  bool mu1;
  bool mu2;
  bool mu3;
  bool mu4;
  bool successflag;
};
#endif

