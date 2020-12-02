# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##-----------------------------------------------------------------------------
## Name: PrimaryDPD_BasicOutput.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the basic output containers to be added to 
##              every output stream.
##
##-----------------------------------------------------------------------------


## for messaging
from AthenaCommon.Logging import logging
primaryDPD_BasicOutput_msg = logging.getLogger( 'PrimaryDPD_BasicOutput' )

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr # For release 14.1.0.2 and newer


# Method for adding items from the input file to the output file,
# except the ones specified in the exclude list
def addAllItemsFromInputExceptExcludeList(streamName,ExcludeList):
    try:
        stream = MSMgr.GetStream(streamName)
    except Exception:
        raise RuntimeError("Unable to get stream %s"%streamName)
    
    # Get the list of all input event-type containers
    try :
        # This is the new way, in 15.4.0
        from RecExConfig.InputFilePeeker import inputFileSummary
        pass
    except ImportError :
        # This is here for backwards compatibility
        from RecExCommon.InputFilePeeker import inputFileSummary
        pass
    fullList = []
    if inputFileSummary['file_type'] == 'pool' :
        fullListTuple = inputFileSummary['eventdata_items']
        for iTuple in fullListTuple :
            item = iTuple[0]+"#"+iTuple[1]
            fullList += [item]
            pass
        stream.AddItem(fullList)
        pass
    else:
        primaryDPD_BasicOutput_msg.info( 'Input file is NOT a pool file! Cannot build the list of eventdata_items from a non-pool input file for the output stream %s!', streamName )
        pass

    # Now, process the ExcludeList and only try to exclude what is actually there
    _exclList = []
    for excludeItem in ExcludeList :
        if fullList.__contains__(excludeItem):
            _exclList.append(excludeItem)
            pass
        # Deal with a wildcard
        if excludeItem.endswith("*"):
            for fullItem in fullList:
                if fullItem.startswith( excludeItem.rstrip("*") ):
                    _exclList.append(fullItem)
                    pass
                pass
            pass
        pass

    # Actually remove the scheduled items from the output stream
    stream.RemoveItem(_exclList)
    return





##====================================================================
## The basic output is needed for all output streams, if you build your output up by hand
##====================================================================
def addBasicOutput(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)
        #stream.Stream.TakeItemsFromInput = True

        ##-----------------------------------------------------------------------------
        ## Write all IOV meta data containers
        ## IOV stands for "Interval Of Validity". This means that in one file, there
        ## can be several blocks of events corresponding to one MetaData block.
        ##-----------------------------------------------------------------------------
        stream.AddMetaDataItem( ["IOVMetaDataContainer#*"] )
        stream.AddMetaDataItem( ["LumiBlockCollection#*"] )

        ##-----------------------------------------------------------------------------
        ## Write event-based containers 
        ##-----------------------------------------------------------------------------
        stream.AddItem( ['EventInfo#*'] )

        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the basic output collections to the stream %s', streamName )
        pass

    return




##====================================================================
## Basic physics objects
##====================================================================
def addBasicPhysics(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)

        ##-----------------------------------------------------------------------------
        ## Write physics object related containers
        ##-----------------------------------------------------------------------------
        stream.AddItem( ["egammaContainer#ElectronAODCollection"] )
        stream.AddItem( ["egammaContainer#PhotonAODCollection"] )
        stream.AddItem( ["egDetailContainer#egDetailAOD"] )
        stream.AddItem( ["Analysis::MuonContainer#StacoMuonCollection"] )
        stream.AddItem( ["Analysis::MuonContainer#MuidMuonCollection"] )
        stream.AddItem( ["Analysis::TauJetContainer#TauRecContainer"] )
        stream.AddItem( ["JetCollection#Cone4TowerJets"] )
        #stream.AddItem( ["JetCollection#AntiKt4TowerJets"] )
        stream.AddItem( ["JetCollection#Cone4TopoJets"] )
        stream.AddItem( ["JetCollection#AntiKt4TopoJets"] ) 
        stream.AddItem( ["JetCollection#Cone7TowerJets"] )
        #stream.AddItem( ["JetCollection#AntiKt6TowerJets"] )
        stream.AddItem( ["JetKeyDescriptor#JetKeyMap"] )            # Needed to read rel 14.1.x jets in 14.2.0
        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the basic output collections to the stream %s', streamName )
        pass

    return



##====================================================================
## EGamma related output
##====================================================================
def addEGamma(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)
        ##-----------------------------------------------------------------------------
        ## Write EGamma related containers
        ##-----------------------------------------------------------------------------
        stream.AddItem( ["egDetailContainer#egDetailAOD"] )
        stream.AddItem( ["egDetailContainer#egDetailContainer"] )
        stream.AddItem( ["egDetailContainer#SofteDetailContainer"] )

        if rec.readESD() :
            stream.AddItem( ["ElectronContainer#softeCollection"] )
            stream.AddItem( ["ElectronContainer#ElectronCollection"] )
            stream.AddItem( ["PhotonContainer#PhotonCollection"] )
        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the basic output collections to the stream %s', streamName )
        pass

    return


