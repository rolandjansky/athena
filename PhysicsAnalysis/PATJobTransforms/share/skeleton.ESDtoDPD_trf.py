###############################################################
#
# Skeleton top job options for ESD->DPD 
# Put here outputs that require rec.doESD=False
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("PATJobTransforms/CommonSkeletonJobOptions.py")
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
    print "WARNING PrimaryDPDFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project."
try:
    from D2PDMaker.D2PDFlags import D2PDFlags
    listOfFlags.append(D2PDFlags)
except ImportError:
    print "Unable to import listAODtoD2PD. This requires D2PDMaker-00-00-55-08 or D2PDMaker-00-00-62"
try:
    from D3PDMakerConfig.D3PDProdFlags import prodFlags
    listOfFlags.append( prodFlags )
except ImportError:
    print "WARNING D3PDProdFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project."


from PATJobTransforms.DPDUtils import SetupOutputDPDs
rec.DPDMakerScripts.append(SetupOutputDPDs(runArgs,listOfFlags))


from PATJobTransforms.OutputsMgr import outputsMgr,magicKey
for key in dir(runArgs):
    if key.startswith(magicKey):
        outputsMgr.addOutput(key,getattr(runArgs,key))

rec.OutputFileNameForRecoStep="ESDtoDPD"

## Input
if hasattr(runArgs,"inputFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
if hasattr(runArgs,"inputBSFile"):
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock( runArgs.inputBSFile )
if hasattr(runArgs,"inputRDOFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDOFile )
if hasattr(runArgs,"inputESDFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDInput.set_Value_and_Lock( runArgs.inputESDFile )
if hasattr(runArgs,"inputTAGFile"):
    rec.readTAG.set_Value_and_Lock( True )
    rec.readESD.set_Value_and_Lock( True )
    rec.doESD.set_Value_and_Lock( False )
    rec.doWriteAOD.set_Value_and_Lock( False )
    rec.TAGFromRDO.set_Value_and_Lock( False )
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputTAGFile )

# Keep track of whether an output format file is requested:
outputRequested = False

#Outputs
if hasattr(runArgs,"outputNTUP_PROMPTPHOTFile"):
    from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
    PAUflags.FileName = runArgs.outputNTUP_PROMPTPHOTFile
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )
    outputRequested = True

if hasattr(runArgs,"outputNTUP_WZFile"):
    from D3PDMakerConfig.D3PDProdFlags import prodFlags
    prodFlags.WZOutputFile=runArgs.outputNTUP_WZFile
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )
    outputRequested = True

if hasattr(runArgs,"outputNTUP_TRTFile"):
    from ConversionDumper.ConversionDumperFlags import CDflags
    CDflags.FileName=runArgs.outputNTUP_TRTFile
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )
    outputRequested = True

if hasattr(runArgs,"outputNTUP_HECNOISEFile"):
    from LArCalibTest.HECNoiseD3PDFlags import HECNoiseflags
    HECNoiseflags.FileName=runArgs.outputNTUP_HECNOISEFile
    rec.UserAlgs += ['LArCalibTest/HECD3PDFilter.py']
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )
    outputRequested = True

if hasattr(runArgs,"outputNTUP_MCPFile"):
    from MuonIDNtupleMakers.MuonIDNtupleMakersFlags import MNMFlags
    MNMFlags.outputFile = runArgs.outputNTUP_MCPFile
    MNMFlags.inputFiles = runArgs.inputESDFile
    outputRequested = True

if hasattr(runArgs,"outputNTUP_SCTFile"):
    from TrackD3PDMaker.TrackD3PDMakerSCTFlags import TrackD3PDSCTFlags
    TrackD3PDSCTFlags.outputFile = runArgs.outputNTUP_SCTFile
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )
    outputRequested = True

if hasattr(runArgs,"outputESDFile"):
    #for TAG->ESD->skimmedESD
    rec.doWriteESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDOutput.set_Value_and_Lock( runArgs.outputESDFile )
    outputRequested = True
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
    outputRequested = True

if hasattr(runArgs,"outputNTUP_ENHBIASFile"):
    from TrigCostAthena.TrigCostAthenaFlags import TrigCostAthenaFlags
    TrigCostAthenaFlags.StoreNtVerticesOutputFile.set_Value_and_Lock( runArgs.outputNTUP_ENHBIASFile )
    TrigCostAthenaFlags.DoStoreNtVertices.set_Value_and_Lock( True )
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )
    include("TrigCostAthena/ESDtoNTUP_ENHBIAS.py")
    outputRequested = True


if hasattr(runArgs,"outputNTUP_LARNOISEFile"):
    from LArMonitoring.LArMonitoringFlags import larNoiseBurstFlags
    larNoiseBurstFlags.outputFile = runArgs.outputNTUP_LARNOISEFile
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )
    include("LArMonitoring/LArNoiseBursts_prodJO.py")
    outputRequested = True

#Import D3PD flags before preExec, for convenience
from D3PDMakerConfig.D3PDProdFlags  import oldProdFlags
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from SUSYD3PDMaker.SUSYD3PDFlags    import SUSYD3PDFlags

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

## Pre-includes defined for the DPDs:
from PATJobTransforms.DPDUtils import SetupDPDPreIncludes
dpdPreIncludeUsed = SetupDPDPreIncludes(runArgs,listOfFlags)
if outputRequested and dpdPreIncludeUsed:
    recoLog.error( "Multiple output types requested with pre-includes present" )
    recoLog.error( "This will most probably lead to weird output" )
    pass

# temporary hack (proper fix would be to cleanly protect all DESD building code against missing trigger)
if not rec.doTrigger:
    rec.doDPD.set_Value_and_Lock(False)
    rec.DPDMakerScripts.set_Value_and_Lock([])

#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
if hasattr(runArgs,"topOptions"): include(runArgs.topOptions)
elif rec.DPDMakerScripts()!=[]: include("PrimaryDPDMaker/esdtodpd.py")
else: include( "RecExCommon/RecExCommon_topOptions.py" )

## Make "old style" D3PDs.
for c in SetupOutputDPDs(runArgs, [oldProdFlags]): c()

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

## Post-includes defined for the DPDs:
from PATJobTransforms.DPDUtils import SetupDPDPostIncludes
dpdPostIncludeUsed = SetupDPDPostIncludes(runArgs,listOfFlags)
if outputRequested and dpdPostIncludeUsed:
    recoLog.error( "Multiple output types requested with post-includes present" )
    recoLog.error( "This will most probably lead to weird output" )
    pass

## Post-exec
if hasattr(runArgs,"postExec"):
    recoLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        recoLog.info(cmd)
        exec(cmd)

