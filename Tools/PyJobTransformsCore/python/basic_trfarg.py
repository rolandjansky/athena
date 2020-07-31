# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @package basic_trfarg
#
#  @brief Package contains the basic argument types for JobTransforms.
#  @details Classes defined in this package are not intended to be used directly.
#  @see Argument classes designed to be used can be found in the full_trfarg package.

from __future__ import print_function

import os, sys, time, fnmatch, subprocess, copy, re
from PyJobTransformsCore import fileutil, envutil
from PyJobTransformsCore.trfutil import AODFile, BSFile, CommentLine, DPDFile, ESDFile, EvgenFile, FileType, HitsFile, PoolDataFile, expandStringToList, strip_suffix
from PyJobTransformsCore.trferr import TransformDefinitionError, TransformArgumentError, InputFileError, OutputFileError
from PyJobTransformsCore.JobReport import FileInfo
from PyJobTransformsCore.TransformLogger import TransformLogger


## @brief Base class of all transform argument classes.
#  @details The Argument base class specifies basic properties of a command line argument.
#  Only fully specified argument classes can be used in a concrete trf.JobTransform implementation.
#  A fully specified argument is derived from one of the basic argument classes defined
#  in this package and should override the member function isFullArgument() allowing
#  it to return @c True.
#  @remarks The @em base class Argument is itself derived from the TransformLogger to benefit from logging facilities.
#  It is probably better to gain such facilities through delegation rather than inheritence. 
class Argument( TransformLogger ):
    ## Argument constructor
    #  @param help A short help (String) to assist the user with the use of the argument.
    #  @param name Name of the argument (String). Defaults to string obtained from Argument.defaultName().
    #  @return Argument instance
    def __init__( self, help = 'default', name = 'default' ):
        if help == 'default': help = self.defaultHelp()
        if name == 'default': name = self.defaultName()
        TransformLogger.__init__( self, name )
        # The name (String) of the argument.
        self._name = name
        # The help (String) to assist with the population of the argument.
        self._help = help
        ## The current value atributed to this argument.
        self._value = None
        ## The original value of the argument prior to any changes.
        # Allows for any changed value to be reverted to its original value.
        self._originalValue = None
        ## Numerical position of the argument in a list of arguments.
        self._position = 0

    ## Allow the argument to be asked if it is valid i.e. if _value has been set.
    #  @return Boolean
    def __nonzero__( self ):
        return self._value is not None

    ## Getter function for the short help (String).
    #  @return String
    def help( self ):
        return self._help

    ## @brief Retrieve the full help (String) supplemented by information in the Argument object.
    #  @details Various additional information are added depending on whether the argument is
    #  @em optional, it's @em position in the command line and the presence of any @em default values.
    #  @return String
    def fullHelp( self ):
        help = ''
        if self.isOptional():
            help += '['
        else:
            help += ' '
        if self.hasPosition():
            help += '%2d ' % self.position()
        else:
            help += ' ? '
        help += '%s' % self._name
        if self.isOptional(): help += ']'
        help += ' (%s)' % self.basicType()
        if self.hasDefault(): help += ' default=%s' % repr( self.default() )
        return '%s # %s' % ( help, self.help() )

    ## Retrieve default help string.
    #  @remarks Must be re-implemented in derived classes.
    #  @return String
    def defaultHelp( self ):
        return self.__class__.__doc__

    ## Check if argument instance has the _default property.
    #  @return Boolean
    def hasDefault( self ):
        return hasattr( self, '_default' )

    ## Getter function for the _default property. 
    #  @return value of _default property (or @c None is property missing).
    def default( self ):
        return getattr( self, '_default', None )

    ## Setter function for the _default property of this argument.
    #  This implies that this argument is @em optional.
    #  @param default The value to assigned to the _default property. 
    #  @remarks If @em default is @c None, the _default property (if it exists) is removed.
    #  @exception TransformDefinitionError, TransformArgumentError 
    #  is raised when the default value cannot be set successfully.
    #  @return None
    def setDefault( self, default ):
        if default is None:
            if self.hasDefault(): del self._default
        else:
            # test that the default value can be converted to the correct type
            try: 
                self.toPython( default )
            except TransformDefinitionError as msg:
                # re-raise with different error message
                raise TransformDefinitionError( 'Unable to set default value: %s' % msg )
            except TransformArgumentError as msg:
                raise TransformArgumentError( 'Unable to set default value: %s' % msg )
            # store original value (so self.setValue() will do the conversion)
            self._default = default

    ## Setter function for the _name property.
    #  @return None
    def setName( self, name ):
        self._name = name

    ## Getter function for the _name property.
    #  @return String
    def name( self ):
        return self._name

    ## Retrieve the default name of the argument which is the String derived from the
    #  argumentType() function with its first character converted to lowercase.
    #  @return String
    def defaultName( self ):
        meta = self.argumentType()
        if meta == '':
            return self.__class__.__name__
        return meta[ 0 ].lower() + meta[ 1: ]

    ## Set the value of the argument.
    #  @param value @em value is converted to the correct type based on the different
    #  toPython() methods defined by different/derived arguments. 
    #  If @c None, the argument value's is cleared.  
    #  @return None
    def setValue( self, value ):
        if value is None:
            self._value = None
            self._originalValue = None
            self.logger().debug( "Cleared value" )
        else:
            oldValue = self._value
            self._value = self.toPython( value )
            if self._value != oldValue:
                self._originalValue = value
    
    ## Getter function for the _value property.
    #  @return The _value property
    def value( self ):
        return self._value

    ## Check if the argument's value has been set.
    #  @return Boolean
    def hasValue( self ):
        return self._value is not None

    ## The original input value as provided at the command line.
    #  @return The _originalValue property
    def originalValue( self ):
        return self._originalValue

    ## Setter for the _position property.
    #  @param pos The position of the argument value at the command line.
    #  @return None
    def setPosition( self, pos ):
        self._position = max( 0, pos )

    ## Check if the position property has been set.
    #  @return Boolean
    def hasPosition( self ):
        return self._position > 0

    ## Getter function for the _position property.
    #  @return Integer
    def position( self ):
        return self._position

    ## Retrieve the type of argument, by inspecting and inferring from it's class name.
    #  The argument type is essentially the class name without the 'Arg' suffix.
    #  @return String
    def argumentType( self ):
        meta = self.__class__.__name__
        # remove the "Arg" postfix from the classname
        meta = strip_suffix( meta, 'Arg' )
        return meta

    ## Determine if the argument if defined in a transform must be given a value or
    #  if there is a default value to rely on.
    #  @return Boolean
    def isOptional( self ):
        return self.hasDefault()

    ## Create a formatting string that will be used in templates that accept dictionaries
    #  when populating the variable components of the template
    #  (rather than tuples of positional sensitive values).
    #  @return String
    def namedTypeFormat( self ):
        return '%%(%s)%s' % ( self._name, self.typeFormat() )

    ## Generate an object member assignment string in Python syntax.
    #  @param objName The name of the object which the argument (self) is a property of. Defaults to @c None.
    #  @return String
    def pythonVariableTemplate( self, objName = None ):
        plate = ''
        if objName: plate += objName + '.'
        plate += '%s = %s' % (self._name,self.namedTypeFormat())
        return plate

    ## Identical to pythonVariableTemplate().
    #  @return String
    def runArgsTemplate( self, objName ):
        return self.pythonVariableTemplate( objName )

    ## Generate a formatted help String.
    #  @return String
    def runArgsComment( self ):
        return os.linesep + CommentLine( self._help ).smallComment()
        
    ## Deprecated function.
    #  @warning Deprecated. Do not use.
    def jobOrTask( self ):
        return 'task'

    ## Return the argument and it's value in a dictionary.
    #  @return Dictionary
    def metaData( self ):
        if not self:
            return {}
        else:
            return { self.name() : self.value() }

    ## Return a single character representing the type of the value of the argument according to
    #  Python's String formatting syntax. Eg. An Integer is @c d, a String is @c s, etc.
    #  @return a single character.
    def typeFormat( self ):
        return 'r'

    ## Private helper function. Raise exception with error message in case something is not implemented in a class.
    #  @param what The name of the method not implemented.
    #  @exception TransformDefinitionError is raised invariantly when function is called.
    def _notImplemented( self, what ):
        raise TransformDefinitionError( '%s not implemented in class %s' % ( what, self.__class__.__name__ ) )

    ## Convert a given @em val to the Python type the argument expects.
    #  @param val Convert the String obtained at the command line to the required Python type.
    #  @remarks This is a virtual method to be re-implemented by a derived class.
    #  A call to _notImplemented() invariantly raises a TransformDefinitionError exception.
    def toPython( self, val ):
        self._notImplemented( 'toPython()' )

    ## Retrieve the basic Python type for this argument.
    #  @warning Deprecated. Do not use.
    #  @remarks This is a virtual method to be re-implemented by a derived class.
    #  A call to _notImplemented() invariantly raises a TransformDefinitionError exception.
    def basicType( self ):
        self._notImplemented( 'basicType()' )

    ## Retrieve the type associated with this argument as defined by the Production System.
    #  @remarks This is a virtual method to be re-implemented by a derived class.
    #  A call to _notImplemented() invariantly raises a TransformDefinitionError exception.
    def type( self ):
        self._notImplemented( 'type()' )

    ## Retrieve the metaType associated with this argument as defined by the Production System.
    #  @remarks This is a virtual method to be re-implemented by a derived class.
    #  A call to _notImplemented() invariantly raises a TransformDefinitionError exception.
    def metaType( self ):
        self._notImplemented( 'metaType()' )

    ## This function determines if the argument has been designed for internal use only or
    #  permitted to be used in a transformation.
    #  @return Boolean
    def isFullArgument( self ):
        return False


