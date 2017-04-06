print "Custom Monitoring from InDetMontoringBon.py"
## set to true in case you would like to write out your own InDetAlignMon.root
InDetAlignMonDoOutput = False

## Set to True if you want to run the TruthComparison
InDetAlignMonDoTruth = False

# Setup vertexing sequence without beam constraint.
# NOTE: Include the following BEFORE adding InDetAlignMonManager to topSequence.
# include('InDetAlignmentMonitoring/InDetAlignmentMonitoring_vertexing.py')

# Setup Track Selection
from TRT_AlignAlgs.TRT_AlignAlgsConf import TRTTrackSelectionAlg
m_minPix = [3,2,-1000,-1000,-1000]
m_minSCT = [9,9,-1000,-1000,5]
m_minTRT = [45,45,45,45,10]
m_maxD0 = [ 500,100, 1000,100,1000]

m_name = ["SelectIDAlignMonTracksCombined"
          ,"SelectIDAlignMonTracksCombinedD0Cut"
          ,"SelectIDAlignMonTracksTRT"
          ,"SelectIDAlignMonTracksTRTD0Cut"
          ,"SelectIDAlignMonTracksEndcap"]

m_tracksName = ["IDAlignMonTracksCombined"
                ,"IDAlignMonTracksCombinedD0Cut"
                ,"IDAlignMonTracksTRT"
                ,"IDAlignMonTracksTRTD0Cut"
                ,"IDAlignMonTracksEndcap"]

m_inTracks = [InDetKeys.Tracks()
              ,InDetKeys.Tracks()
              ,InDetKeys.TRTTracks_CTB()
              ,InDetKeys.TRTTracks_CTB()
              ,InDetKeys.Tracks()]

m_outputLevel = [ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR]

SelectIDAlignMonTracksList = []

for i in range(5):
    SelectIDAlignMonTracksList.append( TRTTrackSelectionAlg( name = m_name[i],
                                                             inputTrackList = m_inTracks[i],
                                                             outputTrackList = m_tracksName[i],
                                                             SummaryTool = InDetTrackSummaryTool,
                                                             MaxChisqPerDof = 1000000,
                                                             PtMin = 2000,
                                                             MinPixelHits = m_minPix[i],
                                                             MinSCTHits = m_minSCT[i],
                                                             MinTRTHits = m_minTRT[i],
                                                             MinEndcapHits = 9999,
                                                             D0Max = m_maxD0[i],
                                                             UseCosmicTrackSelection = True,
                                                             MinEventPhase = 5,
                                                             MaxEventPhase = 30,
                                                             OneTrackPerEvent = True,
                                                             OutputLevel = m_outputLevel[i]) )
    
    # Settings for the endcap tracks
    if i == 4:
        SelectIDAlignMonTracksList[i].RequireEndcapHits = True
        SelectIDAlignMonTracksList[i].MinEndcapHits = 10
        SelectIDAlignMonTracksList[i].MinBarrelHits = 0
    
    if not jobproperties.BField.solenoidOn:
        SelectIDAlignMonTracksList[i].DoPtCut = False
        
    topSequence += SelectIDAlignMonTracksList[i]
    print          SelectIDAlignMonTracksList[i]



from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetCosmicTrackSelectorTool
InDetAlignCosmicTrackSelectorTool = InDet__InDetCosmicTrackSelectorTool(name = "InDetAlignCosmicTrackSelectorTool",
                                                                        maxZ0 = 100000.0, #overriding default
                                                                        maxD0 = 100000.0, #overriding default
                                                                        minPt = 0.0, #overriding default
                                                                        numberOfTRTHits = 0, #overriding default
                                                                        numberOfSiliconHits = 0, #overriding default
                                                                        numberOfSiliconHitsTop = -1, #default
                                                                        numberOfSiliconHitsBottom = -1, #default
                                                                        OutputLevel = ERROR,
                                                                        TrackSummaryTool = InDetTrackSummaryTool)
ToolSvc += InDetAlignCosmicTrackSelectorTool
if (InDetFlags.doPrintConfigurables()):
    print InDetAlignCosmicTrackSelectorTool

#all configuration of track selection cuts should be done using TrkSelectorTool above
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMon__TrackSelectionTool
InDetAlignMonTrackSelectionTool = InDetAlignMon__TrackSelectionTool(name = "InDetAlignMonTrackSelectionTool",
                                                                    PassAllTracks = True, ## Uncomment this line to bypass track slection
                                                                    OutputLevel = ERROR,
                                                                    TrackSelectorTool = InDetAlignCosmicTrackSelectorTool)


