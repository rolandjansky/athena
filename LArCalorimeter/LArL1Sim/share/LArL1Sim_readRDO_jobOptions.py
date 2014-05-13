# +==========================================================================================+
# +                                                                                          +
# + Author ........: F. Ledroit                                                              +
# + Institut ......: LPSC Grenoble                                                           +
# + Creation date .: 06/08/2009      Version 0.01                                            +
# + Subject .......: Job Option file to read TTL1s in a RDO file by means of the calib algo  +
# +==========================================================================================+ 
#import AthenaCommon.AtlasUnixStandardJob
#import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
import traceback

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_geant4()

from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

# following line to be cleaned away as soon as all subsequent jO have migrated
include( "LArDetDescr/LArDetDescr_joboptions.py" )

#setup GeoModel
DetDescrVersion="ATLAS-GEO-06-00-00"
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

 
from AthenaCommon.ConfigurableDb import getConfigurable
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += getConfigurable( "ProxyProviderSvc" )()
svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
svcMgr += getConfigurable( "CondProxyProvider" )()
# la ligne suivante est necessaire, sinon le job plante !?!?!?!!!!!
svcMgr.CondProxyProvider.InputCollections = [ "LArTTCellMap-HadFcalFix.pool.root" ]


# Number of events to be processed (default is 10)
theApp.EvtMax = 10

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.useColors   = TRUE
svcMgr.MessageSvc.debugLimit = 100000


import AthenaPoolCnvSvc.ReadAthenaPool

#svcMgr.IOVDbSvc.GlobalTag="OFLCOND-CSC-01-00-00"
svcMgr.EventSelector.InputCollections = ["rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.105144.PythiaZee.digit.RDO.e380_s523_tid046366/RDO.046366._00001.pool.root.1"]
#PoolRDOInput = ["rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.105144.PythiaZee.digit.RDO.e380_s523_tid046366/RDO.046366._00001.pool.root.1"]

try:        
    from LArL1Sim.LArL1SimConf import LArTTL1Calib                
except:
    mlog.error("could not import LArL1Sim.LArTTL1Calib")
        
theLArTTL1Calib=LArTTL1Calib()
topSequence += theLArTTL1Calib

#theApp.TopAlg += [ "LArTTL1Calib"]
#LArTTL1Calib = Algorithm( "LArTTL1Calib" )
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
#LArTTL1Calib.OutputLevel = 2


#--------------------------------------------------------------
# Select HBOOK or ROOT persistency
#--------------------------------------------------------------
theApp.HistogramPersistency = "HBOOK"
#--------------------------------------------------------------
# Histogram output file 
#--------------------------------------------------------------
# Specify the appropriate output file type
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histoRDO.hbook"
#HistogramPersistencySvc.OutputFile  = "histo.rt"
#--------------------------------------------------------------
# Ntuples
#--------------------------------------------------------------
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output     = [ "FILE1 DATAFILE='tuple1RDO.hbook' OPT='NEW'" ]
#
#
# End of job options file
#
###############################################################
