#--------------------------------------------------------------
# Detector Description
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

# GeoModel
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# Identifier 
ToolSvc = Service("ToolSvc") 
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
#include( "LArRawConditions/LArIdMap_ATLAS_jobOptions.py" )

#--------------------------------------------------------------
# Other shared libs
#--------------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "LArRawUtils" ]
theApp.Dlls += [ "LArTools" ]
theApp.Dlls += [ "LArConditionsTest" ]

# Make sure AthenaPoolServices is loaded for custom streamer
include ("AthenaPoolServices/AthenaRootStreamerSvc_jobOptions.py")
include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

# LAr Converters
include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )
#include ("LArRawConditions/LArRawConditionsDict_joboptions.py")
#include ("LArIdentifier/LArIdentifierDict_joboptions.py")

#--------------------------------------------------------------
# Py related. 
#--------------------------------------------------------------
# interactive Athena
from PyKernel import PyKernel
# this must be the last one, since proxy classes are patched by this
include ('PyKernel/InitPyKernel.py')

