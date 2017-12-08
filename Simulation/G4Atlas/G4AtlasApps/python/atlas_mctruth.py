# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
This module contains the MCTruth strategies that will be applied to the IDET,
CALO and MUON envelopes at level 1 (by default but this may change depending on
the hierarchy of the volumes).
"""

__author__ = 'M. Gallas'

import PyG4Atlas, AtlasG4Eng


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
            self.strg=PyG4Atlas.MCTruthStrg('G4TruthStrategies', 'Bremsstrahlung','IDET::IDET',level)
            MCTruthMenu=PyG4Atlas.G4AtlasEngine.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('BremsPrimaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('BremsSecondaryMinEnergy',100.)

    class StrategyIDET2:
        """  Defines a Conversion strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=PyG4Atlas.MCTruthStrg('G4TruthStrategies', 'Conversion','IDET::IDET',level)
            MCTruthMenu=PyG4Atlas.G4AtlasEngine.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('ConversionPrimaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('ConversionSecondaryMinEnergy',100.)

    class StrategyIDET3:
        """  Defines a Decay strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=PyG4Atlas.MCTruthStrg('G4TruthStrategies', 'Decay','IDET::IDET',level)
            MCTruthMenu=PyG4Atlas.G4AtlasEngine.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('DecayPrimaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('DecaySecondaryMinEnergy',100.)

    class StrategyIDET4:
        """  Defines a Ionization strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=PyG4Atlas.MCTruthStrg('G4TruthStrategies', 'Ionization','IDET::IDET',level)
            MCTruthMenu=PyG4Atlas.G4AtlasEngine.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('IonizationPrimaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('IonizationSecondaryMinEnergy',100.)

    class StrategyIDET5:
        """  Defines a  HadronicInteraction strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=PyG4Atlas.MCTruthStrg('G4TruthStrategies', 'HadronicInteraction','IDET::IDET',level)
            MCTruthMenu=PyG4Atlas.G4AtlasEngine.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('HadronicPrimaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('HadronicSecondaryMinEnergy',100.)

    class StrategyIDET6:
        """ Defines a Compton strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=PyG4Atlas.MCTruthStrg('G4TruthStrategies', 'Compton','IDET::IDET',level)
            MCTruthMenu=PyG4Atlas.G4AtlasEngine.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('ComptonPrimaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('ComptonSecondaryMinEnergy',100.)

    class StrategyCALO:
        """  Defines a CALOCatchAll strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=PyG4Atlas.MCTruthStrg('G4TruthStrategies', 'CALOCatchAll','CALO::CALO',level)
            MCTruthMenu=PyG4Atlas.G4AtlasEngine.menu_MCTruth()
            MCTruthMenu.set_TruthStrategiesParameter('CaloMuBremsPrimaryMinEnergy',500.)
            MCTruthMenu.set_TruthStrategiesParameter('CaloMuBremsSecondaryMinEnergy',100.)
            MCTruthMenu.set_TruthStrategiesParameter('CaloDecayPrimaryMinEnergy',1000.)
            MCTruthMenu.set_TruthStrategiesParameter('CaloDecaySecondaryMinEnergy',500.)

    class StrategyMUON:
        """  Defines a DMuonCatchAll strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=PyG4Atlas.MCTruthStrg('G4TruthStrategies', 'DMuonCatchAll','MUON::MUON',level)

    class StrategyMUONQ02:
        """  Defines a DMuonCatchAll strategy and its parameters

           (by default it will be applied to level 1 but this is optional)
        """
        def __init__(self,level=1):
            self.strg=PyG4Atlas.MCTruthStrg('G4TruthStrategies', 'DMuonCatchAll','MUONQ02::MUONQ02',level)

