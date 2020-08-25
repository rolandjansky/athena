# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('Menu.L1.Config.TopoMultiplicityAlgoDef')

from ..Base.TopoAlgos import EMMultiplicityAlgo, TauMultiplicityAlgo, JetMultiplicityAlgo, XEMultiplicityAlgo

class TopoAlgoDefMultiplicity(object):
    """
    Defines the TopoAlgorithms that calculate multiplicities for L1Calo thresholds
    The thresholds have to be explicitly defined here.
    """
    @staticmethod
    def registerTopoAlgos(tm):
        currentAlgoId = 0

        emThresholds = [
            'eEM7', 'eEM8VH', 'eEM10VH', 'eEM15VH', 'eEM15VHI', 'eEM18VHI', 'eEM20VH', 
            'eEM20VHI', 'eEM22', 'eEM22VHI', 'eEM3', 'eEM8', 'eEM10', 'eEM15', 'eEM20',
        ]

        for em in emThresholds:
            alg = EMMultiplicityAlgo( name = em,
                                      algoId = currentAlgoId,
                                      threshold = em,
                                      nbits = 3)
            tm.registerTopoAlgo(alg)
                

        tauThresholds = [ "eTAU12", "eTAU12IM", "eTAU20", "eTAU20IM", "eTAU25IM", "eTAU40", "eTAU60", "eTAU100" ]

        for tau in tauThresholds:
            alg = TauMultiplicityAlgo( name = tau,
                                       algoId = currentAlgoId,
                                       threshold = tau,
                                       nbits = 3)
            tm.registerTopoAlgo(alg)

        jJThresholds = [ "jJ12", "jJ12.0ETA23", "jJ15", "jJ15.0ETA25", "jJ20", "jJ25", "jJ25.0ETA23", "jJ30", 
                         "jJ35.0ETA23", "jJ40.0ETA25", "jJ40", "jJ50", "jJ85", "jJ100", "jJ15.31ETA49", "jJ20.31ETA49", "jJ75.31ETA49" ]

        for jJet in jJThresholds:
            alg = JetMultiplicityAlgo( name = jJet,
                                       algoId = currentAlgoId,
                                       threshold = jJet,
                                       nbits = 3)
            tm.registerTopoAlgo(alg)


        gXEThresholds = [ "gXERHO20", "gXERHO30", "gXERHO35", "gXERHO40", "gXERHO45", "gXERHO50", "gXEPUFIT20", "gXEPUFIT50", "gXE50" ]

        for gXE in gXEThresholds:
            alg = XEMultiplicityAlgo( name = gXE,
                                      algoId = currentAlgoId,
                                      threshold = gXE,
                                      nbits = 1)
            tm.registerTopoAlgo(alg)


        jXEThresholds = [ "jXE50" ]

        for jXE in jXEThresholds:
            alg = XEMultiplicityAlgo( name = jXE,
                                      algoId = currentAlgoId,
                                      threshold = jXE,
                                      nbits = 1)
            tm.registerTopoAlgo(alg)

