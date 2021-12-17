# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# JetCommon.py
# Schedules all tools needed for jet/MET analyses and writes the
# results into SG. These may then be accessed along the train  
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
from AthenaCommon import CfgMgr
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
#                  Definitions of helper functions 
##################################################################

def defineEDAlg(R=0.4, inputtype="LCTopo"):
    from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAthAlg
    from AthenaCommon.AppMgr import ToolSvc
    from JetRec.JetRecStandard import jtm

    # map a getter to the input argument
    inputgetter = { "LCTopo" : jtm.lcget,
                    "EMTopo" : jtm.emget,
                    "LCTopoOrigin" : jtm.lcoriginget,
                    "EMTopoOrigin" : jtm.emoriginget,
                    "EMPFlow": jtm.empflowget,
                    "EMPFlowPUSB": jtm.empflowpusbget,
                    "EMPFlowNeut": jtm.empflowneutget,
                    "PFlowCustomVtx": jtm.pflowcustomvtxget
                    }[inputtype]

    t=configEventDensityTool("EDTool"+str(int(R*10))+inputtype,
                             inputgetter.Label,
                             radius = R)
    t.OutputLevel = 3
    ToolSvc += t
    return EventDensityAthAlg( "EventDensityAlg"+t.name(), EventDensityTool = t , OutputLevel=3)

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

def reCreatePseudoJets(jetalg, rsize, inputtype, variableRMassScale=-1.0, variableRMinRadius=-1.0, algseq=None, customGetters=None, constmods=[], includeVRGhosts=False):
    """Return a list of tools (possibly empty) to be run in a jetalg. These tools will make sure PseudoJets will be associated
    to the container specified by the input arguments.    
    """
    
    from JetRec.JetRecStandard import jtm
    from JetRec.JetRecUtils import buildJetContName
    constmodstr = "".join(constmods)
    inputname = inputtype+constmodstr
    jetContName = buildJetContName(jetalg, rsize, inputname, variableRMassScale, variableRMinRadius)

    # Set default for the arguments to be passd to addJetFinder
    finderArgs = dict( modifiersin= [], consumers = [], ghostArea= 0.01 , ptmin=40000, constmods=constmods, )
    
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
                           "PV0Track":"track_ungroomed"}
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
    getterMap = dict( LCTopo = 'lctopo', EMTopo = 'emtopo', EMPFlow = 'empflow', EMCPFlow = 'emcpflow', 
                      Truth='truth', TruthWZ='truthwz', TruthDressedWZ='truthdressedwz', TruthCharged='truthcharged',
                      PV0Track='pv0track')
    # create the finder for the temporary collection.

    if customGetters is None:
        getters = jtm.gettersMap[getterMap[inputtype]]
    else:
        getters=customGetters

    for getter in getters:
        if not hasattr(algseq, getter.name()):
            algseq += getter

    if includeVRGhosts:
        if not hasattr(algseq, jtm.gvrtrackget.name()):
            algseq += jtm.gvrtrackget
        getters = getters.copy() # So we don't modify the one in jtm
        getters += [jtm.gvrtrackget]
        

    if len(constmods) > 0:
        finderArgs['modifiersin'] = []

        from JetRecConfig import ConstModHelpers
        from JetRecConfig.JetDefinition import xAODType, JetConstit

        if inputtype == "EMTopo":
            constit = JetConstit( xAODType.CaloCluster, ["EM","Origin"])
        elif inputtype == "LCTopo":
            constit = JetConstit( xAODType.CaloCluster, ["LC","Origin"])
        elif inputtype == "EMPFlow":
            constit = JetConstit( xAODType.FlowElement )

        constit.modifiers += constmods

        constitalg = ConstModHelpers.getConstitModAlg(constit)
        if not hasattr(algseq, constitalg.name()):
            algseq += constitalg

        from JetRecConfig import JetRecConfig
        constitpjalg = JetRecConfig.getConstitPJGAlg( constit )
        if not hasattr(algseq, constitpjalg.name()):
            algseq += constitpjalg

        getterbase = inputtype.lower()
        getters = [constitpjalg]+list(jtm.gettersMap[getterbase])[1:]

    tmpFinderTool= jtm.addJetFinder(tmpName, jetalg, rsize, getters,
                                    **finderArgs   # pass the prepared arguments
                                    )
    return [tmpFinderTool]

