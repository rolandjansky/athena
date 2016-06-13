#*****************************************************************
#
# """topOptions file for Tile Testbeam Reconstruciton and Monitoring in Athena""" 
# """This topOptions is intended to test the monitoring code"""
#=================================================================

from AthenaCommon.Logging import logging
log_tbntuple = logging.getLogger( 'jobOptions_TileTBNtuple.py' )

if not 'TestOnline' in dir():
    TestOnline = True

if not 'InputDirectory' in dir():
    InputDirectory = "/data/daq"

if not 'FileName' in dir():
    FileName = 'ALL'

if not 'RunFromLocal' in dir():
    RunFromLocal = True

if not 'RunNumber' in dir():
    RunNumber = 0

if not "histo_name" in dir():
    histo_name = "tilemon_%s.root" % RunNumber

include('TileMonitoring/jobOptions_TileTBMon.py')

topSequence += CfgMgr.TileTBAANtuple( TBperiod = 2015, NSamples = 7, TileRawChannelContainerFlat = "", TileRawChannelContainerOpt = "TileRawChannelOpt2" )

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
exec 'svcMgr.THistSvc.Output += [ "AANT DATAFILE=\'%(dir)s/tiletb_%(RunNum).f.root\' OPT=\'RECREATE\' " ] ' %  {'dir': OutputDirectory, 'RunNum': RunNumber }
svcMgr.THistSvc.MaxFileSize = 32768
