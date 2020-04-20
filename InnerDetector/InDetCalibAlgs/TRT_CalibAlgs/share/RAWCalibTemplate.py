import os,sys,time,glob,fnmatch

def collisioncalib(config,inputfiles,calibconstants,calpart,calsettings):
#    print "Loading from CalibCollisionTemplate.py"
#    print "Based on Thjis jO: "
    ostring="""
#--------------------------------------------------------------
# Calibration stuff
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr


from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
ServiceMgr += TRTStrawNeighbourSvc

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
InDetCalDbTool=TRT_CalDbTool(name = "TRT_CalDbTool")

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
InDetStrawSummaryTool=TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                             isGEANT4=(globalflags.DataSource == 'geant4'))



from TRT_CalibTools.TRT_CalibToolsConf import FitTool
TRTCalFitTool = FitTool (name = 'TRTCalFitTool')
ToolSvc += TRTCalFitTool
print      TRTCalFitTool

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
                                      NeighbourSvc=TRTStrawNeighbourSvc,
                                      TRTCalDbTool = InDetCalDbTool,
                                      TRTStrawSummaryTool = InDetStrawSummaryTool)

ToolSvc += FillAlignTRTHits
print      FillAlignTRTHits

from TRT_CalibTools.TRT_CalibToolsConf import TRTCalibrator 
TRTCalibrator = TRTCalibrator ( name = 'TRTCalibrator',
                                MinRt               = %s,""" % config["MinRT"]
    ostring+="""
                                MinT0               = %s,""" % config["MinT0"]
    ostring+="""
                                Nevents             = -1,\n"""
    selstring=calsettings
    ostring+="%32sSelstring =  '%s'," % (" ",selstring['_'+calpart]['sel'])
    for setting in selstring['_'+calpart]['user']:
        if len(selstring['_'+calpart]['user'][setting])>0: ostring+= '\n%32s%s =  %s,'%(" ",setting,selstring['_'+calpart]['user'][setting])

    ostring+="""
                                Hittuple            = 'merged.root',
                                RtRel               = '%s',""" % config["RtRelation"]
    ostring+="""
                                RtBinning           = '%s',""" % config["RtBinning"]
    ostring+="""
                                UseP0               = %s,""" % "True"#config["UsePol0"]
    ostring+="""
                                FloatP3             = %s,""" % config["FloatPol3"]
    ostring+="""
                                T0Offset             = %s,""" % config["T0Offset"]
    ostring+="""
                                TrtManagerLocation  = InDetKeys.TRT_Manager(),
				DoShortStrawCorrection = False,
                                NeighbourSvc=TRTStrawNeighbourSvc,
"""	
    if config["DoArXe"]:
    	ostring+="""                                DoArXenonSep    = True,
"""
    ostring+="""                                TRTCalDbTool=InDetCalibDbTool)
ToolSvc += TRTCalibrator
print      TRTCalibrator

# select good quality tracks


from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
TRTTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorTool",
                                        pTMin              =    1000    ,
                                        fitChi2OnNdfMax         = 50.           ,
                                        z0Max           = 30            ,
                                        nHitBLayer      = 0,
                                        nHitPix         = 0,
                                        nHitBLayerPlusPix =     0,
                                        nHitSct         =       0,
                                        nHitSi          = 4,
                                        nHitTrt                         = 20,
                                        nHitTrtPlusOutliers             =0,
                                        nHitTrtHighE                    =0,
                                        nHitTrtPlusOutliersHighE        =0,
                                        nHitTrtHighEFractionMax         =0,
                                        nHitTrtHighEFractionWithOutliersMax=0
                                        )

ToolSvc += TRTTrackSelectorTool
if (InDetFlags.doPrintConfigurables()):
        print TRTTrackSelectorTool




                                        
from TRT_CalibAlgs.TRT_CalibAlgsConf import TRTCalibrationMgr
CosmicsTRTCalibMgr = TRTCalibrationMgr(name                = 'CosmicsTRTCalibMgr',
                                       StreamTool          = TRTCondStream,
                                       Max_ntrk            = 10000,
                                       TrackSelectorTool   = TRTTrackSelectorTool,
                                       TrkCollections      = [ 'TRTCalibTracks' ],
                                       AlignTrkTools       = [ FillAlignTrkInfo, FillAlignTRTHits ],
                                       DoCalibrate         = True,
                                       TrackFitter         = InDetTrackFitter,
                                       FitTools            = [ TRTCalFitTool] )

topSequence += CosmicsTRTCalibMgr
print CosmicsTRTCalibMgr                                       






from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTCondWrite
TRTCondWrite = TRTCondWrite( name = "TRTCondWrite")
topSequence+=TRTCondWrite 

# DCS Data Folders
if (globalflags.InputFormat() == 'bytestream' and globalflags.DataSource() == 'data'):
    if InDetFlags.useTrtDCS():
        conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/BARREL <cache>600</cache>",classname='CondAttrListCollection')
        conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/ENDCAPA <cache>600</cache>",classname='CondAttrListCollection')
        conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/ENDCAPC <cache>600</cache>",classname='CondAttrListCollection')


#############################################conddb.addFolderWithTag('TRT_OFL','/TRT/Calib/errors','TrtCalibErrors-ErrorVal-00-00')
"""
    if config["DoArXe"]:
        ostring+="""
#conddb.addOverride('/TRT/Cond/StatusHT','TrtStrawStatusHT-ArTest-00-00')
"""


    if not calibconstants=="":
        ostring+="""
conddb.blockFolder("/TRT/Calib/RT" )   
conddb.blockFolder("/TRT/Calib/T0" )  
conddb.addFolderWithTag('TRT_OFL','/TRT/Calib/errors','TrtCalibErrors-00') 
"""

    if not calibconstants=="":
        ostring+='TRTCondWrite.CalibInputFile="%s"\n' % (calibconstants)

    return ostring

