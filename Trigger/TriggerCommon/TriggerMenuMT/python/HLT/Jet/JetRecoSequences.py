#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, findAllAlgorithms
from AthenaCommon.Configurable import ConfigurableRun3Behavior
from TriggerMenuMT.HLT.Config.ChainConfigurationBase import RecoFragmentsPool
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable, appendCAtoAthena
from JetRecConfig import JetInputConfig, JetRecConfig
from JetRecConfig.DependencyHelper import solveDependencies, solveGroomingDependencies

from TrigEDMConfig.TriggerEDMRun3 import recordable

from . import JetRecoCommon
from TriggerMenuMT.HLT.CommonSequences.CaloSequences import caloClusterRecoSequence, LCCaloClusterRecoSequence
from eflowRec.PFHLTSequence import PFHLTSequence
from eflowRec.PFHLTSequence import trackvtxcontainers
from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking, makeInDetTrigFastTrackingNoView
from TrigInDetConfig.InDetTrigVertices import makeInDetTrigVertices
from .JetTrackingConfig import jetTTVA
from JetRec.JetRecConf import JetViewAlg

# this code uses CA internally, needs to be in this context manager,
# at least until ATLASRECTS-6635 is closed
with ConfigurableRun3Behavior():
    from ..Bjet.BjetFlavourTaggingConfiguration import getFastFlavourTagging

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

# prefix used in naming HLT collections
jetNamePrefix = JetRecoCommon.getHLTPrefix() # "HLT_"

###############################################################################################
### --- Reco sequence getters ---                                                                  

# The top-level sequence, forwards arguments as appropriate to 
# standard jet reco, grooming or reclustering sequences
# If tracking is required, then the trkcolls dict (returned by the
# JetFSTrackingSequence) must also be passed as kwargs
def jetRecoSequence( configFlags, clustersKey, **jetRecoDict ):

    jetalg, jetradius, extra = JetRecoCommon.interpretRecoAlg(jetRecoDict["recoAlg"])
    doGrooming = extra in ["t","sd"]
    doRecluster = extra == "r"
    dataSource = "mc" if configFlags.Input.isMC else "data"

    if doRecluster:
        return RecoFragmentsPool.retrieve(
            reclusteredJetRecoSequence, 
            configFlags, dataSource=dataSource,
            clustersKey=clustersKey, **jetRecoDict)
    elif doGrooming:
        return RecoFragmentsPool.retrieve(
            groomedJetRecoSequence,
            configFlags, dataSource=dataSource,
            clustersKey=clustersKey, **jetRecoDict)
    else:
        jetRecoSeq, jetsIn, jetDef = RecoFragmentsPool.retrieve(
                                     standardJetRecoSequence,
                                     configFlags, dataSource=dataSource,
                                     clustersKey=clustersKey, **jetRecoDict)
        return jetRecoSeq, jetsIn, jetDef

# Get a configured JetViewAlg that creates a VIEW_ELEMENTS container of jets above a minimum jet pT
# Filtered jets are given to hypo.
# jetPtMin is minimum jet pt in GeV for jets to be seen by hypo
def getJetViewAlg(configFlags,jetsIn,jetPtMin=10,**jetRecoDict):

    decorList = JetRecoCommon.getDecorList(jetRecoDict)
    filteredJetsName = f"{jetsIn}_pt{int(jetPtMin)}"
    jetViewAlg= JetViewAlg("jetview_"+filteredJetsName,
                          InputContainer=jetsIn,
                          OutputContainer=filteredJetsName,
                          PtMin=jetPtMin*1e3, #MeV
                          DecorDeps=decorList
    )
    jetsOut = filteredJetsName

    return jetViewAlg,jetsOut