##################################################################

def buildGenericGroomAlg(jetalg, rsize, inputtype, groomedName, jetToolBuilder,
                         includePreTools=False, algseq=None, outputGroup="CustomJets", doVRGhosts = False,
                         writeUngroomed=False, variableRMassScale=-1.0, variableRMinRadius=-1.0,
                         customGetters=None,constmods=[]):
    algname = "jetalg"+groomedName[:-4]

    from RecExConfig.AutoConfiguration import IsInInputFile
    if algseq is None:
        dfjetlog.info( "No algsequence passed! Will not schedule "+algname )
        return
    elif IsInInputFile("xAOD::JetContainer",groomedName):
        dfjetlog.info( "Collection "+groomedName+" is already in input AOD!" )
        return

    from JetRec.JetRecUtils import buildJetContName
    constmodstr = "".join(constmods)
    inputname = inputtype+constmodstr
    ungroomedName = buildJetContName(jetalg, rsize, inputname, variableRMassScale, variableRMinRadius)
    ungroomedalgname = "jetalg"+ungroomedName[:-4] # Remove "Jets" from name

    # add these groomed jets to the output (use setdefault() to constuct the list if not existing yet)
    OutputJets.setdefault(outputGroup , [] ).append(groomedName)
    if writeUngroomed:
        OutputJets.setdefault(outputGroup , [] ).append(ungroomedName)
        dfjetlog.info( "Write "+ungroomedName )

    from JetRec.JetRecConf import JetAlgorithm
    # return if the alg is already scheduled here :
    if hasattr(algseq,ungroomedalgname):
        finderalg = getattr(algseq, ungroomedalgname)
        dfjetlog.warning( "Algsequence "+algseq.name()+" already has an instance of "+ungroomedalgname )
    elif ungroomedalgname in DFJetAlgs:
        dfjetlog.info( "Added jet finder "+ ungroomedalgname+" to sequence"+ algseq.name() )
        finderalg = DFJetAlgs[ungroomedalgname]
        algseq += DFJetAlgs[ungroomedalgname]
    else:
        # 1. make sure we have pseudo-jet in our original container
        # this returns a list of the needed tools to do so.
        jetalgTools = reCreatePseudoJets(jetalg, rsize, inputtype, variableRMassScale, variableRMinRadius, algseq, customGetters=customGetters, constmods=constmods, includeVRGhosts=doVRGhosts)

        if includePreTools and jetFlags.useTracks() and "Truth" not in inputtype:
            # enable track ghost association and JVF
            from JetRec.JetRecStandard import jtm
            jetalgTools =  [jtm.tracksel, jtm.tvassoc] + jetalgTools 

        finderalg = JetAlgorithm(ungroomedalgname, Tools = jetalgTools )
        DFJetAlgs[ungroomedalgname] = finderalg
        dfjetlog.info( "Added jet finder "+ungroomedalgname+" to sequence "+algseq.name() )
        algseq += finderalg

    # 2nd step run the trimming alg. We can re-use the original largeR jet since we reassociated the PseudoJet already.
    fatjet_groom = jetToolBuilder(groomedName, ungroomedName)
    print (finderalg.Tools)
    print (ungroomedName)
    fatjet_rectool = [t for t in finderalg.Tools if t.name().endswith(ungroomedName)][0]
    fatjet_groom.InputPseudoJets = fatjet_rectool.InputPseudoJets # recopy the InputPseudoJets so tools know how to map fastjet constituents with xAOD constituents
    
    
    dfjetlog.info( "Added jet groomer "+algname+" to sequence "+algseq.name() )
    groomeralg = JetAlgorithm(algname, Tools = [fatjet_groom])
    DFJetAlgs[algname] = groomeralg
    algseq += groomeralg
    return groomeralg

