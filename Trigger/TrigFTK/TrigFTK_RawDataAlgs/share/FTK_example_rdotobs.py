from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput =["/afs/cern.ch/user/b/baines/work/public/data/FTK_RDO/RDO.pool.root"]
#athenaCommonFlags.FilesInput =["RDO_FTK.pool.root"]
athenaCommonFlags.EvtMax=4
# steering file for RDO->RDO step
# see myTopOptions.py for more info
#if not 'rec.doTrigger' in dir():
rec.doTrigger=True
#donewTracking=True
rec.doESD.set_Value_and_Lock(False)
rec.doCBNT=False
rec.doWriteRDO=False
rec.doESD=False
rec.doWriteESD=False # uncomment if do not write ESD
rec.doWriteAOD=False # uncomment if do not write AOD
rec.doAOD=False # uncomment if do not run AOD making algorithms
rec.doWriteTAG=False # uncomment if do not write TAG

rec.doWriteBS=True # enable BS writing

# example how to specify output (preliminary)
#BSRDOOutput='AppName=Athena, OutputDirectory=./, FileTag=Release13.0.30.2, Run=3457'
#


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")



from ByteStreamCnvSvc import WriteByteStream
StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")
StreamBSFileOutput.ItemList =["FTK_RawTrackContainer#*"] 
print StreamBSFileOutput.ItemList 
