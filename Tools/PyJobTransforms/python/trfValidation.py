# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfValidation
#
# @brief Validation control for job transforms
# @details Contains validation classes controlling how the transforms
# will validate jobs they run.
# @author atlas-comp-transforms-dev@cern.ch
# @version $Id: trfValidation.py 782012 2016-11-03 01:45:33Z uworlika $
# @note Old validation dictionary shows usefully different options:
# <tt>self.validationOptions = {'testIfEmpty' : True, 'testIfNoEvents' : False, 'testIfExists' : True,
#                          'testIfCorrupt' : True, 'testCountEvents' : True, 'extraValidation' : False,
#                          'testMatchEvents' : False, 'testEventMinMax' : True , 'stopOnEventCountNone' : True,
#                          'continueOnZeroEventCount' : True}</tt>
import fnmatch
import os
import re

from subprocess import Popen, STDOUT, PIPE

import logging
msg = logging.getLogger(__name__)

from PyUtils import RootUtils

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.trfLogger import stdLogLevels
from PyJobTransforms.trfArgClasses import argFile

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfUtils as trfUtils


# @brief Check a Pool file for corruption, return N events or -1 if access problem, -2 if corruption
def corruptionTestPool(filename, verbose=False):
    if not os.access(filename, os.R_OK):
        msg.info("ERROR can't access file %s" % filename)
        return -1

    ROOT = RootUtils.import_root()
    from ROOT import TFile, TTree
    import cppyy

    try:
        f = TFile.Open(filename)
    except:
        msg.info("Can't open file %s" % filename)
        return -1

    nEvents = None

    keys = f.GetListOfKeys()
    for k in keys:
        try:
            tn = k.GetName()
            t = f.Get(tn)
            if not isinstance(t, TTree): return
        except:
            msg.info("Can't get tree %s from file %s" % (tn, filename))
            f.Close()
            return -1

        if (verbose): msg.info("Working on tree %s" % tn)
        n = t.GetEntriesFast()
        for i in range(n):
            s = t.GetEntry(i)
            if s <= 0:
                msg.info("Tree %s: Found corruption in event %i" % (i, n))
                f.Close()
                return -2
            else:
                if verbose and i > 0 and i % 100 == 0:
                    msg.info("Checking event %s" % i)
        msg.info("Tree %s: %i event(s) ok" % (tn, n))

        # Use CollectionTree determine the number of events
        if tn == 'CollectionTree':
            nEvents = n
        pass  # end of loop over trees

    f.Close()
    msg.info("ROOT file %s looks ok" % filename)
    if n is None:
        msg.info("Failed to determine number of events in file %s. No tree named 'CollectionTree'" % filename)
        return 0
    return nEvents

# @brief Check BS file for corruption
def corruptionTestBS(filename):
    # First try AtlListBSEvents.exe -c %filename:
    cmd = ['AtlListBSEvents.exe', '-c', filename]
    p = Popen(cmd, shell=False, stdout=PIPE, stderr=STDOUT, close_fds=True)
    while p.poll() is None:
        line = p.stdout.readline()
        if line:
            msg.info("AtlListBSEvents.exe Report: %s" % line.strip())
    rc = p.returncode
    return rc


## @brief Class of patterns that can be ignored from athena logfiles
class ignorePatterns(object):

    ## @brief Load error patterns from files
    #  @details Load regular expressions to be used in logfile parsing
    #  @files Files to load up structured error patterns from
    #  @extraSearch Extra regexp strings to @a search against
    def __init__(self, files=['atlas_error_mask.db'], extraSearch = []):
        # Setup structured search patterns
        self._structuredPatterns = []
        self._initalisePatterns(files)

        # Setup extra search patterns
        self._searchPatterns = []
        self._initialiseSerches(extraSearch)

    @property
    def structuredPatterns(self):
        return self._structuredPatterns

    @property
    def searchPatterns(self):
        return self._searchPatterns

    def _initalisePatterns(self, files):
        for patternFile in files:
            if patternFile == "None":
                continue
            fullName = trfUtils.findFile(os.environ['DATAPATH'], patternFile)
            if not fullName:
                msg.warning('Error pattern file {0} could not be found in DATAPATH'.format(patternFile))
                continue
            try:
                with open(fullName) as patternFileHandle:
                    msg.debug('Opened error file {0} from here: {1}'.format(patternFile, fullName))

                    for line in patternFileHandle:
                        line = line.strip()
                        if line.startswith('#') or line == '':
                            continue
                        try:
                            # N.B. At the moment release matching is not supported!
                            (who, level, message) = [ s.strip() for s in line.split(',', 2) ]
                            if who == "":
                                # Blank means match anything, so make it so...
                                who = "."
                            reWho = re.compile(who)
                            reLevel = level # level is not a regexp (for now)
                            reMessage = re.compile(message)
                        except ValueError:
                            msg.warning('Could not parse this line as a valid error pattern: {0}'.format(line))
                            continue
                        except re.error, e:
                            msg.warning('Could not parse valid regexp from {0}: {1}'.format(message, e))
                            continue

                        msg.debug('Successfully parsed: who={0}, level={1}, message={2}'.format(who, level, message))

                        self._structuredPatterns.append({'service': reWho, 'level': level, 'message': reMessage})

            except (IOError, OSError) as (errno, errMsg):
                msg.warning('Failed to open error pattern file {0}: {1} ({2})'.format(fullName, errMsg, errno))


    def _initialiseSerches(self, searchStrings=[]):
        for string in searchStrings:
            try:
                self._searchPatterns.append(re.compile(string))
                msg.debug('Successfully parsed additional logfile search string: {0}'.format(string))
            except re.error, e:
                msg.warning('Could not parse valid regexp from {0}: {1}'.format(string, e))