##====================================================================
## Muon related output
##====================================================================
def addMuons(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)
        ##-----------------------------------------------------------------------------
        ## Write Muon related containers
        ##-----------------------------------------------------------------------------
        stream.AddItem( ["Analysis::MuonContainer#*"] )
        stream.AddItem( ["TrackRecordCollection#MuonEntryRecordFilter"] )
        stream.AddItem( ["Rec::TrackParticleContainer#MuidCombTrackParticles"] )
        stream.AddItem( ["Rec::TrackParticleContainer#MuidCombTrackParticlesLowPt"] )
        stream.AddItem( ["Rec::TrackParticleContainer#MuidExtrTrackParticles"] )
        stream.AddItem( ["Rec::TrackParticleContainer#MuTagTrackParticles"] )
        stream.AddItem( ["Rec::TrackParticleContainer#MuonboyMuonSpectroOnlyTrackParticles"] )
        stream.AddItem( ["Rec::TrackParticleContainer#MuonboyTrackParticles"] )
        stream.AddItem( ["Rec::TrackParticleContainer#StacoTrackParticles"] )
        stream.AddItem( ["Rec::TrackParticleContainer#MooreTrackParticles"] )
        stream.AddItem( ["Rec::MuonSpShowerContainer#MuonSpShowers"] )

        stream.AddItem( ["CaloCompactCellContainer#AODCellContainer"] ) ## Requires detector description
        stream.AddItem( ["CaloCellContainer#AODCellContainer"] ) ## Requires detector description
        stream.AddItem( ["CaloClusterContainer#CaloCalTopoCluster"] )

        if rec.readESD() :
            stream.AddItem( ["TileMuContainer#TileMuObj"] )
            stream.AddItem( ["Trk::SegmentCollection#*"] )
            stream.AddItem( ["Muon::CscPrepDataContainer#CSC_Clusters"] )
            stream.AddItem( ["Muon::MdtPrepDataContainer#MDT_DriftCircles"] )
            stream.AddItem( ["Muon::RpcPrepDataContainer#RPC_Measurements"] )
            stream.AddItem( ["Muon::TgcPrepDataContainer#TGC_Measurements"] )

            if primDPD.UseMCTruth() :
                stream.AddItem( ["PRD_MultiTruthCollection#*"] )
        
        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the basic output collections to the stream %s', streamName )
        pass

    return


##====================================================================
## Tau related output
##====================================================================
def addTau(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)
        ##-----------------------------------------------------------------------------
        ## Write Tau related containers
        ##-----------------------------------------------------------------------------
        stream.AddItem( ["Analysis::TauDetailsContainer#TauRecDetailsContainer"] )
        stream.AddItem( ["Analysis::TauDetailsContainer#TauRecExtraDetailsContainer"] )
        
        stream.AddItem( ["CaloClusterContainer#TauRecCellCluster"] )
        stream.AddItem( ["CaloClusterContainer#Tau1P3PCellCluster"] )
        stream.AddItem( ["CaloClusterContainer#Tau1P3PCellEM012ClusterContainer"] )
        stream.AddItem( ["CaloClusterContainer#Tau1P3PPi0ClusterContainer"] )

        stream.AddItem( ["CaloCellLinkContainer#TauRecCellCluster_Link"] )
        stream.AddItem( ["CaloCellLinkContainer#Tau1P3PCellCluster_Link"] )
        stream.AddItem( ["CaloCellLinkContainer#Tau1P3PCellEM012ClusterContainer_Link"] )
        stream.AddItem( ["CaloCellLinkContainer#Tau1P3PPi0ClusterContainer_Link"] )

        stream.AddItem( ["CaloShowerContainer#TauRecCellCluster_Data"] )
        stream.AddItem( ["CaloShowerContainer#Tau1P3PCellCluster_Data"] )
        stream.AddItem( ["CaloShowerContainer#Tau1P3PCellEM012ClusterContainer_Data"] )
        stream.AddItem( ["CaloShowerContainer#Tau1P3PPi0ClusterContainer_Data"] )

    
        
        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the Tau collections to the stream %s', streamName )
        pass

    return



##====================================================================
## Track related output
##====================================================================
def addTracks(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)
        ##-----------------------------------------------------------------------------
        ## Write Track and Vertex information
        ##-----------------------------------------------------------------------------
        stream.AddItem( ["TrackCollection#*"] )
        stream.AddItem( ["TrackCollection#Tracks"] )
        stream.AddItem( ["TrackCollection#CombinedInDetTracks"] )
        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the basic output collections to the stream %s', streamName )
        pass

    return





