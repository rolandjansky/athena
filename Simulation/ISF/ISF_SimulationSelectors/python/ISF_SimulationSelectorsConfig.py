# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
ISF_SimulationSelectors configurations for ISF
Elmar Ritsch, 04/02/2013
"""

from AthenaCommon import CfgMgr

from ISF_SimulationSelectors import SimulationFlavor
### DefaultSimSelector configurations

def getDefaultSimSelector(name="ISF_DefaultSimSelector", **kwargs):
    return CfgMgr.ISF__DefaultSimSelector(name, **kwargs )

def getDefaultParticleKillerSelector(name="ISF_DefaultParticleKillerSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_ParticleKillerSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.ParticleKiller)
    return getDefaultSimSelector(name, **kwargs )

def getPileupParticleKillerSelector(name="ISF_PileupParticleKillerSelector", **kwargs):
    kwargs.setdefault("PileupBCID"   , [1] )
    kwargs.setdefault("Simulator"   , 'ISF_ParticleKillerSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.ParticleKiller)
    return CfgMgr.ISF__KinematicPileupSimSelector(name, **kwargs)

def getDefaultGeant4Selector(name="ISF_DefaultGeant4Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return getDefaultSimSelector(name, **kwargs )

def getDefaultAFIIGeant4Selector(name="ISF_DefaultAFIIGeant4Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_AFIIGeant4SimSvc')
    return getDefaultGeant4Selector(name, **kwargs )

def getDefaultLongLivedGeant4Selector(name="ISF_DefaultLongLivedGeant4Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_LongLivedGeant4SimSvc')
    return getDefaultGeant4Selector(name, **kwargs )

def getDefaultAFII_QS_Geant4Selector(name="ISF_DefaultAFII_QS_Geant4Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_AFII_QS_Geant4SimSvc')
    return getDefaultGeant4Selector(name, **kwargs )

def getFullGeant4Selector(name="ISF_FullGeant4Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FullGeant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return getDefaultSimSelector(name, **kwargs )

def getPassBackGeant4Selector(name="ISF_PassBackGeant4Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_PassBackGeant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return getDefaultSimSelector(name, **kwargs )

def getDefaultFastCaloSimSelector(name="ISF_DefaultFastCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FastCaloSimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.FastCaloSim)
    return getDefaultSimSelector(name, **kwargs )

def getDefaultLegacyAFIIFastCaloSimSelector(name="ISF_DefaultLegacyAFIIFastCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_LegacyAFIIFastCaloSimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.FastCaloSim)
    return getDefaultSimSelector(name, **kwargs )

def getDefaultFastCaloSimV2Selector(name="ISF_DefaultFastCaloSimV2Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FastCaloSimSvcV2')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.FastCaloSimV2)
    return getDefaultSimSelector(name, **kwargs )

def getDefaultDNNCaloSimSelector(name="ISF_DefaultDNNCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_DNNCaloSimSvc')
    return getDefaultSimSelector(name, **kwargs )

def getFastHitConvAlgFastCaloSimSelector(name="ISF_FastHitConvAlgFastCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FastHitConvAlgFastCaloSimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.FastCaloSim)
    return getDefaultSimSelector(name, **kwargs )

def getFastHitConvAlgLegacyAFIIFastCaloSimSelector(name="ISF_FastHitConvAlgLegacyAFIIFastCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FastHitConvAlgLegacyAFIIFastCaloSimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.FastCaloSim)
    return getDefaultSimSelector(name, **kwargs )

def getDefaultFatrasSelector(name="ISF_DefaultFatrasSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FatrasSimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Fatras)
    return getDefaultSimSelector(name, **kwargs )

def getDefaultFatrasNewExtrapolationSelector(name="ISF_DefaultFatrasNewExtrapolationSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FatrasNewExtrapolationSimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Fatras)
    return getDefaultSimSelector(name, **kwargs )

def getDefaultParametricSimulationSelector(name="ISF_DefaultParametricSimulationSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_ParametricSimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Parametric)
    return getDefaultSimSelector(name, **kwargs )

### PileUpSimSelector Configurations

def getPileupSimSelector(name="ISF_PileupSimSelector", **kwargs):
    return CfgMgr.ISF__KinematicPileupSimSelector(name, **kwargs )

def getFatrasPileupSelector(name="ISF_FatrasPileupSelector", **kwargs):
    kwargs.setdefault("PileupBCID"   , [1] )
    kwargs.setdefault("Simulator"   , 'ISF_FatrasPileupSimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.FatrasPileup)
    return getPileupSimSelector(name, **kwargs )

def getFatrasPileupSelector_noHits(name="ISF_FatrasPileupSelector_noHits", **kwargs):
    kwargs.setdefault("PileupBCID"   , [2] )
    kwargs.setdefault("Simulator"   , 'ISF_FatrasPileupSimSvc_noHits')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.FatrasPileup)
    return getPileupSimSelector(name, **kwargs )

def getFastCaloSimPileupSelector(name="ISF_FastCaloSimPileupSelector", **kwargs):
    from FastChainPileup.FastChain_jobProperties import FastChain_Flags
    kwargs.setdefault("PileupBCID"  , FastChain_Flags.FastChainBCID() )
    kwargs.setdefault("Simulator"   , 'ISF_FastCaloSimPileupSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.FastCaloSimPileup)
    return getPileupSimSelector(name, **kwargs )

def getFastCaloSimPileupOTSelector(name="ISF_FastCaloSimPileupOTSelector", **kwargs):
    from FastChainPileup.FastChain_jobProperties import FastChain_Flags
    kwargs.setdefault("PileupBCID"   , FastChain_Flags.FastChainBCID() )
    kwargs.setdefault("Simulator"   , 'ISF_FastCaloSimPileupOTSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.FastCaloSimPileup)
    return getPileupSimSelector(name, **kwargs )

### KinematicSimSelector Configurations

def getElectronGeant4Selector(name="ISF_ElectronGeant4Selector", **kwargs):
    kwargs.setdefault('ParticlePDG'     , 11)
    kwargs.setdefault('Simulator'       , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getNeutralGeant4Selector(name="ISF_NeutralGeant4Selector", **kwargs):
    kwargs.setdefault('Charge'      , 0)
    kwargs.setdefault('Simulator'   , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getProtonAFIIGeant4Selector(name="ISF_ProtonAFIIGeant4Selector", **kwargs):
    kwargs.setdefault('MaxMom'          , 750)
    kwargs.setdefault('ParticlePDG'     , 2212)
    kwargs.setdefault('Simulator'       , 'ISF_AFIIGeant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getProtonAFII_QS_Geant4Selector(name="ISF_ProtonAFII_QS_Geant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_AFII_QS_Geant4SimSvc')
    return getProtonAFIIGeant4Selector(name, **kwargs)

def getPionAFIIGeant4Selector(name="ISF_PionAFIIGeant4Selector", **kwargs):
    kwargs.setdefault('MaxMom'          , 200)
    kwargs.setdefault('ParticlePDG'     , 211)
    kwargs.setdefault('Simulator'       , 'ISF_AFIIGeant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getPionG4FastCaloGeant4Selector(name="ISF_PionG4FastCaloGeant4Selector", **kwargs):
    kwargs.setdefault('MaxEkin'         , 200)
    kwargs.setdefault('ParticlePDG'     , 211)
    kwargs.setdefault('Simulator'       , 'ISF_AFIIGeant4SimSvc')
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getProtonG4FastCaloGeant4Selector(name="ISF_ProtonG4FastCaloGeant4Selector", **kwargs):
    kwargs.setdefault('MaxEkin'         , 400)
    kwargs.setdefault('ParticlePDG'     , 2212)
    kwargs.setdefault('Simulator'       , 'ISF_AFIIGeant4SimSvc')
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getNeutronG4FastCaloGeant4Selector(name="ISF_NeutronG4FastCaloGeant4Selector", **kwargs):
    kwargs.setdefault('MaxEkin'         , 400)
    kwargs.setdefault('ParticlePDG'     , 2112)
    kwargs.setdefault('Simulator'       , 'ISF_AFIIGeant4SimSvc')
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getChargedKaonG4FastCaloGeant4Selector(name="ISF_ChargedKaonG4FastCaloGeant4Selector", **kwargs):
    kwargs.setdefault('MaxEkin'         , 400)
    kwargs.setdefault('ParticlePDG'     , 321)
    kwargs.setdefault('Simulator'       , 'ISF_AFIIGeant4SimSvc')
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getKLongG4FastCaloGeant4Selector(name="ISF_KLongG4FastCaloGeant4Selector", **kwargs):
    kwargs.setdefault('MaxEkin'         , 400)
    kwargs.setdefault('ParticlePDG'     , 130)
    kwargs.setdefault('Simulator'       , 'ISF_AFIIGeant4SimSvc')
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getPionAFII_QS_Geant4Selector(name="ISF_PionAFII_QS_Geant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_AFII_QS_Geant4SimSvc')
    return getPionAFIIGeant4Selector(name, **kwargs)

def getChargedKaonAFIIGeant4Selector(name="ISF_ChargedKaonAFIIGeant4Selector", **kwargs):
    kwargs.setdefault('MaxMom'          , 750)
    kwargs.setdefault('ParticlePDG'     , 321)
    kwargs.setdefault('Simulator'       , 'ISF_AFIIGeant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getChargedKaonAFII_QS_Geant4Selector(name="ISF_ChargedKaonAFII_QS_Geant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_AFII_QS_Geant4SimSvc')
    return getChargedKaonAFIIGeant4Selector(name, **kwargs)

def getKLongAFIIGeant4Selector(name="ISF_KLongAFIIGeant4Selector", **kwargs):
    kwargs.setdefault('MaxMom'          , 750)
    kwargs.setdefault('ParticlePDG'     , 130)
    kwargs.setdefault('Simulator'       , 'ISF_AFIIGeant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getKLongAFII_QS_Geant4Selector(name="ISF_KLongAFII_QS_Geant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_AFII_QS_Geant4SimSvc')
    return getKLongAFIIGeant4Selector(name, **kwargs)

def getMuonSelector(name="ISF_MuonSelector", **kwargs):
    kwargs.setdefault('ParticlePDG'     , 13)
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getMuonGeant4Selector(name="ISF_MuonGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return getMuonSelector(name, **kwargs)

def getMuonAFIIGeant4Selector(name="ISF_MuonAFIIGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_AFIIGeant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return getMuonGeant4Selector(name, **kwargs)

def getMuonAFII_QS_Geant4Selector(name="ISF_MuonAFII_QS_Geant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_AFII_QS_Geant4SimSvc')
    return getMuonGeant4Selector(name, **kwargs)

def getMuonFatrasSelector(name="ISF_MuonFatrasSelector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_FatrasSimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Fatras)
    return getMuonSelector(name, **kwargs)

def getMuonFatrasPileupSelector(name="ISF_MuonFatrasPileupSelector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_FatrasPileupSimSvc')
    kwargs.setdefault("PileupBCID"      , [1])
    kwargs.setdefault('ParticlePDG'     , 13)
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Fatras)
    return getPileupSimSelector(name, **kwargs)

def getWithinEta5FastCaloSimSelector(name="ISF_WithinEta5FastCaloSimSelector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_FastCaloSimSvc')
    kwargs.setdefault('MinPosEta'       , -5.0 )
    kwargs.setdefault('MaxPosEta'       ,  5.0 )
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.FastCaloSim)
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getEtaGreater5ParticleKillerSimSelector(name="ISF_EtaGreater5ParticleKillerSimSelector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_ParticleKillerSvc')
    kwargs.setdefault('MinPosEta'       , -5.0 )
    kwargs.setdefault('MaxPosEta'       ,  5.0 )
    kwargs.setdefault('InvertCuts'      , True )
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.ParticleKiller)
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getEtaGreater5PileupParticleKillerSimSelector(name="ISF_EtaGreater5PileupParticleKillerSimSelector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_ParticleKillerSvc')
    kwargs.setdefault('MinPosEta'       , -5.0 )
    kwargs.setdefault('MaxPosEta'       ,  5.0 )
    kwargs.setdefault('InvertCuts'      , True )
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.ParticleKiller)
    return CfgMgr.ISF__KinematicPileupSimSelector(name, **kwargs)

### ConeSimSelector Configurations

def getPhotonConeSelector(name="ISF_PhotonConeSelector", **kwargs):
    kwargs.setdefault('ConeCreatorPDGs'           , [ 22 ] ) # photons
    kwargs.setdefault('ConeCreatorMinPt'          , 20000. ) # 20 GeV
    kwargs.setdefault('ConeSize'                  , 0.6    )
    kwargs.setdefault('CheckConeCreatorAncestors' , False  )
    return CfgMgr.ISF__ConeSimSelector(name, **kwargs)

def getPhotonConeFatrasSelector(name="ISF_PhotonConeFatrasSelector", **kwargs):
    kwargs.setdefault('Simulator', 'ISF_FatrasSimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Fatras)
    return getPhotonConeSelector(name, **kwargs)

def getPhotonConeGeant4Selector(name="ISF_PhotonConeGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator', 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return getPhotonConeSelector(name, **kwargs)

def getHiggsLeptonsConeSimSelector(name="ISF_HiggsLeptonsConeSimSelector", **kwargs):
    kwargs.setdefault('ConeCreatorPDGs'              , [ 11 , 13 ] ) # e, mu
    kwargs.setdefault('ConeCreatorMinPt'             , 0.          )
    kwargs.setdefault('ConeSize'                     , 0.4         )
    kwargs.setdefault('CheckConeCreatorAncestors'    , True        )
    kwargs.setdefault('ConeCreatorAncestor'          , [ 24 , 23 ] ) # W , Z
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault('ConeCreatorAncestorRelation'  , 1           )
    return CfgMgr.ISF__ConeSimSelector(name, **kwargs)

def getHiggsLeptonsConeGeant4Selector(name="ISF_HiggsLeptonsConeGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return getHiggsLeptonsConeSimSelector(name, **kwargs)

def getElectronsMuonsConeSimSelector(name="ISF_ElectronsMuonsConeSimSelector", **kwargs):
    kwargs.setdefault('ConeCreatorPDGs'             , [ 11 , 13 ] ) # e, mu
    return CfgMgr.ISF__ConeSimSelector(name, **kwargs)

def getHiggsLeptonsConeGeant4CaloSelector(name="ISF_HiggsLeptonsConeGeant4CaloSelector", **kwargs):
    kwargs.setdefault( 'ExtrapolateToCaloEntry', True                           )
    kwargs.setdefault( 'Extrapolator'          , 'ISF_CaloEntryTrkExtrapolator' )
    return getHiggsLeptonsConeGeant4Selector(name, **kwargs)

def getWLeptonsConeGeant4Selector(name="ISF_WLeptonsConeGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator'                   , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    kwargs.setdefault('ConeCreatorMinPt'            , 0.          )
    kwargs.setdefault('ConeSize'                    , 0.4         )
    kwargs.setdefault('CheckConeCreatorAncestors'   , True        )
    kwargs.setdefault('ConeCreatorAncestor'         , [ 24 ]      ) # W
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault('ConeCreatorAncestorRelation' , 0           )
    return getElectronsMuonsConeSimSelector(name, **kwargs)

def getZLeptonsDirectionConeGeant4Selector(name="ISF_ZLeptonsDirectionConeGeant4Selector", **kwargs):
    # this selector picks all particles with a mometum direction
    # within DeltaR<ConeSize relative to the Z decay leopton directions
    kwargs.setdefault('Simulator'                   , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    kwargs.setdefault('ConeCreatorMinPt'            , 0.          )
    kwargs.setdefault('ConeSize'                    , 0.4         )
    kwargs.setdefault('CheckConeCreatorAncestors'   , True        )
    kwargs.setdefault('ConeCreatorAncestor'         , [ 23 ]      ) # Z
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault('ConeCreatorAncestorRelation' , 0           )
    return getElectronsMuonsConeSimSelector(name, **kwargs)

def getZLeptonsPositionConeGeant4Selector(name="ISF_ZLeptonsPositionConeGeant4Selector", **kwargs):
    # this selector picks all particles with a position inside a cone
    # around the Z decay leptons directions
    kwargs.setdefault('CheckParticlePosition'       , True        )
    return getZLeptonsDirectionConeGeant4Selector(name, **kwargs)

def getJPsiLeptonsConeGeant4Selector(name="ISF_JPsiLeptonsConeGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator'                   , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    kwargs.setdefault('ConeCreatorMinPt'            , 0.          )
    kwargs.setdefault('ConeSize'                    , 0.4         )
    kwargs.setdefault('CheckConeCreatorAncestors'   , True        )
    kwargs.setdefault('ConeCreatorAncestor'         , [ 443 ]     ) # J/Psi
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault('ConeCreatorAncestorRelation' , 0           )
    return getElectronsMuonsConeSimSelector(name, **kwargs)

### TruthAssocSimSelector configurations

def getBHadronProductsSimSelector(name="ISF_BHadronProductsSimSelector", **kwargs):
    kwargs.setdefault('RelativesPDGCode' , [ 511 , 521 , 531 , 541 ,
                                             5122, 5112, 5132, 5232,
                                             5212, 5222              ] ) # b hadrons
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault('Relation'         , 2 )
    return CfgMgr.ISF__TruthAssocSimSelector(name, **kwargs)

def getBHadronProductsGeant4Selector(name="ISF_BHadronProductsGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return getBHadronProductsSimSelector(name, **kwargs)

def getBHadronProductsFatrasSelector(name="ISF_BHadronProductsFatrasSelector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_FatrasSimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Fatras)
    return getBHadronProductsSimSelector(name, **kwargs)

def getTauProductsSimSelector(name="ISF_TauProductsSimSelector", **kwargs):
    kwargs.setdefault('RelativesPDGCode' , [ 15 ] ) # tau
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault('Relation'         , 0 )
    return CfgMgr.ISF__TruthAssocSimSelector(name, **kwargs)

def getTauProductsGeant4Selector(name="ISF_TauProductsGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return getTauProductsSimSelector(name, **kwargs)

def getZProductsSimSelector(name="ISF_ZProductsSimSelector", **kwargs):
    kwargs.setdefault('RelativesPDGCode' , [ 23 ] ) # Z
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault('Relation'         , 0 )
    return CfgMgr.ISF__TruthAssocSimSelector(name, **kwargs)

def getZProductsGeant4Selector(name="ISF_ZProductsGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return getZProductsSimSelector(name, **kwargs)

### HistorySimSelector Configurations

def getSubDetStickyGeant4SimSelector(name="ISF_SubDetStickyGeant4SimSelector", **kwargs):
    kwargs.setdefault('PrevSimSvc'             , 'ISF_Geant4SimSvc')
    kwargs.setdefault('RequiresUnchangedGeoID' , True                           )
    kwargs.setdefault('Simulator'              , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return CfgMgr.ISF__HistorySimSelector(name, **kwargs)

def getGlobalStickyGeant4SimSelector(name="ISF_GlobalStickyGeant4SimSelector", **kwargs):
    kwargs.setdefault('PrevSimSvc'             , 'ISF_Geant4SimSvc')
    kwargs.setdefault('RequiresUnchangedGeoID' , False                          )
    kwargs.setdefault('Simulator'              , 'ISF_Geant4SimSvc')
    kwargs.setdefault('SimulationFlavor', SimulationFlavor.Geant4)
    return CfgMgr.ISF__HistorySimSelector(name, **kwargs)
