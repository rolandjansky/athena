# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# =====================================================================
#  Classes for running ATLAS ID alignment jobs on Grid resources
#    Authors: Jike Wang      (jike.wang@cern.ch)
#             Song-Ming Wang (smwang@phys.sinica.edu.tw)
# =====================================================================


from threading import Thread
import os
import time,datetime
import commands, string
import sys


class runProcess(Thread):
	def __init__ (self,
		      jobOptions,
		      iter,
		      part,
		      OutputPath,
		      OutputLevel):
		Thread.__init__(self)
		self.jobOptions = jobOptions
		self.i = iter
		self.j = part
		self.OutputPath = OutputPath
		self.OutputLevel = OutputLevel
		self.status = -1

	def run(self):
		if self.j == -1:
			print "----------------------------------------------"
			print "  Running Iter%d - Solve in local machine" % (self.i)
			print "----------------------------------------------"
			process=os.popen("athena.py %s | tee %s/Iter%02d/logs/Iter%02dSolve.log" % (self.jobOptions, self.OutputPath, self.i, self.i))
		else:
			print "----------------------------------------------"
			print "  Running Iter%d - Part%02d in local machine" % (self.i, self.j)
			print "----------------------------------------------"
			print " - output path: %s/Iter%02d/%02d" % (self.OutputPath, self.i, self.j)
			process=os.popen("athena.py %s | tee %s/Iter%02d/logs/Iter%02dPart%02d.log" % (self.jobOptions, self.OutputPath, self.i, self.i ,self.j))

		print "Running..."
		while 1:
			line = process.readline()
			if self.OutputLevel == 'DEBUG':
				print line,
			if not line:
				self.status=1
				break
			



def prepareForThisIter(iteration, GridFileOptions):

	GridFileOptions["GridAccSubJobID"]  = 'GridAccSubJobID_%02d.txt'  % (iteration)
	GridFileOptions["GridAccSubInfo"]   = 'GridAccSubInfo_%02d.txt'   % (iteration)
	GridFileOptions["GridAccOutDS"]     = 'GridAccOutDS_%02d.txt'     % (iteration)
	GridFileOptions["GridSolvingOutDS"] = 'GridSolvingOutDS_%02d.txt' % (iteration)


			
class writeJobGrid:
	def __init__(self,
		     OutputPath,
		     CosmicsBoff,
		     CosmicsBon,
		     iter,
		     part,
		     JOBNAME,
		     RecoOptions      = {},
		     extraOptions     = {},
		     AlignmentOptions = {},
		     RecoScript       = "InDetAlignExample/NewTopOptions.py",
		     #AlignmentScript  = "InDetAlignExample/NewInDetAlignAlgSetup.py",
		     AlignmentScript  = "NewInDetAlignAlgSetup.py",
		     AlignmentLevels  = "InDetAlignExample/NewInDetAlignLevels.py",
		     MonitoringScript = "InDetRecExample/InDetMonitoringAlignment.py"
		     ):
		self.OutputPath       = OutputPath
		self.CosmicsBoff      = CosmicsBoff
		self.CosmicsBon       = CosmicsBon
		self.i                = iter
		self.part             = part
		self.JOBNAME          = JOBNAME
		self.RecoOptions      = RecoOptions
		self.extraOptions     = extraOptions
		self.RecoScript       = RecoScript
		self.AlignmentOptions = AlignmentOptions
		self.AlignmentScript  = AlignmentScript
		self.AlignmentLevels  = AlignmentLevels
		self.MonitoringScript = MonitoringScript
		
	def write(self):
		PrefixName="Iter%02d_" % self.i
		if self.part == "Solve":
			TempPath         = "%s/Iter%02d/"                 % (self.OutputPath, self.i)
		else:
			if self.CosmicsBoff:
				TempPath = "%s/Iter%02d/CosmicsBoff/" % (self.OutputPath, self.i)
			elif self.CosmicsBon:
				TempPath = "%s/Iter%02d/CosmicsBon/"   % (self.OutputPath, self.i)
			else:
				TempPath = "%s/Iter%02d/Collision/"       % (self.OutputPath, self.i)

		os.system('mkdir -p %s' % TempPath)
		os.system('mkdir -p %s' % TempPath+"/InDetRecExample")
		if self.MonitoringScript != "":
			os.system('ln -s %s %s' % (self.MonitoringScript,TempPath+"/InDetRecExample/InDetMonitoringAlignment.py") )

		# Make Monitoring script dir and file here

		#job=open(TempPath+self.JOBNAME,'w')
		job=open(self.JOBNAME,'w')
		job.write('##-------- Alignment Configuration --------------------\n')

		for option in self.AlignmentOptions:
			if type(self.AlignmentOptions[option]) is str:
				customoption = option + "\t=\t\"" +  self.AlignmentOptions[option]+'\"\n'
				job.write(customoption)
			else:
				customoption = option + "\t=\t"   +  str(self.AlignmentOptions[option])+'\n'
				job.write(customoption)
			
		job.write("\n")
		job.write('##-------- Loading the Alignment Levels --------------------\n')
		job.write('include("'+str(self.AlignmentLevels)+'") \n')
		job.write("\n")
		job.write('##-------- Reconstruction Configuration --------------------\n')
		
		for option in self.RecoOptions:
			if type(self.RecoOptions[option]) is str:
				customoption = option + "\t=\t\"" +  self.RecoOptions[option]+'\"\n'
				job.write(customoption)
			else:
				customoption = option + "\t=\t"   +  str(self.RecoOptions[option])+'\n'
				job.write(customoption)

		if len(self.extraOptions):
			job.write("\n")
			job.write('##-------- Extra Configuration --------------------\n')
		
		for option in self.extraOptions:
			if type(self.extraOptions[option]) is str:
				customoption = option + "\t=\t\"" +  self.extraOptions[option]+'\"\n'
				job.write(customoption)
			else:
				customoption = option + "\t=\t"   +  str(self.extraOptions[option])+'\n'
				job.write(customoption)

		job.write("\n")
		job.write('##-------- End of custom options -------------\n')

		# Need to write the InDetRec Options Here:
		job.write("\n")
		job.write('##-------- Load Reconstruction or purely build geometry ---------------\n')
		job.write('include("'+str(self.RecoScript)+'") \n')

		job.write("\n")
		job.write('##-------- Load Alignment --------------------\n')
		job.write('include("'+str(self.AlignmentScript)+'") \n')
		job.write("\n")
		job.close()

	





