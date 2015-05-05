# MuonTP_topOptions.py
#
# Massimiliano Bellomo
# July 2014
#
# Example job option for running tag-and-probe tools in Athena from xAOD
#


#Specify input file.
if not "pandaJob" in globals() and not  "FNAME" in globals():
    include ('MuonPerformanceAlgs/Zmumu_19.0.2.1.py')
else:
    InputAODList=[FNAME]


#include ('PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceAlgs/share/Sample_Valid3_147407_e3099_s2578_r6588.py')


#--------------------------------------------------------------
# Input stream
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = InputAODList

print ServiceMgr.EventSelector.InputCollections


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
acf.FilesInput = InputAODList

#--------------------------------------------------------------
# Reduce the event loop spam a bit
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
ServiceMgr += AthenaEventLoopMgr(EventPrintoutInterval = 500)


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)

# we keep a theApp version in case we want to run without RecExCommon
#theApp.EvtMax =  -1
#if not "pandaJob" in globals():     # special setting for local running
#    theApp.EvtMax = -1
#if "EVTMAX" in  globals():
#    theApp.EvtMax = EVTMAX


acf.EvtMax =  -1
if not "pandaJob" in globals():     # special setting for local running
    acf.EvtMax = -1
#    acf.EvtMax = 1900

# optional override for local testing
if "EVTMAX" in  globals():
    acf.EvtMax = EVTMAX

#--------------------------------------------------------------
# Configure algorithm.
#--------------------------------------------------------------

include ("MuonPerformanceAlgs/RecExCommon_for_TP.py")

# Allow messge service more than default 500 lines.
ServiceMgr.MessageSvc.infoLimit = 20000

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()


include ('MuonPerformanceAlgs/ZmumuTPAnalysis.py')
include ('MuonPerformanceAlgs/ZmumuTPIsolationAnalysis.py')
include ('MuonPerformanceAlgs/ZmumuTPTrigAnalysis.py')

# now we can add the TP analysis/es itself/themselves :)

############## Zmumu T&P Configuration ########################
do_Zmumu_RecoEff_TagProbe      = True # this is for Z->mumu, reco eff. You can add your own analysis in a similar way.
do_Zmumu_IsolationEff_TagProbe = True # this is for Z->mumu, isolation eff. You can add your own analysis in a similar way.
do_Zmumu_Trigger_TagProbe      = True  # this is for Z->mumu, Trigger eff.

##### General analysis options
writeNtuple = True                     # Write an ntuple on top of the histos - for detailed studies, but increases output file size
doEtaSlices = False
doClosure   = False
doDRSys     = False
doValid     = False


# Add utilities (tool, upstream algorithms) we need
AddIsolationTools()
AddTrigDecisionTool()
AddTrigMatchingTool()
AddMuonSelectionTool()


##### Reco eff analysis options
if do_Zmumu_RecoEff_TagProbe:
	writeNtuple = True                    
	doEtaSlices = False
	doClosure   = False
	doDRSys     = True
	doValid     = True
	AddZmumuTPAnalysis(doEtaSlices,writeNtuple,doClosure,doDRSys,doValid)      
###############################################################


##### Isolation eff analysis options
if do_Zmumu_IsolationEff_TagProbe: 
    doEtaSlices = False
    doClosure   = False
    doDRSys     = True
    doValid     = True
    AddZmumuTPIsolationAnalysis(doEtaSlices,writeNtuple,doClosure,doDRSys)
    pass
###############################################################


##### Trigger analysis options
if do_Zmumu_Trigger_TagProbe:
        doTriggerL1  = True
        doTriggerL2  = False
        doTriggerEF  = False
        doTriggerHLT = True
        doEtaSlices  = True
        doDRSys      = False
        AddTriggerTPAnalysis(doEtaSlices,writeNtuple,doClosure,doDRSys,doL1=doTriggerL1,doL2=doTriggerL2,doEF=doTriggerEF,doHLT=doTriggerHLT)
###############################################################

print ToolSvc
print job
print ServiceMgr
# Add HistSvc
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["MUONTP DATAFILE='muontp.root' OPT='RECREATE'"]

