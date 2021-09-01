# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

from ..Base.TopoAlgos import EMMultiplicityAlgo, TauMultiplicityAlgo, JetMultiplicityAlgo, XEMultiplicityAlgo

class TopoAlgoDefMultiplicity(object):
    """
    Defines the TopoAlgorithms that calculate multiplicities for L1Calo thresholds
    The thresholds have to be explicitly defined here.
    """
    @staticmethod
    def registerTopoAlgos(tm):
        currentAlgoId = 0

        emThresholds_3bits = [
            'eEM3', 'eEM5', 'eEM8', 'eEM8L', 
        ]
        emThresholds_2bits = [
            'eEM8M', 'eEM10L',  'eEM15', 'eEM15L', 'eEM15M', 
            'eEM20', 'eEM20L', 'eEM20M', 'eEM22', 'eEM22L', 'eEM22M', 'eEM22T',
        ]

        for em in emThresholds_3bits:
            alg = EMMultiplicityAlgo( name = em,
                                      algoId = currentAlgoId,
                                      threshold = em,
                                      nbits = 3, classtype='EMMultiplicity')
            tm.registerTopoAlgo(alg)
        for em in emThresholds_2bits:
            alg = EMMultiplicityAlgo( name = em,
                                      algoId = currentAlgoId,
                                      threshold = em,
                                      nbits = 2, classtype='EMMultiplicity')
            tm.registerTopoAlgo(alg)
                
        etauThresholds_3bits = [ 'eTAU8', 'eTAU12', ]
        jtauThresholds_3bits = [ 'jTAU12', ]        
        ctauThresholds_3bits = [ 'cTAU12M' ]
        etauThresholds_2bits = [ 'eTAU12L', 'eTAU12M', 'eTAU20', 'eTAU25', 'eTAU30H', 'eTAU40', 'eTAU60', 'eTAU100' ]
        jtauThresholds_2bits = [ 'jTAU12M'  ]
        ctauThresholds_2bits = [ 'cTAU20M', 'cTAU25M',  ]

        for tau in etauThresholds_3bits:
            alg = TauMultiplicityAlgo( name = tau,
                                       algoId = currentAlgoId,
                                       threshold = tau,
                                       nbits = 3, classtype='eTauMultiplicity')
            tm.registerTopoAlgo(alg)
        for tau in jtauThresholds_3bits:
            alg = TauMultiplicityAlgo( name = tau,
                                       algoId = currentAlgoId,
                                       threshold = tau,
                                       nbits = 3, classtype='jTauMultiplicity')
            tm.registerTopoAlgo(alg)
        for tau in ctauThresholds_3bits:
            alg = TauMultiplicityAlgo( name = tau,
                                       algoId = currentAlgoId,
                                       threshold = tau,
                                       nbits = 3, classtype='cTauMultiplicity')
            tm.registerTopoAlgo(alg)
        for tau in etauThresholds_2bits:
            alg = TauMultiplicityAlgo( name = tau,
                                       algoId = currentAlgoId,
                                       threshold = tau,
                                       nbits = 2, classtype='eTauMultiplicity')
            tm.registerTopoAlgo(alg)
        for tau in jtauThresholds_2bits:
            alg = TauMultiplicityAlgo( name = tau,
                                       algoId = currentAlgoId,
                                       threshold = tau,
                                       nbits = 2, classtype='jTauMultiplicity')
            tm.registerTopoAlgo(alg)
        for tau in ctauThresholds_2bits:
            alg = TauMultiplicityAlgo( name = tau,
                                       algoId = currentAlgoId,
                                       threshold = tau,
                                       nbits = 2, classtype='cTauMultiplicity')
            tm.registerTopoAlgo(alg)


        jJThresholds_3bits = [ 'jJ12', 'jJ12p0ETA25', 'jJ15', 'jJ15p0ETA25', 'jJ20', 'jJ25', 'jJ25p0ETA23', 'jJ30',]
        jJThresholds_2bits = [ 'jJ15p31ETA49', 'jJ20p31ETA49', 'jJ30p31ETA49', 'jJ35p0ETA23', 'jJ40', 'jJ40p0ETA25', 'jJ45p0ETA20',
                               'jJ50', 'jJ50p31ETA49', 'jJ75', 'jJ75p31ETA49',
                               'jJ85', 'jJ100', 'jJ120', 'jJ400',]

        for jJet in jJThresholds_3bits:
            alg = JetMultiplicityAlgo( name = jJet,
                                       algoId = currentAlgoId,
                                       threshold = jJet,
                                       nbits = 3, classtype='jJetMultiplicity')
            tm.registerTopoAlgo(alg)
        for jJet in jJThresholds_2bits:
            alg = JetMultiplicityAlgo( name = jJet,
                                       algoId = currentAlgoId,
                                       threshold = jJet,
                                       nbits = 2, classtype='jJetMultiplicity')
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