class writeScriptGridForTFile :

	def __init__(self,
		     OutputPath,
		     preName,
		     iter,
		     part,
		     ATHENAREL,
		     SCRIPTNAME,
		     Datasets,
		     outDS,
		     AccSubJobID,
		     AccSubInfo,
		     JOBNAMES,
		     thisJobDir,
		     jobId,
		     retryNo,
		     GridOptions = {},
		     AlignmentOptions = {}
       		     ):

			self.OutputPath       = OutputPath
			self.preName          = preName
			self.i                = iter
			self.part             = part
			self.ATHENAREL	      = ATHENAREL
			self.SCRIPTNAME       = SCRIPTNAME
			self.Datasets         =  Datasets
			self.outDS            = outDS
			self.AccSubJobID      = AccSubJobID
			self.AccSubInfo       = AccSubInfo     
			self.JOBNAMES         = JOBNAMES
			self.thisJobDir       = thisJobDir
			self.jobId            = -99
			self.retryNo	      = 0
			self.GridOptions      = GridOptions
			self.AlignmentOptions = AlignmentOptions



	def write(self) :


		print " start to create the job submit command line ...\n "   
		extOutFileStr    = "AlignmentTFile.root"
		extOutFileStrAcc = extOutFileStr

		if self.GridOptions["doMonitoring"] :
			extOutFileStrAcc = extOutFileStr + ",monitoring.root"
		if self.GridOptions["doTrkNtuple"]  :
			extOutFileStrAcc += ",TrkValidation.root"

		if self.part == "Accumulate":
			self.SCRIPTNAME
			script = open(self.SCRIPTNAME,'w')

			if self.Datasets.containType("Customed") :
				if self.Datasets.doDetailedTagsConfig() :
					for index in range(len(self.Datasets.namesList("Customed"))) :
						if ( "group" in self.GridOptions["userIDnum"] ) :
							scriptStr   = "pathena --official --voms=atlas:/atlas/det-indet/Role=production  %s  " % (self.JOBNAMES["Customed"][index])
						else :
							scriptStr   = "pathena  %s  " % (self.JOBNAMES["Customed"][index])

						if ("accumulateLibDS" in self.GridOptions) and (self.GridOptions["accumulateLibDS"] != "") :
							scriptStr += " --libDS %s "          % self.GridOptions["accumulateLibDS"]
							scriptStr += " --excludeFile %s "    % "*.cxx,*.h,*.o"

						if ("removeFileList" in self.GridOptions) and (self.GridOptions["removeFileList"]   != "") :
							scriptStr += " --removeFileList %s " % self.GridOptions["removeFileList"]

						if ("fileList"       in self.GridOptions) and (self.GridOptions["fileList"]         != "") :
							scriptStr += " --fileList %s "       % self.GridOptions["fileList"]

						if ("siteName"	     in self.GridOptions) and (self.GridOptions["siteName"]         != "") :
							scriptStr += " --site %s "           % self.GridOptions["siteName"]

						if ("excludedSite"   in self.GridOptions) and (self.GridOptions["excludedSite"]     != "") :
							scriptStr += " --excludedSite %s "   % self.GridOptions["excludedSite"]

						if ("SkipFiles"      in self.GridOptions) and (self.GridOptions["SkipFiles"]        != "") :
							scriptStr += " --nSkipFiles %s "     % self.GridOptions["SkipFiles"][self.i]


						if self.GridOptions["dbRelease"]    == "":
							scriptStr += " --dbRelease LATEST "
						else :
							scriptStr += " --dbRelease %s "    % self.GridOptions["dbRelease"]



						scriptStr += " --extFile "

						if (0 == self.i) : 
							if os.path.isfile("initial_AlignmentConstants.root") :
								scriptStr += "   initial_AlignmentConstants.root,Scaling.root  "   
							else : 
								scriptStr += " Scaling.root " 
						else :
							lastAlignConstant = "Iter%02d_AlignmentConstants.root"  %  (self.i-1)
							if os.path.isfile("%s"  % lastAlignConstant ) : 
								scriptStr += "  %s,Scaling.root "      % lastAlignConstant 
								
						
		
						if self.GridOptions["debugLevel"] == 4 :
							self.GridOptions["doNFilesPerJobSplit"] = False
							scriptStr += " --nEventsPerJob %s" % self.GridOptions["nEventsPerJob"]
  	
						if self.GridOptions["doNFilesPerJobSplit" ] :
							if (-1 != self.GridOptions["CustomedNFiles"][index]) :
								scriptStr += " --inDS %s --outDS %s --extOutFile %s --nFiles %d --nFilesPerJob %d --tmpDir %s  >& tmpSubJobInfo_Customed_%02d.txt ; \n" % ( self.Datasets.namesList("Customed")[index], self.outDS["Customed"][index], extOutFileStrAcc, self.GridOptions["CustomedNFiles"][index], self.GridOptions["CustomedNFPJ"][index], self.GridOptions["TmpWorkDir"], int(index) )
							else : 
								scriptStr += " --inDS %s --outDS %s --extOutFile %s  --nFilesPerJob %d --tmpDir %s  >& tmpSubJobInfo_Customed_%02d.txt ; \n" % ( self.Datasets.namesList("Customed")[index], self.outDS["Customed"][index], extOutFileStrAcc, self.GridOptions["CustomedNFPJ"][index], self.GridOptions["TmpWorkDir"], int(index) ) 
						else : 
							if (-1 != self.GridOptions["CustomedNFiles"][index]) :
								scriptStr += " --inDS %s --outDS %s --extOutFile %s --nFiles %d --split %d        --tmpDir %s  >& tmpSubJobInfo_Customed_%02d.txt ; \n" % ( self.Datasets.namesList("Customed")[index], self.outDS["Customed"][index], extOutFileStrAcc, self.GridOptions["CustomedNFiles"][index], self.GridOptions["CustomedCPUs"][index], self.GridOptions["TmpWorkDir"], int(index) )
							
							else :   
								scriptStr += " --inDS %s --outDS %s --extOutFile %s --nFiles %d --split %d        --tmpDir %s  >& tmpSubJobInfo_Customed_%02d.txt ; \n" % ( self.Datasets.namesList("Customed")[index], self.outDS["Customed"][index], extOutFileStrAcc, self.Datasets.nFiles("Customed")[index], self.GridOptions["CustomedCPUs"][index], self.GridOptions["TmpWorkDir"], int(index) )

							
						script.write(scriptStr)
							
					print " You are running alignment accumulation and using TFile for bookkeeping \n" 
					print " You are running on customed dataset and doing detailed tags configuration \n"
					print " the Grid job submission command line is : \n"
					print scriptStr


			if (self.Datasets.containType("900GeV") or self.Datasets.containType("7TeV")) :

				if self.GridOptions["ColCPUs"][self.i]:
					scriptStr   = "pathena  %s  " % (self.JOBNAMES["Collision"])
				if self.GridOptions["CosBonCPUs"][self.i]:
					scriptStr   = "pathena  %s  " % (self.JOBNAMES["CosBon"])
				if self.GridOptions["CosBoffCPUs"][self.i]:
					scriptStr   = "pathena  %s  " % (self.JOBNAMES["CosBoff"])


				if ("fileList" in self.GridOptions) and (self.GridOptions["fileList"]     != "") :
					scriptStr += "--fileList %s "     % self.GridOptions["fileList"]

				if ("siteName" in self.GridOptions ) and (self.GridOptions["siteName"]     != "") :
					scriptStr += "--site %s "         % self.GridOptions["siteName"]
						
				if ("excludedSite" in self.GridOptions) and (self.GridOptions["excludedSite"] != "") :
					scriptStr += "--excludedSite %s " % self.GridOptions["excludedSite"]
						                       
				if self.GridOptions["dbRelease"]    == "":
					scriptStr += "--dbRelease LATEST "
				else:
					scriptStr += "--dbRelease %s "    % self.GridOptions["dbRelease"]




				scriptStr += " --extFile "

				if (0 == self.i) : 
					if os.path.isfile("initial_AlignmentConstants.root") :
						scriptStr += "   initial_AlignmentConstants.root,Scaling.root  "   
					else : 
						scriptStr += " Scaling.root " 
				else :
					lastAlignConstant = "Iter%02d_AlignmentConstants.root"  %  (self.i-1)
					if os.path.isfile("%s"  % lastAlignConstant ) : 
						scriptStr += "  %s,Scaling.root "      % lastAlignConstant 




				if self.GridOptions["ColCPUs"][self.i] :
					if self.GridOptions["doNFilesPerJobSplit" ] :
						scriptStr_forCollision = scriptStr + " --inDS %s --outDS %s --extOutFile %s --nFiles %d --nFilesPerJob %d --tmpDir %s  >& tmpSubJobInfo_collision.txt ; \n" % ( self.Datasets.namesList("Collision"), self.outDS["Collision"], extOutFileStrAcc, self.Datasets.nFiles("Collision"), self.GridOptions["ColNFPJ"][self.i], self.GridOptions["TmpWorkDir"] )   
					else :
						scriptStr_forCollision = scriptStr + " --inDS %s --outDS %s --extOutFile %s --nFiles %d --split %d        --tmpDir %s  >& tmpSubJobInfo_collision.txt ; \n" % ( self.Datasets.namesList("Collision"), self.outDS["Collision"], extOutFileStrAcc, self.Datasets.nFiles("Collision"), self.GridOptions["ColCPUs"][self.i], self.GridOptions["TmpWorkDir"] )
					script.write(scriptStr_forCollision)


				if self.GridOptions["CosBonCPUs"][self.i]:
					scriptStr_forBon       = scriptStr + " --inDS %s --outDS %s --extOutFile %s --nFiles %d --split %d --tmpDir %s  >& tmpSubJobInfo_Bon.txt ;       \n" % ( self.Datasets.namesList("CosBon"),    self.outDS["CosBon"], extOutFileStrAcc, self.Datasets.nFiles("Bon"),       self.GridOptions["CosBonCPUs"][self.i],   self.GridOptions["TmpWorkDir"] )
					script.write(scriptStr_forBon)

				if self.GridOptions["CosBoffCPUs"][self.i]:
					scriptStr_forBoff      = scriptStr + " --inDS %s --outDS %s --extOutFile %s --nFiles %d --split %d --tmpDir %s  >& tmpSubJobInfo_Boff.txt ;      \n" % (self.Datasets.namesList("CosBoff"), self.outDS["CosBoff"],   extOutFileStrAcc, self.Datasets.nFiles("Boff"),      self.GridOptions["CosBoffCPUs"][self.i], self.GridOptions["TmpWorkDir"] )
					script.write(scriptStr_forBoff)
	
			script.close()


		elif self.part == "Prun" :
			print self.SCRIPTNAME
			script     = open(self.SCRIPTNAME,'w')

			prunSolve = open("prunSolve.py",'w')
			prunSolve.write('import os\nimport commands, string\nimport sys\n\n\n')
			prunSolve.write('inputTFiles = []\ninFiles = []\n\n\n')
			prunSolve.write('ret, out = commands.getstatusoutput(\"cat input1.txt\")\n')
			prunSolve.write('print \" the content of file input1.txt:  \", out \n')
			prunSolve.write('lines = out.split(\",\")\n')
			prunSolve.write('for line in lines:\n')
			prunSolve.write('\tif \"AlignmentTFile.root\" in str(line):\n\t\tinputTFiles.append(line)\n')
	
			tmpStrJO = "newSolveJO_Iter%02d.py" % self.i
			prunSolve.write('jofile = open( \'%s\' , \'w+\')\n' % tmpStrJO )

			tmpStr = ""

			if (0 == self.i) :
				if os.path.isfile("initial_AlignmentConstants.root") :
					tmpStr = "pool_insertFileToCatalog  initial_AlignmentConstants.root "


			else : 

				alignConstants = "Iter%02d_AlignmentConstants.root" % (self.i - 1)
				if os.path.isfile(alignConstants) :
					tmpStr = "pool_insertFileToCatalog  %s "  % alignConstants
				else :
					print "ALIGNMENT CONSTANTS %s NOT EXIST, WILL EXIT ANYHOW !!!  "  % alignConstants
					sys.exit()


			prunSolve.write('jofile.write(\'os.system(\\\"%s\\\")\\n\')\n'  % tmpStr)
			prunSolve.write('jofile.write(\"inputTFiles = \" + str(inputTFiles) + \'\\n\')\n')
	
			prunSolve.write('ff = open(\'%s\', \'r\')\n' % self.JOBNAMES["Solve"] )                               
			prunSolve.write('jofile.write(ff.read())\nff.close()\njofile.close()\n')
	
			prunSolve.write('os.system(\"athena.py %s \")\n'  % tmpStrJO )

			# debugging ...
			print "prunSolve.py: "
			os.system(" cat prunSolve.py ")
			print " newSolveJO_Iter%02d.py: " % self.i
			os.system(" cat %s " % tmpStrJO)


			extOutFileStrSol = ""

			if (    ("sctAlignmentLevel"          in self.AlignmentOptions and self.AlignmentOptions["sctAlignmentLevel"]          == 3 ) or 
				("sctAlignmentLevelBarrel"    in self.AlignmentOptions and self.AlignmentOptions["sctAlignmentLevelBarrel"]    == 3)  or 
				("sctAlignmentLevelEndcaps"   in self.AlignmentOptions and self.AlignmentOptions["sctAlignmentLevelEndcaps"]   == 3)  or 
				("pixelAlignmentLevel"        in self.AlignmentOptions and self.AlignmentOptions["pixelAlignmentLevel"]        == 3)  or 
				("pixelAlignmentLevelBarrel"  in self.AlignmentOptions and self.AlignmentOptions["pixelAlignmentLevelBarrel"]  == 3)  or 
				("pixelAlignmentLevelEndcaps" in self.AlignmentOptions and self.AlignmentOptions["pixelAlignmentLevelEndcaps"] == 3) ) :
				print "hmn, you are going to run L3 alignment, Eigen is being used, so no eigen value information!!! "
			 
			else : 

				if ( "writeEigenMat"      in self.GridOptions  and self.GridOptions["writeEigenMat"]    == True  ) and ( self.AlignmentOptions["runLocal"] == False ):
					extOutFileStrSol += "eigenvectors.bin,";
					extOutFileStrSol += "eigenvalues.bin," ;
			   
				if ( "writeEigenMatTxt"   in self.GridOptions  and self.GridOptions["writeEigenMatTxt"] == True  ) and ( self.AlignmentOptions["runLocal"] == False ):
					extOutFileStrSol += "eigenvectors.txt,";
					extOutFileStrSol += "eigenvalues.txt," ;


			extOutFileStrSol = extOutFileStrSol + extOutFileStr + ",Iter%02d_AlignmentConstants.root,OldSiAlignment.txt,OutputSiAlignment.txt,alignlogfile.txt" % (self.i)


			extFileStr = ""

			if (0 == self.i) :
				if os.path.isfile("initial_AlignmentConstants.root") :
					extFileStr = " initial_AlignmentConstants.root,Scaling.root "
				else : 
					extFileStr = " Scaling.root "

			else :
				lastAlignConstant = "Iter%02d_AlignmentConstants.root"  %  (self.i-1)
				if os.path.isfile("%s"  % lastAlignConstant ) :
					extFileStr = lastAlignConstant + ",Scaling.root "


			TNFiles = 0
			tmpInDS = []
			print " self.outDS : " , self.outDS

			print " self.outDS[Customed]: " , self.outDS["Customed"]
			for item in self.outDS["Customed"] :
				print "self.outDS[Customed] item:  " , item
				tmpInDS.append(item+"/")
				rec, nFiles = commands.getstatusoutput("dq2-ls -f %s | grep -i files | grep -i total | cut -c 13-" %  (item+"/") )
				print " nFiles of this outDS[Customed] :" , nFiles
				TNFiles += int(nFiles)
			solveInDS = ",".join(tmpInDS)

			#scriptStr = " prun --exec \" python prunSolve.py \" --writeInputToTxt IN:input1.txt --express --athenaTag %s,AtlasProduction --inDS %s --outputs %s --outDS %s --extFile %s --nJobs 1 --nFilesPerJob %d -v --maxNFilesPerJob 100000 " % (self.ATHENAREL, solveInDS , extOutFileStrSol, self.outDS["Solve"], extFileStr,  int(TNFiles) )
			if ( "group" in self.GridOptions["userIDnum"] ) : 
				scriptStr = " prun --exec \" python prunSolve.py \" --writeInputToTxt IN:input1.txt --express  --useAthenaPackages --match \"*AlignmentTFile.root*\"  --official --voms=atlas:/atlas/det-indet/Role=production --inDS %s --outputs %s --outDS %s --extFile %s --nJobs 1 --nFilesPerJob %d -v --maxNFilesPerJob 100000 " % ( solveInDS , extOutFileStrSol, self.outDS["Solve"], extFileStr,  int(TNFiles) )
			else : 
				scriptStr = " prun --exec \" python prunSolve.py \" --writeInputToTxt IN:input1.txt --express  --useAthenaPackages --match \"*AlignmentTFile.root*\"  --inDS %s --outputs %s --outDS %s --extFile %s --nJobs 1 --nFilesPerJob %d -v --maxNFilesPerJob 100000 " % ( solveInDS , extOutFileStrSol, self.outDS["Solve"], extFileStr,  int(TNFiles) )



			if  (True == self.GridOptions["reUseSolveLibDS"]) and  self.GridOptions["solveLibDS"]   != "":
				scriptStr += " --libDS %s "        % self.GridOptions["solveLibDS"]


			scriptStr += " --tmpDir %s >& tmpSubJobInfo_prunSolve.txt; \n" % self.GridOptions["TmpWorkDir"]

			script.write(scriptStr)
			script.close()


		else : 
			print "Hi, except accumulate and solve, where do you want to go?"



	def send(self) :

		os.system("pwd")
		os.system("chmod 777 %s" % self.SCRIPTNAME)

		print "----------------------------------------------"
		if self.part == "Accumulate":           
			print "Sending the %dth iteration accumulation job to grid at site ... %s" % (self.i, self.GridOptions["siteName"])
		 
			if self.Datasets.containType("Customed") :
				for i in range(len(self.Datasets.namesList("Customed"))) :   
					os.system(" rm -rf tmpSubJobInfo_Customed_%02d.txt" % i )
		     
			os.system(" rm -rf  tmpSubJobInfo_collision.txt tmpSubJobInfo_Bon.txt tmpSubJobInfo_Boff.txt ")
			os.system("source %s" % self.SCRIPTNAME)
		     

			if self.Datasets.containType("Customed") :
				if self.Datasets.doDetailedTagsConfig() :
					for i in range(len(self.Datasets.namesList("Customed"))) :
						os.system("cat tmpSubJobInfo_Customed_%02d.txt                 >> %s\n" % (i, self.AccSubInfo))
						os.system("cat tmpSubJobInfo_Customed_%02d.txt | grep -i JobID >> %s\n" % (i, self.AccSubJobID))
				else :
					print " you don't plan to do detailed tags configuration for every dataset, so will submit only one job with long inDS: "
			     	                                       
			     	
			if (self.Datasets.containType("900GeV") or self.Datasets.containType("7TeV")) :
				if self.GridOptions["ColCPUs"][self.i]:
					os.system("cat tmpSubJobInfo_collision.txt                 >> %s\n" % (self.AccSubInfo))
					os.system("cat tmpSubJobInfo_collision.txt | grep -i JobID >> %s\n" % (self.AccSubJobID))
				 	
				if self.GridOptions["CosBonCPUs"][self.i]:
					os.system("cat tmpSubJobInfo_Bon.txt                       >> %s\n" % (self.AccSubInfo))
					os.system("cat tmpSubJobInfo_Bon.txt       | grep -i JobID >> %s\n" % (self.AccSubJobID))
				   	
				if self.GridOptions["CosBoffCPUs"][self.i]:
					os.system("cat tmpSubJobInfo_Boff.txt                      >> %s\n" % (self.AccSubInfo))
					os.system("cat tmpSubJobInfo_Boff.txt      | grep -i JobID >> %s\n" % (self.AccSubJobID))


		elif self.part == "Prun" :

			print "  Sending the %dth iteration prun matrix solving job to grid site ... %s" % (self.i, self.GridOptions["siteName"])
			os.system("rm -f tmpSubJobInfo_prunSolve.txt")
			os.system("source %s" % self.SCRIPTNAME)
			os.system("cat tmpSubJobInfo_prunSolve.txt                 >> %s\n" % (self.AccSubInfo))
			os.system("cat tmpSubJobInfo_prunSolve.txt | grep -i JobID >> %s\n" % (self.AccSubJobID))
	
			print "----------------------------------------------"
		
		else : 

			print "Hi, where do you want to go?"




	def wait(self,logfilename):
		print "Pathena wait()" 
	
		if self.jobId == -99:
			print "logiflename: ",logfilename
			ret, out = commands.getstatusoutput("cat "+logfilename)
			lines = out.split('\n')
             
			# looping over all the job IDs
			for line in lines:
				items = line.split()
				if len(items)>0 and items[0]=="JobID" :
					self.jobId = int(items[2])
					print "jobId = ",self.jobId

				# check status of each job ID
				# while self.bjobs() == 0:
				while self.bjobs() != 1:       
					print " waiting for jobID ",self.jobId,"..."
					time.sleep(300)





	def bjobs(self) :
		if self.jobId == -99:
			print "need jobId"
			sys.exit(3)
	
		print "Pathena bjobs(), jobId: ",self.jobId
		jobId = self.jobId
		bjobstring = "pbook -c 'show(" + str(jobId) + ")'"
		print bjobstring
		ret, out = commands.getstatusoutput(bjobstring)
		print "statusoutput: ",out
		for line in out.split("\n") :
			items_1 = line.split()
			if len(items_1)>0 and items_1[0] != "jobStatus" :
				continue
	
			if len(items_1)>2 :
				if items_1[2] == "frozen" :
					print "jobStatus: " , items_1[2]
	
					### search the libDS #######   
					for line2 in out.split("\n") :
						print " line2: " , line2
						items_2 = line2.split()
						if items_2[0] == "libDS" :
							break 
	
					if self.part == "Accumulate" and self.GridOptions["accumulateLibDS"] == "" :   
						self.GridOptions["accumulateLibDS"] = items_2[2]
						print " self.GridOptions accumulateLibDS: " , self.GridOptions["accumulateLibDS"]
	
					if (self.part == "Grid" or self.part == "Prun") and self.GridOptions["solveLibDS"] == "":
						self.GridOptions["solveLibDS"     ] = items_2[2]                               
						print " self.GridOptions solveLibDS: "      , self.GridOptions["solveLibDS"]
	                                  
					return 1
				else :
					print "jobStatus: ",items_1[2]
					return 0
	


	def whetherRetry(self) :               
		nfailed   = 0
		nfinished = 0
		if self.jobId == -99 :
			print "need jobId, JOB SUBMISSION FAILED!!!, check the log files"
			sys.exit(3)
	
		print "Pathena bjobs(), jobId: ",self.jobId
		jobId = self.jobId
		bjobstring = "pbook -c 'show(" + str(jobId) + ")'"
		print bjobstring
		ret, out = commands.getstatusoutput(bjobstring)
               
		lines  = out.split("\n")
		nlines = len(lines)
		print " nlines: " , nlines
	
		for i in range(0, nlines) :
			items = lines[i].split()
			print " items: " , items
			if "failed"   in items :
				nfailed   = int(items[-1])
			if "finished" in items :
				nfinished = int(items[-1])
				# Hi, just stop, next line will cause the loop crash ...
				break


		if (self.retryNo == 0) and (self.i == 0) : 
			if ( 0 == (nfailed + nfinished - 1) ) : 
				successRatio = -1 
			else : 
				successRatio = float(nfinished - 1)/(nfailed + nfinished - 1) 
		else : 
			successRatio = float(nfinished)/(nfailed + nfinished) 


		print "the success ratio: ", successRatio
		if successRatio >= self.GridOptions["successRatioCut"] :
			print "The success ratio is higher than the cut, will not retry ---"
			return False
		else :
			print "The success ratio is lower than the cut, will retry ---"
			return True
	




	def retry(self) :
		jobId = self.jobId
		retrystring = "pbook -c 'retry(" + str(jobId) + ")'"   
		ret, out    = commands.getstatusoutput(retrystring)
		print " out1: " , out
		#self.jobId =  self.jobId + 2

		## get the new JobID ## 
		for line in out.split("\n") : 
			items = line.split() 
			nitems = len(items) 
			for i in range(0, nitems) : 
				if items[i] == "New" : 
					jobstring = items[i+1].split("=") 
					self.jobId = int(jobstring[-1]) 
					print "new JobID: " , self.jobId 
					break 
		self.retryNo = self.retryNo + 1 



		while self.bjobs() != 1 :
			print " waiting for the first retry jobID " , self.jobId , "..."
			time.sleep(300)
	
		if self.whetherRetry() :
			jobId = self.jobId
			retrystring = "pbook -c 'retry(" + str(jobId) + ")'"
			ret, out    = commands.getstatusoutput(retrystring)
			print " out2: " , out
	
			#self.jobId = self.jobId + 2

			## get the new JobID ## 
			for line in out.split("\n") : 
				items = line.split() 
				nitems = len(items) 
				for i in range(0, nitems) : 
					if items[i] == "New" : 
						jobstring = items[i+1].split("=") 
						self.jobId = int(jobstring[-1]) 
						print "new JobID: " , self.jobId 
						break 
			self.retryNo = self.retryNo + 1



			while self.bjobs() != 1 :
				print " waiting for the second retry jobID " , self.jobId,"..."
				time.sleep(300)
	
			
	
	def getGridOptions(self, option = "") :
		return self.GridOptions[option]
	
	def setGridOptions(self, option1 = "", option2 = "") :
		self.GridOptions[option1] = option2









