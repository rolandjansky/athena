#--------------------------------
# Heavy Ion main switch settings
#--------------------------------
include.block ('HIRecExample/HIPRec_jobOptions.py')

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from HIRecExample.HIRecExampleFlags import jobproperties
if (rec.doESD() or jobproperties.HIRecExampleFlags.doHIReReco()) and not athenaCommonFlags.isOnline():

    if jobproperties.HIRecExampleFlags.doHIReReco() : include("CaloRec/CaloTopoCluster_jobOptions.py")
    # moments
    include('CaloRec/CaloClusterSignificance_jobOption.py')

    #flow
    from HIGlobal.HIGlobalFlags import jobproperties
    if jobproperties.HIGlobalFlags.doHIFlow:
        from HIGlobal.HIGlobalConf import HIFlow
        for iharmonic in range(jobproperties.HIGlobalFlags.FlowHarmonicMin.get_Value(),jobproperties.HIGlobalFlags.FlowHarmonicMax.get_Value()+1) :
            flowObject = HIFlow("HIFlowAlg_v%i" % iharmonic)
            flowObject.HIFlowDataContainerName= jobproperties.HIGlobalFlags.HIFlowContainerPrefix.get_Value() + str(iharmonic)
            flowObject.Harmonic=iharmonic
            #if globalflags.DataSource == "data": flowObject.isData = True
            flowObject.isData = False
            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()
            topSequence += flowObject
            from RecExConfig.ObjKeyStore import objKeyStore
            objKeyStore.addStreamESD('HIFlowData',flowObject.HIFlowDataContainerName)
      

    #Jets
    from HIRecExample.HIRecExampleFlags import jobproperties
    if jobproperties.HIRecExampleFlags.doHIJetRec:
        include('HIJetRec/HeavyIonPPMode_jobOptions.py')
        if rec.doTruth:
            if jobproperties.HIJetRecFlags.doHIJetMergeEvent : include( "HIJetRec/EmbeddedTruthJets_jobOptions.py" )
            if jobproperties.HIJetRecFlags.doHIJetHijingTruth : include ("HIJetRec/HijingTruthJets_jobOptions.py")
