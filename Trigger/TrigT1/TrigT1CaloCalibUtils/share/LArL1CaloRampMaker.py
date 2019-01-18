################################################################################
# config
################################################################################

EvtMax = 2500
SkipEvents = 0
doLAr = True
doTile = False

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.BSRDOInput = ['/afs/cern.ch/work/j/juraj/public/2015/LAr/264948/data15_calib.00264948.calibration_L1CaloEnergyScan.daq.RAW._lb0000._SFO-1._0003.data']
FilesInput = athenaCommonFlags.BSRDOInput()


################################################################################
# actual job options
################################################################################

# initial setup
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc,theApp,ServiceMgr
topSequence = AlgSequence()

# add EventInfo to xAOD::EventInfo conversion
topSequence += CfgMgr.xAODMaker__EventInfoCnvAlg()

# setup athenaCommonFlags
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.EvtMax = EvtMax
athenaCommonFlags.SkipEvents = SkipEvents
athenaCommonFlags.FilesInput = FilesInput
del SkipEvents
del EvtMax
del FilesInput

# setup globalflags
from AthenaCommon.GlobalFlags  import globalflags

import sys
sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/') # CERN
from AtlCoolBKLib import resolveAlias
conditionsTag = resolveAlias.getCurrent()
print conditionsTag
globalflags.ConditionsTag.set_Value_and_Lock(conditionsTag)

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.beamType.set_Value_and_Lock('cosmics')

from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys, GetRunNumber
ConfigureFromListOfKeys(['everything'])

# database tag
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(globalflags.ConditionsTag())

# setup detflags
import DetDescrCnvSvc.DetStoreConfig
from AthenaCommon.DetFlags import DetFlags
if doLAr: DetFlags.detdescr.LAr_setOn()
if doTile: DetFlags.detdescr.Tile_setOn()

# needed ....
#from RecExConfig.RecFlags import rec
rec.doLArg.set_Value_and_Lock(doLAr)
rec.doTile.set_Value_and_Lock(doTile)
rec.doCalo.set_Value_and_Lock(doLAr or doTile)
rec.Commissioning.set_Value_and_Lock(True)

# setup geometry
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

# setup bytestream reading
from ByteStreamCnvSvc import ReadByteStream
svcMgr.ByteStreamInputSvc.FullFileName = athenaCommonFlags.FilesInput()
theApp.EvtMax = athenaCommonFlags.EvtMax()
svcMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()
# Level-1 bs data
include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")

# detector description
include("CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")
include("TileIdCnv/TileIdCnv_jobOptions.py")
include("TileConditions/TileConditions_jobOptions.py")
        
# extra LAr setup
if doLAr:
    include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")
    include("LArIdCnv/LArIdCnv_joboptions.py")
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
    include("LArROD/LArFebErrorSummaryMaker_jobOptions.py")
    
    from LArConditionsCommon.LArCondFlags import larCondFlags
    larCondFlags.useLArFEBGainThresholds.set_Value_and_Lock(False)

    from LArROD.LArRODFlags import larRODFlags
    larRODFlags.keepDSPRaw = False

    print larRODFlags
    print larCondFlags

    # cell reconstruction properties
    from CaloRec.CaloCellFlags import jobproperties
    # reconstruct cells
    from CaloRec.CaloCellGetter import CaloCellGetter
    CaloCellGetter()

else:
    from TileRecUtils.TileRecFlags import jobproperties
    jobproperties.TileRecFlags.doTileOpt2=True
    jobproperties.TileRecFlags.readDigits=True
    jobproperties.TileRecFlags.noiseFilter=0
    jobproperties.TileRecFlags.TileRunType=8
    jobproperties.TileRecFlags.calibrateEnergy=False
    jobproperties.TileRecFlags.OfcFromCOOL=False
    jobproperties.TileRecFlags.BestPhaseFromCOOL=False
    jobproperties.TileRecFlags.correctTime=False
    jobproperties.TileRecFlags.correctAmplitude=False

    from TileConditions.TileCondToolConf import *
    tileInfoConfigurator.TileCondToolTiming = getTileCondToolTiming( 'COOL','CIS')

    tileCondToolOfcCool = getTileCondToolOfcCool('COOL', 'CISPULSE100')
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += tileCondToolOfcCool

    tileCondToolOfc = CfgMgr.TileCondToolOfc()
    tileCondToolOfc.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','CISPULSE100')

    include( "TileRec/TileRec_jobOptions.py" )
    include( "TileRec/TileCellMaker_jobOptions.py" )
    # turn off masking of bad channels
    ToolSvc.TileCellBuilder.maskBadChannels = False

