# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfArgClasses
# @brief Transform argument class definitions
# @author atlas-comp-transforms-dev@cern.ch
# @version $Id: trfArgClasses.py 609252 2014-07-29 16:20:33Z wbreaden $


import argparse
import bz2
import copy
import glob
import io
import os
import re
import subprocess
import sys
import traceback
import types
import uuid

from fnmatch import fnmatch

import logging
msg = logging.getLogger(__name__)

import PyJobTransforms.trfExceptions as trfExceptions

from PyJobTransforms.trfFileUtils import athFileInterestingKeys, AthenaFileInfo, NTUPEntries, HISTEntries, urlType, ROOTGetSize
from PyJobTransforms.trfUtils import call, cmpMetadata, cliToKey
from PyJobTransforms.trfExitCodes import trfExit as trfExit
from PyJobTransforms.trfDecorators import timelimited
import os


## @class argFactory
#  @brief Factory class used to generate argument class instances for argparse
class argFactory(object):
    def __init__(self, genclass, *args, **kwargs):
        msg.debug('Initialised class %s with args=%s; kwargs=%s' % (genclass, args, kwargs))
        self._genclass = genclass
        self._args = args
        self._kwargs = kwargs
    
    def __call__(self, valueString=None):
        msg.debug('Called class %s with value=%s; args=%s; kwargs=%s' % (self._genclass, valueString, self._args, self._kwargs))
        
        # Wrap this step in our own try/except because if this goes wrong we want to see the exception
        # instead of having it masked by the argparse module
        try:
            # Passing None suppresses the value passed to the constructor, thus the constructor's own
            # default value is used - generally this will match the default value for the underlying
            # python object
            if valueString is None:
                obj = self._genclass(*self._args, **self._kwargs)
            else:
                obj = self._genclass(valueString, *self._args, **self._kwargs)
        except Exception, e:
            msg.fatal('Got this exception raised when calling object factory: {0}'.format(e))
            raise
        return obj
    
    def __str__(self):
        return 'argFactory for {0}, args {1}, kwargs {2}'.format(self._genclass, self._args, self._kwargs)


## @class argument
#  @brief Basic argument class holding a value which can be get and set
#  @note Any argument type is supported
class argument(object):
    
    ## @brief Initialise argument class
    #  @param value Initial value to be set
    #  @param runarg Boolean specifiying if this is a run time argument 
    #  (specifically passed to athena by the AthenaExecutor, other executors may do the same).
    #  @param name Name for this argument 
    def __init__(self, value = None, runarg = True, name = None):
        self._runarg = runarg
        self._name = name
        self._desc = {}
        
        ## @note We have a default of None here, but all derived classes should
        #  definitely have their own value setter and translate this value to something
        #  sensible for their underlying value type. 
        #  N.B. As most argument classes use this default constructor it @b must
        #  call the @c @value.setter function!
        self.value = value

    ## @brief Return argument value
    #  @returns Current value
    @property
    def value(self):
        return self._value

    ## @brief Set argument value
    #  @note No conversion or coercion done
    @value.setter
    def value(self, value):
        self._value = value
    
    ## @brief Return runarg status
    @property
    def isRunarg(self):
        return self._runarg
    
    ## @brief Name getter
    @property
    def name(self):
        return self._name
    
    ## @brief Name setter
    @name.setter
    def name(self, value):
         self._name = value

    @property
    def prodsysDescription(self):
        return {}
        
    ## @brief String conversion of our value
    def __str__(self):
        return '{0}: Value {1} (isRunArg={2})'.format(self.__class__.__name__, self._value, self._runarg)
    
    ## @brief Repr conversion of our value
    def __repr__(self):
        return repr(self.value)
    
    ## Comparison is based on value attribute
    def __cmp__(self, other):
        if self.value == other.value:
            return 0
        if self.value > other.value:
            return 1
        return -1


## @brief String type argument
class argString(argument):

    ## @brief Class initialisation
    #  @param value Initial value to be set
    #  @param runarg Boolean specifiying if this is a run time argument 
    #  (specifically passed to athena by the AthenaExecutor, other executors may do the same).
    #  @param name Name for this argument 
    #  @param choices Optional list of strings from which value must be set
    def __init__(self, value = None, runarg = True, name = None, choices = None):
        self._choices = choices
        super(argString, self).__init__(value = value, runarg = runarg, name=name)

    ## @brief Argument value getter
    #  @returns Current value
    @property
    def value(self):
        return self._value

    ## @brief Argument value setter
    #  @details Sets value directly if it's a @c str, otherwise call the @c str() converter
    @value.setter
    def value(self, value):
        if value == None:
            # For strings, None maps to ''
            self._value = ''
        else:
            # Call string converter - should work for everything...
            self._value = str(value)
        if self._choices:
            if self._value not in self._choices:
                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CHOICES_FAIL'), 'Converted argument {0} for {1} did not match any valid choice: {2}'.format(value, self._name, self._choices))

    ## @brief Choices getter
    #  @returns Valid choices list
    def choices(self):
        return self._choices

    # prodsysDescription: human readable from of type plus possible values
    @property
    def prodsysDescription(self):
        self._desc = {'type' : 'string'}
        if self._choices:
            self._desc['choices'] = self._choices
        return self._desc
    
    ## @note String value can be printed directly
    def __str__(self):
        return self.value


## @brief Int type argument
class argInt(argument):
    
    ## @brief Argument value getter
    #  @returns Current value
    @property
    def value(self):
        return self._value

    ## @brief Argument value setter
    #  @details Sets value directly if it's an @c int, otherwise call the @c int() converter.
    #  @throws trfExceptions.TransformArgException if @c int() conversion fails 
    @value.setter
    def value(self, value):
        if value == None:
            # For ints None maps to 0
            self._value = 0
        else:
            if isinstance(value, int):
                self._value = value
            else:
                ## We try hard to convert the value we were given - anything @c int() swallows we accept
                try:
                    self._value = int(value)
                except ValueError, e:
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert value {0} to int'.format(value))

    # prodsysDescription: human readable from of type plus possible values
    @property
    def prodsysDescription(self):
        self._desc = {'type' : 'INT'}
        return self._desc

                

## @brief Float type argument
class argFloat(argument):
    
    ## @brief Float argument constructor
    # @param value Initial value to set, default None,
    # @param min Minimum value for any limits that need to be set
    # @param max Maximum value for any limits that need to be set
    def __init__(self, value=None, min=None, max=None, runarg=True, name=None):
        self._min = min
        self._max = max
        self._desc = {}
        super(argFloat, self).__init__(value = value, runarg = runarg, name=name)

    ## @brief Argument value getter
    #  @returns Current value
    @property
    def value(self):
        return self._value

    @property
    def prodsysDescription(self):
        self._desc = {'type' : 'float'}
        if self._min:
            self._desc['min'] = self._min
        if self._max:
            self._desc['max'] = self._max
        return self._desc
    
    ## @brief Setter function.
    #  @details The argument can be anything, if it is not of type @c float, it will attempt to convert using @c float().
    #  @param value Value to set.
    #  @throw trfExceptions.TransformArgException Exception thrown if the float conversion fails.
    #  @throw trfExceptions.TransformArgException Exception thrown if value is outside of the given range.
    @value.setter
    def value(self, value=None):
        # Default value will be 0.0 or self._min (if defined)
        if value == None:
            if self._min is not None:
                self._value = self._min
            else:
                self._value = 0.0
        else:
            try:
                if isinstance(value, float):
                    self._value = value
                else:
                    self._value = float(value)
            except ValueError:
                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'),
                                                          'Failed to convert %s to a float' % str(value))
            
            if (self._min != None and self.value < self._min) or (self._max != None and self._value > self._max):
                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_OUT_OF_RANGE'),
                                                          'argFloat value out of range: %g is not between %s and %s' % 
                                                          (self.value, self._min, self._max))


## @brief Boolean type argument
class argBool(argument):
    
    ## @brief Argument value getter
    #  @returns Current value
    @property
    def value(self):
        return self._value

    ## @brief Value setter function.
    #  @details The argument can be a @c bool or a string with value @c true or @c false (case independent).
    #  @param value Value to set.
    #  @throw trfExceptions.TransformArgException Exception thrown if the float conversion fails.
    #  @throw trfExceptions.TransformArgException Exception thrown if value is outside of the given range.
    @value.setter
    def value(self, value):
        # Default value matches the python bool() constructor
        if value == None:
            self._value = False
        else:
            if isinstance(value, bool):
                self._value = value
            else:
                self._value = strToBool(value)

    # prodsysDescription: human readable from of type plus possible values
    @property
    def prodsysDescription(self):
        self._desc = {'type' : 'bool'}
        return self._desc

## @brief List of string arguments
class argList(argument):
    
    ## @brief List of string arguments
    #  @details Argument holding a list of string values
    #  @param splitter The splitter argument determines how the string is split (default by a comma)
    #  @param value Initial value to set (note default is an empty list [])
    #  @param supressEmptyStrings If empty strings are removed from the list
    #  (Can be used to easily set an empty list from the CLI)
    def __init__(self, value = [], supressEmptyStrings = True, splitter=',', runarg=True, name=None):
        self._splitter = splitter
        self._supressEmptyStrings = supressEmptyStrings

        super(argList, self).__init__(value = value, runarg = runarg, name=name)

    ## @brief Argument value getter
    #  @returns Current value
    @property
    def value(self):
        return self._value

    # prodsysDescription: human readable from of type plus possible values
    @property
    def prodsysDescription(self):
        self._desc = {'type' : 'list'}
        if self._supressEmptyStrings:
            self._desc['supress Empty Strings'] = self._supressEmptyStrings
        return self._desc


    ## @brief Argument value setter
    #  @detail If we get a list then set that, otherwise we split a string on the splitter character
    @value.setter
    def value(self, value):
        if isinstance(value, (list, tuple)):
            self._value = list(value)
        elif value==None:
            self._value = []
            return
        else:
            try:
                if self._supressEmptyStrings:
                    self._value = [ v for v in value.split(self._splitter) if v is not '' ]
                else:
                    self._value = value.split(self._splitter)
            except AttributeError:
                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'),
                                                          'Failed to convert %s to a list' % str(value))

    ## @brief Append a value to the list
    #  @param addme Item to add                
    def append(self, addme):
        self._value.append(addme)
    
    ## @brief String conversion
    #  @details Flatten values and space separate
    #  @warning Values sets with spaces will be confused by this conversion
    def __str__(self):
        return " ".join(self._value)
    
    ## @brief Repr conversion
    #  @details Return a python parsable string
    def __repr__(self):
        return '[' + ','.join([ repr(s) for s in self._value ]) + ']'