## @brief A class holding report information from scanning a logfile
#  This is pretty much a virtual class, fill in the specific methods
#  when you know what type of logfile you are dealing with
class logFileReport(object):
    def __init__(self, logfile=None, msgLimit=10, msgDetailLevel=stdLogLevels['ERROR']):

        # We can have one logfile or a set
        if isinstance(logfile, str):
            self._logfile = [logfile, ]
        else:
            self._logfile = logfile

        self._msgLimit = msgLimit
        self._msgDetails = msgDetailLevel
        self._re = None

        if logfile:
            self.scanLogFile(logfile)

    def resetReport(self):
        pass

    def scanLogFile(self):
        pass

    def worstError(self):
        pass

    def firstError(self):
        pass

    def __str__(self):
        return ''


## @class athenaLogFileReport
#  @brief Logfile suitable for scanning logfiles with an athena flavour, i.e.,
#  lines of the form "SERVICE  LOGLEVEL  MESSAGE"
class athenaLogFileReport(logFileReport):
    ## @brief Class constructor
    #  @param logfile Logfile (or list of logfiles) to scan
    #  @param substepName Name of the substep executor, that has requested this log scan
    #  @param msgLimit The number of messages in each category on which a
    def __init__(self, logfile, substepName=None, msgLimit=10, msgDetailLevel=stdLogLevels['ERROR'], ignoreList=None):
        if ignoreList:
            self._ignoreList = ignoreList
        else:
            self._ignoreList = ignorePatterns()

        ## @note This is the regular expression match for athena logfile lines
        # Match first strips off any HH:MM:SS prefix the transform has added, then
        # takes the next group of non-whitespace characters as the service, then
        # then matches from the list of known levels, then finally, ignores any last
        # pieces of whitespace prefix and takes the rest of the line as the message
        self._regExp = re.compile(r'(?P<service>[^\s]+\w)\s+(?P<level>' + '|'.join(stdLogLevels) + r')\s+(?P<message>.*)')

        self._metaPat = re.compile(r"MetaData:\s+(.*?)\s*=\s*(.*)$")
        self._metaData = {}
        self._substepName = substepName
        self._msgLimit = msgLimit

        self.resetReport()

        super(athenaLogFileReport, self).__init__(logfile, msgLimit, msgDetailLevel)

    ## Produce a python dictionary summary of the log file report for inclusion
    #  in the executor report
    @property
    def python(self):
        errorDict = {'countSummary': {}, 'details': {}}
        for level, count in self._levelCounter.iteritems():
            errorDict['countSummary'][level] = count
            if self._levelCounter[level] > 0 and len(self._errorDetails[level]) > 0:
                errorDict['details'][level] = []
                for error in self._errorDetails[level]:
                    errorDict['details'][level].append(error)
        return errorDict

    def resetReport(self):
        self._levelCounter = {}
        for level in stdLogLevels.keys() + ['UNKNOWN', 'IGNORED']:
            self._levelCounter[level] = 0

        self._errorDetails = {}
        for level in self._levelCounter.keys():
            self._errorDetails[level] = []
            # Format:
            # List of dicts {'message': errMsg, 'firstLine': lineNo, 'count': N}
        self._dbbytes = 0
        self._dbtime  = 0.0

    def scanLogFile(self, resetReport=False):
        if resetReport:
            self.resetReport()

        for log in self._logfile:
            msg.debug('Now scanning logfile {0}'.format(log))
            # N.B. Use the generator so that lines can be grabbed by subroutines, e.g., core dump svc reporter
            try:
                myGen = trfUtils.lineByLine(log, substepName=self._substepName)
            except IOError, e:
                msg.error('Failed to open transform logfile {0}: {1:s}'.format(log, e))
                # Return this as a small report
                self._levelCounter['ERROR'] = 1
                self._errorDetails['ERROR'] = {'message': str(e), 'firstLine': 0, 'count': 1}
                return
            for line, lineCounter in myGen:
                m = self._metaPat.search(line)
                if m is not None:
                    key, value = m.groups()
                    self._metaData[key] = value

                m = self._regExp.match(line)
                if m == None:
                    # We didn't manage to get a recognised standard line from the file
                    # But we can check for certain other interesting things, like core dumps
                    if 'Core dump from CoreDumpSvc' in line > -1:
                        msg.warning('Detected CoreDumpSvc report - activating core dump svc grabber')
                        self.coreDumpSvcParser(myGen, line, lineCounter)
                        continue
                    # Add the G4 exceptipon parsers
                    if 'G4Exception-START' in line > -1:
                        msg.warning('Detected G4 exception report - activating G4 exception grabber')
                        self.g4ExceptionParser(myGen, line, lineCounter, 40)
                        continue
                    if '*** G4Exception' in line > -1:
                        msg.warning('Detected G4 9.4 exception report - activating G4 exception grabber')
                        self.g494ExceptionParser(myGen, line, lineCounter)
                        continue
                    # Add the python exception parser
                    if 'Shortened traceback (most recent user call last)' in line:
                        msg.warning('Detected python exception - activating python exception grabber')
                        self.pythonExceptionParser(myGen, line, lineCounter)
                        continue
                    # Add parser for missed bad_alloc
                    if 'terminate called after throwing an instance of \'std::bad_alloc\'' in line:
                        msg.warning('Detected bad_alloc!')
                        self.badAllocExceptionParser(myGen, line, lineCounter)
                        continue
                    # Parser for ROOT reporting a stale file handle (see ATLASG-448)
                    if 'SysError in <TFile::ReadBuffer>: error reading from file' in line:
                        self.rootSysErrorParser(myGen, line, lineCounter)
                        continue

                    if 'SysError in <TFile::WriteBuffer>' in line:
                        self.rootSysErrorParser(myGen, line, lineCounter)
                        continue

                    msg.debug('Non-standard line in %s: %s' % (log, line))
                    self._levelCounter['UNKNOWN'] += 1
                    continue

                # Line was matched successfully
                fields = {}
                for matchKey in ('service', 'level', 'message'):
                    fields[matchKey] = m.group(matchKey)
                msg.debug('Line parsed as: {0}'.format(fields))

                # Check this is not in our ignore list
                ignoreFlag = False
                for ignorePat in self._ignoreList.structuredPatterns:
                    serviceMatch = ignorePat['service'].match(fields['service'])
                    levelMatch = (ignorePat['level'] == "" or ignorePat['level'] == fields['level'])
                    messageMatch = ignorePat['message'].match(fields['message'])
                    if serviceMatch and levelMatch and messageMatch:
                        msg.info('Error message "{0}" was ignored at line {1} (structured match)'.format(line, lineCounter))
                        ignoreFlag = True
                        break
                if ignoreFlag is False:
                    for searchPat in self._ignoreList.searchPatterns:
                        if searchPat.search(line):
                            msg.info('Error message "{0}" was ignored at line {1} (search match)'.format(line, lineCounter))
                            ignoreFlag = True
                            break
                if ignoreFlag:
                    # Got an ignore - message this to a special IGNORED error
                    fields['level'] = 'IGNORED'
                else:
                    # Some special handling for specific errors (maybe generalise this if
                    # there end up being too many special cases)
                    # Upgrade bad_alloc to CATASTROPHE to allow for better automated handling of
                    # jobs that run out of memory
                    if 'std::bad_alloc' in fields['message']:
                        fields['level'] = 'CATASTROPHE'

                # Count this error
                self._levelCounter[fields['level']] += 1

                # Record some error details
                # N.B. We record 'IGNORED' errors as these really should be flagged for fixing
                if fields['level'] is 'IGNORED' or stdLogLevels[fields['level']] >= self._msgDetails:
                    if self._levelCounter[fields['level']] <= self._msgLimit: 
                        detailsHandled = False
                        for seenError in self._errorDetails[fields['level']]:
                            if seenError['message'] == line:
                                seenError['count'] += 1
                                detailsHandled = True
                                break
                        if detailsHandled == False:
                            self._errorDetails[fields['level']].append({'message': line, 'firstLine': lineCounter, 'count': 1})
                    elif self._levelCounter[fields['level']] == self._msgLimit + 1:
                        msg.warning("Found message number {0} at level {1} - this and further messages will be supressed from the report".format(self._levelCounter[fields['level']], fields['level']))
                    else:
                        # Overcounted
                        pass
                if 'Total payload read from COOL' in fields['message']:
                    msg.debug("Found COOL payload information at line {0}".format(line))
                    a = re.match(r'(\D+)(?P<bytes>\d+)(\D+)(?P<time>\d+[.]?\d*)(\D+)', fields['message'])
                    self._dbbytes += int(a.group('bytes'))
                    self._dbtime  += float(a.group('time'))


    ## Return data volume and time spend to retrieve information from the database
    def dbMonitor(self):
        return {'bytes' : self._dbbytes, 'time' : self._dbtime} if self._dbbytes > 0 or self._dbtime > 0 else None

    ## Return the worst error found in the logfile (first error of the most serious type)
    def worstError(self):
        worst = stdLogLevels['DEBUG']
        worstName = 'DEBUG'
        for lvl, count in self._levelCounter.iteritems():
            if count > 0 and stdLogLevels.get(lvl, 0) > worst:
                worstName = lvl
                worst = stdLogLevels[lvl]
        if len(self._errorDetails[worstName]) > 0:
            firstError = self._errorDetails[worstName][0]
        else:
            firstError = None

        return {'level': worstName, 'nLevel': worst, 'firstError': firstError}


    ## Return the first error found in the logfile above a certain loglevel
    def firstError(self, floor='ERROR'):
        firstLine = firstError = None
        firstLevel = stdLogLevels[floor]
        firstName = floor
        for lvl, count in self._levelCounter.iteritems():
            if (count > 0 and stdLogLevels.get(lvl, 0) >= stdLogLevels[floor] and
                (firstError == None or self._errorDetails[lvl][0]['firstLine'] < firstLine)):
                firstLine = self._errorDetails[lvl][0]['firstLine']
                firstLevel = stdLogLevels[lvl]
                firstName = lvl
                firstError = self._errorDetails[lvl][0]

        return {'level': firstName, 'nLevel': firstLevel, 'firstError': firstError}

    ## @brief Attempt to suck a core dump report from the current logfile
    # @note: Current implementation just eats lines until a 'normal' line is seen.
    # There is a slight problem here in that the end of core dump trigger line will not get parsed
    # TODO: fix this (OTOH core dump is usually the very last thing and fatal!)
    def coreDumpSvcParser(self, lineGenerator, firstline, firstLineCount):
        _eventCounter = _run = _event = _currentAlgorithm = _functionLine = _currentFunction = None
        coreDumpReport = 'Core dump from CoreDumpSvc'
        for line, linecounter in lineGenerator:
            m = self._regExp.match(line)
            if m == None:
                if 'Caught signal 11(Segmentation fault)' in line:
                    coreDumpReport = 'Segmentation fault'
                if 'Event counter' in line:
                    _eventCounter = line

                #Lookup: 'EventID: [Run,Evt,Lumi,Time,BunchCross,DetMask] = [267599,7146597,1,1434123751:0,0,0x0,0x0,0x0]'
                if 'EventID' in line:
                    match = re.findall('\[.*?\]', line)
                    if match and match.__len__() >= 2:      # Assuming the line contains at-least one key-value pair.
                        brackets = "[]"
                        commaDelimer = ','
                        keys = (match[0].strip(brackets)).split(commaDelimer)
                        values = (match[1].strip(brackets)).split(commaDelimer)

                        if 'Run' in keys:
                            _run = 'Run: ' + values[keys.index('Run')]

                        if 'Evt' in keys:
                            _event = 'Evt: ' + values[keys.index('Evt')]

                if 'Current algorithm' in line:
                    _currentAlgorithm = line
                if '<signal handler called>' in line:
                    _functionLine = linecounter+1
                if _functionLine and linecounter is _functionLine:
                    _currentFunction = line
            else:
                # Can this be done - we want to push the line back into the generator to be
                # reparsed in the normal way (might need to make the generator a class with the
                # __exec__ method supported (to get the line), so that we can then add a
                # pushback onto an internal FIFO stack
                # lineGenerator.pushback(line)
                break
        _eventCounter = 'Event counter: unknown' if not _eventCounter else _eventCounter
        _run = 'Run: unknown' if not _run else _run
        _event = 'Evt: unknown' if not _event else _event
        _currentAlgorithm = 'Current algorithm: unknown' if not _currentAlgorithm else _currentAlgorithm
        _currentFunction = 'Current Function: unknown' if not _currentFunction else 'Current Function: '+_currentFunction.split(' in ')[1].split()[0]
        coreDumpReport = '{0}: {1}; {2}; {3}; {4}; {5}'.format(coreDumpReport, _eventCounter, _run, _event, _currentAlgorithm, _currentFunction)

        # Core dumps are always fatal...
        msg.debug('Identified core dump - adding to error detail report')
        self._levelCounter['FATAL'] += 1
        self._errorDetails['FATAL'].append({'message': coreDumpReport, 'firstLine': firstLineCount, 'count': 1})

    def g494ExceptionParser(self, lineGenerator, firstline, firstLineCount):
        g4Report = firstline
        g4lines = 1
        if not 'Aborting execution' in g4Report:
            for line, linecounter in lineGenerator:
                g4Report += os.linesep + line
                g4lines += 1
                # Test for the closing string
                if '*** ' in line:
                    break
                if g4lines >= 25:
                    msg.warning('G4 exception closing string not found within {0} log lines of line {1}'.format(g4lines, firstLineCount))
                    break

        # G4 exceptions can be fatal or they can be warnings...
        msg.debug('Identified G4 exception - adding to error detail report')
        if "just a warning" in g4Report:
            if self._levelCounter['WARNING'] <= self._msgLimit:
                self._levelCounter['WARNING'] += 1
                self._errorDetails['WARNING'].append({'message': g4Report, 'firstLine': firstLineCount, 'count': 1})
            elif self._levelCounter['WARNING'] == self._msgLimit + 1:
                msg.warning("Found message number {0} at level WARNING - this and further messages will be supressed from the report".format(self._levelCounter['WARNING']))
        else:
            self._levelCounter['FATAL'] += 1
            self._errorDetails['FATAL'].append({'message': g4Report, 'firstLine': firstLineCount, 'count': 1})

    def g4ExceptionParser(self, lineGenerator, firstline, firstLineCount, g4ExceptionLineDepth):
        g4Report = firstline
        g4lines = 1
        for line, linecounter in lineGenerator:
            g4Report += os.linesep + line
            g4lines += 1
            # Test for the closing string
            if 'G4Exception-END' in line:
                break
            if g4lines >= g4ExceptionLineDepth:
                msg.warning('G4 exception closing string not found within {0} log lines of line {1}'.format(g4lines, firstLineCount))
                break

        # G4 exceptions can be fatal or they can be warnings...
        msg.debug('Identified G4 exception - adding to error detail report')
        if "-------- WWWW -------" in g4Report:
            if self._levelCounter['WARNING'] <= self._msgLimit:
                self._levelCounter['WARNING'] += 1
                self._errorDetails['WARNING'].append({'message': g4Report, 'firstLine': firstLineCount, 'count': 1})
            elif self._levelCounter['WARNING'] == self._msgLimit + 1:
                msg.warning("Found message number {0} at level WARNING - this and further messages will be supressed from the report".format(self._levelCounter['WARNING'])) 
        else:
            self._levelCounter['FATAL'] += 1
            self._errorDetails['FATAL'].append({'message': g4Report, 'firstLine': firstLineCount, 'count': 1})

    def pythonExceptionParser(self, lineGenerator, firstline, firstLineCount):
        pythonExceptionReport = ""
        lastLine = firstline
        pythonErrorLine = firstLineCount
        pyLines = 1
        for line, linecounter in lineGenerator:
            if 'Py:Athena' in line and 'INFO leaving with code' in line:
                pythonExceptionReport = lastLine
                pythonErrorLine = linecounter-1
                break
            if pyLines >= 25:
                msg.warning('Could not identify python exception correctly scanning {0} log lines after line {1}'.format(pyLines, firstLineCount))
                pythonExceptionReport = "Unable to identify specific exception"
                pythonErrorLine = firstLineCount
                break
            lastLine = line
            pyLines += 1

        msg.debug('Identified python exception - adding to error detail report')
        self._levelCounter['FATAL'] += 1
        self._errorDetails['FATAL'].append({'message': pythonExceptionReport, 'firstLine': pythonErrorLine, 'count': 1})

    def badAllocExceptionParser(self, lineGenerator, firstline, firstLineCount):
        badAllocExceptionReport = 'terminate after \'std::bad_alloc\'.'

        msg.debug('Identified bad_alloc - adding to error detail report')
        self._levelCounter['CATASTROPHE'] += 1
        self._errorDetails['CATASTROPHE'].append({'message': badAllocExceptionReport, 'firstLine': firstLineCount, 'count': 1})

    def rootSysErrorParser(self, lineGenerator, firstline, firstLineCount):
        msg.debug('Identified ROOT IO problem - adding to error detail report')
        self._levelCounter['FATAL'] += 1
        self._errorDetails['FATAL'].append({'message': firstline, 'firstLine': firstLineCount, 'count': 1})

    def __str__(self):
        return str(self._levelCounter) + str(self._errorDetails)


