# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.JobProperties import jobproperties
import traceback

class PFLocalHadCal:

    def PFCaloClusterLocalCalib(self,name):

        mlog = logging.getLogger( 'PFLocalHadCal::PFCaloClusterLocalCalib' )

        try:
            from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
            Calib = CaloClusterLocalCalib(name+"_CaloClusterLocalCalib")
        except Exception:
            mlog.error("could not import CaloClusterCorrection.CaloClusterLocalCalib")
            traceback.print_exc()
            return False
        
        try:
            from CaloUtils.CaloUtilsConf import CaloLCClassificationTool
            LCClassify   = CaloLCClassificationTool(name+"_CaloLCClassificationTool")
        except Exception:
            mlog.error("could not import CaloUtils.EMFracClusterClassificationTool")
            traceback.print_exc()
            return False

        LCClassify.ClassificationKey   = "EMFracClassify"
        LCClassify.UseSpread = False
        LCClassify.MaxProbability = 0.5
        LCClassify.StoreClassificationProbabilityInAOD = True
        
        currentDetDescrVersion = jobproperties.Global.DetDescrVersion.get_Value()

        if currentDetDescrVersion.startswith("Rome"):
            LCClassify.MaxProbability = 0.85
            LCClassify.UseNormalizedEnergyDensity = False
        else:
            LCClassify.MaxProbability = 0.50
            LCClassify.UseNormalizedEnergyDensity = True

        Calib+=LCClassify
        
        Calib.ClusterClassificationTool = [LCClassify]
        #New Local Hadron Calibration uses [1,2] instead of [2]
        Calib.ClusterRecoStatus = [1,2]
        
        LCWeight = self.getLCWeightTool(name)

        Calib+=LCWeight

        Calib.LocalCalibTools = [ LCWeight ]

        return Calib

    def getLCWeightTool(self,name):

        mlog = logging.getLogger( 'PFLocalHadCal::getLCWeightTool' )

        try:
            from CaloUtils.CaloUtilsConf import CaloLCWeightTool
            LCWeight = CaloLCWeightTool(name+"_CaloLCWeightTool")
        except Exception:
            mlog.error("could not import CaloUtils.CaloLCWeightTool")
            traceback.print_exc()
            return False

            from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
            #For LCWeightsTool needs electronic noise
            CaloNoiseCondAlg(noisetype="electronicNoise")
                   
        LCWeight.CorrectionKey       = "H1ClusterCellWeights"
        LCWeight.SignalOverNoiseCut  = 2.0
        LCWeight.UseHadProbability   = True

        return LCWeight

    def PFCaloClusterLocalCalibDM(self,name):

        mlog = logging.getLogger( 'PFLocalHadCal::PFCaloClusterLocalCalibDM' )

        try:
            from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
            CalibDM = CaloClusterLocalCalib(name+"_CaloClusterLocalCalibDM")
        except Exception:
            mlog.error("could not import CaloClusterCorrection.CaloClusterLocalCalib")
            traceback.print_exc()
            return False

        CalibDM.ClusterRecoStatus = [1,2]

        DMTool = self.getDeadMaterialCorrectionTool(name)
        
        CalibDM+=DMTool
        CalibDM.LocalCalibTools = [ DMTool ]

        return CalibDM

    def getDeadMaterialCorrectionTool(self,name):

        mlog = logging.getLogger( 'PFLocalHadCal::getDeadMaterialCorrectionTool' )
        
        try:
            from CaloUtils.CaloUtilsConf import CaloLCDeadMaterialTool
            LCDeadMaterial = CaloLCDeadMaterialTool(name+"_CaloLCDeadMaterialTool")
        except Exception:
            mlog.error("could not import CaloUtils.CaloLCDeadMaterialTool")
            traceback.print_exc()
            return False

        LCDeadMaterial.HadDMCoeffKey       = "HadDMCoeff2"
        LCDeadMaterial.ClusterRecoStatus   = 0
        LCDeadMaterial.WeightModeDM        = 2
        LCDeadMaterial.UseHadProbability   = True

        return LCDeadMaterial
        
    def PFCaloClusterLocalCalibOO(self,name):

        mlog = logging.getLogger( 'PFLocalHadCal::PFCaloClusterLocalCalibOO' )

        try:
            from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
            CalibOO = CaloClusterLocalCalib(name+"_CaloClusterLocalCalibOO")
        except Exception:
            mlog.error("could not import CaloClusterCorrection.CaloClusterLocalCalib")
            traceback.print_exc()
            return False

        #New Local Hadron Calibration uses [1,2] instead of [2]
        CalibOO.ClusterRecoStatus = [1,2]

        OOTool = self.getOutOfClusterCorrectionTool(name)

        CalibOO+=OOTool
        CalibOO.LocalCalibTools = [ OOTool ]

        return CalibOO

    def PFCaloClusterLocalCalibOOPi0(self,name):

        mlog = logging.getLogger( 'PFLocalHadCal::PFCaloClusterLocalCalibOOPi0' )
        
        try:
            from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
            CalibOOPi0 = CaloClusterLocalCalib(name+"_CaloClusterLocalCalibOOPi0")
        except Exception:
            mlog.error("could not import CaloClusterCorrection.CaloClusterLocalCalib")
            traceback.print_exc()
            return False
        
        CalibOOPi0.ClusterRecoStatus = [1]
        
        OOPi0Tool = self.getOutOfClusterCorrectionPi0Tool(name)
        
        CalibOOPi0+=OOPi0Tool
        CalibOOPi0.LocalCalibTools = [ OOPi0Tool ]
        
        return CalibOOPi0

    def getOutOfClusterCorrectionTool(self,name):

        mlog = logging.getLogger( 'PFLocalHadCal::getOutOfClusterCorrectionTool' )

        try:
            from CaloUtils.CaloUtilsConf import CaloLCOutOfClusterTool
            OOCC = CaloLCOutOfClusterTool(name+"_CaloLCOutOfClusterTool")
        except Exception:
            mlog.error("Could not import CaloUtils.CaloLCOutOfClusterTool")
            traceback.print_exc()
            return False

        OOCC.CorrectionKey = "OOCCorrection"
        OOCC.UseEmProbability    = False
        OOCC.UseHadProbability   = True

        return OOCC

    def getOutOfClusterCorrectionPi0Tool(self,name):

        mlog = logging.getLogger( 'PFLocalHadCal::getOutOfClusterCorrectionPi0Tool' )

        try:
            from CaloUtils.CaloUtilsConf import CaloLCOutOfClusterTool
            OOCCPi0 = CaloLCOutOfClusterTool(name+"_CaloLCOutOfClusterPi0Tool")
        except Exception:
            mlog.error("Could not import CaloUtils.CaloLCOutOfClusterTool")
            traceback.print_exc()
            return False
        
        OOCCPi0.CorrectionKey = "OOCPi0Correction"
        OOCCPi0.UseEmProbability  = True
        OOCCPi0.UseHadProbability = False

        return OOCCPi0
                                                                                            
