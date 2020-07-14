# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @package AtlasErrorCodes
#
#  @brief This module contains all the error-related classes and functions that facilitate
#  the logistics of error management.
#  @author $LastChangedBy: jchapman $
#  @version $Rev: 355335 $
#  @date $Date: 2011-03-30 13:05:33 +0200 (Wed, 30 Mar 2011) $

from __future__ import with_statement, print_function
import os,re,sys,traceback
from .envutil import find_file_env
from .xmlutil import XMLNode

__all__ = [ 'AtlasErrorCodeException', 'ErrorInfo', 'ErrorCategory' ]

## Standard way to denote a INFO level error severity. 
INFO='INFO'
## Standard way to denote a WARNING level error severity. 
WARNING='WARNING'
## Standard way to denote a ERROR level error severity.
ERROR='ERROR'
## Standard way to denote a FATAL level error severity.
FATAL='FATAL'
## Standard way to denote the absence of an error.
NO_ERROR = None

## Default filename containing the error categories to be used if one is not provided.
defaultCategoryFile = 'atlas_error_categories.db'
## Default filename containing the error patterns to be used if one is not provided.
defaultErrorPatternFile = 'atlas_error_patterns.db'
## Default filename containing the error filters to be used if one is not provided.
defaultIgnorePatternFile = 'atlas_error_ignore.db'

## Error category dictionary with @em code as key
__categoryCodeDict = {}
## Error category dictionary with @em acronym as key
__categoryAcronymDict = {}
## Dictionary storing lists of ErrorPattern instances with the Atlas software @em release as key.
__errorPatterns  = {}
## Dictionary storing lists of error filter regular expressions with Atlas software @em release as key.
__ignorePatterns = {}


## Simple Exception class extension to highlight the fact that an exception
#  occurred in the AtlasErrorCodes package.
class AtlasErrorCodeException( Exception ):
    ## Constructor for the AtlasErrorCodeException class.
    #  @param message A description of the nature of the exception.
    #  @return AtlasErrorCodeException instance
    def __init__( self, message ):
        Exception.__init__( self, message )
        

## Container class for information describing an error category.
class ErrorCategory( object ):
    ## Constructor for the ErrorCategory class.
    #  @param code The code associated with the particular error category.
    #  @param acronym The acronym associated with the particular error category.
    #  @param description The textual description of the particular error category.
    #  @param count The number of errors found that are of this particular error category. Defaults to @c 1.
    #  @return ErrorCategory instance
    def __init__( self, code, acronym, description, count = None ):
        if count is None:
            count = 1
        ## The code associated with the particular error category.
        self.code = code
        ## The acronym associated with the particular error category.
        self.acronym = acronym
        ## The textual description of the particular error category.
        self.description = description
        ## The number of errors found that are of this particular error category.
        self.count = count

    ## Pretty print of the error category.
    #  @return String
    def __str__( self ):
        s = '%s,\t%s,\t%s' % ( self.acronym, self.code, self.description )
        if self.count > 1:
            s += ' (%s times)' % self.count
        return s

    ## Generate an XML node containing all the error category's information.
    #  @return xmlutil.XMLNode instance 
    def xmlNode( self ):
        return XMLNode( "errorcategory", self.description ).setAttributes( code = self.code, acronym = self.acronym, count = str( self.count ) )

    ## Retrieve the string representation of the xmlutil.XMLNode instance associated with this ErrorCategory instance.
    #  @return String 
    def getXML( self ):
        return self.xmlNode().getXML()
        
        
## Special case: The OK category describes a successful transform execution and omits/delays
#  the need to load other error categories from disk.
OK_CATEGORY = ErrorCategory( 0, 'OK', 'Finished successfully' )
## Special case: A category for an error that is not recognised.
#  The code, acronym and description of this unknown category is used to to supplement
#  the creation of ErrorCategory instances where the code and/or acronym is not recognised.
UNKNOWN_CATEGORY = ErrorCategory( 999999, 'UNKNOWN', 'Unknown ErrorCode or Acronym' )
## Special case: This category describes a permissible error.
#  @see JobReport.GPickleContents_new() and JobReport.GPickleContents_old().
NEEDCHECK = ErrorCategory( 1, 'NEEDCHECK', 'Successful but warrants further investigation' )