## Mix-in argument helper base class allowing the setting of value ranges.
class ArgRange:
    ## ArgRange constructor.
    #  @param minimum Set the minimum value permissible. The minimum property is not created if not declared in the constructor.
    #  @param maximum Set the maximum value permissible. The maximum property is not created if not declared in the constructor.
    def __init__( self, minimum = None, maximum = None ):
        if minimum is not None: self.minimum = minimum
        if maximum is not None: self.maximum = maximum

    ## Check the existence of the mininum property.
    #  @return Boolean
    def hasMinimum( self ):
        return hasattr( self, 'minimum' )

    ## Check the existence of the maximum property.
    #  @return Boolean
    def hasMaximum( self ):
        return hasattr( self, 'maximum' )

    ## Getter function for the minimum property.
    #  @return Integer (@c None if minimum property does not exist )
    def getMinimum( self ):
        return getattr( self, 'minimum', None )

    ## Getter function for the maximum property.
    #  @return Integer (@c None if maximum property does not exist )
    def getMaximum( self ):
        return getattr( self, 'maximum', None )

    ## Check if @em val is within the set range.
    #  @param val The value to check against the range.
    #  @return Integer (@c 0 if within the range, @c -1 if less than the minimum and @c 1 if greater than the maximum. 
    def checkRange( self, val ):
        if self.hasMinimum() and val < self.minimum: return -1
        if self.hasMaximum() and val > self.maximum: return +1
        return 0


