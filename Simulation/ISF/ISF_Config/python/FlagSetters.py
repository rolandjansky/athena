# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from ISF_Config.ISF_jobProperties import ISF_Flags


## Base method

def configureFlagsBase():
    from AthenaCommon.DetFlags import DetFlags
    if not DetFlags.any_on():
        # If you configure some of the DetFlags then you're
        # responsible for configuring all of them.
        DetFlags.all_setOn()
        DetFlags.Truth_setOn() # FIXME redundant?
        DetFlags.LVL1_setOff() # LVL1 is not part of G4 sim
        DetFlags.Forward_setOff() # Forward dets are off by default
        DetFlags.geometry.DBM_setOff()

    ## Configure tasks
    DetFlags.digitize.all_setOff()
    DetFlags.makeRIO.all_setOff()
    from G4AtlasApps.SimFlags import simFlags
    if not (hasattr(simFlags, 'IsEventOverlayInputSim') and simFlags.IsEventOverlayInputSim()):
        # Unless this is an Overlay input simulation job switch off
        # the overlay DetFlags.
        DetFlags.overlay.all_setOff()
    DetFlags.pileup.all_setOff()
    DetFlags.readRDOBS.all_setOff()
    DetFlags.readRDOPool.all_setOff()
    DetFlags.readRIOBS.all_setOff()
    DetFlags.readRIOPool.all_setOff()
    DetFlags.simulateLVL1.all_setOff()
    DetFlags.writeBS.all_setOff()
    DetFlags.writeRDOPool.all_setOff()
    DetFlags.writeRIOPool.all_setOff()

    if "G4" in ISF_Flags.Simulator():
        # Short-cut: if G4 is in the simulator name, then it's a pretty
        # safe assumption that the configuration uses Geant4.
        ISF_Flags.UsingGeant4 = True
    ISF_Flags.ParticleBroker = "ISF_ParticleBrokerSvc"
    return

## methods for Geant4 only simulators

def configureFlagsFullG4():
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "FullG4"
    mergeDict = {'ID':False, 'CALO':False, 'MUON':False}
    ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)
    ISF_Flags.ParticleBroker = "ISF_ParticleBrokerSvcNoOrdering"
    return

def configureFlagsFullG4_LongLived():
    configureFlagsFullG4()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "FullG4_LongLived"
    return

def configureFlagsFullG4_IDOnly():
    configureFlagsFullG4()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "FullG4_IDOnly"
    return

def configureFlagsFullG4_IDCalo():
    configureFlagsFullG4()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "FullG4_IDCalo"
    return

def configureFlagsPassBackG4():
    configureFlagsFullG4()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "PassBackG4"
    return

def configureFlagsCosmicsG4():
    configureFlagsFullG4()
    return

## Legacy Geant4 only simulators

def configureFlagsMC12G4():
    configureFlagsFullG4()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "MC12G4"
    return

def configureFlagsMC12G4_longLived():
    configureFlagsFullG4_LongLived()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "MC12G4_longLived"
    return

def configureFlagsMC12G4_IDOnly():
    configureFlagsFullG4_IDOnly()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "MC12G4_IDOnly"
    return

def configureFlagsMC12G4_IDCalo():
    configureFlagsFullG4_IDCalo()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "MC12G4_IDCalo"
    return

## methods for simulators which use G4 + FastCaloSim

def configureFlagsATLFASTII():
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "AtlfastII" # TODO: can we rename this to "ATLFASTII" ?
    from ISF_Config.ISF_jobProperties import ISF_Flags
    ISF_Flags.UsingGeant4 = True
    mergeDict = {'ID':False, 'CALO':True, 'MUON':False}
    ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)
    ISF_Flags.ParticleBroker = "ISF_AFIIParticleBrokerSvc"
    return

def configureFlagsATLFASTII_QS():
    configureFlagsATLFASTII()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "ATLFASTII_QS"
    return

def configureFlagsATLFASTII_PileUp():
    configureFlagsATLFASTII()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "ATLFASTII_PileUp"
    return

## methods for simulators which use G4 + FastCaloSim V2