class writeScriptGrid :		
	def __init__(self,
		     OutputPath,
		     preName,
		     iter,
		     part,
		     CMTDIR,
		     ATHENAREL,
		     TAGS,
		     SCRIPTNAME,
		     Datasets,
		     outDS,
		     AccSubJobID,
		     AccSubInfo,
		     JOBNAMES,
		     thisJobDir,
		     jobId,
		     GridOptions = {},
		     AlignmentOptions = {}
		     ):
		self.OutputPath       = OutputPath
		self.preName          = preName
		self.i                = iter
		self.part             = part
		self.CMTDIR           = CMTDIR
		self.ATHENAREL        = ATHENAREL
		self.TAGS             = TAGS
		self.SCRIPTNAME       = SCRIPTNAME
		self.Datasets         =  Datasets
		self.outDS            = outDS
		self.AccSubJobID      = AccSubJobID 
		self.AccSubInfo       = AccSubInfo	
		self.JOBNAMES         = JOBNAMES
		self.thisJobDir       = thisJobDir
		self.jobId            = -99
		self.GridOptions      = GridOptions
		self.AlignmentOptions = AlignmentOptions


	def write(self):


		extOutFile = []

		extOutFile.append("hitmap.bin")
		extOutFile.append("matrix.bin")
		extOutFile.append("vector.bin")
			

		if (    ("sctAlignmentLevel"          in self.AlignmentOptions and self.AlignmentOptions["sctAlignmentLevel"]          == 3 ) or 
			("sctAlignmentLevelBarrel"    in self.AlignmentOptions and self.AlignmentOptions["sctAlignmentLevelBarrel"]    == 3)  or 
			("sctAlignmentLevelEndcaps"   in self.AlignmentOptions and self.AlignmentOptions["sctAlignmentLevelEndcaps"]   == 3)  or 
			("pixelAlignmentLevel"        in self.AlignmentOptions and self.AlignmentOptions["pixelAlignmentLevel"]        == 3)  or 
			("pixelAlignmentLevelBarrel"  in self.AlignmentOptions and self.AlignmentOptions["pixelAlignmentLevelBarrel"]  == 3)  or 
			("pixelAlignmentLevelEndcaps" in self.AlignmentOptions and self.AlignmentOptions["pixelAlignmentLevelEndcaps"] == 3) ) : 
			#extOutFile.append("vector.txt")
			print "hmmn, you plan to do Level 3 alignment ... "

			### put the three flags writeMatrixFileTxt,writeHitmapTxt,writeEigenMatTxt in the GridOptions, not in the AlignmentOptions
			if ( "writeMatrixFileTxt" in self.GridOptions  and self.GridOptions["writeMatrixFileTxt"] == True  ) : 
				extOutFile.append("matrix.txt")
				extOutFile.append("vector.txt")
			if ( "writeHitmapTxt"     in self.GridOptions  and self.GridOptions["writeHitmapTxt"]     == True  ) :        
				extOutFile.append("hitmap.txt")

		else :
			extOutFile.append("matrix.txt")
			extOutFile.append("hitmap.txt")				
			if ( "writeMatrixFileTxt" in self.GridOptions  and self.GridOptions["writeMatrixFileTxt"] == False  ) :        
				extOutFile.remove("matrix.txt")
			if ( "writeHitmapTxt"     in self.GridOptions  and self.GridOptions["writeHitmapTxt"    ] == False  ) :        
				extOutFile.remove("hitmap.txt")

			
		extOutFileStr = ",".join(extOutFile)


		if self.GridOptions["doMonitoring"] :
			extOutFileStrAcc = extOutFileStr + ",monitoring.root"
		if self.GridOptions["doTrkNtuple"]  :
			extOutFileStrAcc += ",TrkValidation.root"


		if self.part == "Accumulate":
			print self.SCRIPTNAME
			script = open(self.SCRIPTNAME,'w')

			if self.Datasets.containType("Customed") : 
				if self.Datasets.doDetailedTagsConfig() : 
					for index in range(len(self.Datasets.namesList("Customed"))) :

						if ("group" in self.GridOptions["userIDnum"] ) :
							scriptStr   = "pathena  --official --voms=atlas:/atlas/det-indet/Role=production %s  " % (self.JOBNAMES["Customed"][index])
						else : 
							scriptStr   = "pathena  %s  " % (self.JOBNAMES["Customed"][index])
						
						if self.GridOptions["accumulateLibDS"]   != "":
							scriptStr += " --libDS %s "        % self.GridOptions["accumulateLibDS"]
							scriptStr += " --excludeFile %s "  % "*.cxx,*.h,*.o"

						if  "removeFileList" in self.GridOptions : 
							if self.GridOptions["removeFileList"] != "" :
								scriptStr += " --removeFileList %s " % self.GridOptions["removeFileList"] 

						if self.GridOptions["fileList"]     != "":
							scriptStr += " --fileList %s "     % self.GridOptions["fileList"]
						if self.GridOptions["siteName"]     != "":
							scriptStr += " --site %s "         % self.GridOptions["siteName"]
						if self.GridOptions["excludedSite"] != "":
							scriptStr += " --excludedSite %s " % self.GridOptions["excludedSite"]
						if self.GridOptions["dbRelease"]    == "":
							scriptStr += " --dbRelease LATEST "
						else:
							scriptStr += " --dbRelease %s "    % self.GridOptions["dbRelease"]


						#if self.GridOptions["reUseAccumulateLibDS"] :
						if (0 == self.i) and os.path.isfile("initial_AlignmentConstants.root") : 
							scriptStr += " --extFile  initial_AlignmentConstants.root,Scaling.root  "    
						else :
							lastAlignConstant = "Iter%02d_AlignmentConstants.root"  %  (self.i-1)
							if os.path.isfile("%s"  % lastAlignConstant ) :  
								scriptStr += " --extFile %s,Scaling.root  "      % lastAlignConstant    
						
						#print index
						#print self.Datasets.namesList("Customed")[index]
						#print self.outDS["Customed"][index]
						#print self.Datasets.nFiles("Customed")[index]
						#print self.GridOptions["CustomedCPUs"][index]


						if self.GridOptions["debugLevel"] == 4 :
							self.GridOptions["doNFilesPerJobSplit"] = False 
							scriptStr += " --nEventsPerJob %s" % self.GridOptions["nEventsPerJob"]

						if self.GridOptions["doNFilesPerJobSplit" ] :
							if (-1 != self.GridOptions["CustomedNFiles"][index]) : 
								scriptStr += " --inDS %s --outDS %s --extOutFile %s --nFiles %d --nFilesPerJob %d --tmpDir %s  >& tmpSubJobInfo_Customed_%02d.txt ; \n" % ( self.Datasets.namesList("Customed")[index], self.outDS["Customed"][index], extOutFileStrAcc, self.GridOptions["CustomedNFiles"][index], self.GridOptions["CustomedNFPJ"][index], self.GridOptions["TmpWorkDir"], int(index) )

							else : 
								scriptStr += " --inDS %s --outDS %s --extOutFile %s  --nFilesPerJob %d --tmpDir %s  >& tmpSubJobInfo_Customed_%02d.txt ; \n" % ( self.Datasets.namesList("Customed")[index], self.outDS["Customed"][index], extOutFileStrAcc, self.GridOptions["CustomedNFPJ"][index], self.GridOptions["TmpWorkDir"], int(index) )

						else : 
							if (-1 != self.GridOptions["CustomedNFiles"][index]) :
								scriptStr += " --inDS %s --outDS %s --extOutFile %s --nFiles %d --split %d        --tmpDir %s  >& tmpSubJobInfo_Customed_%02d.txt ; \n" % ( self.Datasets.namesList("Customed")[index], self.outDS["Customed"][index], extOutFileStrAcc, self.GridOptions["CustomedNFiles"][index], self.GridOptions["CustomedCPUs"][index], self.GridOptions["TmpWorkDir"], int(index) )

							else :   
								scriptStr += " --inDS %s --outDS %s --extOutFile %s --nFiles %d --split %d        --tmpDir %s  >& tmpSubJobInfo_Customed_%02d.txt ; \n" % ( self.Datasets.namesList("Customed")[index], self.outDS["Customed"][index], extOutFileStrAcc, self.Datasets.nFiles("Customed")[index], self.GridOptions["CustomedCPUs"][index], self.GridOptions["TmpWorkDir"], int(index) )
						script.write(scriptStr)

				else : 
					print " you don't plan to do detailed tags configuration for every dataset, so will submit only one job with a LONG inDS: "

					if ( "group" in self.GridOptions["userIDnum"] ) :
						scriptStr   = "pathena  --official --voms=atlas:/atlas/det-indet/Role=production  %s  " % (self.JOBNAMES["Customed"][0])
					else :
						scriptStr   = "pathena  %s  " % (self.JOBNAMES["Customed"][0])

					#if self.GridOptions["accumulateLibDS"]   != "":
					#	scriptStr += " --libDS %s "        % self.GridOptions["accumulateLibDS"]
					if self.GridOptions["fileList"]     != "":
						scriptStr += " --fileList %s "     % self.GridOptions["fileList"]
					if self.GridOptions["siteName"]     != "":
						scriptStr += " --site %s "         % self.GridOptions["siteName"]
					if self.GridOptions["excludedSite"] != "":
						scriptStr += " --excludedSite %s " % self.GridOptions["excludedSite"]
					if self.GridOptions["dbRelease"]    == "":
						scriptStr += " --dbRelease LATEST "
					else:
						scriptStr += " --dbRelease %s "    % self.GridOptions["dbRelease"]


					#if self.GridOptions["reUseAccumulateLibDS"] :
					if (0 == self.i) and os.path.isfile("initial_AlignmentConstants.root") :
						scriptStr += " --extFile  initial_AlignmentConstants.root "      
					else :
						lastAlignConstant = "Iter%02d_AlignmentConstants.root"  %  (self.i-1)
						if os.path.isfile("%s"  % lastAlignConstant ) :
							scriptStr += " --extFile %s "      % lastAlignConstant


					if self.GridOptions["doNFilesPerJobSplit" ] :
						scriptStr += " --inDS %s --outDS %s --extOutFile %s --nFiles %d --nFilesPerJob %d --tmpDir %s  >& tmpSubJobInfo_Customed.txt ; \n" % ( self.Datasets.namesList("Customed"), self.outDS["Customed"][0], extOutFileStrAcc, self.Datasets.nFiles("Customed"), self.GridOptions["CustomedNFPJ"][0], self.GridOptions["TmpWorkDir"] )
					else : 
						scriptStr += " --inDS %s --outDS %s --extOutFile %s --nFiles %d --split %d        --tmpDir %s  >& tmpSubJobInfo_Customed.txt ; \n" % ( self.Datasets.namesList("Customed"), self.outDS["Customed"][0], extOutFileStrAcc, self.Datasets.nFiles("Customed"), self.GridOptions["CustomedCPUs"][0], self.GridOptions["TmpWorkDir"] )
					script.write(scriptStr)


			######################################################################################################################################		
			if (self.Datasets.containType("900GeV") or self.Datasets.containType("7TeV")) :

				if self.GridOptions["ColCPUs"][self.i]:
					scriptStr   = "pathena  %s  " % (self.JOBNAMES["Collision"])
				if self.GridOptions["CosBonCPUs"][self.i]:
					scriptStr   = "pathena  %s  " % (self.JOBNAMES["CosBon"])
				if self.GridOptions["CosBoffCPUs"][self.i]:
					scriptStr   = "pathena  %s  " % (self.JOBNAMES["CosBoff"])

				#if self.GridOptions["accumulateLibDS"]   != "":
				#	scriptStr += "--libDS %s "        % self.GridOptions["accumulateLibDS"]

				if self.GridOptions["fileList"]     != "":
					scriptStr += "--fileList %s "     % self.GridOptions["fileList"]

				if self.GridOptions["siteName"]     != "":
					scriptStr += "--site %s "         % self.GridOptions["siteName"]

				if self.GridOptions["excludedSite"] != "":
					scriptStr += "--excludedSite %s " % self.GridOptions["excludedSite"]
			
				if self.GridOptions["dbRelease"]    == "":
					scriptStr += "--dbRelease LATEST "
				else:
					scriptStr += "--dbRelease %s "    % self.GridOptions["dbRelease"] 
		

				#if self.GridOptions["reUseAccumulateLibDS"] :

				if (0 == self.i) and os.path.isfile("initial_AlignmentConstants.root") :
					scriptStr += " --extFile  initial_AlignmentConstants.root "
				else :                                         
					lastAlignConstant = "Iter%02d_AlignmentConstants.root"  %  (self.i-1)
					if os.path.isfile("%s"  % lastAlignConstant ) :    
						scriptStr += " --extFile %s "      % lastAlignConstant


        	                ########################## for submitting different topology jobs ########################################################################
				if self.GridOptions["ColCPUs"][self.i] :
					if self.GridOptions["doNFilesPerJobSplit" ] :
						scriptStr_forCollision = scriptStr + " --inDS %s --outDS %s --extOutFile %s --nFiles %d --nFilesPerJob %d --tmpDir %s  >& tmpSubJobInfo_collision.txt ; \n" % ( self.Datasets.namesList("Collision"), self.outDS["Collision"], extOutFileStrAcc, self.Datasets.nFiles("Collision"), self.GridOptions["ColNFPJ"][self.i], self.GridOptions["TmpWorkDir"] )   
					else : 
						scriptStr_forCollision = scriptStr + " --inDS %s --outDS %s --extOutFile %s --nFiles %d --split %d        --tmpDir %s  >& tmpSubJobInfo_collision.txt ; \n" % ( self.Datasets.namesList("Collision"), self.outDS["Collision"], extOutFileStrAcc, self.Datasets.nFiles("Collision"), self.GridOptions["ColCPUs"][self.i], self.GridOptions["TmpWorkDir"] )
					script.write(scriptStr_forCollision)

				if self.GridOptions["CosBonCPUs"][self.i]:
					scriptStr_forBon       = scriptStr + " --inDS %s --outDS %s --extOutFile %s --nFiles %d --split %d --tmpDir %s  >& tmpSubJobInfo_Bon.txt ;       \n" % ( self.Datasets.namesList("CosBon"),    self.outDS["CosBon"], extOutFileStrAcc, self.Datasets.nFiles("Bon"),       self.GridOptions["CosBonCPUs"][self.i],   self.GridOptions["TmpWorkDir"] )
					script.write(scriptStr_forBon)

				if self.GridOptions["CosBoffCPUs"][self.i]:
					scriptStr_forBoff      = scriptStr + " --inDS %s --outDS %s --extOutFile %s --nFiles %d --split %d --tmpDir %s  >& tmpSubJobInfo_Boff.txt ;      \n" % (self.Datasets.namesList("CosBoff"), self.outDS["CosBoff"],   extOutFileStrAcc, self.Datasets.nFiles("Boff"),      self.GridOptions["CosBoffCPUs"][self.i], self.GridOptions["TmpWorkDir"] )
					script.write(scriptStr_forBoff)
			script.close()



		elif self.part == "Grid" :
			print self.SCRIPTNAME
			script     = open(self.SCRIPTNAME,'w')
		
			extOutFileStrSol = extOutFileStr + ",Iter%02d_AlignmentConstants.root,OldSiAlignment.txt,OutputSiAlignment.txt,mycool.db,alignlogfile.txt" % (self.i)
 			#  should consider more details about how to get one files and the corresponding geometry/condition tags, below is just temporary solution 
			'''
			if self.Datasets.containType("Customed") :
				scriptStr  = "pathena %s --inDS %s --fileList %s --outDS %s --extOutFile %s --nEventsPerFile 1 --nEventsPerJob 1 --nFilesPerJob 1  " % (self.JOBNAMES["Solve"], self.Datasets.oneDatasetName("Customed"), self.Datasets.oneFileName("Customed"),   self.outDS["Solve"], extOutFileStrSol)

			if (self.Datasets.containType("900GeV") or self.Datasets.containType("7TeV")) :
				scriptStr  = "pathena %s --inDS %s --fileList %s --outDS %s --extOutFile %s --nEventsPerFile 1 --nEventsPerJob 1 --nFilesPerJob 1  " % (self.JOBNAMES["Solve"], self.Datasets.oneDatasetName("Collision"), self.Datasets.oneFileName("Collision"), self.outDS["Solve"], extOutFileStrSol)
			'''
 
			if ( "group" in self.GridOptions["userIDnum"] ) :
				scriptStr  = "pathena  --official --voms=atlas:/atlas/det-indet/Role=production  %s  --outDS %s --extOutFile %s --nEventsPerFile 1 --nEventsPerJob 1 --nFilesPerJob 1  " % (self.JOBNAMES["Solve"], self.outDS["Solve"], extOutFileStrSol)
			      
			else :
				scriptStr  = "pathena %s  --outDS %s --extOutFile %s --nEventsPerFile 1 --nEventsPerJob 1 --nFilesPerJob 1  " % (self.JOBNAMES["Solve"], self.outDS["Solve"], extOutFileStrSol)


			if ( True == self.GridOptions["reUseSolveLibDS"] ) and self.GridOptions["solveLibDS"]   != "":
				scriptStr += "--libDS %s "        % self.GridOptions["solveLibDS"]

			if self.GridOptions["siteName"]     != "":
				scriptStr += "--site %s "         % self.GridOptions["siteName"]

			if self.GridOptions["excludedSite"] != "":
				scriptStr += "--excludedSite %s " % self.GridOptions["excludedSite"]

			if self.GridOptions["dbRelease"]    == "":
				scriptStr += "--dbRelease LATEST "
			else:
				scriptStr += "--dbRelease %s "    % self.GridOptions["dbRelease"]



			if (0 == self.i) and os.path.isfile("initial_AlignmentConstants.root") :
				scriptStr += " --extFile  initial_AlignmentConstants.root,"
			else :
				lastAlignConstant = "Iter%02d_AlignmentConstants.root"  %  (self.i-1)
				if os.path.isfile("%s"  % lastAlignConstant ) :
					scriptStr += " --extFile %s,"      % lastAlignConstant

			scriptStr += "*.bin "

			if self.GridOptions["reUseSolveLibDS"] : 
				#ret, out = commands.getstatusoutput("cat %s" % GridAccOutDS)
				ret, out = commands.getstatusoutput("cat GridAccOutDS.txt")
				print "out: ",out
				lines = out.split('\n')
				extFiles = []
				 

				#fileName = open('GridAccOutDS.txt', 'r+')				
 				#ret, out = commands.getstatusoutput('fileName.read()')
				#lines = out.split('\n')
				#extFiles = []

				#for line in lines :
				#	print "line: ", line
				#	thisStr = "%s/\*.bin" % line
				#	extFiles.append(thisStr)
				#print " thisStr: " , thisStr
				#extFileStr = ",".join(extFiles)
				#print " extFileStr: " , extFileStr
				#scriptStr += " --extFile %s "     %  extFileStr



			scriptStr += " --tmpDir %s >& tmpSubJobInfo_solve.txt; \n" % self.GridOptions["TmpWorkDir"]

			script.write(scriptStr)
			script.close()

		elif self.part == "Prun" :
			print self.SCRIPTNAME
			script     = open(self.SCRIPTNAME,'w')


			prunSolve = open("prunSolve.py",'w')
			prunSolve.write('import os\nimport commands, string\nimport sys\n\n\n')
			prunSolve.write('inputHitmapFiles = []\ninputMatrixFiles = []\ninputVectorFiles = []\ninFiles = []\n\n\n')
			prunSolve.write('ret, out = commands.getstatusoutput(\"cat input1.txt\")\n')
			prunSolve.write('print \" the content of file input1.txt:  \", out \n')
			prunSolve.write('lines = out.split(\",\")\n')
			prunSolve.write('for line in lines:\n')
			prunSolve.write('\tif \"hitmap.bin\" in str(line):\n\t\tif \"dcap://\" in str(line) :\n\t\t\tos.system(\" dccp %s ./\" % str(line) )\n\t\t\tlineHitmap = line.split(\'/\')[-1]\n\t\t\tprint \'file name retrived after splitting dcap address is %s \' % lineHitmap\n\t\t\tinputHitmapFiles.append(lineHitmap)\n\t\telse : \n\t\t\tinputHitmapFiles.append(line)\n')
			prunSolve.write('\tif \"vector.bin\" in str(line):\n\t\tif \"dcap://\" in str(line) :\n\t\t\tos.system(\" dccp %s ./\" % str(line) )\n\t\t\tlineVector = line.split(\'/\')[-1]\n\t\t\tprint \'file name retrived after splitting dcap address is %s \' % lineVector\n\t\t\tinputVectorFiles.append(lineVector)\n\t\telse : \n\t\t\tinputVectorFiles.append(line)\n')
			prunSolve.write('\tif \"matrix.bin\" in str(line):\n\t\tif \"dcap://\" in str(line) :\n\t\t\tos.system(\" dccp %s ./\" % str(line) )\n\t\t\tlineMatrix = line.split(\'/\')[-1]\n\t\t\tprint \'file name retrived after splitting dcap address is %s \' % lineMatrix\n\t\t\tinputMatrixFiles.append(lineMatrix)\n\t\telse : \n\t\t\tinputMatrixFiles.append(line)\n')

			tmpStrJO = "newSolveJO_Iter%02d.py" % self.i
			prunSolve.write('jofile = open( \'%s\' , \'w+\')\n' % tmpStrJO )


			tmpStr = ""
			if (0 == self.i) : 
				if os.path.isfile("initial_AlignmentConstants.root") :
					tmpStr = "pool_insertFileToCatalog  initial_AlignmentConstants.root "
			else :
				alignConstants = "Iter%02d_AlignmentConstants.root" % (self.i - 1)
				if os.path.isfile(alignConstants) :
					tmpStr = "pool_insertFileToCatalog  %s "  % alignConstants
				else : 
					print "ALIGNMENT CONSTANTS %s NOT EXIST, WILL EXIT ANYHOW !!!  "  % alignConstants
					sys.exit()


			prunSolve.write('jofile.write(\'os.system(\\\"%s\\\")\\n\')\n'  % tmpStr)
			prunSolve.write('jofile.write(\"inputHitmapFiles = \" + str(inputHitmapFiles) + \'\\n\')\n')
			prunSolve.write('jofile.write(\"inputVectorFiles = \" + str(inputVectorFiles) + \'\\n\')\n')
			prunSolve.write('jofile.write(\"inputMatrixFiles = \" + str(inputMatrixFiles) + \'\\n\')\n')

			prunSolve.write('ff = open(\'%s\', \'r\')\n' % self.JOBNAMES["Solve"] )                               
			prunSolve.write('jofile.write(ff.read())\nff.close()\njofile.close()\n')

			prunSolve.write('os.system(\"athena.py %s \")\n'  % tmpStrJO )

			# debugging ...
			print "prunSolve.py: "
			os.system(" cat prunSolve.py ")
			print " newSolveJO_Iter%02d.py: " % self.i
			os.system(" cat %s " % tmpStrJO)


			#extOutFileStrSol = "prunSolve.py,newSolveJO_Iter%02d.py,"  % (self.i)	
			extOutFileStrSol = ""

			if (    ("sctAlignmentLevel"          in self.AlignmentOptions and self.AlignmentOptions["sctAlignmentLevel"]          == 3 ) or 
				("sctAlignmentLevelBarrel"    in self.AlignmentOptions and self.AlignmentOptions["sctAlignmentLevelBarrel"]    == 3)  or 
				("sctAlignmentLevelEndcaps"   in self.AlignmentOptions and self.AlignmentOptions["sctAlignmentLevelEndcaps"]   == 3)  or 
				("pixelAlignmentLevel"        in self.AlignmentOptions and self.AlignmentOptions["pixelAlignmentLevel"]        == 3)  or 
				("pixelAlignmentLevelBarrel"  in self.AlignmentOptions and self.AlignmentOptions["pixelAlignmentLevelBarrel"]  == 3)  or 
				("pixelAlignmentLevelEndcaps" in self.AlignmentOptions and self.AlignmentOptions["pixelAlignmentLevelEndcaps"] == 3) ) : 
				print "hmn, you are going to run L3 alignment, Eigen is going to be used, so no eigen value information!!! "

			else : 
				if ( "writeEigenMat"      in self.GridOptions  and self.GridOptions["writeEigenMat"]    == True  ) and ( self.AlignmentOptions["runLocal"] == False ):
					extOutFileStrSol += "eigenvectors.bin,";
					extOutFileStrSol += "eigenvalues.bin," ;

				if ( "writeEigenMatTxt"   in self.GridOptions  and self.GridOptions["writeEigenMatTxt"] == True  ) and ( self.AlignmentOptions["runLocal"] == False ):
					extOutFileStrSol += "eigenvectors.txt,";
					extOutFileStrSol += "eigenvalues.txt," ;

			extOutFileStrSol = extOutFileStrSol + extOutFileStr + ",Iter%02d_AlignmentConstants.root,OldSiAlignment.txt,OutputSiAlignment.txt,mycool.db,alignlogfile.txt" % (self.i)

			extFileStr = ""
			if (0 == self.i) and os.path.isfile("initial_AlignmentConstants.root") :
				extFileStr = " initial_AlignmentConstants.root "
			else :
				lastAlignConstant = "Iter%02d_AlignmentConstants.root"  %  (self.i-1)

				if os.path.isfile("%s"  % lastAlignConstant ) :
					extFileStr = lastAlignConstant



			TNFiles = 0
			tmpInDS = []
			print " self.outDS : " , self.outDS

			print " self.outDS[Customed]: " , self.outDS["Customed"]
			for item in self.outDS["Customed"] : 
				print "self.outDS[Customed] item:  " , item
				tmpInDS.append(item+"/")
				rec, nFiles = commands.getstatusoutput("dq2-ls -f %s | grep -i files | grep -i total | cut -c 13-" %  (item+"/") )
				print " nFiles of this outDS[Customed] :" , nFiles
				TNFiles += int(nFiles)

			solveInDS = ",".join(tmpInDS) 
			#scriptStr = " prun --exec \" python prunSolve.py \" --writeInputToTxt IN:input1.txt --athenaTag 15.8.0,AtlasProduction --inDS %s --outputs %s --outDS %s --extFile %s --nJobs 1 --nFilesPerJob %d -v --maxNFilesPerJob 100000 " % ( (self.outDS["Customed"][0]+"/") , extOutFileStrSol, self.outDS["Solve"], extFileStr,  int(float(nFiles)) )

			if ( "group" in self.GridOptions["userIDnum"] ) :
				scriptStr = " prun --exec \" python prunSolve.py \" --writeInputToTxt IN:input1.txt --official --voms=atlas:/atlas/det-indet/Role=production --express --athenaTag %s,AtlasProduction --inDS %s --outputs %s --outDS %s --extFile %s --nJobs 1 --nFilesPerJob %d -v --maxNFilesPerJob 100000 " % (self.ATHENAREL, solveInDS , extOutFileStrSol, self.outDS["Solve"], extFileStr,  int(TNFiles) )

			else : 
				scriptStr = " prun --exec \" python prunSolve.py \" --writeInputToTxt IN:input1.txt --express --athenaTag %s,AtlasProduction --inDS %s --outputs %s --outDS %s --extFile %s --nJobs 1 --nFilesPerJob %d -v --maxNFilesPerJob 100000 " % (self.ATHENAREL, solveInDS , extOutFileStrSol, self.outDS["Solve"], extFileStr,  int(TNFiles) )

			if  (True == self.GridOptions["reUseSolveLibDS"]) and  self.GridOptions["solveLibDS"]   != "":
				scriptStr += " --libDS %s "        % self.GridOptions["solveLibDS"]

			scriptStr += " --tmpDir %s >& tmpSubJobInfo_prunSolve.txt; \n" % self.GridOptions["TmpWorkDir"]

			script.write(scriptStr)
			script.close()


		else:
		   print "Hi, except accumulate and solve, where do you want to go?"
			
			
	def send(self):
		os.system("pwd")
		os.system("chmod 777 %s" % self.SCRIPTNAME)

		print "----------------------------------------------"
		if self.part == "Accumulate":		
			print "Sending %s_Iter%02d accumulation job to grid at site ... %s" % (self.preName, self.i, self.GridOptions["siteName"])

			if self.Datasets.containType("Customed") :
				for i in range(len(self.Datasets.namesList("Customed"))) :   
					os.system(" rm -rf tmpSubJobInfo_Customed_%02d.txt" % i )

			os.system(" rm -rf  tmpSubJobInfo_collision.txt tmpSubJobInfo_Bon.txt tmpSubJobInfo_Boff.txt ")
			os.system("source %s" % self.SCRIPTNAME)

			if self.Datasets.containType("Customed") :
				if self.Datasets.doDetailedTagsConfig() :
					for i in range(len(self.Datasets.namesList("Customed"))) :
						os.system("cat tmpSubJobInfo_Customed_%02d.txt                 >> %s\n" % (i, self.AccSubInfo))
						os.system("cat tmpSubJobInfo_Customed_%02d.txt | grep -i JobID >> %s\n" % (i, self.AccSubJobID))
				else : 
					print " you don't plan to do detailed tags configuration for every dataset, so will submit only one job with long inDS: "
					

			if (self.Datasets.containType("900GeV") or self.Datasets.containType("7TeV")) :
				if self.GridOptions["ColCPUs"][self.i]:
					os.system("cat tmpSubJobInfo_collision.txt                 >> %s\n" % (self.AccSubInfo))
					os.system("cat tmpSubJobInfo_collision.txt | grep -i JobID >> %s\n" % (self.AccSubJobID))

				if self.GridOptions["CosBonCPUs"][self.i]:
					os.system("cat tmpSubJobInfo_Bon.txt                       >> %s\n" % (self.AccSubInfo))
					os.system("cat tmpSubJobInfo_Bon.txt       | grep -i JobID >> %s\n" % (self.AccSubJobID))

				if self.GridOptions["CosBoffCPUs"][self.i]:
					os.system("cat tmpSubJobInfo_Boff.txt                      >> %s\n" % (self.AccSubInfo))
					os.system("cat tmpSubJobInfo_Boff.txt      | grep -i JobID >> %s\n" % (self.AccSubJobID))


		elif self.part == "Grid" :

			print "  Sending %s_Iter%02d matrix solving job to grid site %s" % (self.preName, self.i, self.GridOptions["siteName"])
			os.system("rm -f tmpSubJobInfo_solve.txt")
			os.system("source %s" % self.SCRIPTNAME)
			os.system("cat tmpSubJobInfo_solve.txt                 >> %s\n" % (self.AccSubInfo))
			os.system("cat tmpSubJobInfo_solve.txt | grep -i JobID >> %s\n" % (self.AccSubJobID))

			print "----------------------------------------------"

		elif self.part == "Prun" :

			print "  Sending %s_Iter%02d prun matrix solving job to grid site %s" % (self.preName, self.i, self.GridOptions["siteName"])
			os.system("rm -f tmpSubJobInfo_prunSolve.txt")
			os.system("source %s" % self.SCRIPTNAME)
			os.system("cat tmpSubJobInfo_prunSolve.txt                 >> %s\n" % (self.AccSubInfo))
			os.system("cat tmpSubJobInfo_prunSolve.txt | grep -i JobID >> %s\n" % (self.AccSubJobID))

			print "----------------------------------------------"


		else:
			print "Hi, where do you want to go?"




	# take this part from Muon alignment in Jobs.py
	def wait(self,logfilename):
		print "Pathena wait()" 

		if self.jobId == -99:
			print "logiflename: ",logfilename
			ret, out = commands.getstatusoutput("cat "+logfilename)
			lines = out.split('\n')
			
			# looping over all the job IDs
			for line in lines:
				items = line.split()
				if len(items)>0 and items[0]=="JobID" :
					self.jobId = int(items[2])
					print "jobId = ",self.jobId

				# check status of each job ID
				# while self.bjobs() == 0:
				while self.bjobs() != 1:	
					print " waiting for jobID ",self.jobId,"..."
					time.sleep(300)



	def bjobs(self) :
		if self.jobId == -99:
			print "need jobId"
			sys.exit(3)

		print "Pathena bjobs(), jobId: ",self.jobId
		jobId = self.jobId
		bjobstring = "pbook -c 'show(" + str(jobId) + ")'"
		print bjobstring
		ret, out = commands.getstatusoutput(bjobstring)
		print "statusoutput: ",out
		for line in out.split("\n") :
			items_1 = line.split()
			if len(items_1)>0 and items_1[0] != "jobStatus" :
				continue

			if len(items_1)>2 :
				if items_1[2] == "frozen" :
					print "jobStatus: " , items_1[2]

					### search the libDS #######	
					for line2 in out.split("\n") : 
						print " line2: " , line2 
						items_2 = line2.split()
						if items_2[0] == "libDS" :
							break 

					if self.part == "Accumulate" and self.GridOptions["accumulateLibDS"] == "" : 	
						self.GridOptions["accumulateLibDS"] = items_2[2] 
						print " self.GridOptions accumulateLibDS: " , self.GridOptions["accumulateLibDS"]

					if (self.part == "Grid" or self.part == "Prun") and self.GridOptions["solveLibDS"] == "":
						self.GridOptions["solveLibDS"     ] = items_2[2]				
						print " self.GridOptions solveLibDS: "      , self.GridOptions["solveLibDS"]

					
					return 1
				else :
					print "jobStatus: ",items_1[2]
					return 0




	def whetherRetry(self) :	  	
		nfailed   = 0
		nfinished = 0
		if self.jobId == -99 :
			print "need jobId"
			sys.exit(3)

		print "Pathena bjobs(), jobId: ",self.jobId
		jobId = self.jobId
		bjobstring = "pbook -c 'show(" + str(jobId) + ")'"
		print bjobstring
		ret, out = commands.getstatusoutput(bjobstring)
		
		lines  = out.split("\n")
		nlines = len(lines)
		print " nlines: " , nlines

		for i in range(0, nlines) : 
			items = lines[i].split()
			print " items: " , items
			if "failed"   in items :
				nfailed   = int(items[-1])
			if "finished" in items :
				nfinished = int(items[-1])
				# Hi, just stop, next line will cause the loop crash ...
				break

		if (self.retryNo == 0) and (self.i == 0) : 
			if ( 0 == (nfailed + nfinished - 1) ) : 
				successRatio = -1 
			else : 
				successRatio = float(nfinished - 1)/(nfailed + nfinished - 1) 
		else : 
			successRatio = float(nfinished)/(nfailed + nfinished) 



		print "the success ratio: ", successRatio
		if successRatio >= self.GridOptions["successRatioCut"] :
			print "The success ratio is higher than the cut, will not retry ---"
			return False
		else :
			print "The success ratio is lower than the cut, will retry ---"
			return True



	def retry(self) :
		jobId = self.jobId
		retrystring = "pbook -c 'retry(" + str(jobId) + ")'"	
		ret, out    = commands.getstatusoutput(retrystring)
		print " out1: " , out 
		#self.jobId =  self.jobId + 2

		## get the new JobID ## 
		for line in out.split("\n") : 
			items = line.split() 
			nitems = len(items) 
			for i in range(0, nitems) : 
				if items[i] == "New" : 
					jobstring = items[i+1].split("=") 
					self.jobId = int(jobstring[-1]) 
					print "new JobID: " , self.jobId 
					break 
		self.retryNo = self.retryNo + 1



		while self.bjobs() != 1 :
			print " waiting for the first retry jobID " , self.jobId , "..."
			time.sleep(300)

		if self.whetherRetry() :
			jobId = self.jobId
			retrystring = "pbook -c 'retry(" + str(jobId) + ")'"
			ret, out    = commands.getstatusoutput(retrystring)
			print " out2: " , out

			#self.jobId =self.jobId + 2


			## get the new JobID ## 
			for line in out.split("\n") : 
				items = line.split() 
				nitems = len(items) 
				for i in range(0, nitems) : 
					if items[i] == "New" : 
						jobstring = items[i+1].split("=") 
						self.jobId = int(jobstring[-1]) 
						print "new JobID: " , self.jobId 
						break 
			self.retryNo = self.retryNo + 1 


			while self.bjobs() != 1 :
				print " waiting for the second retry jobID " , self.jobId,"..."
				time.sleep(300)





	def getGridOptions(self, option = "") : 
		return self.GridOptions[option]

	def setGridOptions(self, option1 = "", option2 = "") : 
		self.GridOptions[option1] = option2





