# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ISF_SimulationSelectors configurations for ISF
Elmar Ritsch, 04/02/2013
"""

from AthenaCommon import CfgMgr

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *

### DefaultSimSelector configurations

def getDefaultSimSelector(name="ISF_DefaultSimSelector", **kwargs):
    return CfgMgr.ISF__DefaultSimSelector(name, **kwargs )

def getDefaultParticleKillerSelector(name="ISF_DefaultParticleKillerSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_ParticleKillerSvc')
    return getDefaultSimSelector(name, **kwargs )

def getPileupParticleKillerSelector(name="ISF_PileupParticleKillerSelector", **kwargs):
    kwargs.setdefault("PileupBCID"   , [1] ) 
    kwargs.setdefault("Simulator"   , 'ISF_ParticleKillerSvc')
    return CfgMgr.ISF__KinematicPileupSimSelector(name, **kwargs)

def getDefaultGeant4Selector(name="ISF_DefaultGeant4Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_Geant4SimSvc')
    return getDefaultSimSelector(name, **kwargs )

def getDefaultAFIIGeant4Selector(name="ISF_DefaultAFIIGeant4Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_AFIIGeant4SimSvc')
    return getDefaultGeant4Selector(name, **kwargs )

def getDefaultLongLivedGeant4Selector(name="ISF_DefaultLongLivedGeant4Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_LongLivedGeant4SimSvc')
    return getDefaultGeant4Selector(name, **kwargs )

def getFullGeant4Selector(name="ISF_FullGeant4Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FullGeant4SimSvc')
    return getDefaultSimSelector(name, **kwargs )

def getPassBackGeant4Selector(name="ISF_PassBackGeant4Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_PassBackGeant4SimSvc')
    return getDefaultSimSelector(name, **kwargs )

def getDefaultFastCaloSimSelector(name="ISF_DefaultFastCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FastCaloSimSvc')
    return getDefaultSimSelector(name, **kwargs )

def getDefaultLegacyAFIIFastCaloSimSelector(name="ISF_DefaultLegacyAFIIFastCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_LegacyAFIIFastCaloSimSvc')
    return getDefaultSimSelector(name, **kwargs )

def getDefaultFastCaloSimV2Selector(name="ISF_DefaultFastCaloSimV2Selector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FastCaloSimSvcV2')
    return getDefaultSimSelector(name, **kwargs )

def getDefaultDNNCaloSimSelector(name="ISF_DefaultDNNCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_DNNCaloSimSvc')
    return getDefaultSimSelector(name, **kwargs )

def getFastHitConvAlgFastCaloSimSelector(name="ISF_FastHitConvAlgFastCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FastHitConvAlgFastCaloSimSvc')
    return getDefaultSimSelector(name, **kwargs )

def getFastHitConvAlgLegacyAFIIFastCaloSimSelector(name="ISF_FastHitConvAlgLegacyAFIIFastCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FastHitConvAlgLegacyAFIIFastCaloSimSvc')
    return getDefaultSimSelector(name, **kwargs )

def getDefaultFatrasSelector(name="ISF_DefaultFatrasSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FatrasSimSvc')
    return getDefaultSimSelector(name, **kwargs )

def getDefaultFatrasNewExtrapolationSelector(name="ISF_DefaultFatrasNewExtrapolationSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FatrasNewExtrapolationSimSvc')
    return getDefaultSimSelector(name, **kwargs )

def getDefaultParametricSimulationSelector(name="ISF_DefaultParametricSimulationSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_ParametricSimSvc')
    return getDefaultSimSelector(name, **kwargs )

### PileUpSimSelector Configurations

def getPileupSimSelector(name="ISF_PileupSimSelector", **kwargs):
    return CfgMgr.ISF__KinematicPileupSimSelector(name, **kwargs )

def getFatrasPileupSelector(name="ISF_FatrasPileupSelector", **kwargs):
    kwargs.setdefault("PileupBCID"   , [1] )
    kwargs.setdefault("Simulator"   , 'ISF_FatrasPileupSimSvc')
    return getPileupSimSelector(name, **kwargs )

def getFatrasPileupSelector_noHits(name="ISF_FatrasPileupSelector_noHits", **kwargs):
    kwargs.setdefault("PileupBCID"   , [2] )
    kwargs.setdefault("Simulator"   , 'ISF_FatrasPileupSimSvc_noHits')
    return getPileupSimSelector(name, **kwargs )

def getFastCaloSimPileupSelector(name="ISF_FastCaloSimPileupSelector", **kwargs):
    from FastChainPileup.FastChain_jobProperties import FastChain_Flags
    kwargs.setdefault("PileupBCID"  , FastChain_Flags.FastChainBCID() )
    kwargs.setdefault("Simulator"   , 'ISF_FastCaloSimPileupSvc')
    return getPileupSimSelector(name, **kwargs )

def getFastCaloSimPileupOTSelector(name="ISF_FastCaloSimPileupOTSelector", **kwargs):
    from FastChainPileup.FastChain_jobProperties import FastChain_Flags
    kwargs.setdefault("PileupBCID"   , FastChain_Flags.FastChainBCID() )
    kwargs.setdefault("Simulator"   , 'ISF_FastCaloSimPileupOTSvc')
    return getPileupSimSelector(name, **kwargs )

### RandomSimSelector Configurations

def getRandomSimSelector(name="ISF_RandomSimSelector", **kwargs):
    return CfgMgr.ISF__RandomSimSelector(name, **kwargs )

def getFatrasRandomSelector(name="ISF_FatrasRandomSelector", **kwargs):
    kwargs.setdefault("Simulator"   , 'ISF_FatrasSimSvc')
    return getRandomSimSelector(name, **kwargs )

### KinematicSimSelector Configurations

def getElectronGeant4Selector(name="ISF_ElectronGeant4Selector", **kwargs):
    kwargs.setdefault('ParticlePDG'     , 11)
    kwargs.setdefault('Simulator'       , 'ISF_Geant4SimSvc')
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getNeutralGeant4Selector(name="ISF_NeutralGeant4Selector", **kwargs):
    kwargs.setdefault('Charge'      , 0)
    kwargs.setdefault('Simulator'   , 'ISF_Geant4SimSvc')
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getMuonSelector(name="ISF_MuonSelector", **kwargs):
    kwargs.setdefault('ParticlePDG'     , 13)
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getMuonGeant4Selector(name="ISF_MuonGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_Geant4SimSvc')
    return getMuonSelector(name, **kwargs)

def getMuonAFIIGeant4Selector(name="ISF_MuonAFIIGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_AFIIGeant4SimSvc')
    return getMuonGeant4Selector(name, **kwargs)

def getMuonFatrasSelector(name="ISF_MuonFatrasSelector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_FatrasSimSvc')
    return getMuonSelector(name, **kwargs)

def getMuonFatrasPileupSelector(name="ISF_MuonFatrasPileupSelector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_FatrasPileupSimSvc')
    kwargs.setdefault("PileupBCID"      , [1])    
    kwargs.setdefault('ParticlePDG'     , 13)
    return getPileupSimSelector(name, **kwargs)

def getWithinEta5FastCaloSimSelector(name="ISF_WithinEta5FastCaloSimSelector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_FastCaloSimSvc')
    kwargs.setdefault('MinPosEta'       , -5.0 )
    kwargs.setdefault('MaxPosEta'       ,  5.0 )
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)

def getEtaGreater5ParticleKillerSimSelector(name="ISF_EtaGreater5ParticleKillerSimSelector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_ParticleKillerSvc')
    kwargs.setdefault('MinPosEta'       , -5.0 )
    kwargs.setdefault('MaxPosEta'       ,  5.0 )
    kwargs.setdefault('InvertCuts'      , True )
    return CfgMgr.ISF__KinematicSimSelector(name, **kwargs)
  
def getEtaGreater5PileupParticleKillerSimSelector(name="ISF_EtaGreater5PileupParticleKillerSimSelector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_ParticleKillerSvc')
    kwargs.setdefault('MinPosEta'       , -5.0 )
    kwargs.setdefault('MaxPosEta'       ,  5.0 )
    kwargs.setdefault('InvertCuts'      , True )
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
    return getPhotonConeSelector(name, **kwargs)

def getPhotonConeGeant4Selector(name="ISF_PhotonConeGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator', 'ISF_Geant4SimSvc')
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
    kwargs.setdefault('ConeCreatorMinPt'            , 0.          )
    kwargs.setdefault('ConeSize'                    , 0.4         )
    kwargs.setdefault('CheckConeCreatorAncestors'   , True        )
    kwargs.setdefault('ConeCreatorAncestor'         , [ 443 ]     ) # W
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
    return getBHadronProductsSimSelector(name, **kwargs)

def getBHadronProductsFatrasSelector(name="ISF_BHadronProductsFatrasSelector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_FatrasSimSvc')
    return getBHadronProductsSimSelector(name, **kwargs)

def getTauProductsSimSelector(name="ISF_TauProductsSimSelector", **kwargs):
    kwargs.setdefault('RelativesPDGCode' , [ 15 ] ) # tau
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault('Relation'         , 0 )
    return CfgMgr.ISF__TruthAssocSimSelector(name, **kwargs)

def getTauProductsGeant4Selector(name="ISF_TauProductsGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_Geant4SimSvc')
    return getTauProductsSimSelector(name, **kwargs)

def getZProductsSimSelector(name="ISF_ZProductsSimSelector", **kwargs):
    kwargs.setdefault('RelativesPDGCode' , [ 23 ] ) # Z
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault('Relation'         , 0 )
    return CfgMgr.ISF__TruthAssocSimSelector(name, **kwargs)

def getZProductsGeant4Selector(name="ISF_ZProductsGeant4Selector", **kwargs):
    kwargs.setdefault('Simulator'       , 'ISF_Geant4SimSvc')
    return getZProductsSimSelector(name, **kwargs)

### HistorySimSelector Configurations

def getSubDetStickyGeant4SimSelector(name="ISF_SubDetStickyGeant4SimSelector", **kwargs):
    kwargs.setdefault('PrevSimSvc'             , 'ISF_Geant4SimSvc')
    kwargs.setdefault('RequiresUnchangedGeoID' , True                           )
    kwargs.setdefault('Simulator'              , 'ISF_Geant4SimSvc')
    return CfgMgr.ISF__HistorySimSelector(name, **kwargs)

def getGlobalStickyGeant4SimSelector(name="ISF_GlobalStickyGeant4SimSelector", **kwargs):
    kwargs.setdefault('PrevSimSvc'             , 'ISF_Geant4SimSvc')
    kwargs.setdefault('RequiresUnchangedGeoID' , False                          )
    kwargs.setdefault('Simulator'              , 'ISF_Geant4SimSvc')
    return CfgMgr.ISF__HistorySimSelector(name, **kwargs)
