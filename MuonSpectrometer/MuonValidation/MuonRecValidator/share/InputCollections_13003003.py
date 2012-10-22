# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr.EventSelector.InputCollections += [ 
"rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003003/AOD.016886._00001.pool.root.1" ,
"rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003003/AOD.016886._00002.pool.root.2" ,
"rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003003/AOD.016886._00003.pool.root.1" ,
"rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003003/AOD.016886._00004.pool.root.1" ,
"rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003003/AOD.016886._00005.pool.root.1" ,
"rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003003/AOD.016938._00006.pool.root" ,
"rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003003/AOD.016938._00007.pool.root" ,
"rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003003/AOD.016938._00008.pool.root" ,
"rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003003/AOD.016938._00009.pool.root" ,
"rfio:/castor/cern.ch/grid/atlas/caf/valid3/trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13003003/AOD.016938._00010.pool.root" ]
