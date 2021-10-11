# 
# An Athena job to dump a specific, custom TileCal geometry tag
# to a standalone GeoModel SQLite `.db` file. 
# The .db file can then be visualized with the GeoModel's 
# standalone visualization program `gmex`.
#
# How to run this jobOptions file:
#
# ```
# setupATLAS
# asetup Athena,master,latest  # (or any release where `DumpGeo` is present)
# athena dump-geo_jobOptions_TileGeometry.py
# ```
#
# At the end, you will find a `geometry.db` file in the run directory.
# You can now open it with `gmex` on any laptop/desktop machine.
# 
# -----
# Note: 
# `gmex` is not part of Athena, for the GeoModel suite is now 
# standalone and decoupled from Athena completely; 
# that lets you visualize the geometry and use the all GeoModel tools 
# on your laptop! 
#
# More info on `gmex` here:
# https://geomodel.web.cern.ch
# -----
#
#
# Author: Riccardo Maria BIANCHI, <riccardo.maria.bianchi@cern.ch>
# Initial version: Sep 2021
#
#


##############
# --- MWE 
##############
# No input data file here, 
# so we set an Athena 'minimal working environment'

# Create an event selector:
import AthenaCommon.AtlasUnixGeneratorJob
# Configure GlobalFlags
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('geant4')

# Generate one dummy event
from AthenaCommon.AppMgr import AppMgr as appMgr
appMgr.EvtMax = 0

# Set conditions tag, to avoid crashes
vp1GlobCond="OFLCOND-SDR-BS7T-05-14"
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(vp1GlobCond)

PoolSvc = Service("PoolSvc")
PoolSvc.SortReplicas = False
# --- MWE (end)
##############


##############
# --- GEOMETRY
##############

#Detector setup:
# Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.Tile_setOn()
DetFlags.Print()

# GeoModel, minimal setup
# If wanted, here we can set a custom, global geometry tag
globalflags.DetDescrVersion = "ATLAS-R2-2016-01-00-01"
# with this, the default GeoModel tag for the release will be used
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()


# If wanted, we can set a specific tag for a particular sub-system
# Here, we set a specific tag for the TileCal calorimeter
# Get an handle on the TileCal configuration
from TileGeoModel.TileGeoModelConf import TileDetectorTool
GeoModelSvc.DetectorTools += [ TileDetectorTool() ]
from AthenaCommon.GlobalFlags import jobproperties
detDescrVersion = jobproperties.Global.DetDescrVersion()
# Setup geometry versions
GeoModelSvc.AtlasVersion = detDescrVersion
GeoModelSvc.TileVersionOverride = "TileCal-GEO-12"
# --- GEOMETRY (end)
##############



#######################
# --- DumpGeo ALGORITHM
#######################
# Import the Athena's algorithms' scheduler
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#Import and schedule the DumpGeo algorithm itself:
from DumpGeo.DumpGeoConf import DumpGeo
topSequence += DumpGeo()

DumpGeo.NoGui=True

topSequence.TimeOut=0
# --- DumpGeo ALGORITHM (end)
#######################