## Container class for information describing an error pattern
class ErrorPattern( object ):
    ## Constructor for the ErrorPattern class.
    #  @param pattern The regular expression that is able to match an error.
    #  @param acronym The acronym associated with the particular error category.
    #  @param severity The severity level of the error in question. Defaults to @c AtlasErrorCodes.ERROR.
    #  @return ErrorPattern instance
    def __init__( self, pattern, acronym, severity = None ):
        if severity is None:
            severity = ERROR
        self.severity = severity
        self.patternRE = re.compile( pattern )
        self.acronym = acronym

    ## Check if a given string matches the pattern.
    #  @return re.match object if a match is successful, and @c None otherwise.
    def match(self,value):
        return self.patternRE.match(value)

## Container class for information describing an error.
class ErrorInfo( object ):
    ## Properties associated with the class. These properties are automatically set to @c None
    #  if not explicitly declared in the constructor.
    __slots__ = ( 'message', 'code', 'acronym', 'category', 'severity', 'who', 'producer', 'stage', 'events', 'diagnosis', 'backtrace', 'count', 'exitstatus', 'stdout', 'stderr' )
    ## List of properties to omit when ErrorInfo.__str__() is called. 
    __dontPrint = [ 'count', 'category', 'code', 'acronym' ]
    ## List of properties to omit when generating a xmlutil.XMLNode instance when ErrorInfo.getXML() is called. 
    __notInXML  = [ 'count', 'category' ]
    ## List of properties to omit when making comparisons between ErrorInfo instances.
    __skipInComparison = [ 'count', 'events' ]

    ## @brief Constructor for the ErrorInfo class.
    #  @details This contructor does not require any arguments to create a valid ErrorInfo instance.
    #  However, it will accept the following keyword arguments to supplement its properties:
    #  @li @b code    : Error code.
    #  @li @b acronym : Error acronym.
    #  @li @b severity : @em AtlasErrorCodes.FATAL, @em AtlasErrorCodes.ERROR, @em AtlasErrorCodes.WARNING or
    #  @c None (if the error code is @em 0).
    #  @li @b who     : Name of the Algorithm, Tool, Service, JobTransform, etc. that generated the error.
    #  @li @b producer: Name of the program that produced the error (e.g. @c athena.py, the transform's name).
    #  @li @b stage   : Running stage/step of the executable (e.g. in @c athena.py: @em initialise,
    #  @em execute and @em finalise).
    #  @li @b events  : Where available, it will be the list of event numbers where the error occurred.
    #  @li @b message : Additional String description of the error.
    #  @li @b diagnosis : Usually populated by the trferr.errorHandler during the final stages of the transform execution.
    #  It provides some information on the causes of the error.
    #  @li @b backtrace : By default, the backtrace is a list of tuples in the same format as
    #  <tt>traceback.extract_tb(sys.exc_info()[2])</tt>.
    #  @li @b exitstatus: A 2-tuple containing the exit status code of the sub-process and
    #  its interpretation (where available).
    #  @li @b stdout   : The stdout of the sub-process.
    #  @li @b stderr   : The stderr of the sub-process.
    #  @return ErrorInfo instance
    def __init__( self, **kwargs ):
        # Initialise all property values to the default value of @c None.
        self.clear()
        # Overwrite default values with those provided in the constructor.
        for name,value in kwargs.items():
            setattr( self, name, value )
        # set more meaningful defaults for certain properties
        if not self.backtrace:
            self.backtrace = self.defaultBacktrace()
        if self.count is None:
            self.count = 1
        cat = kwargs.get( 'acronym' ) or kwargs.get( 'code' )
        if cat is not None:
            self.setCategory( cat )

    ## Pretty print information contained in the object. Properties specified in ErrorInfo.__dontPrint will be omitted.
    #  @return String (multi-line)
    def __str__( self ):
        acronym = self.acronym
        description = None
        if self.category:
            if not acronym: acronym = self.category.acronym
            if acronym != 'TRF_UNKNOWN':
                description = self.category.description
        atts = {}
        count = self.count
        if count > 1: atts[ 'count' ] = count
        header = 'ErrorInfo'
        if atts:
            header += ' ('
            for n,v in atts.items():
                header += '%s=%s ' % (n,v)
            # overwrite last space
            header = header[ :-1 ] + ')'
        me = [ header ]
        codeLine = ''
        if acronym:
            codeLine += 'acronym=%s' % acronym
        if self.code is not None:
            codeLine += ' (%s)' % self.code
            if description: codeLine += ' ' + description
        if codeLine: me += [ codeLine ]
        # add the other (non-None) variable arguments
        for arg in ErrorInfo.__slots__:
            # don't print some
            if arg in ErrorInfo.__dontPrint: continue
            valStr = self.valueString( arg )
            if valStr:
                indent = ' ' * ( len( arg ) + 1 )
                valStr = valStr.replace( os.linesep, os.linesep + indent ).strip()
                me.append( '%s=%s' % ( arg, valStr ) )
        sep = os.linesep + '  '
        return sep.join( me )

    ## Part of ErrorInfo instance comparison mechanism.
    #  Properties in ErrorInfo.__skipInComparison will not be compared.
    #  @param other The @em other ErrorInfo instance.
    #  @return Boolean
    def __eq__( self, other ):
        for att in ErrorInfo.__slots__:
            if att in ErrorInfo.__skipInComparison: continue
            if getattr( self, att ) != getattr( other, att ): return False
        return True

    ## Part of ErrorInfo instance comparison mechanism. The negated result of ErrorInfo.__eq__().
    #  @param other The @em other ErrorInfo instance.
    #  @return Boolean
    def __ne__( self, other ):
        return not self.__eq__( other )

    ## Convenience function to add ErrorInfo instances together. This allows the @c += syntax to be used.
    #  @param other The @em other ErrorInfo instance.
    #  @return self
    def __iadd__( self, other ):
        self.count += other.count
        self.addEvents( other.events )
        return self

    ## Formats the value of a requested property.
    #  @param name The @em name of the property to retrieve.
    #  @return String (multi-line)
    def valueString( self, name ):
        value = getattr( self, name )
        if value is None: return ''
        valType = type( value ).__name__
        if name == 'backtrace':
            # make nice backtrace format
            valStr = ''.join( traceback.format_list( value ) )
        elif valType == 'list' or valType == 'tuple':
            valStr = ','.join( [ str( s ) for s in value ] )
        elif valType == 'dict':
            valStr = os.linesep.join( [ '%s=%s' % ( n, v ) for n, v in value.items() ] )
        else:
            valStr = str( value )
        return valStr

    ## Initialise all the properties as defined in ErrorInfo.__slots__ to @c None.
    #  @return None
    def clear( self ):
        for arg in ErrorInfo.__slots__:
            setattr( self, arg, None )

    ## Retrieve the default backtrace which excludes @c AthenaCommon/Include.py from the stack trace.
    #  @return List of String (or @c None)
    def defaultBacktrace( self ): 
        exc_info =  sys.exc_info()[ 2 ]
        if not exc_info: return None
        tb = traceback.extract_tb( exc_info )
        if not tb: return None
        short_tb = []
        for frame_info in tb:
            if 'AthenaCommon/Include.py' not in frame_info[ 0 ]:
                short_tb.append( frame_info )
        return short_tb

    ## Set the error category property.
    #  @param cat A ErrorCategory instance, acronym or code may be used.
    #  @return None 
    def setCategory( self, cat ):
        if isinstance( cat, str ) or isinstance( cat, int ):
            cat = getCategory( cat )
        self.category = cat
        self.code = cat.code
        self.acronym = cat.acronym
        # no backtrace needed if error code is 0 (OK)
        if self.code == 0:
            self.backtrace = None
            self.severity = None
        elif self.severity is None:
            self.severity = FATAL

    ## Errors occur when certain events are being processed.
    #  The function adds the given event number to the set of events that gave rise to this error.
    #  @param events A single event number or a list of event numbers permissible.
    #  @return None
    def addEvents( self, events ):
        if events is None:
            return
        try:
            self.events.update( events )
        except AttributeError: # self.events is None
            try:
                self.events = set( events )
            except TypeError:
                self.events = set( [ events ] )
        except TypeError:
            self.events.add( events )

    ## Generate an XML node containing all the error's properties.
    #  Properties listed in ErrorInfo.__notInXML are ignored.
    #  @return xmlutil.XMLNode instance 
    def xmlNode( self ):
        node = XMLNode( 'ErrorInfo' )
        # set attibutes
        node.setAttribute( 'count', self.count )
        for att in ErrorInfo.__slots__:
            # don't print some
            if att in ErrorInfo.__notInXML: continue
            valStr = self.valueString( att ).strip()
            if valStr:
                node.addContents( XMLNode( att, valStr ) )
        return node

    ## Retrieve the string representation of the xmlutil.XMLNode instance associated with this ErrorInfo instance.
    #  @return String 
    def getXML(self):
        return self.xmlNode().getXML()

