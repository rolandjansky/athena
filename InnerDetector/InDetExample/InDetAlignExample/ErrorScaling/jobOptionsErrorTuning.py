# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# include standard InDetRecExample joboptions
#
# get_files InDetRecExample/jobOptions.manual.py
#
# Make sure Inner Detector Monitoring is enabled
# InDetFlags.doMonitoringGlobal    = True
# InDetFlags.doMonitoringPixel     = True
# InDetFlags.doMonitoringSCT       = True
# InDetFlags.doMonitoringTRT       = True
# InDetFlags.doMonitoringAlignment = True

#This block prints out the error scaling used into the athena logfile
# Unfortunately it is bugged and causes segfaults sometimes
#from AthenaCommon.AppMgr import ToolSvc
#from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackErrorScalingTool
#InDetRotErrorScalingTool = Trk__RIO_OnTrackErrorScalingTool(
#    name= 'RIO_OnTrackErrorScalingTool',
#    OutputLevel = DEBUG )
#ToolSvc += InDetRotErrorScalingTool

#data sets are defined here
include('datasets.py')

#choose your dataset from those defined in datasets.py.
#datasample = data_2011_CALIB_RAW#data_2011_Muons_DESD#data_2011_Minbias_DESD
#datasample = data_2011_Muons_DESD
#datasample = data_IDCalibSkim
#datasample = data_155697_Minbias_DESD
#datasample = data_161397_Minbias_DESD
#datasample = data_PeriodI_Minbias_DESD
#datasample = data_2011_Minbias_DESD
#datasample = data_2011_Muons_DESD
#datasample = data_2011_CALIB_RAW
#datasample = mc10_Minbias_DESD
#datasample = data11_Muons_DRAW
#datasample = data11_Muons_DESD
#datasample = data11_calibration_RAW
#datasample = mc11_Ztautau_RAW
#datasample = mc12_muons
#datasample = mc12_Ztautau
#datasample = mc11_Zprime_ESD
#datasample = mc10_muons_RAW
#datasample = mc10_muons_RAW_highStat

datasample = data15_267073_Express_RAW
#datasample = data15_267073_Express_ESD
print 'Pre-job-option data set type:',datasample.getFormat()

#this should be passed in on the command line via the athena.py -c switch.
# Normally it's called by iterateTrkError.py which takes care of the 
# switches being set correctly
if (runmode == 'local'):
    theApp.EvtMax = 10
    datasample.activate()

#this should be passed in on the command line via the athena.py -c switch.
# Normally it's called by iterateTrkError.py which takes care of the 
# switches being set correctly. nSegments and iSegment are passed in this way
# as well
if (runmode == 'batch'):
    #theApp.EvtMax = 2500
    theApp.EvtMax = 500
    datasample.activateSegment(nSegments, iSegment, shuffle=True)

# just the InDetRecExample options as mentioned above
#include('ReadInDet_jobOptions.py')

if datasample.getFormat()=='ESD': 
    print "Using a ESD Dataset"
    include('jobOptionsESD_Run2.py')

if datasample.getFormat()=='RAW':
    print "Using a RAW Dataset"
    include('jobOptionsESD_Run2.py')

print 'This dataset is mc:',datasample.isMC()

#this fixes MC for now- look into why later
#if not datasample.isMC():
#conddb.blockFolder("/Indet/TrkErrorScaling")
    #errorScalingOverride is passed in on the command line when this job is invoked in
    # iterateTrkError.py . It typically contains the name of the override tag from the iteration.
#conddb.addFolderWithTag("LOCAL","/Indet/TrkErrorScaling", errorScalingOverride, force=True)
