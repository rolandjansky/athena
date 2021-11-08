# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from collections import namedtuple
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

from ..Base.TopoAlgos import EMMultiplicityAlgo, TauMultiplicityAlgo, JetMultiplicityAlgo, XEMultiplicityAlgo
from ..Base.TopoAlgorithms import AlgType, AlgCategory

class TopoAlgoDefMultiplicity(object):
    """
    Defines the TopoAlgorithms that calculate multiplicities for L1Calo thresholds
    The thresholds have to be explicitly defined here.
    """
    @staticmethod
    def registerTopoAlgos(tm):

        emThresholds_3bits = [
            'eEM3', 'eEM5', 'eEM8', 'eEM8L', 
        ]
        emThresholds_2bits = [
            'eEM8M', 'eEM10L',  'eEM15', 'eEM15L', 'eEM15M', 
            'eEM18M', 'eEM20', 'eEM20L', 'eEM22', 'eEM22L', 'eEM22M', 'eEM22T',

            # spares
            'eEMSPARE1', 'eEMSPARE2', 'eEMSPARE3', 'eEMSPARE4', 'eEMSPARE5', 'eEMSPARE6', 'eEMSPARE7', 'eEMSPARE8',
        ]

        for em in emThresholds_3bits:
            alg = EMMultiplicityAlgo( name = em,
                                      threshold = em,
                                      nbits = 3, classtype='eEmMultiplicity')
            tm.registerTopoAlgo(alg)

        for em in emThresholds_2bits:
            alg = EMMultiplicityAlgo( name = em,
                                      threshold = em,
                                      nbits = 2, classtype='eEmMultiplicity')
            tm.registerTopoAlgo(alg)

        emThresholds_3bits = [ 'jEM15',]
        emThresholds_2bits = [ 'jEM15M', 'jEM18M', ]
        for em in emThresholds_3bits:
            alg = EMMultiplicityAlgo( name = em,
                                      threshold = em,
                                      nbits = 3, classtype='jEmMultiplicity')
            tm.registerTopoAlgo(alg)

        for em in emThresholds_2bits:
            alg = EMMultiplicityAlgo( name = em,
                                      threshold = em, 
                                      nbits = 2, classtype='jEmMultiplicity')
            tm.registerTopoAlgo(alg)
                
        etauThresholds_3bits = [ 
            'eTAU8', 'eTAU12',    

            # spares
            'eTAUSPARE1', 'eTAUSPARE2',
        ]
        jtauThresholds_3bits = [ 'jTAU12', 'jTAU12M',]        
        ctauThresholds_3bits = [ 
            'cTAU12M', 

            #spares
            'cTAUSPARE1',
        ]
        etauThresholds_2bits = [ 
            'eTAU12L', 'eTAU12M', 'eTAU20', 'eTAU25', 'eTAU30HM', 'eTAU40', 'eTAU60', 'eTAU100', 
 
            #spares
            'eTAUSPARE3', 'eTAUSPARE4', 'eTAUSPARE5', 'eTAUSPARE6', 'eTAUSPARE7', 'eTAUSPARE8', 'eTAUSPARE9', 'eTAUSPARE10',
        ]
        jtauThresholds_2bits = [ 
            'jTAU20', 'jTAU25',

            #spares
            'jTAUSPARE1',
        ]
        ctauThresholds_2bits = [ 
            'cTAU20M', 'cTAU25M',  

            # spares
            'cTAUSPARE2',
        ]

        for tau in etauThresholds_3bits:
            alg = TauMultiplicityAlgo( name = tau,
                                       threshold = tau, 
                                       nbits = 3, classtype='eTauMultiplicity')
            tm.registerTopoAlgo(alg)

        for tau in jtauThresholds_3bits:
            alg = TauMultiplicityAlgo( name = tau,
                                       threshold = tau, 
                                       nbits = 3, classtype='jTauMultiplicity')
            tm.registerTopoAlgo(alg)

        for tau in ctauThresholds_3bits:
            alg = TauMultiplicityAlgo( name = tau,
                                       threshold = tau, 
                                       nbits = 3, classtype='cTauMultiplicity')
            tm.registerTopoAlgo(alg)

        for tau in etauThresholds_2bits:
            alg = TauMultiplicityAlgo( name = tau,
                                       threshold = tau, 
                                       nbits = 2, classtype='eTauMultiplicity')
            tm.registerTopoAlgo(alg)

        for tau in jtauThresholds_2bits:
            alg = TauMultiplicityAlgo( name = tau,
                                       threshold = tau, 
                                       nbits = 2, classtype='jTauMultiplicity')
            tm.registerTopoAlgo(alg)

        for tau in ctauThresholds_2bits:
            alg = TauMultiplicityAlgo( name = tau,
                                       threshold = tau, 
                                       nbits = 2, classtype='cTauMultiplicity')
            tm.registerTopoAlgo(alg)

        jJThresholds_3bits = [ 
            'jJ5', 'jJ12', 'jJ12p0ETA25', 'jJ15', 'jJ15p0ETA25', 'jJ20', 'jJ25', 'jJ25p0ETA23', 'jJ30',

            # spares
            'jJSPARE1', 'jJSPARE2',
        ]
        jJThresholds_2bits = [ 
            'jJ15p31ETA49', 'jJ20p31ETA49', 'jJ30p31ETA49', 'jJ35p0ETA23', 'jJ40', 'jJ40p0ETA25', 'jJ45p0ETA21',
            'jJ50', 'jJ50p31ETA49', 'jJ75', 'jJ75p31ETA49',
            'jJ85', 'jJ100', 'jJ120', 'jJ400',

            # spares
            'jJSPARE3', 'jJSPARE4', 'jJSPARE5',
        ]

        for jJet in jJThresholds_3bits:
            alg = JetMultiplicityAlgo( name = jJet,
                                       threshold = jJet,
                                       nbits = 3, classtype='jJetMultiplicity')
            tm.registerTopoAlgo(alg)

        for jJet in jJThresholds_2bits:
            alg = JetMultiplicityAlgo( name = jJet,
                                       threshold = jJet,
                                       nbits = 2, classtype='jJetMultiplicity')
            tm.registerTopoAlgo(alg)

        jLJThresholds_2bits = [ 
            'jLJ80', 'jLJ100', 'jLJ140', 'jLJ160', 

            #spares
            'jLJSPARE1', 'jLJSPARE2', 'jLJSPARE3', 'jLJSPARE4',
        ]

        for jLJet in jLJThresholds_2bits:
            alg = JetMultiplicityAlgo( name = jLJet, 
                                       threshold = jLJet,
                                       nbits = 2, classtype='jLJetMultiplicity')
            tm.registerTopoAlgo(alg)

        gJThresholds_3bits = [ 'gJ15', 'gJ25', 'gJ35',]
        gJThresholds_2bits = [ 'gJ50', 'gJ100',]
        gJThresholds_1bits = [ 'gJ160',]

        for gJet in gJThresholds_3bits:
            alg = JetMultiplicityAlgo( name = gJet,
                                       threshold = gJet,
                                       nbits = 3, classtype='gJetMultiplicity')
            tm.registerTopoAlgo(alg)

        for gJet in gJThresholds_2bits:
            alg = JetMultiplicityAlgo( name = gJet,
                                       threshold = gJet, 
                                       nbits = 2, classtype='gJetMultiplicity')
            tm.registerTopoAlgo(alg)

        for gJet in gJThresholds_1bits:
            alg = JetMultiplicityAlgo( name = gJet,
                                       threshold = gJet, 
                                       nbits = 1, classtype='gJetMultiplicity')
            tm.registerTopoAlgo(alg)

        gLJThresholds_2bits = [ 
            'gLJ80', 'gLJ100', 'gLJ140', 'gLJ160', 

            # spares
            'gLJSPARE1', 'gLJSPARE2', 'gLJSPARE3', 'gLJSPARE4',
        ]

        for gLJet in gLJThresholds_2bits:
            alg = JetMultiplicityAlgo( name = gLJet,
                                       threshold = gLJet, 
                                       nbits = 2, classtype='gLJetMultiplicity')
            tm.registerTopoAlgo(alg)

        XEThresholds = [ 
            'gXE30', 'gXE40', 'gXE50',
            'gXERHO30', 'gXERHO50', 
            'gXEPUFIT30', 'gXEPUFIT50',

            'jXE30', 'jXE40', 'jXE50', 'jXE55', 'jXE300',

            'jXEC50', 'jTE100', 'jTEC100', 'jTEFWD100', 'jTEFWDA100', 'jTEFWDC100', 
            'gTE50',

            'jXEPerf50',

            #spares (for any energy thresholds)
            'jXESPARE1', 'jXESPARE2', 'jXESPARE3', 'jXESPARE4',
            'jXESPARE5', 'jXESPARE6', 'jXESPARE7', 'jXESPARE8', 'jXESPARE9',
            'jXESPARE10', 'jXESPARE11', 'jXESPARE12', 'jXESPARE13', 'jXESPARE14',
            'jXESPARE15', 'jXESPARE16', 'jXESPARE17', 'jXESPARE18', 'jXESPARE19',
            'jXESPARE20', 'jXESPARE21', 'jXESPARE22', 'jXESPARE23', 'jXESPARE24',
            'jXESPARE25', 'jXESPARE26', 'jXESPARE27', 'jXESPARE28', 'jXESPARE29',

        ]

        for XE in XEThresholds:
            alg = XEMultiplicityAlgo( name = XE, 
                                      threshold = XE,
                                      nbits = 1)
            tm.registerTopoAlgo(alg)


    @staticmethod
    def checkMultAlgoFWconstraints(l1menu):
        """
        List of the constraints in terms of multiplicity algorithms to make sure the menu fits
        in the Topo1 FW
        """

        multLimits = namedtuple('ML', "thrtype, conn, nbit, startbit, endbit")
        multiplicities = [
           multLimits( thrtype='eEM',  conn='Topo1Opt0', nbit=3, startbit=0,  endbit=11),
           multLimits( thrtype='eEM',  conn='Topo1Opt0', nbit=2, startbit=12, endbit=51),
           multLimits( thrtype='eTAU', conn='Topo1Opt1', nbit=3, startbit=0,  endbit=11),
           multLimits( thrtype='eTAU', conn='Topo1Opt1', nbit=2, startbit=12, endbit=43),
           multLimits( thrtype='gLJ',  conn='Topo1Opt1', nbit=2, startbit=44, endbit=59),
           multLimits( thrtype='gJ',   conn='Topo1Opt1', nbit=3, startbit=60, endbit=68),
           multLimits( thrtype='gJ',   conn='Topo1Opt1', nbit=2, startbit=69, endbit=72),
           multLimits( thrtype='gJ',   conn='Topo1Opt1', nbit=1, startbit=73, endbit=73),

           multLimits( thrtype='jJ',   conn='Topo1Opt2', nbit=3, startbit=0,  endbit=32),
           multLimits( thrtype='jJ',   conn='Topo1Opt2', nbit=2, startbit=33, endbit=68),
           multLimits( thrtype='jLJ',  conn='Topo1Opt2', nbit=2, startbit=69, endbit=84),
           multLimits( thrtype='jTAU', conn='Topo1Opt3', nbit=3, startbit=0,  endbit=5 ),
           multLimits( thrtype='jTAU', conn='Topo1Opt3', nbit=2, startbit=6,  endbit=11),
           multLimits( thrtype='cTAU', conn='Topo1Opt3', nbit=3, startbit=12, endbit=17),
           multLimits( thrtype='cTAU', conn='Topo1Opt3', nbit=2, startbit=18, endbit=23),
           multLimits( thrtype='jEM',  conn='Topo1Opt3', nbit=3, startbit=24, endbit=26),
           multLimits( thrtype='jEM',  conn='Topo1Opt3', nbit=2, startbit=27, endbit=30),
           multLimits( thrtype='EN',   conn='Topo1Opt3', nbit=1, startbit=31, endbit=78),
        ]

        for conn in l1menu.connectors:
            if 'Topo1' not in conn.name or conn.legacy:
                continue
            for tl in conn.triggerLines:
                if 'Perf' in tl.name:
                    continue
                tl_name = 'Mult_'+tl.name
                algo = l1menu.topoAlgos.topoAlgos[AlgCategory.MULTI][AlgType.MULT][tl_name]
                goodAlgo = False
                for ml in multiplicities:
                    thrtype = algo.input
                    if 'XE' in algo.input or 'TE' in algo.input or 'MHT' in algo.input:
                        thrtype = 'EN'
                    if conn.name==ml.conn and thrtype==ml.thrtype and algo.nbits==ml.nbit and tl.startbit>=ml.startbit and (tl.startbit+tl.nbits-1)<=ml.endbit:
                        goodAlgo = True
                if not goodAlgo:
                    raise RuntimeError("The multiplicity algoprithm %s does not fit with Topo1 and CTP FW. If this is intended, please correct the multiplicity constraints and communicate the new menu to the L1TOPO and CTP groups." % algo.name)


