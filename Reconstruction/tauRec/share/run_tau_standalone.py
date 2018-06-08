# minimal set of job options to run tau reco on an ESD file
# uses main TauRecRunner script

#from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
# Check for unmet data dependencies

#svcMgr += ForwardSchedulerSvc()
#svcMgr.ForwardSchedulerSvc.CheckDependencies = True

# MT-specific code
# Get number of processes and threads
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
nProc = jp.ConcurrencyFlags.NumProcs()

theApp.EvtMax = 10

if nThreads >=1 :
   # Support for the MT-MP hybrid mode
   if (nProc > 0) :

      from AthenaCommon.Logging import log as msg
      if (theApp.EvtMax == -1) :
         msg.fatal('EvtMax must be >0 for hybrid configuration')
         sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

         if ( theApp.EvtMax % nProc != 0 ) :
            msg.warning('EvtMax[%s] is not divisible by nProcs[%s]: MP Workers will not process all requested events',theApp.EvtMax,nProc)

         chunkSize = int (theApp.EvtMax / nProc)

         from AthenaMP.AthenaMPFlags import jobproperties as jps
         jps.AthenaMPFlags.ChunkSize= chunkSize

         msg.info('AthenaMP workers will process %s events each',chunkSize)

# Set input files
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/work/a/adbailey/public/ESD/mc16_13TeV.301046.PowhegPythia8EvtGen_AZNLOCTEQ6L1_DYtautau_1000M1250.recon.ESD.e3649_s3170_r9466/ESD.11318157._000005.pool.root.1"]

# and add to svcMgr
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

### This can go into config file  ###

# Magnetic Field
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Calo_setOn()
DetFlags.detdescr.ID_setOn()
if hasattr(DetFlags,'BField_on'): DetFlags.BField_setOn()
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
AtlasTrackingGeometrySvc  = svcMgr.AtlasTrackingGeometrySvc
include('RecExCond/AllDet_detDescr.py')
#Magnetic field is now setup

# Detector Description
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Muon_setOn()
if hasattr(DetFlags,'BField_on'): DetFlags.BField_setOn()
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
AtlasTrackingGeometrySvc  = svcMgr.AtlasTrackingGeometrySvc
include('RecExCond/AllDet_detDescr.py')

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# RecEx config
from RecExConfig.RecFlags import rec
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())

from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags
import AthenaCommon.SystemOfUnits as Units
from tauRec.tauRecFlags import jobproperties as taujp

rec.doEgamma.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)

# CaloRec config
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(False)

### End into config file  ###

# use Tau Jet Vertex Association Tool
# each Tau candidate gets its own primary vertex
# and the tracks are selected accroding to this vertex
_doTJVA = True

# Pi0-finding algorithm
_doPi0Clus = taujp.tauRecFlags.doPi0() #False by default
_doPi0Clus = True

# the TauCoreBuilder
from tauRec.TauRecBuilder import TauRecCoreBuilder
TauRecCoreBuilder(doPi0Clus=_doPi0Clus, doTJVA=_doTJVA)

#include("tauRec/Pi0ClusterMaker_Crakow_jobOptions.py")
#if _doPi0Clus:
#    include("tauRec/Pi0ClusterMaker_jobOptions.py")


# minimal set of job options to run tau reco on an ESD file
# uses main TauRecRunner script

#from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
# Check for unmet data dependencies

#svcMgr += ForwardSchedulerSvc()
#svcMgr.ForwardSchedulerSvc.CheckDependencies = True