## Mix-in argument helper base class allowing a choice of permitted values.
class ArgChoices:
    ## ArgChoices constructor
    #  @param choices List of choices or a comma-delimited String of choices.
    #  @remarks If choices list is empty, then any value is accepted.
    #  @return ArgChoices instance 
    def __init__( self, choices ):
        self.setChoices( choices )

    ## Generate the default help String.
    #  @return String
    def defaultHelp( self ):
        help = Argument.defaultHelp( self )
        choicesHelp = self.choicesHelp()
        if choicesHelp:
            help += '. ' + choicesHelp
        return help

    ## Getter function for _choices property.
    #  @return List
    def choices( self ):
        return self._choices

    ## Setter function for the _choices property.
    #  @param choices List of choices or a comma-delimited String of choices.
    #  @return None
    def setChoices( self, choices ):
        try:
            self._choices = choices.split( ',' )
        except AttributeError:
            if isinstance( choices, list ):
                self._choices = choices
            else:
                self._choices = []

    ## Help String associated with the choices allowed.
    def choicesHelp( self ):
        if self._choices:
            return 'Possible values: %s' % ( ','.join( [ str( c ) for c in self._choices ] ) )
        else:
            return ''

    ## Check if @em val is one of choices.
    #  @param val The value to check.
    #  @remarks checkChoices always returns @c True if the _choices property is @c [].
    #  @return Boolean
    def checkChoices( self, val ):
        return not self._choices or val in self._choices


## An argument designed to contain an Integer.
class IntegerArg( Argument ):
    ## IntegerArg constructor.
    #  @param help The help String for this argument.
    #  @param name The name of this argument. Defaults to @em default. 
    #  @return IntegerArg instance
    def __init__( self, help, name = 'default' ):
        Argument.__init__( self, help, name )

    ## The Python type of this argument's value.
    #  @return String
    def basicType( self ):
        return 'int'

    ## Retrieve the type associated with this argument as defined by the Production System.
    #  @return String
    def type( self ):
        return 'natural'

    ## Retrieve the metaType associated with this argument as defined by the Production System.
    #  @return String
    def metaType( self ):
        return 'plain'

    ## Return a single character representing the type of the value of the argument according to
    #  Python's String formatting syntax. Eg. An Integer is @c d, a String is @c s, etc.
    #  @return a single character.
    def typeFormat( self ):
        return 'd'

    ## Convert a given @em val to the Python type the argument expects.
    #  @param val Convert the String obtained at the command line to an Python Integer.
    #  @exception TransformArgumentError is raised if the conversion failed.
    def toPython( self, val ):
        try:
            return int( val )
        except ValueError :
            raise TransformArgumentError( '%s=%s is not of type %s' % ( self.name(), repr( val ), self.basicType() ) )


## An argument designed to contain a Float.
class FloatArg( Argument ):
    ## FloatArg constructor.
    #  @param help The help String for this argument.
    #  @param name The name of this argument. Defaults to @em default. 
    #  @return FloatArg instance
    def __init__( self, help, name = 'default' ):
        Argument.__init__( self, help, name )

    ## The Python type of this argument's value.
    #  @return String
    def basicType(self):
        return 'float'

    ## Retrieve the type associated with this argument as defined by the Production System.
    #  @return String
    def type(self):
        return 'float'


    def metaType(self):
        return 'plain'

    
    def typeFormat(self):
        return 'g'


    def toPython(self,val):
        """Turn a command line argument string into an float python object"""
        try: return float(val)
        except ValueError :
            raise TransformArgumentError( '%s=%s is not of type %s' %
                                          (self.name(), repr(val), self.basicType()) )



class StringArg( Argument ):
    """Basic argument type. Any string."""
    def __init__(self,help,name='default'):
        Argument.__init__(self,help,name)


    def basicType(self):
        return 'str'


    def type(self):
        return 'string'


    def metaType(self):
        return 'plain'


    def toPython(self,val):
        try: return str(val+'')
        except TypeError :
            raise TransformArgumentError( '%s=%s is not of type %s' %
                                          (self.name(), repr(val), self.basicType()) )