## Retrieve error patterns for a given release.
#  @param release Atlas release number which is the key used in the AtlasErrorCodes.__errorPatterns dictionary.
#  @exception KeyError Raised when the given @em release is not a String or a compiled regular expression.
#  @return List of ErrorPattern instances or @c None
def getErrorPatterns( release ):
    global __errorPatterns
    if isinstance(release, str):
        total_pats = [] # list of recognised error patterns
        for rel, pats in __errorPatterns.items():
            if rel.match( release ):
                total_pats += pats
        return total_pats
    elif isinstance(release, re.Pattern):
        for rel, pats in __errorPatterns.items():
            if rel.pattern == release.pattern:
                return pats
    else:
        raise KeyError( 'getErrorPatterns() takes either a string or a compiled regular expression. Got an %s instead.' % type(release).__name__ )
    return None

## Retrieve error filter patterns for a given release.
#  @param release Atlas release number which is the key used in the AtlasErrorCodes.__ignorePatterns dictionary.
#  @exception KeyError Raised when the given @em release is not a String or a compiled regular expression.
#  @return List of compiled regular expression isntances or @c None
def getIgnorePatterns( release ):
    global __ignorePatterns
    if isinstance(release, str):
        total_pats = [] # list of possible patterns to ignore
        for rel, pats in __ignorePatterns.items():
            if rel.match( release ): 
                total_pats += pats  
        return total_pats
    elif isinstance(release, re.Pattern):
        for rel, pats in __ignorePatterns.items():
            if rel.pattern == release.pattern:
                return pats
    else:
        raise KeyError( 'getIgnorePatterns() takes either a string or a compiled regular expression. Got an %s instead.' % type(release).__name__ )
    return None

