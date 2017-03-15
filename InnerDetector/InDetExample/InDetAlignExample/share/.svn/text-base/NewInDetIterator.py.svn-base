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

# =====================================================================
#
#  Main Script
#
# =====================================================================


# extract release configuration

ATHENACFG = getAthenaConfig(ASetupOptions)


print
if not os.path.isdir(OutputPath):
	os.mkdir(OutputPath)

info=open(OutputPath+"/info.txt",'w')
info.write("----------------------------------------------\n")
info.write("\t\t%s      \n" % datetime.date.today() )
info.write("----------------------------------------------\n")
#info.write("Release %s\n" % ATHENAREL)
info.write("Output stored in %s\n\n" % OutputPath)

print "Info stored in: " +OutputPath+"/info.txt"

if runMode == 'batch':
	print "Alignment Algorithm will run in Lxbatch"
	info.write("Alignment Algorithm run in Lxbatch\n")
elif runMode == ' local':
	print "Alignment Algorithm will run on local machine"
	info.write("Alignment Algorithm run on local machine\n")
	info.write("----------------------------------------------\n")
	print "System Info"
	info.write("----------------------------------------------\n")
	info.write("System Info\n")
	os.system('grep processor /var/log/dmesg | grep MHz')
	info.write("----------------------------------------------\n")
	info.write(os.popen('grep processor /var/log/dmesg | grep MHz','r').read())
	info.write("\n")
	os.system('dmesg | grep Brought')
	info.write(os.popen('dmesg | grep Brought','r').read())
	info.write("\n")
	os.system('cat /proc/meminfo | grep MemTotal')
	info.write(os.popen('cat /proc/meminfo | grep MemTotal','r').read())
	info.write("\n")
	info.write("----------------------------------------------\n")
	info.write("\n")

print
info.write("\n")
StartTime=time.time()   # Start the total time counter
info.close()

