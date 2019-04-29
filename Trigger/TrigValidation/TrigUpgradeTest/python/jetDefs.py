#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
from AthenaCommon.Constants import ERROR, DEBUG


def jetAthSequence(ConfigFlags):
    from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    InputMakerAlg= clusterFSInputMaker()

    dataOrMC = "data"  #FIXME
    #if ConfigFlags.Input.isMC:
    #    dataOrMC = "mc"
    # want to make this automatic.

    jetDefinition = ConfigFlags.jetdefinition

    if jetDefinition=="EMTopoSubJESIS":
    	(recoSequence, sequenceOut) = jetRecoSequence("Calib:TrigSubJESIS:"+dataOrMC)
    elif jetDefinition=="EMTopoSubJES":
    	(recoSequence, sequenceOut) = jetRecoSequence("Calib:TrigSubJES:"+dataOrMC)
    elif jetDefinition=="EMTopoNoCalib":
    	(recoSequence, sequenceOut) = jetRecoSequence("")
    elif jetDefinition=="LCWSubJESIS": # call for LC sequence
    	(recoSequence, sequenceOut) = jetRecoSequence("Calib:TrigSubJESIS:"+dataOrMC, jetConstitName = "LC")
    elif jetDefinition=="a10LCWSubJESJMS":
    	#(recoSequence, sequenceOut) = jetRecoSequence("Calib:TrigSubJESJMS:"+dataOrMC, jetAlgoName = 'a10', jetConstitName = "LC")
    	(recoSequence, sequenceOut) = jetRecoSequence("", jetAlgoName = 'a10', jetConstitName = "LC")
    elif jetDefinition=="a10rSubJESIS":
    	(recoSequence, sequenceOut) = jetRecoSequence("Calib:TrigSubJESIS:"+dataOrMC, jetAlgoName = 'a10r')

    JetAthSequence =  seqAND("jetAthSequence_"+jetDefinition,[InputMakerAlg, recoSequence ])
    return (JetAthSequence, InputMakerAlg, sequenceOut)

    