def standardJetBuildSequence( configFlags, dataSource, clustersKey, **jetRecoDict ):
    """This build the standard jet (not groomed or reclustered).

    This is similar to JetRecConfig.getJetDefAlgs(). However due to how the alg flow is organized in the 
    chain steps, we can't use this function directly.
    Instead we 
      - construct a JetDefinition
      - use lower-level function in JetRecConfig with this JetDefinition to get the necessary algs and build our sequence manually.

    """
    
    jetDefString = JetRecoCommon.jetRecoDictToString(jetRecoDict)
    buildSeq = parOR( "JetBuildSeq_"+jetDefString, [])
    doesFSTracking = JetRecoCommon.doFSTracking(jetRecoDict)

    context = JetRecoCommon.getJetContext(jetRecoDict)
    
    # *****************************
    # First part : build a JetDefinition (and a pflow alg if needed)
    # Add particle flow reconstruction if needed
    if JetRecoCommon.isPFlow(jetRecoDict):
        (pfseq, pfoPrefix) = RecoFragmentsPool.retrieve(
            PFHLTSequence,
            configFlags, clustersin=clustersKey, tracktype=jetRecoDict["trkopt"], cellsin="CaloCellsFS")
        buildSeq += pfseq
        jetDef = JetRecoCommon.defineJets(jetRecoDict,pfoPrefix=pfoPrefix,prefix=jetNamePrefix)
    else:
        jetDef = JetRecoCommon.defineJets(jetRecoDict,clustersKey=clustersKey,prefix=jetNamePrefix)
    
    # chosen jet collection
    jetsFullName = jetDef.fullname()
    jetsOut = recordable(jetsFullName)

    # build the list of jetModifiers.
    # Sort and filter
    jetModList = ["Sort", "Filter:"+str(JetRecoCommon.getFilterCut(jetRecoDict["recoAlg"])), "ConstitFourMom_copy"]
    if doesFSTracking:
        jetModList += ["TrackMoments", "JVF", "JVT"]

    if jetRecoDict["recoAlg"] == "a4":
        jetModList += ["CaloEnergies"] # Needed for GSC

    jetDef.modifiers = jetModList    
    # make sure all the modifiers have their dependencies solved 
    jetDef = solveDependencies(jetDef)

    # *****************************
    # Second part : instantiate the actual algs and insert them in the sequence 
    skipConstitMods = (jetRecoDict["constitMod"]=='') and (jetRecoDict["constitType"]=='tc') and (jetRecoDict["clusterCalib"]=="lcw")
    if not skipConstitMods:
        # Then we need a constituent modifier sequence. 
        # Get online monitoring jet rec tool
        from JetRecTools import OnlineMon                                                  
        monJetRecTool = OnlineMon.getMonTool_Algorithm("HLTJets/"+jetsFullName+"/")

        # get the alg from the standard jet config helper :
        constitModAlg = JetRecConfig.getConstitModAlg(jetDef, jetDef.inputdef, monTool=monJetRecTool)
        if constitModAlg:
            buildSeq += constitModAlg

    # Add the PseudoJetGetter alg to the sequence
    constitPJAlg = JetRecConfig.getConstitPJGAlg( jetDef.inputdef , suffix=None)
    buildSeq += conf2toConfigurable( constitPJAlg )
    finalpjs = str(constitPJAlg.OutputContainer)

    if JetRecoCommon.jetDefNeedsTracks(jetRecoDict):
        # We need to do ghost association.
        # The ghost tracks pseudoJet are build in other part of the chain : here
        # we just need to merge our constituents with them
        finalpjs = finalpjs+"MergedWithGhostTracks"
        mergerName = "PJMerger_"+finalpjs
        from JetRec import JetRecConf
        mergeAlg = JetRecConf.PseudoJetMerger(
            mergerName,
            InputPJContainers = [str(constitPJAlg.OutputContainer),context["GhostTracks"]],
            OutputContainer = finalpjs)
        buildSeq += mergeAlg

    # set the name of the final PseudoJetContainer to be used as input :
    jetDef._internalAtt['finalPJContainer'] = finalpjs
            
    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+jetsFullName+"/")

    # finally get the JetRecAlg :
    jetRecAlg = JetRecConfig.getJetRecAlg(jetDef, monTool=monTool)
    buildSeq += conf2toConfigurable( jetRecAlg )
    
    if configFlags.Trigger.Jet.doVRJets and JetRecoCommon.jetDefNeedsTracks(jetRecoDict) and 'a10' in jetRecoDict['recoAlg']:
        buildSeqVR, jetsOutVR, jetDefVR = RecoFragmentsPool.retrieve(VRJetRecoSequence, configFlags, trkopt = jetRecoDict['trkopt'])
        buildSeq += buildSeqVR

    return buildSeq, jetsOut, jetDef

