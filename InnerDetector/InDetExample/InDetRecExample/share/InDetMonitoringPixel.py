# configure the pixel main monitoring tool
doAllHits          = True
doHitsOnTrack      = True

from PixelMonitoring.PixelMonitoringConf import PixelMainMon
from InDetRecExample.InDetKeys import InDetKeys                                                                                     

if doAllHits:
  doDCS = False
  if  (globalflags.DataSource() == 'data' and geoFlags.Run() == "RUN2" and conddb.dbdata == "CONDBR2"):
     doDCS = True 
  InDetPixelMainsMon=PixelMainMon(name                = "InDetPixelMonitoringAllHits",
                                  onTrack             = False,

                                  ##Flags for normal monitoring.  This will run over all hits/clusters/etc, not just ones on track    
                                  doOffline           = True,       #Histograms for offline (tier0) running                                             
                                  doOnline            = True if athenaCommonFlags.isOnline() else False,      #Histograms for online (athenaPT) running
                                  doHeavyIonMon       = InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring

                                  do2DMaps            = True,       #Turn on/off the sets of 2D module maps                                             
                                  doModules           = True if athenaCommonFlags.isOnline() else False, #Turn on/off the sets of 1744 module histograms (for dqmf)
                                  doLowOccupancy      = False,      #Turn on/off histograms with binning for cosmics/single beam                        
                                  doHighOccupancy     = True,       #Turn on/off histograms with binning for collisions                                 
                                  doDetails           = False,      #Turn on/off the set of histograms with detailed info for 4 modules                 
                                  doPixelOccupancy    = False,      #Turn on/off histograms displaying pixel occupancy.  VERY memory heavy!      
                                  doLumiBlock         = False if athenaCommonFlags.isOnline() else True,       #Turn on/off histograms stored for each lumi block
                                  
                                  doTiming            = True,       #Turn on/off histograms with BCID/timing information  
                                  doRDO               = True,       #Turn on/off histograms with RDO/Hit  information                                 
                                  doErrors            = True,       #Turn on/off histograms with ROD Error information                                 
                                  doSpacePoint        = True,       #Turn on/off histograms with Spacepoint information                                 
                                  doCluster           = True,       #Turn on/off histograms with Cluster information
                                  doTrack             = False,      #Turn on/off histograms with Track information                                 
                                  doStatus            = True,       #Turn on/off histograms with Module Status information
                                  doDCS 	      = doDCS, 
                                  RDOName             = InDetKeys.PixelRDOs(),                                                                                
                                  RODErrorName        = "pixel_error_summary",                                                                                
                                  SpacePointName      = InDetKeys.PixelSpacePoints(),                                                                         
                                  ClusterName         = InDetKeys.PixelClusters(),                                                                            
                                                                                                                                      
                                  DetailsMod1         = "D1A_B03_S2_M3",  #Give the 4 modules which you want to do detailed monitoring of                        
                                  DetailsMod2         = "L0_B05_S2_M1A",  #Use the normal name like D1A_B03_S2_M4 or                                             
                                  DetailsMod3         = "L1_B10_S1_M2C",  #L1_B10_S2_M2C and the code should be able to parse
                                  DetailsMod4         = "D2C_B01_S1_M6")
  
  InDetPixelMainsMon.TrackName = InDetKeys.PixelTracks() if InDetFlags.doTrackSegmentsPixel() else InDetKeys.Tracks() 
    
  ##Other parameters                                                                                                                  

  if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
    InDetPixelMainsMon.FilterTools.append(monFilledBunchFilterTool)
                                                                                                                                       
  ToolSvc += InDetPixelMainsMon                 
  if (InDetFlags.doPrintConfigurables()): 
    print InDetPixelMainsMon     
                                                                                                                                       
##########################################################################

