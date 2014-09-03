#******************************************************************************
#
# jopOptions file for testing TileCal commissioning analysis in ATN
#
#==============================================================================

# By default input data for tests are taken from castor.
# To take input data for tests from afs:
# RunFromLocal=True


#------------------------------------------------------------------------------
# Override usage of TileDCS since we're using sqlite
#------------------------------------------------------------------------------
if not 'TileUseDCS' in dir():
    TileUseDCS = False

if not 'Version' in dir():
    Version = "ATN"
if not "RunNumber" in dir():
    RunNumber = 164076

TileCisRun = True

if not 'EvtMin' in dir():
    EvtMin = 1079         # some high number number
if not 'EvtMax' in dir():
    EvtMax = 101          # 101 events

doTileMon=False


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

if not 'OutputDirectory' in dir():
    OutputDirectory = "/tmp/Reco-" + str(RunNumber) + "-" + Version

include("TileRecEx/jobOptions_TileCalibRec.py")


