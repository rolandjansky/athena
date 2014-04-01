#!/afs/cern.ch/sw/lcg/external/Python/2.5.4/slc4_ia32_gcc34/bin/python

#================================
# Example to run over several runs and obtain
# a set of alignment constants for each run
#===============================

runlist = [152220, 152221, 167885 , 167680 , 167661 , 167607 , 167576 , 167575 , 167573 , 166964 , 166927 , 166925 , 166924 , 166856 , 166850 ,
166786 , 166658 , 166466 , 166383 , 166305 , 166198 , 166143 , 166142 , 166097 , 166094 , 165956 , 165954 , 165821 , 165818 , 165817 , 165815 ,
165767 , 165732 , 165703 , 165632 , 165591 , 162882 , 162843 , 162764 , 162690 , 162623 , 162620 , 162577 , 162576 , 162526 , 162347 , 161948 ,
161562 , 161520 , 161407 , 161379 , 161118 , 161116 , 161114 , 160980 , 160975 , 160963 , 160958 , 160954 , 160953 , 160899 , 160879 , 160801 ,
160800 , 160736 , 160613 , 160530 , 160479 , 160472 , 160387 , 160303 , 159224 , 159203 , 159202 , 159179 , 159113 , 159086 , 159041 , 158975 ,
158801 , 158632 , 158582 , 158549 , 158548 , 158545 , 158466 , 158443 , 158392 , 158299 , 158269 , 158116 , 158045 , 156682 , 156463 , 155794 ,
155697 , 155678 , 155669 , 155634 , 155569 , 155280 , 155228 , 155160 , 155116 , 155112 , 155073 , 154822 , 154817 , 154815 , 154813 , 154810 ,
154471 , 154469 , 154465 , 154126 , 153599 , 153565 , 153200 , 153159 , 153136 , 153135 , 153134 , 153030 , 152994 , 152933 , 152878 , 152845 ,
152844 , 152779 , 152777 , 152508 , 152441 , 152409 , 152345]

import os,sys
filesPerCPU = 10
FirstIteration = 0 
Iterations     = 2
preName        = 'L1-2010-Align'
runMode        = 'batch'
QUEUE 	       = 'atlasb1'
HOME 	       = os.environ['HOME']
OutputPath     = "/afs/cern.ch/user/s/spedraza/work/alignment/repro2010/L1-L2PixEC/" 
CMTDIR 	       = HOME+"/athena/cmthome/"
ASetupOptions  = "single"
OutputLevel    = 'INFO'

from InDetAlignExample.NewInDet_SetupData import setupData
DataToRun = []
Data1 =[]
counter = 0
for run in runlist:

	#
	# Delete the lines below if you dont have the 
	# %s_IDTracks.txt file for each run
	###############
	try:
		f = open("%s_IDTracks.txt"%(run),'r')
	except:
		continue
	numberCPUs = len(f.readlines())/filesPerCPU
	if numberCPUs == 0:
		numberCPUs = 1
	f.close()
	###############
	Data1.append(setupData('%s'%(run))) # line to replace
	Data1[counter].setDataType("IDTracks")
	Data1[counter].setRuns([run]) # line to replace
	Data1[counter].setCPUs([numberCPUs,numberCPUs]) # line to replace
	Data1[counter].setEvents([-1,-1])
	Data1[counter].CreateFileList()
	Data1[counter].setGlobalTag("COMCOND-BLKPA-006-10")
	Data1[counter].setDetDescrVersion("ATLAS-GEO-20-19-01")
	DataToRun.append(Data1[counter])
	counter += 1
print len(DataToRun), counter

AlignmentMethod = "GX2"

inputAlignmentPoolFile = ""
doDetailedSplitting = False
useTFiles = True
extraOptions = {}
extraOptions["doMonitoring"] = True
extraOptions["doTrkNtuple"] = False
extraOptions["PtCut"]               = 5000 #Pt in MeV, comment the line to use default value (10000)
extraOptions["lorentzAngleTag"]     ="PIXELLorentzAngleScale-Data-001"
extraOptions["siAlignmentTag"]      = "InDetAlign_Repro2012_d0z0p_constrained"
extraOptions["trtAlignmentTag"]     = "TRTAlign_Repro2012_d0z0p_constrained"
extraOptions["trtAlignmentTagL3"]   = "TRTCalibDX_EoverP_189598_189845v1"
extraOptions["ModCut"]				= 0
extraOptions["softModeCut"]			= 0.
extraOptions["doBSConstraint"]		= False 
extraOptions["doPVConstraint"]		= False
extraOptions["useOldPreProcessor"]	= False
## TRT
extraOptions["useTRT"] 			= True
extraOptions["trtAlignBarrelZ"] = False
extraOptions["trtAlignEndcapZ"] = False
## PIXEL
extraOptions["pixelAlignBarrelX"] 	= False
extraOptions["pixelAlignBarrelY"] 	= False
extraOptions["pixelAlignBarrelZ"] 	= False
extraOptions["pixelAlignBarrelRotX"]= False
extraOptions["pixelAlignBarrelRotY"]= False
extraOptions["pixelAlignBarrelRotZ"]= False
## alignment parameters Endcap
extraOptions["pixelAlignEndcapX"] 	= True
extraOptions["pixelAlignEndcapY"] 	= True
extraOptions["pixelAlignEndcapZ"] 	= False
extraOptions["pixelAlignEndcapRotX"]= False
extraOptions["pixelAlignEndcapRotY"]= False
extraOptions["pixelAlignEndcapRotZ"]= True
## SCT
extraOptions["sctAlignBarrelX"]		= True
extraOptions["sctAlignBarrelY"]		= True
extraOptions["sctAlignBarrelZ"]		= True
extraOptions["sctAlignBarrelRotX"]	= True
extraOptions["sctAlignBarrelRotY"]	= True
extraOptions["sctAlignBarrelRotZ"]	= True
## alignment parameters Endcap
extraOptions["sctAlignEndcapX"]		= True
extraOptions["sctlAlignEndcapY"]	= True
extraOptions["sctAlignEndcapZ"]		= True
extraOptions["sctAlignEndcapRotX"]	= True
extraOptions["sctAlignEndcapRotY"]	= True
extraOptions["sctAlignEndcapRotZ"]	= True

doSolve = True

AlignPixel                 = [True,True]
AlignPixelBarrel           = [True,True]
AlignPixelEndcaps          = [True,True]
PixelAlignmentLevel        = [ 2, 2]
PixelAlignmentLevelBarrel  = [-1,-1]
PixelAlignmentLevelEndcaps = [-1,-1]

AlignSCT                  =  [True,True]
AlignSCTBarrel            =  [True,True]
AlignSCTEndcaps           =  [True,True]
SCTAlignmentLevel         =  [ 1, 1]
SCTAlignmentLevelBarrel   =  [-1,-1]
SCTAlignmentLevelEndcaps  =  [-1,-1]

AlignTRT            = [True,True]
TRTAlignmentLevel   = [1, 1]

ErrorScaling        = [True,True]
errorScalingTag		= ''
MonitoringScript	= ""

os.system("get_files -jo InDetAlignExample/NewInDetIterator.py >/dev/null")
print os.system("get_files -jo InDetAlignExample/NewInDetIterator.py")
execfile("NewInDetIterator.py")
