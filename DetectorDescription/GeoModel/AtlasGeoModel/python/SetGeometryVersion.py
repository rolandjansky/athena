# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.DetFlags    import DetFlags

DDversion    = jobproperties.Global.DetDescrVersion()

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc

GeoModelSvc = GeoModelSvc()
DetDescrCnvSvc = DetDescrCnvSvc()

DataSrc =  jobproperties.Global.DataSource()
DGeo    =  jobproperties.Global.DetGeo()

if (DGeo == "ctbh8"):
    DDversion = "CTB"

elif (DDversion == "CTB"):
    GeoModelSvc.AtlasVersion = "ATLAS-CTB-01"
    if (DetFlags.Tile_on()): 
       GeoModelSvc.TileVersionOverride = "TileTB-3B-00"

    DetDescrCnvSvc.InDetIDFileName="IdDictParser/IdDictInnerDetector_CTB2004.xml"
    DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_H8_2004.xml"

elif (DDversion == "ATLAS-H6-2004-00"):
    GeoModelSvc.AtlasVersion = "ATLAS-H6-2004-00"
    DetFlags.ID_setOff()
    DetFlags.Muon_setOff()
    DetFlags.Tile_setOff()

elif (DDversion == "ATLAS-H6-2003-00"):
    GeoModelSvc.AtlasVersion = "ATLAS-H6-2003-00"
    DetFlags.HEC_setOff()
    DetFlags.em_setOff()

elif (DDversion == "ATLAS-H6-2002-00"):
    GeoModelSvc.AtlasVersion = "ATLAS-H6-2002-00"
    DetFlags.FCal_setOff()

elif (DDversion == "ATLAS-TBEC-00"):
    GeoModelSvc.AtlasVersion = "ATLAS-TBEC-00"
    DetFlags.HEC_setOff()
    DetFlags.FCal_setOff()

else:
    GeoModelSvc.AtlasVersion = DDversion

# Set the SupportedGeometry flag
from AthenaCommon.AppMgr import release_metadata
rel_metadata = release_metadata()
relversion = rel_metadata['release'].split('.')
if len(relversion) < 3:
    relversion = rel_metadata['base release'].split('.')

print "SetGeometryVersion.py obtained major release version %s" % relversion[0]
GeoModelSvc.SupportedGeometry = int(relversion[0])
