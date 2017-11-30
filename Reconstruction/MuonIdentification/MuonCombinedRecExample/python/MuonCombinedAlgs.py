# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from AthenaCommon.CfgGetter import getPublicTool, getAlgorithm,getPublicToolClone
from MuonRecExample.ConfiguredMuonRec import ConfiguredMuonRec
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon import CfgMgr
from AthenaCommon.BeamFlags import jobproperties

def MuonCombinedInDetExtensionAlg(name="MuonCombinedInDetExtensionAlg",**kwargs):
    tools = []
    if muonCombinedRecFlags.doMuGirl():
        tools.append(getPublicTool("MuGirlTagTool"))
    if muonCombinedRecFlags.doCaloTrkMuId():
        tools.append(getPublicTool("MuonCaloTagTool"))
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    return CfgMgr.MuonCombinedInDetExtensionAlg(name,**kwargs)

def MuGirlAlg(name="MuGirlAlg",**kwargs):
    tools = [getPublicTool("MuGirlTagTool")]
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    return CfgMgr.MuonCombinedInDetExtensionAlg(name,**kwargs)


def MuonCaloTagAlg(name="MuonCaloTagAlg",**kwargs):
    tools = [getPublicTool("MuonCaloTagTool")]
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    return CfgMgr.MuonCombinedInDetExtensionAlg(name,**kwargs)

def MuonSegmentTagAlg( name="MuonSegmentTagAlg", **kwargs ):
    kwargs.setdefault("MuonSegmentTagTool", getPublicTool("MuonSegmentTagTool") )
    return CfgMgr.MuonSegmentTagAlg(name,**kwargs)

def MuonInsideOutRecoAlg( name="MuonInsideOutRecoAlg", **kwargs ):
    tools = [getPublicTool("MuonInsideOutRecoTool") ]
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    kwargs.setdefault("usePRDs",True)
    return CfgMgr.MuonCombinedInDetExtensionAlg(name,**kwargs)

def MuGirlStauAlg(name="MuGirlStauAlg",**kwargs):
    tools = [getPublicTool("MuonStauRecoTool")]
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    return CfgMgr.MuonCombinedInDetExtensionAlg(name,**kwargs)
      
def MuonCombinedInDetCandidateAlg( name="MuonCombinedInDetCandidateAlg",**kwargs ):
    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs.setdefault("TrackSelector",getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool") )
    if muonCombinedRecFlags.doSiAssocForwardMuons() and InDetFlags.doForwardTracks():
        kwargs.setdefault("DoSiliconAssocForwardMuons", True )
        kwargs.setdefault("InDetForwardTrackSelector", getPublicTool("MuonCombinedInDetDetailedForwardTrackSelectorTool") )
    return CfgMgr.MuonCombinedInDetCandidateAlg(name,**kwargs)

def MuonCombinedMuonCandidateAlg( name="MuonCombinedMuonCandidateAlg", **kwargs ):
    kwargs.setdefault("MuonCandidateTool",getPublicTool("MuonCandidateTool"))
    return CfgMgr.MuonCombinedMuonCandidateAlg(name,**kwargs)

def MuonCombinedAlg( name="MuonCombinedAlg",**kwargs ):
    kwargs.setdefault("MuonCombinedTool",getPublicTool("MuonCombinedTool"))
    return CfgMgr.MuonCombinedAlg(name,**kwargs)

def MuonCreatorAlg( name="MuonCreatorAlg",**kwargs ):
    kwargs.setdefault("MuonCreatorTool",getPublicTool("MuonCreatorTool"))
    return CfgMgr.MuonCreatorAlg(name,**kwargs)

def StauCreatorAlg( name="StauCreatorAlg", **kwargs ):
    kwargs.setdefault("MuonCreatorTool",getPublicTool("StauCreatorTool"))
    kwargs.setdefault("MuonContainerLocation","Staus")
    kwargs.setdefault("CombinedLocation","CombinedStau")
    kwargs.setdefault("ExtrapolatedLocation","ExtrapolatedStau")
    kwargs.setdefault("MSOnlyExtrapolatedLocation","MSOnlyExtrapolatedStau")
    kwargs.setdefault("MuonCandidateLocation","")
    kwargs.setdefault("SegmentContainerName","StauSegments")
    kwargs.setdefault("BuildSlowMuon",1)
    return MuonCreatorAlg(name,**kwargs)

class MuonCombinedReconstruction(ConfiguredMuonRec):
    def __init__(self,**kwargs):
        ConfiguredMuonRec.__init__(self,"MuonCombinedReconstruction",**kwargs)
        self.configure()

    def configure(self,keys=None):
        super(MuonCombinedReconstruction,self).configure(keys)
        if not self.isEnabled(): return

        topSequence = AlgSequence()
                      
        #if jobproperties.Beam.beamType()=='collisions':
            # creates input collections for ID and MS candidates
        topSequence += getAlgorithm("MuonCombinedInDetCandidateAlg") 
        topSequence += getAlgorithm("MuonCombinedMuonCandidateAlg") 
            
            # runs ID+MS combinations (fit, staco, mugirl, ID-taggers)
        if muonCombinedRecFlags.doStatisticalCombination() or muonCombinedRecFlags.doCombinedFit():
            topSequence += getAlgorithm("MuonCombinedAlg") 

        if muonCombinedRecFlags.doMuGirl():
            topSequence += getAlgorithm("MuonInsideOutRecoAlg") 
            #topSequence += getAlgorithm("MuGirlAlg") 
            if muonCombinedRecFlags.doMuGirlLowBeta():
                topSequence += getAlgorithm("MuGirlStauAlg")

        if muonCombinedRecFlags.doCaloTrkMuId():
            topSequence += getAlgorithm("MuonCaloTagAlg") 
            
        if muonCombinedRecFlags.doMuonSegmentTagger():
            getPublicTool("MuonSegmentTagTool")
            topSequence += getAlgorithm("MuonSegmentTagAlg") 

        # runs over outputs and create xAODMuon collection
        topSequence += getAlgorithm("MuonCreatorAlg")
        
        if muonCombinedRecFlags.doMuGirlLowBeta():
            topSequence += getAlgorithm("StauCreatorAlg")
