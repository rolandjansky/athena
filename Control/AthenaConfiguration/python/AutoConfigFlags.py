# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from PyUtils.MetaReader import read_metadata
from AtlasGeoModel.AtlasGeoDBInterface import AtlasGeoDBInterface

#Module level cache of file-metadata:
_fileMetaData=dict()

#Module level cache of DDDB cursor
_dbGeomCursor = 0

#Module level dictionary of DDDB information:
_detDescrInfo = dict()


def GetFileMD(filenames):
    from AthenaCommon.Logging import logging
    msg = logging.getLogger('AutoConfigFlags')
    filename=filenames[0]
    if filename not in _fileMetaData:
        if len(filenames)>1:
            msg.info("Multiple input files. Use the first one for auto-configuration")
        msg.info("Obtaining metadata of auto-configuration by peeking into %s", filename)


        thisFileMD=read_metadata(filename,None,'peeker')
        _fileMetaData.update(thisFileMD)

    return _fileMetaData[filename]


def initializeGeometryParameters():
    # ----------------------------------------------------------------------------
    # Connect to database

    bVerbose = False
    _dbGeomCursor = AtlasGeoDBInterface(_detDescrInfo["geomTag"],bVerbose)
    _dbGeomCursor.ConnectAndBrowseGeoDB()

    # ----------------------------------------------------------------------------
    # Read version name, layout and dbm from AtlasCommon table

    dbId,dbCommon,dbParam = _dbGeomCursor.GetCurrentLeafContent("AtlasCommon")

    _run = "UNDEFINED"
    _geotype = "UNDEFINED"
    _stripgeotype = "UNDEFINED"
    if len(dbId)>0:
        key=dbId[0]
        if "CONFIG" in dbParam :
            _run = dbCommon[key][dbParam.index("CONFIG")]
        if "GEOTYPE" in dbParam :
            _geotype = dbCommon[key][dbParam.index("GEOTYPE")]
        if "STRIPGEOTYPE" in dbParam :
            _stripgeotype = dbCommon[key][dbParam.index("STRIPGEOTYPE")]

    _detDescrInfo["Run"]=_run
    _detDescrInfo["GeoType"]=_geotype
    _detDescrInfo["StripGeoType"]=_stripgeotype

    # ----------------------------------------------------------------------------
    # Read version name, layout and dbm from PixelSwitches table

    dbId,dbSwitches,dbParam = _dbGeomCursor.GetCurrentLeafContent("PixelSwitches")

    _versionName="UNDEFINED"
    _layout="UNDEFINED"
    _dbm = False

    if len(dbId)>0:
        key=dbId[0]
        if "VERSIONNAME" in dbParam:
            _versionName = dbSwitches[key][dbParam.index("VERSIONNAME")]
        if "LAYOUT" in dbParam :
            _layout = dbSwitches[key][dbParam.index("LAYOUT")]
        if "BUILDDBM" in dbParam :
            _dbm = (dbSwitches[key][dbParam.index("BUILDDBM")] != 0)


    _detDescrInfo["VersionName"] = _versionName
    _detDescrInfo["Layout"] = _layout
    _detDescrInfo["DBM"] = _dbm

    # ----------------------------------------------------------------------------
    # IBL layout

    dbId,dbLayers,dbParam = _dbGeomCursor.GetCurrentLeafContent("PixelLayer")
    IBLStaveIndex = -1
    IBLgeoLayout = -1
    _IBLlayout = "noIBL"
    if len(dbId)>0:
        key=dbId[0]
        if "STAVEINDEX" in dbParam and dbLayers[key][dbParam.index("STAVEINDEX")] not in ["NULL",None]:
            IBLStaveIndex = int(dbLayers[key][dbParam.index("STAVEINDEX")])

        if IBLStaveIndex>-1:
            dbId,dbStaves,dbParam = _dbGeomCursor.GetCurrentLeafContent("PixelStave")

            if len(dbId)>0 and IBLStaveIndex<=len(dbStaves.keys()):
                key=dbId[IBLStaveIndex]
                if "LAYOUT" in dbParam and dbStaves[key][dbParam.index("LAYOUT")] not in ["NULL",None]:
                    IBLgeoLayout = int(dbStaves[key][dbParam.index("LAYOUT")])
                    if IBLgeoLayout in [3,4] :
                        _IBLlayout = "planar"
                    elif IBLgeoLayout in [5] :
                        _IBLlayout = "3D"

    _detDescrInfo["IBLlayout"]=_IBLlayout

    # ----------------------------------------------------------------------------
    # IBL and SLHC parameters

    _detDescrInfo["IBL"] = False
    _detDescrInfo["SLHC"] = False
    if _layout in ['IBL'] :
        _detDescrInfo["IBL"] = True
    if _detDescrInfo["IBL"] is False:
        _detDescrInfo["IBLlayout"] = "noIBL"
    if _layout not in ['SLHC'] and ( _detDescrInfo["Run"] in ["RUN2", "RUN3"] ) :
        _detDescrInfo["IBL"] = True
    if _layout in ['SLHC'] :
        _detDescrInfo["SLHC"] = True

    # ----------------------------------------------------------------------------
    # Read version name, layout and dbm from LArSwitches table

    dbId,dbSwitches,dbParam = _dbGeomCursor.GetCurrentLeafContent("LArSwitches")
    _sagging = None
    _barrelOn = None
    _endcapOn = None
    _FCalFlag="UNDEFINED"
    _detAbsorber = None
    _detAbsorber_EC = None

    if len(dbId)>0:
        key=dbId[0]
        if "SAGGING" in dbParam:
            _sagging = dbSwitches[key][dbParam.index("SAGGING")]
        if "BARREL_ON" in dbParam:
            _barrelOn = dbSwitches[key][dbParam.index("BARREL_ON")]
        if "ENDCAP_ON" in dbParam :
            _endcapOn = dbSwitches[key][dbParam.index("ENDCAP_ON")]
        if "DETAILED_ABSORBER" in dbParam :
            _detAbsorber = dbSwitches[key][dbParam.index("DETAILED_ABSORBER")]
        if "DETAILED_ABSORBER_EC" in dbParam :
            _detAbsorber_EC = dbSwitches[key][dbParam.index("DETAILED_ABSORBER_EC")]
        if "FCAL_GEOTYPE" in dbParam :
            _FCalFlag = dbSwitches[key][dbParam.index("FCAL_GEOTYPE")]

    _detDescrInfo["Sagging"] = _sagging
    _detDescrInfo["BarrelOn"] = _barrelOn
    _detDescrInfo["EndcapOn"] = _endcapOn
    _detDescrInfo["FCal_GeoType"] = _FCalFlag
    _detDescrInfo["DetAbs"] = _detAbsorber
    _detDescrInfo["DetAbs_EC"] = _detAbsorber_EC
    return

def GetDetDescrInfo(geoTag):
    if _dbGeomCursor == 0:
        # set geometry tag name
        _detDescrInfo["geomTag"] = geoTag
        initializeGeometryParameters()
    return _detDescrInfo
