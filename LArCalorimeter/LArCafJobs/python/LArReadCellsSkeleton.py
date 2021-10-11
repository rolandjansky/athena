# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys

from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude
from LArCafJobs.LArReadCellsConfig import LArReadCellsCfg
from AthenaConfiguration.MainServicesConfig import MainServicesCfg


def fromRunArgs(runArgs):
    from AthenaConfiguration.AllConfigFlags import ConfigFlags    

    from LArCafJobs.LArShapeDumperFlags import addShapeDumpFlags
    addShapeDumpFlags(ConfigFlags)

    commonRunArgsToFlags(runArgs, ConfigFlags)

    processPreInclude(runArgs, ConfigFlags)
    processPreExec(runArgs, ConfigFlags)

    ConfigFlags.LAr.ROD.forceIter=True
    ConfigFlags.Input.Files=runArgs.inputBSFile
    ConfigFlags.LArShapeDump.outputNtup="SPLASH"

    #protection for LArPEB event:
    ConfigFlags.Trigger.L1.doMuon=False
    ConfigFlags.Trigger.L1.doCalo=False
    ConfigFlags.Trigger.L1.doTopo=False

    ConfigFlags.lock()
    
    cfg=MainServicesCfg(ConfigFlags)
    from AthenaConfiguration.ComponentFactory import CompFactory
    cfg.addService(CompFactory.THistSvc(Output=["SPLASH DATAFILE='"+runArgs.outputNTUP_LARCELLSFile+"' OPT='RECREATE'",]))
    cfg.merge(LArReadCellsCfg(ConfigFlags))

    processPostInclude(runArgs, ConfigFlags, cfg)
    processPostExec(runArgs, ConfigFlags, cfg)

    # Run the final accumulator
    sc = cfg.run()
    sys.exit(not sc.isSuccess())
