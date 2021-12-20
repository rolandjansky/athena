# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# job options to build geometry of the ATLAS detector
# from a standalone GeoModel SQLite file,
# and then to visualize the built geometry with VP1.
#
# Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>
# Oct 2021
#

## Python settings
from __future__ import print_function

# Create an event selector
import AthenaCommon.AtlasUnixGeneratorJob
# Configure GlobalFlags
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('geant4')

# Set some conditions tag to avoid crashes
vp1GlobCond="OFLCOND-SDR-BS7T-05-14"
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(vp1GlobCond)

# Conditions Service for reading conditions data in serial and MT Athena
from IOVSvc.IOVSvcConf import CondSvc
svcMgr += CondSvc()
 
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
 
# Conditions data access infrastructure for serial and MT Athena
from IOVSvc.IOVSvcConf import CondInputLoader
condSeq += CondInputLoader( "CondInputLoader")
 
import StoreGate.StoreGateConf as StoreGateConf
svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")

# get Athena algorithms' sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()



# Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.BCM_setOff()
DetFlags.DBM_setOff()
DetFlags.bpipe_setOff()
DetFlags.pixel_setOff()
DetFlags.SCT_setOff()
DetFlags.TRT_setOff()
DetFlags.Calo_setOff()  # LAr
DetFlags.Tile_setOn()   # Tile
DetFlags.Muon_setOff()
DetFlags.Lucid_setOff()
DetFlags.Print()

# GeoModel
globalflags.DetDescrVersion = "ATLAS-R2-2016-01-00-01"
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()

# Build from standalone GeoModel SQLite files, if uncommented
#GeoModelSvc.SQLiteDB = "geometry-ATLAS-R2-2016-01-00-01.db"
GeoModelSvc.SQLiteDB = "tile_v4.db"



# add the VP1 algorithm for visualization
from VP1Algs.VP1AlgsConf import VP1Alg
vp1Alg = VP1Alg()
topSeq += vp1Alg


