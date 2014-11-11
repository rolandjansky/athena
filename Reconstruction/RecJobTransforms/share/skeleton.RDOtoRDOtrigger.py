####################################################################
#
# Skeleton top job options for RDO->RDOprime (RDO with trigger done)
#
#===================================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("PATJobTransforms/CommonSkeletonJobOptions.py")
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

if hasattr(runArgs,"outputRDO_TRIGFile"):
    rec.doWriteRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock( runArgs.outputRDO_TRIGFile )
else:
    recoLog.warning("No RDO output file specified")

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
else: include( "RecExCommon/RecExCommon_topOptions.py" )

from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecision
trigDecWriter = WriteTrigDecision()
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

from TrigEDMConfig.TriggerEDM import getLvl1ESDList, getESDList, \
                  getAODList, getLvl1AODList, getTrigIDTruthList
esdList = getESDList()
aodList = getAODList()
l1EsdList = getLvl1ESDList()
l1AodList = getLvl1AODList()
# In HLTTriggerResultGetter.py filled only when rec.doESD or rec.doAOD
trigIDTruthESD = getTrigIDTruthList(TriggerFlags.ESDEDMSet())
trigIDTruthAOD = getTrigIDTruthList(TriggerFlags.AODEDMSet())

def fillTrigList(inlist):
    triglist = []
    for k in inlist:
      items = inlist[k]
      for j in items:
          triglist.append( k + "#" + j)
    return triglist

triglists = fillTrigList(esdList)
triglists += fillTrigList(aodList)
triglists += fillTrigList(l1EsdList)
triglists += fillTrigList(l1AodList)
triglists += fillTrigList(trigIDTruthESD)
triglists += fillTrigList(trigIDTruthAOD)

StreamRDO.ItemList += list(set(triglists))

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
        
