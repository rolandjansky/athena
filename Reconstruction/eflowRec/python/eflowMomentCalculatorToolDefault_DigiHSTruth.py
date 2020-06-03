# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 
from AthenaCommon.SystemOfUnits import deg
from AthenaCommon.Constants import INFO

from eflowRec.eflowRecConf import eflowMomentCalculatorTool

class eflowMomentCalculatorToolDefault_DigiHSTruth(eflowMomentCalculatorTool) :

    def __init__(self, name="eflowMomentCalculatorToolDefault_DigiHSTruth"): 

        eflowMomentCalculatorTool.__init__( self, name )

        mlog = logging.getLogger( 'eflowMomentCalculatorToolDefault_DigiHSTruth::__init__ ' )
        mlog.info("entering")

        try:
            from CaloRec.CaloRecConf import CaloClusterMomentsMaker_DigiHSTruth
            ClusterMomentsMaker = CaloClusterMomentsMaker_DigiHSTruth()
        except:
            mlog.error("could not import CaloRec.CaloClusterMomentsMaker_DigiHSTruth")
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
            "ENERGY_DigiHSTruth" 
            "PHI_DigiHSTruth" 
            "ETA_DigiHSTruth" 
            ,"FIRST_ETA_DigiHSTruth"
            ,"SECOND_R_DigiHSTruth" 
            ,"SECOND_LAMBDA_DigiHSTruth"
            ,"DELTA_PHI_DigiHSTruth"
            ,"DELTA_THETA_DigiHSTruth"
            ,"DELTA_ALPHA_DigiHSTruth" 
            ,"CENTER_X_DigiHSTruth"
            ,"CENTER_Y_DigiHSTruth"
            ,"CENTER_Z_DigiHSTruth"
            ,"CENTER_MAG_DigiHSTruth"
            ,"CENTER_LAMBDA_DigiHSTruth"
            ,"LATERAL_DigiHSTruth"
            ,"LONGITUDINAL_DigiHSTruth"
            ,"FIRST_ENG_DENS_DigiHSTruth" 
            ,"ENG_FRAC_EM_DigiHSTruth" 
            ,"ENG_FRAC_MAX_DigiHSTruth" 
            ,"ENG_FRAC_CORE_DigiHSTruth" 
            ,"FIRST_ENG_DENS_DigiHSTruth" 
            ,"SECOND_ENG_DENS_DigiHSTruth"
            ,"ISOLATION_DigiHSTruth"
            ,"EM_PROBABILITY_DigiHSTruth"
            ,"ENG_POS_DigiHSTruth"
            ,"ENG_BAD_CELLS_DigiHSTruth"
            ,"N_BAD_CELLS_DigiHSTruth"
            ,"BADLARQ_FRAC_DigiHSTruth"
            ,"AVG_LAR_Q_DigiHSTruth"
            ,"AVG_TILE_Q_DigiHSTruth"
            ,"SIGNIFICANCE_DigiHSTruth"
            ]


        self.CaloClusterMomentsMaker=ClusterMomentsMaker

        from eflowRec.eflowMomentCalculatorSetup import setup_eflowMomentCalculatorTool
        return setup_eflowMomentCalculatorTool(self, "LC", mlog)