## @brief List of int arguments
class argIntList(argList):
    ## @brief Argument value getter
    #  @returns Current value
    @property
    def value(self):
        return self._value
    

    ## @brief Argument value setter
    #  @detail If we get a list then set that, otherwise we split a string on the splitter character
    #  @throw trfExceptions.TransformArgException Exception thrown if any list member is not an @c int
    #  @throw trfExceptions.TransformArgException Exception thrown is any @c int() conversion fails
    @value.setter
    def value(self, value):
        if isinstance(value, list):
            for v in value:
                if not isinstance(v, (int, long)):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_ERROR'),
                                                              'Illegal argument %s in list of ints' % str(el))
            self._value = value
        elif value==None:
            self._value = []
            return
        else:
            try:
                if self._supressEmptyStrings:
                    self._value = [ v for v in value.split(self._splitter) if v is not '' ]
                else:
                    self._value = value.split(self._splitter)
                self._value = [ int(el) for el in self._value ]
            except (AttributeError, ValueError):
                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'),
                                                          'Failed to convert %s to a list of ints' % str(value))
                    
    def __str__(self):
        return " ".join([ str(el) for el in self._value ])

    @property
    def prodsysDescription(self):
        self._desc = {'type' : 'list', 'subtype' : 'INT'}
        return self._desc
                    
        
# Special list which stores k:v pairs, where the value is an float (used for AthenaMP merge target size)
class argKeyFloatValueList(argList):
    ## @brief Dictionary of key value arguments, where the values are floats
    #  @param splitter The splitter argument determines how the string is split (default by a comma)
    #  @param kvsplitter The kvsplitter argument determines how the key and value are split (default by a colon)
    #  @param value Initial value to set (note default is an empty dictionary {})
    #  @param supressEmptyStrings If empty strings are removed from the list
    #  (Can be used to easily set an empty list from the CLI)
    def __init__(self, value = {}, supressEmptyStrings = True, splitter=',', kvsplitter=":", runarg=True, name=None):
        self._splitter = splitter
        self._kvsplitter = kvsplitter
        self._supressEmptyStrings = supressEmptyStrings

        super(argList, self).__init__(value = value, runarg = runarg, name=name)
    
    ## @brief Argument value getter
    #  @returns Current value
    @property
    def value(self):
        return self._value

    ## @brief Argument value setter
    #  @detail If we get a dict then set that, otherwise we split a string on the splitter character
    #  and then on the kvsplitter into key and value, with the value being converted to float
    #  @throw trfExceptions.TransformArgException Exception thrown if any dictionary @c key:value member is not @c string:int
    #  @throw trfExceptions.TransformArgException Exception thrown if any @c int() conversion fails of the kvsplitter is not found
    @value.setter
    def value(self, value):
        if isinstance(value, dict):
            for k, v in value.iteritems():
                if not isinstance(k, str):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_ERROR'),
                                                              'Illegal key argument type {0} in dictionary for argKeyFloatValueList'.format(k))
                if not isinstance(v, float):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_ERROR'),
                                                              'Illegal value argument type {0} in dictionary for argKeyFloatValueList'.format(v))
            self._value = value
        elif value==None:
            self._value = {}
            return
        else:
            self._value = {}
            try:
                if self._supressEmptyStrings:
                    kvlist = [ v for v in value.split(self._splitter) if v is not '' ]
                else:
                    kvlist = value.split(self._splitter)
                for item in kvlist:
                    k, v = item.split(self._kvsplitter, 1)
                    self._value[k] = float(v)
            except (AttributeError, ValueError):
                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'),
                                                          'Failed to convert {0} to a dictionary of string:int values'.format(value))

    def __str__(self):
        return str(self._value)

    @property
    def prodsysDescription(self):
        self._desc = {'type' : 'list', 'subtype' : 'key:floatValue'}
        return self._desc
        

