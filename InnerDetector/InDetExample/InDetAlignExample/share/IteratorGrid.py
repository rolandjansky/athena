#################################################################
#    Script for running ATLAS ID alignment jobs on Grid resources
#    Authors: Jike Wang      (jike.wang@cern.ch)
#             Song-Ming Wang (smwang@phys.sinica.edu.tw)
#################################################################

import os, string, time, datetime
import sys, glob

# ===================================================================================================
# ===================================================================================================
#  End of User Options, DO NOT modify the lines in below unless you really know what you are doing!!!  
# ===================================================================================================
# ===================================================================================================

from InDetAlignExample.IteratorGridClasses import *

nowtime = datetime.datetime.now()
thisJobDir = "%04d%02d%02d%02d%02d%02d" % (nowtime.year,nowtime.month,nowtime.day,nowtime.hour,nowtime.minute,nowtime.second)


if GridOptions["submitOnlyOneJobset"] :
	ystr = './%s*' % str(nowtime.year)
	ret  = glob.glob(ystr)
	for rundir in ret : 
		os.system("mv %s  %s "   % ( rundir, GridOptions["TmpWorkDir"])  )

#os.system("mv IteratorGrid.py %s " % GridOptions["TmpWorkDir"])

os.mkdir(thisJobDir)

if ('removeFileList' in GridOptions) and  GridOptions['removeFileList'] != "" : 
	if os.path.isfile('%s' % GridOptions['removeFileList']) :
		os.system(" mv  %s  %s "% (GridOptions['removeFileList'] , thisJobDir))



if ('extraFileList'  in GridOptions) and GridOptions["extraFileList"] != "" : 
	if os.path.isfile('%s' % GridOptions['extraFileList'])  :
		os.system(" cp  %s  %s "% (GridOptions['extraFileList'] , thisJobDir))


os.chdir(thisJobDir)



os.system(" get_files -jo InDetAlignExample/NewInDetAlignAlgSetup.py  > /dev/null ")
print"current path:", (os.getcwd())
cpath = os.getcwd()

GridAccSubJobID          = 'GridAccSubJobID.txt'
GridAccSubInfo           = 'GridAccSubInfo.txt'
GridAccOutDS             = 'GridAccOutDS.txt'
GridTotalAccOutDS        = 'GridTotalAccOutDS.txt'
GridSolvingOutDS         = 'GridSolvingOutDS.txt'

#print GridAccSubJobID
#print GridAccSubInfo


# Prepare number of files and events per CPU
CPUs = {}
FilesByCPU = {}
Events = {}
Collision = {}
CosmicsBon = {}
CosmicsBoff = {}


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


print
info.write("\n")
StartTime=time.time()   # Start the total time counter
info.close()



