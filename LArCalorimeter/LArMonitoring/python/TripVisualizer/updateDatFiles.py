# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import fnmatch

tripIds=[]
for file in os.listdir('HVHistoryDat'):
	if fnmatch.fnmatch(file,'trip_[0-9]*.dat'):
		tripIds.append(int(file.strip('trip_.dat')))


startFrom = max(tripIds)+1

cmd= 'python GenerateHVDatFiles.py %i'%startFrom

print cmd
os.system(cmd) 