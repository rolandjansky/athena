# get the logger
from AthenaCommon.Logging import *
mdtCalibFitlog = logging.getLogger('mdtCalibFit')

mdtCalibFitlog.info('************** STARTING MDT CalibFit !!!!**************')
#==============================================================
# Job definition parameters:
#==============================================================
CalibSite = runArgs.calibSite
geover     = runArgs.geometryVersion
mdtCalibFitlog.info( '**** Transformation run arguments ****' )
#mdtCalibFitlog.info( str(runArgs) )

#==============================================================
# Job Configuration parameters:
#==============================================================
from MuonCalibJobTransforms.MDTCalibFitConfig import mdtCalibFitConfig
#for cf in runArgs.jobConfig:
#    include(cf)
mdtCalibFitlog.info( '**** Transformation configuration arguments ****' )
mdtCalibFitlog.info( str(mdtCalibFitConfig) )

### Setup Athena common flags
#include ("AthenaCommon/AthenaCommonFlags.py")
#include.block("AthenaCommon/AthenaCommonFlags.py")

#include( "IOVDbSvc/IOVRecExCommon.py" )

#load atlas geometry
from AthenaCommon.GlobalFlags import globalflags
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DataSource.set_data()
#GlobalFlags.Print()
globalflags.DetGeo.set_Value_and_Lock('atlas')
ToolSvc = Service( "ToolSvc" )
globalflags.DataSource.set_Value_and_Lock('data')

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.Print()
#Set geometry tag
from AtlasGeoModel import SetGeometryVersion, GeoModelInit
GeoModelSvc = ServiceMgr.GeoModelSvc
GeoModelSvc.AtlasVersion = geover

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc

from MdtCalibValidation.MdtCalibValidationConf import MuonCalib__MdtValidationAlg
MdtValidationAlg = MuonCalib__MdtValidationAlg()

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += MdtValidationAlg
		
theApp.EvtMax = 1

from MuonCalibStandAloneBase.MuonCalibStandAloneBaseConf import RegionSelectionSvc
RegionSelectionSvc = RegionSelectionSvc()
RegionSelectionSvc.PrintList = True
ServiceMgr += RegionSelectionSvc
#RegionSelectionSvc.Region = "[BIL,4,6]"
RegionSelectionSvc.Region = runArgs.selectChamber

MdtValidationAlg.SiteName=CalibSite
if (CalibSite == "MP"):
# MPI
  MdtValidationAlg.ConnectionString = "oracle://oracle01.mppmu.mpg.de/ATLMPI.mppmu.mpg.de/atlas_muoncalib_mpi"
  MdtValidationAlg.WorkingSchema = "atlas_muoncalib_mpi"
elif (CalibSite == "MP_NEW"):
# MPI        
  MdtValidationAlg.ConnectionString = "oracle://oracle01.mppmu.mpg.de/ATLMPI.mppmu.mpg.de/ATLAS_MUONCALIB_READER"
  MdtValidationAlg.WorkingSchema = "ATLAS_MUONCALIB_MPI_META"
  MdtValidationAlg.WriterConnectionString = "oracle://oracle01.mppmu.mpg.de/ATLMPI.mppmu.mpg.de/ATLAS_MUONCALIB_WRITER"
elif (CalibSite == "MI"):
# Michigan
  VlidationAlgalidationAlg.ConnectionString = "oracle://umors.grid.umich.edu/muoncal.grid.umich.edu/atlas_muoncalib_mich"
  MdtValidationAlg.WorkingSchema = "atlas_muoncalib_mich"
elif (CalibSite == "MI_NEW"):
# Michigan        
  MdtValidationAlg.ConnectionString = "oracle://umors.grid.umich.edu/muoncal.grid.umich.edu/ATLAS_MUONCALIB_READER"
  MdtValidationAlg.WorkingSchema = "ATLAS_MUONCALIB_UMICH"
  MdtValidationAlg.WriterConnectionString = "oracle://umors.grid.umich.edu/muoncal.grid.umich.edu/ATLAS_MUONCALIB_WRITER"
elif (CalibSite == "RM"):
# ROMA
  MdtValidationAlg.ConnectionString = "oracle://atlas-oracle-01.roma1.infn.it/atlrome.roma1.infn.it/atlas_muoncalib_rome"
  MdtValidationAlg.WorkingSchema = "atlas_muoncalib_rome"
elif (CalibSite == "RM_NEW"):
# ROMA
  MdtValidationAlg.ConnectionString = "oracle://atlas-cluster.cr.cnaf.infn.it:1521/atlcnafsrv.cr.cnaf.infn.it/ATLAS_MUONCALIB_READER"
  MdtValidationAlg.WorkingSchema = "ATLAS_MUONCALIB_RM_META"
  MdtValidationAlg.WriterConnectionString = "oracle://atlas-cluster.cr.cnaf.infn.it:1521/atlcnafsrv.cr.cnaf.infn.it/ATLAS_MUONCALIB_WRITER"

MdtValidationAlg.GetLimits = 0
MdtValidationAlg.MinSlope = 1.
MdtValidationAlg.MaxSlope = 5
#  Run2 Values
MdtValidationAlg.MinT0 = -100
MdtValidationAlg.MaxT0 = 400
#MdtValidationAlg.MinT0 = 190
#MdtValidationAlg.MaxT0 = 850
MdtValidationAlg.HeadId = runArgs.headId 
#MdtValidationAlg.GetLimits = runArgs.getLimits

#MdtValidationAlg.ValidationTask = "RtVal"
#MdtValidationAlg.ValidationTask = "T0Val"
MdtValidationAlg.ValidationTask = runArgs.calibrationAlgorithm

if mdtCalibFitConfig.postOptions is not None:
    include(mdtCalibFitConfig.postOptions)
