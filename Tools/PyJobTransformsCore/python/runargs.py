# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

__author__ = "clat@hep.ph.bham.ac.uk"

import os
from PyJobTransformsCore.TransformConfig import Descriptor, JobConfig
from PyJobTransformsCore.trfutil import VALIDATION_DICT
from PyJobTransformsCore.trferr import TransformArgumentError


class RunArguments:
    """Dynamic class that holds the run arguments as named members with values."""
    def __str__(self):
        myself = 'RunArguments:'
        for arg in dir(self):
            if not arg.startswith('__'):
                myself += '%s   %s = %s' % (os.linesep, arg, repr(getattr(self,arg)))
        return myself
    



class CommandLineOption( Descriptor ):
    def __init__(self, shortOption, longOption, help, takesArgument=False, default=None, choices=None):
        """For options with or without argument:
        <shortOption>: short option name, to be used with - (added). Empty string if not defined.
        <longOptions> long option name, to be used with -- (added). Empty string if not defined.
        <help> Help string explaining the option
        For options with argument:
        <takesArgument>: should be set to True
        <default> : default value for options that take an argument.
        <choices> : list of possible values the option can have. For options without argument
                    this is (python) True or False.
        """
        self.__takesArgument = takesArgument
        if not takesArgument:
            # option without an argument is considered a boolean
            default = False
            choices = [True,False]
        self.__shortName = shortOption.lstrip('-')
        self.__longName = longOption.lstrip('-')
        self.__actionFunc = None
        Descriptor.__init__(self,help,default,choices)
    
    
    def setActionFunction(self,func):
        """Set function to be called when option value is set. If the option takes
        an argument, the function must take an argument of type string.
        If the option does not take an argument, the function must not take
        any arguments, and will only be called if the option is set to True.
        """
        self.__actionFunc = func
    
    
    def shortName(self):
        return self.__shortName
    
    
    def shortOption(self):
        return '-' + self.__shortName
    
    
    def longName(self):
        return self.__longName
    
    
    def longOption(self):
        return '--' + self.__longName
    
    
    def takesArgument(self):
        return self.__takesArgument
    
    
    def processOption(self,value):
        if value is None: return
        func = self.__actionFunc
        if not func: return
        if self.__takesArgument:
            func(value)
        elif value is True:
            func()
    
    
    def _checkType(self,variableName,value):
        """Allowed types:
        For option not taking argument: bool (True,False) or string (\'True\' or \'False\')
        For option taking argument: string or bool
        """
        valType = type(value).__name__
        if not self.__takesArgument:
            # convert certain strings to bool
            if valType == 'str':
                # empty string is given by getopt (when option is set)
                # convert to boolean if possible.
                try:
                    value = { 'TRUE' : True, 'FALSE' : False, '' : True }[ value.upper() ]
                except Exception:
                    raise TransformArgumentError( '%s value of %s not recognised.' % ( variableName, value ) )
            elif not isinstance( value, bool ):
                raise TransformArgumentError( '%s should be boolean. Got %s (%s) instead.' % (variableName, value, valType) )
        elif valType not in ( 'str', 'bool' ):
            raise TransformArgumentError( '%s should be a string or bool. Got %s (%s) instead.' % (variableName, value, valType) )
        return value
    
    
    def _checkValue(self,variableName,value):
        if self.__takesArgument:
            # handle the argument (always of the string type)
            choices = self.allowedValues()
            try:
                valueList = value.split( ',' )
            except Exception:
                valueList = [ value ]
            # Convert value to the correct case, if there is a list of string choices
            if choices:
                newValueList = []
                for v in valueList:
                    for c in choices:
                        try:
                            x = v.upper() == c.upper()
                        except AttributeError:
                            if v == c:
                                newValueList.append( v )
                                break
                        else:
                            if x:
                                if v != c:
                                    v = c
                                newValueList.append( v )
                                break
                    else:
                        raise TransformArgumentError( '%s value %r is not in %s' % ( variableName, v, choices ) )
                try: # string list
                    return ','.join( newValueList )
                except Exception: # assume boolean
                    return newValueList[0]
        # check against list of possible values
        return Descriptor._checkValue(self,variableName,value)
    
    
    
