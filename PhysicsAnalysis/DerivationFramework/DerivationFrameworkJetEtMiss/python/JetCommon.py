# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# JetCommon.py
# Schedules all tools needed for jet/MET analyses and writes the
# results into SG. These may then be accessed along the train  
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from JetRec.JetRecFlags import jetFlags 
from AthenaCommon.GlobalFlags  import globalflags

##################################################################
#       Schedule common content for all jet-using derivations
##################################################################

jetFlags.eventShapeTools=[]
from JetRec.JetRecStandard import jtm
from RecExConfig.ObjKeyStore import cfgKeyStore
if not cfgKeyStore.isInInput("xAOD::JetContainer","AntiKt2PV0TrackJets"):
    jtm.addJetFinder("AntiKt2PV0TrackJets", "AntiKt", 0.2, "pv0track", ptmin= 2000)
from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence(DerivationFrameworkJob,eventShapeTools=None)

DFJetAlgs = {}

##################################################################
#                  Definitions of helper functions 
##################################################################

def defineEDAlg(R=0.4, inputtype="LCTopo"):
    from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg
    from AthenaCommon.AppMgr import ToolSvc

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
    #from JetSimTools.JetSimToolsConf import JetTruthParticleSelectorTool
    from JetRec.JetRecConf import PseudoJetGetter
    #from JetSimTools.JetSimToolsConf import TruthPseudoJetGetter
    #from ParticleJetTools.ParticleJetToolsConf import Analysis__JetQuarkLabel
    # Truth flavor tags.
    # for ptype in jetFlags.truthFlavorTags():
    #     jtm += PseudoJetGetter(
    #            "gtruthget_" + ptype,
    #            InputContainer = "TruthLabel" + ptype,
    #            Label = "Ghost" + ptype,
    #            OutputContainer = "PseudoJetGhost" + ptype,
    #            SkipNegativeEnergy = True,
    #            GhostScale = 1e-20
    #           )

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

##################################################################

from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import QwTool
jtm += QwTool("qw")

jtm.modifiersMap["dfgroomed"] = jtm.modifiersMap["groomed"] + [
    jtm.nsubjettiness,
    jtm.ktdr,
    jtm.ktsplitter,
    jtm.encorr,
    jtm.charge,
    jtm.angularity,
    jtm.comshapes,
    jtm.ktmassdrop,
    jtm.dipolarity,
    jtm.pull,
    jtm.planarflow,
    jtm.width,
    jtm.qw,
    jtm.trksummoms
    #jtm.showerdec
    ]

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
    from RecExConfig.ObjKeyStore import cfgKeyStore
    if cfgKeyStore.isInInputFile("xAOD::JetContainer",jetContName): # yes !

        # make sure we don't already have what we need
        tmpName = "tmp_"+jetContName
        if tmpName  in jtm.tools:
#            return []
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
            finderArgs['modifiersin'] = "calib" if "Topo" in inputtype else "pflow"
            finderArgs['ptmin'] = 2000
            finderArgs['ptminFilter'] = 50000
            finderArgs['calibOpt'] = "none"
        elif inputtype=="PV0Track":
            finderArgs['modifiersin'] = ""
            finderArgs['ptmin'] = 2000
            finderArgs['ptminFilter'] = 40000
            finderArgs['calibOpt'] = "none"
        if not "PFlow" in inputtype: finderArgs.pop('modifiersin') # leave the default modifiers.

    if (variableRMassScale>0):  
        finderArgs['variableRMassScale']=variableRMassScale
        finderArgs['variableRMinRadius']=variableRMinRadius
        #finderArgs['ghostArea'] =0  ## Cannot afford ghost area calculation for variable-R jets (for now)
    
    # map the input to the jtm code for PseudoJetGetter
    getterMap = dict( LCTopo = 'lctopo', EMTopo = 'emtopo', EMPFlow = 'empflow', EMCPFlow = 'emcpflow', Truth='truth', TruthWZ='truthwz', PV0Track='pv0track')
    # create the finder for the temporary collection.
    tmpFinderTool= jtm.addJetFinder(tmpName, jetalg, rsize, getterMap[inputtype] ,
                                    **finderArgs   # pass the prepared arguments
                                    )
    return [tmpFinderTool]

