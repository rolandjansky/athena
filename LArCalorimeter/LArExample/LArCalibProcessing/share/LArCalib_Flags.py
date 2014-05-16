include.block("LArCalibProcessing/LArCalib_Flags.py")

from string import *

class LArCalib_Flags:
    
    #globalFlagDB = "COMCOND-ES1S-000-00"
    globalFlagDB = "LARCALIB-000-02"
    OutputDB     = "sqlite://;schema=myDB200.db;dbname=COMP200"
    InputDB      = OutputDB

    WritePoolFile = True
    WriteNtuple   = True
    WriteIOV      = True

    LArPedestalFolder            = "/LAR/ElecCalibOfl/Pedestals/Pedestal"
    LArAutoCorrFolder            = "/LAR/ElecCalibOfl/AutoCorrs/AutoCorr"
    LArPhysAutoCorrFolder        = "/LAR/ElecCalibOfl/AutoCorrs/PhysicsAutoCorr"

    LArCaliWaveFolder            = "/LAR/ElecCalibOfl/CaliWaves/CaliWave"
    LArCaliWaveFolderXtlk        = "/LAR/ElecCalibOfl/CaliWaves/CaliWaveXtalkCorr"

    LArPhysWaveFolder            = "/LAR/ElecCalibOfl/PhysWaves/RTM"
    LArMPhysOverMCalFolder       = "/LAR/ElecCalibOfl/MphysOverMcal/RTM"

    LArPhysCaliTdiffFolder       = "/LAR/ElecCalibOfl/PhysCaliTdiff"

    LArOFCPhysFolder             = "/LAR/ElecCalibOfl/OFC/PhysWave/RTM/"
    LArOFCCaliFolder             = "/LAR/ElecCalibOfl/OFC/CaliWave"
    LArOFCCaliFolderXtlk         = "/LAR/ElecCalibOfl/OFC/CaliWaveXtalkCorr"
    LArOFCMasterWaveFolder       = "/LAR/ElecCalibOfl/OFC/MasterWave"
    LArOFCMasterWaveFolderXtlk   = "/LAR/ElecCalibOfl/OFC/MasterWaveXtalkCorr"
                               
    LArShapeFolder               = "/LAR/ElecCalibOfl/Shape/RTM/"
    LArShapeCaliWaveFolder       = "/LAR/ElecCalibOfl/Shape/CaliWave"
    LArShapeCaliWaveFolderXtlk   = "/LAR/ElecCalibOfl/Shape/CaliWaveXtalkCorr"
    LArShapeMasterWaveFolder     = "/LAR/ElecCalibOfl/Shape/MasterWave"
    LArShapeMasterWaveFolderXtlk = "/LAR/ElecCalibOfl/Shape/MasterWaveXtalkCorr"   
                              
    #LArRampFolder               = "/LAR/ElecCalibOfl/Ramps/Ramp"
    LArRampFolder                = "/LAR/ElecCalibOfl/Ramps/RampLinea"

    ## Parameters
    ## these parameters are not ready for all layer :
    LArCaliPulseParamsFolder     = "/LAR/ElecCalibOfl/CaliPulseParams/Measured"
    LArDetCellParamsFolder       = "/LAR/ElecCalibOfl/DetCellParams/Measured"

    ## extracted using RTM methode :
    #LArCaliPulseParamsFolder     = "/LAR/ElecCalibOfl/CaliPulseParams/RTM"
    #LArDetCellParamsFolder       = "/LAR/ElecCalibOfl/DetCellParams/RTM"  

    LArDTimeFolder               = "/LAR/ElecCalibOfl/Tdrift/Computed"
 
    IOVBegin=0
    IOVEnd=0
        

def LArCalibFolderTag(folder,tag):
    return join(split(folder, '/'),'') + tag
    

class FolderTagResover:
    def __init__(self,dbname="COOLOFL_LAR/COMP200"):
        from PyCool import cool
        dbSvc = cool.DatabaseSvcFactory.databaseService()
        self._db = dbSvc.openDatabase("COOLOFL_LAR/COMP200")
        return

    def __del__(self):
        self._db.closeDatabase()
        return

    def getFolderTag(self,foldername,globalTag=LArCalib_Flags.globalFlagDB):
        folder=self._db.getFolder(foldername);
        return folder.resolveTag(globalTag)
            
    def getFolderTagSuffix(self,foldername,globalTag=LArCalib_Flags.globalFlagDB):
        ft=self.getFolderTag(foldername,globalTag)
        p=ft.find("-")
        if p==-1:
            return "-Default"
        else: 
            return ft[p:]

