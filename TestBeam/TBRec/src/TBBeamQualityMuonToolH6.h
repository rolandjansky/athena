/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#ifndef TBREC_TBBEAMQUALITYMUONTOOLH6_H
#define TBREC_TBBEAMQUALITYMUONTOOLH6_H

#include "TBBeamQualityTool.h"

#include <string>
#include <vector>

class TBBeamQualityMuonToolH6 : public TBBeamQualityTool
{
 public:
  TBBeamQualityMuonToolH6(const std::string& type,
			  const std::string& name,
			  const IInterface* parent);
  
  virtual ~TBBeamQualityMuonToolH6();
  
  virtual StatusCode initializeTool() override;
  
  virtual StatusCode accept(const std::vector<std::string>&) override;
  
 protected:
  
  // Keys
  std::string m_SGTrigkey;
  
  // TRIGGERING
  std::vector<unsigned int> m_triggflag;
  
  bool m_mu1;
  bool m_mu2;
  bool m_mu3;
  bool m_mu4;
  bool m_successflag;
};
#endif