## Add an ErrorPattern instance to a specific release.
#  @param release Atlas release number which is the key used in the AtlasErrorCodes.__errorPatterns dictionary.
#  @param pat The ErrorPattern instance to add.
#  @return None
def addErrorPattern( release, pat ):
    global __errorPatterns
    releaseRE = re.compile( release )
    patList = getErrorPatterns( releaseRE )
    if patList:
        patList.append( pat )
    else:
        __errorPatterns[ releaseRE ] = [ pat ]

## Add a compiled regular expression of an error filter to a specific release.
#  @param release Atlas release number which is the key used in the AtlasErrorCodes.__ignorePatterns dictionary.
#  @param pat The compiled regular expression instance to add.
#  @return None
def addIgnorePattern( release, pat ):
    global __ignorePatterns
    releaseRE = re.compile( release )
    patList = getIgnorePatterns( releaseRE )
    if patList:  
        patList.append( pat )
    else:
        __ignorePatterns[ releaseRE ] = [ pat ]

## Add a new ErrorCategory entry to the global dictionaries.
#  @param code New error category code.
#  @param acronym New error acronym.
#  @param description New error description.
#  @return Boolean
def addCategory( code, acronym, description ):
    global __categoryCodeDict, __categoryAcronymDict
    # check that error code is unique
    cat = __categoryCodeDict.get( code )
    if cat:
        print ("ERROR: Error category code %s already exists with acronym %s (new acronym: %s)" % ( code, cat.acronym, acronym ))
        return False
    # check that error acronym is unique
    cat = __categoryAcronymDict.get( acronym )
    if cat:
        print ("ERROR: Error category acronym %s already exists with code %d (new code: %d)" % ( acronym, cat.code, code ))
        return False
    # all OK. Now add it.
    cat = ErrorCategory( code, acronym, description )
    __categoryCodeDict[ code ] = cat
    __categoryAcronymDict[ acronym ] = cat
    return True

