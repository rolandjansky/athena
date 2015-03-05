# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
A set of helper function to configure calculations of additionnal jet moments.

"""
from JetRec.JetAlgConfiguration import checkAndUpdateOptions, standardJetOption
from RecExConfig.RecFlags import rec
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.AlgSequence import AlgSequence    
from AthenaCommon.Logging import logging
from AthenaCommon.JobProperties import jobproperties

from JetRec.JetRecFlags import jetFlags 
from JetRec.JetFinderConfig import setupAreaCalculation    

from JetMomentTools.SetupJetMomentTools import *


## A dedicated logger
_momlog = logging.getLogger( 'JetMomentConfig' )


# We try to schedule cpu intensive moment calculation only once. This requires to keep track of
# moments already scheduled for a given collection. Use this dictionnary.
_specialMomentAlgDict = {}

# Some moments request can translate in modification of option to already scheduled moment tool.
# so we keep only one main algorithm (JetAlgorithm or JetMomentCalculator) per jet collection, in order to be able to
# retrieve & modify the relevant moment tool. 
# However this can cause dependency issue :
# for example, truth ghostAssoc requires JetTruthParticleFilter to run before. If a 1st call doesn't require truth ghost assoc, then later a 2nd call does, then JetTruthParticleFilter will be scheduled *after* the moment calculator alg...
# workaround : move by jand JetTruthParticleFilter at a proper place in the topSequence or the PreD3PDSequence.
def moveAlgInSequence(alg, seq, newpos):
    l = seq.getChildren()
    delattr(seq, alg.getName())
    seq.insert(newpos, alg)


def checkRequiredAlgPosition(seq, momAlg, requiredAlg):
    topSeq = AlgSequence()
    
    if requiredAlg in topSeq:
        truthInd = topSeq.getChildren().index(requiredAlg)
        if momAlg in topSeq:
            index = topSeq.getChildren().index(momAlg)
        else:
            # then it is in seq, and seq must be in topSeq
            index = topSeq.getChildren().index(seq)
        # make sure requiredAlg is before index
        if truthInd > index:
            moveAlgInSequence(requiredAlg, topSeq, index)
    else:
        # will move requiredAlg in topSeq before seq and momAlg
        # where is momAlg ?
        seqInd = topSeq.getChildren().index(seq)
        if momAlg in topSeq:
            index = topSeq.getChildren().index(momAlg)
        else:
            index = seqInd
        delattr(seq, requiredAlg.getName())
        topSeq.insert(index, requiredAlg)
    


def checkTruthParticleFilterPosition(seq, momAlg):
    from JetSimTools.JetTruthParticleFilterGetter import JetTruthParticleFilterGetter
    truthAlg = JetTruthParticleFilterGetter().alg
    checkRequiredAlgPosition(seq, momAlg, truthAlg)
    
    

def retrieveFinderTool(toolSeq):
    from JetRec.JetRecConf import JetConeFinderTool, JetFastJetFinderTool
    for t in toolSeq:
        if isinstance(t, JetFastJetFinderTool) or isinstance(t, JetConeFinderTool):
            return t


import inspect
def whosdaddy():
    l = [ ll[3] for ll in  inspect.stack()[2:] ]
    return l





# Helper functions to schedule jet moments.
# They are typically wrappers around functions in SetupJetMomentTools which are as generic as possible
#  The wrappers below also perform tasks related to the particular jet collection or configuration sequence. 
def scheduleMFTool(toolType,jetcollname, jf, alg, seq ,*l):
    from JetMomentTools.SetupJetMomentTools import getMatchingFractionTool
    tool, requiredAlg = getMatchingFractionTool(jetcollname, toolType, seq)
    checkRequiredAlgPosition(seq, alg, requiredAlg)
    return tool

from JetMomentTools.JetMomentToolsConf import JetWidthTool, NumTowerInJetTool, JetEinSamplingTool


# The dictionnary below lists the known standard moments and how to compute them.
# keys in the dict  are shorcuts, they might represent several moments
# values are function used to actually schedule the moment calculations 
defaultMoments = {
    'ghostAssoc'   : lambda *l : None,
    'area'         : lambda jcoll,jetfinder,*l : setupAreaCalculation(jetfinder, "ActiveArea", addArea4Vec=True, addSecondArea=jetFlags.jetPerformanceJob() ) ,
    'quality'      : lambda jetcollname,*l : getCaloQualityTool(doCellBasedVars = (jetcollname=="AntiKt4TopoEMJets" or "Tower" in jetcollname ), computeFromCluster = "Tower" not in jetcollname ),
    'ktDeltaR'     : lambda *l : getsubJetsMomentTool(),
    'isolation'    : lambda *l : getJetIsolationTool("JetIsolation"),
    'jvf'          : lambda jetcollname,*l : getJetVertexAssociationTool(toolName=jetcollname+'JVAtool')    ,
    'jvt'          : lambda jetcollname,*l : getJetVertexAssociationTool(toolName=jetcollname+'JVAtool')    ,
    'trackMoments' : lambda *l : getJetTracksMomentTool(),
    'clusMoments'  : lambda *l : getJetClusterMomentTool(),
    'truthMF'      : lambda *l : scheduleMFTool("Truth",*l) ,
    'trackMF'      : lambda *l : scheduleMFTool("Track",*l) ,
    'larHV'        : lambda *l : getJetLArHVMomentTool(),
    'width'        : lambda *l : JetWidthTool(),
    'eInSampling'  : lambda *l : JetEinSamplingTool(),
    'numTowerJet'  : lambda *l : NumTowerInJetTool(),
    'badChanCorr'  : lambda jc,jf,alg,seq,jetAlgConfigDict : getJetBadChanCorrTool(**jetAlgConfigDict),
    'origin'       : lambda jetcollname,*l : getORIGINMoments(jetcollname),
    }


## The list of standard moments keys used in reconstruction.
standardMoments = standardJetOption("jetMoments")

class TestWithMsg(object):
    """Simply a boolean and a str message explaining why the boolean is False
    Also supports & and | operation (logical and or) with concatenation of message"""
    jetcollname=""
    def __init__(self, test, failmsg):
        self.test, self.failmsg= test, failmsg
    def check(self, mName):
        if self.test:
            return True
        _momlog.info( "No "+mName+" moments for "+self.jetcollname+". Reason : "+self.failmsg)
        return False
    def __and__(self, o ):
        test = self.test and o.test
        failmsg = [ t.failmsg for t in (self,o) if not t.test ]
        return TestWithMsg( test, ' AND '.join(failmsg) )
    def __or__(self, o ):
        test = self.test or o.test
        failmsg = '' if test else self.failmsg+' AND '+o.failmsg
        return TestWithMsg( test, failmsg )


def checkMomentAvailability(jetcollname, momentList, jetAlgConfigDict={}):
    """ Input :  a jet collection name (jetcollname), list of moment keys (momentList), a jet alg configuration dict (as in JetRec.JetAlgConfiguration)
    returns a filtered list of moment keys where moments incompatible with the jet collection or the job conditions are removed.
    """
    # we'll need to know what the input file is
    from JetRec.JetGetters import guessInputFileType
    inputFileType = guessInputFileType()



    from AthenaCommon.DetFlags import DetFlags
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.GlobalFlags  import globalflags
    from JetRec.TrackSelectionForJets import tracksAvailableForJets

    TestWithMsg.jetcollname = jetcollname
    # short cut flags
    # use TestWithMsg rather than simply bool only to be able to display a reason for the rejection of the moment.
    accessCells = TestWithMsg( (inputFileType=="RDO") or (inputFileType=="ESD") , "No CaloCell available") 
    caloJet     = TestWithMsg( ("Topo" in jetcollname) or ("Tower" in jetcollname), "Not calo jets")
    topoJet     = TestWithMsg( "Topo" in jetcollname, "Not TopoCluster jets")
    lctopoJet   = TestWithMsg( "LCTopo" in jetcollname, "Not LC TopoCluster jets")
    notTruthJet = TestWithMsg( 'Truth' not in jetcollname, "Is Truth jets")
    tracksAvailable = TestWithMsg( tracksAvailableForJets(), "Tracks selection for jets impossible")

    trueTest = TestWithMsg(True,"")

    # build the conditions at which moments can be built
    compatibilityDict = {
        'area'        : TestWithMsg('Cone' not in jetcollname, "no area for cone jets"),
        'ghostAssoc'  : notTruthJet,
        'width' : trueTest,
        'numTowerJet' : topoJet & accessCells,
        'quality'     : caloJet,
        'badChanCorr' : caloJet & accessCells,
        'constitCalib': lctopoJet &  TestWithMsg( not jetAlgConfigDict.get('calibName','').startswith("LC:CONST") , "Calibration differ from LC:CONST") ,
        'jvf'         : caloJet & tracksAvailable,
        'larHV'       : caloJet & TestWithMsg( (inputFileType=='RDO') and DetFlags.dcs.LAr_on() and globalflags.DataSource()=='data', "No proper LAr info available"),
        'eInSampling' : (caloJet & accessCells) | (topoJet), 
        'truthMF'     : TestWithMsg( rec.doTruth() , "Truth not available") & notTruthJet,
        'trackMF'     : notTruthJet & tracksAvailable,
        'trackMoments': notTruthJet & tracksAvailable,
        
        }

    # filter moments :
    availableMoments = set([ m for m in momentList if compatibilityDict.get(m, trueTest).check(m) ])
    return availableMoments
    

    

def findMainAlgForJetCollection(jetcollname, seq = AlgSequence()):
    """Given a JetCollection name (jetcollname) in input, returns an algorithm computing moments for this collection.
    The functions explore all know possibilities for finding such algs.

    The return value is (alg, set) where
     - alg is a JetAlgorithm or a JetMomentCalculator. alg can be None if nothing found or unusable in the case a JetGetter was called with the disable option.
     - set : is the set of known moments associated to the alg
    """
    # try to find a moment calculator for this collection :
    alg , existing_moments = _specialMomentAlgDict.get( jetcollname, (None,set()) )
    unknownAlg = alg is None

    if alg is None:
        # try to find a getter :
        from JetRec.JetGetters import retrieveConfiguredJetAlg
        alg  = retrieveConfiguredJetAlg(jetcollname, retrieveGetter=True)
        if alg is not None: 
            # check if usable :
            if not alg.usable():
                return ('unusable', set())
            
            # get the real JetAlgorithm
            alg = alg.jetAlgorithmHandle()
            # in this case, no moment have been associated yet
            existing_moments = set()
    if alg is None:
        # check if a collection exists in input file
        from RecExConfig.ObjKeyStore import objKeyStore
        if 'JetCollection#'+jetcollname in objKeyStore['inputFile'].list("JetCollection"):
            # then create a new moment alg for this collection
            from JetRec.JetMomentGetter import getJetMomentAlgorithm
            alg = getJetMomentAlgorithm(jetcollname, seq = seq)
        else:
            # likely to fail...
            # try to invoke a standard JetAlgorithm
            from JetRec.JetRec_defaults import standardJetAlgorithm
            try :
                alg = standardJetAlgorithm(jetcollname, seq = seq)
            except:
                alg = None #
    if unknownAlg and alg is not None:
        # we created a new alg, remember it
        _specialMomentAlgDict[jetcollname] = (alg, set() )
    return alg, existing_moments

from JetRec.JetRecConf import JetAlgorithm
from JetMomentTools.JetMomentToolsConf import JetMomentCalculator
def addStandardMoments(jetcollname, moments=standardMoments, seq = AlgSequence(), jetAlgConfigDict={}):
    """Add a list of jet moment calculation for a given jet collection
      - jetcollname : string, the name of the jetcollection
      - moments     : list of string or function. string are expected to be some of the defaultMoment shortcuts. functions must return a configured moment tool
      - seq         : the AlgSequence to which the calculation is attached
      (- jetAlgConfigDict : dictionnary, used to pass JetAlgorithm configuration options when this addStandardMoments is called while configuring the JetAlgorithm.)
      Returns nothing.
      Effect : retrieve (or invoke if needed) the moment calculation algorithm dedicated to jetcollname (this can be a JetAlgorithm or a JetMomentCalculator),
               then add to this algorithm the moments requested in defaultMoment (only if they are not already scheduled).
    """

    if moments == []:
        # nothing to do. Return now to avoid any inconvenience
        return 
    
    # retrieve the main algorithm for this jet collection
    alg, existing_moments = findMainAlgForJetCollection(jetcollname, seq)


    # check if the alg is usable ---------
    if alg == 'unusable':
        _momlog.info("No usable algorithm for "+jetcollname+". No jet moments added")
        return
    if alg is None :
        # didn't find any alg. We can't add moments
        _momlog.error("Can't create momens for "+jetcollname+" : no such collection in input or no algorithms scheduled for it")
        _momlog.error("  ---> possible solution, invoke make_StandardJetGetter() before to schedule a JetAlgorithm for "+jetcollname)
        return

    isJetMomentCalc = isinstance(alg, JetMomentCalculator)
    
    # filter moment list according to jet collection and other conditions
    missing_moments =  checkMomentAvailability( jetcollname, set(moments) - existing_moments , jetAlgConfigDict )

    # enforce ghostAssoc, thus jet refinding, if area requested (could separate the 2 options with more config) when using a JetMomentCalculator
    if 'area' in missing_moments and ('ghostAssoc' not in existing_moments) and isJetMomentCalc:
        missing_moments.add('ghostAssoc')


    jetfinder = None
    # ghost association is very particular since it needs jet finding.  
    if 'ghostAssoc' in missing_moments:
        gAssoc = []
        from JetRec.TrackSelectionForJets import tracksAvailableForJets
        if tracksAvailableForJets() :
            gAssoc += ["TrackAssoc"]
        if rec.doTruth():
            gAssoc+=[ "TruthAssoc"]
        if isJetMomentCalc:
            from JetMomentTools.GhostAssociation import setupGhostAssociationTool
            gtool = setupGhostAssociationTool(jetcollname, gAssoc, seq=seq)
            tmpL = alg.CalculatorTools
            alg.CalculatorTools = [gtool] + tmpL
        else: # it's a JetAlgorithm
            tmpL = list(alg.AlgTools)
            from JetRec.JetGetters import adaptToolListToGhostAssociation
            alg.AlgTools = adaptToolListToGhostAssociation(tmpL, gAssoc, seq)
        if rec.doTruth():
            checkTruthParticleFilterPosition( seq, alg)

        jetfinder = alg.CalculatorTools['JetGhostAsso'].JetFindingSequence["JetFastJetFinder"] if isJetMomentCalc \
                    else alg.AlgTools[0].JetFindingSequence["JetFastJetFinder"]

        # we are done with ghostAssoc
        missing_moments.remove('ghostAssoc')
        existing_moments.add('ghostAssoc')
    else: # still need to retrieve the jetfinder
        if 'ghostAssoc' in existing_moments:
            jetfinder = alg.CalculatorTools['JetGhostAsso'].JetFindingSequence["JetFastJetFinder"] if isJetMomentCalc \
                        else alg.AlgTools[0].JetFindingSequence["JetFastJetFinder"]
        else:
            jetfinder = retrieveFinderTool( alg.CalculatorTools if isJetMomentCalc else alg.AlgTools)
        
    momentTools = []
    # Loop over requested moment types and set-up the related tools
    for momentType in missing_moments:
        if callable(momentType):
            # then assume a function returning a tool has been supplied
            func = momentType
        else:
            func = defaultMoments[momentType]
        # call the setup function, add the returned tool to the sequence 
        tool =  func( jetcollname, jetfinder, alg, seq, jetAlgConfigDict)
        if tool is not None: # some function just adapts existing tools
            momentTools.append(tool)

    # add the moment tools list to the relevant alg :
    if isJetMomentCalc:
        alg.CalculatorTools += momentTools
    else:
        alg.AlgTools += momentTools
    
    _specialMomentAlgDict[jetcollname] = (alg, missing_moments.union(existing_moments) )
    return alg

    
        

def recommendedAreaAndJVFMoments(jetcollname, oldMomentNames=False,seq=AlgSequence()):
    """Recompute the area, JVF and track related jet moment on jetcollname.

    jetcollname : string a JetCollection StoreGate key
    oldMomentNames : bool, if True the moment naming will be the same as in 17.2.5 and previous releases.
    """

    alg = addStandardMoments(jetcollname,moments = ['ghostAssoc','area','jvf', 'trackMoments'],  seq=seq)

    return alg

def recommendedAreaAndJVF_StandardJetGetter( finder, mainParam, input, oldMomentNames=False,  **options):
    """Run a jet algorithm, adding the recomended  area, JVF and track related jet moments calculation.
    
    all argument similar as in make_StandardJetGetter except :
    
    oldMomentNames : bool, if True the moment naming will be the same as in 17.2.5 and previous releases.
    """
    return make_StandardJetAlg(finder, mainParam, input, jetMoments = ['ghostAssoc','area','jvf', 'trackMoments'], **options)
    #alg = specialJet( finder, mainParam, input, oldMomentNames = oldMomentNames, moments=['ghostAssoc','area','jvf', 'trackMoments'], **options)