# setup l1calo database
include('TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py')

# set up tools
ToolSvc += CfgMgr.LVL1__L1TriggerTowerTool()
ToolSvc += CfgMgr.LVL1__L1CaloLArTowerEnergy()

# add cell decoration
include('TrigT1CaloCalibTools/DecorateL1CaloTriggerTowers_prodJobOFragment.py')

# configure actual ramp maker algorithm
topSequence += CfgMgr.L1CaloRampMaker()
topSequence.L1CaloRampMaker.L1TriggerTowerTool = CfgMgr.LVL1__L1TriggerTowerTool()
topSequence.L1CaloRampMaker.DoTile = doTile
topSequence.L1CaloRampMaker.DoLAr = doLAr
topSequence.L1CaloRampMaker.EventsPerEnergyStep = 200
#topSequence.L1CaloRampMaker.NumberOfEnergySteps = 9
topSequence.L1CaloRampMaker.NumberOfEnergySteps = 11
topSequence.L1CaloRampMaker.IsGain1 = True
topSequence.L1CaloRampMaker.CheckProvenance = False
topSequence.L1CaloRampMaker.TileSaturationCut = 255.
# special region 1.3 < |eta| < 1.5, saturation on tile side.
topSequence.L1CaloRampMaker.SpecialChannelRange = { 0x6130f02 : 150, 0x7100003 : 150, 0x7180f03 : 150, 0x7180303 : 150, 0x7100200 : 150,
	0x6130601 : 150, 0x6130302 : 150, 0x61f0303 : 150, 0x71c0e00 : 150, 0x71c0a00 : 150, 0x7180501 : 150, 0x6130003 : 150, 0x7140d01 : 150,
	0x7140600 : 150, 0x7100d00 : 150, 0x6170900 : 150, 0x61b0901 : 150, 0x7180002 : 150, 0x7140c03 : 150, 0x6170901 : 150, 0x6130702 : 150,
	0x7180a00 : 150, 0x61b0403 : 150, 0x6130e00 : 150, 0x7180601 : 150, 0x61f0200 : 150, 0x6130002 : 150, 0x61b0601 : 150, 0x71c0e01 : 150,
	0x7100900 : 150, 0x7100901 : 150, 0x7100501 : 150, 0x6170100 : 150, 0x7140802 : 150, 0x7140003 : 150, 0x7140803 : 150, 0x7180c02 : 150,
	0x7100f02 : 150, 0x61b0b03 : 150, 0x6170302 : 150, 0x6170303 : 150, 0x7180703 : 150, 0x6170b02 : 150, 0x71c0402 : 150, 0x61f0803 : 150,
	0x6170b03 : 150, 0x6130101 : 150, 0x71c0601 : 150, 0x7140702 : 150, 0x61f0500 : 150, 0x71c0403 : 150, 0x71c0501 : 150, 0x7140e00 : 150,
	0x7140703 : 150, 0x7140402 : 150, 0x61f0501 : 150, 0x7140403 : 150, 0x61b0402 : 150, 0x7140e01 : 150, 0x6130703 : 150, 0x7180302 : 150,
	0x61b0a00 : 150, 0x61b0f02 : 150, 0x61b0f03 : 150, 0x7180100 : 150, 0x61b0303 : 150, 0x61f0e01 : 150, 0x71c0b03 : 150, 0x6130d00 : 150,
	0x7180101 : 150, 0x7100c03 : 150, 0x61b0a01 : 150, 0x6170802 : 150, 0x7100d01 : 150, 0x6130500 : 150, 0x7100403 : 150, 0x6130d01 : 150,
	0x7180702 : 150, 0x6170601 : 150, 0x61f0302 : 150, 0x71c0302 : 150, 0x61f0a01 : 150, 0x7180d00 : 150, 0x6130901 : 150, 0x7180d01 : 150,
	0x71c0303 : 150, 0x61f0901 : 150, 0x7140d00 : 150, 0x71c0a01 : 150, 0x7180c03 : 150, 0x6170c03 : 150, 0x6130201 : 150, 0x61b0702 : 150,
	0x71c0b02 : 150, 0x7100b02 : 150, 0x71c0600 : 150, 0x61f0600 : 150, 0x7140901 : 150, 0x61f0f02 : 150, 0x6170702 : 150, 0x6130803 : 150,
	0x6170403 : 150, 0x6170e00 : 150, 0x7180803 : 150, 0x6170703 : 150, 0x71c0c02 : 150, 0x7140f02 : 150, 0x71c0c03 : 150, 0x7100500 : 150,
	0x7140f03 : 150, 0x6130e01 : 150, 0x61b0b02 : 150, 0x6130c02 : 150, 0x6170101 : 150, 0x7100302 : 150, 0x61b0100 : 150, 0x7180003 : 150,
	0x7140501 : 150, 0x7100a00 : 150, 0x6130c03 : 150, 0x71c0900 : 150, 0x7100303 : 150, 0x6170002 : 150, 0x61b0101 : 150, 0x7180802 : 150,
	0x7100b03 : 150, 0x61f0402 : 150, 0x61f0403 : 150, 0x61f0f03 : 150, 0x7180e00 : 150, 0x7100a01 : 150, 0x7100201 : 150, 0x6130402 : 150,
	0x71c0101 : 150, 0x6170d01 : 150, 0x7140c02 : 150, 0x61f0a00 : 150, 0x6130403 : 150, 0x61b0c03 : 150, 0x6170d00 : 150, 0x71c0702 : 150,
	0x6130a01 : 150, 0x71c0d01 : 150, 0x6170c02 : 150, 0x61b0803 : 150, 0x7100600 : 150, 0x6170500 : 150, 0x61f0201 : 150, 0x6130600 : 150,
	0x61b0002 : 150, 0x7180900 : 150, 0x6170501 : 150, 0x7180901 : 150, 0x61b0003 : 150, 0x6130a00 : 150, 0x61f0900 : 150, 0x6170803 : 150,
	0x7140303 : 150, 0x7140100 : 150, 0x71c0200 : 150, 0x7180f02 : 150, 0x7140500 : 150, 0x71c0201 : 150, 0x6170003 : 150, 0x6130200 : 150,
	0x7140601 : 150, 0x6170e01 : 150, 0x61f0b02 : 150, 0x61f0b03 : 150, 0x71c0f02 : 150, 0x61b0e00 : 150, 0x61b0703 : 150, 0x71c0002 : 150,
	0x61b0e01 : 150, 0x7140a01 : 150, 0x6130b02 : 150, 0x71c0802 : 150, 0x7140b02 : 150, 0x71c0803 : 150, 0x7100100 : 150, 0x61f0100 : 150,
	0x61b0900 : 150, 0x7140b03 : 150, 0x71c0003 : 150, 0x6130f03 : 150, 0x7100101 : 150, 0x7140a00 : 150, 0x7140200 : 150, 0x7140201 : 150,
	0x61f0702 : 150, 0x7100802 : 150, 0x61b0d00 : 150, 0x61b0600 : 150, 0x61b0d01 : 150, 0x7180402 : 150, 0x61f0c02 : 150, 0x61f0c03 : 150,
	0x7100703 : 150, 0x61f0002 : 150, 0x6130900 : 150, 0x71c0703 : 150, 0x7180a01 : 150, 0x7180e01 : 150, 0x61f0601 : 150, 0x7140002 : 150,
	0x61f0802 : 150, 0x7100002 : 150, 0x7100c02 : 150, 0x7100f03 : 150, 0x61b0200 : 150, 0x6130100 : 150, 0x6170f02 : 150, 0x6170200 : 150,
	0x61b0201 : 150, 0x6170f03 : 150, 0x6170600 : 150, 0x6130501 : 150, 0x7140900 : 150, 0x61b0501 : 150, 0x71c0901 : 150, 0x7100702 : 150,
	0x61b0500 : 150, 0x7100803 : 150, 0x7180403 : 150, 0x61b0802 : 150, 0x71c0d00 : 150, 0x6130b03 : 150, 0x6130303 : 150, 0x6170201 : 150,
	0x7180600 : 150, 0x61f0003 : 150, 0x7100e01 : 150, 0x7180500 : 150, 0x71c0f03 : 150, 0x6170a00 : 150, 0x61b0c02 : 150, 0x61f0101 : 150,
	0x6170402 : 150, 0x7100402 : 150, 0x6130802 : 150, 0x7100e00 : 150, 0x7140302 : 150, 0x61f0e00 : 150, 0x7180b02 : 150, 0x7180b03 : 150,
	0x71c0500 : 150, 0x7140101 : 150, 0x6170a01 : 150, 0x7180200 : 150, 0x7180201 : 150, 0x61b0302 : 150, 0x61f0703 : 150, 0x71c0100 : 150,
	0x7100601 : 150, 0x61f0d00 : 150, 0x61f0d01 : 150,
	# saturating channels
	0x7120203 : 100
	#0x7120203 : 100, 0x6170c03 : 50, 0x6150b02 : 100, 0x6180d03 : 150, 0x61b0f02 : 100, 0x71d0d02 : 150, 
	#0x61c0a00 : 100, 0x6160f03 : 150, 0x6110901 : 150, 0x6140c02 : 150, 0x61a0e03 : 150, 0x61a0103 : 150,
	#0x61b0f00 : 150, 0x61b0f01 : 150 
}

