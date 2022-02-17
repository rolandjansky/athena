/* emacs: this is -*- c++ -*- */
/**
 **     @file    AnalysisConfig_Tier0.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:37 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysisExample_AnalysisConfig_Tier0_H
#define TrigInDetAnalysisExample_AnalysisConfig_Tier0_H

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrigInDetAnalysisExample/T_AnalysisConfig_Tier0.h"
#include "TrigInDetAnalysisExample/T_AnalysisConfigMT_Tier0.h"
#include "TrigInDetAnalysisExample/T_AnalysisConfigR3_Tier0.h"

typedef T_AnalysisConfig_Tier0<IHLTMonTool>                      AnalysisConfig_Tier0;
typedef T_AnalysisConfigMT_Tier0<IHLTMonTool>                    AnalysisConfigMT_Tier0;
typedef T_AnalysisConfigR3_Tier0<AthAlgorithm,AnalysisR3_Tier0>  AnalysisConfigR3_Tier0;
  
#endif  // TrigInDetAnalysisExample_AnalysisConfig_Tier0_H