class scriptLogFileReport(logFileReport):
    def __init__(self, logfile=None, msgLimit=200, msgDetailLevel=stdLogLevels['ERROR']):
        self._levelCounter = {}
        self._errorDetails = {}
        self.resetReport()
        super(scriptLogFileReport, self).__init__(logfile, msgLimit, msgDetailLevel)

    def resetReport(self):
        self._levelCounter.clear()
        for level in stdLogLevels.keys() + ['UNKNOWN', 'IGNORED']:
            self._levelCounter[level] = 0

        self._errorDetails.clear()
        for level in self._levelCounter.keys():  # List of dicts {'message': errMsg, 'firstLine': lineNo, 'count': N}
            self._errorDetails[level] = []

    def scanLogFile(self, resetReport=False):
        if resetReport:
            self.resetReport()

        for log in self._logfile:
            msg.info('Scanning logfile {0}'.format(log))
            try:
                myGen = trfUtils.lineByLine(log)
            except IOError, e:
                msg.error('Failed to open transform logfile {0}: {1:s}'.format(log, e))
                # Return this as a small report
                self._levelCounter['ERROR'] = 1
                self._errorDetails['ERROR'] = {'message': str(e), 'firstLine': 0, 'count': 1}
                return

            for line, lineCounter in myGen:
                # TODO: This implementation currently only scans for Root SysErrors.
                # General solution would be a have common error parser for all system level
                # errors those all also handled by AthenaLogFileReport.
                if line.__contains__('SysError in <TFile::ReadBuffer>') or \
                   line.__contains__('SysError in <TFile::WriteBuffer>'):
                    self.rootSysErrorParser(line, lineCounter)

    # Return the worst error found in the logfile (first error of the most serious type)
    def worstError(self):
        worstlevelName = 'DEBUG'
        worstLevel = stdLogLevels[worstlevelName]
        for levelName, count in self._levelCounter.iteritems():
            if count > 0 and stdLogLevels.get(levelName, 0) > worstLevel:
                worstlevelName = levelName
                worstLevel = stdLogLevels[levelName]

        if len(self._errorDetails[worstlevelName]) > 0:
            firstError = self._errorDetails[worstlevelName][0]
        else:
            firstError = None

        return {'level': worstlevelName, 'nLevel': worstLevel, 'firstError': firstError}

    def __str__(self):
        return str(self._levelCounter) + str(self._errorDetails)

    def rootSysErrorParser(self, line, lineCounter):
        msg.debug('Identified ROOT IO problem - adding to error detail report')
        self._levelCounter['FATAL'] += 1
        self._errorDetails['FATAL'].append({'message': line, 'firstLine': lineCounter, 'count': 1})