#  Loop over iterations
for iteration in range(FirstIteration, FirstIteration+Iterations):
	IterStartTime=time.time()


	os.system("rm -f %s\n" % GridAccSubJobID)
	os.system("rm -f %s\n" % GridAccSubInfo)
	os.system("rm -f %s\n" % GridAccOutDS)
	os.system("rm -f %s\n" % GridSolvingOutDS)



	#prepareForThisIter(iteration, GridFileOptions)


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
	if os.path.isdir("%s/Iter%02d" % (OutputPath, iteration)):
		countdir=0
		while os.path.isdir("%s/Iter%02d-%s-%d" % (OutputPath, iteration, datetime.date.today(), countdir)):
			countdir += 1
		os.rename("%s/Iter%02d" % (OutputPath, iteration),("%s/Iter%02d-%s-%d" % (OutputPath, iteration, datetime.date.today(), countdir)))

		print "WARNING: %s/Iter%02d directory exists" % (OutputPath, iteration)
		print "Renamed to %s/Iter%02d-%s-%d" % (OutputPath, iteration, datetime.date.today(), countdir)

	# Make OutputPaths
	os.mkdir(OutputPath+'/Iter%02d' % iteration )

	# Settup up a local copy of the alignment levels
	os.system("get_files -jo InDetAlignExample/NewInDetAlignLevels.py >/dev/null")
	#os.system("cp NewInDetAlignLevels.py .")
	alignLevels = "InDetAlignExample/NewInDetAlignLevels.py"

	os.system("get_files -jo InDetAlignExample/BuildGeoForMatrixSolving.py >/dev/null")
	solveJO    = "BuildGeoForMatrixSolving.py"	


	os.mkdir('%s/Iter%02d/logs/' %  (OutputPath, iteration) )
	if GridOptions["ColCPUs"][iteration]         > 0:
		os.mkdir('%s/Iter%02d/Collision/'       % (OutputPath, iteration) )
	if GridOptions["CosBoffCPUs"][iteration] > 0:
		os.mkdir('%s/Iter%02d/CosmicsBoff/' % (OutputPath, iteration) )
	if GridOptions["CosBonCPUs"][iteration]   > 0:
		os.mkdir('%s/Iter%02d/CosmicsBon/'   % (OutputPath, iteration) )

	print "Processing..."

			
		
	####################### Loop over different data topology #############################################################################################


	outputTarFileNames = {}
	JOBNAMES = {}

	if doAccumulate:
		RecoOptions = {}
		# Get the Reconstruction Options
		if ErrorScaling[iteration]:
			RecoOptions["errorScalingTag"] = errorScalingTag

		RecoOptions["numberOfEvents"]      = -1

		#if (Datasets.containType("900GeV") or Datasets.containType("SingleBeam") or Datasets.containType("7TeV")) and  Datasets.stream() == "IDTracks" : 
		#	extraOptions["doReadBS"]   = True
		#else:
		#	extraOptions["doReadBS"]   = False
			

		########### start set the Alignment Options ############################################################################################
		AlignmentOptions = {}

		if AlignmentMethod == "GX2":
			AlignmentOptions["runLocal"     ] = False
			AlignmentOptions["solveLocal"   ] = False
			AlignmentOptions["solvingOption"] = 1
			AlignmentOptions["ModCut"       ] = 6
		else:
			AlignmentOptions["runLocal"     ] = True
			AlignmentOptions["solvingOption"] = 0			

		AlignmentOptions["readConstantsFromPool"] = ReadAlignmentConstants
		if ReadAlignmentConstants:
			if iteration == 0:

				if False == os.path.isfile('%s' % inputAlignmentPoolFile):
					print "Error: the inputAlignmentPoolFile not exit ..."
					sys.exit()
				else:
					os.system("cp %s initial_AlignmentConstants.root " % inputAlignmentPoolFile)
					AlignmentOptions["inputPoolFiles"] = ["initial_AlignmentConstants.root"] 

			else:
				tmpStr = "Iter%02d_AlignmentConstants.root" % ( iteration-1 )
				AlignmentOptions["inputPoolFiles"] = [tmpStr]

		tmpStr = "Iter%02d_AlignmentConstants.root" % ( iteration )
		AlignmentOptions["outputPoolFile"            ] = tmpStr


		AlignmentOptions["alignTRT"                  ]  = AlignTRT[iteration]
		if AlignTRT[iteration]:
			AlignmentOptions["trtAlignmentLevel" ]  = TRTAlignmentLevel[iteration]

		AlignmentOptions["alignSCT"                  ]  = AlignSCT[iteration]
		#if AlignSCTBarrel[iteration]:
		AlignmentOptions["sctAlignmentLevel"         ]  = SCTAlignmentLevel[iteration]

		AlignmentOptions["sctAlignBarrel"            ]  = AlignSCTBarrel[iteration]
		#if AlignSCTBarrel[iteration]:
		AlignmentOptions["sctAlignmentLevelBarrel"   ]  = SCTAlignmentLevelBarrel[iteration]

		AlignmentOptions["sctAlignEndcaps"           ]  = AlignSCTEndcaps[iteration]
		#if AlignSCTEndcaps[iteration]:
		AlignmentOptions["sctAlignmentLevelEndcaps"  ]  = SCTAlignmentLevelEndcaps[iteration]

		AlignmentOptions["alignPixel"                ]  = AlignPixel[iteration]
		#if AlignSCTBarrel[iteration]:
		AlignmentOptions["pixelAlignmentLevel"       ]  = PixelAlignmentLevel[iteration]

		AlignmentOptions["pixelAlignBarrel"          ]  = AlignPixelBarrel[iteration]
		#if AlignPixelBarrel[iteration]:
		AlignmentOptions["pixelAlignmentLevelBarrel" ]  = PixelAlignmentLevelBarrel[iteration]

		AlignmentOptions["pixelAlignEndcaps"         ]  = AlignPixelEndcaps[iteration]
		#if AlignPixelEndcaps[iteration]:
		AlignmentOptions["pixelAlignmentLevelEndcaps"]  = PixelAlignmentLevelEndcaps[iteration]

		AlignmentOptions["runSolving"                ]  = False
		AlignmentOptions["runAccumulate"             ]  = True
		AlignmentOptions["WriteTFile"		     ] =  useTFile



                ############## finish set the Alignment Options ######################################################################


		if Datasets.containType("Customed") : 
			if DatasetsOptions["doDetailedTagsConfig"] :

				JOBNAMES["Customed"] = []
				outputTarFileNames["Customed"] = []
				Datasets.configDatasetTags("Customed")

				for i in range(len(DatasetsOptions["CustomedDatasetsNameList"])) :
					thisJobName   = "%s_Iter%02d_Accumulate_Customed_Part%02d_%s.py" % (preName, iteration, i, thisJobDir) 
					JOBNAMES["Customed"].append(thisJobName)				
					thisOutDSName = "%s.%s.Iter%02d_Accumulate_outDS_Customed_Part%02d_%s" % (GridOptions["userIDnum"], GridOptions["userIDname"], iteration, i, thisJobDir)
					outputTarFileNames["Customed"].append(thisOutDSName)
					os.system("echo %s >> %s\n" % (thisOutDSName, GridAccOutDS))
					os.system("echo %s >> %s\n" % (thisOutDSName, GridTotalAccOutDS))
					
					if 'Cos' in DatasetsOptions["CustomedDatasetsNameList"][i] : 
						RecoOptions["Cosmics"] = True
					else : 
						RecoOptions["Cosmics"] = False

					print " detDescrVersion Customed " ,  Datasets.detDescrVersion("Customed", i)

					if Datasets.detDescrVersion("Customed", i) : 
						RecoOptions["detectorDescription"] = Datasets.detDescrVersion("Customed", i)

					RecoScript       	                   = Datasets.recoScript("Customed", i)


					print " global tag Customed " , Datasets.globalTag("Customed", i)
					if Datasets.globalTag("Customed", i) :
						RecoOptions["globalTag"]  	   = Datasets.globalTag("Customed", i )

					currentjob = writeJobGrid(OutputPath       = OutputPath,
								  CosmicsBoff      = False,
								  CosmicsBon       = False,
								  iter             = iteration,
								  part             = "Accumulate",
								  JOBNAME          = thisJobName,
								  RecoOptions      = RecoOptions,
								  extraOptions     = extraOptions,
								  AlignmentOptions = AlignmentOptions,
								  RecoScript       = RecoScript,
								  AlignmentLevels  = alignLevels,
								  MonitoringScript = MonitoringScript
								 )
					currentjob.write()

			### not do detailed tags configuration, submit one job with long inDS list
			else : 
				print "....."
				Datasets.configDatasetTags("Customed")
				thisJobName   = "%s_Iter%02d_Accumulate_Customed_%s.py" % (preName, iteration, thisJobDir)
				JOBNAMES["Customed"] = thisJobName
				thisOutDSName = "%s.%s.Iter%02d_Accumulate_outDS_Customed_%s" % (GridOptions["userIDnum"], GridOptions["userIDname"], iteration, thisJobDir)
				outputTarFileNames["Customed"] = thisOutDSName
				os.system("echo %s >> %s\n" % (thisOutDSName, GridAccOutDS))
				os.system("echo %s >> %s\n" % (thisOutDSName, GridTotalAccOutDS))

				print " detDescrVersion Customed " ,  Datasets.detDescrVersion("Customed")
				if Datasets.detDescrVersion("Customed") :
					RecoOptions["detectorDescription"] = Datasets.detDescrVersion("Customed")
				RecoScript                                 = Datasets.recoScript("Customed")
				print " global tag Customed " , Datasets.globalTag("Customed")
				if Datasets.globalTag("Customed") :
					RecoOptions["globalTag"]           = Datasets.globalTag("Customed")


				currentjob = writeJobGrid(OutputPath       = OutputPath,
							  CosmicsBoff      = False,
							  CosmicsBon       = False,
							  iter             = iteration,
							  part             = "Accumulate",
							  JOBNAME          = thisJobName,
							  RecoOptions      = RecoOptions,
							  extraOptions     = extraOptions,
							  AlignmentOptions = AlignmentOptions,
							  RecoScript       = RecoScript,
							  AlignmentLevels  = alignLevels,
							  MonitoringScript = MonitoringScript
							 )
				currentjob.write()



		if (Datasets.containType("900GeV") or Datasets.containType("7TeV")) :

			if  GridOptions["ColCPUs"][iteration]:
				JOBNAMES["Collision"]           = "%s_Iter%02d_Accumulate_Collision_%s.py" % (preName, iteration, thisJobDir)
				outputTarFileNames["Collision"] = "%s.%s.Iter%02d_Accumulate_outDS_Collision_%s" % (GridOptions["userIDnum"], GridOptions["userIDname"], iteration, thisJobDir)
				print outputTarFileNames["Collision"]
				os.system("echo %s >> %s\n" % (outputTarFileNames["Collision"], GridAccOutDS))
				os.system("echo %s >> %s\n" % (outputTarFileNames["Collision"], GridTotalAccOutDS))


				if Datasets.detDescrVersion("Collision") : 
					RecoOptions["detectorDescription"] = Datasets.detDescrVersion("Collision")

				RecoScript                       	   = Datasets.recoScript("Collision")
				if Datasets.globalTag("Collision") :
					RecoOptions["globalTag"]  	   = Datasets.globalTag("Collision")

				currentjob = writeJobGrid(OutputPath       = OutputPath,
				    			  CosmicsBoff      = False,
							  CosmicsBon       = False,
							  iter             = iteration,
							  part             = "Accumulate",
							  JOBNAME          = JOBNAMES["Collision"],
							  RecoOptions      = RecoOptions,
							  extraOptions     = extraOptions,
							  AlignmentOptions = AlignmentOptions,
							  RecoScript       = RecoScript,
							  AlignmentLevels  = alignLevels,
							  MonitoringScript = MonitoringScript
							  )
				currentjob.write()


			if GridOptions["CosBonCPUs"][iteration]:
				JOBNAMES["CosmicsBon"]           = "%s_Iter%02d_Accumulate_CosBon_%s.py" % (preName, iteration, thisJobDir)
				outputTarFileNames["CosmicsBon"] = "%s.%s.Iter%02d_Accumulate_outDS_CosBon_%s" % (GridOptions["userIDnum"], GridOptions["userIDname"], iteration, thisJobDir)
				print outputTarFileNames["CosmicsBon"]
				os.system("echo %s >> %s\n" % (outputTarFileNames["CosmicsBon"], GridAccOutDS))
				os.system("echo %s >> %s\n" % (outputTarFileNames["CosmicsBon"], GridTotalAccOutDS))

				if Datasets.detDescrVersion("CosmicBon") : 
					RecoOptions["detectorDescription"] = Datasets.detDescrVersion("CosmicBon")
				RecoScript                       	   = Datasets.recoScript("CosmicBon")
				if Dataset.globalTag("CosmicBon"):
					RecoOptions["globalTag"] 	   = Datasets.globalTag("CosmicBon")

				currentjob = writeJobGrid(OutputPath       = OutputPath,
							  CosmicsBoff      = False,
							  CosmicsBon       = True,
							  iter             = iteration,
							  part             = "Accumulate",
							  JOBNAME          = JOBNAMES["CosmicsBon"],
							  RecoOptions      = RecoOptions,
							  extraOptions     = extraOptions,
							  AlignmentOptions = AlignmentOptions,
							  RecoScript       = RecoScript,
						          AlignmentLevels  = alignLevels,
							  MonitoringScript = MonitoringScript
							  )
				currentjob.write()


			if GridOptions["CosBoffCPUs"][iteration]:
				JOBNAMES["CosmicsBoff"]           = "%s_accumulate_Iter%02d_CosBoff_%s.py" % (preName, iteration, thisJobDir)
				outputTarFileNames["CosmicsBoff"] = "%s.%s.Iter%02d_Accumulate_outDS_CosBoff_%s" % (GridOptions["userIDnum"], GridOptions["userIDname"], iteration, thisJobDir)
				print outputTarFileNames["CosmicsBoff"]
				os.system("echo %s >> %s\n" % (outputTarFileNames["CosmicsBoff"], GridAccOutDS))
				os.system("echo %s >> %s\n" % (outputTarFileNames["CosmicsBoff"], GridTotalAccOutDS))


				if Datasets.detDescrVersion("CosmicBoff") : 				   
					RecoOptions["detectorDescription"] = Datasets.detDescrVersion("CosmicBoff")
				RecoScript 	                           = Datasets.recoScript("CosmicBoff")
				if Dataset.globalTag("CosmicBoff"):
					RecoOptions["globalTag"]  	   = Datasets.globalTag("CosmicBoff")

				currentjob = writeJobGrid(OutputPath       = OutputPath,
							  CosmicsBoff      = True,
							  CosmicsBon       = False,
							  iter             = iteration,
							  part             = "Accumulate",
							  JOBNAME          = JOBNAMES["CosmicsBoff"],
							  RecoOptions      = RecoOptions,
							  extraOptions     = extraOptions,
							  AlignmentOptions = AlignmentOptions,
				 			  RecoScript       = RecoScript,
							  AlignmentLevels  = alignLevels,
							  MonitoringScript = MonitoringScript
							  )
				currentjob.write()



		############ Write the job submission script ###############################################################################################  
		postfix = "_%s" % (thisJobDir)					       
		SCRIPTNAME = "%s_Iter%02d_Accumulate%s.lsf"       % (preName, iteration, postfix)

		if iteration == 0:
			thisInputPoolFile = ["initial_AlignmentConstants.root"]
		else:
			tmpStr = "Iter%02d_AlignmentConstants.root" % ( iteration-1 )
			thisInputPoolFile  = [tmpStr]


		if useTFile : 
			currentscript = writeScriptGridForTFile(OutputPath       = OutputPath,
								preName          = preName,
								iter             = iteration,
								part             = "Accumulate",
								ATHENAREL 	 = ATHENAREL,
								SCRIPTNAME       = SCRIPTNAME,
								Datasets         = Datasets,
								outDS            = outputTarFileNames,
								AccSubJobID      = GridAccSubJobID,
								AccSubInfo       = GridAccSubInfo,
								JOBNAMES         = JOBNAMES,
								thisJobDir       = thisJobDir,
								jobId            = -99,
								retryNo		 = 0,
								GridOptions      = GridOptions,
								AlignmentOptions = AlignmentOptions
								)


			# Write the Batch Script
			currentscript.write()
			# Send the Batch Script
			currentscript.send()
			#  End j loop
			# Wait for signal
			os.system("pbook -c 'sync()'")
			currentscript.wait(GridAccSubJobID)
			GridOptions["accumulateLibDS"] = currentscript.getGridOptions("accumulateLibDS")
			GridOptions["solveLibDS"     ] = currentscript.getGridOptions("solveLibDS")
	
			# enable automatically retry
			if (True == GridOptions['autoRetry']) and (True == currentscript.whetherRetry()) :
				currentscript.retry()




			
		else : 
			# Define the script for submiting Grid accumulation jobs
			currentscript = writeScriptGrid(OutputPath       = OutputPath,
							preName          = preName,
							iter             = iteration,
							part             = "Accumulate",
							CMTDIR           = CMTDIR,
							ATHENAREL        = ATHENAREL,
							TAGS             = TAGS,
							SCRIPTNAME       = SCRIPTNAME,
							Datasets         = Datasets,
							outDS            = outputTarFileNames,
							AccSubJobID      = GridAccSubJobID,
							AccSubInfo       = GridAccSubInfo,
							JOBNAMES         = JOBNAMES,
							thisJobDir       = thisJobDir,
							jobId            = -99,
							retryNo          = 0,
							GridOptions      = GridOptions,
							AlignmentOptions = AlignmentOptions
							)
			# Write the Batch Script
			currentscript.write()
			# Send the Batch Script
			currentscript.send()
			#  End j loop
			# Wait for signal
			os.system("pbook -c 'sync()'")
			currentscript.wait(GridAccSubJobID)
			GridOptions["accumulateLibDS"] = currentscript.getGridOptions("accumulateLibDS")
			GridOptions["solveLibDS"     ] = currentscript.getGridOptions("solveLibDS")

			# enable automatically retry
			if (True == GridOptions['autoRetry']) and (True == currentscript.whetherRetry()) :
				currentscript.retry()




