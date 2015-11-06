# configure the pixel main monitoring tool
doAllHits          = True
# online: only run pixel tracks, offline: only combined tracks available
doHitsOnPixelTrack = True if athenaCommonFlags.isOnline() else False
doHitsOnTrack      = False if athenaCommonFlags.isOnline() else True

# switch on all modules histograms for all hits if pixel online monitoring
if (not 'doPixelOnlyMon' in dir()):
  doPixelOnlyMon = False
doAllHitsModules   = True if (athenaCommonFlags.isOnline() and doPixelOnlyMon) else False

from PixelMonitoring.PixelMonitoringConf import PixelMainMon
from InDetRecExample.InDetKeys import InDetKeys                                                                                     

if doAllHits:
  doDCS = False
  if  (globalflags.DataSource() == 'data' and geoFlags.Run() == "RUN2" and conddb.dbdata == "CONDBR2"):
     doDCS = True 
  InDetPixelMainsMon=PixelMainMon(name                = "InDetPixelMonitoringAllHits",
                                  onTrack             = False,
                                  onPixelTrack        = False,
                                  #TrkSummaryTool      = InDetTrackSummaryTool,
                                  ##Flags for normal monitoring.  This will run over all hits/clusters/etc, not just ones on track    
                                  doOffline           = True,       #Histograms for offline (tier0) running                                             
                                  doOnline            = True if athenaCommonFlags.isOnline() else False,      #Histograms for online (athenaPT) running
                                  doHeavyIonMon       = InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring

                                  do2DMaps            = True,       #Turn on/off the sets of 2D module maps                                             
                                  doModules           = doAllHitsModules, #Turn on/off the sets of 1744 module histograms (for dqmf)
                                  doFEChipSummary     = False,      #Turn on/off the 1744 module FE chip summary histograms (low memory version of doModules)
                                  doLowOccupancy      = True,       #Turn on/off histograms with binning for cosmics/single beam                        
                                  doHighOccupancy     = True,       #Turn on/off histograms with binning for collisions                                 
                                  doRodSim            = False,      #Turn on/off histograms specific for the rod simulator                              
                                  doDetails           = False,      #Turn on/off the set of histograms with detailed info for 4 modules                 
                                  doPixelOccupancy    = False,      #Turn on/off histograms displaying pixel occupancy.  VERY memory heavy!      
                                  doSpectrum          = False,      #Turn on/off histograms displaying pixel occupancy spectrum.  VERY memory heavy!    
                                  doNoiseMap          = False,      #Turn on/off histograms displaying pixel occupancy noise map.  VERY memory heavy!
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
                                  DetailsMod4         = "D2C_B01_S1_M6",  #this for you
                                  OccupancyCut        = 1e-5)
  
  InDetPixelMainsMon.TrackName = InDetKeys.PixelTracks() if InDetFlags.doTrackSegmentsPixel() else InDetKeys.Tracks() 
    
  ##Other parameters                                                                                                                  
  #PixelMainsMon.OfflineDoPixelOccupancy = False  #pixel occupancy plots for offline analysis.  Leave off except for private analysis 

  if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
    InDetPixelMainsMon.FilterTools.append(monFilledBunchFilterTool)
                                                                                                                                       
  ToolSvc += InDetPixelMainsMon                 
  if (InDetFlags.doPrintConfigurables()): 
    print InDetPixelMainsMon     
                                                                                                                                       
##########################################################################

