# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetRec.JetAlgConfiguration import checkAndUpdateOptions
from AthenaCommon.SystemOfUnits import *
from JetRec.JetRecFlags  import jetFlags
from AthenaCommon.AlgSequence import AlgSequence

class memoize(object):
    """memoize is intended to be used as a decorator
    It will force the decorated function to be run only once, cache it's return value
    and return this value on every subsequent call.
    """
    def __init__(self, func):
        # func is the function being decorated
        self.func = func
        self.cache = None
        
    def __call__(self):
        # this gives the memoize object the ability to be called
        #  and thus this object behaves like a function
        
        if self.cache is None:
            # call the function only once.
            self.cache = self.func()
        return self.cache



def getJetVertexAssociationTool( toolName="JetVertexAssociation", *largs, **args):

    #options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input,)
    
    from JetMomentTools.JetMomentToolsConf import JetVertexAssociationTool, JetOriginCorrectionTool
    

    jvaTool = JetVertexAssociationTool(toolName)

    jvaTool.AssociateToHighestJVF = True

    from JetRec.TrackSelectionForJets import getDefaultJetVtxTrackHelper
    jvaTool.JetVtxTrackHelper = getDefaultJetVtxTrackHelper()
    
    # origin correction tool
    origCorr = JetOriginCorrectionTool("JetOriginCorrection")
    origCorr.CellCalibrator = None
    origCorr.UseGCW = False
    origCorr.UseClusters = True
    
    # Official Choice of Jet/MET Group : Primary Vertex
    origCorr.UseJVA           = False
    origCorr.UsePrimaryVertex = True
    origCorr.UseBeamSpot      = True
    jvaTool.OriginCorrectionTool = origCorr

    jvaTool.StoreTrkMoments = False

    jvaTool.sumPtTrkMomentName = "sumPtTrk_pv0_500MeV"
    jvaTool.nTrkMomentName = "nTrk_pv0_500MeV"
    jvaTool.TrackAssocName = "TrackAssoc"
    
    return jvaTool

def getJetTracksMomentTool(**options):
    
    from JetMomentTools.JetMomentToolsConf import JetTracksMoments
    from JetRec.TrackSelectionForJets import getDefaultJetVtxTrackHelper

    suffix = options.get("MomentSuffix", None)
    if suffix is None:
        minpt = options.get("MinPt", 1*GeV)
        if minpt>=GeV :
            suffix = '_%1.1fGeV'%(minpt/GeV,)
            suffix = suffix.replace('.0','') # don't write '1.0GeV', simply '1GeV'
        else:
            suffix = '_%dMeV'%(int(minpt),)
        options["MomentSuffix"] = suffix
    defaultJVtx = getDefaultJetVtxTrackHelper()
    options.setdefault('JetVtxTrackHelper',defaultJVtx)  # update the JetVtxTrackHelper option if not given
    trkTool = JetTracksMoments(
        **options
        )
    return trkTool

def getJetClusterMomentTool(toolName="JetClusterMoments"):
    from JetMomentTools.JetMomentToolsConf import JetClusterMomentsTool
    return JetClusterMomentsTool(toolName)


def getORIGINMoments(jetcollname):
    from JetCalibTools.MakeCalibSequences import alternateCalibSequence 
    from JetMomentTools.JetMomentToolsConf import JetMomentsFromCalib 
    from JetMomentTools.GhostAssociation import interpretJetName 
    from JetRec.JetAlgConfiguration import checkAndUpdateOptions 
    recognizedJet = interpretJetName(jetcollname) 
    if recognizedJet is not None: 
        finder, input, mainParam = recognizedJet  
        calib = 'LC:ORIGIN' if 'LC' in input else 'EM:ORIGIN' 
        d = checkAndUpdateOptions(input=input) 
    return JetMomentsFromCalib( "OriginRecalculator" , Calibrator = alternateCalibSequence(calib, d) )
    

