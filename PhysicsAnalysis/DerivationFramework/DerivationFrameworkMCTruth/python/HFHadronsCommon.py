# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#############################################
# Heavy flavour from tt tools
#############################################
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob

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
  410163,
  410186,
  410187,
  410188,
  410189,
  410225,
  410226,
  410227,
  410232,
  410233,
  410249,
  410250,
  410251,
  410252,
  410342,
  410343,
  410344,
  410345,
  410346,
  410347,
  410350,
  410351,
  410352,
  410353,
  410354,
  410355,
  410357,
  410358,
  410359,
  410361,
  410362,
  410363,
  410364,
  410365,
  410366,
  410367,
  410274,
  410275,
  410500,
  410501,
  410502,
  410503,
  410504,
  410505,
  410506,
  410507,
  410508,
  410511,
  410512,
  410513,
  410514,
  410515,
  410516,
  410517,
  410518,
  410519,
  410520,
  410521,
  410522,
  410523,
  410524,
  410525,
  410526,
  410527,
  410528,
  410529,
  410530,
  410531,
  410532,
  410533,
  410534,
  410535,
  410536,
  410537,
  410568,
  410569,
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
  407029,
  407030,
  407031,
  407032,
  407033,
  407034,
  407035,
  407036,
  407037,
  407038,
  407039,
  407040,
  407342,
  407343,
  407344,
  407345,
  407346,
  407347,
  407354,
  407355,
  407356,
  407348,
  407349,
  407350,
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
  410244,
  410245,
  410323,
  410324,
  410325,
  410369,
  410281,
  410282,
  410283,
  364345,
  364346,
  364347,
  364348,
  410424,
  410425,
  410426,
  410427,
  410441,
  410442,
  410470,
  410471,
  410472,
  410480,
  410481,
  410482,
  410452,
  410557,
  410558,
  410559,
  410464,
  410465,
  410466,
  411073,
  411074,
  411075,
  411076,
  411077,
  411078,
  411079,
  411080,
  411081,
  411082,
  411083,
  411084,
  411085,
  411086,
  411087,
  411088,
  411089,
  411090,
  411068,
  411069,
  411070,
  411178,
  411179,
  411180,
  411275,
  410661,
  410662,
  410663,
  410664,
  410265,
  410266,
  410267,
  412066,
  412067,
  412068,
  412069,
  412070,
  412071,
  412072,
  412073,
  412074,
  411233,
  411234,
  411329,
  411330,
  411331,
  411332,
  411333,
  411334,
  411335,
  411336,
  411337,
  412116,
  412117,
  421152,
  421153,
  421154,
  421156,
  421157,
  421158,
  700051,
  700052,
  700053,
  700054,
  ]

import PyUtils.AthFile as af
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

# Peek at the file -- this depends on what kind of file we have
from RecExConfig.ObjKeyStore import objKeyStore
if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
    f = af.fopen(athenaCommonFlags.FilesInput()[0])
elif objKeyStore.isInInput( "McEventCollection", "TruthEvent"):
    f = af.fopen(athenaCommonFlags.FilesInput()[0])        
else:
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