def jetRecoSequence( calibString, jetConstitName = 'EM', jetAlgoName = 'a4', RoIs = 'FSJETRoI'):

    calibSeq = ""
    dataType = "data" # FIXME
    if calibString:
        calibSeq = calibString.split(":")[1]
        dataType = calibString.split(":")[2]

    radius = 0.4
    if jetAlgoName == 'a10':
        radius = 1.0

    doLC=False
    if jetConstitName=="LC": 
        doLC=True

    cellMakerAlgo = _getHLTCellMakerAlgoForJets("cellMaker"+jetConstitName, RoIs, outputEDM='CaloCells'+jetConstitName, OutputLevel=ERROR) 
    topoClusterMakerAlgo = _getHLTTopoClusterMakerAlgoForJets( "topoClusterMaker"+jetConstitName, inputEDM=cellMakerAlgo.CellsName, doLC=doLC, OutputLevel=ERROR)

    caloMakerSequence = parOR("TopoClusterRecoSequence"+jetConstitName+calibSeq, [cellMakerAlgo, topoClusterMakerAlgo]) 
    caloclusters = topoClusterMakerAlgo.CaloClusters

    from JetRecConfig.JetDefinition import JetConstit, JetDefinition, xAODType, JetModifier
  
    #hardcoded jet collection for now 
    # chosen jet collection
    jetsFullName = "TrigAntiKt"+jetConstitName+calibSeq
    trigMinPt = 7e3 # FIXME : two different values for ptminfilter?
    trigJetConstit = JetConstit( xAODType.CaloCluster, [jetConstitName]) # 'EM' or 'LC' for trigger jets
    trigJetConstit.ptmin = 2e3
    #trigJetConstit.ptminfilter = 15e3
    trigJetConstit.ptminfilter = trigMinPt
    trigJetDef = JetDefinition( "AntiKt", radius, trigJetConstit, ptmin=trigMinPt,ptminfilter=trigMinPt)
    #trigJetDef.modifiers = ["Sort"] + clustermods 
    #if calibString:
    #    trigJetDef.modifiers= [calibString] + trigJetDef.modifiers
    trigJetDef.isTrigger = True #FIXME : no longer needed since configure CalibTool here?

    from JetRecConfig import JetRecConfig
    #deps = JetRecConfig.resolveDependencies( trigJetDef )

    modList = [ (JetModifier("JetCaloEnergies", "jetens"), '') ]
    modList += [ (JetModifier("JetSorter","jetsort"), '') ]
    #deps_mod_tmp = [ modpair for modpair in deps["mods"] if not calibSeq in modpair[1] ]
    #deps["mods"] = deps_mod_tmp
    if calibString:
   	 from JetCalibTools import JetCalibToolsConfig
   	 jetCalibTool = JetCalibToolsConfig.getJetCalibTool( trigJetDef.basename, calibSeq, dataType )
   	 modList += [(JetModifier("JetCalibrationTool", jetCalibTool.name()), calibSeq+':'+dataType)]
    #jetFilterTool = _getJetFilterTool(trigJetConstit.ptminfilter)
    modList += [ ( JetModifier("JetFilterTool","jetptfilter"), str(int(trigJetConstit.ptminfilter)))  ]

    print("modList: ", modList)

    trigJetConstit.rawname = caloclusters
    print("trigJetConstit.rawname = ", trigJetConstit.rawname )
    print("trigJetConstit.inputname = ", trigJetConstit.inputname )
    constitAlg = _getConstitAlg( trigJetConstit )
    jetRecoSequence = parOR( "JetRecSeq_"+jetsFullName, [constitAlg])

    constitPJAlg = _getConstitPJGAlg( trigJetConstit )
    constitPJKey = constitPJAlg.PJGetter.OutputContainer

    pjs = [constitPJKey]

    jetRecoSequence += constitPJAlg 
        
    eventShapeAlg = _getEventShapeAlg( trigJetConstit, constitPJKey )
    jetRecoSequence += eventShapeAlg                    

    # Schedule the ghost PseudoJetGetterAlgs # FIXME : not needed.
    #ghostList  = []
    #for ghostdef in ghostList:
    #    print("ghostdef = ", ghostdef)
    #    ghostPJAlg = _getGhostPJGAlg( ghostdef )
    #    jetRecoSequence += ghostPJAlg
    #    ghostPJKey = ghostPJAlg.PJGetter.OutputContainer
    #    pjs.append( ghostPJKey )

    # Generate a JetAlgorithm to run the jet finding and modifiers
    # (via a JetRecTool instance).
    jetRecAlg = JetRecConfig.getJetAlgorithm(jetsFullName, trigJetDef, pjs, modList)

    jetRecoSequence += jetRecAlg

    if jetAlgoName == 'a10r':

        a10rJetsFullName = "a10rJets"
        a10rJetConstit = JetConstit( xAODType.Jet, [])
        trigAntiKt10rJetDef = JetDefinition( "AntiKt", 1.0, a10rJetConstit)

        a10rmodList= []
        a10rJetConstit.rawname = jetsFullName 
        print("a10rJetConstit.rawname = ", a10rJetConstit.rawname )
        print("a10rconstitPJAlg.inputname = ", a10rJetConstit.inputname )
        a10rconstitPJAlg = _getConstitPJGAlg( a10rJetConstit )
        a10rconstitPJKey = a10rconstitPJAlg.PJGetter.OutputContainer

        print "INFO: output psj container for reclustered jets = ", a10rconstitPJKey

        a10rpjs = [a10rconstitPJKey]
        a10rjetRecAlg = JetRecConfig.getJetAlgorithm(a10rJetsFullName, trigAntiKt10rJetDef, a10rpjs, a10rmodList)

        jetRecoSequence += a10rjetRecAlg
        #jetsFullName = reclusteredJetsFullName

    sequenceOut = jetsFullName

    caloMakerSequence += jetRecoSequence
    #jetRecoSequence += caloMakerSequence

    #jetRecoFullSequence = seqAND("fullJetSeq_"+jetsFullName, [caloMakerSequence, jetRecoSequence]) 
    jetRecoFullSequence = caloMakerSequence
    #jetRecoFullSequence = jetRecoSequence

    return (jetRecoFullSequence,sequenceOut)

def _getConstitAlg( constit ):

        from JetRecConfig import ConstModHelpers
        constitalg = ConstModHelpers.getConstitModAlg(constit)
        return constitalg

