# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging

logAODFix_r160 = logging.getLogger( 'AODFix_r160' )
    
# inside RecExCommon_topOptions.py:
def AODFix_r160_preInclude():
    logAODFix_r160.info("executing preInclude")
    
    #-----------------------------------------------------------------
    #Specific Rec
    from AthenaCommon.JobProperties import jobproperties
    
    from RecExConfig.RecAlgsFlags  import recAlgs
    from RecExConfig.RecFlags  import rec
    # o.b.o. Jovan, disable fixup
    # rec.doEgamma.set_Value_and_Lock(True)
    # MissingET done in AODFix_postMissingETRec
    # recAlgs.doMissingET.set_Value_and_Lock(True)
    recAlgs.doMuonSpShower.set_Value_and_Lock(False)
    
    #-----------------------------------------------------------------
    # Muons reconstruction standalone
    from MuonRecExample.MuonRecFlags import muonRecFlags,mooreFlags,muonboyFlags
    jobproperties.MuonRec.Enabled.set_Value_and_Lock(False)
    muonRecFlags.doMuonboy.set_Value_and_Lock(False)
    muonRecFlags.doMoore.set_Value_and_Lock(False)
    muonRecFlags.doCalib.set_Value_and_Lock(False)
    muonRecFlags.doMDTs.set_Value_and_Lock(False)
    muonRecFlags.doCSCs.set_Value_and_Lock(False)
    muonRecFlags.doRPCs.set_Value_and_Lock(False)
    muonRecFlags.doTGCs.set_Value_and_Lock(False)
    muonRecFlags.useTGCPriorNextBC.set_Value_and_Lock(False)
    muonRecFlags.useAlignmentCorrections.set_Value_and_Lock(True)
    muonRecFlags.doSegmentT0Fit.set_Value_and_Lock(False)
    muonboyFlags.DumpOnly.set_Value_and_Lock(False)
    
    #-----------------------------------------------------------------
    # run combined muon reconstruction 
    from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags 
    jobproperties.MuonCombinedRec.Enabled.set_Value_and_Lock(True)
    muonCombinedRecFlags.doMuGirl.set_Value_and_Lock(False)
    muonCombinedRecFlags.doMuGirlLowBeta.set_Value_and_Lock(False)
    muonCombinedRecFlags.doCaloTrkMuId.set_Value_and_Lock(False)
    muonCombinedRecFlags.doMuTagIMO.set_Value_and_Lock(False)
    muonCombinedRecFlags.doMuidStandalone.set_Value_and_Lock(False)
    muonCombinedRecFlags.doMuidCombined.set_Value_and_Lock(False)
    muonCombinedRecFlags.doMuidVertexAssociator.set_Value_and_Lock(False)
    muonCombinedRecFlags.doExtrapolateMuonToIP.set_Value_and_Lock(False)
    muonCombinedRecFlags.doCombinedMuonFit.set_Value_and_Lock(False)
    muonCombinedRecFlags.doMuTagMoore.set_Value_and_Lock(False)
    
    #-----------------------------------------------------------------
    # ID 
    rec.doInDet.set_Value_and_Lock(True)
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.Enabled.set_Value_and_Lock(True)
    InDetFlags.disableInDetReco.set_Value_and_Lock(True)
    
    muonCombinedRecFlags.doStacoMoore.set_Value_and_Lock(False)
    muonCombinedRecFlags.doMboyCombined.set_Value_and_Lock(True)
    muonCombinedRecFlags.doStaco.set_Value_and_Lock(True)
    muonCombinedRecFlags.doMuTag.set_Value_and_Lock(True)
    muonCombinedRecFlags.doStacoMuonCollection.set_Value_and_Lock(True)
    
    #-----------------------------------------------------------------
    # Egamma
    # o.b.o. Jovan, disable fixup
    # from egammaRec.egammaRecFlags import jobproperties
    # jobproperties.egammaRecFlags.doAODRender = True
        
def AODFix_r160_postAtlfast():
    return

def AODFix_r160_postTrigger():
    return

def AODFix_r160_postSystemRec():
    return

