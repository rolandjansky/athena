# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from PyUtils.MetaReader import read_metadata
from AthenaCommon.Logging import logging
from functools import lru_cache

msg = logging.getLogger('AutoConfigFlags')

# Module level cache of file-metadata:
_fileMetaData = dict()

class DynamicallyLoadMetadata:
    def __init__(self, filename):
        self.metadata = {}
        self.filename = filename
        self.metAccessLevel = 'lite'
        thisFileMD = read_metadata(filename, None, 'lite')
        self.metadata.update(thisFileMD[self.filename])
        msg.debug("Loaded using 'lite' %s", str(self.metadata))

    def _loadMore(self):
        thisFileMD = read_metadata(self.filename, None, 'peeker')
        self.metadata.update(thisFileMD[self.filename])

    def get(self, key, default):
        if key in self.metadata:
            return self.metadata[key]
        if self.metAccessLevel != 'peeker':
            msg.info("Looking into the file in 'peeker' mode as the configuration requires more details: %s ", key)
            self.metAccessLevel = 'peeker'
            self._loadMore()
            if key in self.metadata:
                return self.metadata[key]
        return default

    def __contains__(self, key):
        self.get(key, None)
        return key in self.metadata

    def __getitem__(self, key):
        return self.get(key, None)

    def __repr__(self):
        return repr(self.metadata)

    def keys(self):
        return self.metadata.keys()

def GetFileMD(filenames):
    if isinstance(filenames, str):
        filenames = [filenames]
    if ['_ATHENA_GENERIC_INPUTFILE_NAME_'] == filenames:
        raise RuntimeError('Input file name not set, instead _ATHENA_GENERIC_INPUTFILE_NAME_ found. Cannot read metadata.')
    for filename in filenames:
        if filename not in _fileMetaData:
            msg.info("Obtaining metadata of auto-configuration by peeking into '%s'", filename)
            _fileMetaData[filename] = DynamicallyLoadMetadata(filename)
        if _fileMetaData[filename]['nentries'] not in [None, 0]: 
            return _fileMetaData[filename]
        else:
            msg.info("The file: %s has no entries, going to the next one for harvesting the metadata", filename)
    msg.info("No file with events found, returning anyways metadata associated to the first file %s", filenames[0])
    return _fileMetaData[filenames[0]]

def _initializeGeometryParameters(geoTag):
    """Read geometry database for all detectors"""

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
               'Muon' : MuonGeoDB.InitializeGeometryParameters(dbGeomCursor),
               'Luminosity' : CommonGeoDB.InitializeLuminosityDetectorParameters(dbGeomCursor),
             }

    return params


@lru_cache(maxsize=4)  # maxsize=1 should be enough for most jobs
def DetDescrInfo(geoTag):
    """Query geometry DB for detector description. Returns dictionary with
    detector description. Queries DB for each tag only once.

    geoTag: geometry tag (e.g. ATLAS-R2-2016-01-00-01)
    """
    detDescrInfo = _initializeGeometryParameters(geoTag)
    detDescrInfo["geomTag"] = geoTag
    return detDescrInfo


@lru_cache(maxsize=4)  # maxsize=1 should be enough for most jobs
def getDefaultDetectors(geoTag):
    """Query geometry DB for detector description.
    Returns a set of detectors used in a geometry tag.

    geoTag: geometry tag (e.g. ATLAS-R2-2016-01-00-01)
    """
    detectors = set()
    detectors.add('Bpipe')

    if DetDescrInfo(geoTag)['Common']['Run'] not in ['RUN1', 'RUN2', 'RUN3']: # RUN4 and beyond
        detectors.add('ITkPixel')
        detectors.add('ITkStrip')
        if DetDescrInfo(geoTag)['Luminosity']['BCMPrime']:
            pass  # keep disabled for now
        if DetDescrInfo(geoTag)['Luminosity']['PLR']:
            detectors.add('PLR')
    else:
        detectors.add('Pixel')
        detectors.add('SCT')
        detectors.add('TRT')
        detectors.add('BCM')
    # TODO: wait for special table in the geo DB
    # if DetDescrInfo(geoTag)['Common']['Run'] == 'RUN4':
    #     detectors.add('BCMPrime')
    if DetDescrInfo(geoTag)['Common']['Run'] in ['RUN1', 'RUN2', 'RUN3'] and DetDescrInfo(geoTag)['Pixel']['DBM']:
        detectors.add('DBM')

    if DetDescrInfo(geoTag)['Common']['Run'] not in ['RUN1', 'RUN2', 'RUN3']: # RUN4 and beyond
        detectors.add('HGTD')

    detectors.add('LAr')
    detectors.add('Tile')
    if DetDescrInfo(geoTag)['Common']['Run'] in ['RUN1', 'RUN2', 'RUN3']:
        detectors.add('MBTS')

    detectors.add('MDT')
    detectors.add('RPC')
    detectors.add('TGC')
    if DetDescrInfo(geoTag)['Muon']['HasCSC']:
        detectors.add('CSC')
    if DetDescrInfo(geoTag)['Muon']['HasSTGC']:
        detectors.add('sTGC')
    if DetDescrInfo(geoTag)['Muon']['HasMM']:
        detectors.add('MM')

    return detectors


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


