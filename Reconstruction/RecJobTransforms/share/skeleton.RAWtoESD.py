###############################################################
#
# Skeleton top job options for RAW->ESD
# Put here outputs that require rec.doESD=True
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("PATJobTransforms/CommonSkeletonJobOptions.py")
rec.doESD=True

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('raw_to_esd')
recoLog.info( '****************** STARTING RAW->ESD MAKING *****************' )

## Automatically turn ON/OFF and set output file name of each possible DPD (in this case: DRAW)
listOfFlags=[]
try:
    from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
    listOfFlags.append(primDPD)
except ImportError:
    print "WARNING PrimaryDPDFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project."
    
from PATJobTransforms.DPDUtils import SetupOutputDPDs
rec.DPDMakerScripts.append(SetupOutputDPDs(runArgs,listOfFlags))


## Input
if hasattr(runArgs,"inputFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
if hasattr(runArgs,"inputBSFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock( runArgs.inputBSFile )
if hasattr(runArgs,"inputRDOFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDOFile )
if hasattr(runArgs,"inputEVNTFile"):
    #specific settings for AtlfastIIF
    rec.readRDO.set_Value_and_Lock( True )
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputEVNTFile )


## Outputs (this hack is only needed while waiting for TDAQ update)
if hasattr(runArgs,"outputDRAW_ZEEFile"):
    #from PATJobTransforms.DPDUtils import outputBSNameStripper
    #simpleName = outputBSNameStripper(runArgs.outputDRAW_ZEEFile)
    primDPD.WriteRAWPerfDPD_ZEE.FileName = runArgs.outputDRAW_ZEEFile
if hasattr(runArgs,"outputDRAW_WENUFile"):
    #from PATJobTransforms.DPDUtils import outputBSNameStripper
    #simpleName = outputBSNameStripper(runArgs.outputDRAW_WENUFile)
    primDPD.WriteRAWPerfDPD_WENU.FileName = runArgs.outputDRAW_WENUFile
if hasattr(runArgs,"outputDRAW_ZMUMUFile"):
    #from PATJobTransforms.DPDUtils import outputBSNameStripper
    #simpleName = outputBSNameStripper(runArgs.outputDRAW_ZMUMUFile)
    primDPD.WriteRAWPerfDPD_ZMUMU.FileName = runArgs.outputDRAW_ZMUMUFile
if hasattr(runArgs,"outputDRAW_WMUNUFile"):
    #from PATJobTransforms.DPDUtils import outputBSNameStripper
    #simpleName = outputBSNameStripper(runArgs.outputDRAW_WMUNUFile)
    primDPD.WriteRAWPerfDPD_WMUNU.FileName = runArgs.outputDRAW_WMUNUFile

if hasattr(runArgs,"trigFilterList"):
    rec.doTriggerFilter.set_Value_and_Lock(True)
    triggerListString = "|".join(runArgs.trigFilterList)
    rec.triggerFilterList=triggerListString

if hasattr(runArgs,"outputESDFile"):
    rec.doESD.set_Value_and_Lock( True )
    rec.doWriteESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDOutput.set_Value_and_Lock( runArgs.outputESDFile )
if hasattr(runArgs,"tmpESD"):
    rec.doESD.set_Value_and_Lock( True )
    rec.doWriteESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDOutput.set_Value_and_Lock( runArgs.tmpESD )
    
if hasattr(runArgs,"outputAODFile"):
    rec.doAOD.set_Value_and_Lock( True )
    rec.doWriteAOD.set_Value_and_Lock( True ) 
    athenaCommonFlags.PoolAODOutput.set_Value_and_Lock( runArgs.outputAODFile )
if hasattr(runArgs,"tmpAOD"):
    rec.doAOD.set_Value_and_Lock( True )
    rec.doWriteAOD.set_Value_and_Lock( True ) 
    athenaCommonFlags.PoolAODOutput.set_Value_and_Lock( runArgs.tmpAOD )

if hasattr(runArgs,"outputTAG_COMMFile"):
    rec.doWriteTAGCOM.set_Value_and_Lock( True )
    rec.PoolTAGCOMOutput.set_Value_and_Lock( runArgs.outputTAG_COMMFile )

if hasattr(runArgs,"outputCBNTFile"):
    rec.doCBNT.set_Value_and_Lock( True )
    rec.RootNtupleOutput.set_Value_and_Lock( runArgs.outputCBNTFile ) 

if hasattr(runArgs,"outputNTUP_TRKVALIDFile"):
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.doTrkNtuple.set_Value_and_Lock( True )
    InDetFlags.doPixelTrkNtuple.set_Value_and_Lock( True )

    from InDetRecExample.InDetKeys import InDetKeys
    InDetKeys.trkValidationNtupleName.set_Value_and_Lock( runArgs.outputNTUP_TRKVALIDFile )

    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    InDetTrigFlags.doTrkNtuple.set_Value_and_Lock( True )
     
if hasattr(runArgs,"outputNTUP_MUONCALIBFile"):
    from MuonRecExample.MuonRecFlags import muonRecFlags
    muonRecFlags.doCalib = True
    muonRecFlags.calibNtupleOutput.set_Value_and_Lock( runArgs.outputNTUP_MUONCALIBFile )

if hasattr(runArgs,"outputNTUP_SCTFile"):
    rec.DPDMakerScripts.append("TrackD3PDMaker/SCTNtuple.py") 
    from TrackD3PDMaker.TrackD3PDMakerSCTFlags import TrackD3PDSCTFlags
    TrackD3PDSCTFlags.outputFile = runArgs.outputNTUP_SCTFile

if hasattr(runArgs,"outputDQMonitorFile"):
    rec.doMonitoring.set_Value_and_Lock(True)
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.histogramFile.set_Value_and_Lock( runArgs.outputDQMonitorFile )
    
if hasattr(runArgs, "outputTXT_FTKIPFile"):
    rec.doCalo.set_Value_and_Lock(False)
    rec.doInDet.set_Value_and_Lock(True)
    rec.doMuon.set_Value_and_Lock(False)
    rec.doJetMissingETTag.set_Value_and_Lock(False)
    rec.doEgamma.set_Value_and_Lock(False)
    rec.doMuonCombined.set_Value_and_Lock(False)
    rec.doTau.set_Value_and_Lock(False)
    rec.doTrigger.set_Value_and_Lock(False)
    rec.UserAlgs=["FastTrackSimWrap/FastTrackSimWrap_jobOptions.py"]
    
# Trigger already run in this RDO?
if hasattr(runArgs, "doRDOTrigger") and runArgs.doRDOTrigger == True:
    TriggerFlags.doTriggerConfigOnly=True

# Event display tarballs    
if hasattr(runArgs, 'outputTXT_JIVEXMLTGZFile'):
    jp.Rec.doJiveXML.set_Value_and_Lock(True)
    

rec.OutputFileNameForRecoStep="RAWtoESD"

## Pre-exec
if hasattr(runArgs,"preExec"):
    recoLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        recoLog.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        include(fragment)

#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
if hasattr(runArgs,"topOptions"): include(runArgs.topOptions)
else: include( "RecExCommon/RecExCommon_topOptions.py" )

## Post-include
if hasattr(runArgs,"postInclude"): 
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    recoLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        recoLog.info(cmd)
        exec(cmd)
        
