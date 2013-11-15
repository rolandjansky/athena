import os,sys,time,glob,fnmatch

def collision(config,inputfiles,calibconstants,nevents):
#    print "Loading from CollisionTemplate.py"
#    print "Based on Thjis jO: "
    ostring="""

from AthenaCommon.AppMgr import ToolSvc

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
TRTCalibDBSvc=TRT_CalDbSvc()
ServiceMgr += TRTCalibDBSvc

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
ServiceMgr += TRTStrawNeighbourSvc

from TRT_CalibTools.TRT_CalibToolsConf import FitTool
TRTCalFitTool = FitTool (name = 'TRTCalFitTool')
ToolSvc += TRTCalFitTool
print      TRTCalFitTool


from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
TRTCondStream=AthenaPoolOutputStreamTool(name="CondStream1",OutputFile="trtcalibout.pool.root")

ToolSvc += TRTCondStream
print TRTCondStream

from TRT_CalibTools.TRT_CalibToolsConf import FillAlignTrkInfo 
FillAlignTrkInfo = FillAlignTrkInfo ( name = 'FillAlignTrkInfo',
                                      TrackSummaryTool = InDetTrackSummaryTool)
ToolSvc += FillAlignTrkInfo
print      FillAlignTrkInfo

from TRT_CalibTools.TRT_CalibToolsConf import FillAlignTRTHits 
FillAlignTRTHits = FillAlignTRTHits ( name = 'FillAlignTRTHits',
                                      minTimebinsOverThreshold=0, 
                                      NeighbourSvc=TRTStrawNeighbourSvc,
                                      TRTCalDbSvc=TRTCalibDBSvc)
ToolSvc += FillAlignTRTHits
print      FillAlignTRTHits

# select good quality tracks
#from TRT_AlignAlgs.TRT_AlignAlgsConf import TRTTrackSelectionAlg
#SelectTRTAlignTracks = TRTTrackSelectionAlg(    name = "SelectTRTAlignTracks",
#                                                inputTrackList = "CombinedInDetTracks",
#                                                inputTrackList = "StandaloneTRTTracks",
#                                                outputTrackList = "TRTCalibTracks",
#                                                SummaryTool = InDetTrackSummaryTool,
#                                                MaxChisqPerDof = 50.,
#                                                D0Max                   = 30       ,
#                                                D0Min                   = -30      ,
#                                                DoPtCut                 = True     ,
#                                                PtMax                   = 9.9999999999999998e+23,
#                                                PtMin                   = 1000          ,
#                                                EtaMax                  = 10000         ,
#                                                EtaMin                  = -10000        ,
#                                                PhiMax                  = 10000         ,
#                                                PhiMin                  = -10000        ,
#                                                MaxEventPhase           = 10000         ,
#                                                MinEventPhase           = -10000        ,
#                                                MinTRTHits              = 20            ,
#                                                MinBarrelHits           = 0            ,
#                                                MinEndcapHits           = 0             ,
#                                                RequireEndcapHits       = False         ,
#                                                MinPixelHits            = -1000         ,
#                                                MinSCTHits              = 6         ,
##                                                MinSCTHits              = -1000         ,
#                                                UseBeamSpotConstraint   = False         )
#
#topSequence += SelectTRTAlignTracks
#print          SelectTRTAlignTracks


from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
TRTTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorTool",
                                        pTMin                   =    1000    ,
                                        fitChi2OnNdfMax         = 50. ,
                                        z0Max                   = 300  ,
                                        nHitBLayer              = 0 ,
                                        nHitPix                 = 0 ,
                                        nHitBLayerPlusPix       = 0 ,
                                        nHitSct                 = 0 ,
                                        nHitSi                  = 8 ,
                                        nHolesPixel             = 0,
                                        nHitTrt                         = 20,
                                        nHitTrtPlusOutliers             =0,
                                        nHitTrtHighE                    =0,
                                        nHitTrtPlusOutliersHighE        =0,
                                        nHitTrtHighEFractionMax         =1,
                                        nHitTrtHighEFractionWithOutliersMax=1
                                        )

ToolSvc += TRTTrackSelectorTool
if (InDetFlags.doPrintConfigurables()):
        print TRTTrackSelectorTool




from TRT_CalibAlgs.TRT_CalibAlgsConf import TRTCalibrationMgr
CosmicsTRTCalibMgr = TRTCalibrationMgr(name                = 'CosmicsTRTCalibMgr',
                                       TrackSelectorTool   = TRTTrackSelectorTool,
                                       #TrkCollections      = [ 'TRTCalibTracks' ],
                                       TrkCollections      = [ 'CombinedInDetTracks' ],
                                       AlignTrkTools       = [ FillAlignTrkInfo, FillAlignTRTHits ],
                                       #AccumulatorTools    = [ TRTCalAccumulator ],
                                       TrackFitter         = InDetTrackFitter,
                                       FitTools            = [ TRTCalFitTool] )

topSequence += CosmicsTRTCalibMgr
print CosmicsTRTCalibMgr

from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTCondWrite
TRTCondWrite = TRTCondWrite( name = "TRTCondWrite")
topSequence+=TRTCondWrite 

#==============================
#====  Straw Status stuff  ====
#==============================

from TRT_CalibAlgs.TRT_CalibAlgsConf import InDet__TRT_StrawStatus
TRT_StrawStatus = InDet__TRT_StrawStatus(       name                    = "TRT_StrawStatus",
                                                outputFileName          = "TRT_StrawStatusOutput",
                                                tracksCollection        = "CombinedInDetTracks"                                          
                                               )
topSequence += TRT_StrawStatus
print TRT_StrawStatus





#conddb.addFolder('TRT_OFL','/TRT/Calib/DX')
#conddb.addOverride('/TRT/Calib/DX','TRTCalibDX_L3BarrelEndcap_01')
#conddb.addOverride('/Indet/Beampos','IndetBeampos-ES1-UPD2')  
#conddb.addOverride('/Indet/PixelDist','InDetPixelDist-000-01')  
#conddb.addOverride('/TRT/Align/','TRTAlign_Collision_7T_2010_07')  
#conddb.addOverride('/Indet/Align/','InDetAlign_Collision_7T_2010_07')  
#conddb.addOverride('/Indet/TrkErrorScaling','TrkErrorScaling_7T_2010_02')

#conddb.addFolderWithTag('TRT_OFL','/TRT/Calib/errors','TrtCalibErrors-ErrorVal-00-00')
"""
    if config["DoArXe"]:
        ostring+="""
#conddb.addOverride('/TRT/Cond/StatusHT','TrtStrawStatusHT-ArTest-00-00')
"""

    if not calibconstants=="":
        ostring+="""
conddb.blockFolder("/TRT/Calib/RT" )   
conddb.blockFolder("/TRT/Calib/T0" )   
#conddb.addFolderWithTag('TRT_OFL','/TRT/Calib/errors','TrtCalibErrors-ErrorVal-00-00')
"""

    if not calibconstants=="":
        ostring+='TRTCalibDBSvc.calibTextFile="%s"\n' % (calibconstants)

    ostring+='TRTCalibDBSvc.StreamTool=TRTCondStream'
    return ostring

