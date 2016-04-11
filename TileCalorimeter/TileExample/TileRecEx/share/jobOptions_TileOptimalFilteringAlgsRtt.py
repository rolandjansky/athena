#******************************************************************************
#
# Job options file for testing Optimal Filtering Algorithms
# (Fit, Opt2, OptATLAS) in RTT framework.
#
#==============================================================================


#------------------------------------------------------------------------------
#Override usage of TileDCS since we're using sqlite.
#------------------------------------------------------------------------------
if not 'TileUseDCS' in dir():
    TileUseDCS = False


if not 'Version' in dir():
    Version = "RTT"
if not 'EvtMax' in dir():
    EvtMax = 1000

ReadDigits = True

if not 'OutputDirectory' in dir():
    OutputDirectory = "."


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
        

if not 'doPerfPostProcessing' in dir():
    doPerfPostProcessing = False  


#------------------------------------------------------------------------------
# Optimal Filtering Agorithms (Fit, Opt2, OptATLAS)
# is switched on by default.
#------------------------------------------------------------------------------

# doTileOpt2 = True
# doTileOptATLAS = True
# doTileFit = True

#------------------------------------------------------------------------------
# Class to manage test configurations.
#------------------------------------------------------------------------------
class TestConfiguration:
    """Class to manage test configurations"""
    
    _configurations = ('TilePhysAllTestParamsOnOfaRtt',         \
                       'TilePhysAllTestParamsOffOfaRtt',        \
                       'TilePhysZeroBiasAllTestParamsOnOfaRtt', \
                       'TilePedestalAllTestParamsOnOfaRtt',     \
                       'TileLasAllTestParamsOnOfaRtt',          \
                       'TileCisAllTestParamsOnOfaRtt',          \
                       'TileMonoCisAllTestParamsOnOfaRtt')
    @classmethod 
    def Initialize(cls):
        """Switch off all test configuration apart of one given earlier."""
        for configuration in cls._configurations:
            if not configuration in globals():
                exec(configuration + ' = False', globals())
#------------------------------------------------------------------------------

TestConfiguration.Initialize()


#------------------------------------------------------------------------------
# Class to manage all tested parameters for Optimal Filtering Algorithms
#------------------------------------------------------------------------------
class TestParameters:
    """Parameters of Optimal Filtering Algorithms to be tested in RTT Framework.
    
    Parameters:
    OfcFromCOOL - take Optimal Filtering constants from COOL or calculate on fly;
    PhaseFromCOOL- take best phase from COOL or do not;
    TileCorrectTime - correct time or do not.
    
    """
    
    _parameters = ('OfcFromCOOL',         \
                   'PhaseFromCOOL',       \
                   'TileCorrectTime')
    @classmethod 
    def SetAll(cls, value):
        """Switch on or of all tested parameters"""
        for parameter in cls._parameters:
            exec(parameter + '=' + str(value), globals())
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Testing Optimal Filtering Algorithms for Physics run.
#------------------------------------------------------------------------------
if TilePhysAllTestParamsOnOfaRtt:	
    # Switch on Physcis run.
    TilePhysRun = True
    if not 'RunNumber' in dir():
        RunNumber = 162882  

    # Switch on all tested parameters.
    TestParameters.SetAll(True)
    # Convert ADC counts to MeV in output ntuple.
    TileCalibrateEnergy = True
    TileCorrectAmplitude = True
    EvtMax = 718

elif TilePhysAllTestParamsOffOfaRtt:
    # Switch on Physcis run.
    TilePhysRun = True
    if not 'RunNumber' in dir():
        RunNumber = 162882  
    
    # Switch off all tested parameters.
    TestParameters.SetAll(False)
    # Do not convert ADC counts to MeV in output ntuple.
    TileCalibrateEnergy = False
    # Switch off time correction
    from TileRecUtils.TileRecFlags import jobproperties
    jobproperties.TileRecFlags.correctTimeNI = False
    EvtMax = 718


elif TilePhysZeroBiasAllTestParamsOnOfaRtt:
    # Switch on Physcis run.
    TilePhysRun = True
    if not 'RunNumber' in dir():
        RunNumber = 201555

    FileFilter = "." 
    doTileOF1 = True
    doTileMF = True
    # Switch on all tested parameters.
    TestParameters.SetAll(True)
    # Convert ADC counts to MeV in output ntuple.
    TileCalibrateEnergy = True
    TileCorrectAmplitude = True
    EvtMax = 892


#------------------------------------------------------------------------------
# Testing Optimal Filtering Algorithms for Pedestal run.
#------------------------------------------------------------------------------
elif TilePedestalAllTestParamsOnOfaRtt:
    # Switch on Pedestal run.
    TilePedRun = True
    if not 'RunNumber' in dir():
        RunNumber = 164080  

    # Switch on all tested parameters.
    TestParameters.SetAll(True)
    TileOfflineUnits = 3 # MeV in output ntuple


#------------------------------------------------------------------------------
# Testing Optimal Filtering Algorithms for Laser run.
#------------------------------------------------------------------------------
elif TileLasAllTestParamsOnOfaRtt:
    TileLasRun = True
    if not 'RunNumber' in dir():
        RunNumber = 164088

    # Switch on all tested parameters.
    TestParameters.SetAll(True)
    TileOfflineUnits = 1 # pC in output ntuple


#------------------------------------------------------------------------------
# Testing Optimal Filtering Algorithms for Cis run.
#------------------------------------------------------------------------------
elif TileCisAllTestParamsOnOfaRtt:
    TileCisRun = True
    if not 'RunNumber' in dir():
        RunNumber = 164076

    # Switch on all tested parameters.
    TestParameters.SetAll(True)
    TileOfflineUnits = 0 # ADC counts in output ntuple   
    

#------------------------------------------------------------------------------
# Testing Optimal Filtering Algorithms for MonoCis run.
#------------------------------------------------------------------------------
elif TileMonoCisAllTestParamsOnOfaRtt:
    TileMonoRun = True
    if not 'RunNumber' in dir():
        RunNumber = 164075

    # Switch on all tested parameters.
    TestParameters.SetAll(True)
    TileOfflineUnits = 1 # pC in output ntuple   


#TileRec/h2000 ntuple is created by default.
#doTileNtuple=True

#------------------------------------------------------------------------------
# These parameters are switched on in TilePhysRun by default.
#------------------------------------------------------------------------------
doTileMuId = False
doTileRODMuId = False
doTileMuonFitter = False
doCaloNtuple = False


#------------------------------------------------------------------------------
# To read constants from Oracle.
#------------------------------------------------------------------------------
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
    

#------------------------------------------------------------------------------
# To run a job with performance monitoring data being gathered.
# Since PerfMonDozer seems to be running for all RTT jobs not just 
# those which include PerfMon and returns test result failure 
# if PerfMon has not been run (at least now - 6.08.2010). 
#------------------------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doFullMon = True
jobproperties.PerfMonFlags.doPostProcessing = doPerfPostProcessing
jobproperties.PerfMonFlags.doSemiDetailedMonitoring = True
jobproperties.PerfMonFlags.doSemiDetailedMonitoringFullPrint = True



include("TileRecEx/jobOptions_TileCalibRec.py")

topSequence.TileNtuple.SkipEvents = 0
