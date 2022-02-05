# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys

from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude
from LArCafJobs.LArNoiseConfig import LArNoiseFromRawCfg
from AthenaConfiguration.MainServicesConfig import MainServicesCfg


def fromRunArgs(runArgs):
    from AthenaConfiguration.AllConfigFlags import ConfigFlags    

    from LArCafJobs.LArNoiseFlags import addNoiseFlags
    addNoiseFlags(ConfigFlags)

    commonRunArgsToFlags(runArgs, ConfigFlags)

    processPreInclude(runArgs, ConfigFlags)
    processPreExec(runArgs, ConfigFlags)

    ConfigFlags.Input.Files=runArgs.inputBSFile

    if hasattr(runArgs,"outputHIST_LARNOISEFile"):
       ConfigFlags.LArNoise.outHistLAr=runArgs.outputHIST_LARNOISEFile
       ConfigFlags.Output.HISTFileName =runArgs.outputHIST_LARNOISEFile

    if hasattr(runArgs,"outputNTUP_LARNOISEFile"):
       ConfigFlags.LArNoise.outNtupLAr=runArgs.outputNTUP_LARNOISEFile

    if hasattr(runArgs,"outputNTUP_HECNOISEFile"):
        ConfigFlags.LArNoise.HECNoiseNtup=runArgs.outputNTUP_HECNOISEFile

    if not hasattr(runArgs,"conditionsTag") or runArgs.conditionsTag=="CURRENT":
        print("Resolving 'CURRENT' express conditions tag ...")
        sys.path.append('/afs/cern.ch/user/a/atlcond/utils22/')
        from CondUtilsLib.AtlCoolBKLib import resolveAlias
        resolver=resolveAlias()
        currentGlobalES=resolver.getCurrentES().replace("*","ST")
        print("Found ",currentGlobalES)
        ConfigFlags.IOVDb.GlobalTag=currentGlobalES
    else:
        ConfigFlags.IOVDb.GlobalTag=runArgs.conditionsTag

    if hasattr(runArgs,"skipEvents"):
        ConfigFlags.Exec.SkipEvents=runArgs.skipEvents

    if hasattr(runArgs,"maxEvents"):
        ConfigFlags.Exec.MaxEvents=runArgs.maxEvents

    ConfigFlags.Trigger.doID=False
    ConfigFlags.Trigger.doMuon=False
    ConfigFlags.Trigger.doLVL1=False
    ConfigFlags.Trigger.doHLT=False

    ConfigFlags.Calo.Cell.doDeadCellCorr=True

    ConfigFlags.lock()
    
    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(LArNoiseFromRawCfg(ConfigFlags))

    #OFL LUMI tag not connected to ES tak, doing it here:
    cfg.getService("IOVDbSvc").overrideTags+=['<prefix>/TRIGGER/OFLLUMI/OflPrefLumi</prefix><tag>OflPrefLumi-RUN2-UPD4-12</tag>']
    processPostInclude(runArgs, ConfigFlags, cfg)
    processPostExec(runArgs, ConfigFlags, cfg)

    # Run the final accumulator
    sc = cfg.run()
    sys.exit(not sc.isSuccess())
