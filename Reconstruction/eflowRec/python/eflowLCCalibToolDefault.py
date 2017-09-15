# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 

from eflowRec.eflowRecConf import eflowLCCalibTool

class eflowLCCalibToolDefault(eflowLCCalibTool) :

    def __init__(self, name="eflowLCCalibToolDefault"): 

        eflowLCCalibTool.__init__( self, name )

        mlog = logging.getLogger( 'eflowLCCalibToolDefault::__init__ ' )
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
            from eflowRec.eflowRecConf import eflowClusterCollectionTool
            eflowClusterCollectionTool = eflowClusterCollectionTool("eflowClusterCollectionTool_LCCalib")
        except:
            mlog.error("could not import eflowRec. eflowClusterCollectionTool")
            print traceback.format_exc()
            return False

        self.eflowRecClusterCollectionTool = eflowClusterCollectionTool 
        self.UseLocalWeight = False
