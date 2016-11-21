# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#############################################
# Heavy flavour from tt tools
#############################################
from DerivationFrameworkCore.DerivationFrameworkMaster import *
DSIDList=[
  410000,
  410001,
  410002,
  410003,
  410004,
  410006,
  410007,
  410008,
  410009,
  410051,
  410120,
  410121,
  410159,
  410160,
  410186,
  410187,
  410188,
  410189,
  410232,
  410233,
  410500,
  301528,
  301529,
  301530,
  301531,
  301532,
  303722,
  303723,
  303724,
  303725,
  303726,
  407009,
  407010,
  407011,
  407012,
  410120,
  426090,
  426091,
  426092,
  426093,
  426094,
  426095,
  426096,
  426097,
  429007,
]

import PyUtils.AthFile as af
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
f = af.fopen(athenaCommonFlags.PoolAODInput()[0])
if len(f.mc_channel_number) > 0:
  if(int(f.mc_channel_number[0]) in DSIDList):
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HadronOriginClassifier
    DFCommonhadronorigintool = DerivationFramework__HadronOriginClassifier(name="DFCommonHadronOriginClassifier",DSID=int(f.mc_channel_number[0]))
    ToolSvc += DFCommonhadronorigintool
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HadronOriginDecorator
    DFCommonhadronorigindecorator = DerivationFramework__HadronOriginDecorator(name = "DFCommonHadronOriginDecorator")
    DFCommonhadronorigindecorator.ToolName = DFCommonhadronorigintool
    ToolSvc += DFCommonhadronorigindecorator
    DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("HFHadronsCommonKernel",
                                                                                 AugmentationTools = [DFCommonhadronorigindecorator]
                                                                            ) 