def getDefaultMissingCellTool(rmax=1.0):
    rmax = max(rmax, 1.0)
    if getDefaultMissingCellTool.cache is None:
        from JetMomentTools.JetMomentToolsConf import MissingCellListTool
        from AthenaCommon.AppMgr import ToolSvc

        tool = MissingCellListTool()
        tool.DeltaRmax = rmax
        tool.AddBadCells = True
        getDefaultMissingCellTool.cache = tool
        tool.RemoveCellList = [1275120944, 1275121456, 1275137328, 1275268400, 1275268912, 1275383088, 1275383600, 1275514160, 1275514672, 1275628848, 1275661616, 1275662128, 1275792688, 1275793200, 1275940144, 1275940656, 1275956528, 1276054832, 1276055344, 1276071216, 1283493168, 1283493680, 1283640624, 1283641136, 1283820848, 1283821360, 1283935536, 1283936048, 1284017456, 1284017968, 1284164912, 1284165424, 1284312368, 1284312880, 1284427056, 1284427568, ]
        #1275349024, 1283688480 # these tile cells were not ignored in the original implementation


        ToolSvc += tool
    else:
        if rmax > getDefaultMissingCellTool.cache.DeltaRmax:
            getDefaultMissingCellTool.cache.DeltaRmax = rmax
        
    return getDefaultMissingCellTool.cache
getDefaultMissingCellTool.cache = None


