# Geometry setup. This file is "include"'d in each jobOptions. So changing a geometry
# need only be done here (but, you will affect all jobs that use this file)

from AthenaCommon.GlobalFlags import globalflags

# Select the geometry version. 
globalflags.DetDescrVersion = 'ATLAS-SLHC-02-00-00'
#globalflags.DetDescrVersion = 'ATLAS-P2-ITK-01-00-00' - doesn't work, also with _VALIDATION extension

# import the conditions setup
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-MC12-ITK-27-00')
       
# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.SLHC_Version = "SLHC-empty-gmx"
printfunc ( SLHC_Flags)

# .gmx top-file:
from SCT_GeoModelXml.SCT_GeoModelXmlConf import SCT_GMX_DetectorTool
SCT_GMX_DetectorTool.GmxFilename="../../../InDetDetDescr/GmxLayouts/data/ITk/Strip/LoI/ITkStrip.gmx"

# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
# simFlags.SimLayout='ATLAS-SLHC-02-00-00_VALIDATION'
simFlags.SimLayout='ATLAS-SLHC-02-00-00'
simFlags.SimLayout.set_On()
simFlags.EventFilter.set_On()

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if hasattr(condSeq, "SCTSiLorentzAngleCondAlg"):
    # condSeq.SCTSiLorentzAngleCondAlg.OutputLevel=VERBOSE
    condSeq.SCTSiLorentzAngleCondAlg.SiConditionsTool=""
    condSeq.SCTSiLorentzAngleCondAlg.Temperature=-20.
    condSeq.SCTSiLorentzAngleCondAlg.BiasVoltage=400.
    condSeq.SCTSiLorentzAngleCondAlg.DepletionVoltage=70.

