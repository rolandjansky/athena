# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# JetCommon.py
# Schedules all tools needed for jet/MET analyses and writes the
# results into SG. These may then be accessed along the train  
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from AthenaCommon.GlobalFlags  import globalflags

from AthenaCommon import Logging
dfjetlog = Logging.logging.getLogger('JetCommon')

##################################################################
#       Schedule common content for all jet-using derivations
##################################################################

from JetRec.JetRecFlags import jetFlags 
jetFlags.eventShapeTools=[]
from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence(DerivationFrameworkJob,eventShapeTools=None)

DFJetAlgs = {}

##################################################################
# Schedule the augmentation of a flag to label events with large
# EMEC-IW Noise based on the presence of many bad quality clusters
##################################################################
if hasattr(DerivationFrameworkJob,"BadBatmanAugmentation"):
    dfjetlog.warning( "BadBatmanAugmentation: BadBatmanAugmentation already scheduled on sequence "+DerivationFrameworkJob.name )
else:
    # Check if we have clusters.  If we don't then this cannot run
    from RecExConfig.ObjKeyStore import objKeyStore
    if objKeyStore.isInInput( "xAOD::CaloClusterContainer", "CaloCalTopoClusters" ):
        # schedule it
        batmanaug = CfgMgr.DerivationFramework__CommonAugmentation("BadBatmanAugmentation")
        DerivationFrameworkJob += batmanaug
        batmanaugtool = None
        from AthenaCommon.AppMgr import ToolSvc        
        # create and add the tool to the alg if needed
        if hasattr(ToolSvc,"BadBatmanAugmentationTool"):
            batmanaugtool = getattr(ToolSvc,"BadBatmanAugmentationTool")
        else:
            batmanaugtool = CfgMgr.DerivationFramework__BadBatmanAugmentationTool("BadBatmanAugmentationTool")
            ToolSvc += batmanaugtool
        if not batmanaugtool in batmanaug.AugmentationTools:
            batmanaug.AugmentationTools.append(batmanaugtool)
    else:
        dfjetlog.warning('Could not schedule BadBatmanAugmentation (fine if running on EVNT)')

######################

##################################################################
#                  Definitions of helper functions 
##################################################################

def defineEDAlg(R=0.4, inputtype="LCTopo"):
    from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg
    from AthenaCommon.AppMgr import ToolSvc

    from JetRec.JetRecStandard import jtm

    # map a getter to the input argument
    inputgetter = { "LCTopo" : jtm.lcget,
                    "EMTopo" : jtm.emget,
                    "EMPFlow": jtm.empflowget,
                    "EMCPFlow": jtm.emcpflowget,
                    }[inputtype]

    t=configEventDensityTool("EDTool"+str(int(R*10))+inputtype, inputgetter, R)
    t.OutputLevel = 3
    ToolSvc += t
    return EventDensityAlg( "EventDensityAlg"+t.name(), EventDensityTool = t , OutputLevel=3)

##################################################################

def moveEDAlg(seq):
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    edalg = topSequence.EventEtDensityTester # get the old density calculation alg
    topSequence.remove(edalg)                # remove it from topsequence
    seq += edalg          # readd it to DerivationFrameworkJob)))

##################################################################

# add truth ghost association
def addGhostAssociation(DerivationFrameworkJob):

    from JetRec.JetRecStandard import jtm
    from JetRec.JetRecConf import PseudoJetGetter

    flavorgetters1 = []
    for ptype in jetFlags.truthFlavorTags():
       flavorgetters1.append(getattr(jtm, "gtruthget_"+ptype))
    jtm.gettersMap["lctopo"] += flavorgetters1
    jtm.gettersMap["emtopo"] += flavorgetters1
    jtm.gettersMap["empflow"] += flavorgetters1
    jtm.gettersMap["emcpflow"] += flavorgetters1
    jtm.gettersMap["track"] += flavorgetters1
    jtm.gettersMap["ztrack"] += flavorgetters1
    jtm.gettersMap["pv0track"] += flavorgetters1
    jtm.gettersMap["tcc"] += flavorgetters1

##################################################################