def buildGenericGroomAlg(jetalg, rsize, inputtype, groomedName, jetToolBuilder,
                         includePreTools=False, algseq=None, outputGroup="CustomJets",
                         writeUngroomed=False, variableRMassScale=-1.0, variableRMinRadius=-1.0):
    algname = "jetalg"+groomedName

    if algseq is None:
        print "No algsequence passed! Will not schedule", algname
        return
    elif cfgKeyStore.isInInput("xAOD::JetContainer",groomedName):
        print "Collection ", algname, "is already in input AOD!"
        return

    from JetRec.JetRecUtils import buildJetContName
    ungroomedName = buildJetContName(jetalg, rsize, inputtype, variableRMassScale, variableRMinRadius)
    ungroomedalgname = "jetalg"+ungroomedName[:-4] # Remove "Jets" from name

    # add these groomed jets to the output (use setdefault() to constuct the list if not existing yet)
    OutputJets.setdefault(outputGroup , [] ).append(groomedName+"Jets")
    if writeUngroomed: OutputJets.setdefault(outputGroup , [] ).append(ungroomedName+"Jets")

    from JetRec.JetRecConf import JetAlgorithm
    # return if the alg is already scheduled here :
    if hasattr(algseq,ungroomedalgname):
        print "   Algsequence", algseq, "already has an instance of", ungroomedalgname
    elif ungroomedalgname in DFJetAlgs:
        print "   Added jet finder", ungroomedalgname, "to sequence", algseq
        algseq += DFJetAlgs[ungroomedalgname]
    else:
        # 1. make sure we have pseudo-jet in our original container
        # this returns a list of the needed tools to do so.
        jetalgTools = reCreatePseudoJets(jetalg, rsize, inputtype, variableRMassScale, variableRMinRadius)
        if includePreTools:
            # enable track ghost association and JVF
            jetalgTools =  [jtm.tracksel, jtm.tvassoc] + jetalgTools 
        print jetalgTools

        finderalg = JetAlgorithm(ungroomedalgname, Tools = jetalgTools )
        DFJetAlgs[ungroomedalgname] = finderalg;
        print "   Added jet finder", ungroomedalgname, "to sequence", algseq
        algseq += finderalg

    # 2nd step run the trimming alg. We can re-use the original largeR jet since we reassociated the PseudoJet already.
    fatjet_groom = jetToolBuilder(groomedName+"Jets", ungroomedName)

    print "   Added jet groomer", algname, "to sequence", algseq
    groomeralg = JetAlgorithm(algname, Tools = [fatjet_groom])
    DFJetAlgs[algname] = groomeralg;
    algseq += groomeralg
    return groomeralg

##################################################################
def addTrimmedJets(jetalg, rsize, inputtype, rclus=0.3, ptfrac=0.05, mods="dfgroomed",
                   includePreTools=False, algseq=None, outputGroup="Trimmed",
                   writeUngroomed=False, variableRMassScale=-1.0, variableRMinRadius=-1.0):
    from JetRec.JetRecUtils import buildJetContName
    #trimmedName = "{0}{1}{2}TrimmedPtFrac{3}SmallR{4}".format(jetalg,int(rsize*10),inputtype,int(ptfrac*100),int(rclus*100))
#    Helper for this doesn't quite exist yet
    from JetRec.JetRecUtils import buildJetAlgName
    #trimmedName = "{0}{1}{2}TrimmedPtFrac{3}SmallR{4}".format(buildJetContName(jetalg, rsize, variableRMassScale, variableRMinRadius),int(rsize*10),inputtype,int(ptfrac*100),int(rclus*100))
    trimmedName = "{0}{1}TrimmedPtFrac{2}SmallR{3}".format(buildJetAlgName(jetalg, rsize, variableRMassScale, variableRMinRadius),inputtype,int(ptfrac*100),int(rclus*100))

    # a function dedicated to build Trimmed jet :
    def trimToolBuilder( name, inputJetCont):
        from JetRec.JetRecStandard import jtm
        if name in jtm.tools: return jtm.tools[name]
        else: return jtm.addJetTrimmer( name, rclus=rclus, ptfrac=ptfrac, input=inputJetCont, modifiersin=mods )

    print "Configuring trimmed jets : ", trimmedName
    # pass the trimmedName and our specific trimming tool builder to the generic function :
    return buildGenericGroomAlg(jetalg, rsize, inputtype, trimmedName, trimToolBuilder,
                                includePreTools, algseq, outputGroup,
                                writeUngroomed=writeUngroomed,
                                variableRMassScale=variableRMassScale, variableRMinRadius=variableRMinRadius)