#        if self.__takesArgument:
#            # handle the argument (always of the string type)
#            choices = self.allowedValues()
#            try:
#                valueList = value.split( ',' )
#            except:
#                valueList = [ value ]
#            # Convert value to the correct case, if there is a list of string choices
#            if choices:
#                for v in valueList:
#                    for c in choices:
#                        if v.upper() == c.upper():
#                            if v != c:
#                                v = c
#                            break
#                    else:
#                        raise TransformArgumentError( '%s value %r is not in %s' % ( variableName, v, choices ) )
#                return ','.join( valueList )
#        if self.__takesArgument:
#            # handle the argument (always of the string type)
#            choices = self.allowedValues()
#            # Convert value to the correct case, if there is a list of string choices
#            if choices:
#                valUpper = value.upper()
#                for c in choices:
#                    if valUpper == c.upper() and value != c:
#                        self.logger().warning( 'Changing case %s=%s to %s' % (variableName,value,c) )
#                        value = c
#                        break
#        # check against list of possible values
#        return Descriptor._checkValue(self,variableName,value)
    
    def _setValue(self,variableName,value):
        print ("Setting %s to %r" % (variableName,value))
        if value is not None and (self.__takesArgument or value):
            self.processOption(value)
    
    
    def _checkAllowedValues(self,variableName,allowedValues):
        # Check all allowed values
        if not self.__takesArgument:
            for val in allowedValues:
                if val is not True and val is not False:
                    raise TransformArgumentError( '%s should be True or False. Got %s (%s) instead.' % (variableName, val, type(val).__name__) )
        else:
            for val in allowedValues:
                valType = type(val).__name__
                if valType not in [ 'str', 'bool' ]:
                    raise TransformArgumentError( '%s should be a string or boolean. Got %s (%s) instead.' % (variableName, val, valType) )
        return allowedValues
    





