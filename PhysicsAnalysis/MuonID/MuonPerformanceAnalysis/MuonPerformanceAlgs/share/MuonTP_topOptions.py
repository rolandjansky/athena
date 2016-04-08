# MuonTP_topOptions.py
#
# Massimiliano Bellomo
# July 2014
#
# Example job option for running tag-and-probe tools in Athena from xAOD
#


#Specify input file.

#InputAODList = []
#if not "pandaJob" in globals() and not  "FNAME" in globals():



from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf


if 'FILELIST' in globals():
        #FILELIST="filelist.txt"
    print "running with local file list %s"%FILELIST

    import os

    if os.path.isfile(FILELIST):
      with open(FILELIST,'r') as input:
          InputAODList=[bla for bla in input.read().split("\n") if bla != ""and not '#' in bla ]
          acf.FilesInput = InputAODList
          ServiceMgr.EventSelector.InputCollections = InputAODList

elif 'FNAME' in globals():
    print "running with manually specified input file %s"%FNAME
    InputAODList=[FNAME]
    acf.FilesInput = InputAODList
    ServiceMgr.EventSelector.InputCollections = InputAODList
    
#else:
    #print "Neither FNAME nor FILELIST specified - if we are not on the grid, this might go wrong..."

#if not "pandaJob" in globals() and not  "FNAME" in globals():
    #if not 'FILELIST' in globals():
        #FILELIST="filelist.txt"
    #print "running with file list %s"%FILELIST

    #with open(FILELIST,'r') as input:
        #InputAODList=[bla for bla in input.read().split("\n") if bla != ""and not '#' in bla ]

#else:
    #InputAODList=[FNAME]


#--------------------------------------------------------------
# Input stream
#--------------------------------------------------------------


print ServiceMgr.EventSelector.InputCollections
if len(ServiceMgr.EventSelector.InputCollections) == 0:
    print "Empty input collection - are you running locally without FNAME or FILELIST set? This may go horribly wrong..."
    
    


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

# Configure Muon TP tools
from MuonPerformanceAlgs import MuonTPAnalysis
MuonTPAnalysis.MuonTPConfig('muontp.root',doPlots=True,doEff=True)