ToolSvc += InDetAlignMonTrackSelectionTool
if (InDetFlags.doPrintConfigurables()):
   print InDetAlignMonTrackSelectionTool
    
#================================
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonResiduals
InDetAlignMonResidualsList = []
m_resName = ["InDetAlignMonResidualsComb"
             ,"InDetAlignMonResidualsCombD0Cut"
             ,"InDetAlignMonResidualsTRT"
             ,"InDetAlignMonResidualsTRTD0Cut"
             ,"InDetAlignMonResidualsEndcap"]
             
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonEfficiencies
InDetAlignMonEfficienciesList = []
m_effName = ["InDetAlignMonEfficienciesComb"
             ,"InDetAlignMonEfficienciesCombD0Cut"
             ,"InDetAlignMonEfficienciesTRT"
             ,"InDetAlignMonEfficienciesTRTD0Cut"
             ,"InDetAlignMonEfficienciesEndcap"]

from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonGenericTracks
InDetAlignMonGenericTracksList = []
m_genTrkName = ["InDetAlignMonGenericTracksComb"
                ,"InDetAlignMonGenericTracksCombD0Cut"
                ,"InDetAlignMonGenericTracksTRT"
                ,"InDetAlignMonGenericTracksTRTD0Cut"
                ,"InDetAlignMonGenericTracksEndcap"]

from InDetTrackSplitterTool.InDetTrackSplitterToolConf import InDet__InDetTrackSplitterTool
InDetTrackSplitterToolList=[]
m_trkSplitterName = ["Combined","CombinedD0","TRT","TRTD0","Endcap"]

from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonTrackSegments
InDetAlignMonTrackSegmentsList = []
m_trkSegName = ["InDetAlignMonTrackSegmentsComb"
                ,"InDetAlignMonTrackSegmentsCombD0Cut"
                ,"InDetAlignMonTrackSegmentsTRT"
                ,"InDetAlignMonTrackSegmentsTRTD0Cut"
                ,"InDetAlignMonTrackSegmentsEndcap"]

m_deltaPhi = [0.005,0.005,0.01,0.05,0.005]
m_deltaPhi2D = [0.02,0.02,0.05,0.05,0.02]
m_deltaD0 = [0.8,0.8,8,16,8]
m_deltaD02D = [5,5,10,16,5]
m_d0Range = [200,200,800,200,200]
m_deltaQoverPt = [0.02,0.02,0.1,0.1,0.02]


