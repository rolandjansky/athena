# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags

def InitializeGeometryParameters(dbGeomCursor):
    """Read geometry parameters for Pixel

    dbGeomCursor: AtlasGeoDBInterface instance
    """

    # ----------------------------------------------------------------------------
    # Read versionname, layout and dbm from PixelSwitches table

    dbId,dbSwitches,dbParam = dbGeomCursor.GetCurrentLeafContent("PixelSwitches")

    params = {"VersionName" : "UNDEFINED",
              "Layout" : "UNDEFINED",
              "DBM" : False }

    if len(dbId)>0:
        key=dbId[0]
        if "VERSIONNAME" in dbParam:
            params["VersionName"] = dbSwitches[key][dbParam.index("VERSIONNAME")]
        if "LAYOUT" in dbParam :
            params["Layout"] = dbSwitches[key][dbParam.index("LAYOUT")]
        if "BUILDDBM" in dbParam :
            params["DBM"] = (dbSwitches[key][dbParam.index("BUILDDBM")] != 0)


    # ----------------------------------------------------------------------------
    # IBL layout

    dbId,dbLayers,dbParam = dbGeomCursor.GetCurrentLeafContent("PixelLayer")
    IBLStaveIndex = -1
    IBLgeoLayout = -1
    params["IBLlayout"] = "noIBL"
    if len(dbId)>0:
        key=dbId[0]
        if "STAVEINDEX" in dbParam and dbLayers[key][dbParam.index("STAVEINDEX")] not in ["NULL",None]:
            IBLStaveIndex = int(dbLayers[key][dbParam.index("STAVEINDEX")])

        if IBLStaveIndex>-1:
            dbId,dbStaves,dbParam = dbGeomCursor.GetCurrentLeafContent("PixelStave")

            if len(dbId)>0 and IBLStaveIndex<=len(dbStaves.keys()):
                key=dbId[IBLStaveIndex]
                if "LAYOUT" in dbParam and dbStaves[key][dbParam.index("LAYOUT")] not in ["NULL",None]:
                    IBLgeoLayout = int(dbStaves[key][dbParam.index("LAYOUT")])
                    if IBLgeoLayout in [3,4] : params["IBLlayout"] = "planar"
                    elif IBLgeoLayout in [5] : params["IBLlayout"] = "3D"

    # ----------------------------------------------------------------------------
    # IBL and SLHC parameters

    params["IBL"] = (params["Layout"] == 'IBL')
    params["SLHC"] = (params["Layout"] == 'SLHC')

    if params["IBL"] is False:
       params["IBLlayout"] = "noIBL"
    if params["Layout"]!='SLHC' and ( CommonGeometryFlags.Run() in ["RUN2", "RUN3"] ) :
       params["IBL"] = True

    return params