def reCreatePseudoJets(jetalg, rsize, inputtype, variableRMassScale=-1.0, variableRMinRadius=-1.0):
    """Return a list of tools (possibly empty) to be run in a jetalg. These tools will make sure PseudoJets will be associated
    to the container specified by the input arguments.    
    """
    
    from JetRec.JetRecStandard import jtm
    from JetRec.JetRecUtils import buildJetContName
    jetContName = buildJetContName(jetalg, rsize, inputtype, variableRMassScale, variableRMinRadius)

    # Set default for the arguments to be passd to addJetFinder
    finderArgs = dict( modifiersin= [], consumers = [], ghostArea= 0.01 , ptmin=40000, )
    
    # We do things differently if the container already exists in the input
    from RecExConfig.AutoConfiguration import IsInInputFile
    if IsInInputFile("xAOD::JetContainer",jetContName): # yes !

        # make sure we don't already have what we need
        tmpName = "tmp_"+jetContName
        if tmpName  in jtm.tools:
            return [jtm.tools[tmpName]]
        
        # then we'll have to build a temporary container to re-create the pseudojet
        # and we recopy this pseudojets to the original collection. This done through
        # this tool :
        from JetRec.JetRecConf import JetPseudojetCopier        
        jtm += JetPseudojetCopier( "PJcopierTo"+jetContName ,
                                   DestinationContainer = jetContName,
                                   JetPseudojetRetriever =jtm.jpjretriever )
        # prepare args for this case :
        finderArgs['consumers'] = [jtm.tools["PJcopierTo"+jetContName] ]
        finderArgs['ptmin'] = 20000
        
    else: # no preexisting container
        # make sure we don't already have what we need
        tmpName = jetContName
        if tmpName  in jtm.tools:
#            return []
            return [jtm.tools[tmpName]]

        # no container exist. simply build a new one.
        if inputtype=="LCTopo" or inputtype=="EMTopo" or inputtype == "EMPFlow" or inputtype == "EMCPFlow":
            defaultmods = {"EMTopo":"emtopo_ungroomed",
                           "LCTopo":"lctopo_ungroomed",
                           "EMPFlow":"pflow_ungroomed",
                           "EMCPFlow":"pflow_ungroomed",
                           "Truth":"truth_ungroomed",
                           "TruthWZ":"truth_ungroomed",
                           "PV0Track":"track_ungroomed",
                           "TrackCaloCluster":"tcc_ungroomed"}
            finderArgs['modifiersin'] = defaultmods[inputtype]
            finderArgs['ptmin'] = 2000
            finderArgs['ptminFilter'] = 50000
            finderArgs['calibOpt'] = "none"
        elif inputtype=="PV0Track":
            finderArgs['modifiersin'] = None
            finderArgs['ptmin'] = 2000
            finderArgs['ptminFilter'] = 40000
            finderArgs['calibOpt'] = "none"
        #if not "PFlow" in inputtype: finderArgs.pop('modifiersin') # leave the default modifiers.

    if (variableRMassScale>0):  
        finderArgs['variableRMassScale']=variableRMassScale
        finderArgs['variableRMinRadius']=variableRMinRadius
        #finderArgs['ghostArea'] =0  ## Cannot afford ghost area calculation for variable-R jets (for now)
    
    # map the input to the jtm code for PseudoJetGetter
    getterMap = dict( LCTopo = 'lctopo', EMTopo = 'emtopo', EMPFlow = 'empflow',             EMCPFlow = 'emcpflow', 
                      Truth='truth',     TruthWZ='truthwz', TruthDressedWZ='truthdressedwz', PV0Track='pv0track', 
                      TrackCaloCluster='tcc')

    # create the finder for the temporary collection.
    tmpFinderTool= jtm.addJetFinder(tmpName, jetalg, rsize, getterMap[inputtype] ,
                                    **finderArgs   # pass the prepared arguments
                                    )
    return [tmpFinderTool]

