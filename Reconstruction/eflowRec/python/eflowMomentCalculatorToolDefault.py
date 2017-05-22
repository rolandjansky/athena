# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 
from AthenaCommon.SystemOfUnits import deg
from AthenaCommon.Constants import INFO

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

        from CaloRec.CaloTopoClusterFlags import jobproperties

        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool = CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += theCaloNoiseTool

        ClusterMomentsMaker.MaxAxisAngle = 20*deg
        ClusterMomentsMaker.WeightingOfNegClusters = jobproperties.CaloTopoClusterFlags.doTreatEnergyCutAsAbsolute() 
        ClusterMomentsMaker.MinBadLArQuality = 4000
        ClusterMomentsMaker.CaloNoiseTool = theCaloNoiseTool
        ClusterMomentsMaker.UsePileUpNoise = True
        ClusterMomentsMaker.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
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
