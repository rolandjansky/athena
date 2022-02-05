# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

class CTPInputConfig:
    """
    Defines the cabling of the CTP input
    https://twiki.cern.ch/twiki/bin/view/Atlas/LevelOneCentralTriggerSetup#CTP_inputs
    """

    @staticmethod
    def cablingLayout():
        inputLayout = odict()
        inputLayout["optical"] = odict([
            ( "connector0", "MuCTPiOpt0" ),
            ( "connector1", "Topo1Opt0" ),
            ( "connector2", "Topo1Opt1" ),
            ( "connector3", "Topo1Opt2" ),
            ( "connector4", "Topo1Opt3" )
        ])
        inputLayout["electrical"] = odict([
            ( "connector0", "Topo3El" ),
            ( "connector1", "LegacyTopoMerged" ),
            ( "connector2", "Topo2El" ),  # Eitehr Topo2El or AlfaCtpin
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