## @brief File argument class
#  @details Inherits from argList
class argFile(argList):    

    ## @brief Initialise an argFile
    #  @param io @c input, @c output or @c temporary file, default @c output.
    #  @param splitter changes character a string list is split on, default is a comma (see argList).
    #  @param type Datatype in this instance - this should be the @b major datatype (ESD, AOD, etc).
    #  @param subtype The data subtype, which should match the DATATYPE portion of the corresponding
    #  argument name, e.g., outputDESD_SGLMUFile -> DESD_SGLMU 
    #  @param guid This is a non-standard option and allows the GUID for files without an intrinsic GUID
    #  to be set explicitly at initialisation. The parameter should be a dictionary, keyed by filename,
    #  which contains the GUID string, e.g., <tt>{'file1' : '930de3de-de8d-4819-9129-beef3bb4fadb', 'file2' : ... }</tt>
    #  @param multipleOK Explicit declaration of whether multiple arguments are allowed; default is @c True for @c input,
    #  @c False for @c output and @c temporary
    #  @param name The corresponding key for this argument in the argdict of the transform (e.g., @c inputESDFile)
    #  @param executor List of execution substeps where this file type should be added explicitly (e.g., minbias HITS
    #  for digitisation)
    #  @param mergeTargetSize Target merge size if this instance supports a selfMerge method. Value is in bytes, with the
    #  special values: @c -1 Always merge to a single file, @c 0 never merge these files 
    #  @note When used in argument parser, set nargs='+' to get auto-concatenation of multiple arguments (should be used
    #  when @c multipleOK is @c True)
    def __init__(self, value=list(), type=None, subtype=None, io = 'output', splitter=',', runarg = True, guid = None, 
                 multipleOK = None, name=None, executor=list(), mergeTargetSize=-1):
        # Set these values before invoking super().__init__ to make sure they can be
        # accessed in our setter 
        self._dataset = None
        self._urlType = None
        self._type = type
        self._subtype = subtype
        self._guid = guid
        self._mergeTargetSize = mergeTargetSize
        
        # User setter to get valid value check
        self.io = io

        self._exe = executor

        ## @note Variable listing set of file metadata which corresponds to this class,
        #  Key is the metadata variable name, the value is the function to call to
        #  populate/refresh this metadata value. Function must take a single parameter,
        #  which is the list of files to get metadata for. It must return a metadata
        #  dictionary: {file1 : {key1: value1, key2: value2}, file2: ...}
        #  Keys which start with _ are for transform internal use and should not appear in
        #  jobReports

        self._metadataKeys = {'file_size': self._getSize,
                              'integrity': self._getIntegrity,
                              'file_guid': self._generateGUID,
                              '_exists': self._exists,
                              }

        if multipleOK is None:
            if self._io is 'input':
                self._multipleOK = True
            else:
                self._multipleOK = False
        else:
            self._multipleOK = multipleOK
            
        ## @note N.B. argList.__init__ calls _our_ setter for us
        super(argFile, self).__init__(value=value, splitter=splitter, runarg=runarg, name=name)
    

    ## @brief Argument value getter
    #  @returns Current value
    @property
    def value(self):
        return self._value

    ## @brief Argument value setter
    #  @detail Calls the valueSetter function with the standard options
    @value.setter
    def value(self, value):
        self.valueSetter(value)
        
    ## @brief multipleOK getter
    #  @returns Current value
    @property
    def multipleOK(self):
        return self._multipleOK
    
    ## @brief multipleOK value setter
    @multipleOK.setter
    def multipleOK(self, value):
        self._multipleOK = value
        
    ## @brief mergeTargeSize value getter
    @property
    def mergeTargetSize(self):
        return self._mergeTargetSize

    ## @brief mergeTargeSize value setter
    @mergeTargetSize.setter
    def mergeTargetSize(self, value):
        if value==None:
            self._mergeTargetSize = 0
        else:
            self._mergeTargetSize = value

    @property
    def prodsysDescription(self):
        if type(self._type) is types.DictType:
            if self._type=={}:
               self._desc = {'type' : 'file', 'subtype' : "NONE" }
            else:
                self._desc = {'type' : 'file', 'subtype' : dict((str(k).upper(), str(v).upper()) for (k,v) in self._type.iteritems())}
        else:
            self._desc = {'type' : 'file', 'subtype' : str(self._type).upper()}
        self._desc['multiple'] = self._multipleOK
        return self._desc 
    
    ## @brief Executor status getter
    @property
    def executor(self):
        return self._exe
        
    ## @brief Set the argFile value, but allow parameters here
    #  @note Normally athena only takes a single value for an output file, but when AthenaMP runs
    #  it can produce multiple output files - this is allowed by setting  <tt>allowMultiOutputs = False</tt>
    #  @note The setter protects against the same file being added multiple times
    def valueSetter(self, value):
        ## @note Impossible to use the argList.value setter here? super() doesn't seem to get it right:
        #  <tt>super(argFile, self).value = value</tt> results in an attribute error
        
        prodSysPattern = re.compile(r'(?P<prefix>.*)\[(?P<expand>[\d\.,_]+)\](?P<suffix>.*)')
       
        ## @note First do parsing of string vs. lists to get list of files
        if isinstance(value, (list, tuple)):
            self._value = list(value)
        elif value==None:
            self._value = []
            return
        else:
            try:
                # If there is a prodsys glob in the game we turn off splitting
                prodsysGlob = prodSysPattern.match(value)
                if prodsysGlob and self._splitter is ',':
                    msg.debug('Detected prodsys glob - normal splitting is disabled')
                    self._value = [value]
                else:
                    self._value = value.split(self._splitter)
            except (AttributeError, TypeError):
                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'),
                                                          'Failed to convert %s to a list' % str(value))
                
        ## @note Check for duplicates (N.B. preserve the order, just remove the duplicates)
        deDuplicatedValue = []
        for file in self._value:
            if file not in deDuplicatedValue:
                deDuplicatedValue.append(file)
            else:
                msg.warning("Removing duplicated file {0} from file list".format(file))
        if len(self._value) != len(deDuplicatedValue):
            self._value = deDuplicatedValue
            msg.warning('File list after duplicate removal: {0}'.format(self._value))
        
        ## @note Now look for dataset notation
        # TODO - handle reset of filenames from AthenaMP without trashing DS name
        self._getDatasetFromFilename(reset = True)
        
        # Find our URL type (if we actually have files!)
        # At the moment this is assumed to be the same for all files in this instance
        # although in principle one could mix different access methods in the one input file type
        if len(self._value) > 0:
            self._urlType = urlType(self._value[0])
        else:
            self._urlType = None

        ## @brief Input file globbing and expansion
        if self._io == 'input':
            ## @note TODO: Non-posix URLs
            #  Problem is not so much the [] expansion, but the invisible .N attempt number
            #  One can only deal with this with a listdir() functionality
            #  N.B. Current transforms only do globbing on posix fs too (see trfutil.expandStringToList())  
            if self._urlType is 'posix':
                msg.debug('Found POSIX filesystem input - activating globbing')
                newValue = []
                for filename in self._value:
                    ## @note Weird prodsys style globbing...
                    #  This has the format:
                    #   @c prefix._[NNN,MMM,OOO,PPP].suffix  (@c NNN, etc. are numbers)
                    #  However an invisible .N attempt number also needs to be appended before doing real globbing
                    prodsysGlob = prodSysPattern.match(filename)
                    if prodsysGlob:
                        msg.debug('Detected [MMM,NNN,OOO] style prodsys globbing for {0}'.format(filename))
                        msg.debug('Prefix: {0}; Numerical expansion: {1}; Suffix: {2}'.format(prodsysGlob.group('prefix'), prodsysGlob.group('expand'), prodsysGlob.group('suffix')))
                        numbers = prodsysGlob.group('expand').split(',')
                        for number in numbers:
                            # Add a final '.*' to match against the .AttemptNumber invisible extension
                            globName = prodsysGlob.group('prefix') + str(number) + prodsysGlob.group('suffix') + '*'
                            msg.debug('Will try globbing against {0}'.format(globName))
                            globbedNames = glob.glob(globName)
                            if len(globbedNames) > 1:
                                msg.warning('Warning - matched multiple filenames ({0}) when adding the .AttemptNumber to {1}'.format(globbedNames, globName))
                            elif len(globbedNames) == 0:
                                msg.warning('Warning - matched NO filenames when adding the .AttemptNumber to {0}'.format(globName))
                            newValue.extend(globbedNames)
                    else:
                        # Simple case
                        newValue.extend(glob.glob(filename))
                if len(self._value) > 0 and len(newValue) is 0:
                    # Woops - no files!
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_INPUT_FILE_ERROR'), 
                                                              'Input file argument(s) {0!s} globbed to NO input files - probably the file(s) are missing'.format(self._value))
                self._value = newValue
                msg.debug ('File input is globbed to %s' % self._value)

            elif self._urlType is 'root':
                msg.debug('Found root filesystem input - activating globbing')
                newValue = []
                for filename in self._value:

                    ## @note Weird prodsys style globbing...
                    #  This has the format:
                    #   @c prefix._[NNN,MMM,OOO,PPP].suffix  (@c NNN, etc. are numbers)
                    #  However an invisible .N attempt number also needs to be appended before doing real globbing
                    prodsysGlob = prodSysPattern.match(filename)
                    if prodsysGlob:
                        theNameList = [filename]
                        i = 0
                        msg.debug('Try to split input string if more than one file is given')
                        if ',root:' in filename:
                            theNameList = filename.split(',root:')
                            for name in theNameList:
                                if not name.startswith('root:'):
                                    name = 'root:'+name
                                    theNameList[i] = name
                                i = i + 1 
                                
                            msg.debug('Split input string into files: {0}'.format(theNameList))
                        for fileName in theNameList:
                            prodsysGlob = prodSysPattern.match(fileName)
                            msg.debug('Detected [MMM,NNN,OOO] style prodsys globbing for {0}'.format(fileName))
                            msg.debug('Prefix: {0}; Numerical expansion: {1}; Suffix: {2}'.format(prodsysGlob.group('prefix'), prodsysGlob.group('expand'), prodsysGlob.group('suffix')))
                            numbers = prodsysGlob.group('expand').split(',')
                            for number in numbers:
                                # Add a final '.*' to match against the .AttemptNumber invisible extension
                                globName = prodsysGlob.group('prefix') + str(number) + prodsysGlob.group('suffix') 
                                msg.debug('Will try globbing against {0}'.format(globName))
                                globbedNames =[globName]# glob.glob(globName)
                                if len(globbedNames) > 1:
                                    msg.warning('Warning - matched multiple filenames ({0}) when adding the .AttemptNumber to {1}'.format(globbedNames, globName))
                                elif len(globbedNames) == 0:
                                    msg.warning('Warning - matched NO filenames when adding the .AttemptNumber to {0}'.format(globName))
                                newValue.extend(globbedNames)

                    else:
                        # Simple case
                        if not(str(filename).endswith('/')) and '*' not in filename and '?' not in filename:
                            msg.debug('Seems that only one file was given: {0}'.format(filename))
                            newValue.extend(([filename]))
                        else:
                           #hopefully this recognised wildcards...
                            path = filename
                            fileMask = ''
                            if '*' in filename or '?' in filename:
                                msg.debug('Split input into path for listdir() and a filemask to select available files.')    
                                path = filename[0:filename.rfind('/')+1]
                                msg.debug('path: {0}'.format(path))
                                fileMask = filename[filename.rfind('/')+1:len(filename)]
                                msg.debug('Will select according to: {0}'.format(fileMask))

                            msg.debug('eos command is hard coded - check if it is executable')
                            cmd = ['/afs/cern.ch/project/eos/installation/atlas/bin/eos.select' ]
                            if not os.access ('/afs/cern.ch/project/eos/installation/atlas/bin/eos.select', os.X_OK ):
                                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_INPUT_FILE_ERROR'),
                                'Input file argument(s) {0!s} globbed to NO input files - The eos ls command could not be executed.', cmd)

                            cmd.extend(['ls'])
                            cmd.extend([path])

                            myFiles = []
                            try:
                                proc = subprocess.Popen(args = cmd,bufsize = 1, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
                                while proc.poll() is None:
                                    line = proc.stdout.readline()
                                    if line:
                                        if "root" in line:
                                            myFiles += [str(path)+str(line.rstrip('\n'))]

                                rc=proc.returncode


                                if not rc==0:
                                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_INPUT_FILE_ERROR'),
                                                                'Input file argument(s) {0!s} globbed to NO input files - ls command failed')
                                msg.debug('Executed eos ls, found:')
                                patt = re.compile(fileMask.replace('*','.*').replace('?','.'))
                                for srmFile in myFiles:
                                    if fileMask is not '':
                                        if(patt.search(srmFile)) is not None:
                                        #if fnmatch.fnmatch(srmFile, fileMask):
                                            msg.debug('match: ',srmFile)
                                            newValue.extend(([srmFile]))



                                    else:
                                        newValue.extend(([srmFile]))
                                
                                msg.debug('Selected files: ', newValue)
                            except (AttributeError, TypeError, OSError):
                                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_RUNTIME_ERROR'),
                                                             'Failed to convert %s to a list' % str(value))
                if len(self._value) > 0 and len(newValue) is 0:
                    # Woops - no files!
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_INPUT_FILE_ERROR'),
                                                              'Input file argument(s) {0!s} globbed to NO input files - ls command failed')
                self._value = newValue
                msg.debug ('File input is globbed to %s' % self._value)
        # Check if multiple outputs are ok for this object
        elif self._multipleOK == False and len(self._value) > 1:
            raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'), 
                                                      'Multiple file arguments are not supported for {0} (was given: {1}'.format(self, self._value))
            
        # Reset the self._fileMetadata dictionary
        self._resetMetadata()

    @property
    def io(self):
        return (self._io)

    @io.setter
    def io(self, value):
        if value not in ('input', 'output', 'temporary'):
            raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_RUNTIME_ERROR'), 
                                                      'File arguments must be specified as input, output or temporary - got {0}'.format(value))
        self._io = value
    
    @property
    def dataset(self):
        return self._dataset
    
    @property
    def type(self):
        return self._type
    
    @type.setter
    def type(self, value):
        self._type = value

    @property
    def subtype(self):
        return self._subtype
    
    @subtype.setter
    def subtype(self, value):
        self._subtype = value

    ## @brief Name getter
    @property
    def name(self):
        return self._name
    
    ## @brief Name setter
    #  @note This property setter will also set the type and subtype of the argFile
    #  if they are not yet set. This means that for most arguments the type and subtype
    #  are automatically set correctly.
    @name.setter
    def name(self, value):
        self._name = value
        m = re.match(r'(input|output|tmp.)([A-Za-z0-9_]+?)(File)?$', value)
        if m:
            msg.debug("ArgFile name setter matched this: {0}".format(m.groups()))
            if self._type is None:
                dtype = m.group(2).split('_', 1)[0]
                # But DRAW/DESD/DAOD are really just RAW, ESD, AOD in  format
                if re.match(r'D(RAW|ESD|AOD)', dtype):
                    dtype = dtype[1:]
                msg.debug("Autoset data type to {0}".format(dtype))
                self._type = dtype
            if self._subtype is None:
                msg.debug("Autoset data subtype to {0}".format(m.group(2)))
                self._subtype = m.group(2)
        else:
            msg.debug("ArgFile name setter did not match against '{0}'".format(value))

    
    ## @brief Returns the whole kit and kaboodle...
    #  @note Populates the whole metadata dictionary for this instance
    @property
    def metadata(self):
        self.getMetadata()
        return self._fileMetadata
    
    ## @brief Return total number of events in all constituent files
    @property
    def nentries(self):
        return self.getnentries()
        
    ## @brief Explicit getter, offering fast switch
    def getnentries(self, fast=False):
        totalEvents = 0
        for file in self._value:
            events = self.getSingleMetadata(file, 'nentries', populate = not fast)
            if events is None:
                msg.debug('Got events=None for file {0} - returning None for this instance'.format(file))
                return None
            if events is 'UNDEFINED':
                msg.debug('Got events=UNDEFINED for file {0} - returning UNDEFINED for this instance'.format(file))
                return 'UNDEFINED'
            if not isinstance(events, (int, long)):
                msg.warning('Got unexpected events metadata for file {0}: {1!s} - returning None for this instance'.format(file, events))
                return None
            totalEvents += events
        
        return totalEvents
                
    
    ## @brief Resets all metadata files in this instance
    #  @details Metadata dictionary entry is reset for any files given (default all files)
    #  and any files that are no longer in this instance have any metadata removed
    #  (useful for self merging). 
    #  @note Metadata is set to @c {}, except for the case when an explicit GUID option was given
    def _resetMetadata(self, files=[]):
        if files == [] or '_fileMetadata' not in dir(self):
            self._fileMetadata = {}
            for file in self.value:
                self._fileMetadata[file] = {}
        else:
            for file in files:
                if file in self.value:
                    self._fileMetadata[file] = {}
                elif file in self._fileMetadata:
                    del self._fileMetadata[file]
        msg.debug('Metadata dictionary now {0}'.format(self._fileMetadata))
        
        # If we have the special guid option, then manually try to set GUIDs we find
        if self._guid is not None:
            msg.debug('Now trying to set file GUID metadata using {0}'.format(self._guid))
            for file, guid in self._guid.iteritems():
                if file in self._value:
                    self._fileMetadata[file]['file_guid'] = guid
                else:
                    msg.warning('Explicit GUID {0} was passed for file {1}, but this file is not a member of this instance'.format(guid, file))
    
    ## @brief Return specific keys for specific files
    #  @param files List of files to return metadata for (default - all files in this instance)
    #  @param metadataKeys Keys to return (default - all keys valid for this class of files)
    #  @param maskMetadataKeys Keys to NOT return (useful when metadataKeys is left as default)
    #  @param populate If missing keys should be generated by calling the population subroutines
    #  @param flush If cached data should be flushed and the generators rerun 
    def getMetadata(self, files = None, metadataKeys = None, maskMetadataKeys = None, populate = True, flush = False):
        # Normalise the files and keys parameter
        if files is None:
            files = self._value
        elif isinstance(files, str):
            files = (files,)
        msg.debug('getMetadata will examine these files: {0!s}'.format(files))

        if metadataKeys is None:
            metadataKeys = self._metadataKeys.keys()
        elif isinstance(metadataKeys, str):
            metadataKeys = (metadataKeys,)
        if maskMetadataKeys is not None:
            for key in maskMetadataKeys:
                if key in metadataKeys:
                    metadataKeys.remove(key)
        msg.debug('getMetadata will retrieve these keys: {0!s}'.format(metadataKeys))
        
        if flush is True:
            msg.debug('Flushing cached metadata values')
            self._resetMetadata()
            
        if populate is True:
            msg.debug('Checking metadata values')
            self._readMetadata(files, metadataKeys)

        metadata = {}
        for file in files:
            metadata[file] = {}
            for mdkey in metadataKeys:
                try:
                    metadata[file][mdkey] = self._fileMetadata[file][mdkey]
                except KeyError:
                    # This should not happen, unless we skipped populating
                    if populate:
                        msg.error('Did not find metadata key {0!s} for file {1!s} - setting to None'.format(mdkey, file))
                    metadata[file][mdkey] = None
        return metadata
    
    ## @brief Convenience function to extract a single metadata key for a single file
    #  @details Retrieves a single metadata item for a single file, returning it directly
    #  @return Single metadata value
    #  @param files Files to return metadata for
    #  @param metadataKey Keys to return
    #  @param populate If missing key should be generated by calling the population subroutines
    #  @param flush If cached data should be flushed and the generator rerun 
    def getSingleMetadata(self, file, metadataKey, populate = True, flush = False):
        if not (isinstance(file, str) and isinstance(metadataKey, str)):
            raise trfExceptions.TransformInternalException(trfExit.nameToCode('TRF_INTERNAL'), 
                                                           'Illegal call to getSingleMetadata function: {0!s} {1!s}'.format(file, metadataKey))
        md = self.getMetadata(files = file, metadataKeys = metadataKey, populate = populate, flush = flush)
        return md[file][metadataKey]
    
    
    ## @brief Check metadata is in the cache or generate it if it's missing
    #  @details 
    #  @return: dictionary of files with metadata, for any unknown keys 'UNDEFINED' is returned
    def _readMetadata(self, files, metadataKeys):
        msg.debug('Retrieving metadata keys {1!s} for files {0!s}'.format(files, metadataKeys))
        for file in files:
            if file not in self._fileMetadata:
                self._fileMetadata[file] = {}
        for file in files:
            # Always try for a simple existence test first before producing misleading error messages
            # from metadata populator functions
            if '_exists' not in self._fileMetadata[file]:
                self._metadataKeys['_exists'](files)
            if self._fileMetadata[file]['_exists'] is False:
                # N.B. A log ERROR message has printed by the existence test, so do not repeat that news here
                for key in metadataKeys:
                    if key is not '_exists':
                        self._fileMetadata[file][key] = None
            else:
                # OK, file seems to exist at least... 
                for key in metadataKeys:
                    if key not in self._metadataKeys:
                        msg.debug('Metadata key {0} is unknown for {1}'.format(key, self.__class__.__name__))
                        self._fileMetadata[file][key] = 'UNDEFINED'
                    else:
                        if key in self._fileMetadata[file]:
                            msg.debug('Found cached value for {0}:{1} = {2!s}'.format(file, key, self._fileMetadata[file][key]))
                        else:
                            msg.debug('No cached value for {0}:{1}. Calling generator function {2} ({3})'.format(file, key, self._metadataKeys[key].func_name, self._metadataKeys[key]))
                            try:
                                # For efficiency call this routine with all files we have
                                self._metadataKeys[key](files)
                            except trfExceptions.TransformMetadataException, e:
                                msg.error('Calling {0!s} raised an exception: {1!s}'.format(self._metadataKeys[key].func_name, e))
                            if key not in self._fileMetadata[file]:
                                msg.warning('Call to function {0} for {1} file {2} failed to populate metadata key {3}'.format(self._metadataKeys[key].__name__, self.__class__.__name__, file, key))
                                self._fileMetadata[file][key] = None
                            msg.debug('Now have {0}:{1} = {2!s}'.format(file, key, self._fileMetadata[file][key]))
    
    
    ## @brief Set metadata values into the cache
    #  @detailed Manually sets the metadata cache values to the values given in the
    #  metadata key dictionary here.
    #  This is useful for setting values to make checks on file metadata handling.
    #  @note To really suppress any external function calls that gather metadata be careful
    #  to also set the @c _exists metadatum to @c True.
    #  @warning No checks are done on the values or keys given here, so you'd better
    #  know what you are doing.
    #  @param files Files to set metadata for (@c None means "all")
    #  @param metadataKeys Dictionary with metadata keys and values
    def _setMetadata(self, files=None, metadataKeys={}):
        if files == None:
            files = self._value
        for file in files:
            for k, v in metadataKeys.iteritems():
                msg.debug('Manualy setting %s for file %s to %s'.format(k, file, v))
                self._fileMetadata[file][k] = v
    
    
    ## @brief Test if certain metadata elements are already cached
    #  @detailed Will test for a cached value for all files and all keys
    #  given, aborting as soon as it finds a single uncached value.
    #  @param files Files to check (defaults to all files)
    #  @param metadataKeys Keys to check (defaults to all keys)
    #  @return Boolean if all keys are cached for all files
    def isCached(self, files = None, metadataKeys = None):
        msg.debug('Testing for cached values for files {0} and keys {1}'.format(files, metadataKeys))
        if files is None:
            files = self._value
        elif isinstance(files, str):
            files = (files,)
        if metadataKeys is None:
            metadataKeys = self._metadataKeys.keys()
        elif isinstance(metadataKeys, str):
            metadataKeys = (metadataKeys,)
            
        isCachedFlag = True
        for file in files:
            for key in metadataKeys:
                if key not in self._fileMetadata[file]:
                    isCachedFlag = False
                    break
            if isCachedFlag == False:
                break
            
        return isCachedFlag
    
    ## @brief Look for dataset name in dataset#filename Tier0 convention
    #  @detail At the moment all files must be in the same dataset if it's specified. 
    #          To change this dataset will need to become a per-file metadatum.
    #  @param @c reset If @c True then forget previous dataset setting. Default is @c True.
    #  @return @c None. Side effect is to set @c self._metadata.
    def _getDatasetFromFilename(self, reset = True):
        if reset:
            self._dataset = None
        newValue = []
        for filename in self._value:
            if filename.find('#') > -1:
                (dataset, file) = filename.split('#', 1)
                newValue.append(file)
                msg.debug('Current dataset: {0}; New dataset {1}'.format(self._dataset, dataset))
                if (self._dataset is not None) and (self._dataset != dataset):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_DATASET'), 
                                                              'Found inconsistent dataset assignment in argFile setup: %s != %s' % (self._dataset, dataset))
                self._dataset = dataset
            else:
                newValue.append(filename)
        self._value = newValue


    def __str__(self):
        return "%s (Dataset %s, IO %s)" % (self.value, self.dataset, self.io)

    
    ## @brief Determines the size of files.
    #  @details Currently only for statable files (posix fs). Caches the
    #  @param files List of paths to the files for which the size is determined.
    #  @return None (internal @c self._fileMetadata cache is updated)
    def _getSize(self, files):
        for file in files:
            if self._urlType is 'posix':
                try:
                    self._fileMetadata[file]['size'] = os.stat(file).st_size
                except (IOError, OSError) as e:
                    msg.error('Got exception {0!s} raised while stating file {1}'.format(e, file))
                    self._fileMetadata[file]['size'] = None
            else:
                # OK, let's see if ROOT can do it...
                msg.debug('Calling ROOT TFile.GetSize({0})'.format(file))
                self._fileMetadata[file]['size'] = ROOTGetSize(file)


    ## @brief File integrity checker
    #  @details For a 'plain' file, integrity just checks that we can read it
    #  @param @c files List of paths to the files for which the integrity is determined
    #  @return None (internal @c self._fileMetadata cache is updated)
    def _getIntegrity(self, files):
        for file in files:
            with open(file) as f: 
                try:
                    while True:
                        chunk = len(f.read(1024*1024))
                        msg.debug('Read {0} bytes from {1}'.format(chunk, file)) 
                        if chunk == 0:
                            break
                    self._fileMetadata[file]['integrity'] = True
                except (OSError, IOError) as e:
                    msg.error('Got exception {0!s} raised while checking integrity of file {1}'.format(e, file))
                    self._fileMetadata[file]['integrity'] = False
                    
                    
    ## @brief Generate a GUID on demand - no intrinsic for this file type        
    #  @details Use uuid.uuid4() call to generate a GUID
    #  @note This generation method will be superceeded in any file type which 
    #  actually has an intrinsic GUID (e.g. BS or POOL files)
    def _generateGUID(self, files):
        for file in files:
            msg.debug('Generating a GUID for file {0}'.format(file))
            self._fileMetadata[file]['file_guid'] = str(uuid.uuid4()).upper()
            
            
    ## @brief Try to determine if a file actually exists...
    #  @details For a posix file, just call stat; for anything else call TFile.Open
    #  A small optimisation is to retieve the file_size metadatum at the same time.
    #  @param @c files List of paths to test for existance
    #  @return None (internal @c self._fileMetadata cache is updated)
    def _exists(self, files):
        msg.debug('Testing existance for {0}'.format(files))
        for file in files:
            if self._urlType is 'posix':
                try:
                    size = os.stat(file).st_size
                    self._fileMetadata[file]['file_size'] = size
                    self._fileMetadata[file]['_exists'] = True
                    msg.debug('POSIX file {0} exists'.format(file))
                except (IOError, OSError) as e:
                    msg.error('Got exception {0!s} raised while stating file {1}  - probably it does not exist'.format(e, file))
                    self._fileMetadata[file]['_exists'] = False
            else:
                # OK, let's see if ROOT can do it...
                msg.debug('Calling ROOT TFile.GetSize({0})'.format(file))
                size = ROOTGetSize(file)
                if size is None:
                    self._fileMetadata[file]['_exists'] = False
                    msg.error('Non-POSIX file {0} could not be opened - probably it does not exist'.format(file))
                else:
                    msg.debug('Non-POSIX file {0} exists'.format(file))
                    self._fileMetadata[file]['file_size'] = size
                    self._fileMetadata[file]['_exists'] = True

    ## @brief String representation of a file argument
    def __str__(self):
        return "{0}={1} (Type {2}, Dataset {3}, IO {4})".format(self.name, self.value, self.type, self.dataset, self.io)

