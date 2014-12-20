# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from AthenaCommon.CfgGetter import getPublicTool, getAlgorithm
from MuonRecExample.ConfiguredMuonRec import ConfiguredMuonRec
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon import CfgMgr

def MuonCombinedInDetExtensionAlg(name="MuonCombinedInDetExtensionAlg",**kwargs):
    tools = []
    if muonCombinedRecFlags.doMuGirl():
        tools.append(getPublicTool("MuGirlTagTool"))
    if muonCombinedRecFlags.doCaloTrkMuId():
        tools.append(getPublicTool("MuonCaloTagTool"))
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    return CfgMgr.MuonCombinedInDetExtensionAlg(name,**kwargs)

class MuonCombinedReconstruction(ConfiguredMuonRec):
    def __init__(self,**kwargs):
        ConfiguredMuonRec.__init__(self,"MuonCombinedReconstruction",**kwargs)
        self.configure()

    def configure(self,keys=None):
        super(MuonCombinedReconstruction,self).configure(keys)
        if not self.isEnabled(): return

        topSequence = AlgSequence()

        getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool")
        getPublicTool("MuonCaloParticleCreator")
        getPublicTool("MuonCombinedParticleCreator")
        getPublicTool("InDetCandidateTool")
        getPublicTool("MuonCandidateTool")
        getPublicTool("MuonCreatorTool")
        # creates input collections for ID and MS candidates
        topSequence += getAlgorithm("MuonCombinedInDetCandidateAlg") 
        topSequence += getAlgorithm("MuonCombinedMuonCandidateAlg") 

        # runs ID+MS combinations (fit, staco, mugirl, ID-taggers)
        if muonCombinedRecFlags.doStatisticalCombination() or muonCombinedRecFlags.doCombinedFit():
            getPublicTool("MuonCombinedTool")
            topSequence += getAlgorithm("MuonCombinedAlg") 

        if muonCombinedRecFlags.doMuGirl() or muonCombinedRecFlags.doCaloTrkMuId():
            topSequence += getAlgorithm("MuonCombinedInDetExtensionAlg") 

        if muonCombinedRecFlags.doMuonSegmentTagger():
            getPublicTool("MuonSegmentTagTool")
            topSequence += getAlgorithm("MuonSegmentTagAlg") 

        # runs over outputs and create xAODMuon collection
        topSequence += getAlgorithm("MuonCreatorAlg")
        
        if muonCombinedRecFlags.doMuGirlLowBeta():
            topSequence += getAlgorithm("StauCreatorAlg")
