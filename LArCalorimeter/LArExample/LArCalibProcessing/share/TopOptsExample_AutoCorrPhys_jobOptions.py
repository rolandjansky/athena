import commands

#input bytestream files

FullFileName = ["rfio:/castor/cern.ch/grid/atlas/DAQ/2009/00137909/express_express/data09_cos.00137909.express_express.daq.RAW._lb0000._SFO-1._0001.data"]


IOVBegin = 137909

RunNumberList = [137909]

GainList = ["FREE"]

NSamples = 5

WriteNtuple = True

# some hack to avoid trying to access low gain presampler calibration for AutoCorr
#  (not filled usually but we have a very old file not on disk anymore from a wrong upload long ago...)

from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.LArCoolChannelSelection.set_Value_and_Lock("3:588,653:1036")


include("LArCalibProcessing/LArCalib_AutoCorrPhys_jobOptions.py")

theApp.EvtMax=1000

from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += larCalibCatalogs

