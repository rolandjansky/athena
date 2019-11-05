# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from future.utils import iteritems

####################################################################
#
# Skeleton top job options for RDO->RDOprime (RDO with trigger done)
#
#===================================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")
# disable ESD object making, but still enable trigger
rec.doESD.set_Value_and_Lock(False)
rec.doCalo.set_Value_and_Lock(True)
rec.doInDet.set_Value_and_Lock(True)
rec.doMuon.set_Value_and_Lock(True)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(True)
rec.doRDOTrigger.set_Value_and_Lock(True)
recAlgs.doTrigger.set_Value_and_Lock(True)
rec.doMonitoring.set_Value_and_Lock(False)
from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.doMonitoring.set_Value_and_Lock(False)
DQMonFlags.doLArMon.set_Value_and_Lock(False)

#disable offline ID configuration and reco
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doNewTracking.set_Value_and_Lock(False)

from AthenaCommon.CFElements import findAlgorithm, findOwningSequence, findSubSequence
from AthenaCommon.Logging import logging
recoLog = logging.getLogger('rdo_to_rdotrigger')
recoLog.info( '****************** STARTING RDO->RDOTrigger MAKING *****************' )

## Input
if hasattr(runArgs,"inputRDOFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDOFile )
elif hasattr(runArgs,"inputRDO_FILTFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDO_FILTFile )
elif hasattr(runArgs,"inputRDO_FTKFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDO_FTKFile )

    TriggerFlags.doFTK=True;
else:
     raise RuntimeError("No RDO input file specified")

if hasattr(runArgs,"outputRDO_TRIGFile"):
    rec.doWriteRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock( runArgs.outputRDO_TRIGFile )
else:
    raise RuntimeError("No RDO_TRIG output file specified")

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

# Setup the algorithm and output sequences
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AlgSequence import AthSequencer
outSequence = AthSequencer("AthOutSeq")


if not TriggerFlags.doMT():

    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    # small hack, switching temporarily the ESD writing on, to allow writing of some trigger containers into the RDOTrigger file
    rec.doWriteESD = True
    cfg = TriggerConfigGetter()
    rec.doWriteESD.set_Value_and_Lock( False )
    # end of hack.

def preplist(input):
    triglist = []
    for k,val in input.iteritems():
        for j in val:
            triglist.append(k + "#" + j)
    return triglist


if TriggerFlags.doMT():
    TriggerFlags.doHLT.set_Value_and_Lock(False)
    from CaloRec.CaloRecFlags import jobproperties
    jobproperties.CaloRecFlags.doLArNoisyRO.set_Value_and_Lock(False)

#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
if hasattr(runArgs,"topOptions"): include(runArgs.topOptions)
else: include( "RecExCommon/RecExCommon_topOptions.py" )


if TriggerFlags.doMT():
    
    log.info("configuring MT Trigger")
    TriggerFlags.triggerMenuSetup = "LS2_v1"

    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )
    AlgScheduler.EnableVerboseViews( True )
    recoLog.info( "Configuring LVL1 simulation (MT)" )
    from TriggerJobOpts.Lvl1SimulationConfig import Lvl1SimulationSequence
    topSequence += Lvl1SimulationSequence(None)

    recoLog.info( "Configuring HLT (MT)" )
    from TrigConfigSvc.TrigConfigSvcCfg import getHLTConfigSvc
    svcMgr += getHLTConfigSvc()

    from TrigConfigSvc.TrigConfigSvcConfig import TrigConfigSvc
    svcMgr += TrigConfigSvc("TrigConfigSvc")
    svcMgr.TrigConfigSvc.PriorityList = ["none", "ds", "xml"]

    from L1Decoder.L1DecoderConfig import L1Decoder
    topSequence += L1Decoder()
    
    include( "TriggerRelease/jobOfragment_TransBS_standalone.py" )
    topSequence.StreamBS.ItemList =     [ x for x in topSequence.StreamBS.ItemList if 'RoIBResult' not in x ] # eliminate RoIBResult

    # add a fake data dependency assuring that the StreamBS runs before the L1 decoder of HLT
    fakeTypeKey = ("FakeBSOutType","StoreGateSvc+FakeBSOutKey")
    topSequence.StreamBS.ExtraOutputs += [fakeTypeKey]
    l1Decoder = findAlgorithm( topSequence, "L1Decoder" )
    l1Decoder.ExtraInputs += [fakeTypeKey]
    l1Decoder.ctpUnpacker.ForceEnableAllChains=False # this will make HLT respecting L1 chain decisions

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True
    from TriggerJobOpts.TriggerConfig import triggerIDCCacheCreatorsCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.lock()
    triggerIDCCacheCreatorsCfg(ConfigFlags).appendToGlobals()
    Configurable.configurableRun3Behavior=False


    include ("InDetRecExample/InDetRecCabling.py")

    from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT import GenerateMenuMT
    menu = GenerateMenuMT()
    def signaturesToGenerate():
        TriggerFlags.Slices_all_setOff()
        TriggerFlags.EgammaSlice.setAll()
        TriggerFlags.MuonSlice.setAll()
        TriggerFlags.METSlice.setAll()
        TriggerFlags.JetSlice.setAll()
        TriggerFlags.TauSlice.setAll()
        TriggerFlags.BjetSlice.setAll()
        TriggerFlags.CombinedSlice.setAll()
        TriggerFlags.BphysicsSlice.setAll()

    menu.overwriteSignaturesWith(signaturesToGenerate)
    allChainConfigs = menu.generateMT()

    if not hasattr(svcMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr += THistSvc()

    from TriggerJobOpts.HLTTriggerGetter import setTHistSvcOutput
    setTHistSvcOutput(svcMgr.THistSvc.Output)


    

if rec.doFileMetaData():
   from RecExConfig.ObjKeyStore import objKeyStore
   metadataItems = [ "xAOD::TriggerMenuContainer#TriggerMenu",
                 "xAOD::TriggerMenuAuxContainer#TriggerMenuAux." ]
   objKeyStore.addManyTypesMetaData( metadataItems )


from AnalysisTriggerAlgs.AnalysisTriggerAlgsConfig import \
        RoIBResultToAOD
idx=0
for i in topSequence.getAllChildren():
    idx += 1
    if "TrigSteer_HLT" in i.getName():
       if not hasattr(i,'RoIBResultToxAOD'):
           idx += 1
           topSequence.insert(idx, RoIBResultToAOD("RoIBResultToxAOD"))

for i in outSequence.getAllChildren():
    if "StreamRDO" in i.getName() and ( not TriggerFlags.doMT() ):
        from TrigDecisionMaker.TrigDecisionMakerConfig import TrigDecisionMaker,WritexAODTrigDecision
        topSequence.insert(idx, TrigDecisionMaker('TrigDecMaker'))
        from AthenaCommon.Logging import logging
        log = logging.getLogger( 'WriteTrigDecisionToAOD' )
        log.info('TrigDecision writing enabled')
        from xAODTriggerCnv.xAODTriggerCnvConf import xAODMaker__TrigDecisionCnvAlg
        alg = xAODMaker__TrigDecisionCnvAlg()
        alg.AODKey = "TrigDecision"
        alg.xAODKey = "xTrigDecision"
        topSequence.insert(idx+1, alg)
        from xAODTriggerCnv.xAODTriggerCnvConf import xAODMaker__TrigNavigationCnvAlg
        topSequence.insert(idx+2, xAODMaker__TrigNavigationCnvAlg())
        _TriggerESDList = {}
        _TriggerAODList = {}
        from TrigEDMConfig.TriggerEDM import getTriggerEDMList
        _TriggerESDList.update( getTriggerEDMList(TriggerFlags.ESDEDMSet(),  TriggerFlags.EDMDecodingVersion()) )
        _TriggerAODList.update( getTriggerEDMList(TriggerFlags.AODEDMSet(),  TriggerFlags.EDMDecodingVersion()) )


        StreamRDO.ItemList += ["HLT::HLTResult#HLTResult_HLT"]
        StreamRDO.ItemList += ["TrigDec::TrigDecision#TrigDecision"]
        StreamRDO.ItemList += ["TrigInDetTrackTruthMap#*"]
        StreamRDO.ItemList += preplist(_TriggerESDList)
        StreamRDO.ItemList += preplist(_TriggerAODList)
        from TrigEDMConfig.TriggerEDM import getLvl1ESDList
        StreamRDO.ItemList += preplist(getLvl1ESDList())
        from TrigEDMConfig.TriggerEDM import getLvl1AODList
        StreamRDO.ItemList += preplist(getLvl1AODList())
        StreamRDO.MetadataItemList +=  [ "xAOD::TriggerMenuContainer#*", "xAOD::TriggerMenuAuxContainer#*" ]

    if "StreamRDO" in i.getName() and TriggerFlags.doMT():

        ### Produce the trigger bits:
        from TrigOutputHandling.TrigOutputHandlingConf import TriggerBitsMakerTool
        from TrigDecisionMaker.TrigDecisionMakerConfig import TrigDecisionMakerMT
        bitsmakerTool = TriggerBitsMakerTool()
        tdm = TrigDecisionMakerMT('TrigDecMakerMT') # Replaces TrigDecMaker and finally deprecates Run 1 EDM
        tdm.BitsMakerTool = bitsmakerTool
        topSequence += tdm
        log.info('xTrigDecision writing enabled')

        ### Produce the metadata:
        from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODMenuWriterMT
        md = TrigConf__xAODMenuWriterMT()
        topSequence += md
        log.info('TriggerMenu Metadata writing enabled')

        # Still need to produce Run-2 style L1 xAOD output
        topSequence += RoIBResultToAOD("RoIBResultToxAOD")

        from TrigEDMConfig.TriggerEDM import getTriggerEDMList

        trigEDMListESD = {}
        trigEDMListAOD = {}
        # do the two lines above this need to be changed to this?
        trigEDMListESD.update(getTriggerEDMList(TriggerFlags.ESDEDMSet(),  3) )
        trigEDMListAOD.update(getTriggerEDMList(TriggerFlags.AODEDMSet(),  3) ) 

        StreamRDO.ItemList += preplist(trigEDMListESD)
        StreamRDO.ItemList += preplist(trigEDMListAOD)

        from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects
        hypos = collectHypos( findSubSequence(topSequence, "HLTAllSteps") )
        filters = collectFilters( findSubSequence(topSequence, "HLTAllSteps") )
        decObj = collectDecisionObjects( hypos, filters, 
            findAlgorithm(topSequence, "L1Decoder"), 
            findAlgorithm(topSequence, "DecisionSummaryMakerAlg") )
        StreamRDO.ItemList += [ "xAOD::TrigCompositeContainer#"+obj for obj in decObj ]
        StreamRDO.ItemList += [ "xAOD::TrigCompositeAuxContainer#"+obj+"Aux." for obj in decObj ]
        StreamRDO.MetadataItemList +=  [ "xAOD::TriggerMenuContainer#*", "xAOD::TriggerMenuAuxContainer#*" ]

from AthenaCommon.AppMgr import ServiceMgr, ToolSvc


if hasattr(ToolSvc, 'TrigDecisionTool'):
    if TriggerFlags.doMT():
        # No functional TDT in MT in RDOtoRDOTrigger
        pass
    else:
    	# Causes TDT to use Run-1 style behaviour in this part of the transform
        from TrigDecisionTool.TrigDecisionToolConf import *
        ToolSvc.TrigDecisionTool.TrigDecisionKey = "TrigDecision"
        ToolSvc.TrigDecisionTool.UseAODDecision = True

if TriggerFlags.doMT():
    # inform TD maker that some parts may be missing
    if TriggerFlags.dataTakingConditions()=='Lvl1Only':
        topSequence.TrigDecMakerMT.doHLT=False
    elif TriggerFlags.dataTakingConditions()=='HltOnly':
        topSequence.TrigDecMakerMT.doL1=False
else:
    # inform TD maker that some parts may be missing
    if TriggerFlags.dataTakingConditions()=='Lvl1Only':
        topSequence.TrigDecMaker.doL2=False
        topSequence.TrigDecMaker.doEF=False
        topSequence.TrigDecMaker.doHLT=False
    elif TriggerFlags.dataTakingConditions()=='HltOnly':
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence.TrigDecMaker.doL1=False
        # Decide based on the run number whether to assume a merged, or a
        # split HLT:
    if not TriggerFlags.doMergedHLTResult():
        topSequence.TrigDecMaker.doHLT = False
    else:
        topSequence.TrigDecMaker.doL2 = False
        topSequence.TrigDecMaker.doEF = False


#unconditinally store these items
StreamRDO.ItemList += [ "DataVector<LVL1::TriggerTower>#TriggerTowers" ]
StreamRDO.ItemList += [ "TRT_RDO_Container#TRT_RDOs" ]
StreamRDO.ItemList += [ "SCT_RDO_Container#SCT_RDOs" ]
StreamRDO.ItemList += [ "PixelRDO_Container#PixelRDOs" ]
StreamRDO.ItemList +=["2721#*"]
StreamRDO.ItemList +=["2927#*"]
StreamRDO.ItemList +=["2934#*"]

rec.OutputFileNameForRecoStep="RDOtoRDO_TRIG"

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

ServiceMgr.MessageSvc.debugLimit=10000000
ServiceMgr.MessageSvc.Format = "% F%40W%S%4W%e%s%7W%R%T %0W%M"
#from AthenaCommon.Constants import DEBUG
#findAlgorithm( topSequence, "TauL2CaloHypo").OutputLevel=DEBUG
#findAlgorithm( topSequence, "TrigTauRecMerged_TauPrecisionMVA").OutputLevel=DEBUG


import AthenaCommon.Configurable as Configurable
Configurable.log.setLevel( INFO )
print topSequence

