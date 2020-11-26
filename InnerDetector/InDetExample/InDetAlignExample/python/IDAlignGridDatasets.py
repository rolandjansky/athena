# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


#################################################################
#    Script for running ATLAS ID alignment jobs on Grid resources
#    Authors: Jike Wang      (jike.wang@cern.ch)
#################################################################

import os
import sys
import subprocess


class ConfiguredIDAlignDatasets:
	def __init__ (self, datasetType, geometrySetting, DatasetsOptions) : 
		self.__datasetType            = datasetType
		self.__DatasetsOptions        = DatasetsOptions


		#########################################################################################
		if self.__datasetType == "MC09":
			
			if geometrySetting == 'Nominal':
				self.__DatasetsOptions["CollisionDetDescr" ] = "ATLAS-GEO-08-00-00"
				self.__DatasetsOptions["CollisionGlobalTag"] = 'OFLCOND-SIM-00-00-07'
			if geometrySetting == 'Perfect':
				self.__DatasetsOptions["CollisionDetDescr" ] = "ATLAS-GEO-08-00-00"
				self.__DatasetsOptions["CollisionGlobalTag"] = 'OFLCOND-CSC-01-00-00'
		#########################################################################################
		if self.__datasetType.find("900GeV") != -1 :

			if len(self.__DatasetsOptions["CollisionRunList"]) != len(self.__DatasetsOptions["CollisionRecoTag"]) : 
				print ("Hi, the RunList and the RecoTagList length are not equal!!!")
				sys.exit()

			if "Nominal" == geometrySetting :
				self.__DatasetsOptions["CollisionDetDescr" ] = "ATLAS-GEO-08-00-02"
				self.__DatasetsOptions["CollisionGlobalTag"] = "COMCOND-ES1PS-001-00"
			self.__DatasetsOptions["CollisionDetDescrBoff"     ] = "ATLAS-GEONF-08-00-00"

		#########################################################################################
		if self.__datasetType.find("7TeV") != -1 :

			if len(self.__DatasetsOptions["CollisionRunList"]) != len(self.__DatasetsOptions["CollisionRecoTag"]) :
				print ("Hi, the RunList and the RecoTagList length are not equal!!!")
				sys.exit()

			if 'Nominal' == geometrySetting :
				self.__DatasetsOptions["CollisionDetDescr" ] = "COMCOND-ES1PST-002-00"
				self.__DatasetsOptions["CollisionGlobalTag"] = "ATLAS-GEO-10-00-00"
			self.__DatasetsOptions["CollisionDetDescrBoff"     ] = "ATLAS-GEONF-08-00-00"
		
		#########################################################################################




	def detDescrVersion (self, topology = "", i = 0) :
		if "CosmicBoff" == topology : 
			return self.__DatasetsOptions["CollisionDetDescrBoff"]
		else : 
			if (self.containType("Customed") and  "Customed" == topology ) :
				return self.__DatasetsOptions["CustomedDatasetsDetDescr"][i]
			elif "Collision" == topology : 
				return self.__DatasetsOptions["CollisionDetDescr"]



	def detDescrVersionNoField (self) :
		self.__detDescrVersionNoField


	def globalTag (self, topology = "", i = 0) : 
		if self.containType("Customed") and "Customed" == topology :
			return self.__DatasetsOptions["CustomedDatasetsGlobalTag"][i]
		elif "Collision" == topology :
			return self.__DatasetsOptions["CollisionGlobalTag"]



	def containType (self, type) :
		if self.__datasetType.find(type) != -1 : 
			return True
		else : 
			return False

	def doDetailedTagsConfig(self) :
		if self.__DatasetsOptions["doDetailedTagsConfig"] :   
			return True
		else : 
			return False 


	def stream(self) :
		return self.__DatasetsOptions["CollisionStream"]


	def oneDatasetName (self, topology) :
		oneDatasetName = ""
		if ("mc09" == self.__datasetType or "MC09" == self.__datasetType) :
			oneDatasetName = "mc09_valid.107271.Multimuons_pt9.recon.ESD.e436_s561_r731"
			return oneDatasetName

		elif self.containType("Customed") and "Customed" == topology :
			oneDatasetName = self.__DatasetsOptions["CustomedDatasetsNameList"][0]

		elif ("Collision" == topology) :  
			if self.stream() == "MinBias" and self.containType("900GeV") :
				oneDatasetName = "data09_900GeV.%08d.physics_MinBias.recon.ESD.%s" % ( int(self.__DatasetsOptions["CollisionRunList"][0]), self.__DatasetsOptions["CollisionRecoTag"][0] )

			if self.stream() == "MinBias" and self.containType("7TeV"):
				oneDatasetName = "data10_7TeV.%08d.physics_MinBias.recon.ESD.%s"   % ( int(self.__DatasetsOptions["CollisionRunList"][0]), self.__DatasetsOptions["CollisionRecoTag"][0] )

		#elif ("CosmicBon" == topology) :
		return oneDatasetName



	def oneFileName (self, topology) :
		oneFileName = ""
		if ("mc09" == self.__datasetType or "MC09" == self.__datasetType) :
			datasetName = "mc09_valid.107271.Multimuons_pt9.recon.ESD.e436_s561_r731"

		elif (self.containType("Customed") and ("Customed" == topology) ) :
			datasetName = self.__DatasetsOptions["CustomedDatasetsNameList"][0]

		elif ("Collision" == topology) :
			if self.stream() == "MinBias" and self.containType("900GeV") :
				datasetName = "data09_900GeV.%08d.physics_MinBias.recon.ESD.%s" % ( int(self.__DatasetsOptions["CollisionRunList"][0]), self.__DatasetsOptions["CollisionRecoTag"][0] )
			if self.stream() == "MinBias" and self.containType("7TeV") :
				datasetName = "data10_7TeV.%08d.physics_MinBias.recon.ESD.%s"   % ( int(self.__DatasetsOptions["CollisionRunList"][0]), self.__DatasetsOptions["CollisionRecoTag"][0] )

		#elif ("CosmicBon" == topology) :
		status, oneFileName = subprocess.getstatusoutput(" dq2-ls -f %s | tail -n 6 | head -n 1 |  cut -f2 " % datasetName)
		return oneFileName




	def nFiles(self, topology) :	
		num    = 0
		nFiles = []

		print (self.__datasetType)

		if (self.__datasetType.find("mc09") != -1) or (self.__datasetType.find("MC09") != -1) :
			datasetName = "mc09_valid.107271.Multimuons_pt9.recon.ESD.e436_s561_r731"
			os.system('dq2-ls -f %s | grep -i files | grep -i total | cut -c 13- >> num' % datasetName)

		elif (self.containType("Customed") and ("Customed" == topology) ) : 
			os.system('rm -f tmp_dataset_NFile.txt')
			os.system('touch tmp_dataset_NFile.txt')

			for i in range(len(self.__DatasetsOptions["CustomedDatasetsNameList"])) :
				#print (i)
				datasetName = self.__DatasetsOptions["CustomedDatasetsNameList"][i]
				# protection be here? if the dataset name is not exist, should warn ! 
				os.system('dq2-ls -f %s | grep -i files | grep -i total | cut -c 13- >> tmp_dataset_NFile.txt ' % datasetName )
				print ("datasetName: " , datasetName    )

			inputfiles = open(("tmp_dataset_NFile.txt"), "r")

			#print (inputfiles )
			for line in inputfiles :
				#print (" line: " , line)
				if int(line) < 0 :
					print ("Hi guys, very weird the files number of this customed dataset is not integer or negetive!!! ")
					sys.exit()
				else :
					print ("Hi guys, the files number of this customed dataset is: " , line)
					nFiles.append(int(line))
					num += int(line)

			if self.__DatasetsOptions["doDetailedTagsConfig"] : 
				return nFiles
			else : 
				return num


		elif ("Collision" == topology) :
			os.system('rm -f tmp_dataset_NFile.txt')
			os.system('touch tmp_dataset_NFile.txt')

			for i in range(len(self.__DatasetsOptions["CollisionRunList"])):
				run     = int(self.__DatasetsOptions["CollisionRunList"][i])
				tag     =     self.__DatasetsOptions["CollisionRecoTag"][i]

				if self.stream() == "MinBias" and self.containType("900GeV") :
					datasetName = "data09_900GeV.%08d.physics_MinBias.recon.ESD.%s" % (run,tag)
				if self.stream() == "MinBias" and self.containType("7TeV") :
					datasetName = "data10_7TeV.%08d.physics_MinBias.recon.ESD.%s"   % (run,tag)

				print ("Hi guys, the full name of this run is: ", datasetName)
				os.system('dq2-ls -f %s | grep -i files | grep -i total | cut -c 13- >> tmp_dataset_NFile.txt ' % datasetName)

			inputfiles = open(("tmp_dataset_NFile.txt"), "r")
			for line in inputfiles :
				if int(line) < 0 : 
					print ("Hi guys, very weird the files number of this run is not integer or negative!!! ")
					sys.exit()
				else:	
					print ("Hi guys, the files number of this run is: " , line)
					num += int(line)

		#elif ("CosmicBon" == topology) :
		return num



	def namesList(self, topology="") : 
		namesList = []

		if (self.__datasetType == "mc09") or (self.__datasetType == "MC09" ) :
			return "mc09_valid.107271.Multimuons_pt9.recon.ESD.e436_s561_r731"

		elif (self.containType("Customed")  and "Customed" == topology ) :
			for i in range(   len(self.__DatasetsOptions["CustomedDatasetsNameList"]) ):
				datasetName = self.__DatasetsOptions["CustomedDatasetsNameList"][i]
				namesList.append(datasetName) 

			if self.__DatasetsOptions["doDetailedTagsConfig"] :
				return namesList 
			else : 
				str = ",".join(namesList)
				return str


		elif("Collision" == topology) : 
			for i in range(  len(self.__DatasetsOptions["CollisionRunList"]) ):
				run = int( self.__DatasetsOptions["CollisionRunList"][i] )
				tag = self.__DatasetsOptions["CollisionRecoTag"][i]

				if self.__DatasetsOptions["CollisionStream"] == "MinBias" and self.containType("900GeV") :
					datasetName = "data09_900GeV.%08d.physics_MinBias.recon.ESD.%s" % (run,tag)

				if self.__DatasetsOptions["CollisionStream"] == "MinBias" and self.containType("7TeV") :
					datasetName = "data10_7TeV.%08d.physics_MinBias.recon.ESD.%s"   % (run,tag)

				namesList.append(datasetName)
			str = ",".join(namesList)
			return str


	def recoScript(self, topology = "", i = 0) :

		if ( self.containType("Customed") and "Customed" == topology ) : 

			if ( "Sim" in self.__DatasetsOptions["CustomedDatasetsNameList"][i]) and ("Cos" in self.__DatasetsOptions["CustomedDatasetsNameList"][i]) :
				recoScript = "InDetAlignExample/loadInDetRec_MC_Cosmics.py"

			#elif "mc" in self.__DatasetsOptions["CustomedDatasetsNameList"][i] :
			#	recoScript = "InDetAlignExample/loadInDetRec.py"                               

			else:
				recoScript = "InDetAlignExample/loadInDetRec_new.py"

			return recoScript 


		if ("CosmicBon" == topology or "CosmicBoff" == topology) : 
			recoScript = "InDetAlignExample/loadInDetRec_cosmics.py"

		elif self.__datasetType.find("900GeV") != -1 :
			recoScript      = "InDetAlignExample/loadInDetRec900GeV.py"
		elif self.__datasetType.find("SingleBeam") != -1 :
			recoScript      = "InDetAlignExample/loadInDetRecSingleBeam.py"
		elif self.__datasetType.find("7TeV") != -1 :
			recoScript      = "InDetAlignExample/loadInDetRec_new.py"
		else:
			recoScript      = "InDetAlignExample/loadInDetRec.py"
		
		return recoScript



	def extraInfoFromAMI(self, datasetName) :

		geometry_tag  = ""
		condition_tag = ""
		a, line1 = subprocess.getstatusoutput("dump-athfile.py ami://%s | grep geometry "   % datasetName )
		print ("dumping geometry version of %s from AMI ... "  % datasetName)
		print ( line1)
		item1 = line1.split()
		#print (" item1 : " , item1		)
		if "None" != item1[-1] :
			geometry_tag = item1[-1]
			print ("geometry_tag retrieved from AMI: " ,  geometry_tag)
		else :
			print ("Hmn, seems can't retrieve the geometry tag information for dataset  %s "    % datasetName)

		a, line2 = subprocess.getstatusoutput("dump-athfile.py ami://%s | grep conditions " % datasetName )
		print ("dumping condition tag version of %s from AMI ... "  % datasetName)
		print ( line2)
		item2 = line2.split()
		if "None" != item2[-1] :
			condition_tag = item2[-1]
			print ("condition_tag retrieved from AMI: " , condition_tag)

		else :
			print ("Hmn, seems can't retrieve the conditions tag information for dataset  %s "  % datasetName)
		return (geometry_tag,condition_tag)




	def configDatasetTags(self, topology = "") : 

		geometry_tag_fromAMI  = ""
		condition_tag_fromAMI = ""

		if ( self.containType("Customed") and "Customed" == topology and self.__DatasetsOptions["CompareTagsWithAMI"] ) : 

			for i in range(len(self.__DatasetsOptions["CustomedDatasetsNameList"])):

				(geometry_tag_fromAMI, condition_tag_fromAMI) = self.extraInfoFromAMI(self.__DatasetsOptions["CustomedDatasetsNameList"][i])
				print ("the geometry tag and condition tag from AMI are %s and %s : " %  ( geometry_tag_fromAMI, condition_tag_fromAMI ) 	)

				if self.__DatasetsOptions["CustomedDatasetsDetDescr"][i]  != "" :    
					if ( geometry_tag_fromAMI  and geometry_tag_fromAMI  != self.__DatasetsOptions["CustomedDatasetsDetDescr"][i]) :  	
						print (inred("WARNING!!!") + "you are going to change the intrinsic geometry tag of your CustomedDatasets ")
				else : 
					print (inred("WARNING!!!") + " you have not set the geometry tag for your CustomedDatasets, so set the one from AMI ")
					self.__DatasetsOptions["CustomedDatasetsDetDescr"][i] = geometry_tag_fromAMI

				if self.__DatasetsOptions["CustomedDatasetsGlobalTag"][i] != "" : 
					if ( condition_tag_fromAMI and condition_tag_fromAMI != self.__DatasetsOptions["CustomedDatasetsGlobalTag"][i]) :   
						print (inred("WARNING!!!") + "you are going to change the intrinsic condition tag of your CustomedDatasets ")
				else : 																            
					print (inred("WARNING!!!") + " you have not set the condition tag for your CustomedDatasets, so set the one from AMI ")
					self.__DatasetsOptions["CustomedDatasetsGlobalTag"][i] = condition_tag_fromAMI



def inred( s ):
	return "%s[31;2m%s%s[0m" % (chr(27), s, chr(27))


