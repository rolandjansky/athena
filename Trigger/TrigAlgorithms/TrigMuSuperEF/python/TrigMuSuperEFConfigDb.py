# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm,addTool,addService

# RoI based
addAlgorithm("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF","TrigMuSuperEF")
#addAlgorithm("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_MGfirst","TrigMuSuperEF_MGfirst")
addAlgorithm("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_TMEFfirst","TrigMuSuperEF_TMEFfirst")
#addAlgorithm("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_MGonly","TrigMuSuperEF_MGonly")
addAlgorithm("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_TMEFonly","TrigMuSuperEF_TMEFonly")
#addAlgorithm("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_both","TrigMuSuperEF_both")
addAlgorithm("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_SAonly","TrigMuSuperEF_SAonly")
# combiner only
addAlgorithm("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_TMEFCombinerOnly","TrigMuSuperEF_TMEFCombinerOnly")
# full-scans
addAlgorithm("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_FSCB","TrigMuSuperEF_FSCB")
addAlgorithm("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_FSSA","TrigMuSuperEF_FSSA")
addAlgorithm("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_WideCone","TrigMuSuperEF_WideCone")
addAlgorithm("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_WideCone05","TrigMuSuperEF_WideCone05")