def standardJetRecoSequence( configFlags, dataSource, clustersKey, **jetRecoDict ):
    jetDefString = JetRecoCommon.jetRecoDictToString(jetRecoDict)

    # Schedule reconstruction w/o calibration
    # This is just a starting point -- will change so that
    # the calibration is only ever done at the end for ungroomed
    _jetRecoDictNoJCalib = dict(jetRecoDict)
    _jetRecoDictNoJCalib["jetCalib"] = "nojcalib"

    buildSeq, jetsNoCalib, jetDefNoCalib = RecoFragmentsPool.retrieve( standardJetBuildSequence, configFlags, dataSource=dataSource,
                                                            clustersKey=clustersKey, **_jetRecoDictNoJCalib)

    recoSeq = parOR( "JetRecSeq_"+jetDefString, [buildSeq])

    # prepare and return here if original calibration is nojcalib - no further calibration or modifiers required
    if jetRecoDict["jetCalib"]=="nojcalib":
        jetViewAlg, jetsOut = getJetViewAlg(configFlags,jetsIn=jetDefNoCalib.fullname(),**jetRecoDict)
        recoSeq += jetViewAlg
        return recoSeq, jetsOut, jetDefNoCalib

    # Get the calibration tool if desired. 
    jetDef = jetDefNoCalib.clone()
    jetDef.suffix = jetDefNoCalib.suffix.replace("nojcalib",jetRecoDict["jetCalib"])

    rhoKey = "auto"
    if "sub" in jetRecoDict["jetCalib"]:
        # Add the event shape alg if needed for area subtraction
        # WARNING : offline jets use the parameter voronoiRf = 0.9 ! we might want to harmonize this.
        
        eventShapeAlg = JetInputConfig.buildEventShapeAlg( jetDef, jetNamePrefix, voronoiRf = 1.0 )
        recoSeq += conf2toConfigurable(eventShapeAlg)
        # Not currently written because impossible to merge
        # across event views, which is maybe a concern in
        # the case of regional PFlow
        rhoKey = str(eventShapeAlg.EventDensityTool.OutputContainer)

    jetDef.modifiers = JetRecoCommon.getCalibMods(jetRecoDict,dataSource,rhoKey)
    # If we need JVT, just rerun the JVT modifier
    decorList = JetRecoCommon.getDecorList(jetRecoDict)

    if JetRecoCommon.doFSTracking(jetRecoDict):
        jetDef.modifiers.append("JVT")
   
    if jetRecoDict['recoAlg']=='a4':
        jetDef.modifiers.append("CaloQuality")

    #Configuring jet cleaning mods now
    if not JetRecoCommon.isPFlow(jetRecoDict) and jetRecoDict['recoAlg']=='a4': #Add jet cleaning decorations only to small-R non-PFlow jets for now
        from TriggerMenuMT.HLT.Jet.JetRecoCommon import cleaningDict
        for key,cleanWP in cleaningDict.items(): jetDef.modifiers.append(f"Cleaning:{cleanWP}")

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+jetDef.fullname()+"/")
    copyCalibAlg = JetRecConfig.getJetCopyAlg(jetsin=jetsNoCalib,jetsoutdef=jetDef,decorations=decorList,monTool=monTool)
    recoSeq += copyCalibAlg

    # Check conditions before adding fast flavour tag info to jets
    jetCalibDef=JetRecoCommon.getJetCalibDefaultString(jetRecoDict)
    if(
        configFlags.Trigger.Jet.fastbtagPFlow
        and JetRecoCommon.isPFlow(jetRecoDict)   # tag only PFlow jets
        and jetRecoDict['recoAlg']=='a4'         # tag only anti-kt with R=0.4
        and jetRecoDict['constitMod']==''        # exclude SK and CSSK chains
        and jetRecoDict['jetCalib']==jetCalibDef # exclude jets with not full default calibration
    ):

        # Adding Fast flavor tagging
        jetFTagSeq = getFastFlavourTaggingSequence(
            configFlags,
            "jetFtagSeq_"+jetRecoDict["trkopt"],
            jetDef.fullname(),
            jetRecoDict["Vertices"],
            jetRecoDict["Tracks"],
            isPFlow=True,
        )
        recoSeq += jetFTagSeq

    jetViewAlg, jetsOut = getJetViewAlg(configFlags,jetsIn=jetDef.fullname(),**jetRecoDict)
    recoSeq += jetViewAlg

    # End of basic jet reco
    return recoSeq, jetsOut, jetDef

# Calo cell unpacking and topocluster reconstruction
def jetClusterSequence(configFlags, RoIs, clusterCalib):

    # Start by adding the topocluster reco sequence
    if clusterCalib == "em":
        topoClusterSequence, clustersKey = RecoFragmentsPool.retrieve(
                caloClusterRecoSequence, flags=configFlags, RoIs=RoIs)
    elif clusterCalib == "lcw":
        topoClusterSequence, clustersKey = RecoFragmentsPool.retrieve(
                LCCaloClusterRecoSequence, flags=configFlags, RoIs=RoIs)
    else:
        raise ValueError("Invalid value for calib: '{}'".format(clusterCalib))

    return topoClusterSequence, clustersKey

