# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from xAODTrigMuonCnv.xAODTrigMuonCnvConf import TrigMuonEFInfoToMuonCnvTool, TrigMuonEFInfoCnvTestAlg, xAODMaker__TrigMuonEFInfoToMuonCnvAlg, MuonFeatureCnvTestAlg, xAODMaker__MuonFeatureCnvAlg

# Version of the converter tool that does not
# make links to xAOD ID tracks (needed until EF ID is converted)
def TrigMuonEFInfoToMuonCnvToolNoIDTrkLinks(name='TrigMuonEFInfoToMuonCnvToolNoIDTrkLinks',**kwargs):
    kwargs.setdefault('MakeIDTrackLink',False)
    kwargs.setdefault('xAODEFInDetTrackParticleContainerName','')
    return TrigMuonEFInfoToMuonCnvTool(name, **kwargs)

# Converter algorithm for TrigMuonEF results
def TrigMuonEFInfoToMuonCnvAlgTMEF(name='TrigMuonEFInfoToMuonCnvAlgTMEF',**kwargs):
    kwargs.setdefault('ConverterTool','TrigMuonEFInfoToMuonCnvToolNoIDTrkLinks')
    return xAODMaker__TrigMuonEFInfoToMuonCnvAlg(name, **kwargs)

# Converter algorithm for TrigMuGirl results
def TrigMuonEFInfoToMuonCnvAlgTMG(name='TrigMuonEFInfoToMuonCnvAlgTMG',**kwargs):
    conf = TrigMuonEFInfoToMuonCnvAlgTMEF(name, **kwargs)
    conf.AODContainerName = 'HLT_eMuonEFInfo'
    conf.xAODMuonContainerName = 'HLT_eMuons'
    conf.xAODCombinedTrackParticleContainerName = 'HLT_eMuonCombinedTrackParticles'
    conf.xAODExtrapolatedTrackParticleContainerName = 'HLT_eMuonExtrapolatedTrackParticles'
    return conf

# Test algorithm for TrigMuonEF conversion
def TrigMuonEFInfoCnvTestAlgTMEF(name='TrigMuonEFInfoCnvTestAlgTMEF',**kwargs):
    return TrigMuonEFInfoCnvTestAlg(name,**kwargs)

# Converter algorithm for L2MuonSA results
def MuonFeatureCnvAlgL2SA(name='MuonFeatureCnvAlgL2SA',**kwargs):
    return xAODMaker__MuonFeatureCnvAlg(name, **kwargs)

# Test algorithm for MuonFeature conversion
def MuonFeatureCnvTestAlgL2SA(name='MuonFeatureCnvTestAlgL2SA',**kwargs):
    return MuonFeatureCnvTestAlg(name,**kwargs)
