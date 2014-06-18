from AthenaCommon.Logging import logging
recoLog = logging.getLogger('raw_to_esd')
recoLog.info( '****************** STARTING RAW->ESD MAKING *****************' )

from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from PyJobTransformsCore.trfutil import checkString
from JetRec.JetRecFlags import jetFlags

try: from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
except: print "WARNING PrimaryDPDFlags not available. Normal if HLT-Point1 release."


##Disable all outputs, and enable it on demand
rec.doWriteESD=False
rec.doAOD=False
rec.doWriteAOD=False
rec.doWriteTAG=False	
rec.doWriteTAGCOM=False
rec.doMonitoring=False
rec.doCBNT=False
rec.doDPD=False
rec.Production=True

## Input
if hasattr(runArgs,"inputBSFile"):
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock( runArgs.inputBSFile )
if hasattr(runArgs,"inputRDOFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDOFile )
if hasattr(runArgs,"geometryVersion"): globalflags.DetDescrVersion.set_Value_and_Lock( runArgs.geometryVersion )
if hasattr(runArgs,"conditionsTag"): globalflags.ConditionsTag.set_Value_and_Lock( runArgs.conditionsTag )

## max/skip events
if hasattr(runArgs,"maxEvents"): athenaCommonFlags.EvtMax.set_Value_and_Lock( runArgs.maxEvents )
if hasattr(runArgs,"skipEvents"): athenaCommonFlags.SkipEvents.set_Value_and_Lock( runArgs.skipEvents )

## special arguments
if hasattr(runArgs,"beamType"): jobproperties.Beam.beamType.set_Value_and_Lock( runArgs.beamType )
if hasattr(runArgs,"AMITag"): rec.AMITag=runArgs.AMITag
if hasattr(runArgs,"RunNumber"): rec.RunNumber=runArgs.RunNumber
if hasattr(runArgs,"projectName"): rec.projectName=runArgs.projectName
if hasattr(runArgs,"trigStream"): rec.triggerStream=runArgs.trigStream
if hasattr(runArgs,"triggerConfig"):
    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    tf.triggerConfig=runArgs.triggerConfig

## autoConfiguration keywords triggering pre-defined functions
if hasattr(runArgs,"autoConfiguration"):
    for key in runArgs.autoConfiguration:
        rec.AutoConfiguration.append(key)

## Outputs
if hasattr(runArgs,"RAW_IDPROJCOMM"):
    rec.doESD.set_Value_and_Lock( True )
    rec.doDPD.set_Value_and_Lock( True )
    rec.DPDMakerScripts().append("PrimaryDPDMaker/PrimaryDPDMaker.py")
    from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
    primDPD.WriteRAW_IDPROJCOMM.set_Value_and_Lock( True )
    from PATJobTransforms.DPDUtils import outputBSNameStripper
    simpleName = outputBSNameStripper(runArgs.RAW_IDPROJCOMM)
    primDPD.RAW_IDPROJCOMMName.set_Value_and_Lock(simpleName)

if hasattr(runArgs,"outputESDFile"):
    rec.doESD.set_Value_and_Lock( True )
    rec.doWriteESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDOutput.set_Value_and_Lock( runArgs.outputESDFile )
    
include( "RecExCommission/RecExCommissionFlags_jobOptions.py" )
if hasattr(runArgs,"outputFilteredESD"):
    ATLASCosmicFlags.doFilteredESD=True 
    ATLASCosmicFlags.FilteredESDOutputFile = runArgs.outputFilteredESD
else:
    ATLASCosmicFlags.doFilteredESD=False 

if hasattr(runArgs,"outputAODFile"):
    rec.doAOD.set_Value_and_Lock( True )
    rec.doWriteAOD.set_Value_and_Lock( True ) 
    athenaCommonFlags.PoolAODOutput.set_Value_and_Lock( runArgs.outputAODFile )

if hasattr(runArgs,"outputTAGComm"):
    rec.doWriteTAGCOM.set_Value_and_Lock( True )
    rec.PoolTAGCOMOutput.set_Value_and_Lock( runArgs.outputTAGComm )

if hasattr(runArgs,"outputCBNTFile"):
    rec.doCBNT.set_Value_and_Lock( True )
    rec.RootNtupleOutput.set_Value_and_Lock( runArgs.outputCBNTFile ) 

if hasattr(runArgs,"outputPixelCalibNtp"):
    #WARNING beamType and rec.Commissioning need to be set BEFORE importing InDetFlags otherwise it's wrongly configured 
    if hasattr(runArgs,"beamType"):
        from InDetRecExample.InDetJobProperties import InDetFlags
        InDetFlags.doPixelTrkNtuple.set_Value_and_Lock( True )
        from InDetRecExample.InDetKeys import InDetKeys
        InDetKeys.trkValidationNtupleName.set_Value_and_Lock( runArgs.outputPixelCalibNtp )
    else:
        raise RuntimeError("beamType needs to explicitely set to make outputPixelCalibNtp... FIX ME!")

if hasattr(runArgs,"outputMuonCalibNtp"):
    include( "RecExCommission/RecExCommissionFlags_jobOptions.py" )
    ATLASCosmicFlags.doCalibFillNt = True
    from MuonCalibAlgs.MuonCalibAlgsConf import MuonCalib__MuonCalibAlg
    MuonCalib__MuonCalibAlg.NtupleName = runArgs.outputMuonCalibNtp

if hasattr(runArgs,"outputNTUP_TRIGTAUFile"):
    HLTMonDoOfflineTauTTP = True
    from TrigTauPerformAthena.TrigTauPerformAthenaConf import TrigTauPerformAthenaAlgo
    ttpalgo = TrigTauPerformAthenaAlgo()
    ttpalgo.rootFile = runArgs.outputNTUP_TRIGTAUFile

if hasattr(runArgs,"outputDQMonitorFile"):
    rec.doMonitoring.set_Value_and_Lock(True)
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.histogramFile.set_Value_and_Lock( runArgs.outputDQMonitorFile )

## Pre-exec
if hasattr(runArgs,"preExec"):
    recoLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        cmd=checkString(cmd)
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
else: include( "RecExCond/RecExCommon_topOptions.py" )

## Post-include
if hasattr(runArgs,"postInclude"): 
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    recoLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        cmd=checkString(cmd)
        recoLog.info(cmd)
        exec(cmd)
        