# configure fitting algorithm
topSequence += CfgMgr.L1CaloLinearCalibration()

# configure writing of L1CaloRampData.pool.root
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
outputConditionsAlg = OutputConditionsAlg("outputConditionsAlg", "L1CaloRampData.pool.root")
outputConditionsAlg.ObjectList = ["L1CaloRampDataContainer"]
outputConditionsAlg.WriteIOV = False

# configure writing of calib database
EnergyScanResultOutput = OutputConditionsAlg("EnergyScanResultOutput", "dummy.root")
EnergyScanResultOutput.ObjectList = ["CondAttrListCollection#/TRIGGER/L1Calo/V1/Results/EnergyScanResults",
                                     "AthenaAttributeList#/TRIGGER/L1Calo/V1/Results/EnergyScanRunInfo"]
EnergyScanResultOutput.WriteIOV = True
EnergyScanResultOutput.Run1 = GetRunNumber()
svcMgr.IOVDbSvc.dbConnection="sqlite://;schema=energyscanresults.sqlite;dbname=L1CALO"

# configure writing of additional files for the calibration gui
topSequence += CfgMgr.L1CaloDumpRampData(RootStreamName="RAMPDATA")
#from TrigT1CaloCalibUtils.L1CaloDumpRampDataAlgorithm import L1CaloDumpRampDataAlgorithm
#topSequence += L1CaloDumpRampDataAlgorithm()