#########################################################################################################################
#########################################################################################################################
	if doSolve:

		print "----------------------------------------------"
		print "  Solving Iter"+repr(iteration)
		print "----------------------------------------------"
		info.write('\n')
		info.write("----------------------------------------------\n")
		info.write("  Solving Iter %02d\n" % iteration)
      
		info.write("----------------------------------------------\n")

		postfix    = "%s" % (thisJobDir)

		PrefixName        = "Iter%02d_" % iteration
		JOBNAMES["Solve"] = "%s_Iter%02d_Solve_%s.py"  % (preName, iteration, postfix)
		SCRIPTNAME        = "%s_Iter%02d_Solve_%s.lsf" % (preName, iteration, postfix)

		print "Logs stored in %s/Iter%02d/logs/Iter%02dSolve_%s.log" % (OutputPath, iteration, iteration, postfix)

		##################################################################################################################
		outputTarFileNames["Solve"] = "%s.%s.Iter%02d_%s_Solve" % (GridOptions["userIDnum"], GridOptions["userIDname"], iteration, thisJobDir)
						
		print "outputSolvingDS: ", outputTarFileNames["Solve"]
						
		AlignmentOptions = {}

		if AlignmentMethod == "GX2":
			AlignmentOptions["runLocal"          ] = False
			AlignmentOptions["solveLocal"        ] = False
		else:
			AlignmentOptions["runLocal"          ] = True
			AlignmentOptions["solvingOption"     ] = 0

		AlignmentOptions["readConstantsFromPool"     ] = ReadAlignmentConstants

		if ReadAlignmentConstants:
			if iteration == 0:
				if False == os.path.isfile('%s' % inputAlignmentPoolFile):
					print "Error: the inputAlignmentPoolFile not exit ..."
					sys.exit()
				else : 
					os.system("cp %s initial_AlignmentConstants.root " % inputAlignmentPoolFile)
					AlignmentOptions["inputPoolFiles"] = ["initial_AlignmentConstants.root"]
					if "Batch" == GridOptions["runSolveMode"] or True == GridOptions["runSolveInTmpDir"] :
						tmpStr = "%s/initial_AlignmentConstants.root" % ( cpath )
						AlignmentOptions["inputPoolFiles"] = [tmpStr] 

			else:
				#tmpStr = "%s/Iter%02d/Iter%02d_AlignmentConstants.root" % ( str(OutputPath), iteration-1, iteration-1 )
				tmpStr = "Iter%02d_AlignmentConstants.root" % ( iteration-1 )
				if "Batch" == GridOptions["runSolveMode"] or True == GridOptions["runSolveInTmpDir"] :
					tmpStr = "%s/Iter%02d_AlignmentConstants.root" % ( cpath, iteration-1 )

				AlignmentOptions["inputPoolFiles"] = [tmpStr] 


		tmpStr = "Iter%02d_AlignmentConstants.root" % ( iteration )
		if "Batch" == GridOptions["runSolveMode"] or True == GridOptions["runSolveInTmpDir"] :
			tmpStr = "%s/Iter%02d_AlignmentConstants.root" % ( cpath, iteration )
                                
		AlignmentOptions["outputPoolFile"            ] = tmpStr

		AlignmentOptions["alignTRT"                  ] = AlignTRT[iteration]
		if AlignTRT[iteration]:
			AlignmentOptions["trtAlignmentLevel" ] = TRTAlignmentLevel[iteration]

		AlignmentOptions["alignSCT"                  ] = AlignSCT[iteration]
                #if AlignSCTBarrel[iteration]:
		AlignmentOptions["sctAlignmentLevel"         ] = SCTAlignmentLevel[iteration]
	
		AlignmentOptions["sctAlignBarrel"            ] = AlignSCTBarrel[iteration]
                #if AlignSCTBarrel[iteration]:
		AlignmentOptions["sctAlignmentLevelBarrel"   ] = SCTAlignmentLevelBarrel[iteration]
	
		AlignmentOptions["sctAlignEndcaps"           ] = AlignSCTEndcaps[iteration]
                #if AlignSCTEndcaps[iteration]:
		AlignmentOptions["sctAlignmentLevelEndcaps"  ] = SCTAlignmentLevelEndcaps[iteration]
		
		AlignmentOptions["alignPixel"                ] = AlignPixel[iteration]
                #if AlignSCTBarrel[iteration]:
		AlignmentOptions["pixelAlignmentLevel"       ] = PixelAlignmentLevel[iteration]
	
		AlignmentOptions["pixelAlignBarrel"          ] = AlignPixelBarrel[iteration]
                #if AlignPixelBarrel[iteration]:
		AlignmentOptions["pixelAlignmentLevelBarrel" ] = PixelAlignmentLevelBarrel[iteration]
		
		AlignmentOptions["pixelAlignEndcaps"         ] = AlignPixelEndcaps[iteration]
                #if AlignPixelEndcaps[iteration]:
		AlignmentOptions["pixelAlignmentLevelEndcaps"] = PixelAlignmentLevelEndcaps[iteration]

		# all other modes except Prun, such as run solving Locally, Batch and Grid, should dq2-get back the matrices and vectors 
		if GridOptions["runSolveMode"] != "Prun" :
			matrices, vectors, hitmaps             = mergeMatrix(OutputPath, iteration, GridAccOutDS, GridOptions) 

			print "matrices: ", matrices
			print "vector: "  , vectors
			print "hitmaps: " , hitmaps
		
			AlignmentOptions["inputMatrixFiles"]           = matrices
			AlignmentOptions["inputVectorFiles"] 	       = vectors
			AlignmentOptions["inputHitmapFiles"] 	       = hitmaps

			newms = []
			newvs = []
			newhs = []

			if GridOptions["runSolveMode"] == "Batch" :
				for item in matrices : 
					newitem                        = "%s/%s" % ( cpath , item )
					newms.append(newitem)
				for item in vectors :
					newitem                        = "%s/%s" % ( cpath , item )
					newvs.append(newitem)
				for item in matrices :
					newitem                        = "%s/%s" % ( cpath , item )
					newhs.append(newitem)

				AlignmentOptions["inputMatrixFiles"]   = newms
				AlignmentOptions["inputVectorFiles"]   = newvs
				AlignmentOptions["inputHitmapFiles"]   = newhs


		AlignmentOptions["WriteTFile"]                 = useTFile
		AlignmentOptions["runSolving"]                 = True
		AlignmentOptions["runAccumulate"]              = False

		RecoOptions = {}
		RecoOptions["numberOfEvents"]                  = 10


		if Datasets.containType("Customed") :
			if Datasets.globalTag("Customed") :
				RecoOptions["globalTag"]       = Datasets.globalTag("Customed")
			#RecoScript                             = Datasets.recoScript("Customed")
			RecoScript                             = "InDetAlignExample/BuildGeoForMatrixSolving.py"
			if Datasets.detDescrVersion("Customed") : 
				RecoOptions["detectorDescription"]     = Datasets.detDescrVersion("Customed")


		if (Datasets.containType("900GeV") or Datasets.containType("7TeV")) :
			if Datasets.globalTag("Collision") :
				RecoOptions["globalTag"]       = Datasets.globalTag("Collision")
			#RecoScript                             = Datasets.recoScript("Collision")
			RecoScript 			       = "InDetAlignExample/BuildGeoForMatrixSolving.py"
			if Datasets.detDescrVersion("Collision") :
				RecoOptions["detectorDescription"]     = Datasets.detDescrVersion("Collision")


		currentjob = writeJobGrid(OutputPath   = OutputPath,
		                      CosmicsBoff      = False,
	                              CosmicsBon       = False,
			              iter             = iteration,
			              part             = "Solve",
				      JOBNAME          = JOBNAMES["Solve"],
				      RecoOptions      = RecoOptions,
		                      extraOptions     = extraOptions,
				      AlignmentOptions = AlignmentOptions,
				      RecoScript       = RecoScript,
				      AlignmentLevels  = alignLevels,
				      MonitoringScript = MonitoringScript
			              )

		currentjob.write()


		if iteration == 0:
			thisInputPoolFile = ["initial_AlignmentConstants.root"]
			if GridOptions["runSolveMode"] != "Grid" :
				tmpStr = "%s/initial_AlignmentConstants.root" % ( cpath )
				thisInputPoolFile = [tmpStr]

		else:
			tmpStr = "Iter%02d_AlignmentConstants.root" % ( iteration-1 )
			if GridOptions["runSolveMode"] != "Grid" :
				tmpStr = "%s/Iter%02d_AlignmentConstants.root" % ( cpath, iteration-1 )

			thisInputPoolFile  = [tmpStr]



		if ( GridOptions["runSolveMode"] == "Grid" or GridOptions["runSolveMode"] == "Prun" ) :

			if useTFile :
				currentscript = writeScriptGridForTFile(OutputPath       = OutputPath,
									preName          = preName,
									iter             = iteration,
									part             = GridOptions["runSolveMode"],
									ATHENAREL 	 = ATHENAREL,
									SCRIPTNAME       = SCRIPTNAME,
									Datasets         = Datasets,
									outDS            = outputTarFileNames,
									AccSubJobID      = GridAccSubJobID,
									AccSubInfo       = GridAccSubInfo,
									JOBNAMES         = JOBNAMES,
									thisJobDir       = thisJobDir,
									jobId            = -99,
									retryNo          = 0,
									GridOptions      = GridOptions,
									AlignmentOptions = AlignmentOptions
									)
		    
				currentscript.write()
				currentscript.send()
				os.system("pbook -c 'sync()'")
				currentscript.wait(GridAccSubJobID)
				GridOptions["accumulateLibDS"] = currentscript.getGridOptions("accumulateLibDS")
				GridOptions["solveLibDS"     ] = currentscript.getGridOptions("solveLibDS")
	
				# enable automatically retry
				if (True == GridOptions['autoRetry']) and ( True == currentscript.whetherRetry()) :
					currentscript.retry()





			else : 

				currentscript = writeScriptGrid(OutputPath       = OutputPath,
								preName          = preName,
								iter             = iteration,
								part             = GridOptions["runSolveMode"],
								CMTDIR           = CMTDIR,
								ATHENAREL        = ATHENAREL,
								TAGS             = TAGS,
								SCRIPTNAME       = SCRIPTNAME,
								Datasets         = Datasets, 
								outDS            = outputTarFileNames,
								AccSubJobID      = GridAccSubJobID,
								AccSubInfo       = GridAccSubInfo,
								JOBNAMES         = JOBNAMES,
								thisJobDir       = thisJobDir,
								jobId            = -99,
								retryNo          = 0,
								GridOptions      = GridOptions,
								AlignmentOptions = AlignmentOptions
								)

				currentscript.write()
				currentscript.send()
				os.system("pbook -c 'sync()'")
				currentscript.wait(GridAccSubJobID)
				GridOptions["accumulateLibDS"] = currentscript.getGridOptions("accumulateLibDS")
				GridOptions["solveLibDS"     ] = currentscript.getGridOptions("solveLibDS")

				# enable automatically retry
				if (True == GridOptions['autoRetry']) and ( True == currentscript.whetherRetry()) :
					currentscript.retry()



		else : 
			currentscript = writeScriptAFS(iter           = iteration,
						       JOBNAME        = JOBNAMES["Solve"],
						       SCRIPTNAME     = SCRIPTNAME,
						       preName        = preName,
						       QUEUE          = QUEUE,
						       CMTDIR         = CMTDIR,
						       ATHENAREL      = ATHENAREL,
						       TAGS           = TAGS,
						       inputPoolFiles = thisInputPoolFile)
			currentscript.write()

			### do some preparation for running sloving locally, such as get back on file from Grid, modify the sloving jobOption
			#if self.Datasets.containType("Customed") :
			#	os.system("dq2-get -n 1 -H -V %s  %s/ \n "  %   ("fileForSolve" , self.Datasets.oneDatasetName("Customed")) )

			#else if ( self.Datasets.containType("900GeV") or self.Datasets.containType("7TeV") ) :
			#	os.system("dq2-get -n 1 -H -V %s  %s/ \n "  %   ("fileForSolve" , self.Datasets.oneDatasetName("Collision")) )

			#else :
			#	print " why do you come here ?? "

			#ret, out = commands.getstatusoutput("cat %s") % JOBNAMES["Solve"]
			#solveJO  = open('%s' , 'w') % JOBNAMES["Solve"]
			#lines    = out.split("\n")


			if "Local" == GridOptions["runSolveMode"] :
				if ( True == GridOptions["runSolveInTmpDir"] ) : 
					tmpstr = './%s.%s.*' % (GridOptions["userIDnum"], GridOptions["userIDname"])
					ret    = glob.glob(tmpstr)
					for tmpdir in ret :
						os.system("mv %s  %s "   % ( tmpdir, GridOptions["TmpWorkDir"])  )
					os.system(" cp *.py   %s "  % GridOptions["TmpWorkDir"] )
					os.system(" cp *.lsf  %s "  % GridOptions["TmpWorkDir"] )

					nowDir = os.getcwd()
					os.chdir(GridOptions["TmpWorkDir"]) 
					os.system(" rm PoolFileCatalog.*")
					currentscript.send("Local")

					os.system(" cp OldSiAlignment.txt OutputSiAlignment.txt alignlogfile.txt hitmap.bin hitmap.txt matrix.bin matrix.txt vector.bin vector.txt %s " % nowDir)
					os.chdir(nowDir)
					os.system(" mv OldSiAlignment.txt OutputSiAlignment.txt alignlogfile.txt hitmap.bin hitmap.txt matrix.bin matrix.txt vector.bin vector.txt %s/Iter%02d " % (OutputPath, iteration) )

				else : 
					currentscript.send("Local")
					os.system(" mv OldSiAlignment.txt OutputSiAlignment.txt alignlogfile.txt hitmap.bin hitmap.txt matrix.bin matrix.txt vector.bin vector.txt %s/Iter%02d " % (OutputPath, iteration) )

			if "Batch" == GridOptions["runSolveMode"] :   
				currentscript.send("Batch")
				currentscript.wait()


                #######################################################################################################
		'''
		if "doMonitoring" in extraOptions and extraOptions["doMonitoring"]==True:
			MERGEJOBNAME    = "%s_Iter%02dMergeMonitoring.py"  % (preName,iteration)
			MERGESCRIPTNAME = "%s_Iter%02dMergeMonitoring.lsf" % (preName,iteration)
			monitoringMerge = mergeMonitoringScript(OutputPath       = OutputPath
								,iter            = iteration
								,preName         = preName
								,CosmicsBoff     = CosmicsBoff[iteration]
								,CosmicsBon      = CosmicsBon[iteration]
								,Collision       = Collision[iteration]
								,CMTDIR          = CMTDIR
								,ATHENAREL       = ATHENAREL
								,TAGS            = TAGS
								,SCRIPTNAME      = MERGESCRIPTNAME
								,JOBNAME         = MERGEJOBNAME
								,GridAccOutDS    = GridAccOutDS)
			monitoringMerge.write()
			monitoringMerge.send()
		'''

                #######################################################################################################
		os.system("rm -f %s\n" % GridSolvingOutDS)
		os.system("echo %s >> %s\n" % (outputTarFileNames["Solve"], GridSolvingOutDS))

		prepareForNextIter(OutputPath, iteration, GridSolvingOutDS, GridOptions["runSolveMode"])

		tmpstr = './%s.%s.*' % (GridOptions["userIDnum"], GridOptions["userIDname"])
		ret    = glob.glob(tmpstr)
		for tmpdir in ret :
			os.system("mv %s  %s "   % ( tmpdir, GridOptions["TmpWorkDir"])  )

		#######################################################################################################

	else:

		print "-----------------------------------------------------\n"
		print " WARNING: Skipping the solving due to low statistics or set doSolve=False\n"
		print "-----------------------------------------------------\n"

		print "  Iteration %02d finished: %5.3f seconds \n" % (iteration,(time.time()-IterStartTime))
		print "-------------------------------------------------------"