## Get the full error category information by providing an error code or an error acronym.
#  @param codeOrAcronym An Integer error code or a String acronym is accepted.
#  @exception AtlasErrorCodeException codeOrAcronym is of an unexpected type. 
#  @return ErrorCategory instance (or @c None)
def getCategory( codeOrAcronym ):
    if codeOrAcronym is None: return None
    global __categoryCodeDict, __categoryAcronymDict
    argType = type(codeOrAcronym).__name__
    if argType == 'int': # code
        code = codeOrAcronym
        if code == OK_CATEGORY.code: return OK_CATEGORY
        if not categoriesCount(): readCategories()
        cat = __categoryCodeDict.get(code)
        return cat or ErrorCategory( code, UNKNOWN_CATEGORY.acronym, UNKNOWN_CATEGORY.description )
    if argType == 'str': # acronym
        acronym = codeOrAcronym
        if acronym == OK_CATEGORY.acronym: return OK_CATEGORY
        if not categoriesCount(): readCategories()
        cat = __categoryAcronymDict.get(acronym)
        return cat or ErrorCategory( UNKNOWN_CATEGORY.code, acronym, UNKNOWN_CATEGORY.description )
    # if we get here, there is a problem
    raise AtlasErrorCodeException("Argument to getCategory() should be an error code (integer) or an error acronym (string)")

## Given an error code, retrieve the associated error acronym.
#  @param code The Integer error code.
#  @return String
def getAcronym( code ):
    global __categoryCodeDict
    # no need to load categories if it is OK
    if code == OK_CATEGORY.code: return OK_CATEGORY.acronym
    if not categoriesCount(): readCategories()
    cat = __categoryCodeDict.get( code )
    return cat and cat.acronym

## Given an error acronym, retrieve the associated error code.
#  @param acronym The String error acronym.
#  @return Integer
def getCode( acronym ):
    global __categoryAcronymDict
    # no need to load categories if it is OK
    if acronym == OK_CATEGORY.acronym: return OK_CATEGORY.code
    if not categoriesCount(): readCategories()
    cat = __categoryAcronymDict.get( acronym )
    return cat and cat.code

## Print the values in the global AtlasErrorCodes.__categoryCodeDict dictionary.
#  @return None
def dumpCategories():
    global __categoryCodeDict
    for value in __categoryCodeDict.values():
        print (value)

## Count the number of entries in the global AtlasErrorCodes.__categoryCodeDict dictionary.
#  @return Integer
def categoriesCount():
    global __categoryCodeDict
    return len( __categoryCodeDict )

## Clear the global AtlasErrorCodes.__categoryCodeDict dictionary.
#  @return None
def clearCategories():
    global __categoryCodeDict
    __categoryCodeDict.clear()

