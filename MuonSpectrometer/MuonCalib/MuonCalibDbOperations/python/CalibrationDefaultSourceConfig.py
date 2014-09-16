# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonCalibDbOperations.MuonCalibDbOperationsConf import MuonCalib__CoolInserter,MuonCalib__MuonCalibDefaultCalibrationSource

class MuonCalib__CalibrationDefaultSourceConfig:
	
	__t0_regions=[]
	__t0=[]
	__rt_regions=[]
	__rt_files=[]
	__time_slewing_applid_t0=[]
	__bfield_correction_applid_t0=[]
	__time_slewing_applid_rt=[]
	__bfield_correction_applid_rt=[]
	
	
	def AddDefaultT0(self, region, t0, time_slewing_applied=True, bfield_applied=True):
		self.__t0_regions.append(region)
		self.__t0.append(t0)
		self.__time_slewing_applid_t0.append(time_slewing_applied)
		self.__bfield_correction_applid_t0.append(bfield_applied)
		
	def AddDefaultRt(self, region, filename, time_slewing_applied=True, bfield_applied=True):
		self.__rt_regions.append(region)
		self.__rt_files.append(filename)
		self.__time_slewing_applid_rt.append(time_slewing_applied)
		self.__bfield_correction_applid_rt.append(bfield_applied)
		
	def ConfigureTool(self, tool_svc, cool_inserter):
		if len(self.__t0_regions) == 0 and len(self.__rt_regions) ==0:
			return 
		MuonCalibDefaultCalibrationSource = MuonCalib__MuonCalibDefaultCalibrationSource()
		tool_svc += MuonCalibDefaultCalibrationSource
		cool_inserter.CalibrationSources.append( MuonCalibDefaultCalibrationSource )
		
		MuonCalibDefaultCalibrationSource.T0Regions = self.__t0_regions[:]
		MuonCalibDefaultCalibrationSource.T0 = self.__t0[:]
		MuonCalibDefaultCalibrationSource.TimeSlewingAppliedT0 = self.__time_slewing_applid_t0[:]
		MuonCalibDefaultCalibrationSource.BFieldCorrectionAppliedT0 = self.__bfield_correction_applid_t0[:]
		
		
		MuonCalibDefaultCalibrationSource.RtRegions = self.__rt_regions[:]
		MuonCalibDefaultCalibrationSource.RtFiles = self.__rt_files[:]
		MuonCalibDefaultCalibrationSource.TimeSlewingAppliedRt = self.__time_slewing_applid_rt[:]
		MuonCalibDefaultCalibrationSource.BFieldCorrectionAppliedRt = self.__bfield_correction_applid_rt[:]


MuonCalib_gCalibrationDefaultSourceConfig = MuonCalib__CalibrationDefaultSourceConfig()