##################################################################
def addTrimmedJets(jetalg, rsize, inputtype, rclus=0.3, ptfrac=0.05, mods="groomed",
                   includePreTools=False, algseq=None, outputGroup="Trimmed", includeVRGhosts=False,
                   writeUngroomed=False, variableRMassScale=-1.0, variableRMinRadius=-1.0,
                   customGetters=None, constmods=[]):
    from JetRec.JetRecUtils import buildJetAlgName
    inputname = inputtype + "".join(constmods)
    trimmedName = "{0}{1}TrimmedPtFrac{2}SmallR{3}Jets".format(buildJetAlgName(jetalg, rsize, variableRMassScale, variableRMinRadius),inputname,int(ptfrac*100),int(rclus*100))

    # a function dedicated to build Trimmed jet :
    def trimToolBuilder( name, inputJetCont):
        from JetRec.JetRecStandard import jtm
        if name in jtm.tools: return jtm.tools[name]
        else: return jtm.addJetTrimmer( name, rclus=rclus, ptfrac=ptfrac, input=inputJetCont, modifiersin=mods )

    dfjetlog.info( "Configuring trimmed jets : "+ trimmedName )
    # pass the trimmedName and our specific trimming tool builder to the generic function :
    return buildGenericGroomAlg(jetalg, rsize, inputtype, trimmedName, trimToolBuilder,
                                includePreTools, algseq, outputGroup,
                                writeUngroomed=writeUngroomed, doVRGhosts=includeVRGhosts,
                                variableRMassScale=variableRMassScale, variableRMinRadius=variableRMinRadius, 
                                customGetters=customGetters, constmods=constmods)


##################################################################
def addPrunedJets(jetalg, rsize, inputtype, rcut=0.50, zcut=0.15, mods="groomed",
                  includePreTools=False, algseq=None, outputGroup="Pruned",
                  writeUngroomed=False, customGetters=None, constmods=[]):
    inputname = inputtype + "".join(constmods)
    prunedName = "{0}{1}{2}PrunedR{3}Z{4}Jets".format(jetalg,str(int(rsize*10)),inputname,int(rcut*100),int(zcut*100))

    # a function dedicated to build Pruned jet :
    def pruneToolBuilder( name, inputJetCont):
        from JetRec.JetRecStandard import jtm
        if name in jtm.tools: return jtm.tools[name]
        else: return jtm.addJetPruner( name, rcut=rcut, zcut=zcut , input=inputJetCont, modifiersin=mods )

    dfjetlog.info( "Configuring pruned jets :  "+prunedName )
    # pass the trimmedName and our specific trimming tool builder to the generic function :
    return buildGenericGroomAlg(jetalg, rsize, inputtype, prunedName, pruneToolBuilder,
                                includePreTools, algseq, outputGroup,
                                writeUngroomed=writeUngroomed, customGetters=customGetters, constmods=constmods)


##################################################################
def addFilteredJets(jetalg, rsize, inputtype, mumax=1.0, ymin=0.15, mods="groomed",
                    includePreTools=False, algseq=None, outputGroup="Filtered",
                    writeUngroomed=False, customGetters=None, constmods=[]):
    inputname = inputtype + "".join(constmods)
    filteredName = "{0}{1}{2}BDRSFilteredMU{3}Y{4}Jets".format(jetalg,int(rsize*10),inputname,int(mumax*100),int(ymin*100))

    # a function dedicated to build Filtered jet :
    def filterToolBuilder( name, inputJetCont):
        from JetRec.JetRecStandard import jtm
        if name in jtm.tools: return jtm.tools[name]
        else: return jtm.addJetSplitter( name, mumax=mumax, ymin=ymin,  input=inputJetCont, modifiersin=mods )

    dfjetlog.info( "Configuring filtered jets :  "+filteredName )
    # pass the trimmedName and our specific trimming tool builder to the generic function :
    return buildGenericGroomAlg(jetalg, rsize, inputtype, filteredName, filterToolBuilder,
                                includePreTools, algseq, outputGroup,
                                writeUngroomed=writeUngroomed, customGetters=customGetters, constmods=constmods)


