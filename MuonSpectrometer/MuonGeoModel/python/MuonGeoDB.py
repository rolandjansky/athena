# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def InitializeGeometryParameters(dbGeomCursor):
    """Read muon geometry parameters

    dbGeomCursor: AtlasGeoDBInterface instance
    """
    dbId, dbSwitches, dbParam = dbGeomCursor.GetCurrentLeafContent("MuonSwitches")
    params = {"Layout" : "UNDEFINED",
              "HasCSC" : True,
              "HasSTGC" : True,
              "HasMM" : True}

    if len(dbId)>0:
        key=dbId[0]
        if "LAYOUTNAME" in dbParam:
            params["Layout"] = dbSwitches[key][dbParam.index("LAYOUTNAME")]
        if "HASCSC" in dbParam:
            params["HasCSC"] = (dbSwitches[key][dbParam.index("HASCSC")] != 0)
        if "HASSTGC" in dbParam:
            params["HasSTGC"] = (dbSwitches[key][dbParam.index("HASSTGC")] !=0)
        if "HASMM" in dbParam:
            params["HasMM"] = (dbSwitches[key][dbParam.index("HASMM")] != 0)

    return params
