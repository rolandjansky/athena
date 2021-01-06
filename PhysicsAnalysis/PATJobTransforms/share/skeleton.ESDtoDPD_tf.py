from __future__ import print_function
###############################################################
#
# Skeleton top job options for ESD->DPD 
# Put here outputs that require rec.doESD=False
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")
rec.doESD=False
from AthenaCommon.Logging import logging
recoLog = logging.getLogger('esd_to_dpd')
recoLog.info( '****************** STARTING ESD->DPD MAKING *****************' )

## Automatically turn ON/OFF and set output file name of each possible DPD
listOfFlags=[]
try:
    from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
    listOfFlags.append(primDPD)
except ImportError:
    print("WARNING PrimaryDPDFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project.")
try:
    from D2PDMaker.D2PDFlags import D2PDFlags
    listOfFlags.append(D2PDFlags)
except ImportError:
    print("Unable to import listAODtoD2PD. This requires D2PDMaker-00-00-55-08 or D2PDMaker-00-00-62")
try:
    from D3PDMakerConfig.D3PDProdFlags import prodFlags
    listOfFlags.append( prodFlags )
except ImportError:
    print("WARNING D3PDProdFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project.")


from PATJobTransforms.DPDUtils import SetupOutputDPDs
rec.DPDMakerScripts.append(SetupOutputDPDs(runArgs,listOfFlags))


rec.OutputFileNameForRecoStep="ESDtoDPD"

# New-style config
from AthenaConfiguration.AllConfigFlags import ConfigFlags

## Input
if hasattr(runArgs,"inputFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
if hasattr(runArgs,"inputBSFile"):
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock( runArgs.inputBSFile )
    ConfigFlags.Input.Files = athenaCommonFlags.BSRDOInput()
if hasattr(runArgs,"inputRDOFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDOFile )
    ConfigFlags.Input.Files = athenaCommonFlags.PoolRDOInput()
if hasattr(runArgs,"inputESDFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDInput.set_Value_and_Lock( runArgs.inputESDFile )
    ConfigFlags.Input.Files = athenaCommonFlags.PoolESDInput()
if hasattr(runArgs,"inputTAGFile"):
    rec.readTAG.set_Value_and_Lock( True )
    rec.readESD.set_Value_and_Lock( True )
    rec.doESD.set_Value_and_Lock( False )
    rec.doWriteAOD.set_Value_and_Lock( False )
    rec.TAGFromRDO.set_Value_and_Lock( False )
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputTAGFile )
    
#Outputs
if hasattr(runArgs,"outputNTUP_PROMPTPHOTFile"):
    from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
    PAUflags.FileName = runArgs.outputNTUP_PROMPTPHOTFile
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )

if hasattr(runArgs,"outputNTUP_WZFile"):
    from D3PDMakerConfig.D3PDProdFlags import prodFlags
    prodFlags.WZOutputFile=runArgs.outputNTUP_WZFile
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )

if hasattr(runArgs,"outputNTUP_TRTFile"):
    from ConversionDumper.ConversionDumperFlags import CDflags
    CDflags.FileName=runArgs.outputNTUP_TRTFile
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )

if hasattr(runArgs,"outputNTUP_HECNOISEFile"):
    from LArCalibTest.HECNoiseD3PDFlags import HECNoiseflags
    HECNoiseflags.FileName=runArgs.outputNTUP_HECNOISEFile
    rec.UserAlgs += ['LArCalibTest/HECD3PDFilter.py']
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )

if hasattr(runArgs,"outputNTUP_MCPFile"):
    from MuonIDNtupleMakers.MuonIDNtupleMakersFlags import MNMFlags
    MNMFlags.outputFile = runArgs.outputNTUP_MCPFile
    MNMFlags.inputFiles = runArgs.inputESDFile

if hasattr(runArgs,"outputNTUP_SCTFile"):
    from TrackD3PDMaker.TrackD3PDMakerSCTFlags import TrackD3PDSCTFlags
    TrackD3PDSCTFlags.outputFile = runArgs.outputNTUP_SCTFile
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )

if hasattr(runArgs,"outputESDFile"):
    #for TAG->ESD->skimmedESD
    rec.doWriteESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDOutput.set_Value_and_Lock( runArgs.outputESDFile )
if hasattr(runArgs,"outputRDOFile"):
    #for TAG->RDO->skimmedRDO
    rec.doWriteRDO.set_Value_and_Lock( True )
    athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock( runArgs.outputRDOFile )
    if hasattr(runArgs,"inputTAGFile"):
        rec.readTAG.set_Value_and_Lock( True )
        rec.readRDO.set_Value_and_Lock( True )
        rec.readESD.set_Value_and_Lock( False )
        rec.doWriteAOD.set_Value_and_Lock( False )
        rec.doWriteESD.set_Value_and_Lock( False )

if hasattr(runArgs,"outputNTUP_ENHBIASFile"):
    from TrigCostAthena.TrigCostAthenaFlags import TrigCostAthenaFlags
    TrigCostAthenaFlags.StoreNtVerticesOutputFile.set_Value_and_Lock( runArgs.outputNTUP_ENHBIASFile )
    TrigCostAthenaFlags.DoStoreNtVertices.set_Value_and_Lock( True )
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )
    include("TrigCostAthena/ESDtoNTUP_ENHBIAS.py")


if hasattr(runArgs,"outputNTUP_LARNOISEFile"):
    from LArMonitoring.LArMonitoringFlags import larNoiseBurstFlags
    larNoiseBurstFlags.outputFile = runArgs.outputNTUP_LARNOISEFile
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )
    include("LArMonitoring/LArNoiseBursts_prodJO.py")


if hasattr(runArgs,"outputNTUP_FastCaloSimFile"):
    from ISF_FastCaloSimParametrization.ISF_NativeFastCaloSimJobProperties import ISF_NativeFastCaloSimFlags
    ISF_NativeFastCaloSimFlags.outputFile = runArgs.outputNTUP_FastCaloSimFile
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )
    include("ISF_FastCaloSimParametrization/prodOptions.ISF_ntuple.py")


## Import D3PD flags before preExec, for convenience
try:
    from D3PDMakerConfig.D3PDProdFlags  import oldProdFlags
except(ImportError):
    print("WARNING oldProdFlags not available. ")
    pass
try:
    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
except(ImportError):
    print("WARNING D3PDMakerFlags not available. ")
    pass

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
elif rec.DPDMakerScripts()!=[]: include("PrimaryDPDMaker/esdtodpd.py")
else: include( "RecExCommon/RecExCommon_topOptions.py" )

## Make "old style" D3PDs.
try:
    for c in SetupOutputDPDs(runArgs, [oldProdFlags]): c()
except NameError:
    print("WARNING: oldProdFlags not available")
    pass

## Offline prescales (has to be *after* the topOptions)
if hasattr(runArgs,"prescales"):
    recoLog.info( '**** DPD offline prescale arguments:' )
    from PrimaryDPDMaker.JobTransformConfiguration import ApplyPrescale
    for prescale in runArgs.prescales:
        recoLog.info( prescale )
        ApplyPrescale(prescale)

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

