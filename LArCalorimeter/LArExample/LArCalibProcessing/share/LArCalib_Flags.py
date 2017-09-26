include.block("LArCalibProcessing/LArCalib_Flags.py")

from string import *
from AthenaCommon.GlobalFlags import globalflags

class LArCalib_Flags:

    if globalflags.DatabaseInstance() == 'COMP200':
        globalFlagDB = "LARCALIB-000-02"
    else:
        globalFlagDB = "LARCALIB-RUN2-00"

    OutputDB     = "sqlite://;schema=myDB200.db;dbname=CONDBR2"
    InputDB      = OutputDB

    WritePoolFile = True
    WriteNtuple   = True
    WriteIOV      = True

    LArPedestalFolder            = "/LAR/ElecCalibOfl/Pedestals/Pedestal"
    LArAutoCorrFolder            = "/LAR/ElecCalibOfl/AutoCorrs/AutoCorr"
    LArPhysAutoCorrFolder        = "/LAR/ElecCalibOfl/AutoCorrs/PhysicsAutoCorr"

    LArCaliWaveFolder            = "/LAR/ElecCalibOfl/CaliWaves/CaliWave"
#    LArCaliWaveFolderXtlk        = "/LAR/ElecCalibOfl/CaliWaves/CaliWaveXtalkCorr"
    LArCaliWaveFolderXtlk        = "/LAR/ElecCalibOfl/CaliWaves/CaliWave"

    LArPhysWaveFolder            = "/LAR/ElecCalibOfl/PhysWaves/RTM"
    LArMPhysOverMCalFolder       = "/LAR/ElecCalibOfl/MphysOverMcal/RTM"

    LArPhysCaliTdiffFolder       = "/LAR/ElecCalibOfl/PhysCaliTdiff"

    LArOFCPhysFolder             = "/LAR/ElecCalibOfl/OFC/PhysWave/RTM/"
    LArOFCCaliFolder             = "/LAR/ElecCalibOfl/OFC/CaliWave"
#    LArOFCCaliFolderXtlk         = "/LAR/ElecCalibOfl/OFC/CaliWaveXtalkCorr"
    LArOFCCaliFolderXtlk         = "/LAR/ElecCalibOfl/OFC/CaliWave"
    LArOFCMasterWaveFolder       = "/LAR/ElecCalibOfl/OFC/MasterWave"
#    LArOFCMasterWaveFolderXtlk   = "/LAR/ElecCalibOfl/OFC/MasterWaveXtalkCorr"
    LArOFCMasterWaveFolderXtlk   = "/LAR/ElecCalibOfl/OFC/MasterWave"
                               
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

    PhysWaveShiftFolder          = "/LAR/ElecCalibOfl/OFCBin/PhysWaveShifts"
    OFCBinFolder                 = "/LAR/ElecCalibOfl/OFCBin/Dummy"
    ShapeResidualsFolder         = "/LAR/ElecCalibOfl/Shape/Residuals/5samples"
 
    IOVBegin=0
    IOVEnd=0
        

def LArCalibFolderTag(folder,tag):
    return join(split(folder, '/'),'') + tag
    

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
          return join(split(foldername, '/'),'') + '-UPD3-00'
          
            
    def getFolderTagSuffix(self,foldername,globalTag=LArCalib_Flags.globalFlagDB):
        ft=self.getFolderTag(foldername,globalTag)
        p=ft.find("-")
        if p==-1:
            return "-Default"
        else: 
            return ft[p:]

