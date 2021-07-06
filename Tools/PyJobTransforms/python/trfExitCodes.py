# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfExitCodes
#
# @brief Module for transform exit codes
# @details Define all transform exit codes with their acronymns
# @remarks Usual usage is to import @c trfExit from this module
# @author atlas-comp-transforms-dev@cern.ch
#

import signal

import logging
msg = logging.getLogger(__name__)

## @brief Little class to hold the three attributes of a transform error
class trfExitCode(object):
    
    ## @brief Exit code class instantiation
    #  @param acronymn The internal transform acronymn for this error code
    #  @param numcode The numerical exit code for this error
    #  @param description A human comprehensible description of this error's meaning
    #  @param signame The original signal name (only relevant for signals)
    def __init__(self, acronymn, numcode, description, signalname = ""):
        self._acronymn = acronymn
        self._numcode = numcode
        self._description = description
        self._signalname = signalname
        
    @property
    def acronymn(self):
        return self._acronymn
    
    @property
    def numcode(self):
        return self._numcode
    
    @property
    def description(self):
        return self._description

    @property
    def signalname(self):
        return self._signalname

## @note Restructured exit codes to avoid clashes with the recognised exit codes from
#  the old transforms. These are:
#        self.errorCodes['transExitCode'][1] = 'Unspecified error, consult log file'
#        self.errorCodes['transExitCode'][2] = 'Payload core dump'
#        self.errorCodes['transExitCode'][6] = 'TRF_SEGVIO - Segmentation violation'
#        self.errorCodes['transExitCode'][10] = 'ATH_FAILURE - Athena non-zero exit'
#        self.errorCodes['transExitCode'][26] = 'TRF_ATHENACRASH - Athena crash'
#        self.errorCodes['transExitCode'][30] = 'TRF_PYT - transformation python error'
#        self.errorCodes['transExitCode'][31] = 'TRF_ARG - transformation argument error'
#        self.errorCodes['transExitCode'][32] = 'TRF_DEF - transformation definition error'
#        self.errorCodes['transExitCode'][33] = 'TRF_ENV - transformation environment error'
#        self.errorCodes['transExitCode'][34] = 'TRF_EXC - transformation exception'
#        self.errorCodes['transExitCode'][40] = 'Athena crash - consult log file'
#        self.errorCodes['transExitCode'][41] = 'TRF_OUTFILE - output file error'
#        self.errorCodes['transExitCode'][42] = 'TRF_CONFIG - transform config file error'
#        self.errorCodes['transExitCode'][50] = 'TRF_DB - problems with database'
#        self.errorCodes['transExitCode'][51] = 'TRF_DBREL_TARFILE - Problems with the DBRelease tarfile'
#        self.errorCodes['transExitCode'][60] = 'TRF_GBB_TIME - GriBB - output limit exceeded (time, memory, CPU)'
#        self.errorCodes['transExitCode'][79] = 'Copying input file failed'
#        self.errorCodes['transExitCode'][80] = 'file in trf definition not found, using the expandable syntax'
#        self.errorCodes['transExitCode'][81] = 'file in trf definition not found, using the expandable syntax -- pileup case'
#        self.errorCodes['transExitCode'][85] = 'analysis output merge crash - consult log file'
#        self.errorCodes['transExitCode'][98] = 'Oracle error - session limit reached'
#        self.errorCodes['transExitCode'][99] = 'TRF_UNKNOWN - unknown transformation error'
#        self.errorCodes['transExitCode'][102] = 'One of the output files did not get produced by the job'
#        self.errorCodes['transExitCode'][104] = 'Copying the output file to local SE failed (md5sum or size mismatch, or LFNnonunique)'
#        self.errorCodes['transExitCode'][126] = 'Transformation not executable - consult log file'
#        self.errorCodes['transExitCode'][127] = 'Transformation not installed in CE'
#        self.errorCodes['transExitCode'][134] = 'Athena core dump or timeout, or conddb DB connect exception'
#        self.errorCodes['transExitCode'][141] = "No input file available - check availability of input dataset at site"
#        self.errorCodes['transExitCode'][200] = 'Log file not transferred to destination'
#        self.errorCodes['transExitCode'][220] = 'Proot: An exception occurred in the user analysis code'
#        self.errorCodes['transExitCode'][221] = 'Proot: Framework decided to abort the job due to an internal problem'
#        self.errorCodes['transExitCode'][222] = 'Proot: Job completed without reading all input files'
#        self.errorCodes['transExitCode'][223] = 'Proot: Input files cannot be opened'
#  Taken from: https://svnweb.cern.ch/trac/panda/browser/monitor/ErrorCodes.py?rev=13963
#
#  Because of this restructuring the top bits categorisation of the error codes is no longer maintained. 
#
#  Further error codes to avoid are:
#   ERROR_OPEN_FAILED=232
#   ERROR_HIST_MISSING=233
#   ERROR_SSB_DIRECT=235
#   ERROR_SSB_DIRECT_DOWN=236
#  These are being used by the FAX analysis wrapper
#
# @note In certain circumstances a transform may catch a signal
#  and exit more cleanly (SIGINT, SIGUSR1), in which case
#  it will replicate the shell signaled exit code (128 + SIGNUM).
#  These are mapped dynamically to k/v TRF_SIG_SIGNAME : SIGNUM+128.
#  This is partly because numeric codes vary between platforms 
#  (e.g., SIGUSR1 = 10 (Linux) or 30 (OS X)), but it's also more 
#  reliable than typing it all by hand!
class trfExitCodes(object):
    ## @note Hold error codes in a list of trfExitCode objects
    _errorCodeList = list()
    _errorCodeList.append(trfExitCode('OK', 0, 'Successful exit'))
    _errorCodeList.append(trfExitCode('NEEDCHECK', 1, 'Job needs manual intervention'))
    _errorCodeList.append(trfExitCode('TRF_SETUP', 3, 'Transform setup error'))
    _errorCodeList.append(trfExitCode('TRF_ARG_CONV_FAIL', 4, 'Failure to convert transform arguments to correct type'))
    _errorCodeList.append(trfExitCode('TRF_ARG_OUT_OF_RANGE', 5, 'Argument out of allowed range'))
    _errorCodeList.append(trfExitCode('TRF_ARG_ERROR', 7, 'Problem with an argument given to the transform'))
    _errorCodeList.append(trfExitCode('TRF_ARG_DATASET', 8, 'Inconsistent dataset value in input file list'))
    _errorCodeList.append(trfExitCode('TRF_INPUT_FILE_ERROR', 9, 'Error found with transform input file'))
    _errorCodeList.append(trfExitCode('TRF_OUTPUT_FILE_ERROR', 11, 'Error when handling transform output file'))
    _errorCodeList.append(trfExitCode('TRF_GRAPH_ERROR', 12, 'Problem in setting up the substep graph'))
    _errorCodeList.append(trfExitCode('TRF_EXECUTION_PATH_ERROR', 13, 'Problem trying to generate requested outputs from given inputs - graph give no path to do this'))
    _errorCodeList.append(trfExitCode('TRF_KEY_ERROR', 14, 'Key error in a transform internal dictionary'))
    _errorCodeList.append(trfExitCode('TRF_NOEVENTS', 15, 'No events to be processed in the input file - skipEvents is higher than actual event number'))
    _errorCodeList.append(trfExitCode('TRF_ARG_CHOICES_FAIL', 16, 'Invalid selection in a choice restricted argument'))
    _errorCodeList.append(trfExitCode('TRF_ARG_MISSING', 17, 'Mandatory argument is missing'))
    _errorCodeList.append(trfExitCode('TRF_REDUCTION_CONFIG_ERROR', 18, 'Invalid configuration of a reduction job'))
    _errorCodeList.append(trfExitCode('TRF_GRAPH_STEERING_ERROR', 19, 'Problem when trying to apply steering to the substep graph'))
    _errorCodeList.append(trfExitCode('TRF_INTERNAL_REPORT_ERROR', 35, 'Internal error while generating transform reports'))
    _errorCodeList.append(trfExitCode('TRF_METADATA_CALL_FAIL', 36, 'Call to external metadata program failed'))
    _errorCodeList.append(trfExitCode('TRF_RUNTIME_ERROR', 37, 'General runtime error'))
    _errorCodeList.append(trfExitCode('TRF_EXEC_VALIDATION_EVENTCOUNT', 38, 'Event count validation failed'))
    _errorCodeList.append(trfExitCode('TRF_DBRELEASE_PROBLEM', 39, 'Failed to setup DBRelease properly'))
    _errorCodeList.append(trfExitCode('TRF_FILEMERGE_PROBLEM', 52, 'Problem while attempting to merge output files'))
    _errorCodeList.append(trfExitCode('TRF_EXEC', 64, 'General failure in transform substep executor'))
    _errorCodeList.append(trfExitCode('TRF_EXEC_FAIL', 65, 'Non-zero exit code from transform substep executor'))
    _errorCodeList.append(trfExitCode('TRF_EXEC_VALIDATION_FAIL', 66, 'Validation failure in transform substep executor'))
    _errorCodeList.append(trfExitCode('TRF_EXEC_TIMEOUT', 67, 'Transform substep executor timed out'))
    _errorCodeList.append(trfExitCode('TRF_EXEC_LOGERROR', 68, 'Errors found in substep executor logfile'))
    _errorCodeList.append(trfExitCode('TRF_EXEC_SETUP_FAIL', 69, 'Transform substep executor setup failed'))
    _errorCodeList.append(trfExitCode('TRF_EXEC_SETUP_WRAPPER', 70, 'Transform substep executor wrapper script problem'))
    _errorCodeList.append(trfExitCode('TRF_LOGFILE_FAIL', 71, 'Problem with substep executor logfile'))
    _errorCodeList.append(trfExitCode('TRF_AMI_ERROR' , 72, 'Problem getting AMI tag info'))
    _errorCodeList.append(trfExitCode('TRF_EXEC_RUNARGS_ERROR' , 73, 'Problem with executor runargs file'))    
    _errorCodeList.append(trfExitCode('TRF_INPUT_FILE_VALIDATION_FAIL' , 74, 'Input file failed validation'))
    _errorCodeList.append(trfExitCode('TRF_OUTPUT_FILE_VALIDATION_FAIL' , 75, 'Output file failed validation'))
    _errorCodeList.append(trfExitCode('TRF_UNEXPECTED_TRF_EXCEPTION', 250, 'Transform exception raised which the transform did not handle'))
    _errorCodeList.append(trfExitCode('TRF_UNEXPECTED_OTHER_EXCEPTION', 251, 'General exception raised which the transform did not handle'))
    _errorCodeList.append(trfExitCode('TRF_EXTERNAL', 251, 'Transform error in external call'))
    _errorCodeList.append(trfExitCode('TRF_INTERNAL', 252, 'Internal transform error'))
    _errorCodeList.append(trfExitCode('TRF_UNKNOWN', 253, 'Unknown error code'))
    
    # Add signaled exits without duplicates
    _errorCodeList.extend([trfExitCode('TRF_SIG_'+signalname, getattr(signal, signalname)+128, 'Transform received signal {0}'.format(signalname), signalname) 
                           for signalname in dir(signal) if signalname.startswith('SIG') 
                           and '_' not in signalname
                           and 'SIGIOT' not in signalname
                           and 'SIGCLD' not in signalname
                           and 'SIGPOLL' not in signalname])

    # Now map the entries to fast lookup dictionaries
    _nameToCodeDict = dict()
    _codeToNameDict = dict()
    _nameToDescDict = dict()
    _codeToSignalnameDict = dict()
    for error in _errorCodeList:
        _nameToCodeDict[error.acronymn] = error.numcode
        _codeToNameDict[error.numcode] = error.acronymn
        _nameToDescDict[error.acronymn] = error.description
        _codeToSignalnameDict[error.numcode] = error.signalname
    

    def __init__(self):
        pass    
    
    @staticmethod
    def nameToCode(name = None):
        if name in trfExitCodes._nameToCodeDict:
            return trfExitCodes._nameToCodeDict[name]
        else:
            msg.warning('Could not map exit name {0} to an exit code'.format(name))
            return trfExitCodes._nameToCodeDict['TRF_UNKNOWN']
        
    @staticmethod
    def codeToName(code = None):
        if code in trfExitCodes._codeToNameDict:
            return trfExitCodes._codeToNameDict[code]
        else:
            msg.warning('Could not map exit code {0} to an exit name'.format(code))
            return 'TRF_UNKNOWN'

    @staticmethod
    def nameToDesc(name = None):
        if name in trfExitCodes._nameToDescDict:
            return trfExitCodes._nameToDescDict[name]
        else:
            msg.warning('Could not map exit name {0} to a description'.format(name))
            return 'No description available'

    @staticmethod
    def codeToSignalname(code = None):
        if code in trfExitCodes._codeToSignalnameDict:
            return trfExitCodes._codeToSignalnameDict[code]
        else:
            # Not a signal we knew about
            return ''


trfExit = trfExitCodes()
