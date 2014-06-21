# @file TauD3PDMaker/share/AnyToTauD3PD.py
# @author Dugan O'Neil <doneil@sfu.ca>
# @author Ryan Reece <ryan.reece@cern.ch>
# @author Michel Trottier-McDonald <mtm@cern.ch>
# @date Aug, 2009
# @brief Building TauD3PD from AOD
#
#There are several control flags at the top of this file. They can be
#set here or in a calling script. The calling script values take precedence
#if they are defined in globals(). Some of the important parameters:
# * poolFileInput = /path/to/input/files
# * perfSize = can make small, medium or large D3PDs. You can also make
#   multiple at once by using combinations like "smallmedium".
#   It is not case-sensitive.
# * EvtMax = max number of events to process (-1 is all)
# * doClusters = dumps 4-momenta for all clusters in the event. Adds ~5kB/event
# * doTracks = dumps all tracks in the event. Adds ~5kB/event
# * doHadrons = dumps all species of MC hadrons. doTruth must also be turned on. Adds ~7kB/event
# * doGRL = use (or not) a good run list
# * tupleFileOutput = prefix for the name of the D3PD file. 
# * tauDetailsLevel = mostly obsolete, replaced by perfSize above.
#
##############################################################################


#-----------------------------------------------------------------------------
# Options
#-----------------------------------------------------------------------------
myOptions={}

myOptions['poolFileInput']    = '/afs/cern.ch/user/v/vivek/public/datafiles/data11_7TeV.00178109.physics_Muons.merge.AOD.f351_m765._lb0846-lb0851._0001.1'
myOptions['perfSize']         = "SmallMedium"
myOptions['EvtMax']           = 50
myOptions['doTruth']          = False
myOptions['doClusters']       = True
myOptions['doTracks']         = True
myOptions['doHadrons']        = False
myOptions['doGRL']            = False
myOptions['myGRL']            = 'GRL_Tau_MayRepro.xml'
myOptions['myLBCollection']   = 'MyLBCollection'
myOptions['tauDetailsLevel']  = 10
myOptions['doSkim']           = False
myOptions['tupleFileOutput']  = "tauPerf"

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.FilterCollCand = False # turn true to filter only well-timed
D3PDMakerFlags.JetSGKey = 'AntiKt4LCTopoJets' #rel16 'AntiKt4TopoJets' should be tau seed
D3PDMakerFlags.DoTrigger = True
D3PDMakerFlags.RerunTauID = True
D3PDMakerFlags.DoTJVA = True

##############################################################################
# From this point down, only experts need to edit
##############################################################################

# Override options above if you define them in higher script globals 
for k,v in myOptions.items():
	if not k in globals().keys():
		myOptions[k]=v
	else:
		myOptions[k]=globals()[k]
	
poolFileInput   = myOptions['poolFileInput']
perfSize        = myOptions['perfSize']
EvtMax          = myOptions['EvtMax']
doTruth         = myOptions['doTruth']
doClusters      = myOptions['doClusters']
doTracks        = myOptions['doTracks']
doHadrons       = myOptions['doHadrons']
doGRL           = myOptions['doGRL']
myGRL           = myOptions['myGRL']
myLBCollection  = myOptions['myLBCollection']
tauDetailsLevel = myOptions['tauDetailsLevel']
doSkim          = myOptions['doSkim']
tupleFileOutput = myOptions['tupleFileOutput']


#-----------------------------------------------------------------------------
# Setup input
#-----------------------------------------------------------------------------
if type(poolFileInput) == type(""):
    poolFileInput = [poolFileInput]
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = poolFileInput
athenaCommonFlags.FilesInput.set_Value_and_Lock(poolFileInput)
athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)


#-----------------------------------------------------------------------------
# Configure RecExCommon for autoconfiguration
#-----------------------------------------------------------------------------
# Determine if we are reading an ESD or an AOD.
# Unfortunately, have to switch electron/photon collection
# names by hand until zero-event file problem is solved.
from RecExConfig.RecFlags import rec

rec.readAOD      = True
rec.readESD      = False
rec.readRDO      = False
rec.doCBNT       = False
rec.doWriteESD   = False
rec.doWriteAOD   = False
rec.doAOD        = False
rec.doWriteTAG   = False
rec.doPerfMon    = False
rec.doHist       = False
rec.doTruth.set_Value_and_Lock( doTruth ) # gets overwritten by RecExCommon without locking
#rec.abortOnUncheckedStatusCode.set_Value_and_Lock(False) # temporary for problem with MuonD3PDMaker truth
include ("RecExCommon/RecExCommon_topOptions.py")


#-----------------------------------------------------------------------------
# Algorithms Upstream of D3PDMaker
#-----------------------------------------------------------------------------
# get a handle on the job main sequence
if doSkim or doGRL:
    import AthenaCommon.AlgSequence as acas
    job = acas.AlgSequence()

## filter configuration ##
##  -> we use the special sequence 'AthFilterSeq' which
##      is run before any other algorithm (which are usually in the
##      'TopAlg' sequence
    seq = acas.AthSequencer("AthFilterSeq")

    if doSkim:
	    # Keep only events with at least one tau candidate
	    from PrimaryDPDMaker.TauFilter import TauFilter
	    seq += TauFilter("alg")
	    seq.alg.tauCollectionName = "TauRecContainer"
	    seq.alg.cutTauEtMin       = -1.0
	    seq.alg.cutNTrk           = 0
	    seq.alg.doPreselection    = False

    if doGRL:
	    from GoodRunsLists.GoodRunsListsConf import *
	    ToolSvc += GoodRunsListSelectorTool() 
	    GoodRunsListSelectorTool.GoodRunsListVec = [ myGRL ]
	    from GoodRunsListsUser.GoodRunsListsUserConf import *
	    seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
	    seq.GRLTriggerAlg1.GoodRunsListArray = [ myLBCollection ]



		    
# Put true hadronic taus in StoreGate.
if rec.doTruth():
	from TauD3PDMaker.TauD3PDMakerConf import TruthTausToSG
	topSequence += TruthTausToSG()


###########################################################
#-----------------------------------------------------------------------------
# Make the D3PD.
#-----------------------------------------------------------------------------
# 2 possible sizes: small and medium. Medium has default tuplename.
# You can specify SmallMedium at the top of this script
# in order to produce both D3PDs at the same time.
#
#
if perfSize.lower().find("small") is not -1:
	from TauD3PDMaker.tauPerfSmall import tauPerfSmall
	alg = tauPerfSmall (file=tupleFileOutput+"Small.root", level=tauDetailsLevel, tuplename="tau", streamName="TauSMALL")
	
if perfSize.lower().find("medium") is not -1:
	from TauD3PDMaker.tauPerfMedium import tauPerfMedium
	alg = tauPerfMedium (file=tupleFileOutput+".root", doCl = doClusters, doTrk = doTracks, doHad = doHadrons, level=tauDetailsLevel, tuplename='tau', streamName="TauMEDIUM")	

# Silencing some over-abundant warnings
from AthenaCommon.AppMgr import ToolSvc
if not hasattr( ToolSvc, "TrigDecisionTool" ):
	from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
	ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
	ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR
