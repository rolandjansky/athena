# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF
KG Tan, 17/06/2012
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

ISF__GenericTruthStrategy, ISF__GenParticleFinalStateFilter, ISF__GenParticlePositionFilter, ISF__GenParticleGenericFilter, ISF__GenParticleInteractingFilter=CompFactory.getComps("ISF__GenericTruthStrategy","ISF__GenParticleFinalStateFilter","ISF__GenParticlePositionFilter","ISF__GenParticleGenericFilter","ISF__GenParticleInteractingFilter",)

from AthenaCommon.SystemOfUnits import MeV, mm

#Functions yet to be migrated:
#getParticleSimWhiteList, getParticlePositionFilterMS
#getTruthStrategyGroupID, getTruthStrategyGroupIDHadInt, getTruthStrategyGroupCaloMuBrem_MC15, getTruthStrategyGroupCaloDecay, getValidationTruthStrategy, getLLPTruthStrategy

#--------------------------------------------------------------------------------------------------
## GenParticleFilters

def ParticleFinalStateFilterCfg(ConfigFlags, name="ISF_ParticleFinalStateFilter", **kwargs):
    result = ComponentAccumulator()
    G4NotInUse = not ConfigFlags.Sim.UsingGeant4
    G4NotInUse = G4NotInUse and ConfigFlags.Sim.ISFRun
    # use CheckGenInteracting==False to allow GenEvent neutrinos to propagate into the simulation
    kwargs.setdefault("CheckGenSimStable", G4NotInUse)
    kwargs.setdefault("CheckGenInteracting", G4NotInUse)

    result.setPrivateTools(ISF__GenParticleFinalStateFilter(name, **kwargs))
    return result

def ParticlePositionFilterCfg(ConfigFlags, name="ISF_ParticlePositionFilter", **kwargs):
    result = ComponentAccumulator()
    # ParticlePositionFilter
    kwargs.setdefault('GeoIDService' , 'ISF_GeoIDSvc'    ) #this svc updated - add once Merge request gone in

    result.setPrivateTools(ISF__GenParticlePositionFilter(name, **kwargs))
    return result

