"""ComponentAccumulator HepMC tools configurations for ISF

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.SystemOfUnits import MeV, mm
from ISF_Services.ISF_ServicesCoreConfigNew import GeoIDSvcCfg


# GenParticleFilters
def ParticleFinalStateFilterCfg(ConfigFlags, name="ISF_ParticleFinalStateFilter", **kwargs):
    result = ComponentAccumulator()
    G4NotInUse = not ConfigFlags.Sim.UsingGeant4
    G4NotInUse = G4NotInUse and ConfigFlags.Sim.ISFRun
    # use CheckGenInteracting==False to allow GenEvent neutrinos to propagate into the simulation
    kwargs.setdefault("CheckGenSimStable", G4NotInUse)
    kwargs.setdefault("CheckGenInteracting", G4NotInUse)
    result.setPrivateTools(CompFactory.ISF.GenParticleFinalStateFilter(name, **kwargs))
    return result

def ParticleSimWhiteListCfg(ConfigFlags, name="ISF_ParticleSimWhiteList", **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.ISF.GenParticleSimWhiteList(name, **kwargs))
    return result

def ParticleSimWhiteList_ExtraParticles(ConfigFlags, name="ISF_ParticleSimWhiteList_ExtraParticles", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("WhiteLists" , ["G4particle_whitelist.txt", "G4particle_whitelist_ExtraParticles.txt"] )
    result.setPrivateTools(CompFactory.ISF.GenParticleSimWhiteList(name, **kwargs))
    return result

def ParticlePositionFilterCfg(ConfigFlags, name="ISF_ParticlePositionFilter", **kwargs):
    result = GeoIDSvcCfg(ConfigFlags)
    # ParticlePositionFilter
    kwargs.setdefault("GeoIDService", result.getService("ISF_GeoIDSvc"))
    result.setPrivateTools(CompFactory.ISF.GenParticlePositionFilter(name, **kwargs))
    return result

def ParticlePositionFilterIDCfg(ConfigFlags, name="ISF_ParticlePositionFilterID", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary("AtlasDetDescrDict")
    AtlasRegion = ROOT.AtlasDetDescr

    kwargs.setdefault("CheckRegion"  , [ AtlasRegion.fAtlasID ] )
    return ParticlePositionFilterCfg(ConfigFlags, name, **kwargs)

def ParticlePositionFilterCaloCfg(ConfigFlags, name="ISF_ParticlePositionFilterCalo", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary("AtlasDetDescrDict")
    AtlasRegion = ROOT.AtlasDetDescr

    kwargs.setdefault("CheckRegion"  , [ AtlasRegion.fAtlasID,
                                            AtlasRegion.fAtlasForward,
                                            AtlasRegion.fAtlasCalo ] )
    return ParticlePositionFilterCfg(ConfigFlags, name, **kwargs)

def ParticlePositionFilterMSCfg(name="ISF_ParticlePositionFilterMS", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary("AtlasDetDescrDict")
    AtlasRegion = ROOT.AtlasDetDescr

    kwargs.setdefault("CheckRegion"  , [ AtlasRegion.fAtlasID,
                                            AtlasRegion.fAtlasForward,
                                            AtlasRegion.fAtlasCalo,
                                            AtlasRegion.fAtlasMS ] )
    return ParticlePositionFilterCfg(name, **kwargs)

def ParticlePositionFilterWorldCfg(ConfigFlags, name="ISF_ParticlePositionFilterWorld", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary("AtlasDetDescrDict")
    AtlasRegion = ROOT.AtlasDetDescr
    kwargs.setdefault("CheckRegion"  , [ AtlasRegion.fAtlasID,
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
    #    kwargs.setdefault("AdditionalInteractingParticleTypes", eval(simdict["InteractingPDGCodes"]))
    #if simdict is not None and "NonInteractingPDGCodes" in simdict:
    #    kwargs.setdefault("AdditionalNonInteractingParticleTypes", eval(simdict["InteractingNonPDGCodes"]))

    result.setPrivateTools(CompFactory.ISF.GenParticleInteractingFilter(name, **kwargs))
    return result

def EtaPhiFilterCfg(ConfigFlags, name="ISF_EtaPhiFilter", **kwargs):
    result = ComponentAccumulator()
    # EtaPhiFilter
    EtaRange = 7.0 if ConfigFlags.Detector.SimulateLucid else 6.0
    kwargs.setdefault("MinEta" , -EtaRange)
    kwargs.setdefault("MaxEta" , EtaRange)
    kwargs.setdefault("MaxApplicableRadius", 30*mm)

    result.setPrivateTools(CompFactory.ISF.GenParticleGenericFilter(name, **kwargs))
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
    kwargs.setdefault("ParentMinPt", 100.*MeV)
    kwargs.setdefault("ChildMinPt" , 300.*MeV)
    kwargs.setdefault("VertexTypes", [3, 14, 15, 4, 5, 6, 7, 2, 12, 13])
    kwargs.setdefault("VertexTypeRangeLow", 201)  # All kinds of decay processes
    kwargs.setdefault("VertexTypeRangeHigh", 298)  # ...
    kwargs.setdefault("Regions", [1,2]) # Could import AtlasDetDescr::AtlasRegion enum as in TruthService CfgGetter methods here
    result.setPrivateTools(CompFactory.ISF.GenericTruthStrategy(name, **kwargs))
    return result


def TruthStrategyGroupIDHadInt_MC15Cfg(ConfigFlags, name="ISF_MCTruthStrategyGroupIDHadInt_MC15", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParentMinPt", 100.*MeV)
    kwargs.setdefault("ChildMinPt" , 300.*MeV)
    kwargs.setdefault("VertexTypes", [111, 121, 131, 141, 151, 161, 210])
    kwargs.setdefault("AllowChildrenOrParentPassKineticCuts", True)
    kwargs.setdefault("Regions", [1])
    result.setPrivateTools(CompFactory.ISF.GenericTruthStrategy(name, **kwargs))
    return result


def TruthStrategyGroupCaloMuBremCfg(ConfigFlags, name="ISF_MCTruthStrategyGroupCaloMuBrem", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParentMinEkin", 500.*MeV)
    kwargs.setdefault("ChildMinEkin" , 100.*MeV)
    kwargs.setdefault("VertexTypes"  , [3])
    kwargs.setdefault("ParentPDGCodes", [13, -13])
    kwargs.setdefault("Regions", [3])
    result.setPrivateTools(CompFactory.ISF.GenericTruthStrategy(name, **kwargs))
    return result


def TruthStrategyGroupCaloDecay_MC15Cfg(ConfigFlags, name="ISF_MCTruthStrategyGroupCaloDecay_MC15", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParentMinEkin", 1000.*MeV)
    kwargs.setdefault("ChildMinEkin" , 500.*MeV)
    kwargs.setdefault("VertexTypes"  , [5, 6, 7])
    kwargs.setdefault("VertexTypeRangeLow" , 201)  # All kinds of decay processes
    kwargs.setdefault("VertexTypeRangeHigh", 298)  # ...
    kwargs.setdefault("Regions", [3])
    result.setPrivateTools(CompFactory.ISF.GenericTruthStrategy(name, **kwargs))
    return result


def TruthStrategyGroupIDCfg(ConfigFlags, name="ISF_MCTruthStrategyGroupID", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParentMinPt", 100.*MeV)
    kwargs.setdefault("ChildMinPt" , 100.*MeV)
    kwargs.setdefault("VertexTypes", [3, 14, 15, 4, 5, 6, 7, 2, 12, 13])
    kwargs.setdefault("VertexTypeRangeLow"  , 201)  # All kinds of decay processes
    kwargs.setdefault("VertexTypeRangeHigh" , 298)  # ...
    kwargs.setdefault("Regions", [1,2])
    result.setPrivateTools(CompFactory.ISF.GenericTruthStrategy(name, **kwargs))
    return result


def TruthStrategyGroupIDHadIntCfg(ConfigFlags, name="ISF_MCTruthStrategyGroupIDHadInt", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParentMinPt", 100.*MeV)
    kwargs.setdefault("ChildMinPt" , 100.*MeV)
    kwargs.setdefault("VertexTypes", [111, 121, 131, 141, 151, 161, 210])
    kwargs.setdefault("AllowChildrenOrParentPassKineticCuts", True)
    kwargs.setdefault("Regions", [1])
    result.setPrivateTools(CompFactory.ISF.GenericTruthStrategy(name, **kwargs))
    return result


def TruthStrategyGroupCaloMuBrem_MC15Cfg(ConfigFlags, name="ISF_MCTruthStrategyGroupCaloMuBrem_MC15", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParentMinEkin", 500.*MeV)
    kwargs.setdefault("ChildMinEkin" , 300.*MeV)
    kwargs.setdefault("VertexTypes"  , [3])
    kwargs.setdefault("ParentPDGCodes", [13, -13])
    kwargs.setdefault("Regions", [3])
    result.setPrivateTools(CompFactory.ISF.GenericTruthStrategy(name, **kwargs))
    return result


def TruthStrategyGroupCaloDecayCfg(ConfigFlags, name="ISF_MCTruthStrategyGroupCaloDecay", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParentMinPt", 1000.*MeV)
    kwargs.setdefault("ChildMinPt" , 500.*MeV)
    kwargs.setdefault("VertexTypes", [5, 6, 7])
    kwargs.setdefault("VertexTypeRangeLow" , 201)  # All kinds of decay processes
    kwargs.setdefault("VertexTypeRangeHigh", 298)  # ...
    kwargs.setdefault("Regions", [3])
    result.setPrivateTools(CompFactory.ISF.GenericTruthStrategy(name, **kwargs))
    return result

def ValidationTruthStrategyCfg(ConfigFlags, name="ISF_ValidationTruthStrategy", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParentMinP", 50.*MeV)
    kwargs.setdefault("Regions", [1,3])
    result.setPrivateTools(CompFactory.ISF.ValidationTruthStrategy(name, **kwargs))
    return result


def LLPTruthStrategyCfg(ConfigFlags, name="ISF_LLPTruthStrategy", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("PassProcessCodeRangeLow",  200)
    kwargs.setdefault("PassProcessCodeRangeHigh", 299)
    # ProcessCategory==9 corresponds to the "fUserDefined" G4ProcessType:
    #   http://www-geant4.kek.jp/lxr/source//processes/management/include/G4ProcessType.hh
    kwargs.setdefault("PassProcessCategory", 9) # ==
    kwargs.setdefault("Regions", [1,2,3,4])
    result.setPrivateTools(CompFactory.ISF.LLPTruthStrategy(name, **kwargs))
    return result


def KeepLLPDecayChildrenStrategyCfg(ConfigFlags, name="ISF_KeepLLPDecayChildrenStrategy", **kwargs):
    result = ComponentAccumulator()
    # ProcessCategory==9 corresponds to the "fUserDefined" G4ProcessType:
    #   http://www-geant4.kek.jp/lxr/source//processes/management/include/G4ProcessType.hh
    kwargs.setdefault("PassProcessCategory", 9) # ==
    kwargs.setdefault("VertexTypeRangeLow" , 200) # All kinds of decay processes
    kwargs.setdefault("VertexTypeRangeHigh", 299) # ...
    kwargs.setdefault("BSMParent"          , True)
    result.setPrivateTools(CompFactory.ISF.KeepChildrenTruthStrategy(name, **kwargs))
    return result


def KeepLLPHadronicInteractionChildrenStrategyCfg(ConfigFlags, name="ISF_KeepLLPHadronicInteractionChildrenStrategy", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("VertexTypes", [111, 121, 131, 141, 151, 161, 210])
    kwargs.setdefault("BSMParent"  , True)
    result.setPrivateTools(CompFactory.ISF.KeepChildrenTruthStrategy(name, **kwargs))
    return result


def KeepAllDecayChildrenStrategyCfg(ConfigFlags, name="ISF_KeepAllDecayChildrenStrategy", **kwargs):
    result = ComponentAccumulator()
    # ProcessCategory==9 corresponds to the "fUserDefined" G4ProcessType:
    #   http://www-geant4.kek.jp/lxr/source//processes/management/include/G4ProcessType.hh
    kwargs.setdefault("PassProcessCategory", 9) # ==
    kwargs.setdefault("VertexTypeRangeLow" , 200) # All kinds of decay processes
    kwargs.setdefault("VertexTypeRangeHigh", 299) # ...
    result.setPrivateTools(CompFactory.ISF.KeepChildrenTruthStrategy(name, **kwargs))
    return result


def KeepHadronicInteractionChildrenStrategyCfg(ConfigFlags, name="ISF_KeepHadronicInteractionChildrenStrategy", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("VertexTypes", [111, 121, 131, 141, 151, 161, 210])
    result.setPrivateTools(CompFactory.ISF.KeepChildrenTruthStrategy(name, **kwargs))
    return result
