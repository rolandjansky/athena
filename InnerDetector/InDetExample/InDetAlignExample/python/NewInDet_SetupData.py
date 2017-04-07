# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class setupData:
	def __init__ (self, name):
		self.name = name
		self.dataType = "IDTracks"
		self.fileListName = self.name+"_"+self.dataType+".txt"
		self.Runs = [0]
		self.RecoTags = [""]
		self.CPUs = [1,1,1]
		self.Events = [100,100,100]
		self.DetDescrVersion = ""
		self.limitFiles = -1
		self.GlobalTag = ""
		self.ByteStream = True
		self.STAGE_SVCCLASS = "default"
		self.projectName = ""
		self.AreCosmics = False
		self.pixelDigitalClustering = False
		self.thePtMin = 0
	def setPtMin(self, newPtMin):
		self.thePtMin = newPtMin
	def getPtMin(self):
		return self.thePtMin
	def setDigitalClustering(self, digitalClustering):
		self.pixelDigitalClustering = digitalClustering
	def getDigitalClustering(self):
		return self.pixelDigitalClustering
	def setCosmic (self, cosmics):
		self.AreCosmics = cosmics
	def getCosmics(self):
		return self.AreCosmics
	def setProjectName (self, projectName):
		self.projectName = projectName
	def getProjectName(self):
		return self.projectName
	def setDataType(self, dataType):
		self.dataType = dataType
	def getDataType(self):
		return self.dataType
	def getName(self):
		return self.name
	def setByteStream(self, ByteStream):
		self.ByteStream = ByteStream
	def getByteStream(self):
		return self.ByteStream
	def setRuns(self, Runs):
		self.Runs = Runs
	def getRuns(self):
		return self.Runs
	def setRecoTags(self,RecoTags):
		self.RecoTags = RecoTags
	def getRecoTags(self,i):
		return self.RecoTags[i]
	def setCPUs(self,CPUs):
		self.CPUs = CPUs
	def getCPUs(self,i):
		return self.CPUs[i]
	def setEvents(self, Events):
		self.Events = Events
	def getEvents(self,i):
		return self.Events[i]
	def setLimitFiles(self,i):
		self.limitFiles = i
	def getEventsPerCPU(self,i):
		if self.Events[i] == -1:
			return -1
		else:
			return self.Events[i] / self.CPUs[i]
	def setGlobalTag(self,GlobalTag):
		self.GlobalTag = GlobalTag
	def getGlobalTag(self):
		return self.GlobalTag
	def setDetDescrVersion(self,DetDescrVersion):
		self.DetDescrVersion = DetDescrVersion	
	def getDetDescrVersion(self):
		return self.DetDescrVersion 
	def CreateFileList(self):
		import os
		self.fileListName = self.name+"_"+self.dataType+".txt"
		if os.path.isfile(self.fileListName):
			print "The file ", self.fileListName, " exists, using the existing one."
			return 0
	
		outputFile = open(self.fileListName, "w")
		count = 0
		for run in self.Runs:
			run = int(run)
			if "IDTracks" == self.dataType:
				#path = "/castor/cern.ch/grid/atlas/DAQ/2010/%08d/calibration_IDTracks/" % run
				if run < 170000:
					path = "/castor/cern.ch/grid/atlas/atlasscratchdisk/data10_7TeV/RAW/other/data10_7TeV.%08d.calibration_IDTracks.daq.RAW/" % run
				elif run < 200000:
					path = "/castor/cern.ch/grid/atlas/DAQ/2011/%08d/calibration_IDTracks/" % run
				else:
					path = "/castor/cern.ch/grid/atlas/DAQ/2012/%08d/calibration_IDTracks/" % run

				self.ByteStream = True
			elif "MinBias" == self.dataType:
				path = "/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/%s/data10_7TeV.%08d.physics_MinBias.recon.ESD.%s/" % (self.RecoTags[count],run,self.RecoTags[count])
				#path = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_7TeV/physics_MinBias/%07d/data10_7TeV.%08d.physics_MinBias.merge.RAW/" % (run,run)
				self.ByteStream = False
			elif "IDCosmics" == self.dataType:
				#path = "/castor/cern.ch/grid/atlas/DAQ/merge/2010/%08d/physics_IDCosmic/" % (run)
				#path = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_cos/physics_IDCosmic/%07d/data10_cos.%08d.physics_IDCosmic.merge.RAW/" % (run,run)
				if run < 170000:
					path = " /castor/cern.ch/grid/atlas/atlasscratchdisk/data10_7TeV/RAW/other/data10_7TeV.%08d.physics_IDCosmic.merge.RAW/" % run
				else:
					path = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_7TeV/physics_IDCosmic/%08d/data11_7TeV.%08d.physics_IDCosmic.merge.RAW/" % (run,run)
				self.ByteStream = True
			elif "MC09_Multimuons" == self.dataType:
				path = "/castor/cern.ch/grid/atlas/atlasgroupdisk/perf-idtracking/dq2/mc09_valid/ESD/e436_s561_r731/mc09_valid.107271.Multimuons_pt9.recon.ESD.e436_s561_r731_tid076491/"
				self.ByteStream = False
			elif "MC10_Singlemuons" == self.dataType:
				path = "/eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.107233.singlepart_mu100.digit.RDO.e605_s933_s946_d369/"
				self.ByteStream = False
			elif "HeavyIons" == self.dataType:
				path = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_hi/express_express/%07d/data10_hi.%08d.express_express.merge.RAW/" % (run,run)
				self.ByteStream = True
			elif "Express" == self.dataType:
				if run > 170000:
					path = "/castor/cern.ch/grid/atlas/atlasdatadisk/data11_7TeV/ESD/%s/data11_7TeV.%08d.express_express.recon.ESD.%s/" % (self.RecoTags[count],run,self.RecoTags[count])
			elif "Egamma" == self.dataType:
				path = "/eos/atlas/atlasdatadisk/data11_7TeV/DESD_SGLEL/%s/data11_7TeV.%08d.physics_Egamma.merge.DESD_SGLEL.%s/" % (self.RecoTags[count],run,self.RecoTags[count])
			else:
				print "Creation of the file list for this data type is not coded yet"
				return 0
			self.setSTAGE_SVCCLASS(path)
			
			print "Reading castor directory: ", path
			if "castor" in path:
				inputfiles = os.popen("rfdir "+ path).read().splitlines()
			elif "eos" in path:
				inputfiles = os.popen("/afs/cern.ch/project/eos/installation/0.2.31/bin/eos.select ls " + path).read().splitlines()
				print "Reading input files from eos"
				print inputfiles 
			files = 0
			print self.STAGE_SVCCLASS
			for file in inputfiles:
				if "eos" in path:
					outputFile.write('root://eosatlas/'+path + '/' + file + '\n')
				else:
					check_staged = os.popen("stager_qry -S "+self.STAGE_SVCCLASS+" -M " + path + file.split()[8]).read()
					if "STAGED" in check_staged or "CANBEMIGR" in check_staged:
						if "atlascerngroupdisk" in path:
							outputFile.write("root://castoratlas/"+path + file.split()[8] + '\t' + file.split()[4] + '\n')
						elif "DESD" in path:
							outputFile.write("rfio://"+path + file.split()[8] + '\t' + file.split()[4] + '\n')				
						elif "castor" in path:
							outputFile.write("root://castoratlas/"+path + file.split()[8] + '\t' + file.split()[4] + '\n')
						
						else:
							outputFile.write(path + file.split()[8] + '\t' + file.split()[4] + '\n')
						files = files + 1
						if files >= self.limitFiles and self.limitFiles != -1:
							break
					else:
						print "File ", file.split()[8], " no STAGED, skipping file"
			count = count + 1
		print "Created file " ,self.fileListName, " with the list of datafiles"
	def getFileList(self):
		return self.fileListName
	def setCustomFileList(self,filelist):
		self.fileListName = filelist
	def getNFilesInList(self):
		num_lines = sum(1 for line in open(self.fileListName))
		return num_lines
	def setSTAGE_SVCCLASS(self,path):
		print path
		if "atlasdatadisk" in path:
			self.STAGE_SVCCLASS="atldata"
		if "DAQ" in path or "tzero" in path:
			self.STAGE_SVCCLASS="atlcal"
		if "atlasgroupdisk" in path:
			self.STAGE_SVCCLASS="atlasgroupdisk"
		if "atlasscratchdisk" in path:
			self.STAGE_SVCCLASS="atlasscratchdisk"
		if "atlascerngroupdisk" in path:
			self.STAGE_SVCCLASS="atlascerngroupdisk"
			import os
			os.environ["STAGE_HOST"] = "castoratlast3"
		print "STAGE_SVCCLASS set to ", self.STAGE_SVCCLASS
		import os
		os.environ['STAGE_SVCCLASS']=self.STAGE_SVCCLASS

	def Print(self):
		print "---------------------------"
		print "Dataset with name ", self.name
		print "The data is: ", self.dataType
		print "ProjectName: ", self.projectName
		print "Is ByteStream? ", self.ByteStream
		print "Are data Cosmics? ", self.AreCosmics
		print "The list of files to use is in: ", self.fileListName
		print "CPUs: ", self.CPUs
		print "Events: ", self.Events
		
		
		
		
		
