"""
ISF_SimulationSelectors for ComponentAccumulator configuration

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_SimulationSelectors import SimulationFlavor
from ISF_Services.ISF_ServicesCoreConfigNew import ParticleKillerSvcCfg
from ISF_Geant4Services.ISF_Geant4ServicesConfigNew import (
    Geant4SimCfg, AFIIGeant4SimCfg, LongLivedGeant4SimCfg, AFII_QS_Geant4SimCfg,
    FullGeant4SimCfg, PassBackGeant4SimCfg,
)


def DefaultParticleKillerSelectorCfg(flags, name="ISF_DefaultParticleKillerSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(ParticleKillerSvcCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.ParticleKiller)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def PileupParticleKillerSelectorCfg(flags, name="ISF_PileupParticleKillerSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PileupBCID", [1])
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(ParticleKillerSvcCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.ParticleKiller)
    acc.setPrivateTools(CompFactory.ISF.KinematicPileupSimSelector(name, **kwargs))
    return acc


def DefaultGeant4SelectorCfg(flags, name="ISF_DefaultGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if "Simulator" not in kwargs:
        if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
            kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(Geant4SimCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultAFIIGeant4SelectorCfg(flags, name="ISF_DefaultAFIIGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFIIGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    tool = acc.popToolsAndMerge(DefaultGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def DefaultLongLivedGeant4SelectorCfg(flags, name="ISF_DefaultLongLivedGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(LongLivedGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_LongLivedGeant4SimSvc"))
    tool = acc.popToolsAndMerge(DefaultGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def DefaultAFII_QS_Geant4SelectorCfg(flags, name="ISF_DefaultAFII_QS_Geant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFII_QS_Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    tool = acc.popToolsAndMerge(DefaultGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def FullGeant4SelectorCfg(flags, name="ISF_FullGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(FullGeant4SimCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def PassBackGeant4SelectorCfg(flags, name="ISF_PassBackGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(PassBackGeant4SimCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultFastCaloSimSelectorCfg(flags, name="ISF_DefaultFastCaloSimSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigNew import FastCaloSimSvcCfg
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(FastCaloSimSvcCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultLegacyAFIIFastCaloSimSelectorCfg(flags, name="ISF_DefaultLegacyAFIIFastCaloSimSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigNew import LegacyAFIIFastCaloSimSvcCfg
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(LegacyAFIIFastCaloSimSvcCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultFastCaloSimV2SelectorCfg(flags, name="ISF_DefaultFastCaloSimV2Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigNew import FastCaloSimV2SvcCfg
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(FastCaloSimV2SvcCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSimV2)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultDNNCaloSimSelectorCfg(flags, name="ISF_DefaultDNNCaloSimSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigNew import DNNCaloSimSvcCfg
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(DNNCaloSimSvcCfg(flags)).name)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def FastHitConvAlgFastCaloSimSelectorCfg(flags, name="ISF_FastHitConvAlgFastCaloSimSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigNew import FastHitConvAlgFastCaloSimSvcCfg
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(FastHitConvAlgFastCaloSimSvcCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultFatrasSelectorCfg(flags, name="ISF_DefaultFatrasSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        from ISF_FatrasServices.ISF_FatrasConfig import fatrasSimServiceIDCfg
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(fatrasSimServiceIDCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultFatrasNewExtrapolationSelectorCfg(flags, name="ISF_DefaultFatrasNewExtrapolationSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultParametricSimulationSelectorCfg(flags, name="ISF_DefaultParametricSimulationSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        kwargs.setdefault("Simulator", "ISF_ParametricSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Parametric)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


# PileUpSimSelector Configurations
def PileupSimSelectorCfg(flags, name="DONOTUSEDIRECTLY", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.ISF.KinematicPileupSimSelector(name, **kwargs))
    return acc


def FatrasPileupSelectorCfg(flags, name="ISF_FatrasPileupSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PileupBCID", [1])
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        from ISF_FatrasServices.ISF_FatrasConfig import fatrasPileupSimServiceIDCfg
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(fatrasPileupSimServiceIDCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FatrasPileup)
    tool = acc.popToolsAndMerge(PileupSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


# FatrasPileupSelector_noHits not migrated
# due to missing getter for Simulator "ISF_FatrasPileupSimSvc_noHits"


def FastCaloSimPileupOTSelectorCfg(flags, name="ISF_FastCaloSimPileupOTSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PileupBCID", flags.Sim.FastChain.BCID)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigNew import FastCaloSimPileupOTSvcCfg
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(FastCaloSimPileupOTSvcCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSimPileup)
    tool = acc.popToolsAndMerge(PileupSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def MuonFatrasPileupSelectorCfg(flags, name="ISF_MuonFatrasPileupSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        from ISF_FatrasServices.ISF_FatrasConfig import fatrasPileupSimServiceIDCfg
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(fatrasPileupSimServiceIDCfg(flags)).name)
    kwargs.setdefault("PileupBCID", [1])
    kwargs.setdefault("ParticlePDG", 13)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    tool = acc.popToolsAndMerge(PileupSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def EtaGreater5PileupParticleKillerSimSelectorCfg(flags, name="ISF_EtaGreater5PileupParticleKillerSimSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(ParticleKillerSvcCfg(flags)).name)
    kwargs.setdefault("MinPosEta", -5.0)
    kwargs.setdefault("MaxPosEta",  5.0)
    kwargs.setdefault("InvertCuts", True)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.ParticleKiller)
    tool = acc.popToolsAndMerge(PileupSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def FastCaloSimPileupSelectorCfg(flags, name="ISF_FastCaloSimPileupSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PileupBCID"  , flags.Sim.FastChain.BCID)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigNew import FastCaloSimPileupSvcCfg
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(FastCaloSimPileupSvcCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSimPileup)
    tool = acc.popToolsAndMerge(PileupSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


### KinematicSimSelector Configurations

# BASE METHODS
def BaseKinematicGeant4SelectorCfg(flags, name="DONOTUSEDIRECTLY", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def BaseKinematicAFIIGeant4SelectorCfg(flags, name="DONOTUSEDIRECTLY", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(AFIIGeant4SimCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def BaseKinematicAFII_QS_Geant4SelectorCfg(flags, name="DONOTUSEDIRECTLY", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFII_QS_Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    tool = acc.popToolsAndMerge(BaseKinematicAFIIGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def BaseKinematicFatrasSelectorCfg(flags, name="DONOTUSEDIRECTLY", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        from ISF_FatrasServices.ISF_FatrasConfig import fatrasSimServiceIDCfg
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(fatrasSimServiceIDCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def BaseKinematicFastCaloSimSelectorCfg(flags, name="DONOTUSEDIRECTLY", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigNew import FastCaloSimSvcCfg
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(FastCaloSimSvcCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def BaseKinematicParticleKillerSimSelectorCfg(flags, name="DONOTUSEDIRECTLY", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        kwargs.setdefault("Simulator", acc.getPrimaryAndMerge(ParticleKillerSvcCfg(flags)).name)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.ParticleKiller)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


# Electrons
def ElectronGeant4SelectorCfg(flags, name="ISF_ElectronGeant4Selector", **kwargs):
    kwargs.setdefault("ParticlePDG", 11)
    return BaseKinematicGeant4SelectorCfg(flags, name, **kwargs)


#Neutrals
def NeutralGeant4SelectorCfg(flags, name="ISF_NeutralGeant4Selector", **kwargs):
    kwargs.setdefault("Charge", 0)
    return BaseKinematicGeant4SelectorCfg(flags, name, **kwargs)


#Protons
def ProtonAFIIGeant4SelectorCfg(flags, name="ISF_ProtonAFIIGeant4Selector", **kwargs):
    kwargs.setdefault("MaxMom", 750)
    kwargs.setdefault("ParticlePDG", 2212)
    return BaseKinematicAFIIGeant4SelectorCfg(flags, name, **kwargs)


def ProtonATLFAST3Geant4SelectorCfg(flags, name="ISF_ProtonATLFAST3Geant4Selector", **kwargs):
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 2212)
    return BaseKinematicAFIIGeant4SelectorCfg(flags, name, **kwargs)


def ProtonAFII_QS_Geant4SelectorCfg(flags, name="ISF_ProtonAFII_QS_Geant4Selector", **kwargs):
    kwargs.setdefault("MaxMom", 750)
    kwargs.setdefault("ParticlePDG", 2212)
    return BaseKinematicAFII_QS_Geant4SelectorCfg(flags, name, **kwargs)


def ProtonATLFAST3_QS_Geant4SelectorCfg(flags, name="ISF_ProtonATLFAST3_QS_Geant4Selector", **kwargs):
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 2212)
    return BaseKinematicAFII_QS_Geant4SelectorCfg(flags, name, **kwargs)


#Pions
def PionAFIIGeant4SelectorCfg(flags, name="ISF_PionAFIIGeant4Selector", **kwargs):
    kwargs.setdefault("MaxMom", 200)
    kwargs.setdefault("ParticlePDG", 211)
    return BaseKinematicAFIIGeant4SelectorCfg(flags, name, **kwargs)


def PionATLFAST3Geant4SelectorCfg(flags, name="ISF_PionATLFAST3Geant4Selector", **kwargs):
    kwargs.setdefault("MaxEkin", 200)
    kwargs.setdefault("ParticlePDG", 211)
    return BaseKinematicAFIIGeant4SelectorCfg(flags, name, **kwargs)


def PionAFII_QS_Geant4SelectorCfg(flags, name="ISF_PionAFII_QS_Geant4Selector", **kwargs):
    kwargs.setdefault("MaxEkin", 200)
    kwargs.setdefault("ParticlePDG", 211)
    return BaseKinematicAFII_QS_Geant4SelectorCfg(flags, name, **kwargs)


def PionATLFAST3_QS_Geant4SelectorCfg (flags, name="ISF_PionATLFAST3_QS_Geant4Selector", **kwargs):
    kwargs.setdefault("MaxEkin", 200)
    kwargs.setdefault("ParticlePDG", 211)
    return BaseKinematicAFII_QS_Geant4SelectorCfg(flags, name, **kwargs)


# Neutrons
def NeutronATLFAST3Geant4SelectorCfg(flags, name="ISF_NeutronATLFAST3Geant4Selector", **kwargs):
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 2112)
    return BaseKinematicAFIIGeant4SelectorCfg(flags, name, **kwargs)


def NeutronATLFAST3_QS_Geant4SelectorCfg(flags, name="ISF_NeutronATLFAST3_QS_Geant4Selector", **kwargs):
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 2112)
    return BaseKinematicAFII_QS_Geant4SelectorCfg(flags, name, **kwargs)


# Charged Kaons
def ChargedKaonAFIIGeant4SelectorCfg(flags, name="ISF_ChargedKaonAFIIGeant4Selector", **kwargs):
    kwargs.setdefault("MaxMom", 750)
    kwargs.setdefault("ParticlePDG", 321)
    return BaseKinematicAFIIGeant4SelectorCfg(flags, name, **kwargs)


def ChargedKaonATLFAST3Geant4SelectorCfg(flags, name="ISF_ChargedKaonATLFAST3Geant4Selector", **kwargs):
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 321)
    return BaseKinematicAFIIGeant4SelectorCfg(flags, name, **kwargs)


def ChargedKaonAFII_QS_Geant4SelectorCfg(flags, name="ISF_ChargedKaonAFII_QS_Geant4Selector", **kwargs):
    kwargs.setdefault("MaxMom", 750)
    kwargs.setdefault("ParticlePDG", 321)
    return BaseKinematicAFII_QS_Geant4SelectorCfg(flags, name, **kwargs)


def ChargedKaonATLFAST3_QS_Geant4SelectorCfg(flags, name="ISF_ChargedKaonATLFAST3_QS_Geant4Selector", **kwargs):
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 321)
    return BaseKinematicAFII_QS_Geant4SelectorCfg(flags, name, **kwargs)


# KLongs
def KLongAFIIGeant4SelectorCfg(flags, name="ISF_KLongAFIIGeant4Selector", **kwargs):
    kwargs.setdefault("MaxMom", 750)
    kwargs.setdefault("ParticlePDG", 130)
    return BaseKinematicAFIIGeant4SelectorCfg(flags, name, **kwargs)


def KLongATLFAST3Geant4SelectorCfg(flags, name="ISF_KLongATLFAST3Geant4Selector", **kwargs):
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 130)
    return BaseKinematicAFIIGeant4SelectorCfg(flags, name, **kwargs)


def KLongAFII_QS_Geant4SelectorCfg(flags, name="ISF_KLongAFII_QS_Geant4Selector", **kwargs):
    kwargs.setdefault("MaxMom", 750)
    kwargs.setdefault("ParticlePDG", 130)
    return BaseKinematicAFII_QS_Geant4SelectorCfg(flags, name, **kwargs)


def KLongATLFAST3_QS_Geant4SelectorCfg(flags, name="ISF_KLongATLFAST3_QS_Geant4Selector", **kwargs):
    kwargs.setdefault('MaxEkin'         , 400)
    kwargs.setdefault('ParticlePDG'     , 130)
    return BaseKinematicAFII_QS_Geant4SelectorCfg(flags, name, **kwargs)


#Muons
def MuonGeant4SelectorCfg(flags, name="ISF_MuonGeant4Selector", **kwargs):
    kwargs.setdefault("ParticlePDG", 13)
    return BaseKinematicGeant4SelectorCfg(flags, name, **kwargs)


def MuonAFIIGeant4SelectorCfg(flags, name="ISF_MuonAFIIGeant4Selector", **kwargs):
    kwargs.setdefault("ParticlePDG", 13)
    return BaseKinematicAFIIGeant4SelectorCfg(flags, name, **kwargs)


def MuonAFII_QS_Geant4SelectorCfg(flags, name="ISF_MuonAFII_QS_Geant4Selector", **kwargs):
    kwargs.setdefault("ParticlePDG", 13)
    return BaseKinematicAFII_QS_Geant4SelectorCfg(flags, name, **kwargs)


def MuonFatrasSelectorCfg(flags, name="ISF_MuonFatrasSelector", **kwargs):
    kwargs.setdefault("ParticlePDG", 13)
    return BaseKinematicFatrasSelectorCfg(flags, name, **kwargs)


# General Eta-based selectors
def WithinEta5FastCaloSimSelectorCfg(flags, name="ISF_WithinEta5FastCaloSimSelector", **kwargs):
    kwargs.setdefault('MinPosEta'       , -5.0 )
    kwargs.setdefault('MaxPosEta'       ,  5.0 )
    return BaseKinematicFastCaloSimSelectorCfg(flags, name, **kwargs)


def EtaGreater5ParticleKillerSimSelectorCfg(flags, name="ISF_EtaGreater5ParticleKillerSimSelector", **kwargs):
    kwargs.setdefault("MinPosEta", -5.0)
    kwargs.setdefault("MaxPosEta",  5.0)
    kwargs.setdefault("InvertCuts", True)
    return BaseKinematicParticleKillerSimSelectorCfg(flags, name, **kwargs)
