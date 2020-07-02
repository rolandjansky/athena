from __future__ import print_function
from __future__ import division
from future.utils import iteritems
from future.utils import itervalues


from builtins import object
from future import standard_library
standard_library.install_aliases()

from builtins import int

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfReports
#
#  @brief Transform report classes and helper functions
#  @details Classes whose instance encapsulates transform reports
#   at different levels, such as file, executor, transform
#  @author atlas-comp-transforms-dev@cern.ch
#  @version $Id: trfReports.py 784023 2016-11-14 14:01:07Z mavogel $
#

__version__ = '$Revision: 784023 $'

import pickle as pickle
import json
import os.path
import platform
import pprint
import sys

from xml.etree import ElementTree

import logging
msg = logging.getLogger(__name__)

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.trfUtils import shQuoteStrings, isodate, prettyXML, calcCpuTime, calcWallTime

## @brief Default values for file reporting
defaultFileReport = {'input': 'name', 'temporary': None, 'output': 'full'}

## @brief Base (almost virtual) report from which all real transform reports derive
class trfReport(object):
    def __init__(self):
        self._dataDictionary = {}
        pass

    ## @brief String representation of the job report
    #  @details Uses pprint module to output the python object as text
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

    def writeJSONReport(self, filename, sort_keys = True, indent = 2, fast = False, fileReport = defaultFileReport):
        with open(filename, 'w') as report:
            try:
                if not self._dataDictionary:
                    self._dataDictionary = self.python(fast=fast, fileReport=fileReport)

                json.dump(self._dataDictionary, report, sort_keys = sort_keys, indent = indent)
            except TypeError as e:
                # TypeError means we had an unserialisable object - re-raise as a trf internal
                message = 'TypeError raised during JSON report output: {0!s}'.format(e)
                msg.error(message)
                raise trfExceptions.TransformReportException(trfExit.nameToCode('TRF_INTERNAL_REPORT_ERROR'), message)

    def writeTxtReport(self, filename, dumpEnv = True, fast = False, fileReport = defaultFileReport):
        with open(filename, 'w') as report:
            if not self._dataDictionary:
                self._dataDictionary = self.python(fast = fast, fileReport = fileReport)

            print('# {0} file generated on'.format(self.__class__.__name__), isodate(), file=report)
            print(pprint.pformat(self._dataDictionary), file=report)
            if dumpEnv:
                print('# Environment dump', file=report)
                eKeys = list(os.environ)
                eKeys.sort()
                for k in eKeys:
                    print('%s=%s' % (k, os.environ[k]), file=report)
            print('# Machine report', file=report)
            print(pprint.pformat(machineReport().python(fast = fast)), file=report)

    def writeGPickleReport(self, filename, fast = False):
        with open(filename, 'wb') as report:
            pickle.dump(self.classicPython(fast = fast), report)

    def writeClassicXMLReport(self, filename, fast = False):
        with open(filename, 'w') as report:
            print(prettyXML(self.classicEltree(fast = fast), poolFileCatalogFormat = True), file=report)

    def writePilotPickleReport(self, filename, fast = False, fileReport = defaultFileReport):
        with open(filename, 'w') as report:
            if not self._dataDictionary:
                self._dataDictionary = self.python(fast = fast, fileReport = fileReport)

            pickle.dump(self._dataDictionary, report)


