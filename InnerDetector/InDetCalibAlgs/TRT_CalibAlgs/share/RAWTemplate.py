import os,sys,time,glob,fnmatch

def collision(config,inputfiles,calibconstants,nevents):
#    print "Loading from CollisionTemplate.py"
#    print "Based on Thjis jO: "
    ostring="""

from AthenaCommon.AppMgr import ToolSvc

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
ServiceMgr += TRTStrawNeighbourSvc

from TRT_CalibTools.TRT_CalibToolsConf import FitTool
TRTCalFitTool = FitTool (name = 'TRTCalFitTool')
ToolSvc += TRTCalFitTool
print      TRTCalFitTool

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
InDetCalDbTool=TRT_CalDbTool(name = "TRT_CalDbTool")

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
InDetStrawSummaryTool=TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                             isGEANT4=(globalflags.DataSource == 'geant4'))


from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
TRTCondStream=AthenaOutputStreamTool(name="CondStream1",OutputFile="trtcalibout.pool.root")

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
                                      TRTCalDbTool = InDetCalDbTool,
                                      TRTStrawSummaryTool = InDetStrawSummaryTool)

ToolSvc += FillAlignTRTHits
print      FillAlignTRTHits



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
                                       StreamTool          =  TRTCondStream,
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
#conddb.addOverride('/TRT/Align/','TRTAlign_Collision_7T_2010_07')  
#conddb.addOverride('/Indet/Align/','InDetAlign_Collision_7T_2010_07')  
#conddb.addOverride('/Indet/TrkErrorScaling','TrkErrorScaling_7T_2010_02')

#conddb.addFolderWithTag('TRT_OFL','/TRT/Calib/errors','TrtCalibErrors-ErrorVal-00-00')

# DCS Data Folders
if (globalflags.InputFormat() == 'bytestream' and globalflags.DataSource() == 'data'):
    if InDetFlags.useTrtDCS():
        conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/BARREL <cache>600</cache>",classname='CondAttrListCollection')
        conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/ENDCAPA <cache>600</cache>",classname='CondAttrListCollection')
        conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/ENDCAPC <cache>600</cache>",classname='CondAttrListCollection')

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
        ostring+='TRTCondWrite.CalibInputFile="%s"\n' % (calibconstants)

    return ostring

