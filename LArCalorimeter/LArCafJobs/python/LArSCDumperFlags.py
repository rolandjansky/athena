# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def addSCDumpFlags(flags):
    
    flags.Input.isMC=False

    flags.addFlag("LArSCDump.digitsKey","SC")
    flags.addFlag("LArSCDump.nSamples",32)
    flags.addFlag("LArSCDump.nEt",3)
    flags.addFlag("LArSCDump.doSamples",True)
    flags.addFlag("LArSCDump.doSamplesBas",False)
    flags.addFlag("LArSCDump.doEt",False)
    flags.addFlag("LArSCDump.doEtId",False)
    flags.addFlag("LArSCDump.doHeader",True)
    flags.addFlag("LArSCDump.doBCID",True)
    flags.addFlag("LArSCDump.doHash",False)
    flags.addFlag("LArSCDump.doOfflineId",False)
    flags.addFlag("LArSCDump.doCalib",False)
    flags.addFlag("LArSCDump.doGeom",False)
    flags.addFlag("LArSCDump.doBC",False)
    flags.addFlag("LArSCDump.expandId",False)
    flags.addFlag("LArSCDump.overwriteEvN",False)
    flags.addFlag("LArSCDump.outputNtup","SCDigits.root")
    flags.addFlag("LArSCDump.doRawChan",False)