## @brief Class holding a transform job report
class trfJobReport(trfReport):
    ## @brief This is the version counter for transform job reports
    #  any changes to the format @b must be reflected by incrementing this
    _reportVersion = '2.0.9'
    _metadataKeyMap = {'AMIConfig': 'AMI', }
    _maxMsgLen = 256
    _truncationMsg = " (truncated)"

    ## @brief Constructor
    #  @param parentTrf Mandatory link to the transform this job report represents
    def __init__(self, parentTrf):
        super(trfJobReport, self).__init__()
        self._trf = parentTrf
        self._precisionDigits = 3
        self._dbDataTotal = 0
        self._dbTimeTotal = 0.0

    ## @brief generate the python transform job report
    #  @param type The general type of this report (e.g. fast)
    #  @param fileReport Dictionary giving the type of report to make for each type of file.
    #  This dictionary has to have all io types as keys and valid values are:
    #  @c None - skip this io type; @c 'full' - Provide all details; @c 'name' - only dataset and
    #  filename will be reported on.
    def python(self, fast = False, fileReport = defaultFileReport):
        myDict = {'name': self._trf.name,
                  'reportVersion': self._reportVersion,
                  'cmdLine': ' '.join(shQuoteStrings(sys.argv)),
                  'exitAcronym': trfExit.codeToName(self._trf.exitCode),
                  'exitCode': self._trf.exitCode,
                  'created': isodate(),
                  'resource': {'executor': {}, 'transform': {}},
                  'files': {}
                  }
        if len(self._trf.exitMsg) > self._maxMsgLen:
            myDict['exitMsg'] = self._trf.exitMsg[:self._maxMsgLen-len(self._truncationMsg)] + self._truncationMsg
            myDict['exitMsgExtra'] = self._trf.exitMsg[self._maxMsgLen-len(self._truncationMsg):]
        else:
            myDict['exitMsg'] = self._trf.exitMsg
            myDict['exitMsgExtra'] = ""

        # Iterate over files
        for fileType in ('input', 'output', 'temporary'):
            if fileReport[fileType]:
                myDict['files'][fileType] = []
        # Should have a dataDictionary, unless something went wrong very early...
        for dataType, dataArg in iteritems(self._trf._dataDictionary):
            if dataArg.auxiliaryFile: # Always skip auxilliary files from the report
                continue
            if fileReport[dataArg.io]:
                entry = {"type": dataType}
                entry.update(trfFileReport(dataArg).python(fast = fast, type = fileReport[dataArg.io]))
                # Supress RAW if all subfiles had nentries == 0
                if 'subFiles' in entry and len(entry['subFiles']) == 0 and isinstance(dataArg, trfArgClasses.argBSFile) :
                    msg.info('No subFiles for entry {0}, suppressing from report.'.format(entry['argName']))
                else:
                    myDict['files'][dataArg.io].append(entry)

        # We report on all executors, in execution order
        myDict['executor'] = []
        if hasattr(self._trf, '_executorPath'):
            for executionStep in self._trf._executorPath:
                exe = self._trf._executorDictionary[executionStep['name']]
                myDict['executor'].append(trfExecutorReport(exe).python(fast = fast))
                # Executor resources are gathered here to unify where this information is held
                # and allow T0/PanDA to just store this JSON fragment on its own
                myDict['resource']['executor'][exe.name] = exeResourceReport(exe, self)
                for mergeStep in exe.myMerger:
                    myDict['resource']['executor'][mergeStep.name] = exeResourceReport(mergeStep, self)
            if self._dbDataTotal > 0 or self._dbTimeTotal > 0:
                myDict['resource']['dbDataTotal'] = self._dbDataTotal
                myDict['resource']['dbTimeTotal'] = self.roundoff(self._dbTimeTotal)
        # Resource consumption
        reportTime = os.times()

        # Calculate total cpu time we used -
        myCpuTime = reportTime[0] + reportTime[1]
        childCpuTime = reportTime[2] + reportTime[3]
        wallTime = reportTime[4] - self._trf.transformStart[4]
        cpuTime = myCpuTime
        cpuTimeTotal = 0
        cpuTimePerWorker = myCpuTime
        maxWorkers = 1
        msg.debug('Raw cpu resource consumption: transform {0}, children {1}'.format(myCpuTime, childCpuTime))
        # Reduce childCpuTime by times reported in the executors (broken for MP...?)
        for exeName, exeReport in iteritems(myDict['resource']['executor']):
            if 'mpworkers' in exeReport:
                if exeReport['mpworkers'] > maxWorkers : maxWorkers = exeReport['mpworkers']
            try:
                msg.debug('Subtracting {0}s time for executor {1}'.format(exeReport['cpuTime'], exeName))
                childCpuTime -= exeReport['cpuTime']
            except TypeError:
                pass
            try:
                cpuTime += exeReport['cpuTime']
                cpuTimeTotal += exeReport['total']['cpuTime']
                if 'cpuTimePerWorker' in exeReport:
                    msg.debug('Adding {0}s to cpuTimePerWorker'.format(exeReport['cpuTimePerWorker']))
                    cpuTimePerWorker += exeReport['cpuTimePerWorker']
                else:
                    msg.debug('Adding nonMP cpuTime {0}s to cpuTimePerWorker'.format(exeReport['cpuTime']))
                    cpuTimePerWorker += exeReport['cpuTime']
            except TypeError:
                pass

        msg.debug('maxWorkers: {0}, cpuTimeTotal: {1}, cpuTimePerWorker: {2}'.format(maxWorkers, cpuTime, cpuTimePerWorker))
        reportGenerationCpuTime = reportGenerationWallTime = None
        if self._trf.outFileValidationStop and reportTime:
            reportGenerationCpuTime = calcCpuTime(self._trf.outFileValidationStop, reportTime)
            reportGenerationWallTime = calcWallTime(self._trf.outFileValidationStop, reportTime)

        myDict['resource']['transform'] = {'cpuTime': self.roundoff(myCpuTime),
                                           'cpuTimeTotal': self.roundoff(cpuTimeTotal),
                                           'externalCpuTime': self.roundoff(childCpuTime),
                                           'wallTime': self.roundoff(wallTime),
                                           'transformSetup': {'cpuTime': self.roundoff(self._trf.transformSetupCpuTime),
                                                              'wallTime': self.roundoff(self._trf.transformSetupWallTime)},
                                           'inFileValidation': {'cpuTime': self.roundoff(self._trf.inFileValidationCpuTime),
                                                                'wallTime': self.roundoff(self._trf.inFileValidationWallTime)},
                                           'outFileValidation': {'cpuTime': self.roundoff(self._trf.outFileValidationCpuTime),
                                                                 'wallTime': self.roundoff(self._trf.outFileValidationWallTime)},
                                           'reportGeneration': {'cpuTime': self.roundoff(reportGenerationCpuTime),
                                                                'wallTime': self.roundoff(reportGenerationWallTime)}, }
        if self._trf.processedEvents:
            myDict['resource']['transform']['processedEvents'] = self._trf.processedEvents
        myDict['resource']['transform']['trfPredata'] = self._trf.trfPredata
        # check for devision by zero for fast jobs, unit tests
        if wallTime > 0:
            myDict['resource']['transform']['cpuEfficiency'] = round(cpuTime/maxWorkers/wallTime, 4)
            myDict['resource']['transform']['cpuPWEfficiency'] = round(cpuTimePerWorker/wallTime, 4)
        myDict['resource']['machine'] = machineReport().python(fast = fast)

        return myDict

    ## @brief Classic metadata.xml report
    def classicEltree(self, fast = False):
        trfTree = ElementTree.Element('POOLFILECATALOG')
        # Extract some executor parameters here
        for exeKey in ('preExec', 'postExec', 'preInclude', 'postInclude'):
            if exeKey in self._trf.argdict:
                for substep, pyfrag in iteritems(self._trf.argdict[exeKey].value):
                    if substep == 'all':
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
        for dataArg in itervalues(self._trf._dataDictionary):
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
                if hasattr(executor, '_logScan') and self._trf.exitCode == 0:
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
                    for substep, values in iteritems(self._trf.argdict[substepKey].value):
                        if substep == 'all':
                            trfDict['jobOutputs'][-1]['more']['metadata'][substepKey] = values
                        else:
                            trfDict['jobOutputs'][-1]['more']['metadata'][substepKey + '_' + substep] = values

        # Now retrieve the input event count
        nentries = 'UNKNOWN'
        for fileArg in self._trf.getFiles(io = 'input'):
            thisArgNentries = fileArg.nentries
            if isinstance(thisArgNentries, int):
                if nentries == 'UNKNOWN':
                    nentries = thisArgNentries
                elif thisArgNentries != nentries:
                    msg.warning('Found a file with different event count than others: {0} != {1} for {2}'.format(thisArgNentries, nentries, fileArg))
                    # Take highest number?
                    if thisArgNentries > nentries:
                        nentries = thisArgNentries
        trfDict['nevents'] = nentries

        # Tier 0 expects the report to be in a top level dictionary under the prodsys key
        return {'prodsys' : trfDict}

    # Helper method to format values to number of decimals configured for this jobReport.
    # Safely allows possible and valid None values within jobReport.
    def roundoff(self, value):
        return round(value, self._precisionDigits) if (value is not None) else value