##################################################################

def addSoftDropJets(jetalg, rsize, inputtype, beta=0, zcut=0.1, mods="groomed",
                    includePreTools=False, algseq=None, outputGroup="SoftDrop", includeVRGhosts=False,
                    writeUngroomed=False, customGetters=None, constmods=[]):

    inputname = inputtype + "".join(constmods)
    softDropName = "{0}{1}{2}SoftDropBeta{3}Zcut{4}Jets".format(jetalg,int(rsize*10),inputname,int(beta*100),int(zcut*100))

    # a function dedicated to build SoftDrop jet:
    def softDropToolBuilder( name, inputJetCont):
        from JetRec.JetRecStandard import jtm
        if name in jtm.tools: return jtm.tools[name]
        else: return jtm.addJetSoftDrop( name, beta=beta, zcut=zcut, r0=rsize, input=inputJetCont, modifiersin=mods )

    dfjetlog.info( "Configuring soft drop jets :  "+softDropName )
    #pass the softDropName and our specific soft drop tool to the generic function:
    return buildGenericGroomAlg(jetalg, rsize, inputtype, softDropName, softDropToolBuilder,
                                includePreTools, algseq, outputGroup, doVRGhosts=includeVRGhosts,
                                writeUngroomed=writeUngroomed, customGetters=customGetters, constmods=constmods)

################################################################## 

def addRecursiveSoftDropJets(jetalg, rsize, inputtype, beta=0, zcut=0.1, N=-1, mods="groomed",
                             includePreTools=False, algseq=None, outputGroup="SoftDrop",
                             writeUngroomed=False, customGetters=None, constmods=[]):
    inputname = inputtype + "".join(constmods)
    if N >= 0:
      softDropName = "{0}{1}{2}RecursiveSoftDropBeta{3}Zcut{4}N{5}Jets".format(jetalg,int(rsize*10),inputname,int(beta*100),int(zcut*100), int(N))
    if N < 0:
      softDropName = "{0}{1}{2}RecursiveSoftDropBeta{3}Zcut{4}NinfJets".format(jetalg,int(rsize*10),inputname,int(beta*100),int(zcut*100))


    # a function dedicated to build SoftDrop jet:
    def recursiveSoftDropToolBuilder( name, inputJetCont):
        from JetRec.JetRecStandard import jtm
        if name in jtm.tools: return jtm.tools[name]
        else: return jtm.addJetRecursiveSoftDrop( name, beta=beta, zcut=zcut, N=N, r0=rsize, input=inputJetCont, modifiersin=mods )

    dfjetlog.info( "Configuring soft drop jets :  "+softDropName )
    #pass the softDropName and our specific soft drop tool to the generic function:
    return buildGenericGroomAlg(jetalg, rsize, inputtype, softDropName, recursiveSoftDropToolBuilder,
                                includePreTools, algseq, outputGroup,
                                writeUngroomed=writeUngroomed, customGetters=customGetters, constmods=constmods)

################################################################## 

