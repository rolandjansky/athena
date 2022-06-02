# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from AthenaCommon.CfgGetter import getPublicTool, getPrivateTool, getAlgorithm
from MuonRecExample.ConfiguredMuonRec import ConfiguredMuonRec
from MuonRecExample.MuonRecFlags import muonRecFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
muonRecFlags.setDefaults()

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon import CfgMgr
from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from InDetRecExample.InDetKeys import InDetKeys
from InDetRecExample.InDetJobProperties import InDetFlags
from AthenaConfiguration.AllConfigFlags import ConfigFlags

def MuonCaloTagAlg(name="MuonCaloTagAlg",**kwargs):
    tools = [getPublicTool("MuonCaloTagTool")]
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    kwargs.setdefault("TagMap","caloTagMap")
    kwargs.setdefault("CombinedTrackCollection","")
    kwargs.setdefault("METrackCollection","")
    kwargs.setdefault("usePRDs", False )
    return CfgMgr.MuonCombinedInDetExtensionAlg(name,**kwargs)

def MuonCaloTagAlg_LRT(name="MuonCaloTagAlg_LRT", **kwargs):
    tools = [getPublicTool("MuonCaloTagTool")]
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    kwargs.setdefault("TagMap","caloTagMap_LRT")
    kwargs.setdefault("InDetCandidateLocation", MuonCbKeys.InDetTrackParticlesLargeD0())  
    kwargs.setdefault("CombinedTrackCollection","")
    kwargs.setdefault("METrackCollection","")
    kwargs.setdefault("usePRDs", False )
    return CfgMgr.MuonCombinedInDetExtensionAlg(name,**kwargs)

def MuonSegmentTagAlg( name="MuonSegmentTagAlg", **kwargs ):
    kwargs.setdefault("MuonSegmentTagTool", getPublicTool("MuonSegmentTagTool") )
    kwargs.setdefault("InDetCandidateLocation", 
                      "InDetCandidates" if  not muonCombinedRecFlags.doCombinedFit() else "InDetCandidatesSystemExtened")
    return CfgMgr.MuonSegmentTagAlg(name,**kwargs)

def MuonSegmentTagAlg_LRT( name="MuonSegmentTagAlg_LRT", **kwargs ):
    kwargs.setdefault("MuonSegmentTagTool", getPublicTool("MuonSegmentTagTool") )
    kwargs.setdefault("TagMap","segmentTagMap_LRT")
    kwargs.setdefault("InDetCandidateLocation", MuonCbKeys.InDetTrackParticlesLargeD0() + 
                     ("" if not muonCombinedRecFlags.doCombinedFit() else "SystemExtended"))
    return CfgMgr.MuonSegmentTagAlg(name,**kwargs)

def MuonInsideOutRecoAlg( name="MuonInsideOutRecoAlg", **kwargs ):
    reco_cscs = MuonGeometryFlags.hasCSC() and muonRecFlags.doCSCs()
    reco_stgcs = muonRecFlags.dosTGCs() and MuonGeometryFlags.hasSTGC()
    reco_mm =  muonRecFlags.doMMs() and MuonGeometryFlags.hasMM()  
    tools = [getPublicTool("MuonInsideOutRecoTool") ]
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    kwargs.setdefault("usePRDs",True)
    kwargs.setdefault("HasCSC", reco_cscs )
    kwargs.setdefault("HasSTgc", reco_stgcs )
    kwargs.setdefault("HasMM", reco_mm )
    kwargs.setdefault("TagMap","muGirlTagMap")
    kwargs.setdefault("SegmentCollection","MuGirlSegments")
    kwargs.setdefault("InDetCandidateLocation", 
                      "InDetCandidates" if  not muonCombinedRecFlags.doCombinedFit() else "InDetCandidatesSystemExtened")
    kwargs.setdefault("TGCPrepDataLocation", 
                      'TGC_MeasurementsAllBCs' if not muonRecFlags.useTGCPriorNextBC else 'TGC_Measurements'  )
    return CfgMgr.MuonCombinedInDetExtensionAlg(name,**kwargs)