def AODFix_r160_postCombinedRec():
    logAODFix_r160.info("executing postCombinedRec")
    
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AlgSequence import AlgSequence

    topSequence = AlgSequence()
    from RecExConfig.RecFlags import rec
    #-----------------------------------------------------------------
    #Use AOD container
    if rec.readAOD():
       logAODFix_r160.info("Switching to AOD Cell and Cluster containers")
       ToolSvc.TrackDepositInCaloTool.CaloCellContainerName = "AODCellContainer"
    else:
       logAODFix_r160.info("Use ESD Cell and Cluster containers")
    ToolSvc.MuonIsolationTool.CaloClusterCollection = "CaloCalTopoCluster"
    #-----------------------------------------------------------------
    #write muon calo information into seperate containers, and merge them afterwords
    ToolSvc.MuonIsolationTool.MuonCaloEnergyCollection = "MuonCaloEnergyCollection_NEW"
    ToolSvc.MuonIsolationTool.MuonClusterCollection = "MuonClusterCollection_NEW"
    topSequence.InitializeMuonCaloEnergy.MuonCaloEnergyContainer="MuonCaloEnergyCollection_NEW"
    topSequence.InitializeMuonClusters.MuonClusterContainer="MuonClusterCollection_NEW"
    topSequence.FinalizeMuonClusters.MuonClusterContainer="MuonClusterCollection_NEW"
    from MuonSpShowerBuilderAlgs.MuonSpShowerBuilderAlgsConf import MergeMuonCaloEnergyContainers
    from MuonSpShowerBuilderAlgs.MuonSpShowerBuilderAlgsConf import MergeMuonCaloClusterContainers
    alg_nr=0
    for alg in topSequence:
        alg_nr+=1
        if alg.getName()=="FinalizeMuonClusters":
            mergeMuonCaloContainers = MergeMuonCaloEnergyContainers()
            mergeMuonCaloContainers.MuonCaloEnergyContainerTarget = "MuonCaloEnergyCollection"
            mergeMuonCaloContainers.NewMuonCollection = "StacoMuonCollection"
            mergeMuonClusterContainers = MergeMuonCaloClusterContainers()
            mergeMuonClusterContainers.MuonCaloClusterContainerTarget = "MuonClusterCollection"
            mergeMuonClusterContainers.MuonCaloClusterContainerSource = "MuonClusterCollection_NEW"
            mergeMuonClusterContainers.NewMuonCollection = ["StacoMuonCollection"]
            if rec.doWriteESD():
               mergeMuonClusterContainers.NewMuonCollection.append( "StacoESDMuonCollection" )
               mergeMuonCaloContainersESD = MergeMuonCaloEnergyContainers("mergeMuonCaloContainersESD")
               mergeMuonCaloContainersESD.MuonCaloEnergyContainerTarget = "MuonCaloEnergyCollection"
               mergeMuonCaloContainersESD.NewMuonCollection = "StacoESDMuonCollection"
               topSequence.insert(alg_nr, mergeMuonCaloContainersESD)
            topSequence.insert(alg_nr, mergeMuonCaloContainers)
            topSequence.insert(alg_nr, mergeMuonClusterContainers)
            break
	
    #-----------------------------------------------------------------
    #Force InDetTrackSummaryHelperTool
    ToolSvc.InDetSummaryHelper.usePixel  = True
    ToolSvc.InDetSummaryHelper.useSCT    = True
    ToolSvc.InDetSummaryHelper.useTRT    = True
    
    #-----------------------------------------------------------------
    #switch off CaloDynamic in AtlasExtrapolator
    ToolSvc.AtlasExtrapolator.DoCaloDynamic=False

def AODFix_r160_postHeavyIon():
    return

# inside SystemRec_config.py:
def AODFix_r160_postCaloRec():
    return

def AODFix_r160_postInDetRec():
    return

def AODFix_r160_postMuonRec():
    return

def AODFix_r160_postForwardRec():
    return

# inside CombinedRec_config.py:
def AODFix_r160_postEgammaRec():
    return

def AODFix_r160_postMuonCombinedRec():
    logAODFix_r160.info("executing postMuonCombinedRec")

    from AthenaCommon.AlgSequence import AlgSequence
    
    from RecExConfig.RecFlags import rec
    from ParticleBuilderOptions.AODFlags import AODFlags
    from AthenaCommon.Resilience import protectedInclude
    if rec.doWriteAOD() and rec.readAOD():
        from AthenaServices.Configurables import ThinningSvc
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr, 'ThinningSvc'):
            svcMgr += ThinningSvc()
        protectedInclude( "MuonAlgs/MuonTrackSlimmer_jobOptions.py" )
    
        topSequence = AlgSequence()
        #-----------------------------------------------------------------
        print topSequence
        #Work on MuonTrackSlimmer
        topSequence.MuonTrackSlimmer.MuonContainerNames = ["StacoMuonCollection"]
        topSequence.MuonTrackSlimmer.doAppendToExisting=True
    
def AODFix_r160_postJetRec():
    return

def AODFix_r160_posttauRec():
    return

def AODFix_r160_postMissingETRec():
    logAODFix_r160.info("executing postMissingETRec")
    
    from RecExConfig.ObjKeyStore import objKeyStore
    objKeyStore['inputFile'].removeItem(["MissingET#MET_MuonBoy_Track"])
    objKeyStore['inputFile'].removeItem(["MissingET#MET_MuonBoy_Spectro"])
    objKeyStore['inputFile'].removeItem(["MissingET#MET_MuonBoy"])
    objKeyStore['inputFile'].removeItem(["MissingET#MET_Muon"])
    objKeyStore['inputFile'].removeItem(["MissingET#MET_RefFinal"])
    
    from MissingET.METGetterMuon import METGetterMuon
    METGetterMuon()
    from MissingET.METGetterAOD import METGetterAOD
    METGetterAOD()