def addBottomUpSoftDropJets(jetalg, rsize, inputtype, beta=0, zcut=0.1, mods="groomed",
                            includePreTools=False, algseq=None, outputGroup="SoftDrop",
                            writeUngroomed=False, customGetters=None, constmods=[]):
    inputname = inputtype + "".join(constmods)
    softDropName = "{0}{1}{2}BottomUpSoftDropBeta{3}Zcut{4}Jets".format(jetalg,int(rsize*10),inputname,int(beta*100),int(zcut*100))

    # a function dedicated to build SoftDrop jet:
    def bottomUpSoftDropToolBuilder( name, inputJetCont):
        from JetRec.JetRecStandard import jtm
        if name in jtm.tools: return jtm.tools[name]
        else: return jtm.addJetBottomUpSoftDrop( name, beta=beta, zcut=zcut, r0=rsize, input=inputJetCont, modifiersin=mods )

    dfjetlog.info( "Configuring soft drop jets :  "+softDropName )
    #pass the softDropName and our specific soft drop tool to the generic function:
    return buildGenericGroomAlg(jetalg, rsize, inputtype, softDropName, bottomUpSoftDropToolBuilder,
                                includePreTools, algseq, outputGroup,
                                writeUngroomed=writeUngroomed, customGetters=customGetters, constmods=constmods)

################################################################## 

def addStandardJets(jetalg, rsize, inputtype, ptmin=0., ptminFilter=0.,
                    mods="default", calibOpt="none", ghostArea=0.01,
                    algseq=None, namesuffix="",
                    outputGroup="CustomJets", customGetters=None, pretools = [], constmods = [],
                    overwrite=False):

    jetnamebase = "{0}{1}{2}{3}".format(jetalg,int(rsize*10),inputtype,namesuffix)
    jetname = jetnamebase+"Jets"
    algname = "jetalg"+jetnamebase
    OutputJets.setdefault(outputGroup , [] ).append(jetname)

    if algseq is None:
        dfjetlog.warning( "No algsequence passed! Will not schedule "+algname )
        return
    
    from RecExConfig.AutoConfiguration import IsInInputFile
    if IsInInputFile("xAOD::JetContainer",jetname) and not overwrite:
        dfjetlog.warning( "Collection  "+jetname+" is already in input AOD!" )
        return
    
    from JetRec.JetRecStandard import jtm

    # Check if we need to schedule the PV0 track selection alg
    if inputtype == 'PV0Track':
        if not hasattr(algseq,"pv0tracksel_trackjet"):
            algseq += jtm.pv0tracksel_trackjet

    # return if the alg is already scheduled here :
    if algname in DFJetAlgs:
        if hasattr(algseq,algname):
            dfjetlog.warning( "Algsequence "+algseq.name()+" already has an instance of "+algname )
        else:
            dfjetlog.info( "Added "+algname+" to sequence "+algseq.name() )
            algseq += DFJetAlgs[algname]
        return DFJetAlgs[algname]

    if jetname not in jtm.tools:
        # no container exist. simply build a new one.
        # Set default for the arguments to be passd to addJetFinder
        defaultmods = {"EMTopo":"emtopo_ungroomed",
                       "LCTopo":"lctopo_ungroomed",
                       "EMPFlow":"pflow_ungroomed",
                       "EMCPFlow":"pflow_ungroomed",
                       "Truth":"truth_ungroomed",
                       "TruthWZ":"truth_ungroomed",
                       "PV0Track":"track_ungroomed",
                       }
        if mods=="default":
            mods = defaultmods[inputtype] if inputtype in defaultmods else []
        finderArgs = dict( modifiersin= mods, consumers = [])
        finderArgs['ptmin'] = ptmin
        finderArgs['ptminFilter'] = ptminFilter
        finderArgs['ghostArea'] = ghostArea
        finderArgs['modifiersin'] = mods
        finderArgs['calibOpt'] = calibOpt
        print ("mods in:", finderArgs['modifiersin'])
        if overwrite:
            dfjetlog.info("Will overwrite AOD version of "+jetname)
            finderArgs['overwrite']=True
    
        # map the input to the jtm code for PseudoJetGetter
        getterMap = dict( LCTopo = 'lctopo', EMTopo = 'emtopo', EMPFlow = 'empflow', EMCPFlow = 'emcpflow',
                          Truth = 'truth',  TruthWZ = 'truthwz', TruthDressedWZ = 'truthdressedwz', TruthCharged = 'truthcharged',
                          PV0Track='pv0track')
        # create the finder for the temporary collection.

        if customGetters is None:
            inGetter = getterMap[inputtype]
            for getter in jtm.gettersMap[inGetter]:
                if not hasattr(algseq, getter.name()):
                    algseq += getter
        else:
            inGetter = customGetters
            for getter in customGetters:
                if not hasattr(algseq, getter.name()):
                    algseq += getter

        finderTool= jtm.addJetFinder(jetname, jetalg, rsize, inGetter, constmods=constmods,
                                     **finderArgs   # pass the prepared arguments
                                     )

        from JetRec.JetRecConf import JetAlgorithm
        alg = JetAlgorithm(algname, Tools = pretools+[finderTool])
        dfjetlog.info( "Added "+algname+" to sequence "+algseq.name() )
        algseq += alg
        DFJetAlgs[algname] = alg

