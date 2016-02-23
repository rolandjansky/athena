# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 
from AthenaCommon.SystemOfUnits import * #gives us access to deg
from AthenaCommon.Constants import * #gives us access to INFO

from eflowRec.eflowRecConf import eflowTauLCCalibTool

class eflowTauLCCalibToolDefault(eflowTauLCCalibTool) :

    def __init__(self, name="eflowTauLCCalibToolDefault"): 

        eflowTauLCCalibTool.__init__( self, name )

        mlog = logging.getLogger( 'eflowTauLCCalibToolDefault::__init__ ' )
        mlog.info("entering")

        try:
            from eflowRec.eflowLocalHadCal import eflowLocalHadCal
        except:
            mlog.error("could not import eflowRec.eflowLocalHadCal")
            print traceback.format_exc()
            return False

        LocalHadCal = eflowLocalHadCal()
        Calib = LocalHadCal.eflowCaloClusterLocalCalib(name)
        CalibOO = LocalHadCal.eflowCaloClusterLocalCalibOO(name)
        CalibOOPi0 = LocalHadCal.eflowCaloClusterLocalCalibOOPi0(name)
        CalibDM = LocalHadCal.eflowCaloClusterLocalCalibDM(name)

        from CaloRec.CaloTopoClusterFlags import jobproperties
        if not (jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib()):
            #load local hadron calibration database, if not done so by CaloRec already
            from CaloRec import CaloClusterTopoCoolFolder
                    
        self.CaloClusterLocalCalib=Calib
        self.CaloClusterLocalCalibOOCC=CalibOO
        self.CaloClusterLocalCalibOOCCPi0=CalibOOPi0
        self.CaloClusterLocalCalibDM=CalibDM

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
            ]
        
        #ClusterMomentsMaker.AODMomentsNames = [
        #    "LATERAL"
        #    ,"LONGITUDINAL"
        ##    ,"SECOND_R"
        #    ,"SECOND_LAMBDA"
        #    ,"CENTER_LAMBDA"
        #    ,"FIRST_ENG_DENS"
        #    ,"ENG_FRAC_MAX"
        #    ,"ISOLATION"]   

        self.CaloClusterMomentsMaker=ClusterMomentsMaker
