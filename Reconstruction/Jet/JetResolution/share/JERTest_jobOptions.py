
#
# This job options file defines an Athena testing job for the dual-use JER tools
#

# Specify input file

# mc15 test file
import os
input_file = os.getenv('ASG_TEST_FILE_MC')
#input_file = '/afs/cern.ch/atlas/project/PAT/xAODs/r6594/valid3.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.AOD.e3099_s2578_r6594_tid05292824_00/AOD.05292824._000011.pool.root.1'

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [input_file]

# Access the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Configure the JER tools
from AthenaCommon.AppMgr import ToolSvc
from JetResolution.JetResolutionConf import JERTool, JERSmearingTool
ToolSvc += JERTool('JERTool', CollectionName="AntiKt4EMTopoJets",
                   OutputLevel=INFO)
ToolSvc += JERSmearingTool('JERSmearingTool', JERTool=ToolSvc.JERTool,
                           ApplyNominalSmearing=True, OutputLevel=DEBUG)

# Configure the test algorithm
from JetResolution.JetResolutionConf import JERTestAlg
alg = JERTestAlg('JERTestAlg', JERSmearingTool=ToolSvc.JERSmearingTool,
                 ApplySystematics=True, OutputLevel=DEBUG)

# Add to the alg sequence
theJob += alg

# Configure the job
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 5