def getSpecialConfigurationMetadata(inputFiles, secondaryInputFiles):
    """Read in special simulation job option fragments based on metadata
    passed by the evgen stage
    """
    specialConfigDict = dict()
    legacyPreIncludeToCAPostInclude = { 'SimulationJobOptions/preInclude.AMSB.py' : 'Charginos.CharginosConfigNew.AMSB_Cfg',
                                        'SimulationJobOptions/preInclude.Monopole.py' :  'Monopole.MonopoleConfigNew.MonopoleCfg',
                                        'SimulationJobOptions/preInclude.Quirks.py' : 'Quirks.QuirksConfigNew.QuirksCfg',
                                        'SimulationJobOptions/preInclude.SleptonsLLP.py' : 'Sleptons.SleptonsConfigNew.SleptonsLLPCfg',
                                        'SimulationJobOptions/preInclude.GMSB.py' : 'Sleptons.SleptonsConfigNew.GMSB_Cfg',
                                        'SimulationJobOptions/preInclude.Qball.py' : 'Monopole.MonopoleConfigNew.QballCfg',
                                        'SimulationJobOptions/preInclude.RHadronsPythia8.py' : None, # FIXME
                                        'SimulationJobOptions/preInclude.fcp.py' : 'Monopole.MonopoleConfigNew.fcpCfg' }
    legacyPreIncludeToCAPreInclude = { 'SimulationJobOptions/preInclude.AMSB.py' : None,
                                       'SimulationJobOptions/preInclude.Monopole.py' :  'Monopole.MonopoleConfigNew.MonopolePreInclude',
                                       'SimulationJobOptions/preInclude.Quirks.py' : None,
                                       'SimulationJobOptions/preInclude.SleptonsLLP.py' : None,
                                       'SimulationJobOptions/preInclude.GMSB.py' : None,
                                       'SimulationJobOptions/preInclude.Qball.py' : 'Monopole.MonopoleConfigNew.QballPreInclude',
                                       'SimulationJobOptions/preInclude.RHadronsPythia8.py' : None, # FIXME
                                       'SimulationJobOptions/preInclude.fcp.py' : 'Monopole.MonopoleConfigNew.fcpPreInclude' }
    specialConfigString = ''
    from AthenaConfiguration.AutoConfigFlags import GetFileMD
    if len(inputFiles)>0:
        specialConfigString = GetFileMD(inputFiles).get('specialConfiguration', '')
    if (not len(specialConfigString) or specialConfigString == 'NONE') and len(secondaryInputFiles)>0:
        # If there is no specialConfiguration metadata in the primary
        # input try the secondary inputs (MC Overlay case)
        specialConfigString = GetFileMD(secondaryInputFiles).get('specialConfiguration', '')
    if len(specialConfigString)>0:
        ## Parse the specialConfiguration string
        ## Format is 'key1=value1;key2=value2;...'. or just '
        spcitems = specialConfigString.split(";")
        for spcitem in spcitems:
            #print spcitem
            ## Ignore empty or "NONE" substrings, e.g. from consecutive or trailing semicolons
            if not spcitem or spcitem.upper() == "NONE":
                continue
            ## If not in key=value format, treat as v, with k="preInclude"
            if "=" not in spcitem:
                spcitem = "preInclude=" + spcitem
            ## Handle k=v directives
            k, v = spcitem.split("=")
            if k == "preInclude" and v.endswith('.py'): # Translate old preIncludes into CA-based versions.
                v1 = legacyPreIncludeToCAPreInclude[v]
                if v1 is not None:
                    specialConfigDict[k] = v1
                v2 = legacyPreIncludeToCAPostInclude[v]
                if v2 is not None:
                    specialConfigDict['postInclude'] = v2
            else:
                specialConfigDict[k] = v
    return specialConfigDict