class writeScriptAFS : 
	def __init__(self,
		     iter,
		     JOBNAME,
		     SCRIPTNAME,
		     preName,
		     QUEUE          = "8nh",
		     CMTDIR         = "",
		     ATHENAREL      = "",
		     TAGS           = "",
		     inputPoolFiles = ""
		     ):

		self.i              = iter
		self.JOBNAME        = JOBNAME
		self.SCRIPTNAME     = SCRIPTNAME
		self.preName        = preName
		self.QUEUE          = QUEUE
		self.CMTDIR         = CMTDIR
		self.ATHENAREL      = ATHENAREL
		self.TAGS           = TAGS
		self.inputPoolFiles = inputPoolFiles

	def write(self) : 
		script = open(self.SCRIPTNAME,'w')

		script.write("\n")
		script.write("#   setup the environment \n")
		script.write("source %s/setup.sh -tag=%s,%s \n" % (self.CMTDIR, self.ATHENAREL, self.TAGS))
		for file in self.inputPoolFiles:
			if "atlasdatadisk" in file: 
				script.write("export STAGE_SVCCLASS=atldata\n")
				break
		for file in self.inputPoolFiles:
			if "DAQ" in file:
				script.write("export STAGE_SVCCLASS=atlcal\n")
				break

		for file in self.inputPoolFiles:
			if 'ESD' in file or 'AlignmentConstants' in file:
				script.write("pool_insertFileToCatalog "+ file + " \n")

		nowPath = os.getcwd()
		print"current path: ", nowPath

		script.write("athena %s \n" % (nowPath + "/" + self.JOBNAME) )
		script.close()


	def send(self, runmode) :
		os.system("chmod +x %s" % self.SCRIPTNAME)
		if "Local" == runmode : 
			print "Running Iter%02dSolve job locally ..." % (self.i)
			os.system("sh %s | tee Iter%02dSolveLocally.log \n" % (self.SCRIPTNAME, self.i))

		if "Batch" == runmode :
			print "Submitting Iter%02dSolve job to queue %s ..." % (self.i, self.QUEUE)
			os.system("bsub -q %s  %s" % (self.QUEUE, self.SCRIPTNAME) )


	def wait(self) : 
		print "Processing in lxbatch..."
		time.sleep(60)
		while os.popen('bjobs -w').read().find(self.preName) != -1:
			time.sleep(30)



