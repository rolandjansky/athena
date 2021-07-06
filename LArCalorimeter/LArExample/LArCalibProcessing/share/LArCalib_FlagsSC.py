# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

include.block("LArCalibProcessing/LArCalib_FlagsSC.py")

from string import *

class LArCalib_Flags:
    
    globalFlagDB = "LARCALIB-RUN2-00"
    OutputDB     = "sqlite://;schema=myDB200.db;dbname=CONDBR2"
    InputDB      = OutputDB

    WritePoolFile = True
    WriteNtuple   = True
    WriteIOV      = True

    LArPedestalFolder            = "/LAR/ElecCalibOflSC/Pedestals/Pedestal"
    LArAutoCorrFolder            = "/LAR/ElecCalibOflSC/AutoCorrs/AutoCorr"
    LArPhysAutoCorrFolder        = "/LAR/ElecCalibOflSC/AutoCorrs/PhysicsAutoCorr"

    LArCaliWaveFolder            = "/LAR/ElecCalibOflSC/CaliWaves/CaliWave"
    LArCaliWaveFolderXtlk        = "/LAR/ElecCalibOflSC/CaliWaves/CaliWave"

    LArPhysWaveFolder            = "/LAR/ElecCalibOflSC/PhysWaves/RTM"
    LArMPhysOverMCalFolder       = "/LAR/ElecCalibOflSC/MphysOverMcal/RTM"

    LArPhysCaliTdiffFolder       = "/LAR/ElecCalibOflSC/PhysCaliTdiff"

    LArOFCPhysFolder             = "/LAR/ElecCalibOflSC/OFC/PhysWave/RTM/"
    LArOFCCaliFolder             = "/LAR/ElecCalibOflSC/OFC/CaliWave"
    LArOFCCaliFolderXtlk         = "/LAR/ElecCalibOflSC/OFC/CaliWave"
    LArOFCMasterWaveFolder       = "/LAR/ElecCalibOflSC/OFC/MasterWave"
    LArOFCMasterWaveFolderXtlk   = "/LAR/ElecCalibOflSC/OFC/MasterWave"
                               
    LArShapeFolder               = "/LAR/ElecCalibOflSC/Shape/RTM/"
    LArShapeCaliWaveFolder       = "/LAR/ElecCalibOflSC/Shape/CaliWave"
    LArShapeCaliWaveFolderXtlk   = "/LAR/ElecCalibOflSC/Shape/CaliWaveXtalkCorr"
    LArShapeMasterWaveFolder     = "/LAR/ElecCalibOflSC/Shape/MasterWave"
    LArShapeMasterWaveFolderXtlk = "/LAR/ElecCalibOflSC/Shape/MasterWaveXtalkCorr"   
                              
    LArRampFolder                = "/LAR/ElecCalibOflSC/Ramps/RampLinea"

    ## Parameters
    ## these parameters are not ready for all layer :
    #LArCaliPulseParamsFolder     = "/LAR/ElecCalibOflSC/CaliPulseParams/Measured"
    #LArDetCellParamsFolder       = "/LAR/ElecCalibOflSC/DetCellParams/Measured"

    ## extracted using RTM methode :
    LArCaliPulseParamsFolder     = "/LAR/ElecCalibOflSC/CaliPulseParams/RTM"
    LArDetCellParamsFolder       = "/LAR/ElecCalibOflSC/DetCellParams/RTM"  

    LArDTimeFolder               = "/LAR/ElecCalibOflSC/Tdrift/Computed"

    PhysWaveShiftFolder          = "/LAR/ElecCalibOflSC/OFCBin/PhysWaveShifts"
    OFCBinFolder                 = "/LAR/ElecCalibOflSC/OFCBin/Dummy"
    ShapeResidualsFolder         = "/LAR/ElecCalibOflSC/Shape/Residuals/5samples"
 
    IOVBegin=0
    IOVEnd=0
        

def LArCalibFolderTag(folder,tag):
    return "".join(folder.split('/')) + tag
    

class FolderTagResover:
    def __init__(self,dbname="COOLOFL_LAR/CONDBR2"):
        from PyCool import cool
        dbSvc = cool.DatabaseSvcFactory.databaseService()
        self._db = dbSvc.openDatabase(dbname)
        return

    def __del__(self):
        self._db.closeDatabase()
        return

    def getFolderTag(self,foldername,globalTag=LArCalib_Flags.globalFlagDB):
        try:
          folder=self._db.getFolder(foldername);
          return folder.resolveTag(globalTag)
        except:  
          # new folder, should "create a tag"
          return "".join(foldername.split('/')) + '-UPD3-00'
          
            
    def getFolderTagSuffix(self,foldername,globalTag=LArCalib_Flags.globalFlagDB):
        ft=self.getFolderTag(foldername,globalTag)
        p=ft.find("-")
        if p==-1:
            return "-Default"
        else: 
            return ft[p:]