if GridOptions["getMonitoringFiles"] == True : 

	ret, out = commands.getstatusoutput("cat %s" % GridTotalAccOutDS)
	print " GridTotalAccOutDS files : ",out
	lines = out.split('\n')
	for line in lines :
		temp    = line 
		start   = temp.find('Iter')
		end     = start + 6
		iter    = temp[ start : end ]
		thisDir = GridOptions["TmpWorkDir"] + "/" + iter 
		if os.path.isdir("%s/" % thisDir) == False :
			os.mkdir( "%s/"  % thisDir )
		
		MonitoringFilesList = []
		ValidationFilesList = []

		ret, out = commands.getstatusoutput("dq2-ls -f %s/ " % line)
		items = out.split('\n')
		for item in items : 
			strs = item.split()
			for str in strs :
				print " str: " , str
				if str.find("CombinedMonitoring") != -1 : 
					MonitoringFilesList.append(str)
				if str.find("TRKVAL")             != -1 : 
					ValidationFilesList.append(str)
		MonitoringFilesStr = ",".join(MonitoringFilesList)
		ValidationFilesStr = ",".join(ValidationFilesList)
		print " in iteration : " , iter
		print " MonitoringFilesStr for this iteration : " , MonitoringFilesStr
		print " ValidationFilesStr for this iteration : " , ValidationFilesStr
		os.system(" dq2-get -f %s -H %s/ -V %s/ "  % ( MonitoringFilesStr, thisDir, line ))
		os.system(" dq2-get -f %s -H %s/ -V %s/ "  % ( ValidationFilesStr, thisDir, line ))

		####### do the root files merging ###############################################
		os.system(" find %s/ -name \"*CombinedMonitoring*.root\"  > %s/list1 \n "  % (thisDir, thisDir) )
		os.system(" DQHistogramMerge.py  %s/list1  %s/TotalMonitoring.root  True \n "  % (thisDir, thisDir) )

		os.system(" find %s/ -name \"*TRKVAL*.root\"              > %s/list2 \n "  % (thisDir, thisDir) )
		#os.system(" DQHistogramMerge.py  %s/list2  %s/TotalTrkValidation.root True \n "  % (thisDir, thisDir) )
		os.system(" hadd  %s/TotalTrkValidation.root  %s/*TRKVAL*.root  \n "           % (thisDir, thisDir) )

#if MonitoringScript == True:
if doCompareMonitoring == True:
	print
	print "Comparing the Monitoring Files"
	print
	info.write('\n')
	info.write("Comparing the Monitoring Files \n" )
	info.write("\n")
	compareMonitoring = compareMonitoringScript(OutputPath = OutputPath
	 	                                   ,numIter    = Iterations
	 	                                   ,CMTDIR     = CMTDIR
	 	                                   ,ATHENAREL  = ATHENAREL
	 	                                   ,TAGS       = TAGS)
   
	compareMonitoring.write()
	compareMonitoring.send()
   
	print
	print "Processed %d iterations !!!" % Iterations
	print "  %5.3f  seconds" % (time.time()-StartTime)
   	
	info=open(OutputPath+"/info.txt",'a')
	info.write('\n')
	info.write("Processed %d iterations !!!\n" % Iterations)
	info.write("  %5.3f  seconds \n" % (time.time()-StartTime))
	





