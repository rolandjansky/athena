from __future__ import print_function
###############################################################
#
# Skeleton top job options for RAW->ALL
#
# TODO: Review of options supported here...
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")

#from AthenaCommon.Logging import logging
import logging
recoLog = logging.getLogger('raw_to_all')
recoLog.info( '****************** STARTING RAW->ALL MAKING *****************' )

from AthenaCommon.AppMgr import ServiceMgr; import AthenaPoolCnvSvc.AthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaConfiguration.AllConfigFlags import ConfigFlags



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


## EST/AOD Output
# should be first as some other configuration might depend on it (e.g. trigger output)
if hasattr(runArgs,"outputESDFile"):
    rec.doESD.set_Value_and_Lock( True )
    rec.doWriteESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDOutput.set_Value_and_Lock( runArgs.outputESDFile )
    
if hasattr(runArgs,"outputAODFile"):
    rec.doAOD.set_Value_and_Lock( True )
    rec.doWriteAOD.set_Value_and_Lock( True ) 
    athenaCommonFlags.PoolAODOutput.set_Value_and_Lock( runArgs.outputAODFile )
    # Lock DQ configuration to prevent downstream override
    # RB 15/12/2020: This logic was added in !36737, not sure if still needed
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    print('DQMonFlags.useTrigger override')
    DQMonFlags.useTrigger.set_Value_and_Lock(rec.doTrigger() and DQMonFlags.useTrigger())


## Input
# BS
DRAWInputs = [ prop for prop in dir(runArgs) if prop.startswith('inputDRAW') and prop.endswith('File')]
if hasattr(runArgs,"inputBSFile"):
    if len(DRAWInputs) > 0:
        raise RuntimeError('Impossible to run RAWtoESD with input BS and DRAW files (one input type only!)')
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock( runArgs.inputBSFile )
    ConfigFlags.Input.Files = athenaCommonFlags.BSRDOInput()
if len(DRAWInputs) == 1:
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock( getattr(runArgs, DRAWInputs[0]) )
    ConfigFlags.Input.Files = athenaCommonFlags.BSRDOInput()
elif len(DRAWInputs) > 1:
    raise RuntimeError('Impossible to run RAWtoALL with multiple input DRAW files (viz.: {0})'.format(DRAWInputs))


# RDO
if hasattr(runArgs,"inputRDOFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDOFile )
    ConfigFlags.Input.Files = athenaCommonFlags.PoolRDOInput()
if hasattr(runArgs,"inputRDO_TRIGFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDO_TRIGFile)
    ConfigFlags.Input.Files = athenaCommonFlags.PoolRDOInput()
    rec.doTrigger.set_Value_and_Lock(False)
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
    objKeyStore.addManyTypesStreamESD(getTrigIDTruthList(ConfigFlags.Trigger.ESDEDMSet))
    objKeyStore.addManyTypesStreamAOD(getTrigIDTruthList(ConfigFlags.Trigger.AODEDMSet))
    objKeyStore.addManyTypesStreamESD(getLvl1ESDList())
    objKeyStore.addManyTypesStreamAOD(getLvl1AODList())
    
# EVNT (?)
if hasattr(runArgs,"inputEVNTFile"):
    #specific settings for AtlfastIIF
    rec.readRDO.set_Value_and_Lock( True )
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputEVNTFile )


## Output
if hasattr(runArgs,"trigFilterList"):
    rec.doTriggerFilter.set_Value_and_Lock(True)
    rec.triggerFilterList = "||".join(runArgs.trigFilterList)

if hasattr(runArgs,"outputHIST_R2AFile"):
    rec.doMonitoring.set_Value_and_Lock(True)
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.histogramFile.set_Value_and_Lock( runArgs.outputHIST_R2AFile )
    DQMonFlags.monType='BSall' 

if hasattr(runArgs,"outputTAG_COMMFile"):
    rec.doWriteTAGCOM.set_Value_and_Lock( True )
    rec.PoolTAGCOMOutput.set_Value_and_Lock( runArgs.outputTAG_COMMFile )

if hasattr(runArgs,"outputHISTFile"):
    rec.doMonitoring.set_Value_and_Lock(True)
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.histogramFile.set_Value_and_Lock( runArgs.outputHISTFile )
    DQMonFlags.monType='BSall' 

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

if hasattr(runArgs,"outputNTUP_MUONCALIBFile"):
    from MuonRecExample.MuonRecFlags import muonRecFlags
    muonRecFlags.doCalib = True
    muonRecFlags.calibNtupleOutput.set_Value_and_Lock( runArgs.outputNTUP_MUONCALIBFile )
    
# Event display tarballs    
if hasattr(runArgs, 'outputTXT_JIVEXMLTGZFile'):
    jp.Rec.doJiveXML.set_Value_and_Lock(True)

# DESD, DAOD and DRAW
DRAWOutputs = [ prop for prop in dir(runArgs) if prop.startswith('outputDRAW') and prop.endswith('File')]
DAODOutputs = [ prop for prop in dir(runArgs) if prop.startswith('outputDAOD') and prop.endswith('File')]
DESDOutputs = [ prop for prop in dir(runArgs) if prop.startswith('outputDESD') and prop.endswith('File')]

if len(DESDOutputs) > 0 or len(DAODOutputs) > 0:
    rec.doWriteDPD.set_Value_and_Lock(True)

listOfFlags=[]
try:
    from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
    listOfFlags.append(primDPD)
except ImportError:
    print("WARNING PrimaryDPDFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project.")

from RecJobTransforms.DPDUtils import SetupOutputDPDs
rec.DPDMakerScripts.append(SetupOutputDPDs(runArgs,listOfFlags))

# Need to handle this properly in RecExCommon top options
rec.OutputFileNameForRecoStep="RAWtoALL"


#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
#if len(rec.DPDMakerScripts()) > 0 and len(DRAWOutputs) == 0:
if len(rec.DPDMakerScripts()) > 0:
    rec.doDPD = True
    rec.DPDMakerScripts.append("PrimaryDPDMaker/PrimaryDPDMaker.py")
include( "RecExCommon/RecExCommon_topOptions.py" )

if hasattr(runArgs,"inputRDO_TRIGFile") and rec.doFileMetaData():
    ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool( "TriggerMenuMetaDataTool",
                              OutputLevel = 3 )
    svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]

#==========================================================
# Use ZLIB for compression of all temporary outputs
#==========================================================
from AthenaPoolCnvSvc import PoolAttributeHelper as pah
if hasattr(runArgs, "outputESDFile") and (runArgs.outputESDFile.endswith('_000') or runArgs.outputESDFile.startswith('tmp.')):
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ pah.setFileCompAlg( athenaCommonFlags.PoolESDOutput(), 1 ) ]
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ pah.setFileCompLvl( athenaCommonFlags.PoolESDOutput(), 1 ) ]

if hasattr(runArgs, "outputAODFile") and (runArgs.outputAODFile.endswith('_000') or runArgs.outputAODFile.startswith('tmp.')):
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ pah.setFileCompAlg( athenaCommonFlags.PoolAODOutput(), 1 ) ]
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ pah.setFileCompLvl( athenaCommonFlags.PoolAODOutput(), 1 ) ]

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