#####################################################################

def addStandardVRTrackJets(jetalg, vrMassScale, maxR, minR, inputtype, ptmin=0.,
                           ghostArea=0, algseq=None, outputGroup="CustomJets"):
    
    VRJetNameBase = "{0}VR{1}Rmax{2}Rmin0{3}{4}".format(jetalg,int(vrMassScale/1000),int(maxR*10),int(minR*100),inputtype)
    VRJetOptions = dict(calibOpt = "none")
    VRJetOptions['variableRMinRadius'] = minR
    VRJetOptions['variableRMassScale'] = vrMassScale
    VRJetOptions['ptmin'] = ptmin
    VRJetOptions['ghostArea'] = ghostArea

    VRJetName = VRJetNameBase + "Jets"
    dfjetlog.info("VR jet name: " + VRJetName)

    algname = "jetalg"+VRJetNameBase

    if algseq is None:
        dfjetlog.warning( "No algsequence passed! Will not schedule "+algname )
        return    

    from JetRec.JetRecStandard import jtm

    # map the input to the jtm code for PseudoJetGetter
    getterMap = dict( LCTopo = 'lctopo', EMTopo = 'emtopo', EMPFlow = 'empflow', EMCPFlow = 'emcpflow',
                      Truth = 'truth',  TruthWZ = 'truthwz', TruthDressedWZ = 'truthdressedwz', TruthCharged = 'truthcharged',
                      PV0Track='pv0track')

    # Copy so that if we modify it we don't also change the array in jtm.
    pseudoJetGetters = jtm.gettersMap[getterMap[inputtype]].copy()

    # Check if we need to schedule the PV0 track selection alg
    if inputtype == 'PV0Track':
        if not hasattr(algseq,"pv0tracksel_trackjet"):
            algseq += jtm.pv0tracksel_trackjet

    OutputJets.setdefault(outputGroup , [] ).append(VRJetName)

    # return if the alg is already scheduled here :
    if algname in DFJetAlgs:
        if hasattr(algseq,algname):
            dfjetlog.warning( "Algsequence "+algseq.name()+" already has an instance of "+algname )
        else:
            dfjetlog.info( "Added "+algname+" to sequence "+algseq.name() )
            algseq += DFJetAlgs[algname]
        return DFJetAlgs[algname]

    if VRJetName not in jtm.tools:
        mods = []
        # add VR track jet overlap decorator tool to list of jet modifiers
        vrODT = None
        from AthenaCommon.AppMgr import ToolSvc
        if hasattr(ToolSvc,"VRJetOverlapDecoratorTool"):
            vrODT = getattr(ToolSvc,"VRJetOverlapDecoratorTool")
        else:
            vrODT = CfgMgr.FlavorTagDiscriminants__VRJetOverlapDecoratorTool("VRJetOverlapDecoratorTool")
            ToolSvc += vrODT
        mods += [vrODT]

        # Cone matching for B, C and tau truth for VR track jets.
        if jetFlags.useTruth and jtm.haveParticleJetTools and inputtype == 'PV0Track':
            mods += [jtm.trackjetdrlabeler, jtm.ghostlabeler]

        VRJetOptions['modifiersin'] = mods

        for getter in pseudoJetGetters:
            if not hasattr(algseq, getter.name()):
                algseq += getter

        finderTool= jtm.addJetFinder(VRJetName, jetalg, maxR, pseudoJetGetters, **VRJetOptions)

        from JetRec.JetRecConf import JetAlgorithm
        alg = JetAlgorithm(algname, Tools = [finderTool])
        dfjetlog.info( "Added "+algname+" to sequence "+algseq.name() )
        algseq += alg
        DFJetAlgs[algname] = alg

