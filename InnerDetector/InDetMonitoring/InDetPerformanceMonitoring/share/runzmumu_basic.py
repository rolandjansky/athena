###############################
# Rel 22
###############################
import os
import subprocess
from AthenaCommon.AthenaCommonFlags import jobproperties
#
print (' == runzmumu == START == TestArea = %s' %os.getenv("TestArea"))
print (' == runzmumu == Athena_VERSION = %s' %os.getenv("Athena_VERSION"))
###############################

# MC
MC_bool = True

# use IDAlignment dynamic folders for Run 2 data
useIDADynamicFolders = True

####
if (MC_bool): useIDADynamicFolders = False # dynamic folders must be disabled in MC

EvtMax = 10000 # -1 all events
SkipEvents = 0

PoolInput = ["/eos/user/m/martis/data/InputFileForGridJobs/data18_13TeV.00352436.physics_Main.merge.DAOD_ZMUMU.f938_m1831_f938_m1982._0027.1"]
if (MC_bool): 
    PoolInput = ["/eos/user/m/martis/data/InputFileForGridJobs/ZmumuMC16_AOD.18379878._000123.pool.root.1"]

print (' ========= runzmumu === config == start == ')
print (' == MC_bool =', MC_bool)
print (' == useIDADynamicFolders =', useIDADynamicFolders)
print (' == EvtMax = ',EvtMax)
print (' == input file: ', PoolInput) 
print (' ========= runzmumu === config == end == ')


## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AlgSequence import AthSequencer

job = AlgSequence()
seq = AthSequencer("AthFilterSeq")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
jobproperties.AthenaCommonFlags.FilesInput = PoolInput
jobproperties.AthenaCommonFlags.EvtMax = EvtMax
jobproperties.AthenaCommonFlags.SkipEvents = SkipEvents

from AthenaCommon.GlobalFlags import globalflags

#
# Rec flags
from RecExConfig.RecFlags import rec
rec.AutoConfiguration=['everything']
rec.doInDet.set_Value_and_Lock(True)
rec.doPerfMon.set_Value_and_Lock(False)

#
# use dynamic alignment ?
from InDetRecExample.InDetJobProperties import InDetFlags 
InDetFlags.useDynamicAlignFolders.set_Value_and_Lock(useIDADynamicFolders)

#
# main jobOption
include("RecExCommon/RecExCommon_topOptions.py")

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = False

#
#  Track Selection Tool
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
m_TrackSelectorTool_TightPrimary = InDet__InDetTrackSelectionTool(name     = "InDetTrackSelectorTightPrimary",
                                                                  CutLevel = "TightPrimary",
                                                                  minPt    = 400)
ToolSvc += m_TrackSelectorTool_TightPrimary
print (m_TrackSelectorTool_TightPrimary)

#### run zmumu validation
print ("  ========= runzmumu == including ElectronEoverPTracking.py for defining track fitters")
include("InDetPerformanceMonitoring/ElectronEoverPTracking.py") # imports ServiceMgr

# track to vertex association
print ("  ========= runzmumu == trakc to vertex association tool: defining working point ")
from TrackVertexAssociationTool.TrackVertexAssociationToolConf import CP__TrackVertexAssociationTool
t2vatool = CP__TrackVertexAssociationTool(name = "TrackVertexAssociationTool",
                                            WorkingPoint="SV_Reject")

#
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
ServiceMgr.THistSvc.Output += ["ZmumuValidationUserSel DATAFILE='ZmumuValidationUserSel.root' OPT='RECREATE'"]

from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonZmumu
iDPerfMonZmumu = IDPerfMonZmumu(name = 'IDPerfMonZmumu',
                                ReFitterTool1 = MuonRefitterTool,
                                ReFitterTool2 = MuonRefitterToolIDSiOnly,
                                doRefit = True, #True for evaluation of new alignment constants / False for update of weak mode recommendations -- refits tracks according to provided conditions db and/or external root file containing alignment constants
                                OutputTracksName =  "SelectedMuons",
                                isMC = MC_bool, # If set to True, the truth parameters will be filled, have a look into FillTruthParameters
                                TrackSelectionTool = m_TrackSelectorTool_TightPrimary,
                                UseTrackSelectionTool = True, # If set to True, it will use the specified TrackSelectionTool in the next Line
                                doIPextrToPV = False, # True for IP resolution studies, extrapolates IP parameters to primary vertex
                                TrackParticleName = 'CombinedTrackParticle', # InnerDetectorTrackParticles
                                IDTreeFolder = "/ZmumuValidationUserSel/id",
                                combTreeFolder = "/ZmumuValidationUserSel/comb", #always filled with information retrieved from TrackParticle associated to selected muon
                                MSTreeFolder = "/ZmumuValidationUserSel/ms",
                                refit1TreeFolder = "/ZmumuValidationUserSel/refit1",# Only filled if doRefit is set to True
                                refit2TreeFolder = "/ZmumuValidationUserSel/refit2",# Only filled if doRefit is set to True
                                truthTreeFolder = "/ZmumuValidationUserSel/truth", # Only filled if isMC is set to True
                                FourMuTreeFolder = "/ZmumuValidationUserSel/fourmu",# Only filled if doFourMuAnalysis is set to true
                                doIsoSelection = True,
                                doIPSelection = True,
                                doMCPSelection = True, # Medium 
                                doFourMuAnalysis = False,
                                StoreZmumuNtuple = True,
                                #loose selection to keep Z and JPsi events in the ntuple
                                MassWindowLow = 2., # in GeV
                                MassWindowHigh = 2000.,
                                PtLeadingMuon = 5., 
                                PtSecondMuon =  5., 
                                OpeningAngle = 0.01, # in radians. 1 radian ~60 deg
                                Z0Gap = 5.0, # in mm
                                OutputLevel = INFO)

job += iDPerfMonZmumu
print (iDPerfMonZmumu)

print (' ========= runzmumu === file == completed == ')
