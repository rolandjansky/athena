#!/usr/bin/env python

##########################################################################
# Example job running basic jet reconstruction with JetRecAlg/IJetProvider
# Author: TJ Khoo
# Date: 21.01.2020

# Setting temporarily needed for Run 3 code
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=True

# Config flags steer the job at various levels
from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.Input.isMC  = True
# Grab standard test file (annoyingly, set incorrectly in master?)
ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.AOD.e5458_s3126_r9364_r9315/AOD.11182705._000001.pool.root.1"]

# Flags relating to multithreaded execution
ConfigFlags.Concurrency.NumThreads = 1
# Dump some information about the job scheduling
ConfigFlags.Scheduler.ShowDataDeps = True
ConfigFlags.Scheduler.ShowDataFlow = True
ConfigFlags.Scheduler.ShowControlFlow = True
ConfigFlags.Concurrency.NumConcurrentEvents = 1

# Prevent the flags from being modified
ConfigFlags.lock()

########################################################################
# The ComponentAccumulator is the smallest component of an Athena job in
# the new configuration, and we plug these together with "merge"
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# Get a ComponentAccumulator setting up the fundamental Athena job
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
cfg=MainServicesCfg(ConfigFlags)

# Add the components for reading in pool files
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
cfg.merge(PoolReadCfg(ConfigFlags))

# This import is needed to get components (tools, algs)
# See https://indico.cern.ch/event/871612/contributions/3677824/attachments/1963726/3264714/UseCompFactory.pdf
from AthenaConfiguration.ComponentFactory import CompFactory

# Return a ComponentAccumulator holding the jet input sequence
def JetInputCfg(ConfigFlags):
    inputcfg = ComponentAccumulator()
    # Create a sequence that holds a set of algorithms
    # -- mainly for understanding how chunks of the job
    #    relate to each other
    sequencename = "JetInputSeq"

    inputcfg.addSequence( CompFactory.AthSequencer(sequencename) )

    from xAODBase.xAODType import xAODType

    # Apply some corrections to the topoclusters
    # Example with property assignments
    jetmodseq = CompFactory.JetConstituentModSequence("JetMod_LCOrigin")
    jetmodseq.InputType=xAODType.CaloCluster
    jetmodseq.InputContainer = "CaloCalTopoClusters"
    jetmodseq.OutputContainer = "LCOriginTopoClusters"

    # Build the list of modifiers to run
    # Configuration with constructor keywords
    modlist = [
        CompFactory.CaloClusterConstituentsOrigin("ClusterOrigin",InputType=xAODType.CaloCluster)
    ]
    jetmodseq.Modifiers = modlist

    # We need a JetAlgorithm to run the modseq, which is a tool
    jetmodalg = CompFactory.JetAlgorithm(
        "JetModAlg_LCOrigin",
        Tools = [jetmodseq])

    # Add the alg to the sequence in the ComponentAccumulator
    inputcfg.addEventAlgo(jetmodalg,sequencename)

    # Create a PseudoJetAlgorithm

    constitpjgalg = CompFactory.PseudoJetAlgorithm(
        "pjgalg_LCTopo",
        InputContainer = "LCOriginTopoClusters",
        OutputContainer = "PseudoJetLCTopo",
        Label = "LCTopo",
        SkipNegativeEnergy=True)

    ghostpjgalg = CompFactory.PseudoJetAlgorithm(
        "pjgalg_GhostTruth",
        InputContainer = "TruthParticles",
        OutputContainer = "PseudoJetGhostTruth",
        Label = "GhostTruth",
        SkipNegativeEnergy=True)

    pjcs = [constitpjgalg.OutputContainer,ghostpjgalg.OutputContainer]

    # Add the algs to the sequence in the ComponentAccumulator
    inputcfg.addEventAlgo(constitpjgalg,sequencename)
    inputcfg.addEventAlgo(ghostpjgalg,sequencename)

    return inputcfg, pjcs

# Return a ComponentAccumulator holding the jet building sequence
def JetBuildAlgCfg(ConfigFlags,buildjetsname):
    buildcfg = ComponentAccumulator()

    # Create a sequence that holds a set of algorithms
    # -- mainly for understanding how chunks of the job
    #    relate to each other

    sequencename = "JetBuildSeq"
    buildcfg.addSequence( CompFactory.AthSequencer(sequencename) )
    # Merge in config to get jet inputs
    inputcfg, pjcs = JetInputCfg(ConfigFlags)
    buildcfg.merge(inputcfg)

    # Create a merger to build the PseudoJetContainer for this specific jet collection
    mergepjalg = CompFactory.PseudoJetMerger(
        "pjmergealg_"+buildjetsname,
        InputPJContainers = pjcs,
        OutputContainer = "PseudoJetMerged_"+buildjetsname)

    buildcfg.addEventAlgo(mergepjalg)

    # Create the JetClusterer, set some standard options
    jclust = CompFactory.JetClusterer("builder")
    jclust.JetAlgorithm = "AntiKt"
    jclust.JetRadius = 1.0
    jclust.PtMin = 10e3 # MeV
    jclust.InputPseudoJets = "PseudoJetMerged_"+buildjetsname
    jclust.JetInputType = 1 # Hardcoded "magic number" for now
    # See https://gitlab.cern.ch/atlas/athena/blob/master/Event/xAOD/xAODJet/xAODJet/JetContainerInfo.h
    # This should get its own dictionary.

    # Add a simple jet modifier to the JetRecAlg
    jclsmoms = CompFactory.JetClusterMomentsTool("clsmoms",
                                                 JetContainer = buildjetsname)

    # Create the JetRecAlg, configure it to use the builder
    # using constructor syntax instead
    # (equivalent to setting properties with "=")
    jra = CompFactory.JetRecAlg(
        "JRA_build",
        Provider = jclust,       # Single ToolHandle
        Modifiers = [jclsmoms], # ToolHandleArray
        OutputContainer = buildjetsname)

    # Add the alg to the ComponentAccumulator in the named sequence
    buildcfg.addEventAlgo( jra, sequencename )
    return buildcfg

