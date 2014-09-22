# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfReports
#
#  @brief Transform report classes and helper functions
#  @details Classes whose instance encapsulates transform reports
#   at different levels, such as file, executor, transform
#  @author atlas-comp-transforms-dev@cern.ch
#  @version $Id: trfReports.py 617963 2014-09-22 13:13:07Z graemes $
#

__version__ = '$Revision: 617963 $'
__doc__ = 'Transform report module'

import cPickle as pickle
import json
import os.path
import platform
import pprint
import sys

from copy import deepcopy
from xml.etree import ElementTree

import logging
msg = logging.getLogger(__name__)

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.trfUtils import shQuoteStrings, isodate, prettyXML

## @brief Default values for file reporting
defaultFileReport = {'input': 'name', 'temporary': None, 'output': 'full'}

## @brief Base (almost virtual) report from which all real transform reports derive
class trfReport(object):
    def __init__(self):
        pass

    ## @brief String representation of the job report
    #  @detail Uses pprint module to output the python object as text
    #  @note This is a 'property', so no @c fast option is available
    def __str__(self):
        return pprint.pformat(self.python())

    ## @brief Method which returns a python representation of a report
    #  @param fast Boolean which forces the fastest possible report to be written
    def python(self, fast = False, fileReport = defaultFileReport):
        return {}

    ## @brief Method which returns a JSON representation of a report
    #  @param fast Boolean which forces the fastest possible report to be written
    #  @details Calls @c json.dumps on the python representation
    def json(self, fast = False):
        return json.dumps(self.python, type)

    ## @brief Method which returns an ElementTree.Element representation of the old POOLFILECATALOG report
    #  @param fast Boolean which forces the fastest possible report to be written
    def classicEltree(self, fast = False):
        return ElementTree.Element('POOLFILECATALOG')

    ## @brief Method which returns a python representation of a report in classic Tier 0 style
    #  @param fast Boolean which forces the fastest possible report to be written
    def classicPython(self, fast = False):
        return {}

    def writeJSONReport(self, filename, sort_keys = True, indent = 2, fast = False,
                        fileReport = defaultFileReport):
        with open(filename, 'w') as report:
            try:
                json.dump(self.python(fast = fast, fileReport = fileReport), report, sort_keys = sort_keys, indent = indent)
            except TypeError as e:
                # TypeError means we had an unserialisable object - re-raise as a trf internal
                message = 'TypeError raised during JSON report output: {0!s}'.format(e)
                msg.error(message)
                raise trfExceptions.TransformReportException(trfExit.nameToCode('TRF_INTERNAL_REPORT_ERROR'), message)

    def writeTxtReport(self, filename, dumpEnv = True, fast = False, fileReport = defaultFileReport):
        with open(filename, 'w') as report:
            print >> report, '# {0} file generated on'.format(self.__class__.__name__), isodate()
            print >> report, pprint.pformat(self.python(fast = fast, fileReport = fileReport))
            if dumpEnv:
                print >> report, '# Environment dump'
                eKeys = os.environ.keys()
                eKeys.sort()
                for k in eKeys:
                    print >> report, '%s=%s' % (k, os.environ[k])
            print >> report, '# Machine report'
            print >> report, pprint.pformat(machineReport().python(fast = fast))

    def writeGPickleReport(self, filename, fast = False):
        with open(filename, 'w') as report:
            pickle.dump(self.classicPython(fast = fast), report)

    def writeClassicXMLReport(self, filename, fast = False):
        with open(filename, 'w') as report:
            print >> report, prettyXML(self.classicEltree(fast = fast), poolFileCatalogFormat = True)

    def writePilotPickleReport(self, filename, fast = False, fileReport = defaultFileReport):
        with open(filename, 'w') as report:
            pickle.dump(self.python(fast = fast, fileReport = fileReport), report)