## @brief Read error categories from file.
#  @details Expected file format:
#  One category per line, with 3 items separated by the ',' character:
#  The first word on the line should be an Integer and is the @c errorCode.
#  The second word on the line is interpreted as the @c errorAcronym.
#  The rest of the line is considered the @c errorDescription.
#  Leading and trailing whitespaces of each item are stripped.
#  @param filename Name of the text file containing the error categories.
#  Defaults to @c AtlasErrorCodes.defaultCategoryFile.
#  @param clearExisting Clear all entries from the global 
#  AtlasErrorCodes.__categoryCodeDict before populating it again. Defaults to @c True.
#  @exception AtlasErrorCodeException is raised when the file containing the
#  error categories is missing or when the file is in the wrong format.
#  @return None
def readCategories( filename = defaultCategoryFile, clearExisting = True ):
    if clearExisting: clearCategories()
    if os.path.exists( filename ):
        fullfile = filename
    else:
        # The current directory is searched followed by the paths specified in the @c DATAPATH environment variable.
        fullfile = find_file_env( filename, 'DATAPATH' )
        if not fullfile:
            raise AtlasErrorCodeException( 'Could not find error category file %s' % filename )
    linenum = 0
    nRead = 0
    nAmbiguous = 0
    with open( fullfile ) as catFile:
        for line in catFile:
            linenum += 1
            line = line.strip()
            # skip empty lines and commented out lines
            if not line or line.startswith( '#' ): continue
            parts = line.split( ',' )
            if len( parts ) < 3:
                raise AtlasErrorCodeException( 'missing comma in line %s of error category file %s:\n%s' % ( linenum, filename, line ) )
            code = int( parts[ 0 ].strip() )
            acronym = parts[ 1 ].strip()
            description = ','.join( parts[ 2: ]).strip()
            added = addCategory( code, acronym, description )
            if added:
                nRead += 1
                globals()[ acronym ] = code
            else:
                nAmbiguous += 1
    print ("INFO: read %s error categories from file %s" % ( nRead, fullfile ))
    if nAmbiguous:
        print ("ERROR: Ignored %d ambiguous error category definitions" % nAmbiguous)

## Count the number of ErrorPatterns in the global AtlasErrorCodes.__errorPatterns dictionary
def errorPatternsCount():
    global __errorPatterns
    return sum( [ len(val) for val in __errorPatterns.values() ] )

## Clear the global AtlasErrorCodes.__errorPatterns dictionary
def clearErrorPatterns():
    global __errorPatterns
    __errorPatterns.clear()

## @brief Read error patterns from file.
#  @details Expected file format:
#  One error pattern entry per line, with 4 items separated by the ',' character:
#  The first item is the Atlas @c release.
#  The second item is interpreted as the @c errorAcronym.
#  The third item is the @c producer of the error. 
#  The rest of the line is considered the REGEX @c pattern.
#  Leading and trailing whitespaces of each item are stripped.
#  @param filename Name of the text file containing the error patterns.
#  Defaults to @c AtlasErrorCodes.defaultErrorPatternFile.
#  @param clearExisting Clear all entries from the global 
#  AtlasErrorCodes.__errorPatterns before populating it again. Defaults to @c True.
#  @exception AtlasErrorCodeException is raised when the file containing the
#  error patterns is missing, when the file is in the wrong format or when the
#  extracted acronym is not recognised.
#  @return None
def readErrorPatterns(filename=defaultErrorPatternFile,clearExisting=True):
    if clearExisting: clearErrorPatterns()
    if os.path.exists(filename):
        fullfile = filename
    else:
        fullfile = find_file_env(filename,'DATAPATH',depth=10)
        if not fullfile:
            raise AtlasErrorCodeException('Could not find error patterns file %s' % filename)
    linenum = 0
    nRead = 0
    severityRE = re.compile(r'\s*(?P<severity>%s|%s|%s|%s)\s+' % ( INFO, WARNING, ERROR, FATAL ) )
    with open( fullfile ) as patFile:
        for line in patFile:
            linenum += 1
            line = line.strip()
            # skip empty and commented out lines.
            if not line or line.startswith('#'): continue
            parts = line.split(',')
            if len(parts) < 4:
                raise AtlasErrorCodeException('missing comma in line %s of error pattern file %s:\n%s'% (linenum,filename,line) )
            release = parts[0].strip()
            acronym = parts[1].strip()
            who = parts[2].strip()
            if who:
                pattern = r'(?P<who>%s)\s+?' % who
            else:
                pattern = '(?P<who>)'
            msg = ','.join(parts[3:]).strip()
            pattern += r'(?P<message>(?:%s).*)' % msg
            # check that error acronym is known
            cat = __categoryAcronymDict.get(acronym)
            if not cat:
                raise AtlasErrorCodeException('Unknown acronym: %s in line %s of error pattern file %s:\n%s'% (acronym,linenum,filename,line) )
            # Determine severity of error pattern
            severityRE_result = severityRE.match( msg )
            if not severityRE_result:
                severity = FATAL # if severity cannot be deduced, it is treated as FATAL.
            else:
                severity = severityRE_result.group( 'severity' )
            addErrorPattern( release, ErrorPattern( pattern, acronym, severity ) )
            nRead += 1
    print ("INFO: read %d error patterns from file %s" % ( nRead, fullfile ))