def buildGenericGroomAlg(jetalg, rsize, inputtype, groomedName, jetToolBuilder,
                         includePreTools=False, algseq=None, outputGroup="CustomJets",
                         writeUngroomed=False, variableRMassScale=-1.0, variableRMinRadius=-1.0):
    algname = "jetalg"+groomedName[:-4]

    from RecExConfig.AutoConfiguration import IsInInputFile
    if algseq is None:
        dfjetlog.info( "No algsequence passed! Will not schedule "+algname )
        return
    elif IsInInputFile("xAOD::JetContainer",groomedName):
        dfjetlog.info( "Collection "+groomedName+" is already in input AOD!" )
        return

    from JetRec.JetRecUtils import buildJetContName
    ungroomedName = buildJetContName(jetalg, rsize, inputtype, variableRMassScale, variableRMinRadius)
    ungroomedalgname = "jetalg"+ungroomedName[:-4] # Remove "Jets" from name

    # add these groomed jets to the output (use setdefault() to constuct the list if not existing yet)
    OutputJets.setdefault(outputGroup , [] ).append(groomedName)
    if writeUngroomed:
        OutputJets.setdefault(outputGroup , [] ).append(ungroomedName)
        dfjetlog.info( "Write "+ungroomedName )

    from JetRec.JetRecConf import JetAlgorithm
    # Scheduling the parent jet algorithm
    # return if the alg is already scheduled here :
    if hasattr(algseq,ungroomedalgname):
        dfjetlog.warning( "Algsequence "+algseq.name()+" already has an instance of "+ungroomedalgname )
    elif ungroomedalgname in DFJetAlgs:
        dfjetlog.info( "Added jet finder "+ ungroomedalgname+" to sequence"+ algseq.name() )
        algseq += DFJetAlgs[ungroomedalgname]
    else:
        # 1. make sure we have pseudo-jet in our original container
        # this returns a list of the needed tools to do so.
        jetalgTools = reCreatePseudoJets(jetalg, rsize, inputtype, variableRMassScale, variableRMinRadius)
        if includePreTools and jetFlags.useTracks() and not "Truth" in inputtype:
            # enable track ghost association and JVF
            jetalgTools =  [jtm.tracksel, jtm.tvassoc] + jetalgTools 

        finderalg = JetAlgorithm(ungroomedalgname, Tools = jetalgTools )
        DFJetAlgs[ungroomedalgname] = finderalg;
        dfjetlog.info( "Added jet finder "+ungroomedalgname+" to sequence "+algseq.name() )
        algseq += finderalg

    # Scheduling the groomed jet algorithm
    # return if the alg is already scheduled here :
    if hasattr(algseq,algname):
        dfjetlog.warning( "Algsequence "+algseq.name()+" already has an instance of "+algname )
    elif algname in DFJetAlgs:
        dfjetlog.info( "Added jet groomed "+ algname+" to sequence"+ algseq.name() )
        algseq += DFJetAlgs[algname]
    else:

        # 2nd step run the trimming alg. We can re-use the original largeR jet since we reassociated the PseudoJet already.
        fatjet_groom = jetToolBuilder(groomedName, ungroomedName)

        groomeralg = JetAlgorithm(algname, Tools = [fatjet_groom])
        DFJetAlgs[algname] = groomeralg;
        algseq += groomeralg

        dfjetlog.info( "Added jet groomer "+algname+" to sequence "+algseq.name() )
    return DFJetAlgs[algname]

##################################################################
def addTrimmedJets(jetalg, rsize, inputtype, rclus=0.3, ptfrac=0.05, mods="groomed",
                   includePreTools=False, algseq=None, outputGroup="Trimmed",
                   writeUngroomed=False, variableRMassScale=-1.0, variableRMinRadius=-1.0):
    from JetRec.JetRecUtils import buildJetContName
    from JetRec.JetRecUtils import buildJetAlgName
    trimmedName = "{0}{1}TrimmedPtFrac{2}SmallR{3}Jets".format(buildJetAlgName(jetalg, rsize, variableRMassScale, variableRMinRadius),inputtype,int(ptfrac*100),int(rclus*100))

    # a function dedicated to build Trimmed jet :
    def trimToolBuilder( name, inputJetCont):
        from JetRec.JetRecStandard import jtm
        if name in jtm.tools: return jtm.tools[name]
        else: return jtm.addJetTrimmer( name, rclus=rclus, ptfrac=ptfrac, input=inputJetCont, modifiersin=mods )

    dfjetlog.info( "Configuring trimmed jets : "+ trimmedName )
    # pass the trimmedName and our specific trimming tool builder to the generic function :
    return buildGenericGroomAlg(jetalg, rsize, inputtype, trimmedName, trimToolBuilder,
                                includePreTools, algseq, outputGroup,
                                writeUngroomed=writeUngroomed,
                                variableRMassScale=variableRMassScale, variableRMinRadius=variableRMinRadius)


