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


def InitializeLuminosityDetectorParameters(dbGeomCursor):
   """Read luminosity detectors from the DB

   dbGeomCursor: AtlasGeoDBInterface instance
   """

   dbId, dbCommon, dbParam = dbGeomCursor.GetCurrentLeafContent("LuminositySwitches")

   params = {"BCMPrime" : False,
             "PLR" : False}

   if len(dbId):
      key = dbId[0]
      if "BCMPRIME" in dbParam :
         params["BCMPrime"] = dbCommon[key][dbParam.index("BCMPRIME")] == 1
      if "PLR" in dbParam :
         params["PLR"] = dbCommon[key][dbParam.index("PLR")] == 1

   return params
