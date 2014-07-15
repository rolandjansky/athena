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

if (DataSrc == "geant3"):
    DDversion = "DC1"

if (DGeo == "ctbh8"):
    DDversion = "CTB"

if (DDversion == "DC2" or DDversion == "DC2-Final"):
    GeoModelSvc.AtlasVersion = "ATLAS-DC2-00" 

    DetDescrCnvSvc.IdDictGlobalTag = "final_layout"
    DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml"
    
elif (DDversion == "DC2-Initial"):
    GeoModelSvc.AtlasVersion = "ATLAS-DC2-00"
    if (DetFlags.ID_on()): 
       GeoModelSvc.InDetVersionOverride = "InnerDetector-DC2-Initial-00"
    
    DetDescrCnvSvc.IdDictGlobalTag = "initial_layout"
    DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml"
    
elif (DDversion == "Rome-Initial"):
    GeoModelSvc.AtlasVersion = "ATLAS-Rome-Initial-02" 

    DetDescrCnvSvc.IdDictGlobalTag = "initial_layout"
    DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_Q.02.xml"

elif (DDversion == "Rome-Initial-v00"):
    GeoModelSvc.AtlasVersion = "ATLAS-Rome-Initial-01" 

    DetDescrCnvSvc.IdDictGlobalTag = "initial_layout"
    DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_Q.02.xml"

elif (DDversion == "Rome-Final"):
    GeoModelSvc.AtlasVersion = "ATLAS-Rome-Final-00" 

    DetDescrCnvSvc.IdDictGlobalTag = "final_layout"
    DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_Q.02.xml"
                                       
elif (DDversion == "DC1"):
    GeoModelSvc.AtlasVersion = "ATLAS-DC2-00"
    if (DetFlags.ID_on()): 
       GeoModelSvc.InDetVersionOverride = "InnerDetector-DC1-00"

    DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml"

elif (DDversion == "DC1-Initial"):
    GeoModelSvc.AtlasVersion = "ATLAS-DC2-00" 
    if (DetFlags.ID_on()): 
       GeoModelSvc.InDetVersionOverride = "InnerDetector-DC1-Initial-00"

    DetDescrCnvSvc.IdDictGlobalTag = "initial_layout"
    DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml"
 
elif (DDversion == "DC1-Final"):
    GeoModelSvc.AtlasVersion = "ATLAS-DC2-00"
    if (DetFlags.ID_on()): 
       GeoModelSvc.InDetVersionOverride = "InnerDetector-DC1-Final-00"

    DetDescrCnvSvc.IdDictGlobalTag = "final_layout"
    DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml"
 
elif (DDversion == "CTB"):
    GeoModelSvc.AtlasVersion = "ATLAS-CTB-00"
    if (DetFlags.Tile_on()): 
       GeoModelSvc.TileVersionOverride = "TileTB-3B-00"

    DetDescrCnvSvc.InDetIDFileName="IdDictParser/IdDictInnerDetector_CTB2004.xml"
    DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_H8_2004.xml"

elif (DDversion == "Commissioning-Calo"):
    GeoModelSvc.AtlasVersion = "ATLAS-Commissioning-Calo-00"
    DetFlags.ID_setOff()
    DetFlags.Calo_setOn()
    DetFlags.Muon_setOff()

elif (DDversion == "Commissioning-Muon"):
    GeoModelSvc.AtlasVersion = "ATLAS-Commissioning-Muon-00"
    DetFlags.ID_setOff()
    DetFlags.Calo_setOff()
    DetFlags.Muon_setOn()    

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