#  Loop over iterations
for iteration in range(FirstIteration,Iterations+FirstIteration):
	IterStartTime=time.time()
	if (iteration == 0) and not inputAlignmentPoolFile:
		ReadAlignmentConstants = False
	else:
		ReadAlignmentConstants = True

	print '\n'
	print " ---> Iteration "+repr(iteration)
	print '\n'
	info=open(OutputPath+"/info.txt",'a')
	info.write('\n')
	info.write("---> Iteration "+repr(iteration))
	info.write('\n')
	# Protect existing directories
	if os.path.isdir("%s/Iter%d" % (OutputPath, iteration)):
		countdir=0
		while os.path.isdir("%s/Iter%d-%s-%d" % (OutputPath, iteration, datetime.date.today(), countdir)):
			countdir += 1
		os.rename("%s/Iter%d" % (OutputPath, iteration),("%s/Iter%d-%s-%d" % (OutputPath, iteration, datetime.date.today(), countdir)))

		print "WARNING: %s/Iter%d directory exists" % (OutputPath, iteration)
		print "Renamed to %s/Iter%d-%s-%d" % (OutputPath, iteration, datetime.date.today(), countdir)

	# Make OutputPaths
	#os.mkdir(OutputPath+'/Iter'+repr(iteration))
	#os.mkdir(OutputPath+'/Iter'+repr(iteration)+'/logs/')
	# Settup up a local copy of the alignment levels
	#os.system("get_files -jo InDetAlignExample/NewInDetAlignLevels.py >/dev/null")
	#os.system("cp NewInDetAlignLevels.py %s" % OutputPath+'/Iter'+repr(iteration))
	#alignLevels = OutputPath+'/Iter'+repr(iteration)+"/NewInDetAlignLevels.py"
	
	for data in DataToRun:
		# Protect existing directories
		OutputPaths = ""
		if len(DataToRun) > 1:
			OutputPaths = OutputPath+'/'+data.getName()
			if os.path.isdir("%s/Iter%d" % (OutputPaths, iteration)):
				countdir=0
				while os.path.isdir("%s/Iter%d-%s-%d" % (OutputPaths, iteration, datetime.date.today(), countdir)):
					countdir += 1
				os.rename("%s/Iter%d" % (OutputPaths, iteration),("%s/Iter%d-%s-%d" % (OutputPaths, iteration, datetime.date.today(), countdir)))
				print "WARNING: %s/Iter%d directory exists" % (OutputPaths, iteration)
				print "Renamed to %s/Iter%d-%s-%d" % (OutputPaths, iteration, datetime.date.today(), countdir)
		else:
			OutputPaths = OutputPath
		if not os.path.isdir(OutputPaths):
			os.mkdir(OutputPaths)

		# Make OutputPaths '/'+data.getName()
		os.mkdir(OutputPaths+'/Iter'+repr(iteration))
		os.mkdir(OutputPaths+'/Iter'+repr(iteration)+'/logs/')
		# Settup up a local copy of the alignment levels
		os.system("get_files -jo InDetAlignExample/NewInDetAlignLevels.py >/dev/null")
		os.system("cp NewInDetAlignLevels.py %s" %(OutputPaths+'/Iter'+repr(iteration)))
		alignLevels = OutputPaths+'/Iter'+repr(iteration)+"/NewInDetAlignLevels.py"
		print "----------------------------------------------"
		print "Number of CPUs used to process the sample " + data.getName() + ": " + str(data.getCPUs(iteration))
		print "----------------------------------------------"

		info.write("Number of CPUs used to process the sample %s : %s\n\n" % (data.getName(), data.getCPUs(iteration)))

		if data.getEvents(iteration)!=-1:
			print "Number of events per CPU: " +  str(data.getEventsPerCPU(iteration))
			info.write("Number of events per CPU: %d\n\n" % data.getEventsPerCPU(iteration))
			
		else:
			print "Number of events per CPU: All" 
			info.write("Number of events per CPU: All" )
		os.mkdir(OutputPaths+'/Iter'+repr(iteration)+'/'+data.getName()+'/')

		print "Processing..."

		# Get the Input file
		dataFiles = SortCpus(data.getCPUs(iteration)
					  ,""
					  ,data.getFileList()
					  ,OutputLevel
					  ,doDetailedSplitting=doDetailedSplitting
					  ,nEventsPerFile=data.getEventsPerCPU(iteration))
		# Loop over subjobs
		for subJob in range(0, data.getCPUs(iteration)):
			RecoOptions = {}
			# Get the Reconstruction Options
			if ErrorScaling[iteration]:
				RecoOptions["errorScalingTag"] = errorScalingTag

			JOBNAME="%s_Iter%d_%s_Part%02d.py" % (preName,iteration,data.getName(),subJob)
			RecoScript = "InDetAlignExample/loadInDetRec_Run2Rel17.py"
			if not doDetailedSplitting:
				RecoOptions["inputFiles"] = dataFiles.getCPU(subJob)
				RecoOptions["numberOfEvents"] = data.getEventsPerCPU(iteration)
			else:
				RecoOptions["inputFiles"] = dataFiles.getInputFiles(subJob)
				RecoOptions["numberOfEvents"] = dataFiles.getNumEvents(subJob)
				RecoOptions["SkipEvents"] = dataFiles.getSkipEvents(subJob)
		
			extraOptions["doReadBS"] = data.getByteStream()
	
			if subJob == 0:
				filesForSolve = RecoOptions["inputFiles"]
				solveDoReadBS = extraOptions["doReadBS"]
			if len(data.getGlobalTag()) != 0:
				RecoOptions["globalTag"] = data.getGlobalTag()
			if len(data.getDetDescrVersion()) != 0:
				RecoOptions["detectorDescription"] = data.getDetDescrVersion()
	
		
	
			# Get the Alignment Options
			AlignmentOptions = {}
			if AlignmentMethod == "GX2":
				AlignmentOptions["runLocal"] = False
				AlignmentOptions["solveLocal"] = False
				AlignmentOptions["solvingOption"] = 1
				AlignmentOptions["ModCut"] = 6
			else:
				AlignmentOptions["runLocal"] = True
				AlignmentOptions["solvingOption"] = 0
	
	
			AlignmentOptions["readConstantsFromPool"] = ReadAlignmentConstants
			constantsFile = ""
			if ReadAlignmentConstants:
				if iteration == 0:
					constantsFile = inputAlignmentPoolFile 
					AlignmentOptions["inputPoolFiles"] = [constantsFile]
				else:
					constantsFile = str(OutputPaths)+"/Iter"+str(iteration-1)+"/Iter"+str(iteration-1)+"_AlignmentConstants.root"
					AlignmentOptions["inputPoolFiles"] = [constantsFile]
	
			AlignmentOptions["alignTRT"] = AlignTRT[iteration]
			if AlignTRT[iteration]:
				AlignmentOptions["trtAlignmentLevel"] = TRTAlignmentLevel[iteration]
	
			AlignmentOptions["alignSCT"] = AlignSCT[iteration]
			#if AlignSCTBarrel[iteration]:
			AlignmentOptions["sctAlignmentLevel"] = SCTAlignmentLevel[iteration]
	
			AlignmentOptions["sctAlignBarrel"] = AlignSCTBarrel[iteration]
			#if AlignSCTBarrel[iteration]:
			AlignmentOptions["sctAlignmentLevelBarrel"] = SCTAlignmentLevelBarrel[iteration]
	
			AlignmentOptions["sctAlignEndcaps"] = AlignSCTEndcaps[iteration]
			#if AlignSCTEndcaps[iteration]:
			AlignmentOptions["sctAlignmentLevelEndcaps"] = SCTAlignmentLevelEndcaps[iteration]
	
			AlignmentOptions["alignPixel"] = AlignPixel[iteration]
			#if AlignSCTBarrel[iteration]:
			AlignmentOptions["pixelAlignmentLevel"] = PixelAlignmentLevel[iteration]
	
			AlignmentOptions["pixelAlignBarrel"] = AlignPixelBarrel[iteration]
			#if AlignPixelBarrel[iteration]:
			AlignmentOptions["pixelAlignmentLevelBarrel"] = PixelAlignmentLevelBarrel[iteration]
	
			AlignmentOptions["pixelAlignEndcaps"] = AlignPixelEndcaps[iteration]
			#if AlignPixelEndcaps[iteration]:
			AlignmentOptions["pixelAlignmentLevelEndcaps"] = PixelAlignmentLevelEndcaps[iteration]
	
	
			AlignmentOptions["runSolving"] = False
			AlignmentOptions["runAccumulate"] = True
			AlignmentOptions["WriteTFile"] = useTFiles
							
	
			poolfiles = RecoOptions["inputFiles"]
			poolfiles.append(constantsFile)
		
	
			currentjob = manageJob(OutputPath = OutputPaths,
					      dataName = data.getName(),
					      iter = iteration,
					      part = subJob,
					      JOBNAME = JOBNAME,
					      preName = preName,
					      RecoOptions = RecoOptions,
					      extraOptions = extraOptions,
					      AlignmentOptions = AlignmentOptions,
					      RecoScript = RecoScript,
					      AlignmentLevels = alignLevels,
					      #MonitoringScript = MonitoringScript,
					      QUEUE = QUEUE,
					      CMTDIR = CMTDIR,
					      ATHENACFG = ATHENACFG,
					      inputPoolFiles = poolfiles
					      )
	
			# Write the job
			currentjob.createDirectories()
			currentjob.writeJO()
			currentjob.writeScript()
			
			currentjob.send(runMode)
	#  End j loop
	#if runMode == "batch":
		# Wait for signal
	#	currentjob.wait()


	#  Solving the system
	if doSolve:
		DataToSolve = list(DataToRun)
		while len(DataToSolve):
			for data in DataToSolve:
				batchjobs = "%s_Iter%d_%s_Part" % (preName,iteration,data.getName())
				if os.popen('bjobs -w').read().find(batchjobs)!=-1:
					continue
				time.sleep(20) # to be secure reco step has finished
				OutputPaths = ""
				if len(DataToRun) > 1:
					OutputPaths = OutputPath+'/'+data.getName()
				else:
					OutputPaths = OutputPath
				print "----------------------------------------------"
				print "  Solving dataset %s, Iter %s"%(data.getName(),iteration)
				print "----------------------------------------------"

				info.write('\n')
				info.write("----------------------------------------------\n")
				info.write("  Solving dataset %s, Iter %d\n" %(data.getName(),iteration))
				info.write("----------------------------------------------\n")
			
				alignLevels = OutputPaths+'/Iter'+repr(iteration)+"/NewInDetAlignLevels.py"
				PrefixName="Iter%d_" % iteration
				JOBNAME="%s_R%s_Iter%d_Solve.py" % (preName,data.getName(),iteration)
				SCRIPTNAME="%s_R%s_Iter%dSolve.lsf" % (preName,data.getName(),iteration)

				print "Logs stored in %s/Iter%d/logs/Iter%d_Solve.log" % (OutputPaths, iteration, iteration)
	
				# Get the aligment options
				AlignmentOptions = {}
		
				if AlignmentMethod == "GX2":
					AlignmentOptions["runLocal"] = False
					AlignmentOptions["solveLocal"] = False
				else:
					AlignmentOptions["runLocal"] = True
					AlignmentOptions["solvingOption"] = 0

				AlignmentOptions["readConstantsFromPool"] = ReadAlignmentConstants
	
				AlignmentOptions["inputPoolFiles"] = [constantsFile]
		
				AlignmentOptions["alignTRT"] = AlignTRT[iteration]
				if AlignTRT[iteration]:
					AlignmentOptions["trtAlignmentLevel"] = TRTAlignmentLevel[iteration]

				AlignmentOptions["alignSCT"] = AlignSCT[iteration]
				#if AlignSCTBarrel[iteration]:
				AlignmentOptions["sctAlignmentLevel"] = SCTAlignmentLevel[iteration]
	
				AlignmentOptions["sctAlignBarrel"] = AlignSCTBarrel[iteration]
				#if AlignSCTBarrel[iteration]:
				AlignmentOptions["sctAlignmentLevelBarrel"] = SCTAlignmentLevelBarrel[iteration]
	
				AlignmentOptions["sctAlignEndcaps"] = AlignSCTEndcaps[iteration]
				#if AlignSCTEndcaps[iteration]:
				AlignmentOptions["sctAlignmentLevelEndcaps"] = SCTAlignmentLevelEndcaps[iteration]
	
				AlignmentOptions["alignPixel"] = AlignPixel[iteration]
				#if AlignSCTBarrel[iteration]:
				AlignmentOptions["pixelAlignmentLevel"] = PixelAlignmentLevel[iteration]
	
				AlignmentOptions["pixelAlignBarrel"] = AlignPixelBarrel[iteration]
				#if AlignPixelBarrel[iteration]:
				AlignmentOptions["pixelAlignmentLevelBarrel"] = PixelAlignmentLevelBarrel[iteration]
	
				AlignmentOptions["pixelAlignEndcaps"] = AlignPixelEndcaps[iteration]
				#if AlignPixelEndcaps[iteration]:
				AlignmentOptions["pixelAlignmentLevelEndcaps"] = PixelAlignmentLevelEndcaps[iteration]


				# Get Vectors, Matricies and Hitmaps
				if not useTFiles:
					tdata = [data]
					matrices, vectors, hitmaps = mergeMatrix(OutputPaths, iteration, tdata)
					AlignmentOptions["inputMatrixFiles"] =  matrices
					AlignmentOptions["inputVectorFiles"] = vectors
					AlignmentOptions["inputHitmapFiles"] = hitmaps
					AlignmentOptions["WriteTFile"] = False
					if len(hitmaps) == 0:
						AlignmentOptions["readHitmaps"] = False
				else:
					tdata = [data]
					tfiles = mergeTFiles(OutputPaths, iteration, tdata)
					AlignmentOptions["inputTFiles"] = tfiles
					AlignmentOptions["WriteTFile"] = True			
			
		
				AlignmentOptions["runSolving"] = True
				AlignmentOptions["runAccumulate"] = False

				# Get the reconstruction options
				RecoOptions = {}
				RecoOptions["numberOfEvents"] = 1
				RecoOptions["inputFiles"] = filesForSolve
				extraOptions["doReadBS"] = solveDoReadBS

				if len(data.getGlobalTag()) != 0:
	 				RecoOptions["globalTag"] = data.getGlobalTag()
	
				if len(data.getDetDescrVersion()) != 0:
					RecoOptions["detectorDescription"] = data.getDetDescrVersion()

				RecoScript = "InDetAlignExample/loadInDetRec_Run2Rel17.py"
				constantsFile = ""
				if ReadAlignmentConstants:	
					if iteration == 0:
						constantsFile = inputAlignmentPoolFile 
						AlignmentOptions["inputPoolFiles"] = [inputAlignmentPoolFile]
					else:
						constantsFile = str(OutputPaths)+"/Iter"+str(iteration-1)+"/Iter"+str(iteration-1)+"_AlignmentConstants.root"
						AlignmentOptions["inputPoolFiles"] = [constantsFile]

				poolfiles = [RecoOptions["inputFiles"][0],constantsFile]
		
				currentjob = manageJob(OutputPath = OutputPaths,
				  dataName = data.getName(),
				  iter = iteration,
				  part = -1,
				  JOBNAME = JOBNAME,
				  preName = preName,
				  RecoOptions = RecoOptions,
				  extraOptions = extraOptions,
				  AlignmentOptions = AlignmentOptions,
				  RecoScript = RecoScript,
				  AlignmentLevels = alignLevels,
				  #MonitoringScript = MonitoringScript,
				  QUEUE = QUEUE,
				  CMTDIR = CMTDIR,
				  ATHENACFG = ATHENACFG,
				  inputPoolFiles = poolfiles
				  )

				# Write the job
				currentjob.createDirectories()
				currentjob.writeJO()
				currentjob.writeScript()
				currentjob.send(runMode)
				DataToSolve.remove(data)
			#end loop over DatatoSolve
		#end while
		
		if runMode == "batch":
			# Wait for signal
			currentjob.wait()
		
	else:
		print "-----------------------------------------------------\n"
		print " WARNING: Skipping the solving due to low statistics\n"
		print "-----------------------------------------------------\n"
	for data in DataToRun:
		if "doMonitoring" in extraOptions and extraOptions["doMonitoring"]==True:
			OutputPaths = OutputPath+'/'
			MERGEJOBNAME="%s_Iter%d_%s_Merge.py" % (preName,iteration,data.getName())
			MERGESCRIPTNAME="%s_Iter%d_%s_Merge.lsf" % (preName,iteration, data.getName())
			monitoringMerge = mergeScript(OutputPath = OutputPaths
					      ,iter = iteration
					      ,preName = preName
					      ,dataName = data.getName()
					      ,nCPUs = data.getCPUs(iteration)
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
