###############################################################
#
# Skeleton top job options for ESD->AOD
# Put here outputs that require rec.doAOD=True
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("PATJobTransforms/CommonSkeletonJobOptions.py")
rec.doAOD=True

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('aod_to_aod')
recoLog.info( '****************** STARTING AOD->AOD MAKING *****************' )

## Input
if hasattr(runArgs,"inputFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
# print 'Debug 1.  runArgs.inputFile: {0}'.format(runArgs.inputFile)
if hasattr(runArgs,"inputAODFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readRDO.set_Value_and_Lock( False )
    rec.readESD.set_Value_and_Lock( False )
    rec.readAOD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDInput.set_Value_and_Lock( runArgs.inputAODFile )

## Outputs
if hasattr(runArgs,"outputAODFile"):
    rec.doAOD.set_Value_and_Lock( True )
    rec.doWriteAOD.set_Value_and_Lock( True ) 
    athenaCommonFlags.PoolAODOutput.set_Value_and_Lock( runArgs.outputAODFile )

if hasattr(runArgs,"outputTAGFile"):
    # should be used as outputTAGFile_e2a=myTAG.root so that it does not trigger AODtoTAG
    # if writing TAG file, need AOD object in any case
    rec.doAOD.set_Value_and_Lock( True )
    rec.doWriteTAG.set_Value_and_Lock( True )
    athenaCommonFlags.PoolTAGOutput.set_Value_and_Lock( runArgs.outputTAGFile )

if hasattr(runArgs,"tmpAOD"):
    rec.doAOD.set_Value_and_Lock( True )
    rec.doWriteAOD.set_Value_and_Lock( True ) 
    athenaCommonFlags.PoolAODOutput.set_Value_and_Lock( runArgs.tmpAOD )

if hasattr(runArgs,"outputNTUP_TRIGFile"):
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.NtupleProductionFlags.ProductionLocation='Tier0'
    TriggerFlags.NtupleProductionFlags.FileNames.set_Value_and_Lock( [runArgs.outputNTUP_TRIGFile] )

if hasattr(runArgs,"outputCBNTFile"):
    rec.doCBNT.set_Value_and_Lock(True)
    rec.RootNtupleOutput.set_Value_and_Lock( runArgs.outputCBNTFile ) 

if hasattr(runArgs,"outputDQMonitorFile"):
    rec.doMonitoring.set_Value_and_Lock(True)
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.histogramFile.set_Value_and_Lock( runArgs.outputDQMonitorFile )

if hasattr(runArgs,"outputNTUP_BTAGFile"):
    from BTagging.BTaggingFlags import BTaggingFlags
    BTaggingFlags.doJetTagNtuple = True
    BTaggingFlags.JetTagNtupleName = runArgs.outputNTUP_BTAGFile

if hasattr(runArgs, "outputNTUP_HIGHMULTFile"):
    from TrigMbD3PDMaker.TrigMbD3PDMakerFlags import trigMbD3PDflags
    trigMbD3PDflags.FileName=runArgs.outputNTUP_HIGHMULTFile
    include("TrigMbD3PDMaker/HighMultD3PD_jobOptions.py")

if hasattr(runArgs,"outputNTUP_ENHBIASFile"):
    from TrigCostAthena.TrigCostAthenaFlags import TrigCostAthenaFlags
    TrigCostAthenaFlags.StoreNtVerticesOutputFile.set_Value_and_Lock( runArgs.outputNTUP_ENHBIASFile )
    TrigCostAthenaFlags.DoStoreNtVertices.set_Value_and_Lock( True )
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )
    if hasattr(runArgs,"inputAODFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputAODFile )
    include("TrigCostAthena/ESDtoNTUP_ENHBIAS.py")

if hasattr(runArgs,"outputHIST_PHYSVALMONFile"):
    rec.doPhysValMonHists=True
    
    ## Setup the output file(s):
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr += THistSvc()
    output=svcMgr.THistSvc.Output
    svcMgr.THistSvc.Output+= ["PhysValMon DATAFILE='"+runArgs.outputHIST_PHYSVALMONFile+"' OPT='RECREATE'"]
    include("PhysValMon/PhysValMon_RecoOpt.py")

## Pre-exec
if hasattr(runArgs,"preExec"):
    recoLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        recoLog.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        print "preInclude",fragment
        include(fragment)

rec.OutputFileNameForRecoStep="AODtoAOD"

#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
if hasattr(runArgs,"topOptions"): include(runArgs.topOptions)
else: include( "RecExCommon/RecExCommon_topOptions.py" )


#D3PDMaker outputs
if hasattr(runArgs,"outputNTUP_MINBIASFile"):
    from MinBiasD3PDMaker.MinBiasD3PDMakerFlags import minbiasD3PDflags
    minbiasD3PDflags.FileName=runArgs.outputNTUP_MINBIASFile
    include("MinBiasD3PDMaker/MinBiasD3PD_jobOptions.py")

if hasattr(runArgs,"outputDESDM_BEAMSPOTFile"):
    #needs to be used with: preInclude=InDetBeamSpotFinder/BeamSpotRecoPreInclude_standard.py
    from InDetBeamSpotFinder import BeamSpotDPDFlags 
    primDPD.WriteDESDM_BEAMSPOTStream.FileName=runArgs.outputDESDM_BEAMSPOTFile
    primDPD.WriteDESDM_BEAMSPOTStream.set_Value_and_Lock( True )
    include("InDetBeamSpotFinder/DESDM_BEAMSPOTFragment.py")

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
        
