# $Id: InDetTrackSmearingTool_jobOptions.py 300492 2014-04-30 14:39:05Z krasznaa $

# Set up the file reading:
FNAME = "/afs/cern.ch/user/a/asgbase/patspace/xAODs/r6630/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.AOD.e3698_s2608_s2183_r6630_tid05352803_00/AOD.05352803._000242.pool.root.1"

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()


# Add the test algorithm:
from InDetTrackSystematicsTools.InDetTrackSystematicsToolsConf import InDet__InDetTrackSmearingToolTester
alg = InDet__InDetTrackSmearingToolTester()
alg.OutputLevel = DEBUG
alg.SystematicEffects += ["TRK_RES_D0_MEAS"]
theJob += alg

# the default file is set from within the tool now
# from PathResolver import PathResolver
# PathResolver.FindCalibFile("...")
#alg.InDetTrackSmearingTool.SmearingFile = "IP_smearing-00.root"

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
 

# Define your output file name and stream name
rootStreamName = "MyFirstHistoStream"
rootFileName   = "InDetTrackPartHist.root"
alg.RootStreamName   = rootStreamName
alg.RootDirName      = "/MyHists"

# ====================================================================
# Define your output root file using MultipleStreamManager
# ====================================================================
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
MyFirstHistoXAODStream = MSMgr.NewRootStream( rootStreamName, rootFileName )