##################################################################
def addPrunedJets(jetalg, rsize, inputtype, rcut=0.50, zcut=0.15, mods="dfgroomed",
                  includePreTools=False, algseq=None, outputGroup="Pruned",
                  writeUngroomed=False):
    prunedName = "{0}{1}{2}PrunedR{3}Z{4}".format(jetalg,str(int(rsize*10)),inputtype,int(rcut*100),int(zcut*100))

    # a function dedicated to build Pruned jet :
    def pruneToolBuilder( name, inputJetCont):
        from JetRec.JetRecStandard import jtm
        if name in jtm.tools: return jtm.tools[name]
        else: return jtm.addJetPruner( name, rcut=rcut, zcut=zcut , input=inputJetCont, modifiersin=mods )

    print "Configuring pruned jets : ", prunedName
    # pass the trimmedName and our specific trimming tool builder to the generic function :
    return buildGenericGroomAlg(jetalg, rsize, inputtype, prunedName, pruneToolBuilder,
                                includePreTools, algseq, outputGroup,
                                writeUngroomed=writeUngroomed)


##################################################################
def addFilteredJets(jetalg, rsize, inputtype, mumax=1.0, ymin=0.15, mods="dfgroomed",
                    includePreTools=False, algseq=None, outputGroup="Filtered",
                    writeUngroomed=False):
    filteredName = "{0}{1}{2}BDRSFilteredMU{3}Y{4}".format(jetalg,int(rsize*10),inputtype,int(mumax*100),int(ymin*100))

    # a function dedicated to build Filtered jet :
    def filterToolBuilder( name, inputJetCont):
        from JetRec.JetRecStandard import jtm
        if name in jtm.tools: return jtm.tools[name]
        else: return jtm.addJetSplitter( name, mumax=mumax, ymin=ymin,  input=inputJetCont, modifiersin=mods )

    print "Configuring filtered jets : ", filteredName
    # pass the trimmedName and our specific trimming tool builder to the generic function :
    return buildGenericGroomAlg(jetalg, rsize, inputtype, filteredName, filterToolBuilder,
                                includePreTools, algseq, outputGroup,
                                writeUngroomed=writeUngroomed)


##################################################################

def addStandardJets(jetalg, rsize, inputtype, ptmin=2000, ptminFilter=5000,
                    mods="calib", calibOpt="none", ghostArea=0.01,
                    algseq=None, outputGroup="CustomJets"):
    jetnamebase = "{0}{1}{2}".format(jetalg,int(rsize*10),inputtype)
    jetname = jetnamebase+"Jets"
    algname = "jetalg"+jetnamebase
    OutputJets.setdefault(outputGroup , [] ).append(jetname)

    # return if the alg is already scheduled here :
    if algseq is None:
        print "No algsequence passed! Will not schedule", algname
        return
    elif cfgKeyStore.isInInput("xAOD::JetContainer",jetname):
        print "Collection ", algname, "is already in input AOD!"
        return        
    elif algname in DFJetAlgs:
        if hasattr(algseq,algname):
            print "   Algsequence", algseq, "already has an instance of", algname
        else:
            print "   Added", algname, "to sequence", algseq
            algseq += DFJetAlgs[algname]
        return DFJetAlgs[algname]

    if not jetname in jtm.tools:
        # Set default for the arguments to be passd to addJetFinder
        finderArgs = dict( modifiersin= [], consumers = [])
        finderArgs['ptmin'] = ptmin
        finderArgs['ptminFilter'] = ptminFilter
        finderArgs['ghostArea'] = ghostArea
        # no container exist. simply build a new one.
        if inputtype=="LCTopo" or inputtype=="EMTopo" or inputtype == "EMPFlow" or inputtype == "EMCPFlow":
            finderArgs['modifiersin'] = mods
            finderArgs['calibOpt'] = "none"
        #finderArgs.pop('modifiersin') # leave the default modifiers.
    
        # map the input to the jtm code for PseudoJetGetter
        getterMap = dict( LCTopo = 'lctopo', EMTopo = 'emtopo', EMPFlow = 'empflow', EMCPFlow = 'emcpflow', Truth='truth', TruthWZ='truthwz', PV0Track='pv0track')
        # create the finder for the temporary collection.
        finderTool= jtm.addJetFinder(jetname, jetalg, rsize, getterMap[inputtype] ,
                                     **finderArgs   # pass the prepared arguments
                                     )

        from JetRec.JetRecConf import JetAlgorithm
        alg = JetAlgorithm(algname, Tools = [finderTool])
        print "   Added", algname, "to sequence", algseq
        algseq += alg
        DFJetAlgs[algname] = alg;