# run finetime
topSequence += CfgMgr.L1CaloPprMonitoring("L1CaloPprMonitoring",
                                          lumiBlockMax = 2,
                                          ppmADCMinValue = 80,
                                          ppmADCMaxValue = 963,
                                          doFineTimePlots = True,
                                          doPedestalPlots = False,
                                          doEtCorrelationPlots = False,
                                          doCaloQualCut = False
                                      )
ToolSvc += CfgMgr.LVL1__TrigT1CaloLWHistogramTool("TrigT1CaloLWHistogramTool", LVL1ConfigSvc = "")

ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='output.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["RAMPDATA DATAFILE='graphs.root' OPT='RECREATE'"]
svcMgr.IOVDbSvc.Folders += ["<dbConnection>sqlite://;schema=/afs/cern.ch/user/l/l1ccalib/w0/DaemonData/reference/calibReferences.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/References/FineTimeReferences"]

conddb.addFolderWithTag("TRIGGER", "/TRIGGER/L1Calo/V2/Calibration/Calib1/PprChanCalib", "HEAD")

# override timestamp with SOR time of run
# this overcomes strange timestamps stored in the EventInfo
# code taken from CoolConvUtilities/MagFieldUtils.py
from CoolConvUtilities.AtlCoolLib import indirectOpen
foldername_ = '/TDAQ/RunCtrl/SOR' if svcMgr.IOVDbSvc.DBInstance == 'CONDBR2' else '/TDAQ/RunCtrl/SOR_Params'
db_ = indirectOpen('COOLONL_TDAQ/%s' % svcMgr.IOVDbSvc.DBInstance, oracle=True)
if not db_: raise RuntimeError("Couldn't open connection to TDAQ DB")
folder_ = db_.getFolder(foldername_)
obj_ = folder_.findObject(GetRunNumber() << 32, 0)
payload_ = obj_.payload() 
svcMgr.IOVDbSvc.forceTimestamp = payload_['SORTime'] / 1000000000L
