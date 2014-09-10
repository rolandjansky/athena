from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

from TrackD3PDMaker.TrackD3PDMakerSCTFlags    import TrackD3PDSCTFlags

if hasattr(runArgs,"inputESDFile"):
    TrackD3PDSCTFlags.inputFiles = runArgs.inputESDFile
    athenaCommonFlags.FilesInput=TrackD3PDSCTFlags.inputFiles()
    
if hasattr(runArgs,"inputBSFile"):
    TrackD3PDSCTFlags.inputFiles = runArgs.inputBSFile
    globalflags.InputFormat='bytestream'
    athenaCommonFlags.BSRDOInput=TrackD3PDSCTFlags.inputFiles()

if hasattr(runArgs,"inputRDOFile"):
    TrackD3PDSCTFlags.inputFiles = runArgs.inputRDOFile
    globalflags.InputFormat='pool'
    athenaCommonFlags.PoolRDOInput=TrackD3PDSCTFlags.inputFiles()
    athenaCommonFlags.FilesInput=athenaCommonFlags.PoolRDOInput()


# run the reconstruction
include('TrackD3PDMaker/RecoSetupForSctNtuple.py')

#Condition tag 04.03
if globalflags.InputFormat() == 'bytestream':   
   if len(globalflags.ConditionsTag())!=0:
      from IOVDbSvc.CondDB import conddb
      conddb.setGlobalTag(globalflags.ConditionsTag())
   

# compose the tree
from TrackD3PDMaker.TrackD3PDMakerSCTFlags    import TrackD3PDSCTFlags

# uncomment the necessary one
TrackD3PDSCTFlags.doSCTNtuple = True # default
#TrackD3PDSCTFlags.doSCTNtupleLight = True
#TrackD3PDSCTFlags.doSCTNtupleExtended = True

#if not hasattr(runArgs,"inputRDOFile") and not hasattr(runArgs,"inputBSFile"):
#    include('TrackD3PDMaker/SCTNtuple.py')

include('TrackD3PDMaker/SCTNtuple.py')


#if globalflags.InputFormat() == 'bytestream':
#    from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
#    AANTupleStream('D3PD').ExistDataHeader = False


#################################################################
if hasattr(runArgs,"maxEvents"):
   theApp.EvtMax=runArgs.maxEvents

#theApp.SkipEvents=1870
#ServiceMgr.EventSelector.SkipEvents=1870

if globalflags.InputFormat() == 'bytestream':
    ServiceMgr.FullFileName=athenaCommonFlags.BSRDOInput()
else:
    ServiceMgr.EventSelector.InputCollections = TrackD3PDSCTFlags.inputFiles()