for i in range(5):
    InDetAlignMonResidualsList.append(IDAlignMonResiduals (name = m_resName[i],
                                                           Pixel_Manager = InDetKeys.PixelManager(),
                                                           SCT_Manager = InDetKeys.SCT_Manager(),
                                                           tracksName = m_tracksName[i],
                                                           OutputLevel = m_outputLevel[i],
                                                           trackSelection = InDetAlignMonTrackSelectionTool,
                                                           TRT_Manager = InDetKeys.TRT_Manager())  )
    
    ToolSvc += InDetAlignMonResidualsList[i]
    if (InDetFlags.doPrintConfigurables()):
        print InDetAlignMonResidualsList[i]


    InDetAlignMonEfficienciesList.append(IDAlignMonEfficiencies (name = m_effName[i],
                                                                 HoleSearch = InDetExtendedHoleSearchTool,
                                                                 Pixel_Manager = InDetKeys.PixelManager(),
                                                                 SCT_Manager = InDetKeys.SCT_Manager(),
                                                                 tracksName = m_tracksName[i],
                                                                 trackSelection = InDetAlignMonTrackSelectionTool,
                                                                 TRT_Manager = InDetKeys.TRT_Manager())    )

    ToolSvc += InDetAlignMonEfficienciesList[i]
    if (InDetFlags.doPrintConfigurables()):
        print InDetAlignMonEfficienciesList[i]


    InDetAlignMonGenericTracksList.append(IDAlignMonGenericTracks (name = m_genTrkName[i],
                                                                   tracksName= m_tracksName[i],
                                                                   trackSelection = InDetAlignMonTrackSelectionTool,
                                                                   VxPrimContainerName = InDetKeys.PrimaryVertices())   )

    ToolSvc += InDetAlignMonGenericTracksList[i]
    if (InDetFlags.doPrintConfigurables()):
        print InDetAlignMonGenericTracksList[i]


    InDetTrackSplitterToolList.append(InDet__InDetTrackSplitterTool(name = "TrackSplitterTool_"+m_trkSplitterName[i],
                                                                    TrackFitter = InDetTrackFitter,
                                                                    OutputUpperTracksName = m_trkSplitterName[i]+"_Upper",
                                                                    OutputLowerTracksName = m_trkSplitterName[i]+"_Lower",
                                                                    OutputLevel = ERROR) )
                                           
    ToolSvc += InDetTrackSplitterToolList[i]
    if (InDetFlags.doPrintConfigurables()):
        print InDetTrackSplitterToolList[i]
        
    InDetAlignMonTrackSegmentsList.append(IDAlignMonTrackSegments ( name = m_trkSegName[i],
                                                                    InputTracksName = m_tracksName[i],
                                                                    UpperTracksName = m_trkSplitterName[i]+"_Upper",
                                                                    LowerTracksName = m_trkSplitterName[i]+"_Lower",
                                                                    UseCTBSplitTracks = False,
                                                                    TrackSplitter =  InDetTrackSplitterToolList[i],
                                                                    trackSelectionUp = InDetAlignMonTrackSelectionTool,
                                                                    trackSelectionLow = InDetAlignMonTrackSelectionTool,
                                                                    DeltaD0Range = m_deltaD0[i],
                                                                    DeltaD0Range2D = m_deltaD02D[i],
                                                                    DeltaPhiRange = m_deltaPhi[i],
                                                                    DeltaPhiRange2D = m_deltaPhi2D[i],
                                                                    DeltaQoverPtRange = m_deltaQoverPt[i],
                                                                    OutputLevel = ERROR,
                                                                    D0Range = m_d0Range[i] ))
    ToolSvc += InDetAlignMonTrackSegmentsList[i]
    if (InDetFlags.doPrintConfigurables()):
        print InDetAlignMonTrackSegmentsList[i]
#==============================================================

    

## add an AthenaMonManager algorithm to the list of algorithms to be ran
#from DataQualityTools.DQMonFlags import DQMonFlags
from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
InDetAlignMonManager = AthenaMonManager( name = "InDetAlignMonManager",
                                      FileKey = DQMonFlags.monManFileKey(),
                                      ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                      DataType            = DQMonFlags.monManDataType(),
                                      Environment         = DQMonFlags.monManEnvironment(),
                                      ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),
                                      Run                 = DQMonFlags.monManRun(),
                                      LumiBlock           = DQMonFlags.monManLumiBlock())
if jobproperties.Beam.beamType()=='cosmics':
    InDetAlignMonManager.DataType="cosmics"
    #InDetAlignMonManager.AthenaMonTools += [ IDAlignMonTrackSegments_UpLowAll ]
    #InDetAlignMonManager.AthenaMonTools += [ IDAlignMonTrackSegments_UpLowCombined ]
    #InDetAlignMonManager.AthenaMonTools += [ IDAlignMonTrackSegments_UpLowTRT ]

for i in range(5):
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResidualsList[i] ]
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonEfficienciesList[i] ]
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracksList[i] ]
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonTrackSegmentsList[i] ]

if InDetAlignMonDoTruth:
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonTruthComparison ]
if not hasattr(ToolSvc, 'monTrigDecTool'):
    print "InDetAlignmentMonitoring_InDetRec_jobOptions.py: trigger decision tool not found: don't run trigger-aware monitoring" 
elif jobproperties.Beam.beamType()=='cosmics' or jobproperties.Beam.beamType()=='singlebeam':
    print "singlebeam or cosmics: don't run trigger-aware monitoring"
else:
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonEfficiencies ]
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonBeamSpot ]
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSivsTRT ]

## Setup the output histogram file(s):
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
if InDetAlignMonDoOutput:
    THistSvc = Service( "THistSvc" )
    histOutput = "IDAlignMon DATAFILE='./IDAlignMon.root' OPT='RECREATE'"
    THistSvc.Output += [histOutput]
    InDetAlignMonManager.FileKey = "IDAlignMon"   


topSequence += InDetAlignMonManager
if (InDetFlags.doPrintConfigurables()):
    print InDetAlignMonManager