##################################################################
# Schedule the augmentation of a flag to label events with large
# EMEC-IW Noise based on the presence of many bad quality clusters
##################################################################

def addBadBatmanFlag(sequence=DerivationFrameworkJob):

    from RecExConfig.ObjKeyStore import objKeyStore

    if not objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
        if hasattr(sequence,"BadBatmanAugmentation"):
            dfjetlog.warning( "BadBatmanAugmentation: BadBatmanAugmentation already scheduled on sequence "+sequence.name )
        else:
            # Check if we have clusters.  If we don't then this cannot run
            if objKeyStore.isInInput( "xAOD::CaloClusterContainer", "CaloCalTopoClusters" ):
                # schedule it
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
                if batmanaugtool not in batmanaug.AugmentationTools:
                    batmanaug.AugmentationTools.append(batmanaugtool)
            else:
                dfjetlog.warning('Could not schedule BadBatmanAugmentation!!!')
    else:
        dfjetlog.warning('Running over EVNT filess, BadBatmanAugmentation will not be scheduled')

################################################################## 
# Schedule the adding of BCID info
################################################################## 
def addDistanceInTrain(sequence=DerivationFrameworkJob):
    # simple set up -- either the alg exists and contains the tool, in which case we exit
    if hasattr(sequence,"DistanceInTrainAugmentation"):
        dfjetlog.warning( "DistanceInTrainAugmentation: DistanceInTrainAugmentation already scheduled on sequence"+sequence.name )
        return
    else:
        distanceintrainaug = CfgMgr.DerivationFramework__CommonAugmentation("DistanceInTrainAugmentation")
        sequence += distanceintrainaug

        distanceintrainaugtool = None
        from AthenaCommon.AppMgr import ToolSvc
        # create and add the tool to the alg if needed                                                                                                                                                      
        if hasattr(ToolSvc,"DistanceInTrainAugmentationTool"):
            distanceintrainaugtool = getattr(ToolSvc,"DistanceInTrainAugmentationTool")
        else:
            distanceintrainaugtool = CfgMgr.DerivationFramework__DistanceInTrainAugmentationTool("DistanceInTrainAugmentationTool")
            ToolSvc += distanceintrainaugtool
            from LumiBlockComps.BunchCrossingCondAlgDefault import BunchCrossingCondAlgDefault
            BunchCrossingCondAlgDefault()

        if distanceintrainaugtool not in distanceintrainaug.AugmentationTools:
            distanceintrainaug.AugmentationTools.append(distanceintrainaugtool)

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
        if item not in slimhelper.AppendToDictionary:
            slimhelper.AppendToDictionary[item]='xAOD::JetContainer'
            slimhelper.AppendToDictionary[item+"Aux"]='xAOD::JetAuxContainer'
        if item in smartlist:
            dfjetlog.info( "Add smart jet collection "+item )
            slimhelper.SmartCollections.append(item)
        else:
            dfjetlog.info( "Add full jet collection "+item )
            slimhelper.AllVariables.append(item)