##################################################################
def addPrunedJets(jetalg, rsize, inputtype, rcut=0.50, zcut=0.15, mods="groomed",
                  includePreTools=False, algseq=None, outputGroup="Pruned",
                  writeUngroomed=False):
    prunedName = "{0}{1}{2}PrunedR{3}Z{4}Jets".format(jetalg,str(int(rsize*10)),inputtype,int(rcut*100),int(zcut*100))

    # a function dedicated to build Pruned jet :
    def pruneToolBuilder( name, inputJetCont):
        from JetRec.JetRecStandard import jtm
        if name in jtm.tools: return jtm.tools[name]
        else: return jtm.addJetPruner( name, rcut=rcut, zcut=zcut , input=inputJetCont, modifiersin=mods )

    dfjetlog.info( "Configuring pruned jets :  "+prunedName )
    # pass the trimmedName and our specific trimming tool builder to the generic function :
    return buildGenericGroomAlg(jetalg, rsize, inputtype, prunedName, pruneToolBuilder,
                                includePreTools, algseq, outputGroup,
                                writeUngroomed=writeUngroomed)


##################################################################
def addFilteredJets(jetalg, rsize, inputtype, mumax=1.0, ymin=0.15, mods="groomed",
                    includePreTools=False, algseq=None, outputGroup="Filtered",
                    writeUngroomed=False):
    filteredName = "{0}{1}{2}BDRSFilteredMU{3}Y{4}Jets".format(jetalg,int(rsize*10),inputtype,int(mumax*100),int(ymin*100))

    # a function dedicated to build Filtered jet :
    def filterToolBuilder( name, inputJetCont):
        from JetRec.JetRecStandard import jtm
        if name in jtm.tools: return jtm.tools[name]
        else: return jtm.addJetSplitter( name, mumax=mumax, ymin=ymin,  input=inputJetCont, modifiersin=mods )

    dfjetlog.info( "Configuring filtered jets :  "+filteredName )
    # pass the trimmedName and our specific trimming tool builder to the generic function :
    return buildGenericGroomAlg(jetalg, rsize, inputtype, filteredName, filterToolBuilder,
                                includePreTools, algseq, outputGroup,
                                writeUngroomed=writeUngroomed)


##################################################################

def addStandardJets(jetalg, rsize, inputtype, ptmin=0., ptminFilter=0.,
                    mods="default", calibOpt="none", ghostArea=0.01,
                    algseq=None, namesuffix="",
                    outputGroup="CustomJets", customGetters=None):
    jetnamebase = "{0}{1}{2}{3}".format(jetalg,int(rsize*10),inputtype,namesuffix)
    jetname = jetnamebase+"Jets"
    algname = "jetalg"+jetnamebase
    OutputJets.setdefault(outputGroup , [] ).append(jetname)

    # return if the alg is already scheduled here :
    from RecExConfig.AutoConfiguration import IsInInputFile
    if algseq is None:
        dfjetlog.warning( "No algsequence passed! Will not schedule "+algname )
        return
    elif IsInInputFile("xAOD::JetContainer",jetname):
        dfjetlog.warning( "Collection  "+jetname+" is already in input AOD!" )
        return        
    elif algname in DFJetAlgs:
        if hasattr(algseq,algname):
            dfjetlog.warning( "Algsequence "+algseq.name()+" already has an instance of "+algname )
        else:
            dfjetlog.info( "Added "+algname+" to sequence "+algseq.name() )
            algseq += DFJetAlgs[algname]
        return DFJetAlgs[algname]

    from JetRec.JetRecStandard import jtm
    if not jetname in jtm.tools:
        # no container exist. simply build a new one.
        # Set default for the arguments to be passd to addJetFinder
        defaultmods = {"EMTopo":"emtopo_ungroomed",
                       "LCTopo":"lctopo_ungroomed",
                       "EMPFlow":"pflow_ungroomed",
                       "EMCPFlow":"pflow_ungroomed",
                       "Truth":"truth_ungroomed",
                       "TruthWZ":"truth_ungroomed",
                       "PV0Track":"track_ungroomed",
                       "TrackCaloCluster":"tcc_ungroomed",
                       }
        if mods=="default":
            mods = defaultmods[inputtype] if inputtype in defaultmods else []
        finderArgs = dict( modifiersin= mods, consumers = [])
        finderArgs['ptmin'] = ptmin
        finderArgs['ptminFilter'] = ptminFilter
        finderArgs['ghostArea'] = ghostArea
        finderArgs['modifiersin'] = mods
        finderArgs['calibOpt'] = calibOpt
        print "mods in:", finderArgs['modifiersin']
        #finderArgs.pop('modifiersin') # leave the default modifiers.
    
        # map the input to the jtm code for PseudoJetGetter
        getterMap = dict( LCTopo = 'lctopo', EMTopo = 'emtopo', EMPFlow = 'empflow', EMCPFlow = 'emcpflow',
                          Truth = 'truth',  TruthWZ = 'truthwz', TruthDressedWZ = 'truthdressedwz', 
                          PV0Track = 'pv0track', TrackCaloCluster = 'tcc' )

        # set input pseudojet getter -- allows for custom getters
        if customGetters is None:
            inGetter = getterMap[inputtype]
        else:
            inGetter = customGetters
            
        # create the finder for the temporary collection
        finderTool = jtm.addJetFinder(jetname, jetalg, rsize, inGetter,
                                      **finderArgs   # pass the prepared arguments
                                      )

        from JetRec.JetRecConf import JetAlgorithm
        alg = JetAlgorithm(algname, Tools = [finderTool])
        dfjetlog.info( "Added "+algname+" to sequence "+algseq.name() )
        algseq += alg
        DFJetAlgs[algname] = alg;

