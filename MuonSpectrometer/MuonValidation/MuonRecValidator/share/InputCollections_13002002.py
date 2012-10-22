# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr.EventSelector.InputCollections += [ "rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13002002/AOD.013624._00001.pool.root.1" ]
svcMgr.EventSelector.InputCollections += [ "rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13002002/AOD.013624._00002.pool.root.1" ]
svcMgr.EventSelector.InputCollections += [ "rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13002002/AOD.013624._00003.pool.root.1" ]
svcMgr.EventSelector.InputCollections += [ "rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13002002/AOD.013624._00004.pool.root.1" ]
svcMgr.EventSelector.InputCollections += [ "rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13002002/AOD.013624._00005.pool.root.1" ]


