# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from PyUtils.MetaReader import read_metadata
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
    from AtlasGeoModel.AtlasGeoDBInterface import AtlasGeoDBInterface

    dbGeomCursor = AtlasGeoDBInterface(geoTag)
    dbGeomCursor.ConnectAndBrowseGeoDB()

    params = { 'Common' : CommonGeoDB.InitializeGeometryParameters(dbGeomCursor),
               'Pixel' : PixelGeoDB.InitializeGeometryParameters(dbGeomCursor),
               'LAr' : LArGeoDB.InitializeGeometryParameters(dbGeomCursor),
               'Muon' : MuonGeoDB.InitializeGeometryParameters(dbGeomCursor) }

    return params


@lru_cache(maxsize=4)  # maxsize=1 should be enough for most jobs
def DetDescrInfo(geoTag):
    """Query geometry DB for detector description. Returns dictionary with
    detector description. Queries DB for each tag only once.

    geoTag: gemometry tag (e.g. ATLAS-R2-2016-01-00-01)
    """
    detDescrInfo = _initializeGeometryParameters(geoTag)
    detDescrInfo["geomTag"] = geoTag
    return detDescrInfo


# Based on RunDMCFlags.py
def getRunToTimestampDict():
    # this wrapper is intended to avoid an initial import
    from .RunToTimestampData import RunToTimestampDict
    return RunToTimestampDict


def getInitialTimeStampsFromRunNumbers(runNumbers):
    """This is used to hold a dictionary of the form
    {152166:1269948352889940910, ...} to allow the
    timestamp to be determined from the run.
    """
    run2timestampDict =  getRunToTimestampDict()
    timeStamps = [run2timestampDict.get(runNumber,1) for runNumber in runNumbers] # Add protection here?
    return timeStamps