class StringChoicesArg( ArgChoices, StringArg ):
    """A string from a possible list of strings. Tested before running"""
    def __init__(self,choices,help,name='default',caseSensitive=False):
        ArgChoices.__init__(self, choices)
        StringArg.__init__(self,help,name)
        self._caseSensitive = caseSensitive


    def setDefault(self,default):
        """Add check that the default value is one of the allowed values"""
        if default is not None:
            try: default = self.toPython(default)
            except TransformDefinitionError :
                raise TransformDefinitionError( 'Default value %s=%s is not of type %s' %
                                                (self._name, repr(default), self.basicType()) )
            if not self.checkChoices(default):
                raise TransformDefinitionError( 'Default value %s=%s is not one of %s' %
                                                (self._name, repr(default), self.choices()) )

        Argument.setDefault(self,default)


    def setValue(self,val):
        """If case insensitive, convert value to the correct case (if found in choices)"""
        if not self._caseSensitive:
            #convert value to the correct case
            valUpper = val.upper()
            for c in self.choices():
                if valUpper == c.upper() and val != c:
                    self.logger().warning( 'Changing case of %s to %s', val, c )
                    val = c
                    break
        Argument.setValue(self,val)


    def preRunAction(self):
        """Return boolean indicating if <argVal> is one of choices"""
        val = self.value()
        name = self.name()
        choices = self.choices()
        if not ArgChoices.checkChoices(self,val):
            raise TransformArgumentError( '%s=%r is not one of %s' %
                                          (name, val, ','.join(choices)) )
        self.logger().debug( '%s is in list %s -> OK', repr(val), repr(choices) )

            
#
# end of class StringChoicesArg
#

class BoolArg( ArgChoices, Argument ):
    """Basic argument type. A boolean. Recognised input (string) values:
    'False', 'True' (case insensitive) """
    
    def __init__(self,help,name='default'):
        ArgChoices.__init__(self,['True', 'False'])
        Argument.__init__(self,help,name)


    def basicType(self):
        return 'bool'


    def type(self):
        return 'bool'


    def metaType(self):
        return 'plain'


    def toPython(self,val):
        if isinstance(val, bool): return val is not False
        if isinstance(val, str):
            if val.lower() == 'true':  return True
            if val.lower() == 'false': return False
            raise TransformArgumentError( '%s=%r is not one of %s' %
                                          (self.name(), val, ','.join(self.choices())) )
        else:
            raise TransformArgumentError( '%s=%r is not of type %s' %
                                          (self.name(), val, self.basicType()) )




class FileArg( StringArg ):
    def __init__(self,help,type,name='default', **kwargs ):
        StringArg.__init__(self,help,name)
        self._fileType = type
        self._fileInfo = None
        try:
            self._temporary = kwargs[ 'temporary' ]
        except KeyError:
            self._temporary = False

    def fileInfo( self ):
        return self._fileInfo
    
    def __nonzero__(self):
        """Return whether an input filename is given"""
        return Argument.__nonzero__(self) and self.value() != 'NONE'


    def type(self):
        return 'LFN'


    def setValue(self,value):
        """Convert NONE value to all upper case"""
        if isinstance( value, str ):
            # treat empty string as NONE
            if value == '':
                value = 'NONE'
            else:
                #convert value to the correct case
                valUpper = value.upper()
                if valUpper == 'NONE' and value != 'NONE':
                    self.logger().info( 'Changing case of %s to %s', value, valUpper )
                    value = valUpper
        if value != self.originalValue():
            self.__eventCount = None
            self.__fields = {}
        Argument.setValue(self,value)


    def metaData(self):
        """By default no metadata"""
        return {}

    
    def getGUID(self):
        if not self: return None
        return self._fileType.getGUID(self.value())


    def checkFile(self):
        """check that file exists (possibly with attempt number) and is non-empty"""
        val = self.value()
        if val.startswith( 'LFN:' ):
            self.logger().info( '%s is an LFN. Omitting any local file checks.', val )
            return
#        if not fileutil.exists(val):
#            found = fileutil.exists_suffix_number(val + '.')
#            if not found:
#                raise InputFileError( val, 'not found. Argument %s' % (self.name()), 'TRF_INFILE_NOTFOUND' )
#            if found != val:
#                self.logger().warning('replacing %s with %s' % (val,found) )
#                self.setValue(found)
#                val = found
        if fileutil.getsize(val) == 0:
            raise InputFileError( val, 'empty file. Argument %s' % (self.name()), 'TRF_INFILE_EMPTY' )
                   

    def fileSize(self):
        """File size in bytes"""
        if not self:
            return -1
        return fileutil.getsize(self.value())

    
    def fileType(self):
        if self:
            return self._fileType.type(self.value())
        else:
            return self._fileType.type()


    def fileContents(self):
        if self:
            return self._fileType.contents(self.value())
        else:
            return self._fileType.contents()


    def checkFileType(self):
        """Check if filename ends with .<type>"""
        return self._fileType.checkType(self.value())


    def checkFileContents(self):
        """Check if filename ends with <suffix>"""
        return self._fileType.checkContents(self.value())


    def baseFilename(self):
        """Filename without the path and type"""
        return self._fileType.baseFilename(self.value())


    def bareFilename(self):
        """Filename without the path, the contents and the type."""
        return self._fileType.bareFilename(self.value())