## Count the number of regex instances in the global AtlasErrorCodes.__ignorePatterns dictionary.
def ignorePatternsCount():
    return sum( [ len(val) for key,val in __ignorePatterns.items() ] )

## @brief Read error filter patterns from file.
#  @details Expected file format:
#  One error pattern entry per line, with 3 items separated by the ',' character:
#  The first item is the Atlas @c release.
#  The second item is the @c producer of the error. 
#  The rest of the line is considered the REGEX @c pattern.
#  Leading and trailing whitespaces of each item are stripped.
#  @param filename Name of the text file containing the error patterns.
#  Defaults to @c AtlasErrorCodes.defaultErrorPatternFile.
#  @exception AtlasErrorCodeException is raised when the file containing the
#  error patterns is missing or when the file is in the wrong format
#  @return None
def readIgnorePatterns(filename=defaultIgnorePatternFile):
    if os.path.exists(filename):
        fullfile = filename
    else:
        fullfile = find_file_env(filename,'DATAPATH',depth=10)
        if not fullfile:
            raise AtlasErrorCodeException('Could not find ignore patterns file %s' % filename)
    linenum = 0
    nRead = 0
    with open( fullfile ) as patFile:
        for line in patFile:
            linenum += 1
            line = line.strip()
            # skip empty lines and commented out lines
            if not line or line.startswith('#'): continue
            parts = line.split(',')
            if len(parts) < 3:
                raise AtlasErrorCodeException('missing comma in line %s of ignore pattern file %s:\n%s'% (linenum,filename,line) )
            release = parts[0].strip()
            who = parts[1].strip()
            if who:
                pattern = r'%s\s+?' % who
            else:
                pattern = ''
            pattern += ','.join(parts[2:]).strip()
            addIgnorePattern( release, re.compile(pattern) )
            nRead += 1
    print ("INFO: read %d ignore patterns from file %s" % (nRead,fullfile))

## Import the error codes into shell environment
#  @warning Deprecated. 
#  @return None
def import_errorcodes(environment):
    for code,cat in __categoryCodeDict.items():
        environment[cat.acronym] = code

## Match a line (from the log file) against all known error patterns for a particular Atlas software release.
#  @param line A line (String) from the error logs.
#  @param release The release number of the Atlas software used to execute the transform.
#  @return a tuple containing the matched regex object, and the associated ErrorInfo object. Returns <tt>( None, None )</tt> if match not found.
def matchErrorPattern(line,release):
    rels = [ 'ALL' ]
    # add current release
    if release:
        try:
            release3 = '.'.join(release.split('.')[:3])
        except Exception:
            release3 = release
        rels.insert(0,release3)
    for rel in rels:
        patList = getErrorPatterns(rel)
        if patList:
            for pat in patList:
                match = pat.match(line)
                if match:
                    who = match.group('who')
                    if who is not None:
                        who = who.strip()
                        if not who: who = None
                    return match,ErrorInfo( acronym = pat.acronym,
                                            severity = pat.severity,
                                            who = who,
                                            message = match.group('message') )
    # no match
    return None,None

## Match the line against error patterns to be ignored for a particular Atlas software release.
#  @param line A line (String) from the error logs.
#  @param release The release number of the Atlas software used to execute the transform.
#  @return matched regex object (or @c None if match failed)
def matchIgnorePattern(line,release):
    rels = [ 'ALL' ]
    # add current release
    if release:
        try:
            # reduce 4-digit to 3-digit release
            release3 = '.'.join(release.split('.')[:3])
        except Exception:
            release3 = release
        rels.insert(0,release3)
    for rel in rels:
        patList = getIgnorePatterns(rel)
        if patList:
            for pat in patList:
                match = pat.match(line)
                if match:
                    return match
        else:
            pass
    # no match
    return None


