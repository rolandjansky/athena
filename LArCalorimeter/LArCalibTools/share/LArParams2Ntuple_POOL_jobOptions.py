###########################################################################
#
# Read parameters from POOL file and save them in ROOT Ntuple
#
# Author: Marcello.Fanti@mi.infn.it
#         Marco Delmastro@cern.ch
#
# Last update 16/3/2006
#
###########################################################################

# List of the input data file(s)
PoolFileNameList  = [ "./params.pool.root" ]

# Pool file catalog location
PoolFileCatalog   = "./PoolFileCatalog.xml"

# List of the data structure to dump. Possible structures are:
#
# - LArCaliPulseParamsComplete
# - LArDetCellParamsComplete
# - LArPhysCaliTdiffComplete
# - LArTdriftComplete
# - LArMphysOverMcalComplete
# - LArRinjComplete
# - LArTshaperComplete
# - LArEMEC_CphiComplete
# - LArEMEC_HValphaComplete
# - LArEMEC_HVbetaComplete
# - LArCableLengthComplete
# - LArCableAttenuationComplete
#
# - LArCaliPulseParamsVsCalib
#
# possible examples:
#DataStructureList = [ "LArCaliPulseParamsComplete", "LArDetCellParamsComplete" ]
#DataStructureList = [ "LArCaliPulseParamsVsCalib" ]

# Suffix to be added at the end fo the param class name to generate detStore key
#DetStoreSuffix    = "_RTM"
DetStoreSuffix    = ""

# switch detector description (H8, ATLAS)
DetDescription    = "ATLAS" 

# Output ROOT file name
NtupleFileName    = "LArParams2Ntuple.root" 

###########################################################################

#
# Detector description
#
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers += ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += ["CaloMgr"]

include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" ) 

if ( DetDescription == "ATLAS" ) :
	DetDescrVersion = "ATLAS-DC3-05"
	include( "LArIdCnv/LArIdCnv_joboptions.py" )
	include( "LArCondCnv/LArCondCnv_IdMapAtlas_jobOptions.py" )
        #don't work, due to some G4-related stuff... include("LArDetDescr/LArDetDescr_joboptions.py")

elif ( DetDescription == "H8" ) :
	include( "LArDetDescr/LArDetDescr_H8_joboptions.py" )
	DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
	DetDescrCnvSvc.DoIdChecks
	DetDescrCnvSvc.LArIDFileName ="IdDictParser/IdDictLArCalorimeter_H8_2004.xml"

#
# Flags: LAr only
#
from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.em_setOn() 

# the following is necessary for absolute eta,phi mapping
include ("AtlasGeoModel/SetGeometryVersion.py")
include ("AtlasGeoModel/GeoModelInit.py")

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print = False

#
# Read in from POOL file
#
include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
theApp.Dlls += ["EventSelectorAthenaPool"] 
theApp.Dlls += [ "LArAthenaPoolPoolCnv" ]
theApp.Dlls += [ "LArCondAthenaPoolPoolCnv" ]

CondProxyProvider = Service( "CondProxyProvider" )
ProxyProviderSvc  = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

CondProxyProvider = Service( "CondProxyProvider" )
CondProxyProvider.InputCollections += PoolFileNameList
CondProxyProvider.OutputLevel = DEBUG

EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

PoolSvc = Service ( "PoolSvc" )
PoolSvc.ReadCatalog += [ "xmlcatalog_file:"+PoolFileCatalog ]

#
# Write out to ROOT file
#

theApp.Dlls += ["LArCalibTools"]

theApp.TopAlg += [ "LArParams2Ntuple" ]
LArParams2Ntuple=Algorithm( "LArParams2Ntuple" )
LArParams2Ntuple.OutputLevel = INFO
LArParams2Ntuple.KeyList += DataStructureList
LArParams2Ntuple.DetStoreSuffix = DetStoreSuffix

theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='"+NtupleFileName+"' OPT='NEW'" ]

###########################################################################

theApp.EvtMax = 1

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

###########################################################################
