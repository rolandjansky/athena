#!/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# file   testAFPDB.py
# author Petr Balek <petr.balek@cern.ch> (with a lot of inspiration from Tomasz Bold)
# date   2021-03-22

# brief  A script that will test reading from the local DB file. In order to test it:
#           0. setup athena enviroment
#           1. have the local database; either get it from somewhere else (will be done in future), or create a new one with AFPLocalAlignDBCreate.py:
#               $ python AFPLocalAlignDBCreate.py
#           2. run this script as (feel free to change the input file):
#               $ python testAFPDB.py --filesInput=/afs/cern.ch/user/p/pbalek/workspace/public/data17_13TeV.00338480.physics_Main.daq.RAW/data17_13TeV.00338480.physics_Main.daq.RAW._lb0275._SFO-7._0007.data
#           2a. for another alignment constants, try this input file (n.b.: local constants are the same, only global are different):
#               $ python testAFPDB.py --filesInput=/afs/cern.ch/user/p/pbalek/workspace/public/data17_13TeV.00333380.physics_Main.daq.RAW/data17_13TeV.00333380.physics_Main.daq.RAW._lb0163._SFO-7._0001.data
#           3. the script will read the files and print out alignment variables for the events in the input file (based on run number and LB)

#           footnote: for the python setup with TopLocRecSeq, see AFP_LocReco/AFP_LocReco_joboptions.py

from AthenaConfiguration.ComponentAccumulator import CompFactory, ComponentAccumulator

def testAFPDBCfg(flags):
    acc = ComponentAccumulator()

    from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
    from IOVDbSvc.IOVDbSvcConfig import addFolders

    # these are two randomly picked conditions, checked for comparison
    acc.merge(addFolders(flags, '/CALO/HadCalibration2/CaloEMFrac', 'CALO_ONL', className='CaloLocalHadCoeff', db='CONDBR2'))
    acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Beampos", "/Indet/Beampos", className="AthenaAttributeList"))

    # set from where to read the local information
    # this will read from DB for data
#     schema = "<db>sqlite://;schema=Example.db;dbname=CONDBR2</db>"
#     locFolder = "/FWD/Onl/AFP/Align/Local"
#     locTag = "<tag>AFPAlignLoc-01</tag>"
#     globFolder = "/FWD/Onl/AFP/Align/Global"
#     globTag = "<tag>AFPAlignGlob-01</tag>"
#     acc.merge(addFolders(flags, schema+locFolder+locTag, className='CondAttrListCollection', db='CONDBR2' ))
#     acc.merge(addFolders(flags, schema+globFolder+globTag, className='CondAttrListCollection', db='CONDBR2' ))
#     acc.addCondAlgo(CompFactory.SiAlignDBTester())
    
    # this will read from DB for MC (note that the source file is still data that provides run and LB, thus this is really for testing only)
    schema = "<db>sqlite://;schema=ExampleMC.db;dbname=OFLP200</db>"
    locFolder = "/FWD/AFP/Align/Local"
    locTag = "<tag>AFPMCAlignLoc-329484-02</tag>"
    globFolder = "/FWD/AFP/Align/Global"
    globTag = "<tag>AFPMCAlignGlob-331020-01</tag>"
    acc.merge(addFolders(flags, schema+locFolder+locTag, className='CondAttrListCollection', db='OFLP200' ))
    acc.merge(addFolders(flags, schema+globFolder+globTag, className='CondAttrListCollection', db='OFLP200' ))    
    acc.addCondAlgo(CompFactory.SiAlignDBTester("SiAlignDBTester", locshiftXkey="/FWD/AFP/Align/Local", globshiftXkey="/FWD/AFP/Align/Global"))

    return acc


if __name__ == "__main__":
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    #flags.Input.Files = [] # can hardcode file
    # if you have issues wiht data dependencies and want to debug it
    flags.Scheduler.CheckDependencies = True
    flags.Scheduler.ShowDataDeps = True
    flags.Scheduler.ShowDataFlow = True
    flags.Scheduler.ShowControlFlow = True
    flags.Scheduler.EnableVerboseViews = True
        
    # more threads and more concurent events; change both to 1 if something goes wrong
    flags.Concurrency.NumThreads = 3
    flags.Concurrency.NumConcurrentEvents = 5

    flags.Exec.MaxEvents = 500

    # AFPP align constants are not included in these (yet)
    flags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2017-16"
    flags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"

    acc = MainServicesCfg(flags)
    parser = flags.getArgumentParser()
    args = flags.fillFromArgs(parser=parser)

    flags.lock()


    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge(ByteStreamReadCfg(flags))

    acc.merge(testAFPDBCfg(flags))
    from AthenaCommon.Constants import DEBUG, VERBOSE
    acc.foreach_component("*AFP*").OutputLevel=VERBOSE
    acc.foreach_component("SiAlignDBTester").OutputLevel=DEBUG
    acc.printConfig(withDetails=True, summariseProps=True)
    acc.run()