def prepareForNextIter(OutputPath, iteration, GridSolvingOutDS, runSolveMode):

	if ( "Grid"  == runSolveMode or "Prun" == runSolveMode ) :
		print "GridSolveOutDS = ", (GridSolvingOutDS)
		ret, outDS = commands.getstatusoutput("cat %s" % GridSolvingOutDS)
		print "solve outDS: ",outDS

		ret, out = commands.getstatusoutput("dq2-ls -f %s/ " % outDS)
		rootlist = []

		print "out: ",out
		lines = out.split('\n')
		for line in lines :
			items = line.split()
			for item in items :
				print " item : " , item
				if item.find("AlignmentConstants.root") != -1 :
					rootlist.append(item)
		rootstr = ",".join(rootlist)
		print "rootstr : " , rootstr
		os.system("dq2-get -f %s -H %s/ -V %s/ "  % ( rootstr, outDS, outDS ) )
	

		jobstring2  = "mv %s/*.Iter%02d_AlignmentConstants.root  Iter%02d_AlignmentConstants.root\n\n" % \
			(outDS, iteration, iteration)
	 
		'''
		jobstring2 += "mv %s/*.OldSiAlignment.txt     %s/Iter%02d/OldSiAlignment.txt\n\n" % \
			(outDS, OutputPath, iteration)
		jobstring2 += "mv %s/*.OutputSiAlignment.txt  %s/Iter%02d/OutputSiAlignment.txt\n\n" %  \
			(outDS, OutputPath, iteration)
		jobstring2 += "mv %s/*.mycool.db   %s/Iter%02d/mycool.db\n\n" %  \
			(outDS, OutputPath, iteration)
		jobstring2 += "mv %s/*.vector.txt  %s/Iter%02d/vector.txt\n\n" %  \
			(outDS, OutputPath, iteration)
		jobstring2 += "mv %s/*.vector.bin  %s/Iter%02d/vector.bin\n\n" %  \
			(outDS, OutputPath, iteration)
		jobstring2 += "mv %s/*.matrix.txt  %s/Iter%02d/matrix.txt\n\n" %  \
			(outDS, OutputPath, iteration)
		jobstring2 += "mv %s/*.matrix.bin  %s/Iter%02d/matrix.bin\n\n" %  \
			(outDS, OutputPath, iteration)
		jobstring2 += "mv %s/*.hitmap.txt  %s/Iter%02d/hitmap.txt\n\n" %  \
			(outDS, OutputPath, iteration)
		jobstring2 += "mv %s/*.hitmap.bin  %s/Iter%02d/hitmap.bin\n\n" %  \
			(outDS, OutputPath, iteration)
		jobstring2 += "mv %s/*.alignlogfile.txt  %s/Iter%02d/alignlogfile.txt\n\n" %  \
			(outDS, OutputPath, iteration)
		jobstring2 += "mv %s/*.log.tgz  %s/Iter%02d/\n\n" % (outDS, OutputPath, iteration)     
		# move the merged total monitoring file into every iteration directory
		jobstring2 += "mv TotalMonitoring.root %s/Iter%02d/\n\n" % (OutputPath, iteration)
		'''
		os.system(jobstring2)

	else : 	
		jobstring3 = ""
		jobstring3 += "mv ./OldSiAlignment.txt     %s/Iter%02d/OldSiAlignment.txt\n" %     ( OutputPath, iteration)
		jobstring3 += "mv ./OutputSiAlignment.txt  %s/Iter%02d/OutputSiAlignment.txt\n" %  ( OutputPath, iteration)
		jobstring3 += "mv ./mycool.db              %s/Iter%02d/mycool.db\n" %              ( OutputPath, iteration)
		jobstring3 += "mv ./vector.txt             %s/Iter%02d/vector.txt\n" %             ( OutputPath, iteration)
		jobstring3 += "mv ./vector.bin             %s/Iter%02d/vector.bin\n" %             ( OutputPath, iteration)
		jobstring3 += "mv ./matrix.txt             %s/Iter%02d/matrix.txt\n" %             ( OutputPath, iteration)
		jobstring3 += "mv ./matrix.bin             %s/Iter%02d/matrix.bin\n" %             ( OutputPath, iteration)
		jobstring3 += "mv ./hitmap.txt             %s/Iter%02d/hitmap.txt\n" %             ( OutputPath, iteration)
		jobstring3 += "mv ./hitmap.bin             %s/Iter%02d/hitmap.bin\n" %             ( OutputPath, iteration)
		jobstring3 += "mv ./alignlogfile.txt       %s/Iter%02d/alignlogfile.txt\n" %       ( OutputPath, iteration)
		os.system(jobstring3)