class DataFileArg( FileArg ):
    def __init__(self,help,type,name='default', **kwargs ):
        FileArg.__init__(self,help,type,name, **kwargs) #temporary
        self.__eventCount = None
        # interpreted filename fields as used in the MC production system
        self.__fields = {}
        try:
            self._intermediate = kwargs[ 'intermediate' ]
        except KeyError:
            self._intermediate = False

    @staticmethod
    def extractDataset( filename, omitFromName = None ):
        """Extracts dataset from filename and return a tuple of the dataset name 
        and the filename. If omitFilename is True, only the dataset is returned."""
        if omitFromName is None:
            omitFromName = True
        originalFN = filename
        dirName = os.path.dirname( filename )
        filename = os.path.basename( filename )
        try:
            hashPos = filename.index( '#' )
        except ValueError:
            return "", originalFN
        singleHash = True
        # Check if the format used includes the '##'.
        try:
            if filename[ hashPos + 1 ] == '#': # double hash encountered
                singleHash = False
        except IndexError: # problem with format of filename. filename ends with a single '#'!
            hashPos = -1
            print( "Error trying to extract dataset from %s." % filename )
        dataset = filename[ 0 : hashPos ]
        if omitFromName:
            dsPrefix = ''
        else:
            dsPrefix = dataset + '#'
        if singleHash:
            # 'myCoolDataset#myCoolDataset._0001.crp'
            if dirName:
                fname = os.sep.join( [ dirName, dsPrefix + filename[ hashPos + 1 : ] ] )
            else:
                fname = dsPrefix + filename[ hashPos + 1 : ]
            return dataset, fname
        else: # double hash encountered
            # 'myCoolDataset##._0001.crp'
            try:
                fname = dsPrefix + dataset + filename[ hashPos + 2 : ]
            except IndexError: # problem with format of filename. filename ends with a double '#'!
                print( "Error trying to extract filename from %s." % filename )
            else:
                if dirName:
                    fname = os.sep.join( [ dirName, fname ] )
                return dataset, fname
        return dataset, originalFN

    #
    # Member functions from Argument (overriding them)
    #
    def preRunAction(self):
        """Set pool message level"""
        PoolDataFile.setMessageLevel(PoolDataFile.defaultMessageLevel,self.logger())

    #
    # Extra member functions
    #
    def eventCount(self):
        if self.__eventCount is None and self:
            self.logger().info("Counting events of %s...", self.originalValue())
            start = time.time()
            self.__eventCount = self._fileType.eventCount( self )
            if self.__eventCount is not None:
                duration = time.time() - start
                megaBytes = self.fileSize() / float( 1024**2 )
                if duration > 0.0:
                    eventsPerSec = self.__eventCount / duration
                    megasPerSec  = megaBytes / duration
                else:
                    eventsPerSec = 0.0
                    megasPerSec  = 0.0
                self.logger().info("Counted %d events (%.1f MB) in %.1f seconds --> %.1f events/s and %.1f MB/s",
                                   self.__eventCount, megaBytes, duration, eventsPerSec, megasPerSec)
            else:
                self.logger().info("No event count available for %s", self.originalValue() )
        return self.__eventCount


class InputFileArg( FileArg ):
    """Input file. Existence of file is checked before run"""
    def __init__( self, help, type, name = 'default', **kwargs ):
        FileArg.__init__( self, help, type, name, **kwargs )
        

    def metaType(self):
        return 'inputLFN'


    def preRunAction(self):
        """Check that the file exists, and is readable"""
        if not self: return
        self.checkFile()
        self.logger().debug( 'Inputfile %s is usable -> OK', self.originalValue() )
        

    def eventCount(self):
        return None

    
    def metaData(self):
        """No metadata"""
        return {}



class InputDataFileArg( DataFileArg ):
    """List of input data files. Existence and readability of all files is checked before run"""
    def __init__( self, help, type, name = 'default', **kwargs ):
        DataFileArg.__init__( self, help, type, name, **kwargs )
        self._fileInfo = {}
        
    def __nonzero__(self):
        """Return whether an input filename list is given"""
        return Argument.__nonzero__( self ) and len( self._value ) > 0
        
    def basicType(self):
        return 'list'

    def type(self):
        return 'LFNlist'

    def metaType(self):
        return 'inputLFNlist'

    def preRunAction(self):
        """Execute checkFile routines"""
        if not self: return
        DataFileArg.preRunAction(self)
        self.checkFile()
        if not self._temporary:
            self.prepareFileInfo()

    def checkFile(self):
        # Previously, test for existence done here but this is now done when the filename is first stored in the object. See trfutil.py::getCorrectedFilename().
        return

    def value( self, pos = None, getDataset = None, omitFromName = None ):
        """Attempt to extract a dataset name from the value provided. This is a Tier0 request.
        pos can take integer values 0-n to refer to specific values in its list.
        If pos is set to None, the return value will be entire list is returned. 
        If getDataset is True, (dataset,filename) tuple is returned."""
        if omitFromName is None:
            omitFromName = True
        if getDataset is None:
            getDataset = False
        if pos is None:
            if getDataset:
                return [ DataFileArg.extractDataset( x, omitFromName ) for x in self._value ] # return a list of (dataset, filename) tuples.
            return [ DataFileArg.extractDataset( x, omitFromName )[1] for x in self._value ] # return a list of filenames.
        elif pos >= 0 and pos < len( self._value ):
            if getDataset:
                return DataFileArg.extractDataset( self._value[ pos ], omitFromName ) # return (dataset, filename) tuple
            return DataFileArg.extractDataset( self._value[ pos ], omitFromName )[ 1 ] # return just the filename
        else:
            raise IndexError( "Could not get dataset/filename tuple. The index provided %s is out of range." % pos )

    def setValue( self, value ):
        if value != self.originalValue():
            self.__eventCount = None
            self.__fields = {}
        FileArg.setValue( self, value )

    def toPython(self,valIn):
        """If valIn is a list, check if the names are in the dataset format.
        If so, convert to string for further processing later else just return
        valIn as it is. Strings of space or comma separated entries will be 
        converted to lists accordingly. There is also support for parallel-
        vector notation."""
        if isinstance( valIn, list ):
            valInStr = valIn.__str__()
            # check if dataset prefix or parallel vector format is found in the list items.
            if re.search("[#[]", valInStr ):
                # convert list to a string and continue processing to extract dataset.
                valIn = valInStr[1:-1].replace( "'", "" ).replace(" ","")
            else:
                return valIn
        # make a list of python types out of the strings
        if isinstance( valIn, str ):
            if valIn == 'NONE': return []
            # parallel vector notation support
            try:
                return expandStringToList( valIn )
            except Exception as msg:
                raise TransformDefinitionError( '%s=%s: %s' % ( self.name(), valIn, msg ) )
        else:
            # if we get here, there is problem
            raise TransformDefinitionError( '%s=%s: value is not a list or a string' % (self.name(),valIn) )

    def prepareFileInfo(self):
        fInfos = self._fileInfo
        fInfoTemp = {}
        # # ensure GUID consistency (determine once per filename)
        for ds, filename in self.value( getDataset = True ):
            mData = self._fileType.getMetaData( filename )
            mData.update( { 'dataset' : ds } )
            try:
                guid = copy.copy( fInfos[ filename ].guid() )
                if guid is None:
                    raise Exception
            except ( KeyError, Exception ):
                # create new GUID
                fInfoTemp[ filename ] = FileInfo( filename, self._fileType.getGUID( filename ), mData )
            else:
                # add the metadata
                fInfoTemp[ filename ] = FileInfo( filename, guid, mData )
        self._fileInfo = fInfoTemp