## @brief Athena file class
#  @details Never used directly, but is the parent of concrete classes
class argAthenaFile(argFile):
    def __init__(self, value = list(), type=None, subtype=None, io = 'output', splitter=',', runarg=True, multipleOK = None, 
                 name=None, executor=list(), mergeTargetSize=-1):
        super(argAthenaFile, self).__init__(value=value, subtype=subtype, io=io, type=type, splitter=splitter, runarg=runarg, 
                                            multipleOK=multipleOK, name=name, executor=executor, mergeTargetSize=mergeTargetSize)
        # Extra metadata known for athena files:

        for key in athFileInterestingKeys:
            self._metadataKeys[key] = self._getAthInfo
            
    ## @brief Workhorse which actually calls AthFile 
    def _callAthInfo(self, files, doAllFiles, retrieveKeys):
        if doAllFiles:
            myFiles = self._value
        else:
            myFiles = files
        msg.debug('Will retrieve AthFile info for {0!s}'.format(myFiles))
        athFileMetadata = AthenaFileInfo(myFiles, retrieveKeys=retrieveKeys, timeout=240+30*len(myFiles), defaultrc=None)
        if athFileMetadata == None:
            raise trfExceptions.TransformMetadataException(trfExit.nameToCode('TRF_METADATA_CALL_FAIL'), 'Call to AthenaFileInfo failed')
        for file, fileMetadata in athFileMetadata.iteritems():
            msg.debug('Setting metadata for file {0} to {1}'.format(file, fileMetadata))
            self._fileMetadata[file].update(fileMetadata)

    ## @brief Small wrapper which sets the standard options for doAllFiles and retrieveKeys
    def _getAthInfo(self, files):
        self._callAthInfo(files, doAllFiles = True, retrieveKeys=athFileInterestingKeys)

    ## @brief Utility to strip arguments which should not be passed to the selfMerge methods
    #  of our child classes
    #  @param copyArgs If @c None copy all arguments by default, otherwise only copy the
    #  listed keys
    def _mergeArgs(self, argdict, copyArgs=None):
        if copyArgs:
            myargdict = {}
            for arg in copyArgs:
                if arg in argdict:
                    myargdict[arg] = copy.copy(argdict[arg])
            
        else:
            myargdict = copy.copy(argdict)
        # Never do event count checks for self merging
        myargdict['checkEventCount'] = argSubstepBool('False', runarg=False)
        if 'athenaopts' in myargdict:
            # Need to ensure that "nprocs" is not passed to merger
            newopts = []
            for opt in myargdict['athenaopts'].value:
                if opt.startswith('--nprocs'):
                    continue
                newopts.append(opt)
            myargdict['athenaopts'] = argList(newopts, runarg=False)
        return myargdict

    @property
    def prodsysDescription(self):
        super(argAthenaFile, self).prodsysDescription
        return self._desc


