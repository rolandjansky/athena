# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Transform skeleton for RDO -> RDO_TRIG (running trigger and adding its output to the RDO file).
# These are old job options for Run-2 Trigger. For Run-3 see RDOtoRDOtriggerMT.py
#

from AthenaCommon.Include import include
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from TriggerJobOpts.TriggerFlags import TriggerFlags
import six

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

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('rdo_to_rdotrigger')
recoLog.info( '****************** STARTING RDO->RDOTrigger MAKING *****************' )

## Input
runArgs = globals()['runArgs']
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

    TriggerFlags.doFTK = True
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

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
# small hack, switching temporarily the ESD writing on, to allow writing of some trigger containers into the RDOTrigger file
rec.doWriteESD = True
cfg = TriggerConfigGetter()
rec.doWriteESD.set_Value_and_Lock( False )
# end of hack.

def preplist(input):
    triglist = []
    for k,val in six.iteritems (input):
        for j in val:
            triglist.append(k + "#" + j)
    return triglist


#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
if hasattr(runArgs,"topOptions"):
    include(runArgs.topOptions)
else:
    include( "RecExCommon/RecExCommon_topOptions.py" )


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

_streamRDO = None
for i in outSequence.getAllChildren():
    if 'StreamRDO' in i.getName():
        _streamRDO = i

if _streamRDO:
    from TrigDecisionMaker.TrigDecisionMakerConfig import TrigDecisionMaker
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
    trigNavCnv = xAODMaker__TrigNavigationCnvAlg()
    trigNavCnv.doL2 = False
    trigNavCnv.doEF = False
    trigNavCnv.doHLT = True
    topSequence.insert(idx+2, trigNavCnv)
    _TriggerESDList = {}
    _TriggerAODList = {}
    from TrigEDMConfig.TriggerEDM import getTriggerEDMList
    _TriggerESDList.update( getTriggerEDMList(TriggerFlags.ESDEDMSet(),  TriggerFlags.EDMDecodingVersion()) )
    _TriggerAODList.update( getTriggerEDMList(TriggerFlags.AODEDMSet(),  TriggerFlags.EDMDecodingVersion()) )

    _streamRDO.ItemList += ["HLT::HLTResult#HLTResult_HLT"]
    _streamRDO.ItemList += ["TrigDec::TrigDecision#TrigDecision"]
    _streamRDO.ItemList += ["TrigInDetTrackTruthMap#*"]
    _streamRDO.ItemList += preplist(_TriggerESDList)
    _streamRDO.ItemList += preplist(_TriggerAODList)
    from TrigEDMConfig.TriggerEDM import getLvl1ESDList
    _streamRDO.ItemList += preplist(getLvl1ESDList())
    from TrigEDMConfig.TriggerEDM import getLvl1AODList
    _streamRDO.ItemList += preplist(getLvl1AODList())
    _streamRDO.MetadataItemList +=  [ "xAOD::TriggerMenuContainer#*", "xAOD::TriggerMenuAuxContainer#*" ]

from AthenaCommon.AppMgr import ServiceMgr, ToolSvc


if hasattr(ToolSvc, 'TrigDecisionTool'):
    # Causes TDT to use Run-1 style behaviour in this part of the transform
    from TrigDecisionTool.TrigDecisionToolConf import *  # noqa: F401,F403 old Run-2 code
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

if _streamRDO:
    # always store these items
    _streamRDO.ItemList += [ "DataVector<LVL1::TriggerTower>#TriggerTowers" ]
    _streamRDO.ItemList += [ "TRT_RDO_Container#TRT_RDOs" ]
    _streamRDO.ItemList += [ "SCT_RDO_Container#SCT_RDOs" ]
    _streamRDO.ItemList += [ "PixelRDO_Container#PixelRDOs" ]
    _streamRDO.ItemList +=["2721#*"]
    _streamRDO.ItemList +=["2927#*"]
    _streamRDO.ItemList +=["2934#*"]
    _streamRDO.ItemList += [ "xAOD::EventInfo#*", "xAOD::EventAuxInfo#*" ]
    # Never store wildcarded legacy EventInfo
    if "EventInfo#*" in _streamRDO.ItemList:
        _streamRDO.ItemList.remove("EventInfo#*")

condSeq = AthSequencer("AthCondSeq")
if not hasattr( condSeq, "LumiBlockMuWriter" ):
    include ("LumiBlockComps/LumiBlockMuWriter_jobOptions.py")

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

if recoLog.getEffectiveLevel() <= logging.DEBUG:
    import AthenaCommon.Configurable as Configurable
    Configurable.log.setLevel(logging.INFO)
    recoLog.debug('Printing topSequence:\n%s', topSequence)
