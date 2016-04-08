# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
from PyCool import cool,coral
import CoolConvUtilities.AtlCoolLib as AtlCoolLib
from MuonCalibDbOperations.MuonCalibResolveTag import ResolveTag


MuonCalib__gCalibrationTargetConfigs = []

class MuonCalib__CalibrationTargetConfig:

	def __init__(self, FolderType="T0"):
		if not FolderType in ["T0", "RT", "T0BLOB", "RTBLOB"]:
			print "FATAL Folder type must be 'T0[BLOB]' or 'RT[BLOB]'"
			sys.exit(1)

#set defaults

		# Default None for all FolderTypes
		self.Tag=None

		if FolderType=="T0":
			self.__is_t0=True
			self.Compress=False
			self.Folder = "/MDT/T0"
		elif FolderType == "T0BLOB":
			self.__is_t0=True
			self.Compress=True
			self.Folder = "/MDT/T0BLOB"
		if FolderType=="RT":
			self.__is_t0=False
			self.Compress=False
			self.Folder = "/MDT/RT"
		elif FolderType == "RTBLOB":
			self.__is_t0=False
			self.Compress=True
			self.Folder = "/MDT/RTBLOB"
		
		MuonCalib__gCalibrationTargetConfigs.append(self)

	def ResolveGlobalTag(self, tag, dbstring="COOLOFL_MDT/CONDBR2"):
		if self.Tag:
			print "INFO Tag for", self.Folder, "already set to", self.Tag
			print "INFO Not resolving global tag"
			return
		try:
			db=AtlCoolLib.indirectOpen(dbstring, oracle=True, readOnly=True, debug=True)
		except Exception,e:
			print 'Problem opening database',e
			sys.exit(-1)	
		cool_folder=db.getFolder(self.Folder)
		self.Tag=ResolveTag(cool_folder, tag)
		print "INFO tag '" + tag + "' resolves to '" + self.Tag + "' for folder '" + self.Folder + "'"
		
		
	
	def AddToCoolInserter(self, coolInserter):
		
		if self.__is_t0:
			coolInserter.T0Folder.append(self.Folder)
			coolInserter.T0Tag.append(self.Tag)
			coolInserter.T0FolderCompress.append(self.Compress)
		else:
			coolInserter.RtFolder.append(self.Folder)
			coolInserter.RtTag.append(self.Tag)
			coolInserter.RtFolderCompress.append(self.Compress)