## @brief ByteStream file class
class argBSFile(argAthenaFile):    

    integrityFunction = "returnIntegrityOfBSFile"

    def _getIntegrity(self, files):
        for file in files:
            try:
                rc=call(["AtlListBSEvents.exe", "-c", file], logger=msg, message="Report by AtlListBSEvents.exe: ", timeout=600)
            except trfExceptions.TransformTimeoutException:
                return False
            if rc==0:
                self._fileMetadata[file]['integrity'] = True
            else:
                self._fileMetadata[file]['integrity'] = False

    @property
    def prodsysDescription(self):
        super(argBSFile, self).prodsysDescription
        return self._desc
    

## @brief POOL file class.
#  @details Works for all POOL files
class argPOOLFile(argAthenaFile):
  
    integrityFunction = "returnIntegrityOfPOOLFile"
    
    # trfValidateRootFile is written in an odd way, so timelimit it here.
    @timelimited()
    def _getIntegrity(self, files):
        for file in files:
            from PyJobTransforms.trfValidateRootFile import checkFile
            rc=checkFile(fileName=file, type='event', requireTree=True)
            if rc==0:
                self._fileMetadata[file]['integrity'] = True
            else:
                self._fileMetadata[file]['integrity'] = False

    @property
    def prodsysDescription(self):
        super(argPOOLFile, self).prodsysDescription
        return self._desc

    ## @brief Method which can be used to merge files of this type
    #  @param output Target filename for this merge
    #  @param inputs List of files to merge
    #  @param argdict argdict of the transform
    #  @note @c argdict is not normally used as this is a @em vanilla merge
    def selfMerge(self, output, inputs, argdict={}):
        msg.debug('selfMerge attempted for {0} -> {1} with {2}'.format(inputs, output, argdict))
        
        # First do a little sanity check
        for file in inputs:
            if file not in self._value:
                raise trfExceptions.TransformMergeException(trfExit.nameToCode('TRF_FILEMERGE_PROBLEM'), 
                                                            "File {0} is not part of this agument: {1}".format(file, self))
        
        from PyJobTransforms.trfExe import athenaExecutor, executorConfig
        
        ## @note Modify argdict
        myargdict = self._mergeArgs(argdict) 
        
        # We need a athenaExecutor to do the merge
        # N.B. We never hybrid merge AthenaMP outputs as this would prevent further merging in another
        # task (hybrid merged files cannot be further bybrid merged) 
        myDataDictionary = {'POOL_MRG_INPUT' : argPOOLFile(inputs, type=self.type, io='input'),
                            'POOL_MRG_OUTPUT' : argPOOLFile(output, type=self.type, io='output')}
        myMergeConf = executorConfig(myargdict, myDataDictionary, disableMP=True)
        myMerger = athenaExecutor(name='POOLMerge_AthenaMP.{0}'.format(self._subtype), conf=myMergeConf, 
                                  skeletonFile = 'RecJobTransforms/skeleton.MergePool_tf.py',
                                  inData=set(['POOL_MRG_INPUT']), outData=set(['POOL_MRG_OUTPUT']), perfMonFile = 'ntuple_POOLMerge.pmon.gz')
        myMerger.doAll(input=set(['POOL_MRG_INPUT']), output=set(['POOL_MRG_OUTPUT']))
        
        # OK, if we got to here with no exceptions, we're good shape
        # Now update our own list of files to reflect the merge
        for file in inputs:
            self._value.remove(file)
        self._value.append(output)

        msg.debug('Post self-merge files are: {0}'.format(self._value))
        self._resetMetadata(inputs + [output])


class argHITSFile(argPOOLFile):

    integrityFunction = "returnIntegrityOfPOOLFile"

    ## @brief Method which can be used to merge HITS files
    def selfMerge(self, output, inputs, argdict={}):
        msg.debug('selfMerge attempted for {0} -> {1} with {2}'.format(inputs, output, argdict))
        
        # First do a little sanity check
        for file in inputs:
            if file not in self._value:
                raise trfExceptions.TransformMergeException(trfExit.nameToCode('TRF_FILEMERGE_PROBLEM'), 
                                                            "File {0} is not part of this agument: {1}".format(file, self))
        
        ## @note Modify argdict
        mySubstepName = 'HITSMerge_AthenaMP'
        myargdict = self._mergeArgs(argdict, copyArgs=['geometryVersion', 'conditionsTag', 'preExec', 'postExec', 'preInclude', 'postInclude'])
        
        from PyJobTransforms.trfExe import athenaExecutor, executorConfig
        myDataDictionary = {'HITS' : argHITSFile(inputs, type=self.type, io='input'),
                            'HITS_MRG' : argHITSFile(output, type=self.type, io='output')}
        myMergeConf = executorConfig(myargdict, myDataDictionary, disableMP=True)
        myMerger = athenaExecutor(name = mySubstepName, skeletonFile = 'SimuJobTransforms/skeleton.HITSMerge.py',
                                  conf=myMergeConf, 
                                  inData=set(['HITS']), outData=set(['HITS_MRG']), perfMonFile = 'ntuple_HITSMerge.pmon.gz')
        myMerger.doAll(input=set(['HITS']), output=set(['HITS_MRG']))
        
        # OK, if we got to here with no exceptions, we're good shape
        # Now update our own list of files to reflect the merge
        for file in inputs:
            self._value.remove(file)
        self._value.append(output)

        msg.debug('Post self-merge files are: {0}'.format(self._value))
        self._resetMetadata(inputs + [output])
    

class argRDOFile(argPOOLFile):

    integrityFunction = "returnIntegrityOfPOOLFile"

    ## @brief Method which can be used to merge RDO files
    def selfMerge(self, output, inputs, argdict={}):
        msg.debug('selfMerge attempted for {0} -> {1} with {2}'.format(inputs, output, argdict))
        
        # First do a little sanity check
        for file in inputs:
            if file not in self._value:
                raise trfExceptions.TransformMergeException(trfExit.nameToCode('TRF_FILEMERGE_PROBLEM'), 
                                                            "File {0} is not part of this agument: {1}".format(file, self))

        ## @note Modify argdict
        myargdict = self._mergeArgs(argdict) 
        
        from PyJobTransforms.trfExe import athenaExecutor, executorConfig
        myDataDictionary = {'RDO' : argHITSFile(inputs, type=self.type, io='input'),
                            'RDO_MRG' : argHITSFile(output, type=self.type, io='output')}
        myMergeConf = executorConfig(myargdict, myDataDictionary, disableMP=True)
        myMerger = athenaExecutor(name = 'RDOMerge_AthenaMP', skeletonFile = 'RecJobTransforms/skeleton.MergeRDO_tf.py',
                                  conf=myMergeConf, 
                                  inData=set(['RDO']), outData=set(['RDO_MRG']), perfMonFile = 'ntuple_RDOMerge.pmon.gz')
        myMerger.doAll(input=set(['RDO']), output=set(['RDO_MRG']))
        
        # OK, if we got to here with no exceptions, we're good shape
        # Now update our own list of files to reflect the merge
        for file in inputs:
            self._value.remove(file)
        self._value.append(output)

        msg.debug('Post self-merge files are: {0}'.format(self._value))
        self._resetMetadata(inputs + [output])
    
    

    