##====================================================================
## TrackParticle related output
##====================================================================
def addTrackParticles(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)
        ##-----------------------------------------------------------------------------
        ## Write Track and Vertex information
        ##-----------------------------------------------------------------------------
        stream.AddItem( ["Rec::TrackParticleContainer#TrackParticleCandidate"] )
        stream.AddItem( ["VxContainer#SecVertices"] )
        stream.AddItem( ["VxContainer#ConversionCandidate"] )  ## For photon conversions, new
        stream.AddItem( ["VxContainer#Conversions"] )          ## For photon conversions, before rel 14.2.20
        stream.AddItem( ["VxContainer#VxPrimaryCandidate"] )
        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the basic output collections to the stream %s', streamName )
        pass

    return







##====================================================================
## Calorimeter related output
##====================================================================
def addCalorimeter(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)
        #-----------------------------------------------------------------------------
        # Write Calorimeter information
        #   for more info on the calorimeter collections, see:
        #   https://twiki.cern.ch/twiki/bin/view/Atlas/CaloEvolution1200Persist 
        #-----------------------------------------------------------------------------
        stream.AddItem( ["CaloClusterContainer#egClusterCollection"] )
        stream.AddItem( ["CaloCellLinkContainer#egClusterCollection*"] )
        stream.AddItem( ["CaloShowerContainer#egClusterCollection*"] )
        stream.AddItem( ["CaloClusterContainer#EMTopoCluster*"] )
        stream.AddItem( ["CaloCellLinkContainer#EMTopoCluster*"] )
        stream.AddItem( ["CaloShowerContainer#EMTopoCluster*"] )
        if rec.readESD() :
            stream.AddItem( ["CaloClusterContainer#LArClusterEM*"] )
            stream.AddItem( ["CaloShowerContainer#LArClusterEM*"] )
            stream.AddItem( ["CaloCellLinkContainer#LArClusterEM*"] )
            stream.AddItem( ["CaloShowerContainer#egClusterCollection*"] )
            pass
        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the basic output collections to the stream %s', streamName )
        pass

    return




##====================================================================
## Missing Et related output
##====================================================================
def addMissingEt(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)
        #-----------------------------------------------------------------------------
        # MET_RefFinal and various terms contributing to it:
        #-----------------------------------------------------------------------------
        stream.AddItem( ["MissingET#MET_RefFinal"] )
        stream.AddItem( ["MissingET#MET_RefEle"] )
        stream.AddItem( ["MissingET#MET_RefGamma"] )
        stream.AddItem( ["MissingET#MET_RefJet"] )
        stream.AddItem( ["MissingET#MET_RefMuon"] )
        stream.AddItem( ["MissingET#MET_RefTau"] )
        stream.AddItem( ["MissingET#MET_CellOut"] )
        stream.AddItem( ["MissingET#MET_LocHadTopo"] )
        stream.AddItem( ["MissingET#MET_LocHadTopoObj"] )
        stream.AddItem( ["MissingET#MET_MuonBoy"] )
        stream.AddItem( ["MissingET#MET_CryoCone"] )
        stream.AddItem( ["MissingET#MET_Final"] )
        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the basic output collections to the stream %s', streamName )
        pass

    return




##====================================================================
## Inner detector related output
##====================================================================
def addInnerDetectorPrepRawData(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)
        stream.AddItem( ["InDet::PixelClusterContainer#PixelClusters"] )
        stream.AddItem( ["InDet::SCT_ClusterContainer#SCT_Clusters"] )
        stream.AddItem( ["InDet::TRT_DriftCircleContainer#TRT_DriftCircles"] )
        stream.AddItem( ["InDet::PixelGangedClusterAmbiguities#PixelClusterAmbiguitiesMap"] )
        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the basic output collections to the stream %s', streamName )
        pass

    return