def ParticlePositionFilterIDCfg(ConfigFlags, name="ISF_ParticlePositionFilterID", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr

    kwargs.setdefault('CheckRegion'  , [ AtlasRegion.fAtlasID ] )
    return ParticlePositionFilterCfg(ConfigFlags, name, **kwargs)

def ParticlePositionFilterCaloCfg(ConfigFlags, name="ISF_ParticlePositionFilterCalo", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr

    kwargs.setdefault('CheckRegion'  , [ AtlasRegion.fAtlasID,
                                            AtlasRegion.fAtlasForward,
                                            AtlasRegion.fAtlasCalo ] )
    return ParticlePositionFilterCfg(ConfigFlags, name, **kwargs)

def ParticlePositionFilterMSCfg(name="ISF_ParticlePositionFilterMS", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr

    kwargs.setdefault('CheckRegion'  , [ AtlasRegion.fAtlasID,
                                            AtlasRegion.fAtlasForward,
                                            AtlasRegion.fAtlasCalo,
                                            AtlasRegion.fAtlasMS ] )
    return ParticlePositionFilterCfg(name, **kwargs)

def ParticlePositionFilterWorldCfg(ConfigFlags, name="ISF_ParticlePositionFilterWorld", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr
    kwargs.setdefault('CheckRegion'  , [ AtlasRegion.fAtlasID,
                                            AtlasRegion.fAtlasForward,
                                            AtlasRegion.fAtlasCalo,
                                            AtlasRegion.fAtlasMS,
                                            AtlasRegion.fAtlasCavern ] )
    return ParticlePositionFilterCfg(ConfigFlags, name, **kwargs)

def ParticlePositionFilterDynamicCfg(ConfigFlags, name="ISF_ParticlePositionFilterDynamic", **kwargs):
    # automatically choose the best fitting filter region
    
    #if ConfigFlags.Detector.SimulateMuon:
    if True:
      return ParticlePositionFilterWorldCfg(ConfigFlags, name, **kwargs)
    elif ConfigFlags.Detector.SimulateCalo:
      return ParticlePositionFilterCaloCfg(ConfigFlags, name, **kwargs)
    elif ConfigFlags.Detector.SimulateID:
      return ParticlePositionFilterIDCfg(ConfigFlags, name, **kwargs)
    else:
      return ParticlePositionFilterWorldCfg(ConfigFlags, name, **kwargs)

def GenParticleInteractingFilterCfg(ConfigFlags, name="ISF_GenParticleInteractingFilter", **kwargs):
    result = ComponentAccumulator()

    #todo (dnoel) - add this functionality
    #from G4AtlasApps.SimFlags import simFlags
    #simdict = simFlags.specialConfiguration.get_Value()
    #if simdict is not None and "InteractingPDGCodes" in simdict:
    #    kwargs.setdefault('AdditionalInteractingParticleTypes', eval(simdict["InteractingPDGCodes"]))
    #if simdict is not None and "NonInteractingPDGCodes" in simdict:
    #    kwargs.setdefault('AdditionalNonInteractingParticleTypes', eval(simdict["InteractingNonPDGCodes"]))

    result.setPrivateTools(ISF__GenParticleInteractingFilter(name, **kwargs))
    return result

def EtaPhiFilterCfg(ConfigFlags, name="ISF_EtaPhiFilter", **kwargs):
    result = ComponentAccumulator()
    # EtaPhiFilter
    EtaRange = 7.0 if ConfigFlags.Detector.SimulateLucid else 6.0
    kwargs.setdefault('MinEta' , -EtaRange)
    kwargs.setdefault('MaxEta' , EtaRange)
    kwargs.setdefault('MaxApplicableRadius', 30*mm)

    result.setPrivateTools(ISF__GenParticleGenericFilter(name, **kwargs))
    return result

#--------------------------------------------------------------------------------------------------
## Truth Strategies

# Brems: fBremsstrahlung (3)
# Conversion: fGammaConversion (14), fGammaConversionToMuMu (15), fPairProdByCharged (4)
# Decay: 201-298, fAnnihilation(5), fAnnihilationToMuMu (6), fAnnihilationToHadrons (7)
# Ionization: fIonisation (2), fPhotoElectricEffect (12)
# Hadronic: (111,121,131,141,151,161,210)
# Compton: fComptonScattering (13)
# G4 process types:
#  http://www-geant4.kek.jp/lxr/source//processes/management/include/G4ProcessType.hh
# G4 EM sub types:
#  http://www-geant4.kek.jp/lxr/source//processes/electromagnetic/utils/include/G4EmProcessSubType.hh
# G4 HadInt sub types:
#  http://www-geant4.kek.jp/lxr/source//processes/hadronic/management/include/G4HadronicProcessType.hh#L46
def TruthStrategyGroupID_MC15Cfg(ConfigFlags, name="ISF_MCTruthStrategyGroupID_MC15", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault('ParentMinPt'         , 100.*MeV)
    kwargs.setdefault('ChildMinPt'          , 300.*MeV)
    kwargs.setdefault('VertexTypes'         , [ 3, 14, 15, 4, 5, 6, 7, 2, 12, 13 ])
    kwargs.setdefault('VertexTypeRangeLow'  , 201)  # All kinds of decay processes
    kwargs.setdefault('VertexTypeRangeHigh' , 298)  # ...
    kwargs.setdefault('Regions', [1,2]) # Could import AtlasDetDescr::AtlasRegion enum as in TruthService CfgGetter methods here
    result.setPrivateTools(ISF__GenericTruthStrategy(name, **kwargs))
    return result

def TruthStrategyGroupIDHadInt_MC15Cfg(ConfigFlags, name="ISF_MCTruthStrategyGroupIDHadInt_MC15", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault('ParentMinPt'                       , 100.*MeV)
    kwargs.setdefault('ChildMinPt'                        , 300.*MeV)
    kwargs.setdefault('VertexTypes'                       , [ 111, 121, 131, 141, 151, 161, 210 ])
    kwargs.setdefault('AllowChildrenOrParentPassKineticCuts' , True)
    kwargs.setdefault('Regions', [1])
    result.setPrivateTools(ISF__GenericTruthStrategy(name, **kwargs))
    return result

def TruthStrategyGroupCaloMuBremCfg(ConfigFlags, name="ISF_MCTruthStrategyGroupCaloMuBrem", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault('ParentMinEkin'       , 500.*MeV)
    kwargs.setdefault('ChildMinEkin'        , 100.*MeV)
    kwargs.setdefault('VertexTypes'         , [ 3 ])
    kwargs.setdefault('ParentPDGCodes'      , [ 13, -13 ])
    kwargs.setdefault('Regions', [3])
    result.setPrivateTools(ISF__GenericTruthStrategy(name, **kwargs))
    return result

def TruthStrategyGroupCaloDecay_MC15Cfg(ConfigFlags, name="ISF_MCTruthStrategyGroupCaloDecay_MC15", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault('ParentMinEkin'       , 1000.*MeV)
    kwargs.setdefault('ChildMinEkin'        , 500.*MeV)
    kwargs.setdefault('VertexTypes'         , [ 5, 6, 7 ])
    kwargs.setdefault('VertexTypeRangeLow'  , 201)  # All kinds of decay processes
    kwargs.setdefault('VertexTypeRangeHigh' , 298)  # ...
    kwargs.setdefault('Regions', [3])
    result.setPrivateTools(ISF__GenericTruthStrategy(name, **kwargs))
    return result