def MuGirlAlg_LRT( name="MuGirlAlg_LRT", **kwargs ):
    reco_cscs = MuonGeometryFlags.hasCSC() and muonRecFlags.doCSCs()
    reco_stgcs = muonRecFlags.dosTGCs() and MuonGeometryFlags.hasSTGC()
    reco_mm =  muonRecFlags.doMMs() and MuonGeometryFlags.hasMM()  
    tools = [getPublicTool("MuonInsideOutRecoTool") ]
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    kwargs.setdefault("usePRDs",True)
    kwargs.setdefault("HasCSC", reco_cscs )
    kwargs.setdefault("HasSTgc", reco_stgcs )
    kwargs.setdefault("HasMM", reco_mm )
    kwargs.setdefault("TagMap","MuGirlMap_LRT")
    kwargs.setdefault("METrackCollection","MuGirlMETracks_LRT")
    kwargs.setdefault("SegmentCollection","MuGirlSegments_LRT")
    kwargs.setdefault("CombinedTrackCollection", MuonCbKeys.MuGirlMuonsLargeD0())
    kwargs.setdefault("InDetCandidateLocation", MuonCbKeys.InDetTrackParticlesLargeD0() + 
                     ("" if not muonCombinedRecFlags.doCombinedFit() else "SystemExtended"))
    kwargs.setdefault("TGCPrepDataLocation", 
                      'TGC_MeasurementsAllBCs' if not muonRecFlags.useTGCPriorNextBC else 'TGC_Measurements'  )
   
    return CfgMgr.MuonCombinedInDetExtensionAlg(name,**kwargs)

def MuGirlStauAlg(name="MuGirlStauAlg",**kwargs):
    reco_cscs = MuonGeometryFlags.hasCSC() and muonRecFlags.doCSCs()
    reco_stgcs = muonRecFlags.dosTGCs() and MuonGeometryFlags.hasSTGC()
    reco_mm =  muonRecFlags.doMMs() and MuonGeometryFlags.hasMM()  
    tools = [getPublicTool("MuonStauRecoTool")]
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    kwargs.setdefault("TagMap","stauTagMap")
    kwargs.setdefault("HasCSC", reco_cscs )
    kwargs.setdefault("HasSTgc", reco_stgcs )
    kwargs.setdefault("HasMM", reco_mm )
    kwargs.setdefault("CombinedTrackCollection","MuGirlStauCombinedTracks")
    kwargs.setdefault("METrackCollection","")
    kwargs.setdefault("SegmentCollection","TrkStauSegments")
    kwargs.setdefault("InDetCandidateLocation", 
                      "InDetCandidates" if not muonCombinedRecFlags.doCombinedFit() else "InDetCandidatesStaus")
    return CfgMgr.MuonCombinedInDetExtensionAlg(name,**kwargs)

