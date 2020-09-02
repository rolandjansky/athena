from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

from InDetD3PDMaker.InDetD3PDMakerSCTFlags    import InDetD3PDSCTFlags

if hasattr(runArgs,"inputESDFile"):
    InDetD3PDSCTFlags.inputFiles = runArgs.inputESDFile
    athenaCommonFlags.FilesInput=InDetD3PDSCTFlags.inputFiles()
    
if hasattr(runArgs,"inputBSFile"):
    InDetD3PDSCTFlags.inputFiles = runArgs.inputBSFile
    globalflags.InputFormat='bytestream'
    athenaCommonFlags.BSRDOInput=InDetD3PDSCTFlags.inputFiles()

if hasattr(runArgs,"inputRDOFile"):
    InDetD3PDSCTFlags.inputFiles = runArgs.inputRDOFile
    globalflags.InputFormat='pool'
    athenaCommonFlags.PoolRDOInput=InDetD3PDSCTFlags.inputFiles()
    athenaCommonFlags.FilesInput=athenaCommonFlags.PoolRDOInput()


# run the reconstruction
include('InDetD3PDMaker/RecoSetupForSctNtuple.py')

#Condition tag 04.03
if globalflags.InputFormat() == 'bytestream':   
   if len(globalflags.ConditionsTag())!=0:
      from IOVDbSvc.CondDB import conddb
      conddb.setGlobalTag(globalflags.ConditionsTag())
   

# compose the tree
from InDetD3PDMaker.InDetD3PDMakerSCTFlags    import InDetD3PDSCTFlags

# uncomment the necessary one
InDetD3PDSCTFlags.doSCTNtuple = True # default
#InDetD3PDSCTFlags.doSCTNtupleLight = True
#InDetD3PDSCTFlags.doSCTNtupleExtended = True

#if not hasattr(runArgs,"inputRDOFile") and not hasattr(runArgs,"inputBSFile"):
#    include('InDetD3PDMaker/SCTNtuple.py')

include('InDetD3PDMaker/SCTNtuple.py')


#if globalflags.InputFormat() == 'bytestream':
#    from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
#    AANTupleStream('D3PD').ExistDataHeader = False


#################################################################
if hasattr(runArgs,"maxEvents"):
   theApp.EvtMax=runArgs.maxEvents

#theApp.SkipEvents=1870
#ServiceMgr.EventSelector.SkipEvents=1870

if globalflags.InputFormat() == 'bytestream':
    ServiceMgr.EventSelector.Input = athenaCommonFlags.BSRDOInput()
else:
    ServiceMgr.EventSelector.InputCollections = InDetD3PDSCTFlags.inputFiles()