##################################################################
# Schedule the augmentation of a flag to label events with large
# EMEC-IW Noise based on the presence of many bad quality clusters
##################################################################
def addBadBatmanFlag(sequence=DerivationFrameworkJob):
    # simple set up -- either the alg exists and contains the tool, in which case we exit
    if hasattr(sequence,"BadBatmanAugmentation"):
        print "BadBatmanAugmentation: BadBatmanAugmentation already scheduled on sequence", sequence.name
        return
    else:
        # otherwise schedule it
        batmanaug = CfgMgr.DerivationFramework__CommonAugmentation("BadBatmanAugmentation")
        sequence += batmanaug

        batmanaugtool = None
        from AthenaCommon.AppMgr import ToolSvc        
        # create and add the tool to the alg if needed
        if hasattr(ToolSvc,"BadBatmanAugmentationTool"):
            batmanaugtool = getattr(ToolSvc,"BadBatmanAugmentationTool")
        else:
            batmanaugtool = CfgMgr.DerivationFramework__BadBatmanAugmentationTool("BadBatmanAugmentationTool")
            ToolSvc += batmanaugtool
        if not batmanaugtool in batmanaug.AugmentationTools:
            batmanaug.AugmentationTools.append(batmanaugtool)

##################################################################

##################################################################
#       Set up helpers for adding jets to the output streams
##################################################################
# Below we build an explicit list of all jet containers.
# replacing occurances of
# "xAOD::JetContainer_v1#*"
# "xAOD::JetAuxContainer_v1#*"

OutputJets = {}
OutputJets["SmallR"] = [
    "AntiKt2PV0TrackJets",
    "AntiKt4EMTopoJets",
    "AntiKt4LCTopoJets",
    "AntiKt4PV0TrackJets",
    "AntiKt4TruthJets",
#    "AntiKt4TruthWZJets",
    "AntiKt4EMPFlowJets",
    ]

OutputJets["LargeR"] = [
    "AntiKt10LCTopoJets",
    "AntiKt10TrackCaloClusterJets",
    "AntiKt10TruthJets",
    "AntiKt10TruthWZJets",
    ]

Tier0Jets = [
    "AntiKt4EMPFlowJets",
    "AntiKt4EMTopoJets",
    "AntiKt4LCTopoJets",
    ]

def addJetOutputs(slimhelper,contentlist,smartlist=[],vetolist=[]):
    outputlist = []
    for content in contentlist:
        if content in OutputJets.keys():
            for item in OutputJets[content]:
                if item in vetolist: continue
                outputlist.append(item)
        else:
            outputlist.append(content)

    for item in outputlist:
        if not slimhelper.AppendToDictionary.has_key(item):
            slimhelper.AppendToDictionary[item]='xAOD::JetContainer'
            slimhelper.AppendToDictionary[item+"Aux"]='xAOD::JetAuxContainer'
        if item in smartlist:
            dfjetlog.info( "Add smart jet collection "+item )
            slimhelper.SmartCollections.append(item)
        else:
            dfjetlog.info( "Add full jet collection "+item )
            slimhelper.AllVariables.append(item)