def MuonCombinedInDetCandidateAlg( name="MuonCombinedInDetCandidateAlg",**kwargs ):    
    kwargs.setdefault("TrackSelector",getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool") )
    if not ConfigFlags.Muon.MuonTrigger:
        kwargs.setdefault("CaloExtensionLocation", ["ParticleCaloExtension"])    
    
    if muonCombinedRecFlags.doSiAssocForwardMuons() and InDetFlags.doForwardTracks():
        kwargs.setdefault("DoSiliconAssocForwardMuons", True )
        kwargs.setdefault("InDetForwardTrackSelector", getPublicTool("MuonCombinedInDetDetailedForwardTrackSelectorTool") )
    
    kwargs.setdefault("MuonSystemExtensionTool", getPublicTool("MuonSystemExtensionTool"))
    kwargs.setdefault("ExtendBulk", not muonCombinedRecFlags.doCombinedFit())
    return CfgMgr.MuonCombinedInDetCandidateAlg(name,**kwargs)

def MuonInDetToMuonSystemExtensionAlg(name= "MuonInDetToMuonSystemExtensionAlg", **kwargs):
    kwargs.setdefault("MuonSystemExtensionTool", getPublicTool("MuonSystemExtensionTool"))
    kwargs.setdefault("WriteStauCandidates", "InDetCandidatesStaus")
    return CfgMgr.MuonInDetToMuonSystemExtensionAlg(name,**kwargs) 

def MuonInDetToMuonSystemExtensionAlg_LRT(name= "MuonInDetToMuonSystemExtensionAlg_LRT", **kwargs):
    kwargs.setdefault("MuonSystemExtensionTool", getPublicTool("MuonSystemExtensionTool"))
    kwargs.setdefault("InputInDetCandidates", MuonCbKeys.InDetTrackParticlesLargeD0())
    kwargs.setdefault("WriteInDetCandidates", MuonCbKeys.InDetTrackParticlesLargeD0()+"SystemExtended")
    kwargs.setdefault("WriteStauCandidates","")
    
    kwargs.setdefault("CombinedTagMap", "muidcoTagMap_LRT")
    return CfgMgr.MuonInDetToMuonSystemExtensionAlg(name,**kwargs) 


def MuonCombinedInDetCandidateAlg_LRT( name="MuonCombinedInDetCandidateAlg_LRT",**kwargs ):
    kwargs.setdefault("TrackSelector",getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool_LRT") )
    kwargs.setdefault("MuonSystemExtensionTool", getPublicTool("MuonSystemExtensionTool"))
    ### Use the Standard Track particle container in cases where no separate containters will be
    ### saved for the LRT tracking
    track_container = InDetKeys.xAODLargeD0TrackParticleContainer()
    if not InDetFlags.storeSeparateLargeD0Container(): track_container = InDetKeys.xAODTrackParticleContainer()
    kwargs.setdefault("TrackParticleLocation", [track_container])
    if not ConfigFlags.Muon.MuonTrigger:    
        kwargs.setdefault("CaloExtensionLocation", ["ParticleCaloExtension_LRT"] 
                if InDetFlags.storeSeparateLargeD0Container() else ["ParticleCaloExtension"])
    kwargs.setdefault("InDetCandidateLocation",MuonCbKeys.InDetTrackParticlesLargeD0())
    kwargs.setdefault("DoSiliconAssocForwardMuons", False)
    kwargs.setdefault("ExtendBulk", not muonCombinedRecFlags.doCombinedFit() or ConfigFlags.Muon.MuonTrigger)  
    return CfgMgr.MuonCombinedInDetCandidateAlg(name,**kwargs)



def MuonCombinedMuonCandidateAlg( name="MuonCombinedMuonCandidateAlg", **kwargs ):
    kwargs.setdefault("MuonCandidateTool",getPublicTool("MuonCandidateTool"))
    return CfgMgr.MuonCombinedMuonCandidateAlg(name,**kwargs)

def MuonCombinedMuonCandidateAlg_EMEO(name = "MuonCombinedMuonCandidateAlg_EMEO"):
    return MuonCombinedMuonCandidateAlg(name = name, 
                                        MuonCandidateTool = getPublicTool("MuonCandidateTool_EMEO"),
                                        MuonSpectrometerTrackParticleLocation = "EMEO_MuonSpectrometerTrackParticles",
                                        MuonCandidateLocation = "MuonCandidates_EMEO",
                                        MSOnlyExtrapolatedTrackLocation= "EMEO_MSOnlyExtrapolatedTracks"
                                        )

def MuonCombinedAlg( name="MuonCombinedAlg",**kwargs ):
    kwargs.setdefault("MuonCombinedTool",getPublicTool("MuonCombinedTool"))
    tagmaps = []
    # CombinedTagMaps must be in a 1-1 correspondence
    # with MuonCombinedTagTools.
    for h in kwargs['MuonCombinedTool'].MuonCombinedTagTools:
        if h.getFullName().find('FitTagTool') >= 0:
            tagmaps.append ('muidcoTagMap')
        elif h.getFullName().find('StacoTagTool') >= 0:
            tagmaps.append ('stacoTagMap')
    kwargs.setdefault("CombinedTagMaps", tagmaps)
    return CfgMgr.MuonCombinedAlg(name,**kwargs)

def MuonCombinedAlg_LRT( name="MuonCombinedAlg_LRT",**kwargs ):
    kwargs.setdefault("MuonCombinedTool",getPublicTool("MuonCombinedTool"))
    kwargs.setdefault("InDetCandidateLocation",MuonCbKeys.InDetTrackParticlesLargeD0())
    kwargs.setdefault("CombinedTagMaps", ["muidcoTagMap_LRT","stacoTagMap_LRT"])
    kwargs.setdefault("MuidCombinedTracksLocation", "MuidCombinedTracks_LRT")
    kwargs.setdefault("MuidMETracksLocation", "MuidMETracks_LRT")    
    return CfgMgr.MuonCombinedAlg(name,**kwargs)

def MuonCombinedAlg_EMEO(name="MuonCombinedAlg_EMEO", **kwargs):
    kwargs.setdefault("MuonCombinedTool",getPublicTool("MuonCombinedTool_EMEO"))
    kwargs.setdefault("CombinedTagMaps", [
                      "muidcoTagMap_EMEO", "stacoTagMap_EMEO"])
    kwargs.setdefault("MuidCombinedTracksLocation", "MuidCombinedTracks_EMEO")
    kwargs.setdefault("MuidMETracksLocation", "MuidMETracks_EMEO")
    kwargs.setdefault("MuonCandidateLocation", "MuonCandidates_EMEO")
    return CfgMgr.MuonCombinedAlg(name,**kwargs)

def MuonSegContainerMergerAlg(name = "MuonSegContainerMergerAlg", **kwargs):
    kwargs.setdefault("saveUnusedSegments", muonCombinedRecFlags.saveUnassocSegments())    
    combined_maps =  []
    if muonCombinedRecFlags.doMuGirl(): combined_maps+=["muGirlTagMap"]
    if muonCombinedRecFlags.doStatisticalCombination(): combined_maps+=["stacoTagMap"]
    if muonCombinedRecFlags.doCombinedFit(): combined_maps+=["muidcoTagMap"]
    if muonCombinedRecFlags.doMuonSegmentTagger(): combined_maps+=["segmentTagMap"]
    muon_maps = ["MuonCandidates"]
    if muonRecFlags.runCommissioningChain():
        if muonCombinedRecFlags.doStatisticalCombination(): combined_maps+=["stacoTagMap_EMEO"]
        if muonCombinedRecFlags.doCombinedFit(): combined_maps+=["muidcoTagMap_EMEO"]
        muon_maps += ["MuonCandidates_EMEO"]
    if InDetFlags.doR3LargeD0():
        if muonCombinedRecFlags.doMuGirl(): combined_maps+=["MuGirlMap_LRT"]
        if muonCombinedRecFlags.doStatisticalCombination(): combined_maps+=["stacoTagMap_LRT"]
        if muonCombinedRecFlags.doCombinedFit(): combined_maps+=["muidcoTagMap_LRT"]
        if muonCombinedRecFlags.doMuonSegmentTagger(): combined_maps+=["segmentTagMap_LRT"]   
    kwargs.setdefault("MuonCandidateMaps", muon_maps)
    kwargs.setdefault("TagMaps", combined_maps)
    kwargs.setdefault("AmbiguityProcessor", getPublicTool("MuonAmbiProcessor"))
    return CfgMgr.MuonSegContainerMergerAlg(name, **kwargs)


def MuonCreatorAlg( name="MuonCreatorAlg",**kwargs ):
    kwargs.setdefault("MuonCreatorTool",getPublicTool("MuonCreatorTool"))
    # if muGirl is off, remove "muGirlTagMap" from "TagMaps"
    # but don't set this default in case the StauCreatorAlg is created (see below)
    
    muon_maps = ["MuonCandidates"]
    combined_maps = []
    if muonCombinedRecFlags.doMuGirl(): combined_maps+=["muGirlTagMap"]
    if muonCombinedRecFlags.doStatisticalCombination(): combined_maps+=["stacoTagMap"]
    if muonCombinedRecFlags.doCombinedFit(): combined_maps+=["muidcoTagMap"]
    if muonCombinedRecFlags.doCaloTrkMuId(): combined_maps+=["caloTagMap"]
    if muonCombinedRecFlags.doMuonSegmentTagger(): combined_maps+=["segmentTagMap"]
    
    
    if ConfigFlags.Muon.MuonTrigger:
        kwargs.setdefault("MakeClusters", False)
        kwargs.setdefault("ClusterContainerName", "")
        kwargs.setdefault("SegmentContainerName", "")
        kwargs.setdefault("TagToSegmentKey", "")
        
                
    
    kwargs.setdefault("TagMaps", combined_maps)
    kwargs.setdefault("MuonCandidateLocation", muon_maps)
    return CfgMgr.MuonCreatorAlg(name,**kwargs)

def MuonCreatorAlg_EMEO(name = "MuonCreatorAlg_EMEO", **kwargs):
    kwargs.setdefault("MuonCreatorTool",getPublicTool("MuonCreatorTool"))
    muon_maps = ["MuonCandidates_EMEO"]
    combined_maps = []
    if muonCombinedRecFlags.doStatisticalCombination(): combined_maps+=["stacoTagMap_EMEO"]
    if muonCombinedRecFlags.doCombinedFit(): combined_maps+=["muidcoTagMap_EMEO"]
    kwargs.setdefault("TagMaps", combined_maps)
    kwargs.setdefault("MuonCandidateLocation", muon_maps)
    kwargs.setdefault("MuonContainerLocation", "EMEO_Muons")
    kwargs.setdefault("ExtrapolatedLocation", "EMEO_ExtrapolatedMuon")
    kwargs.setdefault("MSOnlyExtrapolatedLocation", "EMEO_MSOnlyExtrapolatedMuon")   
    kwargs.setdefault("CombinedLocation", "EMEO_CombinedMuon")
    kwargs.setdefault("BuildSlowMuon", False)
    kwargs.setdefault("MakeClusters", False)
    kwargs.setdefault("ClusterContainerName", "")
    return CfgMgr.MuonCreatorAlg(name,**kwargs)
    

def MuonCreatorAlg_LRT( name="MuonCreatorAlg_LRT",**kwargs ):
    kwargs.setdefault("MuonCreatorTool",getPublicTool("MuonCreatorTool_LRT"))    
    combined_maps = [] 
    if muonCombinedRecFlags.doMuGirl(): combined_maps+=["MuGirlMap_LRT"]
    if muonCombinedRecFlags.doStatisticalCombination(): combined_maps+=["stacoTagMap_LRT"]
    if muonCombinedRecFlags.doCombinedFit(): combined_maps+=["muidcoTagMap_LRT"]
    if muonCombinedRecFlags.doCaloTrkMuId(): combined_maps+=["caloTagMap_LRT"]
    if muonCombinedRecFlags.doMuonSegmentTagger(): combined_maps+=["segmentTagMap_LRT"]   
    kwargs.setdefault("TagMaps", combined_maps)
    kwargs.setdefault("MuonContainerLocation", MuonCbKeys.FinalMuonsLargeD0())
    kwargs.setdefault("ExtrapolatedLocation", "ExtraPolated"+MuonCbKeys.FinalMuonsLargeD0())
    kwargs.setdefault("MSOnlyExtrapolatedLocation", "MSOnlyExtraPolated"+MuonCbKeys.FinalMuonsLargeD0())
    kwargs.setdefault("CombinedLocation", "Combined"+MuonCbKeys.FinalMuonsLargeD0())
    kwargs.setdefault("BuildSlowMuon", False)
    kwargs.setdefault("MakeClusters", False)
    kwargs.setdefault("ClusterContainerName", "")
    return MuonCreatorAlg(name,**kwargs)

def StauCreatorAlg( name="StauCreatorAlg", **kwargs ):
    kwargs.setdefault("MuonCreatorTool",getPrivateTool("StauCreatorTool"))
    kwargs.setdefault("MuonContainerLocation","Staus")
    kwargs.setdefault("CombinedLocation","CombinedStau")
    kwargs.setdefault("ExtrapolatedLocation","ExtrapolatedStau")
    kwargs.setdefault("MSOnlyExtrapolatedLocation","MSOnlyExtrapolatedStau")
    kwargs.setdefault("MuonCandidateLocation",[])
    kwargs.setdefault("SegmentContainerName","StauSegments")    
    kwargs.setdefault("BuildSlowMuon", True)
    kwargs.setdefault("ClusterContainerName", "SlowMuonClusterCollection")
    kwargs.setdefault("TagMaps",["stauTagMap"])
    kwargs.setdefault("TagToSegmentKey", "")
    return MuonCreatorAlg(name,**kwargs)

def GetCombinedTrackParticles():
    from RecExConfig.RecFlags  import rec
    colsTP = [ "ExtrapolatedMuonTrackParticles", "CombinedMuonTrackParticles", "MSOnlyExtrapolatedMuonTrackParticles" ]
    cols = [ "ExtrapolatedMuonTracks", "CombinedMuonTracks", "MSOnlyExtrapolatedTracks" ]

    if InDetFlags.doR3LargeD0():
        colsTP+= ["CombinedMuonsLRTTrackParticles", "ExtraPolatedMuonsLRTTrackParticles"]
        cols += ["CombinedMuonsLRTTracks", "ExtraPolatedMuonsLRTTracks"]
        ### These two will be remocved in 50080
        colsTP +=["MSOnlyExtraPolatedMuonsLRTTrackParticles"]
        cols  += ["MSOnlyExtraPolatedMuonsLRTTrackParticlesTracks"]
    if muonRecFlags.runCommissioningChain():
        cols +=["EMEO_MSOnlyExtrapolatedTracks", "EMEO_ExtrapolatedMuonTracks", "EMEO_CombinedMuonTracks"]
        colsTP += ["EMEO_MSOnlyExtrapolatedMuonTrackParticles", "EMEO_ExtrapolatedMuonTrackParticles", "EMEO_CombinedMuonTrackParticles"]
    if rec.readESD or (muonCombinedRecFlags.doMuGirl() and muonCombinedRecFlags.doMuGirlLowBeta()):
        colsTP += ["CombinedStauTrackParticles", "ExtrapolatedStauTrackParticles"]
        cols += ["CombinedStauTracks", "ExtrapolatedStauTracks"]
    return colsTP, cols



class MuonCombinedReconstruction(ConfiguredMuonRec):
    def __init__(self,**kwargs):
        ConfiguredMuonRec.__init__(self,"MuonCombinedReconstruction",**kwargs)
        self.configure()

    def configure(self,keys=None):
        super(MuonCombinedReconstruction,self).configure(keys)
        if not self.isEnabled(): return

        topSequence = AlgSequence()

        ## creates input collections for ID and MS candidates
        topSequence += getAlgorithm("MuonCombinedInDetCandidateAlg")
        topSequence += getAlgorithm("MuonCombinedMuonCandidateAlg")
        
        ##### Prepare the MuonCombinedMuonCandidateAlg for EMEO tracks only
        if muonRecFlags.runCommissioningChain():
            topSequence += getAlgorithm("MuonCombinedMuonCandidateAlg_EMEO")
                     
        if InDetFlags.doR3LargeD0(): 
            topSequence += getAlgorithm("MuonCombinedInDetCandidateAlg_LRT")

        # runs ID+MS combinations (fit, staco, mugirl, ID-taggers)
        if muonCombinedRecFlags.doStatisticalCombination() or muonCombinedRecFlags.doCombinedFit():
            topSequence += getAlgorithm("MuonCombinedAlg")
            if InDetFlags.doR3LargeD0(): topSequence += getAlgorithm("MuonCombinedAlg_LRT")
        
        # Creates a new set of candidates from the remnants from MuidCo
        # Candidates are equipped with the muon system extensions
        if muonCombinedRecFlags.doCombinedFit():
            topSequence += getAlgorithm("MuonInDetToMuonSystemExtensionAlg")
            if InDetFlags.doR3LargeD0():
                topSequence += getAlgorithm("MuonInDetToMuonSystemExtensionAlg_LRT")
            

        if muonCombinedRecFlags.doMuGirl():
            topSequence += getAlgorithm("MuonInsideOutRecoAlg")
            if muonCombinedRecFlags.doMuGirlLowBeta():
                topSequence += getAlgorithm("MuGirlStauAlg")
            if InDetFlags.doR3LargeD0(): topSequence += getAlgorithm("MuGirlAlg_LRT")

        if muonCombinedRecFlags.doCaloTrkMuId():
            topSequence += getAlgorithm("MuonCaloTagAlg")
            if InDetFlags.doR3LargeD0(): topSequence += getAlgorithm("MuonCaloTagAlg_LRT")

        if muonCombinedRecFlags.doMuonSegmentTagger():
            getPublicTool("MuonSegmentTagTool")
            topSequence += getAlgorithm("MuonSegmentTagAlg")
            if InDetFlags.doR3LargeD0(): topSequence += getAlgorithm("MuonSegmentTagAlg_LRT")
        
        if muonRecFlags.runCommissioningChain(): 
            topSequence += getAlgorithm("MuonCombinedAlg_EMEO")
    
        # Gather together all segment containers
        topSequence += getAlgorithm("MuonSegContainerMergerAlg")
        ## Create the segment container
        topSequence += CfgMgr.xAODMaker__MuonSegmentCnvAlg("MuonSegmentCnvAlg",
                                                             SegmentContainerName="TrkMuonSegments",
                                                             xAODContainerName="MuonSegments") 
        if muonCombinedRecFlags.saveUnassocSegments():
            topSequence += CfgMgr.xAODMaker__MuonSegmentCnvAlg("UnAssocMuonSegmentCnvAlg",
                                                             SegmentContainerName="UnAssocMuonTrkSegments",
                                                             xAODContainerName="UnAssocMuonSegments") 
        
        if muonCombinedRecFlags.doMuGirlLowBeta() and muonCombinedRecFlags.doMuGirl():
            topSequence += CfgMgr.xAODMaker__MuonSegmentCnvAlg("MuonStauSegmentCnvAlg",
                                                             SegmentContainerName="TrkStauSegments",
                                                             xAODContainerName="StauSegments") 
        # runs over outputs and create xAODMuon collection
        topSequence += getAlgorithm("MuonCreatorAlg")
        if InDetFlags.doR3LargeD0(): topSequence += getAlgorithm("MuonCreatorAlg_LRT")
        # Commissioning chain
        if muonRecFlags.runCommissioningChain(): 
            topSequence += getAlgorithm("MuonCreatorAlg_EMEO") 
            topSequence.MuonCreatorAlg_EMEO.MuonCreatorTool.ParticleCaloExtensionTool.StartFromPerigee=True

        # setting this here, in the new configuration it will be easier to do
        topSequence.MuonCreatorAlg.MuonCreatorTool.ParticleCaloExtensionTool.StartFromPerigee=True
     
        if muonCombinedRecFlags.doMuGirl() and muonCombinedRecFlags.doMuGirlLowBeta():
            topSequence += getAlgorithm("StauCreatorAlg")
        
        track_coll = ["ExtrapolatedMuonTrackParticles", 
                      "CombinedMuonTrackParticles", 
                      "MSOnlyExtrapolatedMuonTrackParticles" ]
        stau_coll = ["CombinedStauTrackParticles",
                    "ExtrapolatedStauTrackParticles"] if muonCombinedRecFlags.doMuGirl() and muonCombinedRecFlags.doMuGirlLowBeta() else []

        track_coll_lrt = ["CombinedMuonsLRTTrackParticles", 
                          "ExtraPolatedMuonsLRTTrackParticles",
                          "MSOnlyExtraPolatedMuonsLRTTrackParticles"] if InDetFlags.doR3LargeD0() else []
        ### Decorate the muon tracks
        for coll in track_coll + stau_coll + track_coll_lrt:
            topSequence += CfgMgr.MuonTrkIDMSScatterDecorAlg("MuonCombIDMSScatterDecorAlg"+coll,
                                                            TrackContainer=coll)
            topSequence+= CfgMgr.MuonTrkAEOTDecorationAlg("MuonCombAEOTDecorAlg"+coll,
                                                          TrackContainer = coll)
        topSequence += CfgMgr.MuonPrecisionLayerDecorAlg("MuonCombPrecisionLayerDecorAlg",
                                                         MuonContainer="Muons",
                                                         TrackContainer=track_coll)
        
        if muonCombinedRecFlags.doMuGirl() and muonCombinedRecFlags.doMuGirlLowBeta(): 
            topSequence += CfgMgr.MuonPrecisionLayerDecorAlg("MuonCombStauPrecisionLayerDecorAlg",
                                                            MuonContainer="Staus",
                                                            TrackContainer=stau_coll)
        if InDetFlags.doR3LargeD0():
              topSequence += CfgMgr.MuonPrecisionLayerDecorAlg("MuonCombLRTPrecisionLayerDecorAlg",
                                                         MuonContainer="MuonsLRT",
                                                         TrackContainer=track_coll_lrt)
            
