# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file MuonRecStandaloneFlags.py Some extra flags available when running Muon Reconstruction using MuonRec_myTopOptions.py

## @namespace python::MuonRecStandaloneFlags @copydoc MuonRecStandaloneFlags.py

from AthenaCommon.JobProperties import JobProperty,jobproperties
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.Logging import logging
from MuonRecExample.MuonRecUtils import FileList, fillJobPropertyContainer
from MuonRecExample.DataFiles import DataInfo,DataFilesInfoMap
from MuonRecExample.MuonRecFlags import muonRecFlags,muonboyFlags,mooreFlags
from RecExConfig.RecFlags import rec

muonRecFlags.setDefaults()

## @brief Switch on VP1 event display. Only used in Muon Standalone.
#
# This flag should really be in AthenaCommon or RecExConfig
class doVP1(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    

_athenaCommonInputFilesFlags = {
    'BS'    : 'BSRDOInput'     ,
    'RAW'   : 'BSRDOInput'     ,  # RAW is alias for BS
    'EVGEN' : 'PoolRDOInput' ,
    'HITS'  : 'PoolRDOInput'  ,
    'RDO'   : 'PoolRDOInput'   ,
    'ESD'   : 'PoolESDInput'   ,
    'AOD'   : 'PoolAODInput'   ,
    'TAG'   : 'PoolTAGInput'
    }

_recFlagsReadFlags = {
    'BS'    : 'readRDO'  ,
    'RAW'   : 'readRDO'  ,  # RAW is alias for BS
    'EVGEN' : 'readRDO'  ,
    'HITS'  : 'readRDO'  ,
    'RDO'   : 'readRDO'  ,
    'ESD'   : 'readESD'  ,
    'AOD'   : 'readAOD'  ,
    'TAG'   : 'readTAG'  ,
    }

_poolDataTypes = [ 'EVGEN', 'HITS', 'RDO', 'ESD', 'AOD', 'TAG' ]
_byteStreamDataTypes = [ 'BS', 'RAW' ]

## @brief Select a set of input data.
#
# Only used in Muon Standalone running (MuonRec_myTopOptions.py)
# Possible values are given in DataFiles.py
# It serves several purposes:
# - shorthand for a standard list of input files
# - a database: it selects the correct data type, data format, geometry tag and conditions tag
# - it sets the flags rec.readRDO, .readESD, .readAOD etc. depending on input data type
# - it sets globalflags.InputFormat, .DetDescrVersion, .ConditionsTag
# - it assigns the input files to the correct athenaCommonFlags.PoolXYZInput and .BSRDOInput
class InputData(JobProperty):

    statusOn=False
    allowedTypes=['str']
    allowedValues=[]
    StoredValue=''

    selectedData = DataInfo([],'','','')
    _rawFiles = []
    _expandedFiles = []
    _maxFiles = 10
    _dataType = ''
    _detDescr = ''
    _condTag  = ''

    _log = logging.getLogger( 'muonRecFlags.InputData' )

    def _syncFileList(self):
        fileList = self.fileList()
        dataType = self.dataType()
        # ignore empty list
        if self._rawFiles and not fileList:
            self._log.warning("Not setting athenaCommonFlags.FilesInput to empty input filelist coming from %r",self._rawFiles)
            return
            
        oldList = athenaCommonFlags.FilesInput()
        if oldList != fileList:
            self._log.info("Setting athenaCommonFlags.FilesInput = %r",fileList)
            athenaCommonFlags.FilesInput = fileList
            
    def dataType(self):
        return self._dataType
    
    def setDataType(self,value):
        global athenaCommonFlags,rec,muonRecFlags,globalflags
        self._dataType = value
        # init all read flags to False
        for f in _recFlagsReadFlags.values():
            setattr(rec,f,False)
        # enable the chosen type
        try:
            recFlagToEnable = _recFlagsReadFlags[value]
        except KeyError:
            raise RuntimeError("Unknown rec.readXXX for dataType: %r"%value)
        else:
            self._log.info("Setting rec.%s = True", recFlagToEnable)
            setattr(rec,recFlagToEnable,True)
        
        # set input format
        if value in _poolDataTypes:
            self._log.info("Setting globalflags.InputFormat = 'pool'")
            globalflags.InputFormat = 'pool'
        elif value in _byteStreamDataTypes:
            self._log.info("Setting globalflags.InputFormat = 'bytestream'")
            globalflags.InputFormat = 'bytestream'
            # no Truth for bytestream
            self._log.info("Setting rec.doTruth = False for bytestream data format")
            rec.doTruth = False

        # synchronise doDigitization
        if value == 'HITS':
            if not muonRecFlags.doDigitization():
                self._log.info("Setting doDigitization = True" )
                muonRecFlags.doDigitization = True
        else:
            if muonRecFlags.doDigitization():
                self._log.info("Setting doDigitization = False" )
                muonRecFlags.doDigitization = False
                

    def fileList(self):
        if self._expandedFiles is None:
            self._expandedFiles = FileList.expandAndReadDirs(self._rawFiles)

        nFiles = min(self._maxFiles,len(self._expandedFiles))
        return self._expandedFiles[:nFiles-1]
    
    def setFileList(self,value):
        self._log.debug("Setting filelist to %r", value)
        # only set if list if different (maintain raw version of list)
        if value != self._rawFiles:
            self._rawFiles = value
            self._expandedFiles = None
            self._syncFileList()


    def maxFiles(self):
        return self._maxFiles

    def setMaxFiles(self,value):
        if value != self._maxFiles:
            self._maxFiles = value
            self._syncFileList()

    def detDescr(self):
        return self._detDescr
    
    def setDetDescr(self,value):
        global globalflags,rec
        self._detDescr = value
        # synchronise globalflags.DetDescrVersion
        oldDetDescr = globalflags.DetDescrVersion()
        if value != oldDetDescr:
            if value == 'AUTO':
                self._log.info("Adding FieldAndGeo to AutoConfig to configure DetDescrVersion and BField settings")
                if "FieldAndGeo" not in rec.AutoConfiguration():
                    rec.AutoConfiguration += [ 'FieldAndGeo' ]
            else:
                if "FieldAndGeo" in rec.AutoConfiguration():
                    self._log.info("Removing FieldAndGeo from AutoConfig, and adding Field")
                    rec.AutoConfiguration = [ c for c in rec.AutoConfiguration() if c != 'FieldAndGeo' and c != 'Geo' ]
                    rec.AutoConfiguration += [ 'Field' ]
                if "Geo" in rec.AutoConfiguration():
                    self._log.info("Removing Geo from AutoConfig")
                    rec.AutoConfiguration = [ c for c in rec.AutoConfiguration() if c != 'FieldAndGeo' and c != 'Geo' ]
                self._log.info("Setting globalflags.DetDescrVersion = %r" % (value,) )
                globalflags.DetDescrVersion = value

    def setCondTag(self,value):
        global globalflags,rec
        self._condTag = value
        # synchronise globalflags.ConditionsTag
        oldConditionsTag = globalflags.ConditionsTag()
        if value != oldConditionsTag:
            if value == 'AUTO':
                self._log.info("Adding ConditionsTag to AutoConfig")
                if "ConditionsTag" not in rec.AutoConfiguration():
                    rec.AutoConfiguration += [ 'ConditionsTag' ]
            else:
                if "ConditionsTag" in rec.AutoConfiguration():
                    self._log.info("Removing ConditionsTag from AutoConfig")
                    rec.AutoConfiguration = [ c for c in rec.AutoConfiguration() if c != 'ConditionsTag' ]
                self._log.info("Setting globalflags.ConditionsTag = %r" % (value,) )
                globalflags.ConditionsTag = value        

    def _do_action(self):
        """Set some other flags"""
        global DataFilesInfoMap
        val = self.get_Value()
        self._log.info("Setting InputData = %r", val)
        self.selectedData = DataFilesInfoMap[val]
        # trigger synchronisation with other flags
        self.setDetDescr(self.selectedData.detDescr)
        self.setCondTag(self.selectedData.condTag)
        self.setDataType(self.selectedData.type)
        self.setFileList(self.selectedData.fileList)
        self.setMaxFiles(self.selectedData.maxFiles)
        self._syncFileList()


fillJobPropertyContainer(muonRecFlags,__name__)