## @brief TAG file class
#  @details Has a different validation routine to ESD/AOD POOL files
class argTAGFile(argPOOLFile):

    integrityFunction = "returnIntegrityOfTAGFile"

    ## @brief Simple integrity checked for TAG files
    #  @details Checks that the event count in the POOLCollectionTree is the same as the AthFile value
    def _getIntegrity(self, files):
        for file in files:
            from PyJobTransforms.trfFileUtils import NTUPEntries
            eventCount = NTUPEntries(file, ['POOLCollectionTree'])
            if eventCount is None:
                msg.error('Got a bad event count for the POOLCollectionTree in {0}: {1}'.format(file, eventCount))
                self._fileMetadata[file]['integrity'] = False
                return
            if eventCount != self.getSingleMetadata(file, 'nentries'):
                msg.error('Event count for {0} from POOLCollectionTree disagrees with AthFile: {1} != {2}'.format(file, eventCount, self.getSingleMetadata(file, 'nentries')))
                self._fileMetadata[file]['integrity'] = False
                return
            self._fileMetadata[file]['integrity'] = True
            
    ## @brief Method which can be used to merge files of this type
    #  @param output Target filename for this merge
    #  @param inputs List of files to merge
    #  @param argdict argdict of the transform
    #  @note @c argdict is not normally used as this is a @em vanilla merge
    def selfMerge(self, output, inputs, argdict={}):
        msg.debug('selfMerge attempted for {0} -> {1} with {2}'.format(inputs, output, argdict))
        
        # First do a little sanity check
        for file in inputs:
            if file not in self._value:
                raise trfExceptions.TransformMergeException(trfExit.nameToCode('TRF_FILEMERGE_PROBLEM'), 
                                                            "File {0} is not part of this agument: {1}".format(file, self))
        
        from PyJobTransforms.trfExe import tagMergeExecutor, executorConfig
        
        ## @note Modify argdict
        myargdict = self._mergeArgs(argdict) 
        
        # We need a tagMergeExecutor to do the merge
        myDataDictionary = {'TAG_MRG_INPUT' : argTAGFile(inputs, type=self.type, io='input'),
                            'TAG_MRG_OUTPUT' : argTAGFile(output, type=self.type, io='output')}
        myMergeConf = executorConfig(myargdict, myDataDictionary, disableMP=True)
        myMerger = tagMergeExecutor(name='TAGMerge_AthenaMP.{0}'.format(self._subtype), exe = 'CollAppend', 
                                        conf=myMergeConf, 
                                        inData=set(['TAG_MRG_INPUT']), outData=set(['TAG_MRG_OUTPUT']),)
        myMerger.doAll(input=set(['TAG_MRG_INPUT']), output=set(['TAG_MRG_OUTPUT']))
        
        # OK, if we got to here with no exceptions, we're good shape
        # Now update our own list of files to reflect the merge
        for file in inputs:
            self._value.remove(file)
        self._value.append(output)

        msg.debug('Post self-merge files are: {0}'.format(self._value))
        self._resetMetadata(inputs + [output])


    @property
    def prodsysDescription(self):
        super(argTAGFile, self).prodsysDescription
        return self._desc


## @brief Data quality histogram file class
class argHISTFile(argFile):

    integrityFunction = "returnIntegrityOfHISTFile"

    def __init__(self, value=list(), io = 'output', type=None, subtype=None, splitter=',', runarg=True, countable=True, multipleOK = None,
                 name=None):
        super(argHISTFile, self).__init__(value=value, io=io, type=type, subtype=subtype, splitter=splitter, runarg=runarg, multipleOK=multipleOK,
                                          name=name)

        # Make events optional for HISTs (can be useful for HIST_AOD, HIST_ESD before hist merging)
        if countable:
            self._metadataKeys.update({
                                       'nentries': self._getNumberOfEvents
                                       })
        
    ## @brief There is no integrity check for HIST files - return 'UNDEFINED'
    def _getIntegrity(self, files):
        for file in files:
            self._fileMetadata[file]['integrity'] = 'UNDEFINED'
        
        
    def _getNumberOfEvents(self, files):
        for file in files:
            try:
                self._fileMetadata[file]['nentries'] = HISTEntries(file)
            except trfExceptions.TransformTimeoutException:
                msg.error('Timeout counting events for {0}'.format(file))

    @property
    def prodsysDescription(self):
        super(argHISTFile, self).prodsysDescription
        return self._desc
    

## @brief NTUP (plain ROOT) file class
#  @details Used for ATLAS D3PDs
class argNTUPFile(argFile):

    integrityFunction = "returnIntegrityOfNTUPFile"

    def __init__(self, value=list(), io = 'output', type=None, subtype=None, splitter=',', treeNames=None, runarg=True, multipleOK = None, 
                 name=None, mergeTargetSize=-1):
        super(argNTUPFile, self).__init__(value=value, io=io, type=type, subtype=subtype, splitter=splitter, runarg=runarg, multipleOK=multipleOK, 
                                          name=name, mergeTargetSize=mergeTargetSize)
        self._treeNames=treeNames

        self._metadataKeys.update({
                                   'nentries': self._getNumberOfEvents,
                                   'file_guid': self._generateGUID,
                                   'integrity': self._getIntegrity,
                                   })
                
        
    def _getNumberOfEvents(self, files):
        msg.debug('Retrieving event count for NTUP files {0}'.format(files))
        if self._treeNames is None:
            msg.debug('treeNames is set to None - event count undefined for this NTUP')
            for file in files:
                self._fileMetadata[file]['nentries'] = 'UNDEFINED'
        else:
            for file in files:
                try:
                    self._fileMetadata[file]['nentries'] = NTUPEntries(fileName=file, treeNames=self._treeNames)
                except trfExceptions.TransformTimeoutException:
                    msg.error('Timeout counting events for {0}'.format(file))


    def _getIntegrity(self, files):
        for file in files:
            from PyJobTransforms.trfValidateRootFile import checkFile
            rc=checkFile(fileName=file, type='basket', requireTree=False)
            if rc==0:
                self._fileMetadata[file]['integrity'] = True
            else:
                self._fileMetadata[file]['integrity'] = False
                
                
    def selfMarge(self, files):
        msg.debug('selfMerge attempted for {0} -> {1} with {2}'.format(inputs, output, argdict))
        
        # First do a little sanity check
        for file in inputs:
            if file not in self._value:
                raise trfExceptions.TransformMergeException(trfExit.nameToCode('TRF_FILEMERGE_PROBLEM'), 
                                                            "File {0} is not part of this agument: {1}".format(file, self))
        
        from PyJobTransforms.trfExe import NTUPMergeExecutor, executorConfig
        
        ## @note Modify argdict
        myargdict = self._mergeArgs(argdict) 
        
        # We need a NTUPMergeExecutor to do the merge
        myDataDictionary = {'NTUP_MRG_INPUT' : argPOOLFile(inputs, type=self.type, io='input'),
                            'NYUP_MRG_OUTPUT' : argPOOLFile(output, type=self.type, io='output')}
        myMergeConf = executorConfig(myargdict, myDataDictionary, disableMP=True)
        myMerger = NTUPMergeExecutor(name='NTUPMerge_AthenaMP.{0}'.format(self._subtype), conf=myMergeConf, 
                                     inData=set(['NTUP_MRG_INPUT']), outData=set(['NTUP_MRG_OUTPUT']))
        myMerger.doAll(input=set(['NTUP_MRG_INPUT']), output=set(['NYUP_MRG_OUTPUT']))
        
        # OK, if we got to here with no exceptions, we're good shape
        # Now update our own list of files to reflect the merge
        for file in inputs:
            self._value.remove(file)
        self._value.append(output)

        msg.debug('Post self-merge files are: {0}'.format(self._value))
        self._resetMetadata(inputs + [output])

                
    @property
    def prodsysDescription(self):
        super(argNTUPFile, self).prodsysDescription
        return self._desc  



## @brief TarBZ filetype
class argBZ2File(argFile):
    def _getIntegrity(self, files):
        for file in files:
            # bz2 only supports 'with' from python 2.7
            try:
                f = bz2.BZ2File(file, 'r')
                while True:
                    chunk = len(f.read(1024*1024))
                    msg.debug('Read {0} bytes from {1}'.format(chunk, file)) 
                    if chunk == 0:
                        break
                self._fileMetadata[file]['integrity'] = True
                f.close()
            except (OSError, IOError) as e:
                msg.error('Got exception {0!s} raised while checking integrity of file {1}'.format(e, file))
                self._fileMetadata[file]['integrity'] = False
                

    @property
    def prodsysDescription(self):
        super(argBZ2File, self).prodsysDescription
        return self._desc  


## @brief Class which defines a special input file format used in FTK simulation     
class argFTKIPFile(argBZ2File):
    def __init__(self, value=list(), io = 'output', type=None, splitter=',', runarg=True, multipleOK=None, name=None):
        super(argFTKIPFile, self).__init__(value=value, io=io, type=type, splitter=splitter, runarg=runarg, multipleOK=multipleOK,
                                           name=name)
        self._metadataKeys.update({
                                   'nentries': self._getNumberOfEvents
                                   })

    def _getNumberOfEvents(self, files):
        for file in files:
            try:
                eventCount = 0
                f = bz2.BZ2File(file, 'r')
                for line in f:
                    if line.startswith('F'):
                        eventCount += 1
                self._fileMetadata[file]['nentries'] = eventCount
            except (OSError, IOError) as e:
                msg.error('Event count for file {0} failed: {1!s}'.format(file, e))
                self._fileMetadata[file]['nentries'] = None

    @property
    def prodsysDescription(self):
        super(argFTKIPFile, self).prodsysDescription
        return self._desc
    
## @brief HEP ASCII file
#  @details An ASCII file representation of HEP events
class argHepEvtAsciiFile(argFile):
    def __init__(self, value=list(), io = 'output', type='txt_evt', splitter=',', runarg=True, multipleOK=None, name=None):
        super(argHepEvtAsciiFile, self).__init__(value=value, io=io, type=type, splitter=splitter, runarg=runarg, 
                                                 multipleOK=multipleOK, name=name)
        self._metadataKeys.update({
                                   'nentries': self._getNumberOfEvents
                                   })
        
    def _getNumberOfEvents(self, files):
        for file in files:
            try:
                eventCount = 0
                f = open(file, 'r')
                for line in f:
                    if len(line.split(" "))==3:
                        eventCount += 1
                self._fileMetadata[file]['nentries'] = eventCount
            except (OSError, IOError) as e:
                msg.error('Event count for file {0} failed: {1!s}'.format(file, e))
                self._fileMetadata[file]['nentries'] = None
                