##====================================================================
## Trigger related output
##====================================================================
def addTrigger(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)
        stream.AddItem( ["TrigConf::Lvl1AODPrescaleConfigData#AODConfig-0"] ) 
        stream.AddItem( ["LVL1_ROI#LVL1_ROI"] )
        stream.AddItem( ["TrigInDetTrackCollection#HLTAutoKey*"] )
        stream.AddItem( ["TrigInDetTrackCollection#HLT"] )
        stream.AddItem( ["TrigInDetTrackCollection#HLT_TRTSegmentFinder"] )
        stream.AddItem( ["TrigInDetTrackCollection#HLT_TRTxK"] )
        stream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma*"] )
        stream.AddItem( ["TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma*"] )
        stream.AddItem( ["CombinedMuonFeature#HLT_egamma"] )
        stream.AddItem( ["TrigEMCluster#HLTAutoKey*"] )
        stream.AddItem( ["TrigEMClusterContainer#HLT*"] )
        stream.AddItem( ["TrigT2Jet#HLTAutoKey*"] )
        stream.AddItem( ["TrigElectronContainer#HLTAutoKey*"] )
        stream.AddItem( ["TrigPhotonContainer#HLTAutoKey*"] )
        stream.AddItem( ["TrigTau#HLTAutoKey*"] )
        stream.AddItem( ["JetCollection#HLTAutoKey*"] )
        stream.AddItem( ["TrigMuonEFContainer#HLTAutoKey*"] )
        stream.AddItem( ["egammaContainer#HLTAutoKey*"] )
        stream.AddItem( ["egammaContainer#NoIDEF_RoI*"] )  
        stream.AddItem( ["egammaContainer#egamma_RoI*"] )  
        stream.AddItem( ["egammaContainer#HLT_egamma"] )  
        stream.AddItem( ["Analysis::TauDetailsContainer#HLTAutoKey*"] )
        stream.AddItem( ["Analysis::TauJetContainer#HLTAutoKey*"] )
        stream.AddItem( ["TrigMissingET#HLTAutoKey*"] )
        stream.AddItem( ["HLT::HLTResult#HLTResult_EF"] )
        stream.AddItem( ["HLT::HLTResult#HLTResult_L2"] )
        stream.AddItem( ["egDetailContainer#HLTAutoKey*"] )
        stream.AddItem( ["egDetailContainer#HLT_egamma*"] )
        stream.AddItem( ["Rec::TrackParticleContainer#HLTAutoKey*" ] )         #Needed by one of the above trigger containers...
        stream.AddItem( [ "TrigDec::TrigDecision#TrigDecision*"] )
        stream.AddItem( [ "TrigConf::HLTAODConfigData#AODConfig*"] )
        stream.AddItem( [ "TrigConf::Lvl1AODConfigData#AODConfig*"] )
        stream.AddItem( [ "TrigConf::Lvl1AODPrescaleConfigData#AODConfig*"] )

        # new names and additional containers:
        
        stream.AddItem( ["LVL1::JEMRoI#JEMRoIs"] ) 
        stream.AddItem( ["TrigRoiDescriptorCollection#HLT"] )
        stream.AddItem( ["TrigRoiDescriptorCollection#HLT_T2TauFinal"] )
        stream.AddItem( ["TrigRoiDescriptorCollection#HLT_TrigT2CaloEgamma"] )
        stream.AddItem( ["TrigRoiDescriptorCollection#HLT_TrigT2CaloJet"] )
        stream.AddItem( ["TrigRoiDescriptorCollection#HLT_TrigT2CaloTau"] )
        stream.AddItem( ["TrigRoiDescriptorCollection#HLT_forMS"] )                     
        
        stream.AddItem( ["TrigTauClusterContainer#HLT_TrigT2CaloTau"] )
        stream.AddItem( ["TrigTauContainer#HLT"] ) 
        stream.AddItem( ["Analysis::TauJetContainer#HLT_TrigTauRecMerged"] )
        stream.AddItem( ["Analysis::TauJetContainer#HLT_TrigTauRecCalo"] )
        stream.AddItem( ["TrigTauTracksInfoCollection#HLT"] )
        stream.AddItem( ["Analysis::TauDetailsContainer#HLT_TrigTauDetailsCalo"] )
        stream.AddItem( ["Analysis::TauDetailsContainer#HLT_TrigTauDetailsMerged"] )
        
        stream.AddItem( ["VxContainer#HLT_PrimVx"] )
        stream.AddItem( ["LVL1::TriggerTower#TriggerTowers"] )
        stream.AddItem( ["LVL1::JEMEtSums#JEMEtSums"] )
        stream.AddItem( ["TrigEMClusterContainer#HLT_TrigT2CaloEgamma"] )
        stream.AddItem( ["TrigElectronContainer#HLT_L2IDCaloFex"] ) 
        stream.AddItem( ["TrigPhotonContainer#HLT_L2PhotonFex"] )
        stream.AddItem( ["JetCollection#HLT"] )    
        stream.AddItem( ["JetCollection#HLT_TrigJetRec"] )
        stream.AddItem( ["TrigT2JetContainer#HLT_TrigT2CaloJet"] )
        stream.AddItem( ["TrigMissingETContainer#HLT_T2MissingET"] ) 
        stream.AddItem( ["TrigMissingETContainer#HLT_TrigEFMissingET"] )   
        stream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_Tau"] )
        stream.AddItem( ["TrigInDetTrackCollection#HLT_TrigSiTrack_Tau"] )
        stream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_Muon"] )
        stream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_Jet"] )
        stream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma"] )
        stream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Tau_EFID"] )
        stream.AddItem( ["CombinedMuonFeature#HLT"] )
        stream.AddItem( ["MuonFeatureContainer#HLT"] ) 
        stream.AddItem( ["TrigMuonEFContainer#HLT_MuonEF"] )    

        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the basic output collections to the stream %s', streamName )
        pass

    return



