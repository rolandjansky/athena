## *****************************************************************************
## \file PartonTruthJets_jobOptions.py
## \author Eric Feng (Chicago) <Eric.Feng@cern.ch>
## \date June 2009
##
## \brief Job options fragment to build parton jets from JetsFromPartonsTool or
## JetPartonSelectorTool (preferred), as well as truth jets from their
## analogues.
##
## Generator-dependent -- you must specify the generator at runtime via
## doPythia or doHerwig flags (see below).
## *****************************************************************************

from JetRec.JetMomentGetter import make_JetMomentGetter
from JetRec.JetGetters import *  

from AthenaCommon.SystemOfUnits import MeV, GeV
from JetRec.JetRecConf import JetAlgorithm

from JetSimTools.JetSimToolsConf import JetsFromTruthTool
from JetSimTools.JetSimToolsConf import JetsFromPartonsTool
from JetSimTools.JetSimToolsConf import JetPartonSelectorTool

# WARNING:  Must set to True for generator of sample
doPythia = False       #Pythia
doHerwig = False       #Herwig (or Alpgen, MC@NLO, etc showered with Herwig)

mcEventKey = 'GEN_EVENT'    #EVNT (evgen)
# mcEventKey = 'GEN_AOD'      #AOD
# mcEventKey = 'TruthEvent'  #ESD

minJetPt = 7.*GeV
absEtaMax = 5

JetPartonSelectorTool = JetPartonSelectorTool("JetPartonSelectorTool")
JetPartonSelectorTool.DoPythia = doPythia
JetPartonSelectorTool.DoHerwig = doHerwig
JetPartonSelectorTool.max_absEta = absEtaMax
JetPartonSelectorTool.OutputLevel = INFO
ToolSvc += JetPartonSelectorTool


def jetsFromTruth( jetName ):  
    jetAlg = JetAlgorithm( "%sJetAlg" % jetName )
    truthtool = JetsFromTruthTool(
        "JetTruthTool",
        MinPt                = 0.*MeV,
        MaxEta               = absEtaMax,  #default = 5
        IncludeMuons         = False,
        TruthCollectionName  = mcEventKey,
        OutputCollectionName = "TruthJets",
        TruthType            = "Signal"
        )
    
    jetAlg += truthtool
    jetAlg.AlgTools = [ truthtool.getFullName() ]
    return jetAlg


def jetsFromPartons( jetName ):  #uses JetsFromPartonsTool
    jetAlg = JetAlgorithm( "%sJetAlg" % jetName )
    truthTool = JetsFromPartonsTool(
        "%sJetTool" % jetName,
        MinPt        = 0.*MeV,
        IncludeMuons = False,
        TruthCollectionName  = mcEventKey,
        OutputCollectionName = "%sJets" % jetName,
        TruthType            = "Signal",
        DoPythia               = doPythia,
        DoHerwig               = doHerwig
        )
    jetAlg += truthTool
    jetAlg.AlgTools = [ truthTool.getFullName() ]
    #jetAlg.JetFinalPtCut.MinimumSignal = minJetPt
    #jetAlg.AlgTools['JetFinalPtCut'].MinimumSignal = minJetPt
    return jetAlg


def getTruthJets(finder, size):  #uses JetTruthParticleSelectorTool
    truthJetAlg = make_StandardJetGetter(finder,size,'Truth').jetAlgorithmHandle()
    truthJetAlg.AlgTools['JetFinalPtCut'].MinimumSignal = minJetPt
    truthJetAlg.AlgTools['TruthLoader'].InputSelector.max_absEta = absEtaMax  #all three settings for max_absEta, minEta, maxEta are required
    truthJetAlg.AlgTools['TruthLoader'].InputSelector.minEta = -1.E6  #don't use asymmetric
    truthJetAlg.AlgTools['TruthLoader'].InputSelector.maxEta =  1.E6  #don't use asymmetric
    return truthJetAlg

def getPartonJets(finder, size):  #uses JetPartonSelectorTool
    partonJetAlg = make_StandardJetGetter(finder,size,'Truth',inputSuff='Parton').jetAlgorithmHandle()
    partonJetAlg.AlgTools['JetFinalPtCut'].MinimumSignal = minJetPt
    partonJetAlg.AlgTools['TruthLoader'].InputSelector = JetPartonSelectorTool
    partonJetAlg.OutputLevel = INFO
    return partonJetAlg


### Use JetsFromTruthTool & JetsFromPartonsTool
# topSequence += jetsFromTruth( "Truth" )
# topSequence += jetsFromPartons( "Parton" )

### Use JetTruthParticleSelectorTool & JetPartonSelectorTool
for finder in [ 'AntiKt', 'Kt', 'CamKt' ]:
    for size in [ 0.4, 0.6 ]:
        getPartonJets(finder, size)
        getTruthJets(finder, size)

for finder in [ 'Cone', 'SISCone' ]:
    for size in [ 0.4, 0.7 ]:
        getPartonJets(finder, size)
        getTruthJets(finder, size)

from JetRec.CBNTJets import schedule_CBNTJets_fromJetAlgs
excludeList = []
schedule_CBNTJets_fromJetAlgs(excludeList)
