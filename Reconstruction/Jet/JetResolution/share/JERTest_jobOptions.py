
#
# This job options file defines an Athena testing job for the dual-use JER tools
#

# Input file
input_file = '/afs/cern.ch/atlas/project/PAT/xAODs/r5591/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e1727_s1933_s1911_r5591/AOD.01494882._111853.pool.root.1'
#input_file = '/afs/cern.ch/atlas/project/PAT/xAODs/r5597/data12_8TeV.00204158.physics_JetTauEtmiss.recon.AOD.r5597/AOD.01495682._003054.pool.root.1'
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [input_file]

# Access the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Configure the JER tools
from AthenaCommon.AppMgr import ToolSvc
from JetResolution.JetResolutionConf import JERTool, JERSmearingTool
ToolSvc += JERTool('JERTool', CollectionName="AntiKt4LCTopoJets",
                   OutputLevel=DEBUG)
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
#theApp.EvtMax = 50