##====================================================================
## MC Truth related output
##====================================================================
def addTruth(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)
        if primDPD.UseMCTruth():

            from ParticleBuilderOptions.AODFlags import AODFlags

            if rec.readAOD:
                AODFlags.McEventKey="GEN_AOD"
            else:
                AODFlags.McEventKey="GEN_EVENT"

            if primDPD.ApplyTruthSlimming() :
                #include("PrimaryDPDMaker/SlimTruth.py")
                stream.AddItem( ["McEventCollection#GEN_DPD"] )
                stream.AddItem( ["TruthEtIsolationContainer#*"] )
                stream.AddItem( ["TruthParticleContainer#SpclMC"] )
                pass
            else :
                stream.AddItem( ["McEventCollection#GEN_AOD"] )
                stream.AddItem( ["TruthParticleContainer#SpclMC"] )
                pass
            stream.AddItem( ["JetCollection#Cone4TruthJets"] )
            stream.AddItem( ["JetCollection#Cone7TruthJets"] )
            #stream.AddItem( ["JetCollection#AntiKt4TruthJets"] )
            stream.AddItem( ["MissingEtTruth#MET_Truth"] )
            stream.AddItem( ["MissingEtTruth#MET_Truth_PileUp"] )
            stream.AddItem( ["TrackParticleTruthCollection#TrackParticleTruthCollection"] )
              
            pass
        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the basic output collections to the stream %s', streamName )
        pass

    return

##====================================================================
## Lightweight performance
##====================================================================
def addPerfLite(streamName=""):
    if not streamName=="" :
        stream = MSMgr.GetStream(streamName)
        stream.AddItem( ["egDetailContainer#egDetailAOD"] )        
        stream.AddItem( ["Analysis::TauDetailsContainer#TauRecDetailsContainer"] )
        stream.AddItem( ["Rec::TrackParticleContainer#MuonboyMuonSpectroOnlyTrackParticles"] )
        stream.AddItem( ["Rec::TrackParticleContainer#StacoTrackParticles"] )
        stream.AddItem( ["Rec::MuonSpShowerContainer#MuonSpShowers"] )
        stream.AddItem( ["Analysis::MuonContainer#CaloMuonCollection"] )
        stream.AddItem( ["Rec::TrackParticleContainer#MuonboyTrackParticles"] )
        stream.AddItem( ["Rec::TrackParticleContainer#MooreTrackParticles"] )
        pass
    else :
        primaryDPD_BasicOutput_msg.warning( 'Can not add the basic output collections to the stream %s', streamName )
        pass

    return



