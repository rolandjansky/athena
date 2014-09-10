#
## @file TrackD3PDMaker/share/VertexGroupD3PD_topOptions.py
## @brief Example of job options for producing Vertex Group D3PD (standalone, othewise use Reco_trf directly)
## @author Simone Pagan Griso
## @date Mar, 2012
##

"""
Example of job options for producing Vertex Group D3PD (standalone, othewise use Reco_trf directly)
"""


## Set input and configration
readESD=False
readAOD= not readESD
InputPath='/eliza18/atlas/spagan/VtxStudies/VtxPUTF/AOD/'
NEvtProcess=100

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [InputPath+'mc11_valid.105200.T1_McAtNlo_Jimmy.recon.AOD.e835_s1256_s1258_r2529_tid464329_00/AOD.464329._000135.pool.root.1']

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())
#cfg = apcl.AutoCfg(cfg='everything', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

theApp.EvtMax  = NEvtProcess

## Set specific options for D3PD production
from TrackD3PDMaker.VertexD3PDAnalysisFlags import VertexD3PDAnalysisFlags
VertexD3PDAnalysisFlags.useAllVertexCollections = True
VertexD3PDAnalysisFlags.useTrigger=True
VertexD3PDAnalysisFlags.useSecondaryVertex=True
VertexD3PDAnalysisFlags.useMET=True
VertexD3PDAnalysisFlags.useElectrons=True
VertexD3PDAnalysisFlags.useMuons=True
VertexD3PDAnalysisFlags.usePhotons=True
VertexD3PDAnalysisFlags.useJets=True
VertexD3PDAnalysisFlags.useTaus=True

## Set other InDetRecExample configurations
doWriteESD = False
doWriteAOD = False
OutputLevel     = INFO
doJiveXML       = False
doVP1           = False
doAuditors      = True
doEdmMonitor    = False
doNameAuditor   = False

from RecExConfig.RecFlags import rec
rec.Commissioning=True

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.disableInDetReco.set_Value_and_Lock(True) # Do not run ID reconstruction

from InDetRecExample.InDetKeys import InDetKeys
if InDetFlags.doVertexFinding() and readAOD:
  InDetKeys.Tracks = InDetKeys.TrackParticles()
  print "InDetKeys.Tracks = "+InDetKeys.Tracks()

if readESD:
  InDetKeys.UnslimmedTracks              = 'Tracks'
  InDetKeys.UnslimmedTracksTruth         = 'TrackTruthCollection'
if readAOD:
  InDetKeys.McEventCollection = 'GEN_AOD'

TrackCollectionKeys        = [InDetKeys.Tracks()]
TrackCollectionTruthKeys   = [InDetKeys.TracksTruth()]


## Now include main job options for D3PD production (with IDRE)
include("TrackD3PDMaker/VertexGroupD3PD_jobOptions.py")