# This sets up the reconstruction starting from topoclusters.
# No tracking is permitted.
def jetCaloRecoSequences( configFlags, RoIs, **jetRecoDict ):
    if JetRecoCommon.doTracking(jetRecoDict):
        raise ValueError("Calorimeter jet reco called with a tracking option!")

    log.debug("Generating jetCaloRecoSequences for configuration %s",JetRecoCommon.jetRecoDictToString(jetRecoDict))

    # Get the topocluster reconstruction sequence
    topoClusterSequence, clustersKey = RecoFragmentsPool.retrieve(
        jetClusterSequence, configFlags, RoIs=RoIs, clusterCalib=jetRecoDict["clusterCalib"])

    # Get the jet reconstruction sequence including the jet definition and output collection
    jetRecoSeq, jetsOut, jetDef  = RecoFragmentsPool.retrieve(
        jetRecoSequence, configFlags, clustersKey=clustersKey, **jetRecoDict )

    return [topoClusterSequence,jetRecoSeq], jetsOut, jetDef, clustersKey

# This function is for conversion of flavour-tagging algorithms from new to old-style
def getFastFlavourTaggingSequence( dummyFlags, name, inputJets, inputVertex, inputTracks, 
                                   addAlgs=[], isPFlow=False):

    with ConfigurableRun3Behavior():
        ca_ft_algs = getFastFlavourTagging( dummyFlags, inputJets, inputVertex, inputTracks, isPFlow)

    # 1) We need to do the algorithms manually and then remove them from the CA
    #
    # Please see the discussion on
    # https://gitlab.cern.ch/atlas/athena/-/merge_requests/46951#note_4854474
    # and the description in that merge request.

    ft_algs = [conf2toConfigurable(alg) for alg in findAllAlgorithms(ca_ft_algs._sequence)]
    jetFFTSeq = parOR(name, addAlgs+ft_algs)

    # you can't use accumulator.wasMerged() here because the above
    # code only merged the algorithms. Instead we rely on this hacky
    # looking construct.
    ca_ft_algs._sequence = []
    # 2) the rest is done by the generic helper
    # this part is needed to accomodate parts of flavor tagging that
    # aren't algorithms, e.g. JetTagCalibration.
    appendCAtoAthena(ca_ft_algs)

    return jetFFTSeq

# Returns reco sequence for full scan track & primary vertex reconstruction
def JetFSTrackingSequence(dummyFlags,trkopt,RoIs):

    IDTrigConfig = getInDetTrigConfig( 'jet' )

    trackcollmap = None

    viewAlgs = makeInDetTrigFastTrackingNoView( config = IDTrigConfig, rois=RoIs)

    # add the collections for the eflowRec reconstriction in the trigger
    trackvtxcontainers[trkopt] =  ( IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex_jet ) 

    vtxAlgs = makeInDetTrigVertices( "jet", IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex_jet, IDTrigConfig, IDTrigConfig.adaptiveVertex_jet )

    # now run the actual vertex finders and TTVA tools
    if IDTrigConfig.vertex_jet != IDTrigConfig.vertex:
        vtxAlgs += makeInDetTrigVertices( "amvf", IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex, IDTrigConfig, IDTrigConfig.adaptiveVertex )

    jetTrkSeq = parOR(f"JetFSTracking_{trkopt}_RecoSequence", viewAlgs+vtxAlgs)
    trackcollmap = jetTTVA( "jet", jetTrkSeq, trkopt, IDTrigConfig, verticesname=IDTrigConfig.vertex_jet,  adaptiveVertex=IDTrigConfig.adaptiveVertex_jet )

    return jetTrkSeq, trackcollmap


def getFastFtaggedJetCopyAlg(dummyFlags,jetsIn,jetRecoDict):

    caloJetRecoDict = JetRecoCommon.jetRecoDictFromString(jetsIn)
    caloJetDef = JetRecoCommon.defineJets(caloJetRecoDict,clustersKey=JetRecoCommon.getClustersKey(caloJetRecoDict),prefix=jetNamePrefix,suffix='fastftag')
    decorList = JetRecoCommon.getDecorList(jetRecoDict)
    copyJetAlg = JetRecConfig.getJetCopyAlg(jetsin=jetsIn,jetsoutdef=caloJetDef,decorations=decorList)
    ftaggedJetsIn = caloJetDef.fullname()
    return copyJetAlg,ftaggedJetsIn

