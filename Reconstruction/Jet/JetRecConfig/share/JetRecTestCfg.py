from JetRecConfig import JetRecConfig
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from pprint import pprint, pformat

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger("testJetRecDeps")

def JetRecTestCfg(configFlags,args):

    jetlist = configFlags.JetRec.Definitions
    if args.printDependencies:
        for jetdef in jetlist:
            deps = JetRecConfig.resolveDependencies(jetdef)
            jetlog.info("Dumping dependency dict for {0}".format(jetdef))
            depstr = pformat(deps)
            jetlog.info(depstr)

    if args.printAccumulators:
        jetlog.info("Printing component accumulators for each jet collection")
    jetcas = []
    for jetdef in jetlist:
       jetcomps = JetRecConfig.JetRecCfg(jetdef,configFlags,jetnameprefix=configFlags.JetRec.Prefix)
       if args.printAccumulators:
           jetcomps.printConfig(withDetails=args.verboseAccumulators,summariseProps=True)
       jetcas.append(jetcomps)

    jetlog.info("Printing component accumulator for entire sequence")
    components = ComponentAccumulator()
    for ca in jetcas:
        components.merge(ca)
    components.printConfig(args.verboseAccumulators,summariseProps=True)

    return components

def DefineJetCollections():

    # Here we define the jet configurations we want to build
    # These mod and ghost lists should go in a common module
    standardrecomods = ["Width","TrackMoments","TrackSumMoments","JVF","JVT","OriginSetPV",
                        "CaloEnergies","LArHVCorr"]
    clustermods = ["ECPSFrac","ClusterMoments"]
    truthmods = ["PartonTruthLabel","TruthPartonDR","JetDeltaRLabel:5000"]

    from JetRecConfig.JetDefinition import JetGhost
    ghostlist = ["Track","MuonSegment","Truth"]
    standardghosts = [JetGhost(ghosttype) for ghosttype in ghostlist]
    flavourghostlist = ["BHadronsInitial", "BHadronsFinal", "BQuarksFinal",
                        "CHadronsInitial", "CHadronsFinal", "CQuarksFinal",
                        "TausFinal",
                        "WBosons", "ZBosons", "HBosons", "TQuarksFinal",
                        "Partons",
                        ]
    flavourghosts = [JetGhost("TruthLabel"+ghosttype) for ghosttype in flavourghostlist]
    standardghosts += flavourghosts

    ########################################################################
    # First a demonstration of just building jets using standard definitions
    from JetRecConfig.StandardJetDefs import AntiKt4LCTopo, AntiKt4EMPFlow, AntiKt4Truth, AntiKt4TruthWZ

    # This updates the original jet definitions, so might be a little risky
    # in derivation code. Safer would be to always deepcopy into a local variable.
    AntiKt4LCTopo.ptminfilter = 15e3
    AntiKt4LCTopo.modifiers = ["Calib:T0:mc","Sort"] + standardrecomods + clustermods + truthmods
    AntiKt4LCTopo.ghostdefs = standardghosts
    #AntiKt4LCTopo.modifiers = ["Calib:AnalysisLatest:mc"]

    AntiKt4EMPFlow.ptminfilter = 10e3
    AntiKt4EMPFlow.modifiers = ["Calib:T0:mc","Sort"] + standardrecomods + truthmods
    AntiKt4EMPFlow.ghostdefs = standardghosts
    #AntiKt4EMPFlow.modifiers = ["Calib:AnalysisLatest:mc"]

    from copy import deepcopy
    AntiKt4Truth.ptminfilter = 2e3

    AntiKt4TruthWZ.ptminfilter = 2e3

    ########################################################################
    # Now we define our own definitions
    from JetRecConfig.JetDefinition import JetConstit, JetDefinition, xAODType
    LCTopoCSSK = JetConstit(xAODType.CaloCluster, ["LC","Origin","CS","SK"])
    AntiKt4LCTopoCSSK = JetDefinition("AntiKt",0.4,LCTopoCSSK,ptmin=2e3,ptminfilter=2e3)
    AntiKt4LCTopoCSSK.modifiers = ["ConstitFourMom"] + standardrecomods + clustermods + truthmods
    AntiKt4LCTopoCSSK.ghostdefs = standardghosts

    ########################################################################
    # We can also copy and modify the standard ones
    from copy import deepcopy
    from JetRecConfig.StandardJetDefs import CHSPFlow

    CSSKPFlow = deepcopy(CHSPFlow)
    CSSKPFlow.modifiers = ["CS","SK"]
    AntiKt4EMPFlowCSSK = deepcopy(AntiKt4EMPFlow)
    AntiKt4EMPFlowCSSK.inputdef = CSSKPFlow
    AntiKt4EMPFlowCSSK.modifiers = ["ConstitFourMom"] + standardrecomods + truthmods
    AntiKt4EMPFlowCSSK.ptmin = 2e3
    AntiKt4EMPFlowCSSK.ptminfilter = 2e3
    AntiKt4EMPFlowCSSK.ghostdefs = standardghosts

    jetdefs = [#AntiKt4Truth,
               #AntiKt4TruthWZ,
               AntiKt4LCTopo,
               AntiKt4EMPFlow,
               AntiKt4LCTopoCSSK,
               AntiKt4EMPFlowCSSK]
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
    ConfigFlags.Input.isMC  = True
    ConfigFlags.Input.Files = args.filesIn.split(",")

    # Flags relating to multithreaded execution
    ConfigFlags.Concurrency.NumThreads = args.nThreads
    if args.nThreads>0:
        ConfigFlags.Scheduler.ShowDataDeps = True
        ConfigFlags.Scheduler.ShowDataFlow = True
        ConfigFlags.Scheduler.ShowControlFlow = True
        ConfigFlags.Concurrency.NumConcurrentEvents = args.nThreads

    ########################################################################
    # Define flags steering the jet reco config
    jetdefs = DefineJetCollections()
    ConfigFlags.addFlag("JetRec.Definitions",jetdefs)
    ConfigFlags.addFlag("JetRec.Prefix","New")

    # Prevent the flags from being modified
    ConfigFlags.lock()

    # Get a ComponentAccumulator setting up the fundamental Athena job
    from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg 
    cfg=MainServicesThreadedCfg(ConfigFlags) 

    # Add the components for reading in pool files
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    # Add the components from our jet reconstruction job
    cfg.merge(JetRecTestCfg(ConfigFlags,args))

    # # build eventinfo attribute list
    # from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import EventInfoAttListTool, EventInfoTagBuilder
    # EventInfoTagBuilder = EventInfoTagBuilder(AttributeList="SimpleTag")
    # cfg.addEventAlgo(EventInfoTagBuilder,"AthAlgSeq")
    
    # Write what we produced to AOD
    # First define the output list
    outputlist = ["EventInfo#*"]
    originaljets = ["AntiKt4EMPFlowJets"]
    for jetcoll in originaljets:
        outputlist += ["xAOD::JetContainer#"+jetcoll,
                       "xAOD::JetAuxContainer#"+jetcoll+"Aux."]
    for jetdef in ConfigFlags.JetRec.Definitions:
        key = "{0}{1}Jets".format(ConfigFlags.JetRec.Prefix,jetdef.basename)
        outputlist += ["xAOD::JetContainer#"+key,
                       "xAOD::JetAuxContainer#"+key+"Aux."]

    # Now get the output stream components
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(ConfigFlags,"xAOD",ItemList=outputlist))
    pprint( cfg.getEventAlgo("OutputStreamxAOD").ItemList )
  
    # Optionally, print the contents of the store every event
    cfg.getService("StoreGateSvc").Dump = args.dumpSG

    # Save this configuration to a pickle file
    f=open("JetRecTest.pkl","w")
    cfg.store(f)
    f.close()

    # Run the job
    cfg.run(maxEvents=args.nEvents)
