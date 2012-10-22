# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr.EventSelector.InputCollections += [ "rfio:/castor/cern.ch/grid/atlas/caf/valid3/valid1.005145.PythiaZmumu.atlfast.AOD.v13003002/AOD.015629._00001.pool.root.1" ]