# Return a ComponentAccumulator holding the jet groom sequence
def JetGroomAlgCfg(ConfigFlags,buildjetsname,groomjetsname):
    groomcfg = ComponentAccumulator()

    # Create a sequence that holds a set of algorithms
    # -- mainly for understanding how chunks of the job
    #    relate to each other
    sequencename = "JetGroomSeq"
    groomcfg.addSequence( CompFactory.AthSequencer(sequencename) )

    # Create the JetGroomer, provide it with a JetTrimmer
    jtrim = CompFactory.JetTrimming("trimSmallR2Frac5",RClus=0.2,PtFrac=0.05)
    jtrim.UngroomedJets = buildjetsname
    jtrim.ParentPseudoJets = "PseudoJetMerged_"+buildjetsname

    # Create the JetRecAlg, configure it to use the builder
    # using constructor syntax instead
    # (equivalent to setting properties with "=")
    jra = CompFactory.JetRecAlg(
        "JRA_trim",
        Provider = jtrim,       # Single ToolHandle
        Modifiers = [], # ToolHandleArray
        OutputContainer = groomjetsname)

    # Add the alg to the ComponentAccumulator in the named sequence
    groomcfg.addEventAlgo( jra, sequencename )
    return groomcfg

# Return a ComponentAccumulator holding the jet copy sequence
def JetCopyAlgCfg(ConfigFlags,buildjetsname,copyjetsname):
    copycfg = ComponentAccumulator()

    # Create a sequence that holds a set of algorithms
    # -- mainly for understanding how chunks of the job
    #    relate to each other
    sequencename = "JetCopySeq"
    copycfg.addSequence( CompFactory.AthSequencer(sequencename) )

    # Create the JetCopier, set some standard options
    jcopy = CompFactory.JetCopier("copier")
    jcopy.InputJets = buildjetsname

    # Add a simple jet modifier to the JetRecAlg
    jclsmoms = CompFactory.JetClusterMomentsTool("clsmoms",
                                                 JetContainer = copyjetsname)

    # Create the JetRecAlg, configure it to use the copier
    # using constructor syntax instead
    # (equivalent to setting properties with "=")
    jra = CompFactory.JetRecAlg(
        "JRA_copy",
        Provider = jcopy,       # Single ToolHandle
        Modifiers = [jclsmoms], # ToolHandleArray
        OutputContainer = copyjetsname)

    # Add the alg to the ComponentAccumulator in the named sequence
    copycfg.addEventAlgo( jra, sequencename )
    return copycfg

if __name__=="__main__":
    # Add the build config to the job
    # One could add options to make it more customisable
    buildjetsname = "MyAntiKt10LCTopoJets"
    groomjetsname = "MyAntiKt10LCTopoTrimmedSmallR5Frac20Jets"
    copyjetsname = "CopyAntiKt10LCTopoJets"
    cfg.merge( JetBuildAlgCfg(ConfigFlags, buildjetsname) )
    cfg.merge( JetGroomAlgCfg(ConfigFlags, buildjetsname, groomjetsname) )
    cfg.merge( JetCopyAlgCfg(ConfigFlags, buildjetsname, copyjetsname) )

    # Write what we produced to AOD
    # First define the output list
    outputlist = ["EventInfo#*"]
    jetlist = [buildjetsname,groomjetsname,copyjetsname]
    for jetcoll in jetlist:
        if "Copy" in jetcoll:
            outputlist += ["xAOD::JetContainer#"+copyjetsname,
                           "xAOD::ShallowAuxContainer#"+copyjetsname+"Aux.-PseudoJet"]
        else:
            outputlist += ["xAOD::JetContainer#"+jetcoll,
                           "xAOD::JetAuxContainer#"+jetcoll+"Aux.-PseudoJet"]

    # Now get the output stream components
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(ConfigFlags,"xAOD",ItemList=outputlist))
    from pprint import pprint
    pprint( cfg.getEventAlgo("OutputStreamxAOD").ItemList )

    # For local tests, not in the CI
    # Print the contents of the store every event
    # cfg.getService("StoreGateSvc").Dump = True

    # Run the job
    cfg.run(maxEvents=10)