def _getConstitPJGAlg(basedef):

    from JetRec import JetRecConf
    getter = JetRecConf.PseudoJetGetter("pjg_"+basedef.label,
        InputContainer = basedef.inputname,
        OutputContainer = "PseudoJet"+basedef.label,
        Label = basedef.label,
        SkipNegativeEnergy=True,
        GhostScale=0.
        )

    pjgalg = JetRecConf.PseudoJetAlgorithm(
        "pjgalg_"+basedef.label,
        PJGetter = getter
        )
    return pjgalg

def _getGhostPJGAlg(ghostdef):

    label = "Ghost"+ghostdef.inputtype
    kwargs = {
        "OutputContainer":    "PseudoJet"+label,
        "Label":              label,
        "SkipNegativeEnergy": True,
        "GhostScale":         1e-40
        }

    from JetRec import JetRecConf
    pjgclass = JetRecConf.PseudoJetGetter
    if ghostdef.inputtype=="MuonSegment":
        # Muon segments have a specialised type
        pjgclass = JetRecConf.MuonSegmentPseudoJetGetter
        kwargs = {
            "InputContainer":"MuonSegments",
            "OutputContainer":"PseudoJet"+label,
            "Label":label,
            "Pt":1e-20
            }
    elif ghostdef.inputtype=="Track":
        kwargs["InputContainer"] = "JetSelectedTracks"
    elif ghostdef.inputtype.startswith("TruthLabel"):
        truthsuffix = ghostdef.inputtype[5:]
        kwargs["InputContainer"] = "TruthLabel"+truthsuffix
    elif ghostdef.inputtype == "Truth":
        kwargs["InputContainer"] = "JetInputTruthParticles"
    else:
        raise ValueError("Unhandled ghost type {0} received!".format(ghostdef.inputtype))

    getter = pjgclass("pjg_"+label, **kwargs)

    pjgalg = JetRecConf.PseudoJetAlgorithm(
        "pjgalg_"+label,
        PJGetter = getter
        )
    return pjgalg

def _getTrackPrepAlg( trackPrepAlgName ):

    from JetRecTools import JetRecToolsConfig
    from JetRec import JetRecConf
    # Jet track selection
    jettrackselloose = JetRecToolsConfig.getTrackSelTool()
    jettvassoc = JetRecToolsConfig.getTrackVertexAssocTool()

    jettrkprepalg = JetRecConf.JetAlgorithm(trackPrepAlgName)
    jettrkprepalg.Tools = [ jettrackselloose, jettvassoc ]

    return jettrkprepalg

def _getEventShapeAlg( constit, constitpjkey ):

    rhokey = "Kt4"+constit.label+"EventShape"
    rhotoolname = "EventDensity_Kt4"+constit.label
    
    from EventShapeTools import EventShapeToolsConf
    rhotool = EventShapeToolsConf.EventDensityTool(rhotoolname)
    rhotool.InputContainer = constitpjkey
    rhotool.OutputContainer = rhokey
    
    eventshapealg = EventShapeToolsConf.EventDensityAthAlg("{0}Alg".format(rhotoolname))
    eventshapealg.EventDensityTool = rhotool

    return eventshapealg

def _getHLTCellMakerAlgoForJets( name, RoIs, outputEDM='CaloCells', OutputLevel=ERROR ):

    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if not hasattr(svcMgr,'TrigCaloDataAccessSvc'):
      from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc
      svcMgr+=TrigCaloDataAccessSvc()
      svcMgr.TrigCaloDataAccessSvc.OutputLevel=ERROR
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker

    inputEDM = RoIs
    algo=HLTCaloCellMaker(name)
    algo.RoIs=inputEDM
    algo.TrigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
    algo.OutputLevel=OutputLevel
    algo.CellsName=outputEDM
   
    return algo

def _getHLTTopoClusterMakerAlgoForJets(name, inputEDM, doLC=False, OutputLevel=ERROR):

    from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMakerMT_topo

    algo = TrigCaloClusterMakerMT_topo(name, doMoments=True, doLC=doLC, cells=inputEDM)
    algo.CaloClusters="caloclusters"
    if doLC:
        algo.CaloClusters+="LC"
    algo.OutputLevel=OutputLevel
    return algo

def _getJetFilterTool( threshold ):
	
    from JetRec import JetRecConf
    threshold = int(threshold)
    jetptfilter = JetRecConf.JetFilterTool("jetptfilter_{0}mev".format(threshold))
    jetptfilter.PtMin = threshold     
    return jetptfilter


