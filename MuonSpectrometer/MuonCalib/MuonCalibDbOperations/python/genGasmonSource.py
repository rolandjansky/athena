# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import sys
from MuonCalibDbOperations.CalibrationSourceConfig import MuonCalib__gCalibrationSourceConfigs, MuonCalib__CalibrationSourceConfig
import time

def MuonCalib__genGasmonSource(toroidOn, cal_time, reader_cursor):
	if len(MuonCalib__gCalibrationSourceConfigs)==0 and not time:
		print ("FATAL Cannot generate gasmon source if no time and no other calibration sources are given")
		sys.exit(1)
	
	if not cal_time:
		cal_time = MuonCalib__gCalibrationSourceConfigs[0].GetTime(reader_cursor)
	
	if not isinstance(type(cal_time), str):
		tm=time.gmtime(cal_time)
		cal_time=str(tm.tm_year) + str(tm.tm_mon).zfill(2) + str(tm.tm_mday).zfill(2)
	imp_str="GASMON_RT_" + cal_time + "_TP"
	if toroidOn:
		imp_str += "M"
	
	print ("INFO Getting", imp_str)
	
	reader_cursor.execute("select head_id, data_schema from ATLAS_MUONCALIB_UM_META.MDT_HEAD where IMPLEMENTATION=:imp", {'imp':imp_str})
	gm_head_id, gm_schema= reader_cursor.fetchone()
	
	return MuonCalib__CalibrationSourceConfig("UM", gm_head_id, "", True, False)
	
	
	
