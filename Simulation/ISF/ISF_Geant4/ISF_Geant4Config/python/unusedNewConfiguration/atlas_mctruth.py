# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
This module contains the MCTruth strategies that will be applied to the IDET,
CALO and MUON envelopes at level 1 (by default but this may change depending on
the hierarchy of the volumes).
"""

__author__ = 'M. Gallas'

#import PyG4Atlas, AtlasG4Eng
from ISF_Geant4Config.iGeant4 import G4Eng


class MCTruthStrategies(object):
    """ Defines the common MCTruth strategies in use in the ATLAS
        simulations, together with their default parameters.

         The MCTruth strategies will be applied (in SimAtlasKernel
        SimCtbKernel or in your custom simulation entity) for those
        detectors which are on. The avalilables strategies are:

           StrategyIDET1      =  Brems strategy for IDET
           StrategyIDET2      =  Conversion strategy for IDET
           StrategyIDET3      =  Decay strategy for IDET
           StrategyIDET4      =  Ionization strategy for IDET
           StrategyIDET5      =  HadronicInteraction strategy for IDET
           StrategyCALO       =  CaloCatchAll strategy for CALO
           StrategyMUON       =  MuonCatchAll strategy for MUON


        The parameters for the MCTruth strategies are common to all
        of them and can be obtained at the Athena prompt

        AtlasG4Eng.G4Eng.menu_MCTruth.list_Parameters()
         List of all MCTruth configuration parameters
        --->          BremsPrimaryMinEnergy            value= 100.0000
        --->        BremsSecondaryMinEnergy            value= 100.0000
        --->      CaloDecayPrimaryMinEnergy            value= 1000.0000
        --->    CaloDecaySecondaryMinEnergy            value= 500.0000
        --->    CaloMuBremsPrimaryMinEnergy            value= 1000.0000
        --->  CaloMuBremsSecondaryMinEnergy            value= 500.0000
        --->        ComptonPrimaryMinEnergy            value= 100.0000
        --->      ComptonSecondaryMinEnergy            value= 100.0000
        --->     ConversionPrimaryMinEnergy            value= 100.0000
        --->   ConversionSecondaryMinEnergy            value= 100.0000
        --->          DecayPrimaryMinEnergy            value= 100.0000
        --->        DecaySecondaryMinEnergy            value= 100.0000
        --->       HadronicPrimaryMinEnergy            value= 100.0000
        --->     HadronicSecondaryMinEnergy            value= 100.0000
        --->     IonizationPrimaryMinEnergy            value= 100.0000
        --->   IonizationSecondaryMinEnergy            value= 100.0000

    """

    class StrategyIDET1:
        """  Defines a Bremsstrahlung strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=MCTruthStrg('G4TruthStrategies', 'Bremsstrahlung','IDET::IDET',level)
            MCTruthMenu=G4Eng.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('BremsPrimaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('BremsSecondaryMinEnergy',100.)

    class StrategyIDET2:
        """  Defines a Conversion strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=MCTruthStrg('G4TruthStrategies', 'Conversion','IDET::IDET',level)
            MCTruthMenu=G4Eng.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('ConversionPrimaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('ConversionSecondaryMinEnergy',100.)

    class StrategyIDET3:
        """  Defines a Decay strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=MCTruthStrg('G4TruthStrategies', 'Decay','IDET::IDET',level)
            MCTruthMenu=G4Eng.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('DecayPrimaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('DecaySecondaryMinEnergy',100.)

    class StrategyIDET4:
        """  Defines a Ionization strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=MCTruthStrg('G4TruthStrategies', 'Ionization','IDET::IDET',level)
            MCTruthMenu=G4Eng.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('IonizationPrimaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('IonizationSecondaryMinEnergy',100.)

    class StrategyIDET5:
        """  Defines a  HadronicInteraction strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=MCTruthStrg('G4TruthStrategies', 'HadronicInteraction','IDET::IDET',level)
            MCTruthMenu=G4Eng.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('HadronicPrimaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('HadronicSecondaryMinEnergy',100.)

    class StrategyIDET6:
        """ Defines a Compton strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=MCTruthStrg('G4TruthStrategies', 'Compton','IDET::IDET',level)
            MCTruthMenu=G4Eng.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('ComptonPrimaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('ComptonSecondaryMinEnergy',100.)

    class StrategyCALO:
        """  Defines a CALOCatchAll strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=MCTruthStrg('G4TruthStrategies', 'CALOCatchAll','CALO::CALO',level)
            MCTruthMenu=G4Eng.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('CaloMuBremsPrimaryMinEnergy',500.)
            MCTruthMenu.set_TruthStrategiesParameter('CaloMuBremsSecondaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('CaloDecayPrimaryMinEnergy',1000.)
            MCTruthMenu.set_TruthStrategiesParameter('CaloDecaySecondaryMinEnergy',500.)

    class StrategyMUON:
        """  Defines a DMuonCatchAll strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=MCTruthStrg('G4TruthStrategies', 'DMuonCatchAll','MUON::MUON',level)

    class StrategyMUONQ02:
        """  Defines a DMuonCatchAll strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=MCTruthStrg('G4TruthStrategies', 'DMuonCatchAll','MUONQ02::MUONQ02',level)


class RecordingEnvelopes(object):
    """
    Defines the recording envelopes in use in the full ATLAS simulations

    The hits are recorded with the TrackRecorderSD in the
    TrackRecordCollection collection.
    """

    class AtlasCaloEntryLayer:
        """ Defines the recording envelope AtlasCaloEntryLayer

            Hits in TrackRecordCollection#CaloEntryLayer
            (by default it will be applied to level 2 but this is optional)
        """
        def __init__(self,level=2,allowMods=False):
            self.recenv=RecEnvelope('CaloEntryLayer','IDET::IDET',level,allowMods)

    class AtlasMuonEntryLayer:
        """ Defines the recording envelope MuonEntryLayer

            Hits in TrackRecordCollection#MuonEntryLayer
            (by default it will be applied to level 2 but this is optional)
        """
        def __init__(self,level=2,allowMods=False):
            self.recenv=RecEnvelope('MuonEntryLayer','CALO::CALO',level,allowMods)

    class AtlasMuonExitLayer:
        """ Defines the recording envelope MuonExitLayer

            Hits in TrackRecordCollection#MuonExitLayer
            (by default it will be applied to level 2 but this is optional)
        """
        def __init__(self,level=2,allowMods=False):
            self.recenv=RecEnvelope('MuonExitLayer','MUONQ02::MUONQ02',level,allowMods)


class RecEnvelope:
    """ Recording envelope.


    """

    def __init__(self, name, volume_name, volume_level,allowMods=False):
        self._Built = False
        self.Name = name 
        self.Dict_Volumes = dict()
        self.Dict_Volumes[volume_name] = volume_level
        self._AllowMods=allowMods
     
    def _construct(self):
        if not self._Built:
           for i in self.Dict_Volumes.keys():
               G4Eng._ctrl.mctruthMenu.addRecordingEnvelope(i, self.Dict_Volumes.get(i), self.Name)
               G4Eng._ctrl.sendetectorMenu.setAllowMods(self.Name,self._AllowMods)
     
               G4Eng.log.debug(' RecEnvelope:'+
                                             '_construct: '+self.Name+' and store at the exit of volume  '+i)
           self._Built = True 
           G4Eng._app_profiler('  _build RecEnvelope: '+self.Name)


class MCTruthStrg:
    """ MCTruth strategy.

        Different MCTruth strategies can be defined and applied to
       several volumes (typically to the DetFacilities that are the
       envelopes of the sub-detectors).

       The G4Eng.menu_MCTruth menu is the way to deal with
       the MCTruthStrg.
    """
    def __init__(self,G4TruthStrgLib,NameStrg,volume_name,volume_level):
        """
           volume_level = position of the volume in the G4 hierarchy.
        """
        self._Built=False
        self.Lib=G4TruthStrgLib
        self.Name=NameStrg
        self.Dict_Volumes = dict()
        self.Dict_Volumes[volume_name]=volume_level


    def add_Volumes(self,vol_name, vol_level):
        """ Adds volumes to the list of volumes in which the strategy
            will be used.

        """
        if vol_name not in self.Dict_Volumes:
            self.Dict_Volumes[vol_name] = vol_level
        else:
            G4Eng.log.warning(' MCTruthStrg: add_Volumes: The the volume '+vol_name+'is already in the list')


    def _construct(self):
        if self.Lib not in G4Eng.List_LoadedLib:
            G4Eng._ctrl.load(self.Lib)
            G4Eng.List_LoadedLib.append(self.Lib)
        if not(self._Built):
           for i in self.Dict_Volumes.keys():
               G4Eng._ctrl.mctruthMenu.activateStrategy(self.Name, i, self.Dict_Volumes.get(i))
               G4Eng.log.debug(' MCTruthStrg:'+
                                             '_construct: '+self.Name +' and apply it to volume '+i)
           self._Built=True
           G4Eng._app_profiler('  _build MCTruthStrg: '+self.Name)
