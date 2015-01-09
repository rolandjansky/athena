# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 

from eflowRec.eflowRecConf import eflowTauTool

class eflowTauToolDefault(eflowTauTool) :

    def __init__(self, name="eflowTauToolDefault"): 

        eflowTauTool.__init__( self, name )

        mlog = logging.getLogger( 'eflowTauToolDefault::__init__ ' )
        mlog.info("entering")
        
        try:
            from eflowRec.eflowRecConf import eflowTrackCaloExtensionTool 
            TrackCaloExtensionTool=eflowTrackCaloExtensionTool()
        except:
            mlog.error("could not import eflowRec.eflowTrackCaloExtensionTool")
            print traceback.format_exc()
            return False

        eflowTauTool.eflowExtrapolator = TrackCaloExtensionTool



        self.SubtractionSigmaCut = 2.0
        self.ConsistencySigmaCut = 1.0
        
        try:
            from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12
            CellEOverPTool = eflowCellEOverPTool_mc12()
        except:
            mlog.error("could not import eflowRec.eflowCellEOverPTool_mc12")
            print traceback.format_exc()
            return False

        self.eflowCellEOverPTool=CellEOverPTool
