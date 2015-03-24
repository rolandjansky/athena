#******************************************************************************
#
# jopOptions file for testing Cosmic in ATN and RTT framework.
#
#==============================================================================

# By default this jobOptions file is used for testing Cosmic in ATN.
# For testing Cosmic in RTT framework using this jobOptions file the following 
# parameter is needed:
# doRTT=True

# By default input data for tests are taken from castor.
# To take input data for tests from afs:
# RunFromLocal=True

#------------------------------------------------------------------------------
# Override usage of TileDCS since we're using sqlite
#------------------------------------------------------------------------------
if not 'TileUseDCS' in dir():
    TileUseDCS = False


if not 'doRTT' in dir():
    doRTT = False


if not 'RunNumber' in dir():
    RunNumber = 156463    


TilePhysRun = True
FileFilter = "."               # Default file filter is ".data"
TileCorrectAmplitude = True    # Do parabolic correction in OptATLAS
TileCorrectTime = True         # Apply time correction
TileNoiseFilter = 1            # Shimpei filter
OfcFromCOOL = False            # Calculated Optimal Filtering constants on the fly
PhaseFromCOOL = False          # Assume phase=0


doCaloTopoCluster = True


if not 'InputDirectory' in dir():
    if not 'RunFromLocal' in dir():
        RunFromLocal = False
        InputDirectory = "/eos/atlas/atlascerngroupdisk/det-tile/test"
    else:
        if RunFromLocal:
            InputDirectory = "/afs/cern.ch/user/t/tilecomm/w0/rawdata"
else:
    if InputDirectory.startswith("/afs/"):
        RunFromLocal = True

if not 'RunFromLocal' in dir():
    RunFromLocal = False

if doRTT:
    # Parameters for testing cosmic in RTT framework:
    if not 'Version' in dir():
        Version = "RTT"
    if not 'OutputDirectory' in dir():
        OutputDirectory = "."
    if not 'EvtMax' in dir():
        #EvtMax = 1000
        EvtMax = 741
    doTileNtuple = False
    
    if not 'doPerfPostProcessing' in dir():
        doPerfPostProcessing = False    
  
    
    #--------------------------------------------------------------------------
    # To read constants from Oracle.
    #--------------------------------------------------------------------------
    from AthenaCommon.AppMgr import theApp
    svcMgr = theApp.serviceMgr()

    from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    svcMgr+=DBReplicaSvc(UseCOOLSQLite=False)
    svcMgr+=DBReplicaSvc(UseGeomSQLite=False)

    # Just in case delete all references to sqlite200 in current directory in 
    # order to read constants from Oracle. Since references to 
    # sqlite200 are copied to current directory in RTT framework 
    # by default.
    import shutil
    import os.path

    if os.path.exists('sqlite200') and os.path.isdir('sqlite200'):
        shutil.rmtree('sqlite200')

    if os.path.exists('geomDB') and os.path.islink('geomDB'):
        os.unlink('geomDB')

    if os.path.exists('triggerDB') and os.path.islink('triggerDB'):
        os.unlink('triggerDB')
    

    #--------------------------------------------------------------------------
    # To run a job with performance monitoring data being gathered.
    # Since PerfMonDozer seems to be running for all RTT jobs not just 
    # those which include PerfMon and returns test result failure 
    # if PerfMon has not been run (at least now - 6.08.2010). 
    #--------------------------------------------------------------------------
    from PerfMonComps.PerfMonFlags import jobproperties
    jobproperties.PerfMonFlags.doFullMon = True
    jobproperties.PerfMonFlags.doPostProcessing = doPerfPostProcessing
    jobproperties.PerfMonFlags.doSemiDetailedMonitoring = True
    jobproperties.PerfMonFlags.doSemiDetailedMonitoringFullPrint = True
    


else:
    # Parameters for testing cosmic in ATN:
    if not 'Version' in dir():
        Version = "ATN"
    if not 'EvtMax' in dir():
        EvtMax = 101
    if not 'OutputDirectory' in dir():
        OutputDirectory = "/tmp/Reco-" + str(RunNumber) + "-" + Version
    


include("TileRecEx/jobOptions_TileCalibRec.py")