class RunOptions(JobConfig):
    """Class to hold the running options"""
    __slots__ = ()
    help       = CommandLineOption('h', 'help', 'Print detailed help')
    #loglevel   = CommandLineOption('l', 'loglevel', '**DEPRECATED**', True, 'INFO', TransformLogger.messageLevels.keys())
    test       = CommandLineOption('t', 'test', 'Run in test mode. Tests/checks omitted using the --omitvalidation=XXX option applied everywhere rather than being limited to validation routines. GUIDs are not extracted either.')
    command    = CommandLineOption('c', 'command', 'Python code to be pre-executed before any scripts', True )
    mcinput    = CommandLineOption('', 'mcinput', 'Monte Carlo input file used. Omit if real data is used. Enabling this option also implies setting --usesqlite=True', True, False, choices = [ True, False ] )
    usesqlite  = CommandLineOption('', 'usesqlite', 'Force the use of local/symlinked sqlite files rather than ORACLE.', True, False, choices = [ True, False ]  )
    omitvalidation = CommandLineOption('', 'omitvalidation', 'Omit certain validation routines. Use "ALL" to omit all validation routines, use "NONE" to remove all validation ommissions.', True, choices = list(VALIDATION_DICT.keys()) + ['NONE'] )
    athenaopts = CommandLineOption('a', 'athenaopts', 'Options to be passed on to athena', True)
    argdict    = CommandLineOption('',  'argdict', 'Run arguments given in a python dictionary in a pickle file', True)
    
    # options setting log file error masks.
    ignoreunknown = CommandLineOption('', 'ignoreunknown', '**DEPRECATED** Mask out any unknown errors found (i.e. ignore TRF_UNKNOWN errors)' )
    ignoreall = CommandLineOption( '', 'ignoreall', 'Force transformations, composite ones in particular, to proceed regardless of any errors encountered.')
    ignoreerrors = CommandLineOption('', 'ignoreerrors', 'Mask all errors. Transform returns successful if athena.py is successful. Contributes to NEEDCHECK flag. This option no longer accepts an additional file to supplement existing error ignore filters. For that purpose, use =--extraignorefilter=XXX= option.', True, False, choices = [ True, False, 'ALL' ] )
    
    # Memory optimisation options to be passed to Athena.
    tcmalloc   = CommandLineOption('', 'tcmalloc', '**DEPRECATED** Use tcmalloc for memory allocation instead of stdcmalloc' )
    leakcheckexecute = CommandLineOption('', 'leakcheckexecute', 'Perform basic memory leak checking. This disables the use of tcmalloc')
    
    # Memory-related flags to be set in the execution shell
    rss = CommandLineOption( '', 'rss', 'Set maximum resident memory value (in megabytes).', True )
    vmem = CommandLineOption( '', 'vmem', 'Set maximum virtual memory (in megabytes).', True )
    
    # file containing dictionary of metadata filters (forced insertions and additional metadata filters )
    extrametadatadict = CommandLineOption( '', 'extrametadatadict', 'Full path to file containing extraMetadataDict, a dictionary of compiled patterns and/or strings for explicit inclusion', True )
    
    # file containing additional error ignore filters (in the format set out in atlas_error_ignore.db)
    extraignorefilters = CommandLineOption( '', 'extraignorefilters', 'Full path to file containing additional error ignore filters in the format set out in atlas_error_ignore.db', True )
    
    # temporary options for testing
    usenewmeta = CommandLineOption( '', 'usenewmeta', 'Use new metadata format. Experimental', True, False, choices = [ True, False ] )
    
    #upload to AMI
    uploadtoami = CommandLineOption( '-m', 'uploadtoami', 'Upload performance data to AMI. Should be used only by T0 or T1. Value is probability of job to really report. (For throtling reporting of MC jobs).', True, '0.0' )
    
    def __init__(self):
        JobConfig.__init__(self,'runOpts')
    
    
    def __str__(self,indent=''):
        """String containing a formatted description of the options"""
        help = []
        maxOpt = 0
        optsList = []
        optsHelp = []
        # make options list and determine max string length
        nOpts = len(self)
        for opt in self:
            hlp = opt.doc()
            if (opt.takesArgument()):
                optName=''
                if opt.shortOption() != '':
                    optName += '%s <value>' % opt.shortOption()
                if opt.longOption() != '':
                    if optName: optName += ', '
                    optName += '%s=<value>' % opt.longOption()
                allowed = opt.allowedValues()
                if allowed:
                    hlp += '. Possible values: %s' % allowed
            else:
                optName = '%s, %s' % ( opt.shortOption(), opt.longOption() )
            maxOpt = max(maxOpt,len(optName))
            optsList.append( optName )
            optsHelp.append( hlp )
        helpColumn = min(40,len(indent) + maxOpt + 2)
        helpIndent = ' '*helpColumn # indent for help continuation lines
        for iOpt in range(nOpts):
            opt = optsList[iOpt]
            helpSpaceCount = max(2,helpColumn - len(opt) - len(indent))
            helpSpace = ' '*helpSpaceCount
            h = optsHelp[iOpt]
            hLines = h.split(os.linesep)
            # add first line
            help += [ '%s%s%s%s' % (indent, opt, helpSpace, hLines[0]) ]
            # add other lines (if present)
            if len(hLines) > 1:
                help += [ '%s%s' % (helpIndent,s) for s in hLines[1:] ]
        return os.linesep.join(help)
    
    
    def toLongName(self,name):
        """Convert any allowed option name to the long option name without '--'"""
        longName = None
        shortName = None
        if name.startswith('--'):
            longName = name[2:]
        elif name.startswith('-'):
            shortName = name[1:]
        elif len(name) == 1:
            shortName = name
        else:
            longName = name
        if longName and self.isLongOption(longName):
            return longName
        if shortName:
            for opt in self:
                if opt.shortName() == shortName:
                    return opt.longName()
        raise KeyError
    
    
    def isLongOption(self,name):
        """Return bool indicating whether <name> is one of the allowed long options.
        <name> can optionally begin with --
        """
        if name.startswith('--'): name = name[2:]
        for opt in self:
            if opt.longName() == name:
                return True
        return False
    
    def isShortOption(self,name):
        if name.startswith('-'): name = name[1:]
        for opt in self:
            if opt.shortName() == name:
                return True
        return False
    
    def setOption(self,name,value):
        setattr(self,self.toLongName(name),value)
    
    def processOptions(self):
        """Call associated functions on all options"""
        for opt in self:
            opt.processOption(getattr(self,opt.name()))
    
    def extractOptions(self, argList):
        """Scan the command line argument list given in <argList>. It returns a new
        list where all the options have been removed from <argList>.
        It considers an argument a valid option if it is:
        -<shortname>
        -<shortname> <value>
        --<longname>
        --<longname>=<value>
        <longname>=<value>
        Any argument starting with '-' or '--' will be considered an option, and
        will be checked against the list of known options. If no match is found,
        it is considered an error unless the argument is a negative number
        (integer or float), in which case it will be included in the returned list.
        If an argument equals '--', then all following arguments are considered not options,
        and will be included in the returned list.
        If there is any problems with the options, an TransformArgumentError exception is raised.
        """
        cleanList = []
        nArgs = len(argList)
        iArg = 0
        while iArg < nArgs:
            arg = argList[iArg]
            if arg == '--':
                # no more options. Add all remaining arguments
                iArg += 1
                while iArg < nArgs:
                    cleanList.append( argList[iArg] )
                    iArg += 1
                return cleanList
            # get argument name
            equal=arg.find('=')
            value = None
            if equal == -1:
                name = arg
            else:
                name = arg[:equal]
                if equal+1 < len(arg):
                    value=arg[equal+1:]
                    # convert to boolean if possible.
                    try:
                        value = { 'TRUE' : True, 'FALSE' : False }[ value.upper() ]
                    except Exception:
                        pass
                else:
                    value=''
            if name.startswith('--'):
                # cases --<longname> and --<longname>=<value>
                longName = name[2:]
                if not self.isLongOption(longName):
                    raise TransformArgumentError('option %s not supported' % name)
                option = self.getProperty(longName) # gets the option descriptor
                if option.takesArgument():
                    if equal == -1:
                        value = option.default()