def getJetBadChanCorrTool(finder, mainParam, input, **options):

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    from JetMomentTools.JetMomentToolsConf import JetBadChanCorrTool
    from CaloClusterCorrection.StandardCellWeightCalib import getCellWeightTool

    cellcalibtool = getCellWeightTool(options['finder'], options['mainParam'], input, onlyCellWeight=True)
    coneSize = options['mainParam']

    BadChanCorrT = JetBadChanCorrTool("JetBadChanCorrTool")
    # Explicitly list flags
    BadChanCorrT.ConeDr         = coneSize 
    BadChanCorrT.CellCalibrator = cellcalibtool
    BadChanCorrT.UseCalibScale  = False
    BadChanCorrT.AttachCorrCell = True
    BadChanCorrT.AttachCorrDOTX = True
    BadChanCorrT.AttachCorrJet  = True

    BadChanCorrT.MissingCellTool = getDefaultMissingCellTool(coneSize)
    # Optional
    BadChanCorrT.AttachCorrJetForCell = False
    if options['addJetQualityMoments'] == True:        
        BadChanCorrT.AttachCorrJetForCell = True

    # new setup for THistService
    import os
    dataPathList = os.environ[ 'DATAPATH' ].split(os.pathsep)
    dataPathList.insert(0, os.curdir)
    from AthenaCommon.Utils.unixtools import FindFile
    RefFileName = FindFile( "JetBadChanCorrTool.root" ,dataPathList, os.R_OK )


    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr(ServiceMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()
    ServiceMgr.THistSvc.Input += ["JetBadChanCorrTool DATAFILE=\'%s\' OPT=\'READ\'" % RefFileName]

    return BadChanCorrT

def getCaloQualityTool(computeFromCluster = False,doCellBasedVars = False,addJetQualityMoments=False):

    from JetMomentTools.JetMomentToolsConf import JetCaloQualityTool
    caloQualityT = JetCaloQualityTool("JetCaloQualityTool")
    caloQualityT.doCellBasedVariables = doCellBasedVars
    caloQualityT.cutOnLAr = 4000
    caloQualityT.doLArQuality = True
    caloQualityT.doTiming = True
    caloQualityT.doHECQuality = True
    caloQualityT.doNegativeE = True
    caloQualityT.doTileQuality = False
    caloQualityT.doAverageLArQF = True        
    #caloQualityT.timingMomentCells     = [5,10]  # produces jet moments: ootFracCells5, ootFracCells10 (names built on the fly from caloQualityT.timingMomentCellsName)
    caloQualityT.timingMomentClusters  = [5,10]  #
    caloQualityT.timingMomentCells = []
    caloQualityT.doJetCentroid = True
    
    caloQualityT.ComputeVariableFromCluster = computeFromCluster

    if addJetQualityMoments :
        caloQualityT.doConstituentBasedVariables = True
        caloQualityT.doSamplingBasedVariables = True
        caloQualityT.cutOnTile = 254
        caloQualityT.doTileQuality = True


    return caloQualityT


def getJetIsolationTool(toolname,**options):
    from JetMomentTools.JetMomentToolsConf import JetIsolationTool

    default_options = dict(JetRParam = 0.4,
                           IsolationMoments = ["IsoKR","IsoDelta", "IsoFixedCone", "IsoFixedArea", "Iso6To8"],
                           IsolationParam = [ 2., 0.2, 0.8, 1.4, 0.8],
                           DoIsoSumPt = True,
                       )
    default_options.update(options)

    
    isot = JetIsolationTool(toolname, **default_options)                            
    return isot

@memoize
def _getTruthJetIsoSelector():
    from JetSimTools.JetSimToolsConf import JetTruthParticleSelectorTool
    from AthenaCommon.AppMgr import ToolSvc
    
    selector = JetTruthParticleSelectorTool("JetTruthIsoSelector")
    selector.includeMuons = False
    selector.useInteractingParticlesOnly = True

    ToolSvc += selector
    return selector
    
def getTruthJetIsolationTool(toolname, doTruthPileup=False, **options):
    """We need special options for truth jets.
    """
    isot = getJetIsolationTool(toolname, **options)

    from RecExConfig.RecFlags import rec
    defaultCollName = "SpclMC"  if rec.readAOD()  else "INav4MomTruthEvent"
    defaultContList = [defaultCollName] if not doTruthPileup else [defaultCollName, defaultCollName+'INTIME']
    truthCont = options.get('TruthInputContainers', defaultContList)

    isot.TruthInputContainers = truthCont
    
    # make sure we have a truth jet selector.
    isot.TruthSelector = _getTruthJetIsoSelector()

    return isot

def getJetTruthLabelTool(truthCont='TruthInputContainers') :
    from JetMomentTools.JetMomentToolsConf import JetTruthLabel

    tool = JetTruthLabel("JetTruthLabel")
    tool.TruthInputContainers = truthCont

    return tool

def getsubJetsMomentTool(toolname="KtDeltaR"):
    from JetMomentTools.JetMomentToolsConf import JetSubJetsMoments
    from JetRec.JetFastJetConfig import  configFastJet
    
    ktDrTool = JetSubJetsMoments(toolname)
    fjtool = configFastJet("Kt",0.4, Inclusive=False, ExclusiveNjets=2, ExclusiveDcut=-1)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += fjtool
    ktDrTool.FastJetTool = fjtool
    ktDrTool.FromJetAssociation = "TrackAssoc"
    ktDrTool.MinPt = 1*GeV
    return ktDrTool



def getMatchingFractionTool(jetcollname, matchType, seq=AlgSequence()):
    """ ATTENTION this tools returns (tool, alg) where alg is a dependency for  Matching.
    It *has* to be scheduled before (caller's reponsability).
    It return None if it can not guess what truth alg to request.
    """
    from JetMomentTools.GhostAssociation import interpretJetName
    from JetMomentTools.JetMomentToolsConf import JetMatchingFractionTool
    recognizedJet = interpretJetName(jetcollname)
    if recognizedJet is None:
        return None

    finder, input, mainParam = recognizedJet
    algtype = finder+str(int(mainParam*10))        
    from JetRec.JetGetters import make_StandardJetGetter
    if matchType == "Truth":
        from JetRec.JetRec_defaults import standardJetAlgorithm
        alg = standardJetAlgorithm(algtype+"TruthJets")
        if alg is None: # possible if truth collection already exist
            # !!!! WARNNIG minPt cut is ambiguous !!!!
            alg=make_StandardJetGetter(finder,  mainParam,"Truth",  minPt=4*GeV, jetMoments=[], outputCollectionName=algtype+"TruthNewJets", seq=seq).jetAlgorithmHandle()

        # Then the mftool can be scheduled
        mftool = JetMatchingFractionTool("TruthMF",MFCollection = alg.JetCollectionName, AssociatedConstitKey="TruthAssoc",
                                         MomentTag="Truth",UseHighestShareFrac=True)
    elif matchType == "Track":
        alg=make_StandardJetGetter(finder,  mainParam,"TrackZ",   addJetQualityMoments=False,  seq=seq).jetAlgorithmHandle()        
        # Then the mftool can be scheduled
        mftool = JetMatchingFractionTool("TrackMF",MFCollection = alg.JetCollectionName, AssociatedConstitKey="TrackAssoc",
                                         MomentTag="Track",UseHighestShareFrac=True)
    else:
        return None

    return (mftool, alg)

def getJetLArHVMomentTool():

    from LArRecUtils.LArHVCorrToolDefault import LArHVCorrToolDefault
    theLArHVCorrTool=LArHVCorrToolDefault()
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+=theLArHVCorrTool

    #Now declare the jet moment algorithm:
    from JetMomentTools.JetMomentToolsConf import JetLArHVMoment
    JetLArHV = JetLArHVMoment("JetLArHVMoment")
    JetLArHV.HVCorrTool = theLArHVCorrTool
    return JetLArHV