## @brief Base class for substep arguments
#  @details Sets up a dictionary with {substep1: value1, substep2: value2, ...}
#  In this base class we don't put any restructions on the values - they will be specialised
#  in children.
class argSubstep(argument):
    
    ## @brief argSubstep constructor
    #  @param defaultSubstep Default substep to use when no specific substep has been given - this should be
    #  set to @c first for arguments that apply, by default, to the first executor in the chain (e.g., @c maxEvents)
    def __init__(self, value = None, runarg = True, name = None, defaultSubstep = 'all', separator = ':'):
        self._defaultSubstep = defaultSubstep
        self._separator = separator
        super(argSubstep, self).__init__(value, runarg, name)
    
    # Reset getter
    @property
    def value(self):
        return self._value

    # The default setter for sustep class
    @value.setter
    def value(self, value):
        msg.debug('Attempting to set argSubstep from {0!s} (type {1}'.format(value, type(value)))
        if value is None:
            self._value = {}
        elif isinstance(value, str):
            subStep, subStepValue = self._parseStringAsSubstep(value)
            self._value = {subStep: subStepValue}
        elif isinstance(value, (list, tuple)):
            # This is a list of strings to parse, so we go through them one by one
            self._value = {}
            for item in value:
                if not isinstance(item, str):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert list item {0!s} to substep (should be a string)'.format(item))
                subStep, subStepValue = self._parseStringAsSubstep(item)
                self._value[subStep] = subStepValue
        elif isinstance(value, dict):
            self._value = value
        else:
            raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Setter value {0!s} (type {1}) for substep argument cannot be parsed'.format(value, type(value)))
            
    
    ## @brief Parse a string for substep:value format
    #  @details If the string matches the substep specifier regexp then we return the two parts;
    #  if not then the substep is returned as @c self._defaultSubstep, with the entire string passed
    #  back as the value
    #  @param string The string which should be parsed
    def _parseStringAsSubstep(self, string):
        subStepMatch = re.match(r'([a-zA-Z0-9]+)' + self._separator + r'(.*)', string)
        if subStepMatch:
            subStep = subStepMatch.group(1)
            subStepValue = subStepMatch.group(2)
        else:
            subStep = self._defaultSubstep
            subStepValue = string
        msg.debug('Parsed {0} as substep {1}, argument {2}'.format(string, subStep, subStepValue))
        return subStep, subStepValue
    
    
    ## @brief Return the value of this substep arg for an executor with the given parameters
    #  @param name Executor name
    #  @parem substep Executor substep nickname
    #  @param first Boolean flag set true if this is the first executor in the chain
    #  @param exe Executor instance, from which 'name', 'substep' and 'first' can be taken.  
    def returnMyValue(self, name=None, substep=None, first=False, exe=None):
        if exe:
            name = exe.name
            substep = exe.substep
            first = exe.conf.firstExecutor
            
        value = None
        ## @note First we see if we have an explicit name or substep match, then a special 'first' or 'default' match
        if name in self._value.keys():
            value = self._value[name]
        elif substep in self._value.keys():
            value = self._value[substep]  
        elif first and 'first' in self._value.keys():
            value = self._value['first']
        elif 'default' in self._value.keys(): 
            value = self._value['default']
            
        ## @note Now see how we should handle an 'all', if it exists.
        #  This means using it as the value, if that's not yet defined or,
        #  if it is defined and we have a mutable seq type, using it as
        #  a prefix to the more specific setting (this behaviour requested here:
        #  https://its.cern.ch/jira/browse/ATLASJT-24)
        ## @note Defining all: for a key which is not composable (like a list)
        #  doesn't make much sense and, in this case, the specific value is allowed
        #  to trump the all:
        if 'all' in self._value.keys():
            if value is None:
                value = self._value['all']
            elif isinstance(value, list):
                value = self._value['all'] + value
                
        msg.debug('From substep argument {myvalue} picked value "{value}" for {name}, {substep}, first={first}'.format(myvalue=self._value, value=value, name=name, substep=substep, first=first))
        
        return value

    @property
    def prodsysDescription(self):
        self._desc = {'type' : 'Substep'}
        return self._desc
    
## @brief Argument class for substep lists, suitable for preExec/postExec 
#  @details substep is followed by a ':'  then the python fragments.
#  The substep is validated and set as the key in a dictionary with the value being the python following the ':'.
#  - If no substep (r2e, e2e, etc) is specified, it will be parsed for 'all'.
#  @note substep can be either the subspet property of an executor or the executor's name.
#  Thus r2e:something is the same as RAWtoESD:something.
class argSubstepList(argSubstep):
    
    ## @brief argSubstepList constructor
    #  @details Adds the optional splitter argument to the base argSubstep class
    #  @param splitter Character used to split the string into multiple list elements
    def __init__(self, value = None, runarg = True, name = None, defaultSubstep = 'all', splitter = None, separator=':'):
        self._splitter = splitter
        super(argSubstepList, self).__init__(value, runarg, name, defaultSubstep, separator)

        
    # Reset getter
    @property
    def value(self):
        return self._value

    @value.setter
    def value(self, value):
        msg.debug('Attempting to set argSubstep from {0!s} (type {1}'.format(value, type(value)))
        if value is None:
            self._value = {}
        elif isinstance(value, str):
            subStep, subStepValue = self._parseStringAsSubstep(value)
            self._value = {subStep: subStepValue}
        elif isinstance(value, (list, tuple)):
            # This is a list of strings to parse
            self._value = {}
            for item in value:
                if not isinstance(item, str):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert list item {0!s} to substep (should be a string)'.format(item))
                subStep, subStepValue = self._parseStringAsSubstep(item)
                if subStep in self._value:
                    self._value[subStep].extend(subStepValue)
                else:
                    self._value[subStep] = subStepValue
        elif isinstance(value, dict):
            for k, v in value.iteritems():
                if not isinstance(k, str):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Dictionary key {0!s} for substep is not a string'.format(k))
                if not isinstance(v, list):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Dictionary value {0!s} for substep is not a list'.format(v))
            self._value = value
        else:
            raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Setter value {0!s} (type {1}) for substep argument cannot be parsed'.format(value, type(value)))

    ## @brief Specialist parser for lists, which applies the splitter string, if defined
    #  @return Tuple of substep plus a list of strings
    def _parseStringAsSubstep(self, value):
        subStep, subStepValue = super(argSubstepList, self)._parseStringAsSubstep(value)
        if self._splitter:
            return subStep, subStepValue.split(self._splitter)
        else:
            return subStep, [subStepValue]


## @brief Boolean substep argument
class argSubstepBool(argSubstep):
    
    # Reset getter
    @property
    def value(self):
        return self._value
    
    @value.setter
    def value(self, value):
        msg.debug('Attempting to set argSubstep from {0!s} (type {1}'.format(value, type(value)))
        if value is None:
            self._value = {}
        elif isinstance(value, str):
            subStep, subStepValue = self._parseStringAsSubstep(value)
            self._value = {subStep: strToBool(subStepValue)}
        elif isinstance(value, (list, tuple)):
            # This is a list of strings to parse
            self._value = {}
            for item in value:
                if not isinstance(item, str):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert list item {0!s} to substep (should be a string)'.format(item))
                subStep, subStepValue = self._parseStringAsSubstep(item)
                self._value[subStep] = strToBool(subStepValue)
        elif isinstance(value, dict):
            for k, v in value.iteritems():
                if not isinstance(k, str):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Dictionary key {0!s} for substep is not a string'.format(k))
                if not isinstance(v, bool):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Dictionary value {0!s} for substep is not a bool'.format(v))
            self._value = value
        else:
            raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Setter value {0!s} (type {1}) for substep argument cannot be parsed'.format(value, type(value)))


## @brief Int substep argument
class argSubstepInt(argSubstep):
    
    # Reset getter
    @property
    def value(self):
        return self._value
    
    @value.setter
    def value(self, value):
        msg.debug('Attempting to set argSubstep from {0!s} (type {1}'.format(value, type(value)))
        try:
            if value is None:
                self._value = {}
            elif isinstance(value, str):
                subStep, subStepValue = self._parseStringAsSubstep(value)
                self._value = {subStep: int(subStepValue)}
            elif isinstance(value, (list, tuple)):
                # This is a list of strings to parse
                self._value = {}
                for item in value:
                    if not isinstance(item, str):
                        raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert list item {0!s} to substep (should be a string)'.format(item))
                    subStep, subStepValue = self._parseStringAsSubstep(item)
                    self._value[subStep] = int(subStepValue)
            elif isinstance(value, dict):
                for k, v in value.iteritems():
                    if not isinstance(k, str):
                        raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Dictionary key {0!s} for substep is not a string'.format(k))
                    if not isinstance(v, (int, long)):
                        raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Dictionary value {0!s} for substep is not an int'.format(v))
                self._value = value
            else:
                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Setter value {0!s} (type {1}) for substep argument cannot be parsed'.format(value, type(value)))
        except ValueError, e:
            raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert substep value {0} to int'.format(value))



## @brief Float substep argument
class argSubstepFloat(argSubstep):
    
    def __init__(self, value=None, min=None, max=None, runarg=True, name=None):
        self._min = min
        self._max = max
        self._desc = {}
        super(argSubstepFloat, self).__init__(value = value, runarg = runarg, name=name)
        
    @property
    def prodsysDescription(self):
        self._desc = {'type' : 'argSubstepFloat'}
        if self._min:
            self._desc['min'] = self._min
        if self._max:
            self._desc['max'] = self._max
        return self._desc
    
        
    # Reset getter
    @property
    def value(self):
        return self._value
    
    @value.setter
    def value(self, value):
        msg.debug('Attempting to set argSubstep from {0!s} (type {1}'.format(value, type(value)))
        try:
            if value is None:
                self._value = {}
            elif isinstance(value, str):
                if (self._min != None and float(value) < self._min) or (self._max != None and float(value) > self._max):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_OUT_OF_RANGE'),
                                                          'argFloat value out of range: %s is not between %s and %s' % 
                                                          (value, self._min, self._max))
                else:
                    subStep, subStepValue = self._parseStringAsSubstep(value)
                    self._value = {subStep: float(subStepValue)}
            elif isinstance(value, (list, tuple)):
                # This is a list of strings to parse
                self._value = {}
                for item in value:
                    if not isinstance(item, str):
                        raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert list item {0!s} to substep (should be a string)'.format(item))
                    if (self._min != None and float(item) < self._min) or (self._max != None and float(item) > self._max):
                        raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_OUT_OF_RANGE'),
                                                          'argFloat value out of range: %s is not between %s and %s' % 
                                                          (item, self._min, self._max))
                    subStep, subStepValue = self._parseStringAsSubstep(item)
                    self._value[subStep] = float(subStepValue)
            elif isinstance(value, dict):
                for k, v in value.iteritems():
                    if not isinstance(k, str):
                        raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Dictionary key {0!s} for substep is not a string'.format(k))
                    if not isinstance(v, float):
                        raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Dictionary value {0!s} for substep is not an float'.format(v))
                    if (self._min != None and float(v) < self._min) or (self._max != None and float(v) > self._max):
                        raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_OUT_OF_RANGE'),
                                                          'argFloat value out of range: %s is not between %s and %s' % 
                                                          (v, self._min, self._max))
                self._value = value
            else:
                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Setter value {0!s} (type {1}) for substep argument cannot be parsed'.format(value, type(value)))
        except ValueError, e:
            raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert substep value {0} to float'.format(value))