# Returns reco sequence for RoI-based track reco + low-level flavour tagging 
def JetRoITrackJetTagSequence(dummyFlags,jetsIn,trkopt,RoIs):

    IDTrigConfig = getInDetTrigConfig( 'jetSuper' )

    viewAlgs, viewVerify = makeInDetTrigFastTracking( config = IDTrigConfig, rois=RoIs)
    viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs ),( 'xAOD::JetContainer' , 'StoreGateSvc+%s' % jetsIn)]

    IDTrigConfig = getInDetTrigConfig('jetSuper')
    tracksIn = IDTrigConfig.tracks_FTF()

    jetTrkSeq=getFastFlavourTaggingSequence(
        dummyFlags,
        f"JetRoITrackJetTag_{trkopt}_RecoSequence",
        jetsIn,
        "",
        tracksIn,
        addAlgs=viewAlgs,
    )

    return jetTrkSeq

# This sets up the reconstruction where full scan tracks are required.
# Topoclustering will not be scheduled, we just pass in the name of the cluster collection.
def jetTrackingRecoSequences(configFlags, RoIs, clustersKey, **jetRecoDict):
    if not JetRecoCommon.doFSTracking(jetRecoDict):
        raise ValueError("Jet reco with tracks called without full scan tracking option 'ftf'!")

    log.debug("Generating jetTrackingRecoSequences for configuration %s",JetRecoCommon.jetRecoDictToString(jetRecoDict))

    # Get the track reconstruction sequence
    (jetTrkSeq, trkcolls) = RecoFragmentsPool.retrieve(
        JetFSTrackingSequence, configFlags, trkopt=jetRecoDict["trkopt"], RoIs=RoIs)

    # Get the jet reconstruction sequence including the jet definition and output collection
    # Pass in the cluster and track collection names
    jetRecoSeq, jetsOut, jetDef  = RecoFragmentsPool.retrieve(
        jetRecoSequence,
        configFlags, clustersKey=clustersKey, **trkcolls, **jetRecoDict )

    return [jetTrkSeq,jetRecoSeq], jetsOut, jetDef

# Grooming needs the ungroomed jets to be built first,
# so call the basic jet reco seq, then add a grooming alg
def groomedJetRecoSequence( configFlags, dataSource, clustersKey, **jetRecoDict ):
    jetDefString = JetRecoCommon.jetRecoDictToString(jetRecoDict)
    recoSeq = parOR( "JetGroomSeq_"+jetDefString, [])

    ungroomedJetRecoDict = dict(jetRecoDict)
    ungroomedJetRecoDict["recoAlg"] = ungroomedJetRecoDict["recoAlg"].rstrip("tsd") # Drop grooming spec
    ungroomedJetRecoDict["jetCalib"] = "nojcalib" # No need to calibrate

    # Only jet building -- we do jet calib in a larger sequence via copy+calib
    (ungroomedJetBuildSequence,ungroomedJetsName,ungroomedDef) = RecoFragmentsPool.retrieve(
        standardJetBuildSequence,
        configFlags, dataSource=dataSource, clustersKey=clustersKey,
        **ungroomedJetRecoDict)
    recoSeq += ungroomedJetBuildSequence

    groomDef = JetRecoCommon.defineGroomedJets(jetRecoDict,ungroomedDef)
    groomedJetsFullName = groomDef.fullname()
    groomDef.modifiers = JetRecoCommon.getCalibMods(jetRecoDict,dataSource)
    groomDef.modifiers += ["Sort","Filter:"+str(JetRecoCommon.getFilterCut(jetRecoDict["recoAlg"]))]
    # Can add substructure mods here

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+groomedJetsFullName+"/")

    groomDef = solveGroomingDependencies(groomDef)
    groomalg = JetRecConfig.getJetRecGroomAlg(groomDef,monTool)
    recoSeq += conf2toConfigurable( groomalg )

    jetsOut = recordable(groomedJetsFullName)
    return recoSeq, jetsOut, groomDef


