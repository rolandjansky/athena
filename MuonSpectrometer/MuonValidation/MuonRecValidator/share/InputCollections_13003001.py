# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr.EventSelector.InputCollections += [ "rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003001/AOD.014945._00006.pool.root" ]
svcMgr.EventSelector.InputCollections += [ "rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003001/AOD.014945._00007.pool.root" ]
svcMgr.EventSelector.InputCollections += [ "rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003001/AOD.014945._00008.pool.root" ]
svcMgr.EventSelector.InputCollections += [ "rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003001/AOD.014945._00009.pool.root" ]
svcMgr.EventSelector.InputCollections += [ "rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003001/AOD.014945._00010.pool.root" ]
