from __future__ import print_function
###############################################################
#
# Skeleton top job options for RAW->ESD
# Put here outputs that require rec.doESD=True
#
# TODO: Review of options supported here...
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")
rec.doESD=True

#from AthenaCommon.Logging import logging
import logging
recoLog = logging.getLogger('raw_to_esd')
recoLog.info( '****************** STARTING RAW->ESD MAKING *****************' )

## Automatically turn ON/OFF and set output file name of each possible DPD (in this case: DRAW)
listOfFlags=[]
try:
    from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
    listOfFlags.append(primDPD)
except ImportError:
    print("WARNING PrimaryDPDFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project.")
    
from PATJobTransforms.DPDUtils import SetupOutputDPDs
rec.DPDMakerScripts.append(SetupOutputDPDs(runArgs,listOfFlags))

from AthenaCommon.AppMgr import ServiceMgr; import AthenaPoolCnvSvc.AthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags



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


## Input
# BS
DRAWInputs = [ prop for prop in dir(runArgs) if prop.startswith('inputDRAW') and prop.endswith('File')]
if hasattr(runArgs,"inputBSFile"):
    if len(DRAWInputs) > 0:
        raise RuntimeError('Impossible to run RAWtoESD with input BS and DRAW files (one input type only!)')
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock( runArgs.inputBSFile )
if len(DRAWInputs) == 1:
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock( getattr(runArgs, DRAWInputs[0]) )
elif len(DRAWInputs) > 1:
    raise RuntimeError('Impossible to run RAWtoESD with multiple input DRAW files (viz.: {0})'.format(DRAWInputs))


# RDO
if hasattr(runArgs,"inputRDOFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDOFile )
if hasattr(runArgs,"inputRDO_TRIGFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDO_TRIGFile)
    rec.doTrigger.set_Value_and_Lock(False)
    recAlgs.doTrigger.set_Value_and_Lock(False)
    from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
    HLTMonFlags.doMonTier0 = False
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.doCTPMon = False
    DQMonFlags.doHLTMon = False
    DQMonFlags.useTrigger = False
    DQMonFlags.doLVL1CaloMon = False
    # Configure HLT output
    from TriggerJobOpts.HLTTriggerResultGetter import HLTTriggerResultGetter
    hltOutput = HLTTriggerResultGetter()
    # Add Trigger menu metadata
    from RecExConfig.ObjKeyStore import objKeyStore
    if rec.doFileMetaData():
       metadataItems = [ "xAOD::TriggerMenuContainer#TriggerMenu",
                        "xAOD::TriggerMenuAuxContainer#TriggerMenuAux." ]
       objKeyStore.addManyTypesMetaData( metadataItems )
    # Configure other outputs
    from TrigEDMConfig.TriggerEDM import getLvl1ESDList
    from TrigEDMConfig.TriggerEDM import getLvl1AODList
    from TrigEDMConfig.TriggerEDM import getTrigIDTruthList
    objKeyStore.addManyTypesStreamESD(getTrigIDTruthList(TriggerFlags.ESDEDMSet()))
    objKeyStore.addManyTypesStreamAOD(getTrigIDTruthList(TriggerFlags.AODEDMSet()))
    objKeyStore.addManyTypesStreamESD(getLvl1ESDList())
    objKeyStore.addManyTypesStreamAOD(getLvl1AODList())

if hasattr(runArgs,"inputRDO_FILTFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDO_FILTFile )

if hasattr(runArgs,"inputRDO_FTKFile"):
    rec.readRDO.set_Value_and_Lock( True )
    rec.doFTK.set_Value_and_Lock(True);
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDO_FTKFile )
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.doFTK=True;
    
# EVNT (?)
if hasattr(runArgs,"inputEVNTFile"):
    #specific settings for AtlfastIIF
    rec.readRDO.set_Value_and_Lock( True )
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputEVNTFile )


# For these DRAW names note that a bit of name mangling is applied by the DPD writer,
# with, e.g., a '.pool.root' extension being dropped. (N.B. they are bytestream so 
# .pool.root is misleading anyway). If you give a mangleable name then the transform
# will fail on output file validation. We assume if you ask for these filetypes
# that you know what you are doing!
if hasattr(runArgs,"outputDRAW_ZEEFile"):
    primDPD.WriteRAWPerfDPD_ZEE.FileName = runArgs.outputDRAW_ZEEFile
if hasattr(runArgs,"outputDRAW_WENUFile"):
    primDPD.WriteRAWPerfDPD_WENU.FileName = runArgs.outputDRAW_WENUFile
if hasattr(runArgs,"outputDRAW_ZMUMUFile"):
    primDPD.WriteRAWPerfDPD_ZMUMU.FileName = runArgs.outputDRAW_ZMUMUFile
if hasattr(runArgs,"outputDRAW_WMUNUFile"):
    primDPD.WriteRAWPerfDPD_WMUNU.FileName = runArgs.outputDRAW_WMUNUFile

if hasattr(runArgs,"trigFilterList"):
    rec.doTriggerFilter.set_Value_and_Lock(True)
    rec.triggerFilterList = "||".join(runArgs.trigFilterList)

if hasattr(runArgs,"outputESDFile"):
    rec.doESD.set_Value_and_Lock( True )
    rec.doWriteESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDOutput.set_Value_and_Lock( runArgs.outputESDFile )
    
if hasattr(runArgs,"outputAODFile"):
    rec.doAOD.set_Value_and_Lock( True )
    rec.doWriteAOD.set_Value_and_Lock( True ) 
    athenaCommonFlags.PoolAODOutput.set_Value_and_Lock( runArgs.outputAODFile )

if hasattr(runArgs,"outputTAG_COMMFile"):
    rec.doWriteTAGCOM.set_Value_and_Lock( True )
    rec.PoolTAGCOMOutput.set_Value_and_Lock( runArgs.outputTAG_COMMFile )

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

if hasattr(runArgs,"outputHIST_ESD_INTFile"):
    rec.doMonitoring.set_Value_and_Lock(True)
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.histogramFile.set_Value_and_Lock( runArgs.outputHIST_ESD_INTFile )
    
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
    
# Event display tarballs    
if hasattr(runArgs, 'outputTXT_JIVEXMLTGZFile'):
    jp.Rec.doJiveXML.set_Value_and_Lock(True)
    

rec.OutputFileNameForRecoStep="RAWtoESD"

#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
if hasattr(runArgs,"topOptions"): include(runArgs.topOptions)
else: include( "RecExCommon/RecExCommon_topOptions.py" )

if hasattr(runArgs,"inputRDO_TRIGFile") and rec.doFileMetaData():
    ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool( "TriggerMenuMetaDataTool",
                              OutputLevel = 3 )
    svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]

#==========================================================
# Use ZLIB for compression of all temporary outputs
#==========================================================
if hasattr(runArgs, "outputESDFile") and ('_000' in runArgs.outputESDFile or 'tmp.' in runArgs.outputESDFile):
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" +  athenaCommonFlags.PoolESDOutput()+ "'; COMPRESSION_ALGORITHM = '1'" ]
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" +  athenaCommonFlags.PoolESDOutput()+ "'; COMPRESSION_LEVEL = '1'" ]

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
