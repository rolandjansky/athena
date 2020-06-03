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
    from TriggerJobOpts.TriggerFlags import TriggerFlags
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

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# small hack, switching temporarily the ESD writing on, to allow writing of some trigger containers into the RDOTrigger file
rec.doWriteESD = True 
cfg = TriggerConfigGetter()
rec.doWriteESD.set_Value_and_Lock( False )
# end of hack. 

#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
if hasattr(runArgs,"topOptions"): include(runArgs.topOptions)
else: include( "RecExCommon/RecExCommon_topOptions.py" )

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
           topSequence.insert(idx+1, RoIBResultToAOD("RoIBResultToxAOD"))
    if "StreamRDO" in i.getName():
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

from AthenaCommon.AppMgr import ServiceMgr, ToolSvc
from TrigDecisionTool.TrigDecisionToolConf import *

if hasattr(ToolSvc, 'TrigDecisionTool'):
    ToolSvc.TrigDecisionTool.TrigDecisionKey = "TrigDecision"
    ToolSvc.TrigDecisionTool.UseAODDecision = True

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

_TriggerESDList = {}
_TriggerAODList = {}
from TrigEDMConfig.TriggerEDM import getTriggerEDMList
_TriggerESDList.update( getTriggerEDMList(TriggerFlags.ESDEDMSet(),  TriggerFlags.EDMDecodingVersion()) )
_TriggerAODList.update( getTriggerEDMList(TriggerFlags.AODEDMSet(),  TriggerFlags.EDMDecodingVersion()) )

def preplist(input):
   triglist = []
   for k,val in input.iteritems():
      for j in val:
         triglist.append(k + "#" + j)
   return triglist

StreamRDO.ItemList += [ "xAOD::EventInfo#*", "xAOD::EventAuxInfo#*" ]

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
