#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


# should choose a better default ??
DEFAULT_INPUTFILE = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.14795494._005958.pool.root.1"

from argparse import ArgumentParser
parser = ArgumentParser(prog="StandardTests: runs standard jet reconstruction from an ESD",
                        usage="Call with an input file, pass -n=0 to skip execution, -t 0 for serial or 1 for threaded execution.")
#
parser.add_argument("-H", "--Help", default=False, action="store_true", help="Evidently pyROOT interferes with help :(")
#
parser.add_argument("-f", "--filesIn", type=str, help="Comma-separated list of input files",
                    default=DEFAULT_INPUTFILE)
parser.add_argument("-M", "--msgLvl",   default="INFO", help="The message verbosity")
parser.add_argument("-n", "--nEvents",  default=10, type=int, help="The number of events to run. 0 skips execution")
#
parser.add_argument("-t", "--nThreads", default=1, type=int, help="The number of concurrent threads to run. 0 uses serial Athena.")
parser.add_argument("-D", "--dumpSG",   default=False, action="store_true", help="Toggle StoreGate dump on each event")
parser.add_argument("-j", "--jetType",   default="smallR", type=str, choices={"smallR","largeR", "cssk", "VR"},
                    help="the type of jet definitions to test")

#
args = parser.parse_args()

if args.Help:
    parser.print_help()
    import sys
    sys.exit(0)



from pprint import pprint
from JetRecConfig.JetRecConfig import JetRecCfg, jetlog

# Set message levels
from AthenaCommon import Logging, Constants
msgLvl = getattr(Constants,args.msgLvl)
Logging.log.setLevel(msgLvl)
tlog = Logging.logging.getLogger('test_StandardJets')

    
# Setting needed for the ComponentAccumulator to do its thing
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=True


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


# ***********************************************
# Prepare the JetDefinition to be tested
if args.jetType=='smallR':
    from JetRecConfig.StandardSmallRJets import AntiKt4EMPFlow, AntiKt4LCTopo, AntiKt4Truth
    jetdefs0 = [AntiKt4EMPFlow  , AntiKt4LCTopo, AntiKt4Truth]
    # we add a suffix to avoid conflicts with in-file existing jets
    jetdefs =[ d.clone(prefix="New") for d in jetdefs0 ]
    alljetdefs = jetdefs0+jetdefs

elif args.jetType=='largeR':
    from JetRecConfig.StandardLargeRJets import AntiKt10LCTopoTrimmed, AntiKt10TruthTrimmed, AntiKt10LCTopoSoftDrop
    jetdefs = [ AntiKt10LCTopoTrimmed, AntiKt10TruthTrimmed , AntiKt10LCTopoSoftDrop ]
    alljetdefs = jetdefs

elif args.jetType=='cssk':
    from JetRecConfig.StandardSmallRJets import AntiKt4LCTopo,AntiKt4EMPFlow
    from JetRecConfig.StandardJetConstits import stdConstitDic as cst
    # remove the calib tool from modifiers, because no standard calib defined for the custom def below
    nocalibL = [m for m in AntiKt4LCTopo.modifiers if m!="Calib:T0:mc" ]
    AntiKt4LCTopoCSSK = AntiKt4LCTopo.clone(inputdef = cst.LCTopoCSSK, modifiers=nocalibL )
    nocalibL = [m for m in AntiKt4EMPFlow.modifiers if m!="Calib:T0:mc" ]
    AntiKt4EMPFlowCSSK = AntiKt4EMPFlow.clone(inputdef = cst.EMPFlowCSSK, modifiers=nocalibL )
    jetdefs = [AntiKt4LCTopoCSSK,AntiKt4EMPFlowCSSK]
    alljetdefs = jetdefs
elif args.jetType=='VR':
    from JetRecConfig.StandardSmallRJets import AntiKtVR30Rmax4Rmin02PV0Track
    jetdefs = [AntiKtVR30Rmax4Rmin02PV0Track]
    alljetdefs = jetdefs

# ***********************************************
if args.nEvents == 0:
    # Don't run over events --> just run the jet config.
    # Add the components from our jet reconstruction job
    for jetdef in jetdefs:
        cfg.merge( JetRecCfg(ConfigFlags,jetdef) )        
    import sys
    tlog.info("Performed jet config. Exiting now")
    sys.exit(0)


    
# ***********************************************
# else setup a full job



# Add the components for reading in pool files
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
cfg.merge(PoolReadCfg(ConfigFlags))

# Nowadays the jet calibration tool requires the EventInfo
# to be decorated with lumi info, which is not in Run 2 AODs
from LumiBlockComps.LuminosityCondAlgConfig import LuminosityCondAlgCfg
cfg.merge(LuminosityCondAlgCfg(ConfigFlags))

from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
cfg.merge(LumiBlockMuWriterCfg(ConfigFlags))



# =======================
# If running on ESD the CHSXYZParticleFlowObjects container pre-exist and can get in the way. Just rename them manually here :
if 'CHSChargedParticleFlowObjects' in ConfigFlags.Input.Collections:
    from SGComps.AddressRemappingConfig import InputRenameCfg
    cfg.merge( InputRenameCfg("xAOD::PFOContainer", "CHSNeutralParticleFlowObjects" , "CHSNeutralParticleFlowObjects_original") )
    cfg.merge( InputRenameCfg("xAOD::PFOContainer", "CHSChargedParticleFlowObjects" , "CHSChargedParticleFlowObjects_original") )


    
# Add the components from our jet reconstruction job
for jetdef in jetdefs:
    cfg.merge( JetRecCfg(ConfigFlags,jetdef) )        


# Now get the output stream components
outputlist = ["EventInfo#*"]    
# append all these jets to our output file
for j in alljetdefs:
    key = j.fullname()
    outputlist += [f"xAOD::JetContainer#{key}",
                   f"xAOD::JetAuxContainer#{key}Aux.-PseudoJet"]

from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
cfg.merge(OutputStreamCfg(ConfigFlags,"xAOD",ItemList=outputlist))
pprint( cfg.getEventAlgo("OutputStreamxAOD").ItemList )

# Optionally, print the contents of the store every event
cfg.getService("StoreGateSvc").Dump = args.dumpSG

# Run the job
cfg.run(maxEvents=args.nEvents)
