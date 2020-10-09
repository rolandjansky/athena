# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from JetRecConfig.StandardSmallRJets import AntiKt4EMPFlow, AntiKt4LCTopo
from JetRecConfig.JetRecConfig import JetRecCfg, jetlog

jetlog.setLevel(2)

from pprint import pprint, pformat

if __name__=="__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser(prog="StandardTests: runs standard jet reconstruction from an ESD",
                            usage="Call with an input file, pass -n=0 to skip execution, -t 0 for serial or 1 for threaded execution.")
    #
    parser.add_argument("-H", "--Help", default=False, action="store_true", help="Evidently pyROOT interferes with help :(")
    #
    parser.add_argument("-f", "--filesIn", type=str, help="Comma-separated list of input files")
    parser.add_argument("-M", "--msgLvl",   default="INFO", help="The message verbosity")
    parser.add_argument("-n", "--nEvents",  default=0, type=int, help="The number of events to run. 0 skips execution")
    #
    parser.add_argument("-t", "--nThreads", default=1, type=int, help="The number of concurrent threads to run. 0 uses serial Athena.")
    parser.add_argument("-D", "--dumpSG",   default=False, action="store_true", help="Toggle StoreGate dump on each event")
    #
    args = parser.parse_args()

    if args.Help:
        parser.print_help()
        import sys
        sys.exit(0)

    # Setting needed for the ComponentAccumulator to do its thing
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True

    # Set message levels
    from AthenaCommon import Constants
    msgLvl = getattr(Constants,args.msgLvl)
    from AthenaCommon.Logging import log
    log.setLevel(msgLvl)
    
    # Config flags steer the job at various levels
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = args.filesIn.split(",")

    # Flags relating to multithreaded execution
    ConfigFlags.Concurrency.NumThreads = args.nThreads
    if args.nThreads>0:
        ConfigFlags.Scheduler.ShowDataDeps = True
        ConfigFlags.Scheduler.ShowDataFlow = True
        ConfigFlags.Scheduler.ShowControlFlow = True
        ConfigFlags.Concurrency.NumConcurrentEvents = args.nThreads

    # Prevent the flags from being modified
    ConfigFlags.lock()

    

    # Get a ComponentAccumulator setting up the fundamental Athena job
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    cfg=MainServicesCfg(ConfigFlags) 

    # Add the components for reading in pool files
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    # Nowadays the jet calibration tool requires the EventInfo
    # to be decorated with lumi info, which is not in Run 2 AODs
    from LumiBlockComps.LuminosityCondAlgConfig import LuminosityCondAlgCfg
    cfg.merge(LuminosityCondAlgCfg(ConfigFlags))

    from AthenaConfiguration.ComponentFactory import CompFactory
    muWriter = CompFactory.LumiBlockMuWriter("LumiBlockMuWriter",LumiDataKey="LuminosityCondData")
    cfg.addEventAlgo(muWriter,"AthAlgSeq")


    
    # Add the components from our jet reconstruction job
    jetdefs = [AntiKt4EMPFlow.clone(prefix="New")  , AntiKt4LCTopo.clone(prefix="New") ]
    for jetdef in jetdefs:
        cfg.merge( JetRecCfg(jetdef,ConfigFlags) )        

    # Write what we produced to AOD
    # First define the output list
    outputlist = ["EventInfo#*"]
    originaljets = ["AntiKt4EMPFlowJets","AntiKt4EMTopoJets"]
    for jetcoll in originaljets:
        outputlist += ["xAOD::JetContainer#"+jetcoll,
                       "xAOD::JetAuxContainer#"+jetcoll+"Aux."]
    for jetdef in jetdefs:
        key = jetdef.fullname()
        outputlist += ["xAOD::JetContainer#"+key,
                       "xAOD::JetAuxContainer#"+key+"Aux."]

    # Now get the output stream components
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(ConfigFlags,"xAOD",ItemList=outputlist))
    pprint( cfg.getEventAlgo("OutputStreamxAOD").ItemList )
  
    # Optionally, print the contents of the store every event
    cfg.getService("StoreGateSvc").Dump = args.dumpSG

    # Save this configuration to a pickle file
    f=open("JetRecTest.pkl","wb")
    cfg.store(f)
    f.close()

    # Run the job
    cfg.run(maxEvents=args.nEvents)