##====================================================================
## Containers to be excluded for the thin primary physics DPDs
##====================================================================
def excludeContainersList():

    _excludeContList = [
        #'LVL1_ROI#LVL1_ROI', # 4.3. request by Sara Barroni
        'CTP_Decision#CTP_Decision',
        #'TrigDec::TrigDecision#TrigDecision', #AT 19-Feb-2009
        'TauDetailsContainer#HLT_TrigTauDetailsCalo',
        'TauJetContainer#HLT_TrigTauRecCalo',
        #'CombinedMuonFeatureContainer#HLT', # 4.3. request by Sara Barroni
        #'TrigRoiDescriptorCollection#HLT', # 23.2. request from Monika Wielers
        #'TrigRoiDescriptorCollection#HLT_forID', # 23.2. request from Monika Wielers
        #'TrigRoiDescriptorCollection#HLT_forMS', # 23.2. request from Monika Wielers
        #'TrigRoiDescriptorCollection#HLT_TrigT2CaloJet', # 23.2. request from Monika Wielers
        #'TrigRoiDescriptorCollection#HLT_T2TauFinal', # 23.2. request from Monika Wielers
        #'TrigRoiDescriptorCollection#HLT_TrigT2CaloTau', # 23.2. request from Monika Wielers
        #'TrigRoiDescriptorCollection#HLT_TrigT2CaloEgamma', # 4.5. request from Monika Wielers
        'TrigRoiDescriptorCollection#HLT_initialRoI',
        'TrigSpacePointCountsCollection#HLT_spacepoints',
        'TrigEFBjetContainer#HLT_EFBjetFex',
        #'TrigMuonEFContainer#HLT_MuonEF', # 4.3. request by Sara Barroni
        'TrigT2MbtsBitsContainer#HLT_T2Mbts',
        'MuonFeatureContainer#HLT',
        #'TrigEMClusterContainer#HLT', # 23.2. request from Monika Wielers
        #'TrigEMClusterContainer#HLT_TrigT2CaloCosmic', # 23.2. request from Monika Wielers
        'TrigMissingETContainer#HLT_T2MissingET',
        'TrigMissingETContainer#HLT_TrigEFMissingET',
        'TrigVertexCollection#HLT_T2HistoPrmVtx',
        'TrigInDetTrackCollection#HLT',
        'TrigInDetTrackCollection#HLT_TRTSegmentFinder',
        'TrigInDetTrackCollection#HLT_TRTxK',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Bphysics',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Cosmics',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_FullScan',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Muon',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Tile',
        #'TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma_Brem', # 23.2. request from Monika Wielers
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_muonIso',
        #'TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma', # 23.2. request from Monika Wielers
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Jet',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Tau',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Bphysics',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Cosmics',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_FullScan',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Muon',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Tau',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_muonIso',
        #'TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma', # 23.2. request from Monika Wielers
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Jet',
        'TrigL2BphysContainer#HLT_L2BMuMuFex',
        'TrigL2BphysContainer#HLT_L2BMuMuXFex',
        'TrigL2BphysContainer#HLT_L2DiMuXFex',
        'TrigL2BphysContainer#HLT_L2DsPhiPiFexDs',
        'TrigL2BphysContainer#HLT_L2DsPhiPiFexPhi',
        'TrigL2BphysContainer#HLT_L2JpsieeFex',
        'TrigL2BphysContainer#HLT_TrigDiMuon',
        'TrigL2BphysContainer#HLT_EFBMuMuFex',
        'TrigL2BphysContainer#HLT_EFDsPhiPiFex',
        'TrigL2BphysContainer#HLT_EFMuPairs',
        'TrigEFBphysContainer#HLT_EFBMuMuFex',
        'TrigEFBphysContainer#HLT_EFDsPhiPiFex',
        'TrigEFBphysContainer#HLT_EFMuPairs',
        'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Bphysics_EFID',
        'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_FullScan_EFID',
        'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Muon_EFID',
        'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Photon_EFID',
        'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Bjet_EFID',
        'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Tau_EFID',
        'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Electron_EFID',
        'VxContainer#HLT_PrimVx',
        #'TrigTauContainer#HLT', # 23.2. request from Monika Wielers
        #'TrigTauTracksInfoCollection#HLT', # 23.2. request from Monika Wielers
        'TrigT2JetContainer#HLT_TrigT2CaloJet',
        'TrigL2BjetContainer#HLT_L2BjetFex',
        #'TrigElectronContainer#HLT_L2IDCaloFex', # 23.2. request from Monika Wielers
        #'TrigPhotonContainer#HLT_L2PhotonFex', # 23.2. request from Monika Wielers
        #'egammaContainer#HLT_egamma', # 4.5. request from Monika Wielers
        #'egDetailContainer#HLT_egamma', # 23.2. request from Monika Wielers
        #'TrigEMClusterContainer#HLT_TrigT2CaloEgamma', # 23.2. request from Monika Wielers
        'Analysis::TauJetContainer#HLT_TrigTauRecMerged',
        'Analysis::TauJetContainer#HLT_TrigTauRecCalo',
        'Analysis::TauDetailsContainer#HLT_TrigTauDetailsMerged',
        'Analysis::TauDetailsContainer#HLT_TrigTauExtraDetailsMerged',
        'Analysis::TauDetailsContainer#HLT_TrigTauDetailsCalo',
        #'TrigTauClusterContainer#HLT_TrigT2CaloTau', # 23.2. request from Monika Wielers
        'TrigTrackCountsCollection#HLT',
        'JetCollection#HLT',
        'CaloClusterContainer#HLT',
        #'CaloClusterContainer#HLT_TrigCaloClusterMaker', # 23.2. request from Monika Wielers
        'CaloShowerContainer#HLT',
        'CaloTowerContainer#HLT_TrigCaloTowerMaker',
        'TileTrackMuFeatureContainer#HLT',
        'TileMuFeatureContainer#HLT',
        'JetCollection#HLT_TrigJetRec',
        'MissingET#MET_DM_All',
        'MissingET#MET_DM_Crack1',
        'MissingET#MET_DM_Crack2',
        'MissingET#MET_DM_Cryo',
        'MissingET#MET_RefMuon_Track',
        'MissingET#MET_RefMuon',
        'MissingET#MET_MuonBoy',
        'MissingET#MET_RefTau',
        'MissingET#MET_RefGamma',
        'MissingET#MET_RefEle',
        'MissingET#ObjMET_TauJet',
        'MissingET#ObjMET_IdTrk',
        'MissingET#ObjMET_Muon',
        'MissingET#MET_MuonBoy_Track',
        'MissingET#MET_Cryo',
        'MissingET#MET_CryoCone',
        'MissingET#MET_CellOut',
        #'MissingET#MET_Final', #AT 19-Feb-2009
        'MissingET#MET_LocHadTopoObj',
        'MissingET#MET_TopoObj',
        'MissingET#MET_RefJet',
        'MissingET#MET_MuonBoy_Spectro',
        'MissingET#MET_Muon',
        'MissingET#ObjMET_Elec',
        'MissingETSigObjContainer#EtMissObjCollection',
        'MissingET#ObjMET_Jet',
        'MissingET#ObjMET_MiniJet',
        'MissingET#ObjMET_Rest',
        #'MissingET#ObjMET_Final', #AT 19-Feb-2009
        'MissingETSig#METSig',
        'MissingEtCalo#MET_Base',
        'MissingEtCalo#MET_Calib',
        'MissingEtCalo#MET_Topo',
        'MissingEtCalo#MET_CorrTopo',
        'MissingEtCalo#MET_LocHadTopo',
        'MissingETSigHypoContainer#EtMissHypoCollection',
        'Rec::MuonSpShowerContainer#MuonSpShowers',
        #'MuonContainer#CaloMuonCollection', #AT 19-Feb-2009
        'TileContainer<TileMu>#TileMuObj',
        'TileCellVec#MBTSContainer',
        'TileMuContainer#TileMuObj',
        'TileCellContainer#MBTSContainer',
        #'CaloClusterContainer#egClusterCollection', # 4.3. request by Iro Koletsou
        'CaloClusterContainer#LArClusterEMSofte', #AT 19-Feb-2009
        #'CaloClusterContainer#CaloCalTopoCluster',
        #'CaloClusterContainer#EMTopoCluster430',
        #'CaloClusterContainer#Tau1P3PCellCluster',
        'CaloClusterContainer#Tau1P3PCellEM012ClusterContainer', #AT 19-Feb-2009
        #'CaloClusterContainer#MuonClusterCollection',
        #'CaloClusterContainer#Tau1P3PPi0ClusterContainer',
        'CaloCellLinkContainer#MuonClusterCollection_Link',
        'CaloCellLinkContainer#egClusterCollection_Link',
        'CaloCellContainer#AODCellContainer',
        'Rec::TrackParticleContainer#MuTagTrackParticles',
        'Rec::TrackParticleContainer#MuGirlRefittedTrackParticles',
        'Rec::TrackParticleContainer#StacoTrackParticles',
        'Rec::TrackParticleContainer#MuonboyTrackParticles',
        'Rec::TrackParticleContainer#MuonboyMuonSpectroOnlyTrackParticles',
        'Rec::TrackParticleContainer#MuidCombTrackParticles',
        'Rec::TrackParticleContainer#MuidExtrTrackParticles',
        'Rec::TrackParticleContainer#MooreTrackParticles',
        'TrackRecordCollection#MuonEntryLayerFilter',
        'Trk::SegmentCollection#MuGirlSegments',
        'Trk::SegmentCollection#ConvertedMBoySegments',
        'Trk::SegmentCollection#MooreSegments',
        'Trk::TrackCollection#Tracks',
        'TrackCollection#Tracks',
        'TrackCollection#MuonSlimmedTrackCollection',
        'TPCnv::MuonMeasurements',
        #'JetCollection#Cone4TopoJets',
        #'JetCollection#Cone7TowerJets',
        'Analysis::JetTagInfo',
        'InDet::Track'
        ]

    return _excludeContList