## @brief Class to contain metadata for an executor
class trfExecutorReport(object):
    def __init__(self, executor):
        self._exe = executor

    ## @brief Get a python representation of executor report
    #  @details Returns the python representation of this executor
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
        for k, v in iteritems(self._exe.extraMetadata):
            reportDict['exeConfig'][k] = v

        # Do we have a logscan to add?
        if hasattr(self._exe, '_logScan'):
            try:
                json.dumps(self._exe._logScan.python)
                reportDict['logfileReport'] = self._exe._logScan.python
            except UnicodeDecodeError as e:
                msg.error('Problem with serialising logfile report as JSON - this will be skipped from the report ({0})'.format(e))
            reportDict['metaData'] = self._exe._logScan._metaData

        # Asetup information
        if hasattr(self._exe, '_asetup'):
            reportDict['asetup'] = self._exe._asetup

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
        if type == 'name':
            fileArgProps = {'dataset': self._fileArg.dataset,
                            'nentries': self._fileArg.getnentries(fast),
                            'subFiles' : []}
        elif type == 'full':
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
        #  in a different directory
        uniqueBasenames = set([ os.path.basename(fname) for fname in self._fileArg.value ])
        uniqueDirectories = set([ os.path.dirname(os.path.relpath(os.path.normpath(fname))) for fname in self._fileArg.value ])
        if len(uniqueBasenames) != len(self._fileArg.value):
            msg.info('Detected two files with the same basename in a file argument - report for file {0} will be produced with the path as a key'.format(self._fileArg))
            basenameReport = False
        elif len(uniqueDirectories) > 1:
            msg.warning('Detected output files in different directories - report for file {0} will be produced with the path as a key'.format(self._fileArg))
            basenameReport = False
        else:
            basenameReport = True
        suppressed = []
        for fname in self._fileArg.value:
            subFile = None
            if basenameReport:
                subFile = self.singleFilePython(fname, fast = fast, type = type)
            else:
                subFile = self.singleFilePython(fname, fast = fast, type = type, basename = False)
            if subFile is not None:
                # if nentries == 0 for DRAW, suppress subfile from report
                if 'nentries' in subFile and subFile['nentries'] == 0 and isinstance(self._fileArg, trfArgClasses.argBSFile):
                    msg.info('Suppressing file {0}, nentries is 0'.format(subFile['name']))
                    suppressed.append(subFile['name'])
                else:
                    fileArgProps['subFiles'].append(subFile)

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
        if type == 'name':
            # For 'name' we return only the GUID
            entry.update(self._fileArg.getMetadata(files = filename, populate = not fast, metadataKeys = ['file_guid'])[filename])
        elif type == 'full':
            # Suppress io because it's the key at a higher level and _exists because it's internal
            entry.update(self._fileArg.getMetadata(files = filename, populate = not fast, maskMetadataKeys = ['io', '_exists', 'integrity', 'file_type'])[filename])
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
        for fname in self._fileArg.value:
            treeList.append(self.classicSingleEltree(fname, fast = fast))

        return treeList


    ## @brief Return file metadata in 'classic' POOLFILECATALOG format
    #  @param filename Filename to generate Element object for
    #  @param fast Prevent external metadata calls if true
    def classicSingleEltree(self, filename, fast = False):
        if filename not in self._fileArg.value:
            raise trfExceptions.TransformReportException(trfExit.nameToCode('TRF_INTERNAL_REPORT_ERROR'),
                                                         'Unknown file ({0}) in the file report for {1}'.format(filename, self._fileArg))
        tree = ElementTree.Element('File', ID = str(self._fileArg.getSingleMetadata(fname = filename, metadataKey = 'file_guid', populate = not fast)))
        for myKey, classicKey in iteritems(self._internalToClassicMap):
            # beam_type is tricky - we return only the first list value,
            # (but remember, protect against funny stuff!)
            if myKey == 'beam_type':
                beamType = self._fileArg.getSingleMetadata(fname = filename, metadataKey = myKey, populate = not fast)
                if isinstance(beamType, list):
                    if len(beamType) == 0:
                        ElementTree.SubElement(tree, 'metadata', att_name = classicKey, att_value = '')
                    else:
                        ElementTree.SubElement(tree, 'metadata', att_name = classicKey, att_value = str(beamType[0]))
                else:
                    # This is really not normal, but best we can do is str conversion
                    ElementTree.SubElement(tree, 'metadata', att_name = classicKey, att_value = str(beamType))
            else:
                ElementTree.SubElement(tree, 'metadata', att_name = classicKey,
                                       att_value = str(self._fileArg.getSingleMetadata(fname = filename, metadataKey = myKey, populate = not fast)))
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
        for fname in self._fileArg.value:
            fileList.append(self.classicSinglePython(fname, fast = fast))
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
        for myKey, classicKey in iteritems(self._internalToGpickleMap):
            fileDict[classicKey] = self._fileArg.getSingleMetadata(fname = filename, metadataKey = myKey, populate = not fast)
            if classicKey == 'checkSum' and fileDict[classicKey] == 'UNDEFINED':
                # Old style is that we give back None when we don't know
                fileDict[classicKey] = None
            elif fileDict[classicKey] == 'UNDEFINED':
                # Suppress things we don't generally expect to know
                del fileDict[classicKey]
        # Base 'more' stuff which is known by the argFile itself
        fileDict['more'] = {'metadata' : {'fileType' : self._fileArg.type}}
        for myKey, classicKey in iteritems(self._internalToGpickleMoreMap):
            value = self._fileArg.getSingleMetadata(fname = filename, metadataKey = myKey, populate = not fast)
            if value != 'UNDEFINED':
                fileDict['more']['metadata'][classicKey] = value

        return fileDict