if doHitsOnTrack:
  InDetPixelMainsMonOnTrack=PixelMainMon(name                = "InDetPixelMonitoringOnTrack",
                                         onTrack             = True,
                                         onPixelTrack        = False,
                                         #TrkSummaryTool      = InDetTrackSummaryTool,                                         
                                         ##Flags for data container types                                                                                                    
                                         doOffline           = True,      #Histograms for offline (tier0) running                                          
                                         doOnline            = True if athenaCommonFlags.isOnline() else False,     #Histograms for online (athenaPT) running                                        
                                         doHeavyIonMon       = InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring

                                         do2DMaps            = True ,     #Turn on/off the sets of 2D module maps                                          
                                         doModules           = False,     #Turn on/off the sets of 1744 module histograms (for dqmf)                       
                                         doFEChipSummary     = False,     #Turn on/off the 1744 module FE chip summary histograms (low memory version of doModules)
                                         doLowOccupancy      = True,      #Turn on/off histograms with binning for cosmics/single beam                     
                                         doHighOccupancy     = True,      #Turn on/off histograms with binning for collisions                              
                                         doRodSim            = False,     #Turn on/off histograms specific for the rod simulator                           
                                         doDetails           = False,     #Turn on/off the set of histograms with detailed info for 4 modules              
                                         doPixelOccupancy    = False,     #Turn on/off histograms displaying pixel occupancy.  VERY memory heavy!          
                                         doSpectrum          = False,     #Turn on/off histograms displaying pixel occupancy spectrum.  VERY memory heavy! 
                                         doNoiseMap          = False,     #Turn on/off histograms displaying pixel occupancy noise map.  VERY memory heavy!
                                         doLumiBlock         = False if athenaCommonFlags.isOnline() else True,      #Turn on/off histograms stored for each lumi block
                                         doHoleSearch        = True,
                                         doTiming            = False,     #Turn on/off histograms with BCID/timing information                             
                                         doRDO               = False,     #Turn on/off histograms with RDO/Hit  information                                
                                         doErrors            = False,     #Turn on/off histograms with ROD Error information                               
                                         doSpacePoint        = False,     #Turn on/off histograms with Spacepoint information                              
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
                                         DetailsMod4         = "D2C_B01_S1_M6",  #this for you                                                                   
                                         OccupancyCut        = 1e-5)                                                                           
  
  InDetPixelMainsMonOnTrack.TrackName      = InDetKeys.Tracks()

  ##Other parameters                                                                                                                  
  #PixelMainsMon.OfflineDoPixelOccupancy = False  #pixel occupancy plots for offline analysis.  Leave off except for private analysis 

  if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
    InDetPixelMainsMonOnTrack.FilterTools.append(monFilledBunchFilterTool)
  
  ToolSvc += InDetPixelMainsMonOnTrack                
  if (InDetFlags.doPrintConfigurables()): 
    print InDetPixelMainsMonOnTrack            

##########################################################################

if doHitsOnPixelTrack:
  InDetPixelMainsMonOnPixelTrack=PixelMainMon(name                = "InDetPixelMonitoringOnPixelTrack",
                                              onTrack             = False,
                                              onPixelTrack        = True,
                                              #TrkSummaryTool      = InDetTrackSummaryTool,                                              
                                              ##Flags for data container types
                                              doOffline           = True,      #Histograms for offline (tier0) running                                          
                                              doOnline            = True if athenaCommonFlags.isOnline() else False,     #Histograms for online (athenaPT) running                                        
                                              doHeavyIonMon       = InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring

                                              do2DMaps            = True ,     #Turn on/off the sets of 2D module maps                                          
                                              doModules           = False,     #Turn on/off the sets of 1744 module histograms (for dqmf)                       
                                              doFEChipSummary     = False,     #Turn on/off the 1744 module FE chip summary histograms (low memory version of doModules)
                                              doLowOccupancy      = True,      #Turn on/off histograms with binning for cosmics/single beam                     
                                              doHighOccupancy     = True,      #Turn on/off histograms with binning for collisions                              
                                              doRodSim            = False,     #Turn on/off histograms specific for the rod simulator                           
                                              doDetails           = False,     #Turn on/off the set of histograms with detailed info for 4 modules              
                                              doPixelOccupancy    = False,     #Turn on/off histograms displaying pixel occupancy.  VERY memory heavy!          
                                              doSpectrum          = False,     #Turn on/off histograms displaying pixel occupancy spectrum.  VERY memory heavy! 
                                              doNoiseMap          = False,     #Turn on/off histograms displaying pixel occupancy noise map.  VERY memory heavy!
                                              doLumiBlock         = False if athenaCommonFlags.isOnline() else True,      #Turn on/off histograms stored for each lumi block
                                              
                                              doTiming            = False,     #Turn on/off histograms with BCID/timing information                             
                                              doRDO               = False,     #Turn on/off histograms with RDO/Hit  information                                
                                              doErrors            = False,     #Turn on/off histograms with ROD Error information                               
                                              doSpacePoint        = False,     #Turn on/off histograms with Spacepoint information                              
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
                                              DetailsMod4         = "D2C_B01_S1_M6",  #this for you                                                                   
                                              OccupancyCut        = 1e-5)                                                                           
  
  InDetPixelMainsMonOnPixelTrack.TrackName = InDetKeys.PixelTracks() 

  ##Other parameters                                                                                                                  
  #PixelMainsMon.OfflineDoPixelOccupancy = False  #pixel occupancy plots for offline analysis.  Leave off except for private analysis 

  if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
    InDetPixelMainsMonOnPixelTrack.FilterTools.append(monFilledBunchFilterTool)
  
  ToolSvc += InDetPixelMainsMonOnPixelTrack                
  if (InDetFlags.doPrintConfigurables()): 
    print InDetPixelMainsMonOnPixelTrack            


######################################


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
if doHitsOnPixelTrack:
  InDetPixMonMan.AthenaMonTools     += [ InDetPixelMainsMonOnPixelTrack ]

topSequence += InDetPixMonMan
if (InDetFlags.doPrintConfigurables()):
  print InDetPixMonMan