##################################################################

def addPFlowJets(inputtype="EM", rsize=0.4, algseq=None, outputGroup="PFlow"):
    from JetRec.JetRecStandard import jtm
    jtm.modifiersMap["mods"] = ["jetfilter", jtm.nsubjettiness, jtm.pull ]

    pflowName = "AntiKt"+str(int(rsize*10))+inputtype+"Pflow" # input : EM, EMC, LC
    algname = "jetalg"+pflowName
    OutputJets.setdefault(outputGroup , [] ).append(pflowName)

    # return if the alg is already scheduled here :
    if algseq is None:
        print "No algsequence passed! Will not schedule", algname
        return
    elif cfgKeyStore.isInInput("xAOD::JetContainer",pflowName):
        print "Collection ", algname, "is already in input AOD!"
        return        
    elif algname in DFJetAlgs:
        if hasattr(algseq,algname):
            print "   Algsequence", algseq, "already has an instance of", algname
        else:
            print "   Added", algname, "to sequence", algseq
            algseq += DFJetAlgs[algname]
        return DFJetAlgs[algname]

    if inputtype=="EM":
        inputName = "empflow"
    elif inputtype=="EMC":
        inputName = "emcpflow"
    elif inputtype=="LC":
        inputName = "lcpflow"

    if pflowName in jtm.tools:
        pflowjet = jtm.tools[pflowName]
    else:
        pflowjet = jtm.addJetFinder(pflowName+"Jets", "AntiKt", rsize, inputName, "mods", ghostArea=0.01 , ptmin=2000, ptminFilter=7000)


    from JetRec.JetRecConf import JetAlgorithm
    print "Adding PFlow jets : ", pflowName
    alg = JetAlgorithm(algname, Tools = [pflowjet] )
    print "   Added", algname, "to sequence", algseq
    algseq += alg
    DFJetAlgs[algname] = alg;
    return alg

#if jetFlags.useTruth:
#    addGhostAssociation(DerivationFrameworkJob)

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
    "AntiKt3PV0TrackJets",
    "AntiKt4EMTopoJets",
    "AntiKt4LCTopoJets",
    "AntiKt4PV0TrackJets",
    "AntiKt4TruthJets",
#    "AntiKt4TruthWZJets",
    "AntiKt4EMPFlowJets",
    ]

OutputJets["LargeR"] = [
    "AntiKt10LCTopoJets",
    "AntiKt10TruthJets",
    "AntiKt10TruthWZJets",
    "CamKt12LCTopoJets",
    "CamKt12TruthJets",
    "CamKt12TruthWZJets",
    ]

Tier0Jets = [
    "AntiKt10LCTopoJets",
    "AntiKt10TruthJets",
    "AntiKt10TruthWZJets",
    "AntiKt2PV0TrackJets",
    "AntiKt3PV0TrackJets",
    "AntiKt4EMPFlowJets",
    "AntiKt4EMTopoJets",
    "AntiKt4LCTopoJets",
    "AntiKt4PV0TrackJets",
    "AntiKt4TruthJets",
    "AntiKt4TruthWZJets",
    "CamKt12LCTopoJets",
    "CamKt12TruthJets",
    "CamKt12TruthWZJets"
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
        if item in Tier0Jets:
            if item in smartlist:
                print "Add smart jet collection", item
                slimhelper.SmartCollections.append(item)
            else:
                print "Add full jet collection", item
                slimhelper.AllVariables.append(item)
        else:
            print "Add on-the-fly jet collection", item
            slimhelper.StaticContent.append("xAOD::JetContainer#"+item)
            slimhelper.StaticContent.append("xAOD::JetAuxContainer#"+item+"Aux.")
