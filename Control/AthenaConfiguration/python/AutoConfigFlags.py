# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from PyUtils.MetaReader import read_metadata
from AtlasGeoModel.AtlasGeoDBInterface import AtlasGeoDBInterface
from AthenaCommon.Logging import logging
from functools import lru_cache

msg = logging.getLogger('AutoConfigFlags')

# Module level cache of file-metadata:
_fileMetaData = dict()

def GetFileMD(filenames):
    filename=filenames[0]
    if filename == '_ATHENA_GENERIC_INPUTFILE_NAME_':
        raise RuntimeError('Input file name not set, instead _ATHENA_GENERIC_INPUTFILE_NAME_ found. Cannot read metadata.')
    if filename not in _fileMetaData:
        if len(filenames)>1:
            msg.info("Multiple input files. Use the first one for auto-configuration")
        msg.info("Obtaining metadata of auto-configuration by peeking into %s", filename)


        thisFileMD=read_metadata(filename,None,'peeker')
        _fileMetaData.update(thisFileMD)

    return _fileMetaData[filename]


def _initializeGeometryParameters(geoTag):
    """Read geometry database for all detetors"""

    from AtlasGeoModel import CommonGeoDB
    from PixelGeoModel import PixelGeoDB
    from LArGeoAlgsNV import LArGeoDB
    from MuonGeoModel import MuonGeoDB

    dbGeomCursor = AtlasGeoDBInterface(geoTag)
    dbGeomCursor.ConnectAndBrowseGeoDB()

    # FIXME: geometry parameter names need to be unique across systems!
    params = {}
    params.update(CommonGeoDB.InitializeGeometryParameters(dbGeomCursor))
    params.update(PixelGeoDB.InitializeGeometryParameters(dbGeomCursor))
    params.update(LArGeoDB.InitializeGeometryParameters(dbGeomCursor))
    params.update(MuonGeoDB.InitializeGeometryParameters(dbGeomCursor))

    return params


@lru_cache(maxsize=4)  # maxsize=1 should be enough for most jobs
def GetDetDescrInfo(geoTag):
    """Query geometry DB for detector description"""

    detDescrInfo = _initializeGeometryParameters(geoTag)
    detDescrInfo["geomTag"] = geoTag
    return detDescrInfo