## @brief Report on the machine where we ran
class machineReport(object):

    def python(self, fast = False):
        machine = {}
        # Get the following from the platform module
        attrs = ['node', 'platform', 'linux_distribution']
        for attr in attrs:
            try:
                machine[attr] = getattr(platform, attr).__call__()
            except AttributeError as e:
                msg.error('Failed to get "{0}" attribute from platform module: {1}'.format(attr, e))

        # Now try to get processor information from /proc/cpuinfo
        try:
            with open('/proc/cpuinfo') as cpuinfo:
                for line in cpuinfo:
                    try:
                        k, v = [ l.strip() for l in line.split(':') ]
                        if k == 'cpu family' and 'cpu_family' not in machine:
                            machine['cpu_family'] = v
                        elif k == 'model' and 'model' not in machine:
                            machine['model'] = v
                        elif k == 'model name' and 'model_name' not in machine:
                            machine['model_name'] = v
                    except ValueError:
                        pass
        except Exception as e:
            msg.warning('Unexpected error while parsing /proc/cpuinfo: {0}'.format(e))
        try:
            with open('/etc/machinefeatures/hs06') as hs:
                machine['hepspec'] = hs.readlines()[0].strip()
        except IOError:
            pass
        return machine


## @brief Small helper function to extract a per-datatype dictionary from a job report of lists
def pyJobReportToFileDict(jobReport, io = 'all'):
    dataDict = {}
    if 'files' not in jobReport:
        msg.warning('Job report has no "files" section')
        return dataDict
    for iotype in jobReport['files']:
        if io == 'all' or io == iotype:
            for filedata in jobReport['files'][iotype]:
                dataDict[filedata['type']] = filedata
    return dataDict


