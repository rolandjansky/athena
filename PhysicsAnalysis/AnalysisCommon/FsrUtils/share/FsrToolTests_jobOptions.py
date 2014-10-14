# $Id: FsrToollTests_jobOptions.py $ 
# Set up the file reading in 19.1.2 (asetup 19.1.2,here):
FNAME = "/afs/cern.ch/user/t/tcuhadar/public/xAODTestSamples/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2657_s1933_s1964_r5589_tid01494017_00/AOD.01494017._000038.pool.root.1"


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

athenaCommonFlags.FilesInput = [ FNAME ]
athenaCommonFlags.EvtMax = 10
athenaCommonFlags.SkipEvents = 0

# Turn off TAG building:
from RecExConfig.RecFlags import rec
rec.doWriteTAG.set_Value_and_Lock( False )

# Set up the job using RecExCommon:
include( "RecExCommon/RecExCommon_topOptions.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from FsrUtils.FsrUtilsConf import FSR__ToolTester
alg = FSR__ToolTester()
alg.FsrPhotonTool.far_fsr_drcut = 0.12
alg.OutputLevel = DEBUG
theJob += alg