if doHitsOnTrack:
  InDetPixelMainsMonOnTrack=PixelMainMon(name                = "InDetPixelMonitoringOnTrack",
                                         onTrack             = True,

                                         ##Flags for data container types                                                                                                    
                                         doOffline           = True,      #Histograms for offline (tier0) running                                          
                                         doOnline            = True if athenaCommonFlags.isOnline() else False,     #Histograms for online (athenaPT) running
                                         doHeavyIonMon       = InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring

                                         do2DMaps            = True ,     #Turn on/off the sets of 2D module maps                                          
                                         doModules           = True if athenaCommonFlags.isOnline() else False,     #Turn on/off the sets of 1744 module histograms (for dqmf)
                                         doLowOccupancy      = False,     #Turn on/off histograms with binning for cosmics/single beam                     
                                         doHighOccupancy     = True,      #Turn on/off histograms with binning for collisions                              
                                         doDetails           = False,     #Turn on/off the set of histograms with detailed info for 4 modules              
                                         doPixelOccupancy    = False,     #Turn on/off histograms displaying pixel occupancy.  VERY memory heavy!          
                                         doLumiBlock         = False if athenaCommonFlags.isOnline() else True,      #Turn on/off histograms stored for each lumi block
                                         doHoleSearch        = True,
                                         doTiming            = False,     #Turn on/off histograms with BCID/timing information                             
                                         doRDO               = False,     #Turn on/off histograms with RDO/Hit  information                                
                                         doErrors            = False,     #Turn on/off histograms with ROD Error information                               
                                         doSpacePoint        = True,      #Turn on/off histograms with Spacepoint information                              
                                         doCluster           = True,      #Turn on/off histograms with Cluster information
                                         doTrack             = True,      #Turn on/off histograms with Track information                                   
                                         doStatus            = False,     #Turn on/off histograms with Module Status information                           
                                         doDCS               = False,
                                        
                                         ##Names of storegate containers                                                                                                     
                                         RDOName             = InDetKeys.PixelRDOs(),                                                                         
                                         RODErrorName        = "pixel_error_summary",                                                                         
                                         SpacePointName      = InDetKeys.PixelSpacePoints(),                                                                  
                                         ClusterName         = InDetKeys.PixelClusters(),                                                                     
                                         
                                         DetailsMod1         = "D1A_B03_S2_M3",  #Give the 4 modules which you want to do detailed monitoring of                 
                                         DetailsMod2         = "L0_B05_S2_M1A",  #Use the normal name like D1A_B03_S2_M4 or                                      
                                         DetailsMod3         = "L1_B10_S1_M2C",  #L1_B10_S2_M2C and the code should be able to parse                             
                                         DetailsMod4         = "D2C_B01_S1_M6")                                                                   
  
  InDetPixelMainsMonOnTrack.TrackName      = InDetKeys.Tracks()

  ## Track Selection Criteria
  # InDetTrackSelectionToolPixelMon = InDet__InDetTrackSelectionTool(name = "InDetTrackSelectionToolPixelMon",
  #                                                                               UseTrkTrackTools = True,
  #                                                                               CutLevel = "TightPrimary",
  #                                                                               minPt = 5000,
  #                                                                               TrackSummaryTool    = InDetTrackSummaryTool,
  #                                                                               Extrapolator        = InDetExtrapolator)
  #ToolSvc += InDetTrackSelectionToolPixelMon
  #InDetPixelMainsMonOnTrack.TrackSelectionTool = InDetTrackSelectionToolPixelMon
  InDetPixelMainsMonOnTrack.TrackSelectionTool.UseTrkTrackTools = True
  InDetPixelMainsMonOnTrack.TrackSelectionTool.CutLevel = "TightPrimary"
  InDetPixelMainsMonOnTrack.TrackSelectionTool.maxNPixelHoles = 1
  InDetPixelMainsMonOnTrack.TrackSelectionTool.maxD0 = 2
  InDetPixelMainsMonOnTrack.TrackSelectionTool.maxZ0 = 150
  InDetPixelMainsMonOnTrack.TrackSelectionTool.TrackSummaryTool    = InDetTrackSummaryTool
  InDetPixelMainsMonOnTrack.TrackSelectionTool.Extrapolator        = InDetExtrapolator
  ##Other parameters                                                                                                                  

  if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
    InDetPixelMainsMonOnTrack.FilterTools.append(monFilledBunchFilterTool)
  
  ToolSvc += InDetPixelMainsMonOnTrack                
  if (InDetFlags.doPrintConfigurables()): 
    print InDetPixelMainsMonOnTrack            

##########################################################################

# configure the pixel mon manager and add main pixel monitoring tool
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
from AthenaMonitoring.DQMonFlags           import DQMonFlags

InDetPixMonMan = AthenaMonManager( "InDetPixelMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),
                                   ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                   ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),
                                   DataType            = DQMonFlags.monManDataType(),
                                   Environment         = DQMonFlags.monManEnvironment(),
                                   Run                 = DQMonFlags.monManRun(),
                                   LumiBlock           = DQMonFlags.monManLumiBlock())
if doAllHits:
  InDetPixMonMan.AthenaMonTools     += [ InDetPixelMainsMon ]
if doHitsOnTrack:
  InDetPixMonMan.AthenaMonTools     += [ InDetPixelMainsMonOnTrack ]

topSequence += InDetPixMonMan
if (InDetFlags.doPrintConfigurables()):
  print InDetPixMonMan