## @brief Class holding a transform job report
class trfJobReport(trfReport):
    ## @brief This is the version counter for transform job reports
    #  any changes to the format @b must be reflected by incrementing this
    _reportVersion = '0.9.6'
    _metadataKeyMap = {'AMIConfig': 'AMI', }
    _maxMsgLen = 256
    _truncationMsg = " (truncated)"

    ## @brief Constructor
    #  @param parentTrf Mandatory link to the transform this job report represents
    def __init__(self, parentTrf):
        self._trf = parentTrf


    ## @brief compress the given filereport dict
    #  @param fileDict the dict to compress
    def _compressFileDict(self, fileDict):
        resultDict = {}

        for fileType, fileList in fileDict.iteritems():
            # check if compression makes sense
            if len(fileList) > 1:
                # which keys need to be ignored definitely??
                keyIgnoreList = ['nentries', 'size']
                compressionHappend = False
                compressedDict = {'common' : {},
                                  'files' : fileList}

                # iterate over all keys in first file, if they are not in here, why look int he others??
                file0 = fileList[0]
                for k, v in file0.iteritems():
                    if k in keyIgnoreList:
                        continue

                    # there are already compressions for this subfile
                    if k == 'subFiles':
                        #special handling, now search in all subfile lists
                        keyCompressible = True
                        for checkFile in fileList[1:]:
                            if 'subFiles' not in checkFile.keys():
                                keyCompressible = False
                                break
                        if not keyCompressible:
                            # not al have subFiles, leave all subFiles alone
                            continue

                        subFilesDict0 = file0['subFiles']
                        for entry in subFilesDict0:
                            for k, v in entry.iteritems():
                                subFileKeyCompressible = True
                                localCommonKeyToDelete = []
                                for checkFile in fileList[1:]:
                                    if 'common' in checkFile.keys():
                                        # we do not need to look up in subFiles
                                        if k not in checkFile['common'].keys():
                                            #it is not in common, so it CAN NOT be in all the subfiles, we can break
                                            subFileKeyCompressible = False
                                            break
                                    else:
                                        for subFileEntry in checkFile['subFiles']:
                                            if k not in subFileEntry.keys():
                                                subFileKeyCompressible = False
                                                break
                                            if str(v) != str(subFileEntry[k]):
                                                subFileKeyCompressible = False
                                                break

                                if subFileKeyCompressible:
                                    compressionHappend = True
                                    compressedDict['common'][k] = subFilesDict0[0][k]
                        # end of subfiles
                        continue
                    if k == 'common':
                        #special handling, now search in all subfile lists
                        commonKeyCompressible = True
                        localCommonKeyToDelete = []

                        commonDict0 = file0['common']
                        for k, v in commonDict0.iteritems():
                            for checkFile in fileList[1:]:
                                if 'common' in checkFile.keys():
                                    # we do not need to look up in subFiles
                                    if k not in checkFile['common'].keys():
                                        #it is not in common, so it CAN NOT be in all the subfiles, we can break
                                        commonKeyCompressible = False
                                        break
                                    else:
                                        localCommonKeyToDelete.append(checkFile)
                                else:
                                    for subFileEntry in checkFile['subFiles']:
                                        if k not in subFileEntry.keys():
                                            commonKeyCompressible = False
                                            break
                                        if str(v) != str(subFileEntry[k]):
                                            commonKeyCompressible = False
                                            break

                                if k in checkFile.keys():
                                    if str(v) != str(subFileEntry[k]):
                                        commonKeyCompressible = False
                                        break

                            if commonKeyCompressible:
                                compressionHappend = True
                                compressedDict['common'][k] = commonDict0[k]
                        # end of common
                        continue

                    # this is the normal case for all keys not in common or subfiles
                    keyCompressible = True
                    inCommon = True
                    inSubFiles = True
                    for checkFile in fileList[1:]:
                        if 'common' in checkFile.keys():
                            if k in checkFile['common'].keys():

                                if str(file0[k]) != str(checkFile['common'][k]):
                                    inCommon = False
                            else:
                                inCommon = False
                        else:
                            inCommon = False

                        if 'subFiles' in checkFile.keys():
                            # has to be in all subfiles
                            for subFile in checkFile['subFiles']:
                                if k not in subFile.keys():
                                    inSubFiles = False
                                    break
                                else:
                                    if str(file0[k]) != str(subFile[k]):
                                        inSubFiles = False
                                        break

                        if k in checkFile.keys():
                            if file0[k] != checkFile[k]:
                                keyCompressible = False
                        else:
                            keyCompressible = False

                        if keyCompressible or inCommon or inSubFiles:
                            compressionHappend = True
                            compressedDict['common'][k] = v

                # restore old layout
                if not compressionHappend:
                    resultDict[fileType] = fileList
                else:
                    # clean all keys that are compressible
                    resultDict[fileType] = compressedDict
                    for key in compressedDict['common'].keys():
                        for fileEntry in fileList:
                            if key in fileEntry.keys():
                                del fileEntry[key]
                                continue
                            if 'common' in fileEntry.keys():
                                if key in fileEntry['common'].keys():
                                    del fileEntry['common'][key]
                                    continue
                            for subFileEntry in fileEntry['subFiles']:
                                if key in subFileEntry.keys():
                                    del subFileEntry[key]
            else:
                resultDict[fileType] = fileList

        return resultDict

    ## @brief generate the python transform job report
    #  @param type Then general type of this report (e.g. fast)
    #  @param fileReport Dictionary giving the type of report to make for each type of file.
    #  This dictionary has to have all io types as keys and valid values are:
    #  @c None - skip this io type; @c 'full' - Provide all details; @c 'name' - only dataset and
    #  filename will be reported on.
    #  @param machineReport Boolean as to whether to include machine information
    def python(self, fast = False, fileReport = defaultFileReport, machineReport = False):
        myDict = {'name': self._trf.name,
                  'reportVersion': self._reportVersion,
                  'cmdLine': ' '.join(shQuoteStrings(sys.argv)),
                  'exitAcronym': trfExit.codeToName(self._trf.exitCode),
                  'exitCode': self._trf.exitCode,
                  'created': isodate(),
                  }
        if len(self._trf.exitMsg) > self._maxMsgLen:
            myDict['exitMsg'] = self._trf.exitMsg[:self._maxMsgLen-len(self._truncationMsg)] + self._truncationMsg
            myDict['exitMsgExtra'] = self._trf.exitMsg[self._maxMsgLen-len(self._truncationMsg):]
        else:
            myDict['exitMsg'] = self._trf.exitMsg
            myDict['exitMsgExtra'] = ""
            
        # Iterate over argValues...
        myDict['argValues'] = {}
        for k, v in self._trf.argdict.iteritems():
            if isinstance(v, trfArgClasses.argument):
                myDict['argValues'][k] = v.value
            else:
                myDict['argValues'][k] = v

        # Iterate over files
        myDict['files'] = {}
        for fileType in ('input', 'output', 'temporary'):
            if fileReport[fileType]:
                myDict['files'][fileType] = []
        # Should have a dataDictionary, unless something went wrong very early...
        for dataType, dataArg in self._trf._dataDictionary.iteritems():
            if fileReport[dataArg.io]:
                entry = {"type": dataType}
                entry.update(trfFileReport(dataArg).python(fast = fast, type = fileReport[dataArg.io]))
                myDict['files'][dataArg.io].append(entry)

        try:
            saveFilesDict = deepcopy(myDict['files'])
            myDict['files'] = self._compressFileDict(myDict['files'])
        except Exception, e:
            msg.warning('Exception raised during file dictionary compression: {0}'.format(e))
            myDict['files'] = saveFilesDict

        # We report on all executors, in execution order
        myDict['executor'] = []
        if hasattr(self._trf, '_executorPath'):
            for executionStep in self._trf._executorPath:
                myDict['executor'].append(trfExecutorReport(self._trf._executorDictionary[executionStep['name']]).python(fast = fast))

        # By default we do not include the machine report - delegated to pilot/Tier 0
        if machineReport:
            myDict['machine'] = machineReport().python(fast = fast)

        # Resource consumption
        reportTime = os.times()

        # Calculate total cpu time we used -
        myCpuTime = reportTime[0] + reportTime[1]
        childCpuTime = reportTime[2] + reportTime[3]
        wallTime = reportTime[4] - self._trf.transformStart[4]
        msg.debug('Raw cpu resource consumption: transform {0}, children {1}'.format(myCpuTime, childCpuTime))
        # Reduce childCpuTime by times reported in the executors
        for exeReport in myDict['executor']:
            if isinstance(exeReport['resource']['cpuTime'], (float, int, long)):
                msg.debug('Subtracting {0}s time for executor {1}'.format(exeReport['resource']['cpuTime'], exeReport['name']))
                childCpuTime -= exeReport['resource']['cpuTime']

        myDict['resource'] = {'transformCpuTime': int(myCpuTime + 0.5),
                              'externalsCpuTime': int(childCpuTime + 0.5),
                              'wallTime': int(wallTime + 0.5),
                              'cpuUnit': 'seconds',
                              'memUnit': 'kB'}  # Not clear if this is 10^3 or 2^10

        return myDict

    ## @brief Classic metadata.xml report
    def classicEltree(self, fast = False):
        trfTree = ElementTree.Element('POOLFILECATALOG')
        # Extract some executor parameters here
        for exeKey in ('preExec', 'postExec', 'preInclude', 'postInclude'):
            if exeKey in self._trf.argdict:
                for substep, pyfrag in self._trf.argdict[exeKey].value.iteritems():
                    if substep is 'all':
                        ElementTree.SubElement(trfTree, 'META', type = 'string', name = exeKey, value = str(pyfrag))
                    else:
                        ElementTree.SubElement(trfTree, 'META', type = 'string', name = exeKey + '_' + substep, value = str(pyfrag))
        for exeKey in ('autoConfiguration', 'AMIConfig', 'AMITag'):
            if exeKey in self._trf.argdict:
                if exeKey in self._metadataKeyMap:
                    classicName = self._metadataKeyMap[exeKey]
                else:
                    classicName = exeKey
                ElementTree.SubElement(trfTree, 'META', type = 'string', name = classicName,
                                       value = str(self._trf.argdict[exeKey].value))

        # Now add information about output files
        for dataType, dataArg in self._trf._dataDictionary.iteritems():
            if dataArg.io == 'output':
                for fileEltree in trfFileReport(dataArg).classicEltreeList(fast = fast):
                    trfTree.append(fileEltree)

        return trfTree

    ## @brief Classic Tier 0 metadata python object
    #  @details Metadata in python nested dictionary form, which will produce a Tier 0
    #  .gpickle when pickled
    def classicPython(self, fast = False):
        # Things we can get directly from the transform
        trfDict = {'jobInputs' : [],  # Always empty?
                   'jobOutputs' : [],  # Filled in below...
                   'more' : {'Machine' : 'unknown'},
                   'trfAcronym' : trfExit.codeToName(self._trf.exitCode),
                   'trfCode' :  self._trf.exitCode,
                   'trfExitCode' :  self._trf.exitCode,
                   }

        if self._trf.lastExecuted is not None:
            trfDict.update({'athAcronym' : self._trf.lastExecuted.errMsg,
                            'athCode' : self._trf.lastExecuted.rc})


        # Emulate the NEEDCHECK behaviour
        if hasattr(self._trf, '_executorPath'):
            for executor in self._trf._executorPath:
                if hasattr(executor, '_logScan') and self._trf.exitCode is 0:
                    if executor._logScan._levelCounter['FATAL'] > 0 or executor._logScan._levelCounter['CRITICAL'] > 0:
                        # This should not happen!
                        msg.warning('Found FATAL/CRITICAL errors and exit code 0 - reseting to TRF_LOGFILE_FAIL')
                        self._trf.exitCode = trfExit.nameToCode('TRF_LOGFILE_FAIL')
                        trfDict['trfAcronym'] = 'TRF_LOGFILE_FAIL'
                    elif executor._logScan._levelCounter['ERROR'] > 0:
                        msg.warning('Found errors in logfile scan - changing exit acronymn to NEEDCHECK.')
                        trfDict['trfAcronym'] = 'NEEDCHECK'

        # Now add files
        fileArgs = self._trf.getFiles(io = 'output')
        for fileArg in fileArgs:
            # N.B. In the original Tier 0 gpickles there was executor
            # information added for each file (such as autoConfiguration, preExec).
            # However, Luc tells me it is ignored, so let's not bother.
            trfDict['jobOutputs'].extend(trfFileReport(fileArg).classicPython(fast = fast))
            # AMITag and friends is added per-file, but it's known only to the transform, so set it here:
            for argdictKey in ('AMITag', 'autoConfiguration',):
                if argdictKey in self._trf.argdict:
                    trfDict['jobOutputs'][-1]['more']['metadata'][argdictKey] = self._trf.argdict[argdictKey].value
            # Mangle substep argumemts back to the old format
            for substepKey in ('preExec', 'postExec', 'preInclude', 'postInclude'):
                if substepKey in self._trf.argdict:
                    for substep, values in self._trf.argdict[substepKey].value.iteritems():
                        if substep is 'all':
                            trfDict['jobOutputs'][-1]['more']['metadata'][substepKey] = values
                        else:
                            trfDict['jobOutputs'][-1]['more']['metadata'][substepKey + '_' + substep] = values

        # Now retrieve the input event count
        nentries = 'UNKNOWN'
        for fileArg in self._trf.getFiles(io = 'input'):
            thisArgNentries = fileArg.nentries
            if isinstance(thisArgNentries, (int, long)):
                if nentries is 'UNKNOWN':
                    nentries = thisArgNentries
                elif thisArgNentries != nentries:
                    msg.warning('Found a file with different event count than others: {0} != {1} for {2}'.format(thisArgNentries, nentries, fileArg))
                    # Take highest number?
                    if thisArgNentries > nentries:
                        nentries = thisArgNentries
        trfDict['nevents'] = nentries

        # Tier 0 expects the report to be in a top level dictionary under the prodsys key
        return {'prodsys' : trfDict}