def mergeMatrix(OutputPath, iteration, GridAccOutDS, GridOptions):

	matrixlist = []
	vectorlist = []
	hitmaplist = []

	print "GridAccOutDS = ", (GridAccOutDS)

	ret, out = commands.getstatusoutput("cat %s" % GridAccOutDS)
	print "out: ",out
	lines=out.split('\n')

	nowDir = os.getcwd()
	os.chdir(GridOptions["TmpWorkDir"])

	# looping over all output dataset names
	for line in lines:
		items = line.split()
		if len(items)>0 :
			outDS = items[0]
			print "when merging matrixes and vectors, this outDS name : ", outDS
			binlist = []
			ret, out = commands.getstatusoutput("dq2-ls -f %s/ " % outDS)
			print " dq2-ls -f, out : ", out
			lines = out.split('\n')
			for line in lines :
				items = line.split()
				for item in items :
					print " item : " , item
					if item.find(".bin") != -1 :
						binlist.append(item)
			binstr = ",".join(binlist)
			print "binary files string : " , binstr
			os.system("dq2-get -f %s -H %s/ -V %s/ "  % ( binstr, outDS, outDS ))


			jobstr2 = "ls %s/*.matrix.bin" % (outDS)
			job2 = open("job2.sh",'w')
			os.system("chmod 777 job2.sh")
			job2.write(jobstr2)
			job2.close()
			ret, out = commands.getstatusoutput('sh job2.sh')
			for line in out.split("\n"):
				MatrixPath =  line
				print "MatrixPath: ",MatrixPath
				#if os.path.isfile(MatrixPath):
				matrixlist.append(MatrixPath)


			jobstr3 = "ls %s/*.vector.bin" % (outDS)
			job3 = open("job3.sh",'w')
			os.system("chmod 777 job3.sh")
			job3.write(jobstr3)
			job3.close()
			print "job3: ",job3
			ret, out = commands.getstatusoutput('sh job3.sh')
			for line in out.split("\n"):
				VectorPath = line
				#		if os.path.isfile(VectorPath):
				vectorlist.append(VectorPath)

			print "vectorlist: ",vectorlist

			jobstr4 = "ls %s/*.hitmap.bin" % (outDS)
			job4 = open("job4.sh",'w')
			os.system("chmod 777 job4.sh")
			job4.write(jobstr4)
			job4.close()
			print "job4: ",job4
			ret, out = commands.getstatusoutput('sh job4.sh')
			for line in out.split("\n"):
				HitmapPath =  line
				#		if os.path.isfile(HitmapPath):
				hitmaplist.append(HitmapPath)
			print "hitmaplist: ",hitmaplist
		
		else:
			print "Problem getting the outDS files"
			
  
	print "------------------------------------------"
	print "  Setting Matrices list" 
	print "------------------------------------------"

	os.system("rm *.sh")
	os.chdir(nowDir)

	return matrixlist,vectorlist,hitmaplist
				
		
			 