def excludeAtlfastContainersList():

    _excludeAtlfastContList = [
        'JetCollection#AtlfastJetContainer',
        'INav4MomAssocs#AtlfastMcAodAssocs',
        'PhotonContainer#AtlfastPhotonCollection',
        'egDetailContainer#AtlfastEleShowerContainer',
        'egDetailContainer#AtlfastIsoEleShowerContainer',
        'ElectronContainer#AtlfastElectronCollection',
        'ElectronContainer#AtlfastIsoElectronCollection',
        'Analysis::MuonContainer#AtlfastIsoMuonCollection',
        'Analysis::MuonContainer#AtlfastMuonCollection',
        'Rec::TrackParticleContainer#AtlfastTrackParticles',
        'MissingET#AtlfastMissingEt',
        'Analysis::TauJetContainer#AtlfastTauJet1p3pContainer',
        'Analysis::TauJetContainer#AtlfastTauJetContainer'
        ]

    return _excludeAtlfastContList



def excludeMCTruthContainersList():

    _excludeMCTruthContList = [
        'TruthParticleContainer#SpclMC',
        'MissingEtTruth#MET_Truth',
        'MissingEtTruth#MET_Truth_PileUp',
        'McEventCollection#GEN_AOD',
        'TrackParticleTruthCollection#TrackParticleTruthCollection'
        ]

    return _excludeMCTruthContList



