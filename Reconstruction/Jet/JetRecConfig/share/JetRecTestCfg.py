#!/usr/bin/env python
from JetRecConfig import JetRecConfig
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from pprint import pprint, pformat

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger("testJetRecDeps")

def DefineJetCollections(configFlags):

    ########################################################################
    # import the standard definitions 
    from JetRecConfig.StandardSmallRJets import  AntiKt4EMPFlow, AntiKt4EMTopo, AntiKt4Truth, AntiKt4TruthWZ

    # Example for defining a custom definition
    from JetRecConfig.JetDefinition import JetConstitSeq, JetDefinition, xAODType
    from JetRecConfig.StandardSmallRJets import standardrecomods, clustermods, truthmods, standardghosts
    EMTopoCSSK = JetConstitSeq("EMTopoOriginCSSK", xAODType.CaloCluster, ["EM","Origin","CS","SK"], "CaloCalTopoClusters", "EMOriginTopoCSSK", label="EMTopoOriginCSSK")
    AntiKt4EMTopoCSSK = JetDefinition("AntiKt",0.4,EMTopoCSSK,ptmin=2e3,prefix="New")
    recomods = tuple( mod for mod in standardrecomods if not mod.startswith("Calib") ) # remove calib modifier : no calib exists for EMTopoCSSK
    AntiKt4EMTopoCSSK.modifiers   = ["Filter:2000","ConstitFourMom"]
    AntiKt4EMTopoCSSK.modifiers  += recomods + clustermods + truthmods
    AntiKt4EMTopoCSSK.ghostdefs   = standardghosts 
    AntiKt4EMTopoCSSK.extrainputs = ["EventDensity"]

    ########################################################################
    # We can also clone and modify an standard definition
    from JetRecConfig.StandardJetConstits import jetconstitdic
    AntiKt4EMPFlowCSSK = AntiKt4EMPFlow.clone(prefix="New")
    AntiKt4EMPFlowCSSK.inputdef    = jetconstitdic["EMPFlowCSSK"]
    AntiKt4EMPFlowCSSK.modifiers   = ["Filter:2000","ConstitFourMom"]
    AntiKt4EMPFlowCSSK.extrainputs = ["EventDensity"]

    jetdefs = [
        # we re-clone the standard with a prefix to avoid conflicts with pre-existing in-file collections
        AntiKt4EMTopo.clone(prefix="New"),
        AntiKt4EMPFlow.clone(prefix="New"),
        AntiKt4EMTopoCSSK,
        AntiKt4EMPFlowCSSK,
    ]
    if configFlags.Input.isMC:
        jetdefs += [AntiKt4Truth.clone(prefix="New"),
                    AntiKt4TruthWZ.clone(prefix="New"),]
    
    return jetdefs

if __name__=="__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser(prog="JetRecTestCfg: An example configuration module for jet reconstruction",
                            usage="Call with an input file, pass -n=0 to skip execution, -t 0 for serial or 1 for threaded execution.")
    #
    parser.add_argument("-H", "--Help", default=False, action="store_true", help="Evidently pyROOT interferes with help :(")
    #
    parser.add_argument("-f", "--filesIn", type=str, help="Comma-separated list of input files")
    parser.add_argument("-M", "--msgLvl",   default="INFO", help="The message verbosity")
    parser.add_argument("-n", "--nEvents",  default=0, type=int, help="The number of events to run. 0 skips execution")
    #
    parser.add_argument("-t", "--nThreads", default=1, type=int, help="The number of concurrent threads to run. 0 uses serial Athena.")
    #
    parser.add_argument("-D", "--dumpSG",   default=False, action="store_true", help="Toggle StoreGate dump on each event")
    parser.add_argument("-d", "--printDependencies",   default=False, action="store_true", help="Print the dependency dicts for each jet collection")
    parser.add_argument("-a", "--printAccumulators",   default=False, action="store_true", help="Print the component accumulators for each jet collection separately")
    parser.add_argument("-V", "--verboseAccumulators", default=False, action="store_true", help="Print full details of the AlgSequence for each accumulator")
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

    ########################################################################
    # Define flags steering the jet reco config
    jetdefs = DefineJetCollections(ConfigFlags)

    # Add the components from our jet reconstruction job
    from JetRecConfig.JetRecConfig import JetRecCfg
    for jetdef in jetdefs:
        comp = JetRecCfg(jetdef,ConfigFlags)
        comp.printConfig(withDetails=args.verboseAccumulators,summariseProps=True)
        
        cfg.merge(comp)

    # Write what we produced to AOD
    # First define the output list
    outputlist = ["EventInfo#*"]
    originaljets = ["AntiKt4EMPFlowJets","AntiKt4EMTopoJets"]
    for jetcoll in originaljets:
        outputlist += ["xAOD::JetContainer#"+jetcoll,
                       "xAOD::JetAuxContainer#"+jetcoll+"Aux.-PseudoJet"]
    for jetdef in jetdefs:
        key = "{0}{1}Jets".format("New",jetdef.basename)
        outputlist += ["xAOD::JetContainer#"+key,
                       "xAOD::JetAuxContainer#"+key+"Aux.-PseudoJet"]

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