## @brief Class to contain metadata for an executor
class trfExecutorReport(object):
    def __init__(self, executor):
        self._exe = executor

    ## @brief Get a python representation of executor report
    #  @detail Returns the python representation of this executor
    #  @return Python dictionary
    def python(self, fast = False):
        reportDict = {'name': self._exe.name,
                      'rc' : self._exe.rc,
                      'validation' : self._exe.isValidated,
                      'statusOK' : self._exe.hasExecuted and self._exe.isValidated and (not bool(self._exe.rc)),
                      'errMsg': self._exe.errMsg,
                      'exeConfig' : {}
                      }
        # Add executor config information
        for k, v in self._exe.extraMetadata.iteritems():
            reportDict['exeConfig'][k] = v

        # Do we have a logscan to add?
        if hasattr(self._exe, '_logScan'):
            reportDict['logfileReport'] = self._exe._logScan.python
            reportDict['metaData'] = self._exe._logScan._metaData

        # Asetup information
        if hasattr(self._exe, '_asetup'):
            reportDict['asetup'] = self._exe._asetup

        # Resource consumption
        reportDict['resource'] = {'cpuTime': self._exe.cpuTime, 'wallTime': self._exe.wallTime, }
        if self._exe.memStats:
            reportDict['resource'].update(self._exe.memStats)

        return reportDict


