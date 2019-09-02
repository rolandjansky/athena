# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
Service configurations for ISF
KG Tan, 17/06/2012
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from BarcodeServices.BarcodeServicesConfigNew import MC15aPlusBarcodeSvcCfg
from ISF_HepMC_Tools.ISF_HepMC_ToolsConfigNew import TruthStrategyGroupID_MC15Cfg, TruthStrategyGroupCaloMuBremCfg, TruthStrategyGroupCaloDecay_MC15Cfg, TruthStrategyGroupIDHadInt_MC15Cfg, ParticleFinalStateFilterCfg, ParticlePositionFilterDynamicCfg, EtaPhiFilterCfg, GenParticleInteractingFilterCfg
from SubDetectorEnvelopes.SubDetectorEnvelopesConfigNew import EnvelopeDefSvcCfg


from ISF_Services.ISF_ServicesConf import ISF__TruthSvc, ISF__GeoIDSvc, ISF__ISFEnvelopeDefSvc, ISF__InputConverter

#Functions yet to be migrated:
#getParticleBrokerSvcNoOrdering, getParticleBrokerSvc, getAFIIParticleBrokerSvc, getAFIIEnvelopeDefSvc, getAFIIGeoIDSvc
#getLongLivedInputConverter, getValidationTruthService, getMC12BeamPipeTruthStrategies, getMC12IDTruthStrategies
#getMC12CaloTruthStrategies, getMC12MSTruthStrategies, getMC12TruthService, getTruthService, getMC12LLPTruthService, getMC12PlusTruthService,  getMC15IDTruthStrategies
#getMC15CaloTruthStrategies

def ISFEnvelopeDefSvcCfg(ConfigFlags, name="ISF_ISFEnvelopeDefSvc", **kwargs):
    result = EnvelopeDefSvcCfg(ConfigFlags)
    # ATLAS common envlope definitions
    kwargs.setdefault("ATLASEnvelopeDefSvc", result.getService("AtlasGeometry_EnvelopeDefSvc"))

    result.addService(ISF__ISFEnvelopeDefSvc(name, **kwargs))
    return result



def GeoIDSvcCfg(ConfigFlags, name="ISF_GeoIDSvc", **kwargs):
    result = ISFEnvelopeDefSvcCfg(ConfigFlags)
    # with ISF volume definitions
    kwargs.setdefault("EnvelopeDefSvc", result.getService("ISF_ISFEnvelopeDefSvc"))
    result.addService(ISF__GeoIDSvc(name, **kwargs))
    return result


def GenParticleFiltersToolCfg(ConfigFlags):
    result = ComponentAccumulator()

    #acc1 = ParticlePositionFilterDynamicCfg(ConfigFlags)
    genParticleFilterList = []

    acc1 = ParticleFinalStateFilterCfg(ConfigFlags)
    #genParticleFilterList = ['ISF_ParticleFinalStateFilter'] # not used for Quasi-stable particle simulation
    genParticleFilterList += [result.popToolsAndMerge(acc1)]

    #from G4AtlasApps.SimFlags import simFlags
    #if "ATLAS" in simFlags.SimLayout():
    if "ATLAS" in ConfigFlags.GeoModel.Layout or "atlas" in ConfigFlags.GeoModel.Layout:
        #from AthenaCommon.BeamFlags import jobproperties
        #if jobproperties.Beam.beamType() != "cosmics":

        if True:
        #if ConfigFlags.Beam.Type != "cosmics":

            acc2 = ParticlePositionFilterDynamicCfg(ConfigFlags)
            genParticleFilterList += [result.popToolsAndMerge(acc2)]
            #genParticleFilterList += ['ISF_ParticlePositionFilterDynamic']


            #if (not simFlags.CavernBG.statusOn) or simFlags.CavernBG.get_Value() == 'Signal':
            #if ConfigFlags.Sim.CavernBG == 'Signal': #or if off?
            if True: 
                acc3 = EtaPhiFilterCfg(ConfigFlags)
                genParticleFilterList += [result.popToolsAndMerge(acc3)]
                #genParticleFilterList += ['ISF_EtaPhiFilter']

    acc4 = GenParticleInteractingFilterCfg(ConfigFlags)
    genParticleFilterList += [result.popToolsAndMerge(acc4)]
    #genParticleFilterList += ['ISF_GenParticleInteractingFilter']

    result.setPrivateTools(genParticleFilterList)
    return result