def exeResourceReport(exe, report):
    exeResource = {'cpuTime': report.roundoff(exe.cpuTime),
                   'wallTime': report.roundoff(exe.wallTime),
                   'preExe': {
                       'cpuTime': report.roundoff(exe.preExeCpuTime),
                       'wallTime': report.roundoff(exe.preExeWallTime),
                       },
                   'postExe': {
                       'cpuTime': report.roundoff(exe.postExeCpuTime),
                       'wallTime': report.roundoff(exe.postExeWallTime),
                       },
                   'validation': {
                       'cpuTime': report.roundoff(exe.validationCpuTime),
                       'wallTime': report.roundoff(exe.validationWallTime),
                       },
                   'total': {
                       'cpuTime': report.roundoff(exe.cpuTimeTotal),
                       'wallTime': report.roundoff(exe.wallTimeTotal),
                       },
                   }

    if exe.memStats:
        exeResource['memory'] = exe.memStats
    if exe.eventCount:
        exeResource['nevents'] = exe.eventCount
    if exe.athenaMP:
        exeResource['mpworkers'] = exe.athenaMP
        exeResource['cpuTimePerWorker'] = report.roundoff(exe.cpuTime/exe.athenaMP)
    if exe.dbMonitor:
        exeResource['dbData'] = exe.dbMonitor['bytes']
        exeResource['dbTime'] = report.roundoff(exe.dbMonitor['time'])
        report._dbDataTotal += exeResource['dbData']
        report._dbTimeTotal += exeResource['dbTime']
    return exeResource