# For the merging of the monitoring Files
class mergeMonitoringScript:
	def __init__(self,
		     OutputPath,
		     preName,
		     iter,
		     CosmicsBoff,
		     CosmicsBon,
		     Collision,
		     CMTDIR,
		     ATHENAREL,
		     TAGS,
		     SCRIPTNAME,
		     JOBNAME, 
		     GridAccOutDS
		     ):
		self.OutputPath      = OutputPath
		self.preName         = preName
		self.i               = iter
		self.CosmicsBoff     = CosmicsBoff
		self.CosmicsBon      = CosmicsBon
		self.Collision       = Collision
		self.CMTDIR          = CMTDIR
		self.ATHENAREL       = ATHENAREL
		self.TAGS            = TAGS
		self.SCRIPTNAME      = SCRIPTNAME
		self.JOBNAME         = JOBNAME
		self.GridAccOutDS    = GridAccOutDS


	def write(self):
		TempPath="%s/Iter%02d" % (self.OutputPath, self.i)
		#self.SCRIPTNAME = TempPath + '/' + self.SCRIPTNAME
		# list of Files to be merged
		mergeMonitoringFilesName = 'mergeMonitoringFiles.txt'

		script=open(self.SCRIPTNAME,'w')
		script.write("#BSUB -J %s_Iter%02dMerge \n" % (self.preName, self.i))
		script.write("#BSUB -o %s/Iter%02d/logs/Iter%02dMerge.log \n" % (self.OutputPath,self.i,self.i))
		script.write("\n")
		script.write("#   setup the environment \n")
		script.write("source %s/setup.sh -tag=%s,%s \n" % (self.CMTDIR, self.ATHENAREL, self.TAGS))
		#script.write("cd %s/Iter%d/ \n" % (self.OutputPath,self.i))
		script.write("DQHistogramMerge.py %s TotalMonitoring.root True\n" % mergeMonitoringFilesName)
		script.close()

		mergeMonitoringFile = open(mergeMonitoringFilesName,"w")
		ret, out = commands.getstatusoutput("cat %s" % self.GridAccOutDS)
		print "out: ",out
		lines = out.split('\n')
		# looping over all output dataset names
		for line in lines:
			items = line.split()
			if len(items)>0 :
				outDS = items[0]
				print "outDS = ",outDS
				#print "hmn", glob.glob(("%s/*.root") % outDS)
				os.system("find %s/*.root >> %s \n " % (outDS, mergeMonitoringFilesName))

		mergeMonitoringFile.close()


	def send(self):
		os.system("chmod +x %s" % self.SCRIPTNAME)
		print "in doing merge----------------------------------------------"
		os.system("sh %s  \n"   % self.SCRIPTNAME)
		print "after doing merge----------------------------------------------"




