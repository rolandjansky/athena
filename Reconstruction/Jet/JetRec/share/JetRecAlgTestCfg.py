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
from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg 
cfg=MainServicesThreadedCfg(ConfigFlags) 

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

    # Enable dictionary for xAODType enum
    import cppyy
    cppyy.loadDictionary('xAODBaseObjectTypeDict')
    from ROOT import xAODType

    # Apply some corrections to the topoclusters
    # Example with property assignments
    jetmodseq = CompFactory.JetConstituentModSequence("JetMod_EMOrigin")
    jetmodseq.InputType=xAODType.CaloCluster
    jetmodseq.InputContainer = "CaloCalTopoClusters"
    jetmodseq.OutputContainer = "EMOriginTopoClusters"

    # Build the list of modifiers to run
    # Configuration with constructor keywords
    modlist = [
        CompFactory.ClusterAtEMScaleTool("ClusterEM",InputType=xAODType.CaloCluster),
        CompFactory.CaloClusterConstituentsOrigin("ClusterOrigin",InputType=xAODType.CaloCluster)
    ]
    jetmodseq.Modifiers = modlist

    # We need a JetAlgorithm to run the modseq, which is a tool
    jetmodalg = CompFactory.JetAlgorithm(
        "JetModAlg_EMOrigin",
        Tools = [jetmodseq])

    # Add the alg to the sequence in the ComponentAccumulator
    inputcfg.addEventAlgo(jetmodalg,sequencename)

    # Create a PseudoJetGetter & corresponding algorithm
    getter = CompFactory.PseudoJetGetter(
        "pjg_EMTopo",
        InputContainer = "EMOriginTopoClusters",
        OutputContainer = "PseudoJetEMTopo",
        Label = "EMTopo",
        SkipNegativeEnergy=True,
        GhostScale=0.)

    pjgalg = CompFactory.PseudoJetAlgorithm(
        "pjgalg_EMTopo",
        PJGetter = getter)

    # Add the alg to the sequence in the ComponentAccumulator
    inputcfg.addEventAlgo(pjgalg,sequencename)

    return inputcfg

# Return a ComponentAccumulator holding the jet building sequence
def JetBuildAlgCfg(ConfigFlags,buildjetsname):
    buildcfg = ComponentAccumulator()

    # Create a sequence that holds a set of algorithms
    # -- mainly for understanding how chunks of the job
    #    relate to each other

    sequencename = "JetBuildSeq"
    buildcfg.addSequence( CompFactory.AthSequencer(sequencename) )
    # Merge in config to get jet inputs
    buildcfg.merge(JetInputCfg(ConfigFlags))

    # Create the JetClusterer, set some standard options
    jclust = CompFactory.JetClusterer("builder")
    jclust.JetAlgorithm = "AntiKt"
    jclust.JetRadius = 0.4
    jclust.PtMin = 500 # MeV
    jclust.InputPseudoJets = "PseudoJetEMTopo"
    jclust.JetInputType = 1 # Hardcoded "magic number" for now
    # See https://gitlab.cern.ch/atlas/athena/blob/master/Event/xAOD/xAODJet/xAODJet/JetContainerInfo.h
    # This should get its own dictionary.

    # Add a simple jet modifier to the JetRecAlg
    jclsmoms = CompFactory.JetClusterMomentsTool("clsmoms")

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

# Return a ComponentAccumulator holding the jet copy sequence
def JetCopyAlgCfg(ConfigFlags,buildjetsname,copyjetsname):
    copycfg = ComponentAccumulator()

    # Create a sequence that holds a set of algorithms
    # -- mainly for understanding how chunks of the job
    #    relate to each other
    sequencename = "JetCopySeq"
    copycfg.addSequence( CompFactory.AthSequencer(sequencename) )

    # Create the JetClusterer, set some standard options
    jcopy = CompFactory.JetCopier("copier")
    jcopy.InputJets = buildjetsname

    # Add a simple jet modifier to the JetRecAlg
    jclsmoms = CompFactory.JetClusterMomentsTool("clsmoms")

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

# Add the build config to the job
# One could add options to make it more customisable
buildjetsname = "MyAntiKt4EMTopoJets"
copyjetsname = "CopyAntiKt4EMTopoJets"
cfg.merge( JetBuildAlgCfg(ConfigFlags, buildjetsname) )
cfg.merge( JetCopyAlgCfg(ConfigFlags, buildjetsname, copyjetsname) )

# Write what we produced to AOD
# First define the output list
outputlist = ["EventInfo#*"]
jetlist = ["AntiKt4EMTopoJets",buildjetsname,copyjetsname]
for jetcoll in jetlist:
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
cfg.run(maxEvents=1)