#        events = self.eventCount()
#        if events is not None:
#            fInfo.addMetaData( { 'events' : events } )

    def fileSize( self, filename = None ):
        """File size in bytes of all files in the list combined"""
        if not self: return -1
        if filename is None:            
            totalSize = 0
            for f in self.value():
                totalSize += fileutil.getsize( f )
            return totalSize
        if filename in self.value():
            return fileutil.getsize( filename )
        return -1
        

    def getGUID(self):
        if not self: return None
        if self._fileInfo:
            return [ x.guid() for x in self._fileInfo ]
        return [ self._fileType.getGUID(f) for f in self.value() ]


    def fileType(self):
        """File type of the first file in the list"""
        if self:
            try:
                val = self.value()[0]
            except Exception:
                pass
            else:
                return self._fileType.type( val )
        return self._fileType.type()


    def fileContents(self):
        """File contents of the first file in the list"""
        if self:
            try:
                val = self.value()[0]
            except Exception:
                pass
            else:
                return self._fileType.contents( val )
        return self._fileType.contents()



    def checkFileType(self):
        """Check if all filenames have expected type"""
        for f in self.value():
            if not self._fileType.checkType(f):
                return False
        return True


    def checkFileContents(self):
        """Check if all filenames have expected contents"""
        for f in self.value():
            if not self._fileType.checkContents(f):
                return False
        return True

    def baseFilename(self):
        """First filename without the path and type"""
        try:
            return self._fileType.baseFilename( self.value()[0] )
        except Exception:
            return ""

    def bareFilename(self):
        """First filename without the path, the contents and the type."""
        try:
            return self._fileType.bareFilename( self.value()[0] )
        except Exception:
            return ""