# For Comparing the before and after monitoring files 
class compareMonitoringScript:
	def __init__(self,
		     OutputPath,
		     numIter,
		     CMTDIR,
		     ATHENAREL,
		     TAGS
		     ):
		self.OutputPath = OutputPath
		self.numIter    = numIter
		self.CMTDIR     = CMTDIR
		self.ATHENAREL  = ATHENAREL
		self.TAGS       = TAGS

		
	def write(self):
		# Names of the Job and the Script
		self.SCRIPTNAME = self.OutputPath + '/MonitoringComparison.lsf'
		self.JOBNAME = 'MonitoringComparison.py'
		
		# Write the Script
		script=open(self.SCRIPTNAME,'w')
		script.write("#BSUB -J MonitoringComparision \n")
		script.write("#BSUB -o "+self.OutputPath+"/MonitoringComparision.log \n")
		script.write("\n")
		script.write("#   setup the environment \n")
		script.write("source %s/setup.sh -tag=%s,%s \n" % (self.CMTDIR, self.ATHENAREL, self.TAGS))
		script.write("cd "+self.OutputPath+" \n")
		script.write("athena.py "+self.JOBNAME+" \n")
		script.write("cd -")
		script.close()
		
		# Write the Job
		job=open(self.OutputPath+"/"+self.JOBNAME,"w")
		job.write(" \n")
		job.write("# ==================================== \n")
		job.write("# The Reference File (Black)           \n")
		job.write('afterAlignmentFile = "'+self.OutputPath+'/Iter'+str(self.numIter-1)+'/TotalMonitoring.root" \n')
		job.write(" \n")
		job.write("# ==================================== \n")
		job.write("# The Monitored File (Red)             \n")
		job.write('beforeAlignmentFile = "'+self.OutputPath+'/Iter0/TotalMonitoring.root"\n')
		job.write("\n")
		job.write("# ==================================== \n")
		job.write("# The Output File                      \n")
		job.write('outputFile = "AlignmentOutput.root"    \n')
		job.write("\n")
		job.write("# ==================================== \n")
		job.write("include('InDetAlignmentMonitoring/makeComparision.py') \n")
		job.write(" \n")
		job.close()
		
	def send(self):
		os.system("chmod +x %s" % self.SCRIPTNAME)
		print "----------------------------------------------"
		print "  Running MonitoringComparision.lsf job"
		os.system("sh "+self.SCRIPTNAME+" | tee "+self.OutputPath+"/MonitoringComparison.log \n")
		print "----------------------------------------------"
			
         

