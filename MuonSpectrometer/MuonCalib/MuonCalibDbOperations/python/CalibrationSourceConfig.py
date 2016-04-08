# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
from MuonCalibDbOperations.MuonCalibDbOperationsConf import MuonCalib__MuonCalibDbCalibrationSource
import cx_Oracle


MuonCalib__gCalibrationSourceConfigs = []

class MuonCalib__CalibrationSourceConfig:

	__working_schemata={'MP': "ATLAS_MUONCALIBR_MPI", 'RM': "ATLAS_MUONCALIBR_RM", 'UM': "ATLAS_MUONCALIBR_UM"}

	def __init__(self, site_name, head_id, region="", store_rt=True, store_t0=True):
		
		if not site_name in self.__working_schemata:
			print "FATAL: unknown site name ", site_name
			print "FATAL: must be one of", self.__working_schemata.keys()
			sys.exit(1)
			
		self.UserName="ATLAS_MUONCALIBR_READER"
		self.SiteName=site_name
		self.HeadID=head_id
		self.Region=region
		self.StoreRt=store_rt
		self.StoreT0=store_t0
		self.T0Offset=0.0
		self.DriftTimeOffsetsVsR=[]
		self.__name = "MuonCalibDbCalibrationSource_" + str(len(MuonCalib__gCalibrationSourceConfigs))
		MuonCalib__gCalibrationSourceConfigs.append(self)
		self.TimeSlewingApplied=True
		self.BFieldCorrectionApplied=True
		
	def GenCalibrationSource(self, reader_passwd):
		source=MuonCalib__MuonCalibDbCalibrationSource(self.__name)	
		source.WorkingSchema = self.__working_schemata[self.SiteName]
		source.UserName = self.UserName
		if hasattr(self, "ReaderPassword"):
			source.Password = self.ReaderPassword
		else:
			source.Password = reader_passwd
		source.SiteName = self.SiteName
		source.HeadId = self.HeadID
		source.Region = self.Region
		source.StoreRT = self.StoreRt
		source.StoreT0 = self.StoreT0
		source.T0Offset = self.T0Offset
		source.DriftTimeOffsetsVsR = self.DriftTimeOffsetsVsR
		source.TimeSlewingApplied = self.TimeSlewingApplied
		source.BFieldCorrectionApplied = self.BFieldCorrectionApplied
		if hasattr(self, "ConnectionString"):
			source.ConnectionString = self.ConnectionString
		else:
			source.ConnectionString =  "oracle://ATLAS_MUON/" + source.UserName 
		return source
			
	def GetTime(self, reader_cursor):
		reader_cursor.execute("select LOWTIME from " + self.__working_schemata[self.SiteName] + ".MDT_HEAD where head_id=:hid", {'hid': self.HeadID})
		lowtime, =reader_cursor.fetchone()
		return lowtime
		
		