## @brief return integrity of file using appropriate validation function
#  @ detail This method returns the integrity of a specified file using a
#  @ specified validation function.
def returnIntegrityOfFile(file, functionName):
    try:
        import PyJobTransforms.trfFileValidationFunctions as trfFileValidationFunctions
    except Exception as exception:
        msg.error('Failed to import module PyJobTransforms.trfFileValidationFunctions with error {error}'.format(error = exception))
        raise
    validationFunction = getattr(trfFileValidationFunctions, functionName)
    return validationFunction(file)


## @brief perform standard file validation
#  @ detail This method performs standard file validation in either serial or
#  @ parallel and updates file integrity metadata.
def performStandardFileValidation(dictionary, io, parallelMode = False):
    if parallelMode == False:
        msg.info('Starting legacy (serial) file validation')
        for (key, arg) in dictionary.items():
            if not isinstance(arg, argFile):
                continue
            if not arg.io == io:
                continue
            if arg.auxiliaryFile:
                continue
            
            msg.info('Validating data type %s...' % key)
    
            for fname in arg.value:
                msg.info('Validating file %s...' % fname)
    
                if io == "output":
                    msg.info('{0}: Testing corruption...'.format(fname))
                    if arg.getSingleMetadata(fname, 'integrity') is True:
                        msg.info('Corruption test passed.')
                    elif arg.getSingleMetadata(fname, 'integrity') is False:
                        msg.error('Corruption test failed.')
                        raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_VALIDATION_FAIL'), 'File %s did not pass corruption test' % fname)
                    elif arg.getSingleMetadata(fname, 'integrity') == 'UNDEFINED':
                        msg.info('No corruption test defined.')
                    elif arg.getSingleMetadata(fname, 'integrity') is None:
                        msg.error('Could not check for file integrity')
                        raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_VALIDATION_FAIL'), 'File %s might be missing' % fname)
                    else:    
                        msg.error('Unknown rc from corruption test.')
                        raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_VALIDATION_FAIL'), 'File %s did not pass corruption test' % fname)
    
    
                msg.info('{0}: Testing event count...'.format(fname))
                if arg.getSingleMetadata(fname, 'nentries') is not None:
                    msg.info('Event counting test passed ({0!s} events).'.format(arg.getSingleMetadata(fname, 'nentries')))
                else:    
                    msg.error('Event counting test failed.')
                    raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_VALIDATION_FAIL'), 'File %s did not pass corruption test' % fname)
                  
    
                msg.info('{0}: Checking if guid exists...'.format(fname))
                if arg.getSingleMetadata(fname, 'file_guid') is None:
                    msg.error('Guid could not be determined.')
                    raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_VALIDATION_FAIL'), 'File %s did not pass corruption test' % fname)
                elif arg.getSingleMetadata(fname, 'file_guid') == 'UNDEFINED':
                    msg.info('Guid not defined.')
                else:
                    msg.info('Guid is %s' % arg.getSingleMetadata(fname, 'file_guid'))
        msg.info('Stopping legacy (serial) file validation')
    if parallelMode == True:
        msg.info('Starting parallel file validation')
        # Create lists of files and args. These lists are to be used with zip in
        # order to check and update file integrity metadata as appropriate.
        fileList = []
        argList = []
        # Create a list of the integrity functions for files.
        integrityFunctionList = []
        # Create a list for collation of file validation jobs for submission to
        # the parallel job processor.
        jobs = []
        for (key, arg) in dictionary.items():
            if not isinstance(arg, argFile):
                continue
            if not arg.io == io:
                continue
            msg.debug('Collating list of files for validation')
            for fname in arg.value:
                msg.debug('Appending file {fileName} to list of files for validation'.format(fileName = str(fname)))
                # Append the current file to the file list.
                fileList.append(fname)
                # Append the current arg to the arg list.
                argList.append(arg)
                # Append the current integrity function name to the integrity
                # function list if it exists. If it does not exist, raise an
                # exception.
                if arg.integrityFunction:
                    integrityFunctionList.append(arg.integrityFunction)
                else:
                    msg.error('Validation function for file {fileName} not available for parallel file validation'.format(fileName = str(fname)))
                    raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_VALIDATION_FAIL'), 'Validation function for file %s not available for parallel file validation' % str(fname))
                # Compose a job for validation of the current file using the
                # appropriate validation function, which is derived from the
                # associated data attribute arg.integrityFunction.
                jobs.append(
                    trfUtils.Job(
                        name = "validation of file {fileName}".format(
                        fileName = str(fname)),
                        workFunction = returnIntegrityOfFile,
                        workFunctionKeywordArguments = {
                            'file': fname,
                            'functionName': arg.integrityFunction
                        },
                        workFunctionTimeout = 600
                    )
                )
        # Contain the file validation jobs in a job group for submission to the
        # parallel job processor.
        jobGroup1 = trfUtils.JobGroup(
            name = "standard file validation",
            jobs = jobs
        )
        # Prepare the parallel job processor.
        parallelJobProcessor1 = trfUtils.ParallelJobProcessor()
        # Submit the file validation jobs to the parallel job processor.
        msg.info('Submitting file validation jobs to parallel job processor')
        parallelJobProcessor1.submit(jobSubmission = jobGroup1)
        resultsList = parallelJobProcessor1.getResults()
        msg.info('Parallel file validation complete')
        # Update file metadata with integrity results using the lists fileList,
        # argList and resultsList.
        msg.info('Processing file integrity results')
        for currentFile, currentArg, currentIntegrityFunction, currentResult in zip(fileList, argList, integrityFunctionList, resultsList):
            msg.info('{IO} file {fileName} has integrity status {integrityStatus} as determined by integrity function {integrityFunction}'.format(
                IO = str(io),
                fileName = str(currentFile),
                integrityStatus = str(currentResult),
                integrityFunction = str(currentIntegrityFunction)
            ))
            # If the first (Boolean) element of the result tuple for the current
            # file is True, update the integrity metadata. If it is False, raise
            # an exception.
            if currentResult[0] == True:
                msg.info('Updating integrity metadata for file {fileName}'.format(fileName = str(currentFile)))
                currentArg._setMetadata(files=[currentFile,], metadataKeys={'integrity': currentResult[0]})
            else:
                exceptionMessage = "{IO} file validation failure on file {fileName} with integrity status {integrityStatus} as determined by integrity function {integrityFunction}".format(
                    IO = str(io),
                    fileName = str(currentFile),
                    integrityStatus = str(currentResult),
                    integrityFunction = str(currentIntegrityFunction)
                )
                msg.error("exception message: {exceptionMessage}".format(
                    exceptionMessage = exceptionMessage
                ))
                if io == 'input':
                    exitCodeName = 'TRF_INPUT_FILE_VALIDATION_FAIL'
                elif io == 'output':
                    exitCodeName = 'TRF_OUTPUT_FILE_VALIDATION_FAIL'
                raise trfExceptions.TransformValidationException(
                    trfExit.nameToCode(exitCodeName),
                    exceptionMessage
                )
            # Perform a check to determine if the file integrity metadata is
            # correct.
            if currentArg.getSingleMetadata(currentFile, metadataKey = 'integrity', populate = False) == currentResult[0]:
                msg.debug("file integrity metadata update successful")
            else:
                msg.error("file integrity metadata update unsuccessful")
        msg.info('Stopping parallel file validation')


