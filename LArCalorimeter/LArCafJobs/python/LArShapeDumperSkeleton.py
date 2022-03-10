# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys

from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude
from LArCafJobs.LArShapeDumperConfig import LArShapeDumperCfg
from AthenaConfiguration.MainServicesConfig import MainServicesCfg


def fromRunArgs(runArgs):
    from AthenaConfiguration.AllConfigFlags import ConfigFlags    

    from LArCafJobs.LArShapeDumperFlags import addShapeDumpFlags
    addShapeDumpFlags(ConfigFlags)

    commonRunArgsToFlags(runArgs, ConfigFlags)

    processPreInclude(runArgs, ConfigFlags)
    processPreExec(runArgs, ConfigFlags)

    ConfigFlags.LAr.ROD.forceIter=True
    ConfigFlags.LAr.OFCShapeFolder="4samples3bins17phases"
    ConfigFlags.Input.Files=runArgs.inputBSFile
    ConfigFlags.LArShapeDump.outputNtup=runArgs.outputNTUP_SAMPLESMONFile

    #protection for LArPEB event:
    ConfigFlags.Trigger.L1.doMuon=False
    ConfigFlags.Trigger.L1.doCalo=False
    ConfigFlags.Trigger.L1.doTopo=False

    if hasattr(runArgs,"outputNTUP_HECNOISEFile"):
        ConfigFlags.LArShapeDump.HECNoiseNtup=runArgs.outputNTUP_HECNOISEFile
        
    ConfigFlags.lock()
    
    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(LArShapeDumperCfg(ConfigFlags))

    processPostInclude(runArgs, ConfigFlags, cfg)
    processPostExec(runArgs, ConfigFlags, cfg)

    # Run the final accumulator
    sc = cfg.run()
    sys.exit(not sc.isSuccess())