def InputConverterCfg(ConfigFlags, name="ISF_InputConverter", **kwargs):
    result = ComponentAccumulator()

    #just use this barcodeSvc for now. TODO - make configurable
    #from G4AtlasApps.SimFlags import simFlags
    #kwargs.setdefault('BarcodeSvc', simFlags.TruthStrategy.BarcodeServiceName())
    result = MC15aPlusBarcodeSvcCfg(ConfigFlags)
    kwargs.setdefault('BarcodeSvc', result.getService("Barcode_MC15aPlusBarcodeSvc") )

    kwargs.setdefault("UseGeneratedParticleMass", False)

    acc_GenParticleFiltersList = GenParticleFiltersToolCfg(ConfigFlags)
    kwargs.setdefault("GenParticleFilters", result.popToolsAndMerge(acc_GenParticleFiltersList) )

    result.addService(ISF__InputConverter(name, **kwargs))
    return result


#
# Generic Truth Service Configurations
#

def GenericTruthServiceCfg(ConfigFlags, name="ISF_TruthService", **kwargs):
    #from G4AtlasApps.SimFlags import simFlags
    #kwargs.setdefault('BarcodeSvc', simFlags.TruthStrategy.BarcodeServiceName())
    result = MC15aPlusBarcodeSvcCfg(ConfigFlags)
    kwargs.setdefault('BarcodeSvc', result.getService("Barcode_MC15aPlusBarcodeSvc") )
    
    kwargs.setdefault('SkipIfNoChildren', True)
    kwargs.setdefault('SkipIfNoParentBarcode', True)
    kwargs.setdefault('ForceEndVtxInRegions', [])
    #long_lived_simulators = ['LongLived', 'longLived', 'QS']
    #from ISF_Config.ISF_jobProperties import ISF_Flags
    #is_long_lived_simulation = any(x in ISF_Flags.Simulator() for x in long_lived_simulators) #FIXME this should be set in a nicer way.
    is_long_lived_simulation = True
    if is_long_lived_simulation:
        kwargs.setdefault('QuasiStableParticlesIncluded', True)

    result.addService(ISF__TruthSvc(name, **kwargs))
    return result


def MC15TruthServiceCfg(ConfigFlags, name="ISF_MC15TruthService", **kwargs):
    result = ComponentAccumulator()
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr

    acc1 = TruthStrategyGroupID_MC15Cfg(ConfigFlags)
    acc2 = TruthStrategyGroupIDHadInt_MC15Cfg(ConfigFlags)
    acc3 = TruthStrategyGroupCaloMuBremCfg(ConfigFlags)
    acc4 = TruthStrategyGroupCaloDecay_MC15Cfg(ConfigFlags)

    kwargs.setdefault('TruthStrategies', [result.popToolsAndMerge(acc1),
                                          result.popToolsAndMerge(acc2),
                                          result.popToolsAndMerge(acc3), #FIXME this should be ISF_MCTruthStrategyGroupCaloMuBrem_MC15!!
                                          result.popToolsAndMerge(acc4)])


    kwargs.setdefault('IgnoreUndefinedBarcodes', False)
    kwargs.setdefault('PassWholeVertices', False) # new for MC15 - can write out partial vertices.
    kwargs.setdefault('ForceEndVtxInRegions', [AtlasRegion.fAtlasID])
    accTruthService = GenericTruthServiceCfg(ConfigFlags, name, **kwargs)
    result.merge(accTruthService)
    return result


def MC15aPlusTruthServiceCfg(ConfigFlags, name="ISF_MC15aPlusTruthService", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr
    kwargs.setdefault('ForceEndVtxInRegions', [AtlasRegion.fAtlasID])
    result = MC15TruthServiceCfg(ConfigFlags, name, **kwargs)
    return result

