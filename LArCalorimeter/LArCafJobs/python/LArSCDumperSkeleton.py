# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import sys
import os

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

    # real geom not working yet
    ConfigFlags.LArSCDump.doGeom=False

    # this should be later deduced from SCRunLog
    ConfigFlags.LArSCDump.doEt=True
    ConfigFlags.LArSCDump.nSamples=5
    ConfigFlags.LArSCDump.nEt=1
    ConfigFlags.LArSCDump.doRawChan=True
    ConfigFlags.LArSCDump.digitsKey=""
    CKeys=["SC_ET","LArRawChannels"]    

    ConfigFlags.lock()
    
    cfg=MainServicesCfg(ConfigFlags)
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    cfg.merge(LArGMCfg(ConfigFlags))

    if ConfigFlags.LArSCDump.doBC:
       # FIXME should be SC version
       from LArBadChannelTool.LArBadChannelConfig import  LArBadFebCfg, LArBadChannelCfg
       cfg.merge(LArBadChannelCfg(ConfigFlags))
       cfg.merge(LArBadFebCfg(ConfigFlags))

    cfg.merge(LArSC2NtupleCfg(ConfigFlags, AddBadChannelInfo=ConfigFlags.LArSCDump.doBC, AddFEBTempInfo=False, isSC=True, isFlat=False,
                            OffId=ConfigFlags.LArSCDump.doOfflineId, AddHash=ConfigFlags.LArSCDump.doHash, AddCalib=ConfigFlags.LArSCDump.doCalib, RealGeometry=ConfigFlags.LArSCDump.doGeom, ExpandId=ConfigFlags.LArSCDump.expandId, # from LArCond2NtupleBase 
                            NSamples=ConfigFlags.LArSCDump.nSamples, FTlist={}, FillBCID=ConfigFlags.LArSCDump.doBCID, ContainerKey=ConfigFlags.LArSCDump.digitsKey,  # from LArDigits2Ntuple
                            SCContainerKeys=CKeys, OverwriteEventNumber = ConfigFlags.LArSCDump.overwriteEvN, Net=ConfigFlags.LArSCDump.nEt, # from LArSC2Ntuple
                            FillRODEnergy = ConfigFlags.LArSCDump.doRawChan,
                            OutputLevel=3))

    if os.path.exists(ConfigFlags.LArSCDump.outputNtup):
          os.remove(ConfigFlags.LArSCDump.outputNtup)
    from AthenaConfiguration.ComponentFactory import CompFactory
    cfg.addService(CompFactory.NTupleSvc(Output = [ "FILE1 DATAFILE='"+ConfigFlags.LArSCDump.outputNtup+"' OPT='NEW'" ]))
    cfg.setAppProperty("HistogramPersistency","ROOT")

    processPostInclude(runArgs, ConfigFlags, cfg)
    processPostExec(runArgs, ConfigFlags, cfg)

    #example how to dump the stores
    #cfg.getService("StoreGateSvc").Dump=True
    #from AthenaCommon.Constants import DEBUG
    #cfg.getService("StoreGateSvc").OutputLevel=DEBUG
    # Run the final accumulator
    sc = cfg.run()
    sys.exit(not sc.isSuccess())