def configureFlagsG4FastCalo():
    configureFlagsATLFASTII()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "G4FastCalo"
    return

def configureFlagsG4FastCaloTest():
    configureFlagsATLFASTII()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "G4FastCaloTest"
    return

def configureFlagsG4FastCaloDNN():
    configureFlagsATLFASTII()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "G4FastCaloDNN"
    return

## methods for simulators which use Fatras + FastCaloSim

def configureFlagsATLFASTIIF():
    from AthenaCommon.DetFlags import DetFlags
    # BCM simulation not supported in FATRAS. Currently it is the
    # geometry task which is used to determine whether a detector has
    # been simulated (see ISF_Example/python/ISF_Output.py)
    DetFlags.geometry.BCM_setOff()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "ATLFASTIIF"
    from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
    TrkDetFlags.MaterialVersion=21
    TrkDetFlags.TRT_BuildStrawLayers=True
    from ISF_Config.ISF_jobProperties import ISF_Flags
    mergeDict = {'ID':True, 'CALO':True, 'MUON':True}
    ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)
    return

def configureFlagsFatras_newExtrapolation():
    configureFlagsATLFASTIIF()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "Fatras_newExtrapolation"
    return

def configureFlagsFatras_newExtrapolation_IDOnly():
    configureFlagsATLFASTIIF()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "Fatras_newExtrapolation_IDOnly"
    return

def configureFlagsATLFASTIIF_PileUp():
    configureFlagsATLFASTIIF()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "ATLFASTIIF_PileUp"
    return
  
def configureFlagsG4HS_FastPileup():
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "G4HS_FastPileup"
    from ISF_Config.ISF_jobProperties import ISF_Flags
    ISF_Flags.UsingGeant4 = True
    from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
    TrkDetFlags.MaterialVersion=21
    TrkDetFlags.TRT_BuildStrawLayers=True
    from ISF_Config.ISF_jobProperties import ISF_Flags
    mergeDict = {'ID':True, 'CALO':True, 'MUON':True}
    ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)
    return

def configureFlagsATLFASTIIF_IDOnly():
    configureFlagsATLFASTIIF()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "ATLFASTIIF_IDOnly"
    return

def configureFlagsATLFASTIIF_IDCalo():
    configureFlagsATLFASTIIF()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "ATLFASTIIF_IDCalo"
    return

def configureFlagsFastOnly():
    configureFlagsATLFASTIIF()
    return

## methods for simulators which combine Geant4,  Fatras and FastCaloSim

def configureFlagsATLFASTIIF_G4MS():
    configureFlagsATLFASTIIF()
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "ATLFASTIIF_G4MS"
    from ISF_Config.ISF_jobProperties import ISF_Flags
    ISF_Flags.UsingGeant4 = True
    ISF_Flags.ParticleBroker = "ISF_AFIIParticleBrokerSvc"
    return

def configureFlagsMultiSimTest():
    from ISF_Config.ISF_jobProperties import ISF_Flags
    ISF_Flags.UsingGeant4 = True
    mergeDict = {'ID':True, 'CALO':True, 'MUON':True}
    ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)
    return

def configureFlagsG4GammaCones():
    return

def configureFlagsG4GammaCones_FastCalo():
    return

def configureFlagsFatrasGammaCones_FastCalo():
    return

def configureFlagsG4BHadronsOnly_FastCalo():
    return

def configureFlagsG4BHadrons_FatrasID_FastCalo():
    return

def configureFlagsG4TauProductsOnly():
    return

def configureFlagsG4HiggsLeptonsConeOnly():
    return

def configureFlagsG4HiggsLeptons():
    return

def configureFlagsG4WLeptonsCone_FatrasID_FastCalo ():
    return

def configureFlagsG4JPsiLeptonsCone_FatrasID_FastCalo ():
    return

def configureFlagsG4ZLeptonsCone_FatrasID_FastCalo ():
    return

def configureFlagsG4ZLeptonsConeSticky_FatrasID_FastCalo ():
    return

def configureFlagsG4ZDecayProducts_FatrasID_FastCalo ():
    return

