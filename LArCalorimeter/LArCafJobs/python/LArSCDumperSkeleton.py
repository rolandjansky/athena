# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import sys

from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude
from LArCalibProcessing.LArSC2NtupleConfig import LArSC2NtupleCfg
from AthenaConfiguration.MainServicesConfig import MainServicesCfg


def fromRunArgs(runArgs):
    from AthenaConfiguration.AllConfigFlags import ConfigFlags    

    from LArCafJobs.LArSCDumperFlags import addSCDumpFlags
    addSCDumpFlags(ConfigFlags)

    commonRunArgsToFlags(runArgs, ConfigFlags)

    processPreInclude(runArgs, ConfigFlags)
    processPreExec(runArgs, ConfigFlags)

    ConfigFlags.Input.Files=runArgs.inputBSFile
    ConfigFlags.LArSCDump.outputNtup=runArgs.outputNTUP_SCMONFile

    # this should be later deduced from SCRunLog
    ConfigFlags.LArSCDump.doEt=True
    ConfigFlags.LArSCDump.nSamples=5
    CKeys=["SC_ET"]    

    ConfigFlags.lock()
    
    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(LArSC2NtupleCfg(ConfigFlags, AddBadChannelInfo=ConfigFlags.LArSCDump.doBC, AddFEBTempInfo=False, isSC=True, isFlat=False,
                            OffId=ConfigFlags.LArSCDump.doOfflineId, AddHash=ConfigFlags.LArSCDump.doHash, AddCalib=ConfigFlags.LArSCDump.doCalib, RealGeometry=ConfigFlags.LArSCDump.doGeom, ExpandId=ConfigFlags.LArSCDump.expandId, # from LArCond2NtupleBase 
                            NSamples=ConfigFlags.LArSCDump.nSamples, FTlist={}, FillBCID=ConfigFlags.LArSCDump.doBCID, ContainerKey=ConfigFlags.LArSCDump.digitsKey,  # from LArDigits2Ntuple
                            SCContainerKeys=CKeys, OverwriteEventNumber = ConfigFlags.LArSCDump.overwriteEvN,                        # from LArSC2Ntuple
                            OutputLevel=4))

    from AthenaConfiguration.ComponentFactory import CompFactory
    cfg.addService(CompFactory.NTupleSvc(Output = [ "FILE1 DATAFILE='"+ConfigFlags.LArSCDump.outputNtup+"' OPT='NEW'" ]))
    cfg.setAppProperty("HistogramPersistency","ROOT")

    processPostInclude(runArgs, ConfigFlags, cfg)
    processPostExec(runArgs, ConfigFlags, cfg)

    # Run the final accumulator
    sc = cfg.run()
    sys.exit(not sc.isSuccess())
