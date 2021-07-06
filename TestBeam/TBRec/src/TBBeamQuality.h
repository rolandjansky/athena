/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQuality.h                          #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#          based on TBEventStreamer                 #
//# Last Modified: Nov 18/2004                        #
//#####################################################

#ifndef TBREC_TBBEAMQUALITY_H
#define TBREC_TBBEAMQUALITY_H

/* The TBBeamQuality algorithm takes a particle type as input and returns a success or failurs based on different tests. The different tests are tools which run until a failure occurs. */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class TBBeamQualityTool;

#include <string>
#include <map>
#include <vector>

class TBBeamQuality : public AthAlgorithm
{
 public:
  TBBeamQuality(const std::string& name, ISvcLocator* pService);
  
  virtual ~TBBeamQuality();
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
 protected:
  /* Properties*/
  std::vector<std::string> m_bqparticle;  //particles to select
//  std::vector<std::string> m_bqtoolNames;
  
  ToolHandleArray<TBBeamQualityTool> m_bqtools;
  
  // output maps
  std::map<ToolHandle<TBBeamQualityTool>, unsigned int> m_bqacceptCounter;
  std::map<ToolHandle<TBBeamQualityTool>, unsigned int> m_bqrejectCounter;
  std::map<ToolHandle<TBBeamQualityTool>, unsigned int> m_bqtotalCounter;
};
#endif
