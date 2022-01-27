if not hasattr(prefiltSeq, 'xAODCnv'):
    from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
    prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
    prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

def createxAODSlimmedContainer(container_name,prefiltSeq):
    if container_name=="TruthElectrons":
        if not hasattr(prefiltSeq, "xAODTruthParticleSlimmerElectron"):
            from GeneratorFilters.GeneratorFiltersConf import xAODTruthParticleSlimmerElectron
            prefiltSeq += xAODTruthParticleSlimmerElectron('xAODTruthParticleSlimmerElectron')
    elif container_name=="TruthMuons":
        if not hasattr(prefiltSeq, "xAODTruthParticleSlimmerMuon"):
            from GeneratorFilters.GeneratorFiltersConf import xAODTruthParticleSlimmerMuon
            prefiltSeq += xAODTruthParticleSlimmerMuon('xAODTruthParticleSlimmerMuon')
    elif container_name=="TruthTaus":
        if not hasattr(prefiltSeq, "xAODTruthParticleSlimmerTau"):
            from GeneratorFilters.GeneratorFiltersConf import xAODTruthParticleSlimmerTau
            prefiltSeq += xAODTruthParticleSlimmerTau('xAODTruthParticleSlimmerTau')
    elif container_name=="TruthPhotons":
        if not hasattr(prefiltSeq, "xAODTruthParticleSlimmerPhoton"):
            from GeneratorFilters.GeneratorFiltersConf import xAODTruthParticleSlimmerPhoton
            prefiltSeq += xAODTruthParticleSlimmerPhoton('xAODTruthParticleSlimmerPhoton')
    elif container_name=="TruthMET":
        if not hasattr(prefiltSeq, "xAODTruthParticleSlimmerMET"):
            from GeneratorFilters.GeneratorFiltersConf import xAODTruthParticleSlimmerMET
            prefiltSeq += xAODTruthParticleSlimmerMET('xAODTruthParticleSlimmerMET')
    elif container_name=="TruthLightLeptons":
        if not hasattr(prefiltSeq, "xAODTruthParticleSlimmerLightLepton"):
            from GeneratorFilters.GeneratorFiltersConf import xAODTruthParticleSlimmerLightLepton
            prefiltSeq += xAODTruthParticleSlimmerLightLepton('xAODTruthParticleSlimmerLightLepton')           
    else:
        from AthenaCommon.Logging import logging
        msg_logger = logging.getLogger("CreatexAODSlimContainers ")       
        msg_logger.fatal("GeneratorFilters/CreatexAODSlimmedContainers -> container "+container_name+ " not implemented yet!!!")
        theApp.exit(-1)


