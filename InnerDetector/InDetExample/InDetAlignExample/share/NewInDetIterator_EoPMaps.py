#################################################################
#    Script to run InDetAlignExample with parallelization       #
#      (local or lxbatch mode) & (physic data or cosmics)       #
#             Vicente Lacuesta [started 12-03-2008]             #
#################################################################

import os, string, time, datetime
import sys

# ==============================================================
#  End of User Options
# ==============================================================

# =====================================================================
# =====================================================================
# =====================================================================
# =====================================================================
#
# Don not edit the lines below unless you know what you're doing!
#

print "Output saved in:",OutputPath

# AW: moved that one to python dir such that it can be imported from anywhere
from InDetAlignExample.NewInDet_IteratorClasses import *
from InDetAlignExample.NewInDet_IteratorClasses_EoverPMaps import *

# =====================================================================
#
#  Main Script
#
# =====================================================================


# extract release configuration

ATHENACFG = getAthenaConfig(ASetupOptions)

if runMode == 'batch':
	print "E over P map jobs will run in Lxbatch"
elif runMode == ' local':
	print "E over P map jobs will run on local machine"

print




IterStartTime=time.time()
ReadAlignmentConstants = True
if os.path.isdir("%s" % OutputPath):
	countdir=0
	while os.path.isdir("%s-%s-%d/" % (OutputPath[:-1], datetime.date.today(), countdir)):
		countdir += 1
	os.rename("%s" % OutputPath,"%s-%s-%d/" % (OutputPath[:-1], datetime.date.today(), countdir))
	
	print "WARNING: %s directory exists" % OutputPath
	
	print "Renamed to %s-%s-%d" % (OutputPath[:-1], datetime.date.today(), countdir)
os.mkdir(OutputPath)

for data in DataToRun:
	print "----------------------------------------------"
	print "Number of CPUs used to process the sample " + data.getName() + ": " + str(data.getCPUs(0))
	print "----------------------------------------------"

	if data.getEvents(0)!=-1:
		print "Number of events per CPU: " +  str(data.getEventsPerCPU(0))
	else:
		print "Number of events per CPU: All" 

	os.mkdir(OutputPath+data.getName()+'/')
	if not os.path.isdir(OutputPath+'/logs'):
		os.mkdir(OutputPath+'/logs')
	print "Processing..."
	# Get the Input file
	dataFiles = SortCpus(data.getCPUs(0)
				  ,""
				  ,data.getFileList()
				  ,OutputLevel
				  ,doDetailedSplitting=doDetailedSplitting
				  ,nEventsPerFile=data.getEventsPerCPU(0))
	# Loop over subjobs
	for subJob in range(0, data.getCPUs(0)):
		RecoOptions = {}
		#extraOptions = {}
		AlignmentOptions = {}
		# Get the Reconstruction Options
		JOBNAME="%s_%s_Part%02d.py" % (preName,data.getName(),subJob)
		SCRIPTNAME = "%s_%s_Part%02d.lsf" % (preName,data.getName(),subJob)
		RunPath="%s/%s/%02d/" % (OutputPath, data.getName(), subJob)
		RecoScript = "InDetAlignExample/loadInDetRec_EoverPMaps.py"
		if not doDetailedSplitting:
			RecoOptions["inputFiles"] = dataFiles.getCPU(subJob)
			RecoOptions["numberOfEvents"] = data.getEventsPerCPU(0)
		else:
			RecoOptions["inputFiles"] = dataFiles.getInputFiles(subJob)
			RecoOptions["numberOfEvents"] = dataFiles.getNumEvents(subJob)
			RecoOptions["SkipEvents"] = dataFiles.getSkipEvents(subJob)
		

		if len(data.getGlobalTag()) != 0:
			RecoOptions["globalTag"] = data.getGlobalTag()
		if len(data.getDetDescrVersion()) != 0:
			RecoOptions["detectorDescription"] = data.getDetDescrVersion()

			RecoOptions["readConstantsFromPool"] = ReadAlignmentConstants
			constantsFile = ""
		if ReadAlignmentConstants:
			constantsFile = inputAlignmentPoolFile 
			RecoOptions["inputPoolFiles"] = [constantsFile]
		RecoOptions["GoodRunList"] = GoodRunList
	
		currentjob = manageJobEoverPMaps(OutputPath = OutputPath,
				      dataName = data.getName(),
				      iter = -1,
				      part = subJob,
				      JOBNAME = JOBNAME,
				      SCRIPTNAME = SCRIPTNAME,
				      RunPath = RunPath,
				      preName = preName,
				      RecoOptions = RecoOptions,
				      extraOptions = extraOptions,
				      AlignmentOptions = AlignmentOptions,
				      RecoScript = RecoScript,
				      AlignmentLevels = 0,
				      #MonitoringScript = MonitoringScript,
				      QUEUE = QUEUE,
				      CMTDIR = CMTDIR,
				      ATHENACFG = ATHENACFG,
				      inputPoolFiles = ""
				      )

		# Write the job
		currentjob.createDirectories()
		currentjob.writeJO()
		currentjob.writeScript()
		
		currentjob.send(runMode)
			
				
	
	#  End j loop
if runMode == "batch":
	# Wait for signal
	currentjob.wait()

MERGEJOBNAME="%s_Merge.py" % preName
MERGESCRIPTNAME="%s_Merge.lsf" % preName
monitoringMerge = mergeScriptEoverPMaps(OutputPath = OutputPath
			      ,preName = preName
			      ,QUEUE = QUEUE
			      ,CMTDIR = CMTDIR
			      ,ATHENACFG = ATHENACFG
			      ,SCRIPTNAME = MERGESCRIPTNAME
			      ,JOBNAME = MERGEJOBNAME)

monitoringMerge.write()
monitoringMerge.send(runMode)

#	print "  Iteration %d finished: %5.3f seconds \n" % (iteration,(time.time()-IterStartTime))
#	print "----------------------------------------------"
#
#	info.write("\n")
#	info.write("----------------------------------------------\n")
#	info.write("  Iteration %d finished: %5.3f  seconds\n" % (iteration, (time.time()-IterStartTime)))
#	info.write("----------------------------------------------\n")
#
#	lastIteration = iteration
#	os.system("gzip %s/Iter%d/logs/Iter*.log" % (OutputPath, iteration))
#
#
## =======================
## Post Processing
## =======================
##if MonitoringScript == True:
#if False:
#	print
#	print "Comparing the Monitoring Files"
#	print
#	info.write('\n')
#	info.write("Comparing the Monitoring Files \n" )
#	info.write("\n")
#	compareMonitoring = compareMonitoringScript(OutputPath = OutputPath
#						    ,numIter = Iterations
#						    ,QUEUE = QUEUE
#						    ,CMTDIR = CMTDIR
#						    ,ATHENACFG = ATHENACFG)
#						    
#	compareMonitoring.write()
#	compareMonitoring.send(runMode)
#
#	print
#	print "Processed %d iterations !!!" % Iterations
#	print "  %5.3f  seconds" % (time.time()-StartTime)
#
#info=open(OutputPath+"/info.txt",'a')
#info.write('\n')
#info.write("Processed %d iterations !!!\n" % Iterations)
#info.write("  %5.3f  seconds \n" % (time.time()-StartTime))
#
