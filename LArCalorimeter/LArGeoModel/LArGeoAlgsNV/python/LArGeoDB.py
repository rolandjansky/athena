# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def InitializeGeometryParameters(dbGeomCursor):
    """Read versionname, layout and dbm from LArSwitches table

    dbGeomCursor: AtlasGeoDBInterface instance
    """

    dbId, dbSwitches, dbParam = dbGeomCursor.GetCurrentLeafContent("LArSwitches")
    params = {"Sagging" : None,
              "BarrelOn" : None,
              "EndcapOn" : None,
              "FCal_GeoType" : "UNDEFINED",
              "DetAbs" : None,
              "DetAbs_EC" : None}

    if len(dbId)>0:
        key=dbId[0]
        if "SAGGING" in dbParam:
            params["Sagging"] = dbSwitches[key][dbParam.index("SAGGING")]
        if "BARREL_ON" in dbParam:
            params["BarrelOn"] = dbSwitches[key][dbParam.index("BARREL_ON")]
        if "ENDCAP_ON" in dbParam :
            params["EndcapOn"] = dbSwitches[key][dbParam.index("ENDCAP_ON")]
        if "DETAILED_ABSORBER" in dbParam :
            params["DetAbs"] = dbSwitches[key][dbParam.index("DETAILED_ABSORBER")]
        if "DETAILED_ABSORBER_EC" in dbParam :
            params["DetAbs_EC"] = dbSwitches[key][dbParam.index("DETAILED_ABSORBER_EC")]
        if "FCAL_GEOTYPE" in dbParam :
            params["FCal_GeoType"] = dbSwitches[key][dbParam.index("FCAL_GEOTYPE")]

    return params
