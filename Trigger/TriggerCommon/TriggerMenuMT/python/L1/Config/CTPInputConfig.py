# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from AthenaCommon.Logging import logging
log = logging.getLogger('Menu.L1.Config.CTPInputConfig')

class CTPInputConfig:
    """
    Defines the cabling of the CTP input
    """

    @staticmethod
    def cablingLayout():
        inputLayout = odict()
        inputLayout["optical"] = odict([
            ( "connector0", "Topo1Opt0" ),
            ( "connector1", "Topo1Opt1" ),
            ( "connector2", "MuCTPiOpt0" )
        ])
        inputLayout["electrical"] = odict([
            ( "connector0", "Topo3El" ),
            ( "connector1", "LegacyTopo0" ),
            ( "connector2", "AlfaCtpin" )
        ])
        inputLayout["ctpin"] = odict()
        inputLayout["ctpin"]["slot7"] = odict([
            ( "connector0", "EM1" ),
            ( "connector1", "EM2" ),
            ( "connector2", "TAU1" ),
            ( "connector3", "TAU2" )
        ])
        inputLayout["ctpin"]["slot8"] = odict([
            ( "connector0", "JET1" ),
            ( "connector1", "JET2" ),
            ( "connector2", "EN1" ),
            ( "connector3", "EN2" )
        ])
        inputLayout["ctpin"]["slot9"] = odict([
            ( "connector0", "" ),
            ( "connector1", "CTPCAL" ),
            ( "connector2", "NIM1" ),
            ( "connector3", "NIM2" )
        ])
        return inputLayout