def excludeESDOnlyContainersList():

    _excludeESDOnlyContList = [
        'CMMRoI#CMMRoIs',
        #'CaloTowerContainer#CombinedTower', # NEEDED! CRASHES IF EXCLUDED!!!
        'TruthParticleContainer#INav4MomTruthEvent',
        'TrigRoiDescriptorCollection#HLT_secondaryRoI_EF',
        'TrigRoiDescriptorCollection#HLT_secondaryRoI_L2', 
        'CPMRoICollection#CPMRoIs',
        'JEMRoICollection#JEMRoIs',
        'MuonContainer#StacoESDMuonCollection',
        'JEMHitsCollection#JEMHits',
        'MuonContainer#MuidESDMuonCollection',
        'CPMHitsCollection#CPMHits',
        'TrigL2BphysContainer#HLT',
        'Muon::TgcPrepDataContainer#TGC_Measurements',
        'Muon::RpcPrepDataContainer#RPC_Measurements',
        'BCM_RDO_Container#BCM_RDOs',
        'CMMJetHitsCollection#CMMJetHits',
        'Muon::CscPrepDataContainer#CSC_Clusters',        
        'MuonContainer#CaloESDMuonCollection',
        'CMMCPHitsCollection#CMMCPHits',
        'CaloCellLinkContainer#Tau1P3PPi0ClusterContainer_Link',
        'CaloShowerContainer#Tau1P3PPi0ClusterContainer_Data',
        'Trk::TrackCollection#HLT_InDetTrigTrackSlimmer_Bphysics_EFID',
        'Trk::TrackCollection#HLT_InDetTrigTrackSlimmer_FullScan_EFID',
        'Trk::TrackCollection#HLT_InDetTrigTrackSlimmer_Muon_EFID',
        'Trk::TrackCollection#ConvertedStacoTracks',
        'Trk::TrackCollection#ConvertedMuTagTracks',
        'Trk::TrackCollection#ConvertedMBoyMuonSpectroOnlyTracks',
        'Trk::TrackCollection#ConvertedMBoyTracks',
        'CaloShowerContainer#LArClusterEMSofte_Data',
        'Trk::TrackCollection#MuGirlRefittedTracks',
        'Trk::TrackCollection#ConvertedMuIdCBTracks',
        'Trk::TrackCollection#MuidExtrapolatedTracks',
        'CaloShowerContainer#Tau1P3PCellEM012ClusterContainer_Data',
        'Trk::TrackCollection#MooreTracks',
        'CaloClusterContainer#LArClusterEM7_11Nocorr',
        'Muon::MdtPrepDataContainer#MDT_DriftCircles',
        'CaloShowerContainer#MuonClusterCollection_Data',
        'Trk::TrackTruthCollection#TrackTruthCollection',
        'JEMEtSumsCollection#JEMEtSums',        
        'CaloShowerContainer#Tau1P3PCellCluster_Data',
        'CaloCellLinkContainer#Tau1P3PCellEM012ClusterContainer_Link',
        'CaloCellLinkContainer#Tau1P3PCellCluster_Link',
        'Trk::TrackCollection#HLT_InDetTrigTrackSlimmer_Bjet_EFID',
        'CMMEtSumsCollection#CMMEtSums',
        'TauDetailsContainer#TauRecExtraDetailsContainer',
        'Trk::DetailedTrackTruthCollection#DetailedTrackTruth',
        'CPMTowerCollection#CPMTowers',
        'Trk::PRD_MultiTruthCollection#PRD_MultiTruthPixel',
        'Trk::TrackCollection#HLT_InDetTrigTrackSlimmer_Tau_EFID',
        'Trk::PRD_MultiTruthCollection#PRD_MultiTruthSCT',
        'Trk::PRD_MultiTruthCollection#PRD_MultiTruthTRT',
        'CaloCellLinkContainer#CaloCalTopoCluster_Link',
        'CaloShowerContainer#CaloCalTopoCluster_Data'
        
        ## Keep the following containers for now
        #'EventStreamInfo#StreamESD', # EventInfo needed
        #'PhotonContainer#PhotonCollection',
        #'CaloCellLinkContainer#LArClusterEMSofte_Link', 
        #'ElectronContainer#softeCollection',
        #'CaloCellLinkContainer#LArClusterEM_Link',
        #'ElectronContainer#ElectronCollection',
        #'CaloCellLinkContainer#LArClusterEM7_11Nocorr_Link',
        #'CaloShowerContainer#egClusterCollection_Data',
        #'CaloShowerContainer#LArClusterEM_Data',
        #'CaloShowerContainer#LArClusterEM7_11Nocorr_Data',
        #'CaloClusterContainer#LArClusterEM',
        #'egDetailContainer#SofteDetailContainer',
        #'egDetailContainer#egDetailContainer',
        #'InDet::PixelGangedClusterAmbiguities#PixelClusterAmbiguitiesMap',
        #'Trk::TrackCollection#HLT_InDetTrigTrackSlimmer_Photon_EFID',
        #'JetElementCollection#JetElements', #test
        #'Trk::TrackCollection#HLT_InDetTrigTrackSlimmer_Electron_EFID',
        #'CaloCellLinkContainer#EMTopoCluster430_Link',
        #'TriggerTowerCollection#TriggerTowers',
        #'CaloShowerContainer#EMTopoCluster430_Data',
        #'InDet::PixelClusterContainer#PixelClusters',
        #'InDet::SCT_ClusterContainer#SCT_Clusters',
        #'InDet::TRT_DriftCircleContainer#TRT_DriftCircles',
        #'McEventCollection#TruthEvent',
        #'CaloCompactCellContainer#AllCalo',
        
        ]

    return _excludeESDOnlyContList
