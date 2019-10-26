#
# Joboptions for bootstrapping the loading of the DetectorStore with
# the DetDescrCnvSvc
#
# DLLs 
theApp.Dlls += [ "DetDescrCnvSvc" ]

# Must create the service
theApp.CreateSvc += [ "DetDescrCnvSvc" ]

# Set DetDescrCnvSvc to be a proxy/address provider
#ProxyProviderSvc.ProviderNames += { "DetDescrCnvSvc" };
EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices += [ "DetDescrCnvSvc" ]

# Specify primary Identifier dictionary to be used
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.IdDictName = "IdDictParser/ATLAS_IDS.xml"

# Specify whether CSC/sTgc/MM muon chambers are part of the detector
from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
DetDescrCnvSvc.HasCSC = MuonGeometryFlags.hasCSC()
DetDescrCnvSvc.HasSTgc = (CommonGeometryFlags.Run() in ["RUN3", "RUN4"])
DetDescrCnvSvc.HasMM = (CommonGeometryFlags.Run() in ["RUN3", "RUN4"])

# Specify the tag to be used
# DetDescrCnvSvc.IdDictGlobalTag = "initial_layout"

# Always include the IdDict jobOptions
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