# Reclustering -- call the basic jet reco and add this to the sequence,
# then add another jet algorithm to run the reclustering step
def reclusteredJetRecoSequence( configFlags, dataSource, clustersKey, **jetRecoDict ):
    jetDefString = JetRecoCommon.jetRecoDictToString(jetRecoDict)
    recoSeq = parOR( "JetReclusterSeq_"+jetDefString, [])

    basicJetRecoDict = dict(jetRecoDict)
    basicJetRecoDict["recoAlg"] = "a4" # Standard size for reclustered
    (basicJetRecoSequence,basicJetsFiltered, basicJetDef) = RecoFragmentsPool.retrieve(
        standardJetRecoSequence,
        configFlags, dataSource=dataSource, clustersKey=clustersKey,
        **basicJetRecoDict)
    recoSeq += basicJetRecoSequence

    rcJetPtMin = 15 # 15 GeV minimum pt for jets to be reclustered
    jetViewAlg, filteredJetsName = getJetViewAlg(configFlags,jetsIn=basicJetDef.fullname(),jetPtMin=rcJetPtMin,**jetRecoDict)
    recoSeq+=jetViewAlg

    rcJetDef = JetRecoCommon.defineReclusteredJets(jetRecoDict, filteredJetsName, basicJetDef.inputdef.label, jetNamePrefix, '_'+jetRecoDict["jetCalib"])
    rcModList = [] # Could set substructure mods
    rcJetDef.modifiers = rcModList

    rcConstitPJAlg = JetRecConfig.getConstitPJGAlg( rcJetDef.inputdef, suffix=jetDefString)
    rcConstitPJKey = str(rcConstitPJAlg.OutputContainer)
    recoSeq += conf2toConfigurable( rcConstitPJAlg )

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+rcJetDef.fullname()+"/")

    rcJetDef._internalAtt['finalPJContainer'] = rcConstitPJKey
    # Depending on whether running the trackings step
    ftf_suffix = "" if not JetRecoCommon.doFSTracking(jetRecoDict) else "_ftf" 
    rcJetRecAlg = JetRecConfig.getJetRecAlg(rcJetDef, monTool, ftf_suffix)
    recoSeq += conf2toConfigurable( rcJetRecAlg )

    jetsOut = recordable(rcJetDef.fullname())
    jetDef = rcJetDef
    return recoSeq, jetsOut, jetDef

# VR track jets reconstruction sequence
def VRJetRecoSequence(configFlags, trkopt):
    recoSeq = parOR("VRJetRecSeq", [])
    VRTrackJetDef = JetRecoCommon.defineVRTrackJets(Rmax=0.4, Rmin=0.02, VRMassScale=30000, Ptmin=4000, prefix=jetNamePrefix, suffix="")
    VRTrackJetName = VRTrackJetDef.fullname()
    VRTrackJetDef = solveDependencies(VRTrackJetDef)
    constitPJAlg = JetRecConfig.getConstitPJGAlg(VRTrackJetDef.inputdef)
    recoSeq += conf2toConfigurable(constitPJAlg)
    finalpjs = str(constitPJAlg.OutputContainer)
    VRTrackJetDef._internalAtt['finalPJContainer'] = finalpjs
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+VRTrackJetName+"/")
    VRTrackJetRecAlg = JetRecConfig.getJetRecAlg(VRTrackJetDef,  monTool)
    recoSeq += conf2toConfigurable(VRTrackJetRecAlg)
    jetsOut = recordable(VRTrackJetName)
    jetDef = VRTrackJetDef
    return recoSeq, jetsOut, jetDef

# This sets up the reconstruction starting from calo towers for heavy ion events.
def jetHICaloRecoSequences( configFlags, RoIs, **jetRecoDict ):
    if jetRecoDict["ionopt"] == "noion":
        raise ValueError("Heavy-ion calorimeter jet reco called without a ion option!")

    # Get the tower reconstruction sequence 
    from .JetHIConfig import jetHIClusterSequence
    jetHIClusterSequence, clustersKey, towerKey = RecoFragmentsPool.retrieve(
        jetHIClusterSequence, configFlags, ionopt=jetRecoDict["ionopt"], RoIs=RoIs)

    # Get the jet reconstruction sequence including the jet definition and output collection
    from .JetHIConfig import jetHIRecoSequence
    jetHIRecoSeq, jetsOut, jetDef  = RecoFragmentsPool.retrieve(
        jetHIRecoSequence, configFlags, clustersKey=clustersKey, towerKey=towerKey, **jetRecoDict )

    return [jetHIClusterSequence,jetHIRecoSeq], jetsOut, jetDef, clustersKey