#                        raise TransformArgumentError('option %s value missing. Syntax: %s=<value>' % (name,name) )
                    # set the value
                    setattr(self,longName,value)
                else:
                    if equal != -1:
                        raise TransformArgumentError('option %s does not take a value (Found =%s)' % (name,value) )
                    # set value to True
                    setattr(self,longName,True)
            elif name.startswith('-'):
                # cases -<shortname>, -<shortname> <value> or a negative number
                if not self.isShortOption(name):
                    try:
                        # test if it is a negative number
                        float(name)  # covers both float and int
                    except Exception:
                        raise TransformArgumentError('option %s not supported' % name)
                    else:
                        # it is a negative number: add it to the list, and go to next
                        cleanList.append( arg )
                        iArg += 1
                        continue
                longName = self.toLongName(name)
                option = self.getProperty(longName) # gets the option descriptor
                if option.takesArgument():
                    if equal != -1:
                        raise TransformArgumentError('option %s syntax error: no \'=\' allowed. Value should be specified as next argument: %s <value>' % (name,name) )
                    # value is next argument
                    iArg += 1
                    if iArg >= nArgs:
                        raise TransformArgumentError('option %s missing value. Syntax: %s <value>' % (name,name) )
                    value = argList[iArg]
                    # convert to boolean if possible.
                    try:
                        value = { 'TRUE' : True, 'FALSE' : False }[ value.upper() ]
                    except Exception:
                        pass
                    setattr(self,longName,value)
                else:
                    if equal != -1:
                        raise TransformArgumentError('option %s syntax error: no \'=\' allowed in name (option takes no value.)' % (name,) )
                    # set value to True
                    setattr(self,longName,True)                    
            elif equal != -1 and self.isLongOption(name):
                option = self.getProperty(name)
                if not option.takesArgument():
                    raise TransformArgumentError('option %s takes no value' % (name,))
                # case <longname>=<value>
                setattr(self,name,value)
            else:
                # not an option
                cleanList.append( arg )
            # move to next argument    
            iArg += 1
            # end of while iArg < nArgs
        return cleanList
    