## @brief Special argument class to hold steering information
class argSubstepSteering(argSubstep):
    # Reset getter
    @property
    def value(self):
        return self._value

    ## @details For strings passed to the setter we expect the format to be @c substep:{in/out}{+/-}DATATYPE
    #  This is then cast into a dictionary of tuples {substep: [('in/out', '+/-', DATATYPE), ...], ...}
    @value.setter
    def value(self, value):
        msg.debug('Attempting to set argSubstepSteering from {0!s} (type {1}'.format(value, type(value)))
        if value is None:
            self._value = {}
        elif isinstance(value, dict):
            # OK, this should be the direct setable dictionary - but do a check of that
            for k, v in value.iteritems():
                if not isinstance(k, str) or not isinstance(v, list):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert dict {0!s} to argSubstepSteering'.format(value))
                for subv in v:
                    if not isinstance(subv, (list, tuple)) or len(subv) != 3 or subv[0] not in ('in', 'out') or subv[1] not in ('+', '-'):                    
                        raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert dict {0!s} to argSubstepSteering'.format(value))                    
            self._value = value
        elif isinstance(value, str):
            # Single string value case
            subStep, subStepValue = self._parseStringAsSubstep(value)
            self._value = {subStep: self._parseSteeringString(subStepValue)}
        elif isinstance(value, (list, tuple)):
            # This is a list of strings to parse
            self._value = {}
            for item in value:
                if not isinstance(item, str):
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert list item {0!s} to substep (should be a string)'.format(item))
                subStep, subStepValue = self._parseStringAsSubstep(item)
                self._value.update({subStep: self._parseSteeringString(subStepValue)})
        else:
            raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Setter value {0!s} (type {1}) for substep argument cannot be parsed'.format(value, type(value)))

    def _parseSteeringString(self, ivalue):
        retvalue = []
        for subvalue in ivalue.split(','):
            matchedParts = re.match(r'(in|out)(\+|\-)([A-Z_]+)$', subvalue)
            if not matchedParts:
                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert string {0!s} to argSubstepSteering'.format(value))
            retvalue.append((matchedParts.group(1), matchedParts.group(2), matchedParts.group(3)))
        return retvalue

        
class trfArgParser(argparse.ArgumentParser):

    ## @brief Subclassing argparse.    
    # @details Changed the defualt methods for add_arguments and parse_args.
    # This is so that the help functions and the namespace object can be more useful.
    # @note @c self._helpString. Is a dictionary with the where the key-value pairs are the argument name and the help text.
    # @note @c self._argClass. Is a dictionary so the 'type' of argument, i.e. the class that it uses can be extracted.
    def __init__(self, *args, **kwargs):
        self._helpString = {}
        self._argClass = {}
        self._argGroups = {}
        super(trfArgParser, self).__init__(*args, **kwargs)

    def add_argument(self, *args, **kwargs):
        # Convert argument name to argparse standard
        argName = cliToKey(args[0])
        msg.debug('found arg name {0}'.format(argName))
        
        # Prevent a crash if this argument already exists (there are valid use cases for 'grabbing' an
        # argument, so this is INFO, not WARNING)
        if argName in self._argClass:
            msg.info('Double definition of argument {0} - ignored'.format(argName))
            return
        
        # if there is a help function defined for the argument then populate the helpString dict
        if 'help' in kwargs: 
            self._helpString[argName] = kwargs['help'] # if the help option is present for the argument then put it into the helpString dict key = argument name, value = help 
        else:
            self._helpString[argName] = None
        if 'type' in kwargs:
            self._argClass[argName] = kwargs['type']
        else:
            self._argClass[argName] = None

        # Remove kwargs which are not understood by ArgumentParser.add_argument()
        strippedArgs = {}
        for arg in ('group',):
            if arg in kwargs:
                strippedArgs[arg] = kwargs.pop(arg)
            
        # Optinally add an argument to an argparse argument group
        if 'group' in strippedArgs:
            if strippedArgs['group'] in self._argGroups:
                msg.debug('Adding argument to group {0}: ({1}; {2})'.format(strippedArgs['group'], args, kwargs))
                self._argGroups[strippedArgs['group']].add_argument(*args, **kwargs)
            else:
                msg.warning('Argument group {0} not defined - adding argument to main parser'.format(strippedArgs['group']))
                msg.debug('Adding argument: ({0}; {1})'.format(args, kwargs))
                super(trfArgParser, self).add_argument(*args, **kwargs)
        else:
            msg.debug('Adding argument: ({0}; {1})'.format(args, kwargs))
            super(trfArgParser, self).add_argument(*args, **kwargs)

    def getProdsysDesc(self):
        desc = {}
        for name, argClass in self._argClass.iteritems():
            if type(argClass)!=type(None):
                desc[name] = argClass().prodsysDescription
                if self._helpString[name]:
                    desc[name].update({'help': self._helpString[name]})
        return desc
            
    ## Define an argparse argument group for the main parser to use
    def defineArgGroup(self, *args):
        # Get an argparse group
        if args[0] in self._argGroups:
            msg.warning('Argument group %s already exists' % args[0])
            return
        self._argGroups[args[0]] = self.add_argument_group(*args)
        
    ## Return the help string for a given argument
    def getHelpString(self, argument):
        try:
            return(self._helpString[argument])
        except KeyError:
            raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_KEY_ERROR'), 'no help string available for argument %s' %argument)
        return None

    ## @brief Return a list of all arguments understood by this transform in prodsys style
    #  @details Arguments which are irrelevant for production are removed and the '--' is added back on          
    def dumpArgs(self):
        keyArray = [ '--' + str(key) for key in self._helpString.keys() if key not in ('h', 'verbose', 'loglevel', 'dumpargs', 'argdict') ]
        keyArray.sort()
        print 'ListOfDefaultPositionalKeys={0}'.format(keyArray)
        
    ## Getter for argument list
    @property
    def allArgs(self):
        return self._helpString.keys()


    ## @brief Call argument_parser parse_args, then concatenate values
    #  @details Sets-up the standard argparse namespace, then use a special
    #           treatment for lists (arising from nargs='+'), where values
    #           are appropriately concatenated and a single object is returned 
    #  @return argument_parser namespace object
    def parse_args(self, args = None, namespace = None): 
        if namespace:
            super(trfArgParser, self).parse_args(args = args, namespace = namespace) 
        else:
            namespace = super(trfArgParser, self).parse_args(args = args)
        for k, v in namespace.__dict__.iteritems(): 
            msg.debug('Treating key %s (%s)' % (k, v))
            if isinstance(v, list):
                # We build on the v[0] instance as this contains the correct metadata
                # and object references for this instance (shallow copying can 
                # mess up object references and deepcopy thows exceptions!)
                newValueObj = v[0] 
                msg.debug('Started with: %s = %s' % (type(newValueObj), newValueObj))
                if isinstance(v[0], argSubstep):
                    # Make sure you do not have a reference to the original value - this is a deeper copy
                    newValues = dictSubstepMerge(v[0].value, {})
                elif isinstance(v[0].value, list):
                    newValues = v[0].value
                elif isinstance(v[0].value, dict):
                    newValues = v[0].value
                else:
                    newValues = [v[0].value,]
                for valueObj in v[1:]:
                    msg.debug('Value Object: %s = %s' % (type(valueObj), valueObj))
                    if isinstance(v[0], argSubstep):
                        # Special merger for lists attached to substeps
                        newValues = dictSubstepMerge(newValues, valueObj.value)
                    elif isinstance(valueObj.value, list):
                        # General lists are concatenated
                        newValues.extend(valueObj.value)
                    elif isinstance(valueObj.value, dict):
                        # General dictionaries are merged
                        newValues.update(valueObj.value)
                    else:
                        newValues.append(valueObj.value)
                newValueObj.value = newValues
                namespace.__dict__[k] = newValueObj
                msg.debug('Set to %s' % newValueObj.value)                

        return namespace


## @brief Small utility to convert a string value to a boolean
def strToBool(string):
    try:
        if string.lower() == 'false':
            return False
        elif string.lower() == 'true':
            return True
        else:
            raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert value {0} to bool'.format(string))
    except AttributeError:
        raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 'Failed to convert value {0} to bool'.format(string))

## @brief special dictionary merger which is used for substep type arguments
#  @details Uses set class to get the unique list of keys. Any key present in only one dictionary
#  is unaltered. If the values are lists they are catenated, otherwise the values are picked
#  from the dictionary in which it is set.
#  @param @c dict1 First dictionary
#  @param @c dict2 Second dictionary
#  @return Merged dictionary
#  @raise trfExceptions.TransformArgException If both dictionaries contain non-list keys which
#  are not the same.
def dictSubstepMerge(dict1, dict2):
    mergeDict = {}
    allKeys = set(dict1.keys()) | set(dict2.keys())
    # Find the value type - lists are special...
    listType = False
    if len(dict1) > 0:
        if isinstance(dict1.values()[0], list):
            listType = True
    elif len(dict2) > 0:
        if isinstance(dict2.values()[0], list):
            listType = True
    if listType:
        for key in allKeys:
            mergeDict[key] = dict1.get(key, []) + dict2.get(key, [])
    else:
        for key in allKeys:
            if key in dict1 and key in dict2:
                # Don't really know what to do if these clash...
                if dict1[key] != dict2[key]:
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_CONV_FAIL'), 
                                                              'Merging substep arguments found clashing values for substep {0}: {1}!={2}'.format(key, dict1[key], dict2[key]))
                mergeDict[key] = dict1[key]
            elif key in dict1:
                mergeDict[key] = dict1[key]
            else:
                mergeDict[key] = dict2[key]
                
    return mergeDict