## @brief Small class used for vailiadating event counts between input and output files
class eventMatch(object):

    ## @brief check in- and output event counts
    # @details Class to verify that in- and output event counts are in a reasonable relationship.
    # @param @c executor eventCountConf instance for this check
    # @param @c eventCountConf dictionary to replace or append to default, see code for details
    # @param @c eventCountConfOver write Replace rather than append eventCountConf (Default: False)
    # Takes efficiencies into account.
    # All data is taken from _trf dict
    def __init__(self, executor, eventCountConf=None, eventCountConfOverwrite=False):
        self._executor = executor
        self._eventCount = None

        ## @note This double dictionary is formed of INPUT data, then a dictionary of the expected
        #  event counts from different output data types. If there is no exact match for the output
        #  datatype then globbing matches are allowed.
        #  Thus self._eventCountConf[input][output] gives the test for input -> output.
        #  The dictionary recognises the following options:
        #  - @c match : exact match of input and output events, n_in = n_out
        #  - @c filter : any event count from 0 up to input events is ok, 0 <= n_out <= n_in
        #  - @c minEff : any event count from n_in * eventAcceptanceEfficiency <= n_out <= n_in
        #  - @c float in range [0,1] : same as minEff with this efficiency factor
        #  For any case where the output events can be less than the input ones an integer conversion
        #  is applied, so the result is rounded down. i.e., 1 * 0.5 -> 0.
        simEventEff = 0.995
        self._eventCountConf = {}
        self._eventCountConf['EVNT'] = {'EVNT_MRG':"match", "HITS": simEventEff, "EVNT_CAVERN": simEventEff, "EVNT_Stopped": simEventEff}
        self._eventCountConf['EVNT_CAVERN'] = {'HITS': simEventEff}
        self._eventCountConf['EVNT_COSMICS'] = {'HITS': simEventEff}
        self._eventCountConf['EVNT_Stopped'] = {'HITS': simEventEff}
        self._eventCountConf['HITS'] = {'RDO':"match", "HITS_MRG":"match", 'HITS_FILT': simEventEff, "RDO_FILT": "filter"}
        self._eventCountConf['BS'] = {'ESD': "match", 'DRAW_*':"filter", 'NTUP_*':"filter", "BS_MRG":"match", 'DESD*': "filter", 'AOD':"match", 'DAOD*':"filter"}
        self._eventCountConf['RDO*'] = {'ESD': "match", 'DRAW_*':"filter", 'NTUP_*':"filter", "RDO_MRG":"match", "RDO_TRIG":"match", 'AOD':"match", 'DAOD*':"filter"}
        self._eventCountConf['ESD'] = {'ESD_MRG': "match", 'AOD':"match", 'DESD*':"filter", 'DAOD_*':"filter", 'NTUP_*':"filter"}
        self._eventCountConf['AOD'] = {'AOD_MRG' : "match", 'TAG':"match", "NTUP_*":"filter", "DAOD_*":"filter", 'NTUP_*':"filter"}
        self._eventCountConf['AOD_MRG'] = {'TAG':"match"}
        self._eventCountConf['DAOD_*'] = {'DAOD_*_MRG' : "match"}
        self._eventCountConf['TAG'] = {'TAG_MRG': "match"}
        self._eventCountConf['HIST'] = {'HIST_MRG': "match"}
        self._eventCountConf['NTUP_COMMON'] = {'DNTUP*': "filter"}
        self._eventCountConf['NTUP_*'] = {'NTUP_*_MRG': "match"}
        # Next one comprises special data type names for smart merging of AthenaMP worker outputs
        self._eventCountConf['POOL_MRG_INPUT'] = {'POOL_MRG_OUTPUT': "match"}


        if eventCountConf:
            if eventCountConfOverwrite == True:
                self._eventCountConf = eventCountConf
            else:
                self._eventCountConf.update(eventCountConf)

        msg.debug('Event count check configuration is: {0}'.format(self._eventCountConf))
        if hasattr(self._executor, 'name'):
            msg.debug('Event count check ready for executor {0}'.format(self._executor.name))

        if self._executor is not None:
            self.configureCheck(override=False)

    @property
    def eventCount(self):
        return self._eventCount

    ## @brief Setup the parameters needed to define particular checks
    #  @param override If set then configure the checks using this dictionary, which needs
    #  to have keys @c inEventDict, @c outEventDict, @c skipEvents, @c maxEvents, @c evAccEff
    #  @note Default is to configure the checks from the associated executor
    def configureCheck(self, override=False):
        if override:
            msg.info('Overriding check configuration with: {0}'.format(override))
            self._inEventDict = override['inEventDict']
            self._outEventDict = override['outEventDict']
            self._skipEvents = override['skipEvents']
            self._maxEvents = override['maxEvents']
            self._evAccEff = override['evAccEff']
        else:
            # Input data from executor
            self._inEventDict = {}
            for dataTypeName in self._executor.input:
                try:
                    self._inEventDict[dataTypeName] = self._executor.conf.dataDictionary[dataTypeName].nentries
                    msg.debug('Input data type {0} has {1} events'.format(dataTypeName, self._inEventDict[dataTypeName]))
                except KeyError:
                    msg.warning('Found no dataDictionary entry for input data type {0}'.format(dataTypeName))

            # Output data from executor
            self._outEventDict = {}
            for dataTypeName in self._executor.output:
                try:
                    self._outEventDict[dataTypeName] = self._executor.conf.dataDictionary[dataTypeName].nentries
                    msg.debug('Output data type {0} has {1} events'.format(dataTypeName, self._outEventDict[dataTypeName]))
                except KeyError:
                    msg.warning('Found no dataDictionary entry for output data type {0}'.format(dataTypeName))

            # Find if we have a skipEvents applied
            if self._executor.conf.argdict.has_key("skipEvents"):
                self._skipEvents = self._executor.conf.argdict['skipEvents'].returnMyValue(exe=self._executor)
            else:
                self._skipEvents = None

            # Find if we have a maxEvents applied
            if self._executor.conf.argdict.has_key("maxEvents"):
                self._maxEvents = self._executor.conf.argdict['maxEvents'].returnMyValue(exe=self._executor)
                if self._maxEvents == -1:
                    self._maxEvents = None
            else:
                self._maxEvents = None

            # Global eventAcceptanceEfficiency set?
            if self._executor.conf.argdict.has_key("eventAcceptanceEfficiency"):
                self._evAccEff = self._executor.conf.argdict['eventAcceptanceEfficiency'].returnMyValue(exe=self._executor)
                if (self._evAccEff == None):
                    self._evAccEff = 0.99
            else:
                self._evAccEff = 0.99

        msg.debug("Event check conf: {0} {1}, {2}, {3}, {4}".format(self._inEventDict, self._outEventDict, self._skipEvents,
                                                                    self._maxEvents, self._evAccEff))


    ## @brief Perform an event count check
    def decide(self):
        # We have all that we need to proceed: input and output data, skip and max events plus any efficiency factor
        # So loop over the input and output data and make our checks
        for inData, neventsInData in self._inEventDict.iteritems():
            if type(neventsInData) not in (int, long):
                msg.warning('File size metadata for {inData} was not countable, found {neventsInData}. No event checks possible for this input data.'.format(inData=inData, neventsInData=neventsInData))
                continue
            if inData in self._eventCountConf:
                inDataKey = inData
            else:
                # OK, try a glob match in this case (YMMV)
                matchedInData = False
                for inDataKey in self._eventCountConf.keys():
                    if fnmatch.fnmatch(inData, inDataKey):
                        msg.info("Matched input data type {inData} to {inDataKey} by globbing".format(inData=inData, inDataKey=inDataKey))
                        matchedInData = True
                        break
                if not matchedInData:
                    msg.warning('No defined event count match for {inData} -> {outData}, so no check(s) possible in this case.'.format(inData=inData, outData=self._outEventDict.keys()))
                    continue

            # Now calculate the expected number of processed events for this input
            expectedEvents = neventsInData
            if self._skipEvents is not None and self._skipEvents > 0:
                expectedEvents -= self._skipEvents
                if expectedEvents < 0:
                    msg.warning('skipEvents was set higher than the input events in {inData}: {skipEvents} > {neventsInData}. This is not an error, but it is not a normal configuration. Expected events is now 0.'.format(inData=inData, skipEvents=self._skipEvents, neventsInData=neventsInData))
                    expectedEvents = 0
            if self._maxEvents is not None:
                if expectedEvents < self._maxEvents:
                    if self._skipEvents is not None:
                        msg.warning('maxEvents was set higher than inputEvents-skipEvents for {inData}: {maxEvents} > {neventsInData}-{skipEvents}. This is not an error, but it is not a normal configuration. Expected events remains {expectedEvents}.'.format(inData=inData, maxEvents=self._maxEvents, neventsInData=neventsInData, skipEvents=self._skipEvents, expectedEvents=expectedEvents))
                    else:
                        msg.warning('maxEvents was set higher than inputEvents for {inData}: {maxEvents} > {neventsInData}. This is not an error, but it is not a normal configuration. Expected events remains {expectedEvents}.'.format(inData=inData, maxEvents=self._maxEvents, neventsInData=neventsInData, expectedEvents=expectedEvents))
                else:
                    expectedEvents = self._maxEvents
            msg.debug('Expected number of processed events for {0} is {1}'.format(inData, expectedEvents))

            # Loop over output data - first find event count configuration
            for outData, neventsOutData in self._outEventDict.iteritems():
                if type(neventsOutData) not in (int, long):
                    msg.warning('File size metadata for {outData} was not countable, found "{neventsOutData}". No event checks possible for this output data.'.format(outData=outData, neventsOutData=neventsOutData))
                    continue
                if outData in self._eventCountConf[inDataKey]:
                    checkConf = self._eventCountConf[inDataKey][outData]
                    outDataKey = outData
                else:
                    # Look for glob matches
                    checkConf = None
                    for outDataKey, outDataConf in self._eventCountConf[inDataKey].iteritems():
                        if fnmatch.fnmatch(outData, outDataKey):
                            msg.info('Matched output data type {outData} to {outDatakey} by globbing'.format(outData=outData, outDatakey=outDataKey))
                            outDataKey = outData
                            checkConf = outDataConf
                            break
                    if not checkConf:
                        msg.warning('No defined event count match for {inData} -> {outData}, so no check possible in this case.'.format(inData=inData, outData=outData))
                        continue
                msg.debug('Event count check for {inData} to {outData} is {checkConf}'.format(inData=inData, outData=outData, checkConf=checkConf))

                # Do the check for thsi input/output combination
                if checkConf is 'match':
                    # We need an exact match
                    if neventsOutData == expectedEvents:
                        msg.info("Event count check for {inData} to {outData} passed: all processed events found ({neventsOutData} output events)".format(inData=inData, outData=outData, neventsOutData=neventsOutData))
                    else:
                        raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_VALIDATION_EVENTCOUNT'),
                                                                         'Event count check for {inData} to {outData} failed: found {neventsOutData} events, expected {expectedEvents}'.format(inData=inData, outData=outData, neventsOutData=neventsOutData, expectedEvents=expectedEvents))
                elif checkConf is 'filter':
                    if neventsOutData <= expectedEvents and neventsOutData >= 0:
                        msg.info("Event count check for {inData} to {outData} passed: found ({neventsOutData} output events selected from {expectedEvents} processed events)".format(inData=inData, outData=outData, neventsOutData=neventsOutData, expectedEvents=expectedEvents))
                    else:
                        raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_VALIDATION_EVENTCOUNT'),
                                                                         'Event count check for {inData} to {outData} failed: found {neventsOutData} events, expected from 0 to {expectedEvents}'.format(inData=inData, outData=outData, neventsOutData=neventsOutData, expectedEvents=expectedEvents))
                elif checkConf is 'minEff':
                    if neventsOutData >= int(expectedEvents * self._evAccEff) and neventsOutData <= expectedEvents:
                        msg.info("Event count check for {inData} to {outData} passed: found ({neventsOutData} output events selected from {expectedEvents} processed events)".format(inData=inData, outData=outData, neventsOutData=neventsOutData, expectedEvents=expectedEvents))
                    else:
                        raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_VALIDATION_EVENTCOUNT'),
                                                                         'Event count check for {inData} to {outData} failed: found {neventsOutData} events, expected from {minEvents} to {expectedEvents}'.format(inData=inData, outData=outData, neventsOutData=neventsOutData,
                                                                                                                                                                 minEvents=int(expectedEvents * self._evAccEff), expectedEvents=expectedEvents))
                elif isinstance(checkConf, (float, int, long)):
                    checkConf = float(checkConf)
                    if checkConf < 0.0 or checkConf > 1.0:
                        raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_VALIDATION_EVENTCOUNT'),
                                                                         'Event count check for {inData} to {outData} is misconfigured: the efficiency factor of {eff} is not between 0 and 1.'.format(inData=inData, outData=outData, eff=checkConf))
                    if neventsOutData >= int(expectedEvents * checkConf) and neventsOutData <= expectedEvents:
                        msg.info("Event count check for {inData} to {outData} passed: found ({neventsOutData} output events selected from {expectedEvents} processed events)".format(inData=inData, outData=outData, neventsOutData=neventsOutData, expectedEvents=expectedEvents))
                    else:
                        raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_VALIDATION_EVENTCOUNT'),
                                                                         'Event count check for {inData} to {outData} failed: found {neventsOutData} events, expected from {minEvents} to {expectedEvents}'.format(inData=inData, outData=outData, neventsOutData=neventsOutData,
                                                                                                                                                                 minEvents=int(expectedEvents * checkConf), expectedEvents=expectedEvents))
                else:
                    raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_VALIDATION_EVENTCOUNT'),
                                                                     'Unrecognised event count configuration for {inData} to {outData}: "{conf}" is not known'.format(inData=inData, outData=outData, conf=checkConf))
            self._eventCount = expectedEvents
        return True
