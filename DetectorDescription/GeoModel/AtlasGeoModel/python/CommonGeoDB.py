# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def InitializeGeometryParameters(dbGeomCursor):
   """Read version name, layout and dbm from AtlasCommon table.

   dbGeomCursor: AtlasGeoDBInterface instance
   """

   dbId, dbCommon, dbParam = dbGeomCursor.GetCurrentLeafContent("AtlasCommon")

   params = {"Run" : "UNDEFINED",
             "GeoType" : "UNDEFINED"}

   if len(dbId)>0:
      key = dbId[0]
      if "CONFIG" in dbParam :
         params["Run"] = dbCommon[key][dbParam.index("CONFIG")]
      if "GEOTYPE" in dbParam :
         params["GeoType"] = dbCommon[key][dbParam.index("GEOTYPE")]

   return params
