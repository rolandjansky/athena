from __future__ import print_function
###############################################################
#
# Skeleton top job options for AOD->DPD 
# Put here outputs that require rec.doAOD=False
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")
rec.doAOD=False

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('aod_to_dpd')
recoLog.info( '****************** STARTING AOD->DPD MAKING *****************' )


## Automatically turn ON/OFF and set output file name of each possible DPD
listOfFlags=[]
try:
    from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
    listOfFlags.append(primDPD)
except ImportError:
    print("WARNING PrimaryDPDFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project.")
try:
    from D3PDMakerConfig.D3PDProdFlags import prodFlags
    listOfFlags.append( prodFlags )
except ImportError:
    print("WARNING D3PDProdFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project.")

from PATJobTransforms.DPDUtils import SetupOutputDPDs
rec.DPDMakerScripts.append(SetupOutputDPDs(runArgs,listOfFlags))

# New-style config
from AthenaConfiguration.AllConfigFlags import ConfigFlags

## Input
if hasattr(runArgs,"inputFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
if hasattr(runArgs,"inputEVNTFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputEVNTFile )
if hasattr(runArgs,"inputAODFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readAOD.set_Value_and_Lock( True )
    rec.readRDO.set_Value_and_Lock( False )
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )
    ConfigFlags.Input.Files  = athenaCommonFlags.PoolAODInput()
if hasattr(runArgs,"inputTAGFile") or hasattr(runArgs,"inputTAG_AODFile"):
    #for TAG->AOD->skimmedAOD
    rec.readTAG.set_Value_and_Lock( True )
    rec.readAOD.set_Value_and_Lock( True )
    rec.doAOD.set_Value_and_Lock( False )
    rec.TAGFromRDO.set_Value_and_Lock( False )
        
    if hasattr(runArgs,"inputTAGFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputTAGFile )
    else:
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputTAG_AODFile )

## Outputs
if hasattr(runArgs,"outputAODFile"):
    #for TAG->AOD->skimmedAOD
    rec.doWriteAOD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolAODOutput.set_Value_and_Lock( runArgs.outputAODFile )

if hasattr(runArgs,"outputNTUP_BTAGFile"):
    from BTagging.BTaggingFlags import BTaggingFlags
    BTaggingFlags.doJetTagNtuple = True
    BTaggingFlags.JetTagNtupleName = runArgs.outputNTUP_BTAGFile

if hasattr(runArgs,"outputNTUP_PROMPTPHOTFile"):
    from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
    PAUflags.FileName = runArgs.outputNTUP_PROMPTPHOTFile
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputAODFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputAODFile )

if hasattr(runArgs,"outputNTUP_SMEWFile"):
    from WWAnalyze.WWD3PDFlags import WWD3PDFlags
    WWD3PDFlags.OutputFilename = runArgs.outputNTUP_SMEWFile
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputAODFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputAODFile )

if hasattr(runArgs,"outputNTUP_TRUTHFile"):
    from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags
    TruthD3PDFlags.TruthD3PDOutputFileName = runArgs.outputNTUP_TRUTHFile
    include("TruthD3PDMaker/TruthD3PDfromEVGEN_preInclude.py")
    
if hasattr(runArgs,"outputNTUP_SUSYTRUTHFile"):
    from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags
    TruthD3PDFlags.TruthD3PDOutputFileName = runArgs.outputNTUP_SUSYTRUTHFile
    include("TruthD3PDMaker/TruthSusyD3PDfromEVGEN_preInclude.py")

if hasattr(runArgs,"outputDAOD_2LHSG2File"):
    #FIXME: input/outputs should configured via job properties instead of directly using the runArgs object
    from HSG2DPDUtils import HSG2DPDFlags
if hasattr(runArgs,"outputDAOD_HSG2File"):
    #FIXME: input/outputs should configured via job properties instead of directly using the runArgs object
    from HSG2DPDUtils import HSG2DPDFlags


