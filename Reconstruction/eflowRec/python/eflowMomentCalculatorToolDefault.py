# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 
from AthenaCommon.SystemOfUnits import * #gives us access to deg
from AthenaCommon.Constants import * #gives us access to INFO

from eflowRec.eflowRecConf import eflowMomentCalculatorTool

class eflowMomentCalculatorToolDefault(eflowMomentCalculatorTool) :

    def __init__(self, name="eflowMomentCalculatorToolDefault"): 

        eflowMomentCalculatorTool.__init__( self, name )

        mlog = logging.getLogger( 'eflowMomentCalculatorToolDefault::__init__ ' )
        mlog.info("entering")

        try:
            from CaloRec.CaloRecConf import CaloClusterMomentsMaker
            ClusterMomentsMaker = CaloClusterMomentsMaker()
        except:
            mlog.error("could not import CaloRec.CaloClusterMomentsMaker")
            print traceback.format_exc()
            return False

        ClusterMomentsMaker.MaxAxisAngle = 30*deg
        ClusterMomentsMaker.OutputLevel = INFO
        ClusterMomentsMaker.MomentsNames = [
            "FIRST_PHI" 
            ,"FIRST_ETA"
            ,"SECOND_R" 
            ,"SECOND_LAMBDA"
            ,"DELTA_PHI"
            ,"DELTA_THETA"
            ,"DELTA_ALPHA" 
            ,"CENTER_X"
            ,"CENTER_Y"
            ,"CENTER_Z"
            ,"CENTER_MAG"
            ,"CENTER_LAMBDA"
            ,"LATERAL"
            ,"LONGITUDINAL"
            ,"FIRST_ENG_DENS" 
            ,"ENG_FRAC_EM" 
            ,"ENG_FRAC_MAX" 
            ,"ENG_FRAC_CORE" 
            ,"FIRST_ENG_DENS" 
            ,"SECOND_ENG_DENS"
            ,"ISOLATION"
            ,"EM_PROBABILITY"
            ,"ENG_POS"
            ,"ENG_BAD_CELLS"
            ,"N_BAD_CELLS"
            ,"BADLARQ_FRAC"
            ,"AVG_LAR_Q"
            ,"AVG_TILE_Q"
            ,"SIGNIFICANCE"
            ]


        self.CaloClusterMomentsMaker=ClusterMomentsMaker

        from eflowRec.eflowMomentCalculatorSetup import setup_eflowMomentCalculatorTool
        return setup_eflowMomentCalculatorTool(self, "LC", mlog)