class InputTarFileArg( InputFileArg ):
    def __init__( self, help, destdir = os.curdir, name = 'default', **kwargs ): #temporary = True ):
        InputFileArg.__init__( self, help, FileType( type = 'tar|tar.gz|tgz' ), name, **kwargs )
        self._filelist = []
        self._destdir = destdir


    def setValue(self,value):
        self._filelist = []
        super(InputTarFileArg,self).setValue(value)
        

    def filelist(self,pattern=None):
        """Return a list of filenames in archive. <pattern> is an optional regular expression to match the filenames.
        This list contains the filename paths as in the archive, not including the destination directory set in the constructor."""
        if not self._filelist:
            if not self:
                raise TransformDefinitionError( "%s.filelist() called before value is set" % self.name() )
            filename = self.value()
            cmd = 'tar -t'
            if self.isZipped(): cmd += ' -z'
            cmd += ' -f %s' % filename
            self.logger().info(cmd)
            p = subprocess.Popen(cmd,bufsize=1,shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
            while p.poll() is None:
                line = p.stdout.readline().strip()
                if not line: continue
                self._filelist.append(line)
          # hack for slving bug on poll (Python 2.4?)      
            p.wait()
            while True:
                line = p.stdout.readline().strip()
                if not line: break
                self._filelist.append(line)

            if p.returncode:
                self._filelist.clear()
                raise TransformArgumentError( "Argument %s: problem reading filelist from archive %s" % (self.name(), filename) )
            
##             tar = tarfile.open(self.value())
##             self._filelist = tar.getnames()
##             tar.close()
            
        if pattern is None:
            return self._filelist
        else:
            return [ s for s in self._filelist if re.match( pattern, s ) ]    


    def isZipped(self):
        if not self: return False
        val = self.value()
        return val.endswith('.gz') or val.endswith('.tgz')


    def extract(self):
        """Extract all files from the archive and check their presence"""
        # check that we have a value
        if not self:
            raise TransformDefinitionError( "Argument %s extract() called before value is set" % self.name() )
        # create and goto subdir if needed
        curdir = None
        destdir = self._destdir
        filename = self.value()
        if destdir != os.curdir:
            curdir = os.getcwd()
            if not os.path.exists(destdir):
                os.mkdir(destdir)
            elif not os.path.isdir(destdir):
                raise  TransformDefinitionError( "Argument %s: requested destination directory '%s' is not a directory" % (self.name(),destdir) )
            os.chdir(destdir)
        try:
            # do the extraction while ignoring the original modification time
            cmd = 'tar -mxv'
            if self.isZipped(): cmd += ' -z'
            cmd += ' -f %s' % filename
            self.logger().info(cmd)
            p = subprocess.Popen(cmd,shell=True)
            status = p.wait()
            if status:
                raise TransformArgumentError( "Argument %s: problem extracting archive %s" % (self.name(),filename) )
            # check that all files are extracted
            for f in self.filelist():
                if not os.path.exists( f ):
                    raise TransformArgumentError( "Argument %s: failed to extract file %s from archive %s" % (self.name(),f,filename) )

        finally:
            # go back to original dir, whatever happens
            if curdir: os.chdir(curdir)

        
    def destinationDir(self):
        return self._destdir


    def preRunAction(self):
        """Check presence of file and untar/zip it"""
        if not self: return
        InputFileArg.preRunAction(self)
        self.extract()



class InputTarFileAndSetupArg( InputTarFileArg ):
    def __init__( self, help, name, setupfile, envVars = None, destdir = os.curdir, **kwargs ): #temporary = True ):
        """Input tarfile is untarred and a setup script is executed during preRunActions.
        <setupfile>: name of the setup file. If the extension is .py, it will be executed
                     as pyton (with execfile). Otherwise it will be sourced as a shell script.
                     Any environment setup in the shell script is imported into the python
                     environment (os.environ), as far as it matches <envVars>.
        This full path to the file is searched for
                   in the tarball: the file found in the highest directory will be taken.
                   It can contain the usual (unix) shell wildcards.
        <envVars>: a regular expression selecting the environment variables to export from
                   the shell setup script into the python running environment. None (default) means all.
                   In the case of a python script, all environment variables (when added to os.environ)
                   are imported, and the value of <envVars> is ignored.
        <destdir>: the destination directory for the untarred files (default: current directory)
        """
        InputTarFileArg.__init__( self, help, destdir, name, **kwargs )
        self.__setupfile = setupfile


    def untarAndSetup(self):
        """Untar the tarball and run the setup file"""
        # check that file is there and non-empty
        self.checkFile()
        # untar archive
        self.extract()
        # run the setup script
        setup = self.findSetupFile()
        if setup is None: # Failure to find it
            raise TransformArgumentError( "Could not find setup script %s in %s=%s" % (self.__setupfile, self.name(), self.originalValue()) )
        # if no setup file is given, don't run it
        if setup == '': return
        fullsetup = os.path.join(self.destinationDir(),setup)
        if not os.path.exists(fullsetup):
            raise TransformArgumentError( "Setup script %s not found after untarring %s=%s" % (self.__setupfile, self.name(), self.originalValue()) )
        self.logger().info("Executing setup file %s", fullsetup)
        setupdir,setupbase = os.path.split(fullsetup)
        # go to the directory of the setup script
        pwd = os.getcwd()
        if setupdir and setupdir != os.curdir: os.chdir(setupdir)
        if fullsetup.endswith('.py'):
            from past.builtins import execfile
            # avoid pollution of global namespace
            env = {}
            execfile( setupbase,env )
        else:
            envutil.source_setup( setupbase )
        # go back to original directory
        os.chdir(pwd)
        

    def preRunAction(self):
        """Untar and run setup file. Raises an exception if setup file is not found."""
        if not self: return
        self.untarAndSetup()


    def findSetupFile(self):
        """Return the full sub-path to the setup file inside the tarball.
        Does not include the destination directory.
        Return '' (empty string) if a setup file is not specified.
        Return None if the specified setup file is not found in the tarball."""
        setup = self.__setupfile
        if not setup: return ''
        found = None
        foundLevel = 99999
        setupdir,setupbase = os.path.split(setup)
        if setupdir == os.curdir: setupdir = ''
        # get the number of sub-directories to match
        if setupdir:
            dircount = setupdir.count(os.sep) + 1
        else:
            dircount = 0

        for f in self.filelist():
            dirsplit = f.split(os.sep)
            base = dirsplit[-1]
            matchdir = os.sep.join(dirsplit[-dircount-1:-1])
            fm=os.path.join(matchdir,base)
            if fnmatch.fnmatch(fm,setup):
                level = len(dirsplit) - 1
                if level < foundLevel:
                    foundLevel = level
                    found = f

        return found



class OutputFileArg( DataFileArg ):
    """Baseclass for all output file classes. File is deleted before run (if needed). Existence of file is tested after run"""
    def __init__( self, help, type, name = 'default', temporary = False, intermediate = False, **validationDict):
        DataFileArg.__init__( self, help, type, name, temporary = temporary, intermediate = intermediate )
        self.__validationDict = validationDict
    
    def metaType(self):
        return 'outputLFN'
    
    def preRunAction(self):
        """Remove output file before running"""
        DataFileArg.preRunAction(self)
        val = self.value()
        self._fileInfo = None
        if not self:
            self.logger().debug( 'No output file expected. Nothing to be done.' )
        elif fileutil.exists(val):
            raise OutputFileError( val, 'already exists. Please use a different name. Argument %s' % self.name() )
#            try:
#                os.remove(val)
#            except:
#                raise OutputFileError( val, 'could not be removed before running. Argument %s' % self.name() )
#            else:
#                mess = 'Removed output file %s -> OK' % (val)
    
    def validateFile( self ):
        if self._temporary or self._intermediate:
            self.logger().info( '%s is a temporary/intermediate file. It will not be validated.', self.name() )
            return
        try:
            self._fileType.validateFile( self, **self.__validationDict )
#        except TransformValidationError:
#            self.logger().info( 'Retrying validation of %s in %d seconds.' % ( self.name(), TRF_SETTING[ 'validationSleepInterval' ] ) )
#            time.sleep( TRF_SETTING[ 'validationSleepInterval' ] )
#            self._fileType.validateFile( self, **self.__validationDict )
        except AttributeError:# file type does not implement validateFile()
            pass
    
    def postRunAction(self):
        """Check that output file exists."""
        if not self:
            self.logger().debug('No output file expected. Nothing to be done.')
        else:
            self.validateFile()
            if self._temporary: return
            self.prepareFileInfo()
    
    def getDataForAmi(self, alltheData):
        if not self: 
            return
        if self._temporary or self._intermediate: 
            # print ('basic_trfarg.py file is temporary or intermediate')
            return
        if isinstance(self._fileType, AODFile) or isinstance(self._fileType,ESDFile) or isinstance(self._fileType,DPDFile) or isinstance(self._fileType,EvgenFile) or isinstance(self._fileType,HitsFile):
            try:
                FT=self._fileInfo.metaData('fileType')
                if FT is None: FT='unset'
                necoll=self._fileType.writeSize(self)
                if necoll is not None:
                    # returns filetype, number_of_events and tuple_of_sizes
                    alltheData.append(FT.upper())
                    alltheData.append(necoll[0])
                    alltheData.append(necoll[1])
                    return
            except Exception:
                print ("basic_trfarg.py  exception caught:", sys.exc_type, ":", sys.exc_value)
        else:
            print ('basic_trfarg.py  Not checking object sizes for this file type')
    
    def prepareFileInfo(self):
        """Populate _fileInfo with metadata and guid."""
        ds, filename = self.value( getDataset = True )
        mData = self._fileType.getMetaData( filename )
        mData.update( { 'dataset' : ds,
                        'size' : self.fileSize(),
                        'events' : self.eventCount() } )
        try:
            guid = copy.copy( self._fileInfo.guid() )
            if guid is None:
                raise Exception
        except ( KeyError, Exception ):
            # create new GUID
            self._fileInfo = FileInfo( filename, self._fileType.getGUID( filename ), mData )
        else:    
            self._fileInfo = FileInfo( filename, guid, mData )
    
    def value( self, getDataset = None, omitFromName = None ):
        """Attempt to extract a dataset name from the value provided. This is a Tier0 request."""
        if omitFromName is None:
            omitFromName = True
        if getDataset:
            return DataFileArg.extractDataset( self._value, omitFromName ) # return (dataset, filename) tuple
        return DataFileArg.extractDataset( self._value, omitFromName )[ 1 ] # return filename
    
    def metaData(self):
        """output filenames are not added to the metadata"""
        return {}
    


class OutputDataFileArg( OutputFileArg ):
    """Class for input POOL data files"""
    def __init__( self, help, type, name = 'default', temporary = False, intermediate = False, **validationDict ):
        OutputFileArg.__init__( self, help, type, name, temporary, intermediate, **validationDict )


if __name__ == '__main__':
    a=InputDataFileArg("default",BSFile(),"default")

    x = "abc.AOD.*.root"
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))

    x = "abc.AOD.0[1-7].root"
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))

    x = "abc.AOD._1.root"
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))
    
    x = "abc.AOD._1.root,abc.AOD._2.root,abc.AOD._3.root"
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))

    x = ["abc.AOD._1.root", "abc.AOD._2.root", "abc.AOD._3.root"]
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))

    x = "abc##.AOD._1.root"
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))

    x = "abc#abc.AOD._1.root"
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))
    
    x = "abc.AOD._1.root,abc#abc.AOD._2.root,abc##.AOD._3.root"
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))

    x = "abc.AOD.[A,B,C]._[1,2,3].[X,Y,Z].root"
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))

    x = ["abc.AOD._[1,2,3].root"]
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))

    x = "abc.AOD._[10,11,12].root,abc#abc.AOD._[21,22,23].root,abc##.AOD._[31,32,33].root"
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))

    x = ["abc.AOD._[10,11,12].root","abc#abc.AOD._[21,22,23].root","abc##.AOD._[31,32,33].root"]
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))

    x = '/afs/cern.ch/user/j/jboyd/gencomm/GetInterestingEvents/BSfiles/87764/evt.194586._0001.data,/afs/cern.ch/user/j/jboyd/gencomm/GetInterestingEvents/BSfiles/87764/DSTEST##.evt.194728._[0001,0002,0003].data,/afs/cern.ch/user/j/jboyd/gencomm/GetInterestingEvents/BSfiles/87764/DSTEST#DSTEST.evt.216134._0001.data'
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))

    x = ["abc##.AOD._[SFO_1,SFO_4]._0001.[2,3]", "abc#abc.AOD._[SFO_1,SFO_4].[3,4]"]
    a.setValue( x )
    print ("\nInput: %s" % x)
    for ds,fn in a.value( getDataset = True ):
        print ("dataset: %s, filename: %s" % (ds,fn))