if hasattr(runArgs,"outputNTUP_1LHSG2File"):
    #FIXME: input/outputs should configured via job properties instead of directly using the runArgs object
    from HSG2DPDUtils import HSG2DPDFlags
if hasattr(runArgs,"outputNTUP_2LHSG2File"):
    #FIXME: input/outputs should configured via job properties instead of directly using the runArgs object
    from HSG2DPDUtils import HSG2DPDFlags
if hasattr(runArgs,"outputNTUP_HSG2File"):
    #FIXME: input/outputs should configured via job properties instead of directly using the runArgs object
    from HSG2DPDUtils import HSG2DPDFlags


 
if hasattr(runArgs,"outputNTUP_SCTFile"):
    from TrackD3PDMaker.TrackD3PDMakerSCTFlags import TrackD3PDSCTFlags
    TrackD3PDSCTFlags.outputFile = runArgs.outputNTUP_SCTFile
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )


if hasattr(runArgs,"outputNTUP_FASTMONFile"):
    rec.doFastPhysMonitoring.set_Value_and_Lock(True)
    rec.RootFastPhysMonOutput.set_Value_and_Lock(runArgs.outputNTUP_FASTMONFile)

if hasattr(runArgs,"outputNTUP_MCPTPFile"):
    from MuonPerformanceAlgs import MuonTPAnalysis
    MuonTPAnalysis.MuonTPConfig(runArgs.outputNTUP_MCPTPFile)

if hasattr(runArgs,"outputNTUP_MCPScaleFile"):
    from MuonPtCalibNtupleMaker import MuonPtCalibNtupleMaker_Config
    MuonPtCalibNtupleMaker_Config.MuonScaleConfig(runArgs.outputNTUP_MCPScaleFile)



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
dpdPreInclude = SetupDPDPreIncludes(runArgs, listOfFlags)
# Examine if we have more output types
if not isinstance(dpdPreInclude, list):
    recoLog.warning("Old return value from SetupDPDPostIncludes() - no check for output incompatibility done")
else:
    additionalOutputs = set( [ outfile for outfile in dir(runArgs) if outfile.startswith('output') and outfile.endswith('File') ] ) - set(dpdPreInclude)
    if len(dpdPreInclude) > 0 and len(additionalOutputs) > 0:
        recoLog.error( "Multiple output types requested with pre-includes present" )
        recoLog.error( "Pre-includes came from {0}, with {1} also requested".format(dpdPreInclude, list(additionalOutputs)) )
        recoLog.error( "This will most probably lead to weird output" )

#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
if hasattr(runArgs,"topOptions"): include(runArgs.topOptions)
else: include( "RecExCommon/RecExCommon_topOptions.py" )

try:
    #Import D3PD flags before preExec, for convenience
    from D3PDMakerConfig.D3PDProdFlags  import oldProdFlags
    ## Make "old style" D3PDs.
    for c in SetupOutputDPDs(runArgs, [oldProdFlags]): c()
except ImportError:
    print("WARNING: Old prod flags could not be included this. Only OK if you're using job transforms without the AtlasAnalysis project.")
    
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
dpdPostIncludeUsed = SetupDPDPostIncludes(runArgs, listOfFlags)
if not isinstance(dpdPostIncludeUsed, list):
    recoLog.warning("Old return value from SetupDPDPostIncludes() - no check for output incompatibility done")
else:
    additionalOutputs = set( [ outfile for outfile in dir(runArgs) if outfile.startswith('output') and outfile.endswith('File') ] ) - set(dpdPostIncludeUsed)
    if len(dpdPostIncludeUsed) > 0 and len(additionalOutputs) > 0:
        recoLog.error( "Multiple output types requested with post-includes present" )
        recoLog.error( "Post-includes came from {0}, with {1} also requested".format(dpdPostIncludeUsed, list(additionalOutputs)) )
        recoLog.error( "This will most probably lead to weird output" )

## Post-exec
if hasattr(runArgs,"postExec"):
    recoLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        recoLog.info(cmd)
        exec(cmd)