## @brief Class to contain metadata for file types
class trfFileReport(object):
    # Class variable with the mapping from internal (usually = AthFile key) metadata keys to classic keys
    # In the case of 'size' it's the same, but it's convenient to list it here to be able to just loop over dictionary
    _internalToClassicMap = {'conditions_tag' : 'conditionsTag',
                             'beam_type' : 'beamType',
                             'geometry' : 'geometryVersion',
                             'nentries' : 'events',
                             'file_size': 'size',
                             }
    # Similar for Tier 0 file metadata keys
    _internalToGpickleMap = {'file_guid' : 'GUID',
                             'checkSum' : 'checkSum',  # We don't have this, but Tier 0 want it in the dictionary
                             'nentries' : 'events',
                             'file_size' : 'size',
                             }
    # Keep a separate map for the data which Tier 0 gets in the 'more : {}' dictionary
    _internalToGpickleMoreMap = {'beam_type' : 'beamType',
                                 'conditions_tag' : 'conditionsTag',
                                 'geometry' : 'geometryVersion',
                                 }

    ## @brief Constructor
    #  @param @c fileArg argFile object which this report represents
    def __init__(self, fileArg):
        self._fileArg = fileArg

    ## @brief Get a python representation of file report
    #  @detail Returns the python representation of this file report
    #          which is a list of dictionaries (a la Tier 0)
    #  @param type Report type (e.g., None = normal or fast)
    #  @param base How extensive to make the report: name or full
    def python(self, fast = False, type = 'full'):
        # First entity contains shared properties - same for all files in this argFile
        if type is 'name':
            fileArgProps = {'dataset': self._fileArg.dataset,
                            'nentries': self._fileArg.getnentries(fast),
                            'subFiles' : []}
        elif type is 'full':
            fileArgProps = {'dataset' : self._fileArg.dataset,
                            'type' : self._fileArg.type,
                            'subFiles' : [],
                            'argName' : self._fileArg.name,
                            }
        else:
            raise trfExceptions.TransformReportException(trfExit.nameToCode('TRF_INTERNAL_REPORT_ERROR'),
                                                         'Unknown file report type ({0}) in the file report for {1}'.format(type, self._fileArg))

        ## @note We try to strip off the path when there are multiple files to be reported on,
        #  however we should not do this if any of the files share a basename or anything is
        #  in adifferent directory
        uniqueBasenames = set([ os.path.basename(file) for file in self._fileArg.value ])
        uniqueDirectories = set([ os.path.dirname(os.path.relpath(os.path.normpath(file))) for file in self._fileArg.value ])
        if len(uniqueBasenames) != len(self._fileArg.value):
            msg.info('Detected two files with the same basename in a file argument - report for file {0} will be produced with the path as a key'.format(file))
            basenameReport = False
        elif len(uniqueDirectories) > 1:
            msg.warning('Detected output files in different directories - report for file {0} will be produced with the path as a key'.format(file))
            basenameReport = False
        else:
            basenameReport = True
        for file in self._fileArg.value:
            if basenameReport:
                fileArgProps['subFiles'].append(self.singleFilePython(file, fast = fast, type = type))
            else:
                fileArgProps['subFiles'].append(self.singleFilePython(file, fast = fast, type = type, basename = False))

        if type == 'full':
            # move metadata to subFile dict, before it can be compressed
            metaData = self._fileArg._fileMetadata
            for fileName in metaData.keys():
                if basenameReport == False:
                    searchFileName = fileName
                else:
                    searchFileName = os.path.basename(fileName)

                thisFile = None
                for subFile in fileArgProps['subFiles']:
                    if subFile['name'] == searchFileName:
                        thisFile = subFile
                        break

                if thisFile is None:
                    raise trfExceptions.TransformReportException(trfExit.nameToCode('TRF_INTERNAL_REPORT_ERROR'),
                                                                 'file metadata mismatch in subFiles dict')

                # append metadata keys, except all existing, to subfile dict and ignore _exists
                for k, v in metaData[fileName].iteritems():
                    if k not in thisFile.keys() and k != '_exists':
                        thisFile[k] = v

        # Try to compress subFile metadata
        fileArgProps['common'] = {}
        if len(self._fileArg.value) > 1:
            fileList = fileArgProps['subFiles']
            mdKeys = fileList[0].keys()
            for key in mdKeys:
                # Certain keys we never 'compress'
                if key in ('nentries', 'size', '_exists'):
                    continue
                compressible = True

                file0 = fileList[0]
                for file in fileList[1:]:
                    if key in file.keys():
                        if file0[key] != file[key]:
                            compressible = False
                            break
                    else:
                        compressible = False
                        break

                msg.debug('key: >>%s<<  -->> compressible: %r' % (key, compressible))
                if compressible:
                    fileArgProps['common'][key] = file0[key]
                    for file in fileList:
                        del file[key]

        # remove common if its empty
        if len(fileArgProps['common'].keys()) == 0:
            del fileArgProps['common']

        return fileArgProps

    ## @brief Return unique metadata for a single file in an argFile class
    #  @param filename File for which report should be generated
    #  @param type Type of report to produce
    #  @param fast Generate a fast report, i.e., no external matadata calls
    #  @return Python report dictionary
    def singleFilePython(self, filename, fast = False, type = 'full', basename = True):
        if filename not in self._fileArg.value:
            raise trfExceptions.TransformReportException(trfExit.nameToCode('TRF_INTERNAL_REPORT_ERROR'),
                                                         'Unknown file ({0}) in the file report for {1}'.format(filename, self._fileArg))
        if basename:
            entry = {'name': os.path.basename(filename)}
        else:
            entry = {'name': os.path.relpath(os.path.normpath(filename))}
        if type is 'name':
            # For 'name' we return only the GUID
            entry.update(self._fileArg.getMetadata(files = filename, populate = not fast, metadataKeys = ['file_guid'])[filename])
        elif type is 'full':
            # Suppress io because it's the key at a higher level and _exists because it's internal
            entry.update(self._fileArg.getMetadata(files = filename, populate = not fast, maskMetadataKeys = ['io', '_exists'])[filename])
        else:
            raise trfExceptions.TransformReportException(trfExit.nameToCode('TRF_INTERNAL_REPORT_ERROR'),
                                                         'Unknown file report type ({0}) in the file report for {1}'.format(type, self._fileArg))

        return entry


    ## @brief Return an element tree representation of the classic file report
    #  @detail Returns a list of eltree entities giving the classic metadata of a files
    #  for this argument. This is a list so that it can be attached as a
    #  series of nodes into the final PFC description of the job
    #  @return List of ElementTree.Element objects
    def classicEltreeList(self, fast = False):
        treeList = []
        for file in self._fileArg.value:
            treeList.append(self.classicSingleEltree(file, fast = fast))

        return treeList


    ## @brief Return file metadata in 'classic' POOLFILECATALOG format
    #  @param filename Filename to generate Element object for
    #  @param fast Prevent external metadata calls if true
    def classicSingleEltree(self, filename, fast = False):
        if filename not in self._fileArg.value:
            raise trfExceptions.TransformReportException(trfExit.nameToCode('TRF_INTERNAL_REPORT_ERROR'),
                                                         'Unknown file ({0}) in the file report for {1}'.format(filename, self._fileArg))
        tree = ElementTree.Element('File', ID = str(self._fileArg.getSingleMetadata(file = filename, metadataKey = 'file_guid', populate = not fast)))
        logical = ElementTree.SubElement(tree, 'logical')
        lfn = ElementTree.SubElement(logical, 'lfn', name = filename)
        for myKey, classicKey in self._internalToClassicMap.iteritems():
            # beam_type is tricky - we return only the first list value,
            # (but remember, protect against funny stuff!)
            if myKey is 'beam_type':
                beamType = self._fileArg.getSingleMetadata(file = filename, metadataKey = myKey, populate = not fast)
                if isinstance(beamType, list):
                    if len(beamType) is 0:
                        ElementTree.SubElement(tree, 'metadata', att_name = classicKey, att_value = '')
                    else:
                        ElementTree.SubElement(tree, 'metadata', att_name = classicKey, att_value = str(beamType[0]))
                else:
                    # This is really not normal, but best we can do is str conversion
                    ElementTree.SubElement(tree, 'metadata', att_name = classicKey, att_value = str(beamType))
            else:
                ElementTree.SubElement(tree, 'metadata', att_name = classicKey,
                                       att_value = str(self._fileArg.getSingleMetadata(file = filename, metadataKey = myKey, populate = not fast)))
        # Now add the metadata which is stored at the whole argument level
        ElementTree.SubElement(tree, 'metadata', att_name = 'fileType', att_value = str(self._fileArg.type))
        if self._fileArg.dataset is not None:
            ElementTree.SubElement(tree, 'metadata', att_name = 'dataset', att_value = self._fileArg.dataset)

        return tree

    ## @brief Return file metadata in classic Tier 0 python style
    #  @param fast Prevent external metadata calls if true
    #  @return List of file metadata dictionaries
    def classicPython(self, fast = False):
        fileList = []
        for file in self._fileArg.value:
            fileList.append(self.classicSinglePython(file, fast = fast))
        return fileList

    ## @brief Return single file metadata in classic Tier 0 python style
    #  @param fast Prevent external metadata calls if true
    #  @param @c filename Filename to generate Element object for
    def classicSinglePython(self, filename, fast = False):
        if filename not in self._fileArg.value:
            raise trfExceptions.TransformReportException(trfExit.nameToCode('TRF_INTERNAL_REPORT_ERROR'),
                                                         'Unknown file ({0}) in the file report for {1}'.format(filename, self._fileArg))
        # Direct population of some keys
        fileDict = {'lfn' : filename,
                    'dataset' : self._fileArg.dataset,
                    }
        # Fill in the mapped 'primary' keys
        for myKey, classicKey in self._internalToGpickleMap.iteritems():
            fileDict[classicKey] = self._fileArg.getSingleMetadata(file = filename, metadataKey = myKey, populate = not fast)
            if classicKey is 'checkSum' and fileDict[classicKey] is 'UNDEFINED':
                # Old style is that we give back None when we don't know
                fileDict[classicKey] = None
            elif fileDict[classicKey] is 'UNDEFINED':
                # Suppress things we don't generally expect to know
                del fileDict[classicKey]
        # Base 'more' stuff which is known by the argFile itself
        fileDict['more'] = {'metadata' : {'fileType' : self._fileArg.type}}
        for myKey, classicKey in self._internalToGpickleMoreMap.iteritems():
            value = self._fileArg.getSingleMetadata(file = filename, metadataKey = myKey, populate = not fast)
            if value is not 'UNDEFINED':
                fileDict['more']['metadata'][classicKey] = value

        return fileDict


## @brief Report on the machine where we ran
class machineReport(object):

    def python(self, fast = False):
        machine = {}
        # Get the following from the platform module
        attrs = ['architecture', 'machine', 'node', 'platform', 'processor', 'python_version', 'uname', 'linux_distribution']
        for attr in attrs:
            try:
                machine[attr] = getattr(platform, attr).__call__()
            except AttributeError, e:
                msg.error('Failed to get "{0}" attribute from platform module: {1}'.format(attr, e))
        return machine


## @brief Small helper function to extract a per-datatype dictionary from a job report of lists
def pyJobReportToFileDict(jobReport, io = 'all'):
    dataDict = {}
    if 'files' not in jobReport:
        msg.warning('Job report has no "files" section')
        return dataDict
    for iotype in jobReport['files']:
        if io is 'all' or io == iotype:
            for filedata in jobReport['files'][iotype]:
                dataDict[filedata['type']] = filedata
    return dataDict
