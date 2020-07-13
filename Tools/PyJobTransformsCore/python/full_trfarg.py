# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """End-user Fully specialised arguments that can be used in JobTransform implemenations."""

import os
import copy
from PyJobTransformsCore.basic_trfarg import Argument, BoolArg, InputDataFileArg, InputTarFileAndSetupArg, InputTarFileArg, IntegerArg, OutputDataFileArg, OutputFileArg, StringArg
from PyJobTransformsCore.envutil import find_joboptions
from PyJobTransformsCore.trfutil import AODFile, BSFile, DPDFile, ESDFile, EvgenFile, FTKIPFile, HistogramFile, HitsFile, JiveXMLTGZFile, MonitorHistFile, NtupleFile, RDOFile, SANFile, TAGFile, expandStringToList, strip_suffix
from PyJobTransformsCore.trferr import JobOptionsNotFoundError, TransformArgumentError, TransformDefinitionError
from PyJobTransformsCore import fileutil


class InputGeneratorFileArg(InputTarFileArg):
    """Input file used by the particle generator to generate events"""
    def __init__(self,help='default',destdir='.',name='default', **kwargs ):
        InputTarFileArg.__init__(self,help,destdir,name, **kwargs )

    def isFullArgument(self):
        return True
    

class InputEvgenFileArg(InputDataFileArg):
    """Input file that contains generated events"""
    def __init__(self,help='default',name='default', **kwargs ):
        InputDataFileArg.__init__(self,help,EvgenFile(),name, **kwargs )

    def isFullArgument(self):
        return True


class OutputEvgenFileArg(OutputDataFileArg):
    """Output file that contains generated events"""
    def __init__(self,help='default',name='default', **kwargs ):
        OutputDataFileArg.__init__(self,help,EvgenFile(),name, **kwargs )

    def isFullArgument(self):
        return True


class InputHitsFileArg(InputDataFileArg):
    """Input file that contains hits"""
    def __init__(self,help='default',name='default', **kwargs ):
        InputDataFileArg.__init__(self,help,HitsFile(),name,**kwargs)

    def isFullArgument(self):
        return True


class OutputHitsFileArg(OutputDataFileArg):
    """Output file that contains hits"""
    def __init__(self,help='default',name='default', temporary = False , intermediate = False, **validationDict ):
        OutputDataFileArg.__init__(self,help,HitsFile(),name, temporary, intermediate, **validationDict)

    def isFullArgument(self):
        return True


class InputRDOFileArg(InputDataFileArg):
    """Input file that contains RDO's"""
    def __init__(self,help='default',name='default', **kwargs ):
        InputDataFileArg.__init__(self,help,RDOFile(),name,**kwargs)

    def isFullArgument(self):
        return True


class OutputRDOFileArg(OutputDataFileArg):
    """Output file that contains RDO's"""
    def __init__(self,help='default',name='default', temporary = False, intermediate = False, **validationDict):
        OutputDataFileArg.__init__(self,help,RDOFile(),name, temporary, intermediate, **validationDict)

    def isFullArgument(self):
        return True


class InputBSFileArg(InputDataFileArg):
    """Input file that contains BSs"""
    def __init__(self,help='default',name='default', **kwargs ):
        InputDataFileArg.__init__(self,help,BSFile(),name, **kwargs )

    def isFullArgument(self):
        return True


class OutputBSFileArg(OutputDataFileArg):
    """Output file that contains BSs"""
    def __init__(self,help='default',name='default', temporary = False, intermediate = False, **validationDict ):
        OutputDataFileArg.__init__(self,help,BSFile(),name, temporary, intermediate, **validationDict )

    def isFullArgument(self):
        return True


class OutputSkimmedBSFileArg(OutputDataFileArg):
    """Output file that contains skimmed BSs"""
    def __init__(self,help='default',name='default', temporary = False, intermediate = False, **validationDict ):
        vDict = { 'testIfExists' : False }
        vDict.update( validationDict )
        OutputDataFileArg.__init__(self,help,BSFile(),name, temporary, intermediate, **vDict )

    def isFullArgument(self):
        return True
    
    def prepareFileInfo( self ):
        #Somewhat complicated case: The file could have _0001.data appended to its name or not exist at all
        filename=self.value()
        idx=filename.rfind("._")
        if idx==-1 or not filename[idx+2:idx+3].isdigit():
            filenameTDAQ=filename+"._0001.data"
        else:
            filenameTDAQ=filename[:idx]+"._0001.data"

        #print "Filename",filename
        #print "FilenameTDAQ",filenameTDAQ
            
        if  fileutil.exists(filename):
            self.logger().info("Found skimmed bystream file called %s", filename)
            OutputDataFileArg.prepareFileInfo( self )
            return
        elif  fileutil.exists(filenameTDAQ):
            self.logger().info("Found skimmed bystream file called %s, renaming back to %s", filenameTDAQ, filename)
            os.rename(filenameTDAQ,filename) #try - except?
            #That's of course a hack that will work only in local file system. 
            OutputDataFileArg.prepareFileInfo( self )
            return
        else:
            self.logger().info("No skimmed bystream file corresponding to %s found.", filename)
            return
        
class InputESDFileArg(InputDataFileArg):
    """Input file that contains ESDs"""
    def __init__(self,help='default',name='default', **kwargs ):
        InputDataFileArg.__init__(self,help,ESDFile(),name,**kwargs)

    def isFullArgument(self):
        return True


class OutputESDFileArg(OutputDataFileArg):
    """Output file that contains ESDs"""
    def __init__(self,help='default',name='default', temporary = False, intermediate = False, **validationDict ):
        OutputDataFileArg.__init__(self,help,ESDFile(),name, temporary, intermediate, **validationDict )

    def isFullArgument(self):
        return True


class InputAODFileArg(InputDataFileArg):
    """Input file that contains AODs"""
    def __init__(self,help='default',name='default', **kwargs ):
        InputDataFileArg.__init__(self,help,AODFile(),name,**kwargs)

    def isFullArgument(self):
        return True


class OutputAODFileArg(OutputDataFileArg):
    """Output file that contains AODs"""
    def __init__(self,help='default',name='default', temporary = False, intermediate = False, **validationDict ):
        OutputDataFileArg.__init__(self,help,AODFile(),name, temporary, intermediate, **validationDict )

    def isFullArgument(self):
        return True


class InputTAGFileArg(InputDataFileArg):
    """Input file that contains TAGs"""
    def __init__(self,help='default',name='default', **kwargs ):
        InputDataFileArg.__init__(self,help,TAGFile(),name, **kwargs )

    def isFullArgument(self):
        return True

class InputTAG_AODFileArg(InputDataFileArg):
    """Input file that contains TAGs pointing to AODs, in particular"""
    def __init__(self,help='default',name='default', **kwargs ):
        InputDataFileArg.__init__(self,help,TAGFile(),name, **kwargs )

    def isFullArgument(self):
        return True

class OutputTAGFileArg(OutputDataFileArg):
    """Output file that contains TAGs"""
    def __init__(self,help='default',name='default', temporary = False, intermediate = False, **validationDict ):
        OutputDataFileArg.__init__(self,help,TAGFile(),name, temporary, intermediate, **validationDict )

    def isFullArgument(self):
        return True


class OutputSANFileArg(OutputFileArg):
    """Output file that contains SANs"""
    def __init__(self,help='default',name='default', temporary = False, intermediate = False, **validationDict ):
        OutputFileArg.__init__(self,help,SANFile(),name, temporary, intermediate, **validationDict )

    def isFullArgument(self):
        return True


class HistogramFileArg(OutputFileArg):
    """Output file that contains histograms."""
    def __init__(self,help='default',name='default', temporary = False, intermediate = False, **validationDict ):
        OutputFileArg.__init__(self,help,HistogramFile(),name, temporary, intermediate, **validationDict )

    def isFullArgument(self):
        return True


class InputNtupleFileArg(InputDataFileArg):
    """List of NTUP input files"""
    def __init__(self,help='default', name='default', tree_names=None, **kwargs):
        if tree_names is None:
            tree_names = []
        InputDataFileArg.__init__( self, help, NtupleFile( tree_names = tree_names ),name, **kwargs )

    def isFullArgument(self):
        return True

class OutputFTKIPFileArg(OutputDataFileArg):
    '''Output file for FTK inputs'''
    def __init__(self, help='default', name='default', temporary=False, intermediate=False, **validationDict):
        OutputDataFileArg.__init__(self,help,FTKIPFile(),name, temporary, intermediate, **validationDict)

    def isFullArgument(self):
        return True
    
class OutputJiveXMLTGZFileArg(OutputDataFileArg):
    '''Output for tar+gzipped JiveXML files'''
    def __init__(self, help='default', name='default', temporary=False, intermediate=False, **validationDict):
        OutputDataFileArg.__init__(self,help,JiveXMLTGZFile(),name, temporary, intermediate, **validationDict)

    def isFullArgument(self):
        return True
        

class NtupleFileArg(OutputFileArg):
    """Output file that contains ntuples."""
    def __init__( self, help = 'default', name = 'default', temporary = False, intermediate = False, tree_names = None, **validationDict ):
        if tree_names is None:
            tree_names = []
        OutputFileArg.__init__( self, help, NtupleFile( tree_names = tree_names ),name, temporary, intermediate, **validationDict )

    def isFullArgument(self):
        return True


class InputMonitorHistArg(InputDataFileArg):
    """List of HIST input files"""
    def __init__(self,help='default', name='default', **kwargs):
        InputDataFileArg.__init__( self, help, MonitorHistFile(), name, **kwargs )

    def isFullArgument(self):
        return True


class MonitorHistArg(OutputFileArg):
    """Derived class for monitoring HIST."""
    def __init__( self, help = 'default', name = 'default', temporary = False, intermediate = False, **validationDict ):
        OutputFileArg.__init__( self, help, MonitorHistFile(), name, temporary, intermediate, **validationDict )

    def isFullArgument(self):
        return True


class InputDPDFileArg(InputDataFileArg): 
    """Input file that contains DPD's""" 
    def __init__(self,help='default',name='default', **kwargs ): 
        InputDataFileArg.__init__(self,help,DPDFile(),name, **kwargs) 
        
    def isFullArgument(self): 
        return True


class OutputDPDFileArg(OutputDataFileArg): 
     """Output file that contains DPD's""" 
     def __init__(self,help='default',name='default', temporary = False, intermediate = False, **validationDict ): 
         OutputDataFileArg.__init__(self,help,DPDFile(),name, temporary, intermediate, **validationDict )

     def isFullArgument(self): 
         return True

 
class NbOutputDPDArg(IntegerArg): 
    """Number of simultaneous output DPD streams.""" 
    def __init__(self,help='default',name='default'): 
        IntegerArg.__init__(self,help,name) 
        
    def isFullArgument(self): 
        return True

class BasicBoolArg(BoolArg):
    """Basic Boolean"""
    def __init__(self,help='default',name='default'):
        BoolArg.__init__(self,help,name)

    def isFullArgument(self):
        return True

class BasicStringArg(StringArg):
    """Basic string"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True
    
class BasicIntArg(IntegerArg):
    '''Anonymous integer'''
    def __init__(self,help='default',name='default'): 
        IntegerArg.__init__(self,help,name) 
        
    def isFullArgument(self): 
        return True


class BasicExec(Argument): 
    """Transfrom a string into generic multi-line python fragment where each line is an element in a list.""" 
    def __init__(self,help='default',name='default'): 
        Argument.__init__(self,help,name) 
    def isFullArgument(self): 
        return True
    def basicType(self):
        return 'list'
    def metaType(self):
        return 'plain'

    def toPython(self,valIn):
        from PyJobTransformsCore.trfutil import StringToExec
        try:
            valOut=StringToExec(valIn)
            return valOut
        except Exception:
            raise TransformArgumentError( '%s=%s: syntax error in BasicExec' % (self.name(),valIn) )
        return None
    
class AsetupArg(StringArg):
    '''String holding arguments to be passed to asetup before running a substep'''
    def __init__(self,help='default',name='default'): 
        super(AsetupArg, self).__init__(help,name) 
        
    def isFullArgument(self): 
        return True  


class ListOfStringsArg(StringArg): 
    """List of strings separated by commas""" 
    def __init__(self,help='default',name='default'): 
        StringArg.__init__(self,help,name)
 
    def isFullArgument(self): 
        return True

    def basicType(self):
        return 'list'

    def toPython(self,valIn):
        # if already a list, nothing to be done
        if isinstance(valIn,list):
            return valIn
        # make a list of python types out of the string separated either by , or ,, 
        else:
            try:
                valTmp=valIn.replace(',,',',')
                valList=valTmp.split(',')
            except Exception:
                raise TransformArgumentError( '%s=%s: syntax error in list of strings' % (self.name(),valIn) )
            return valList


class DpdConfigArg(ListOfStringsArg):
    """Configuration fragment for DPD.""" 
    def __init__(self,help='default',name='default'): 
        ListOfStringsArg.__init__(self,help,name)


class JobOptionsArg(StringArg):
    """(List of) JobOptions file(s) to use."""
    def __init__(self,help='default',package='',name='default'):
        """<package> = string with comma separated list of default packages to be searched in.
        The current working directory will be automatically added as the first place to look."""
        pacType=type(package)
        if pacType == str:
            self._packages = package.split(',')
        elif pacType == list:
            self._packages = package
        else:
            raise TypeError("%s constructor argument \'package\' is not of type str or list (got type %s)",
                            self.__class__.__name__, pacType.__name__)
        # prepend current directory if not already present
        if os.curdir not in self._packages: self._packages.insert(0,os.curdir)
        self._fullFiles = []
        self._exeEnv = {} # copy of python environment after last execution of joboptions
        StringArg.__init__(self,help,name)

    def __nonzero__(self):
        return Argument.__nonzero__(self) and  len(self.value()) > 0

    def isFullArgument(self):
        return True

    def basicType(self):
        return 'list'

    def toPython(self,valIn):
        # if already a list, nothing to be done
        valType = type(valIn)
        if valType == list:
            return valIn
        # make a list of python types out of the strings
        elif valType == str:
            if valIn == 'NONE': return []
            # split the comma separated list (outside brackets)
            try:
                return expandStringToList( valIn, False )
            except Exception as msg:
                raise TransformDefinitionError( '%s=%s: %s' % ( self.name(), valIn, msg ) )
        else:
            # if we get here, there is problem
            raise TransformDefinitionError( '%s=%s: value is not a list or a string' % (self.name(),valIn) )

    def defaultHelp(self):
        help=Argument.defaultHelp(self)
        if self._packages: help += '. Default packages: %s' % ','.join(self._packages)
        return help

    def hasPackage(self):
        return len(self._packages) > 0

    def package(self):
        return self._packages

    def value(self):
        """Prepend default package to all joboptions without explicit package (i.e. subdirectory)"""
        val = Argument.value(self)
        newVal = []
        for v in val:
            if self._packages and os.sep not in v:
                for p in self._packages:
                    full = os.path.join(p,v)
                    if find_joboptions( full ):
                        self.logger().info( "Found %s in %s", v, p )
                        newVal.append( full )
                        break
                else:
                    newVal.append( v ) # either in local dir or not found (will trigger error later)
            else:
                newVal.append( v )
        return newVal

    def setValue(self,value):
        """Convert NONE value to all upper case"""
        self._fullFiles = []  # is redetermined on-demand
        if type(value) == str:
            #convert value to the correct case
            valUpper = value.upper()
            if valUpper == 'NONE' and value != 'NONE':
                self.logger().info( 'Changing case of %s to %s', value,valUpper )
                value = valUpper
            # treat empty string as NONE
            if value == '': value = 'NONE'
        Argument.setValue(self,value)

    def fullFilenames(self):
        if not self._fullFiles:
            for val in self.value():
                fullFile = find_joboptions( val )
                if not fullFile:
                    raise JobOptionsNotFoundError( val, "Argument %s" % self.name() )
                self._fullFiles.append(fullFile)
        return self._fullFiles

    def preRunAction(self):
        """Check than jobOptions file can be found"""
        if not self: return
        shortList = self.value()
        fullList = self.fullFilenames()
        for i in range(len(shortList)):
            short = shortList[i]
            full  = fullList[i]
            self.logger().info( 'Found %s in %s', short, strip_suffix(full,short) )

    def exeJobOptions( self, env = {} ):
        """Execute the lines in the jobOptions file in environment <env>. The runtime
        environment will be added to <env> (theApp etc)"""
        if not self:
            return
        from PyJobTransformsCore import FakeAthena
        FakeAthena.theApp.setLoggerParentName( self.loggerName() )
        shortList = self.value()
        fullList = self.fullFilenames()
        for i in range( len( shortList ) ):
            short = shortList[ i ]
            full  = fullList[ i ]
            self.logger().info( "Executing jobOptions file %s.", full )
            FakeAthena.theApp.exeJobOptions( short, env )
        self._exeEnv.update( copy.copy( env ) )

    def exeEnvironment(self):
        """copy of python environment after last execution of joboptions"""
        return self._exeEnv

    def metaData(self):
        if not self:
            return {}
        else:
            # return list as string with comma separated entries
            return { self.name() : ','.join(self.value()) }
#
# end of class JobOptionsArg
#


class JobConfigArg(JobOptionsArg):
    """Joboptions file with user settings, in particular the configuration settings"""
    def __init__(self,help='default',package='',name='default'):
        # split comma separated string into list
        if type(package) == str: package = package.split(',')
        # always add 'PyJobTransforms' package (which contain common jobConfig files)
        commonPack = 'PyJobTransforms'
        if commonPack not in package: package.append(commonPack)
        JobOptionsArg.__init__(self,help=help,package=package,name=name)
        self.__config = None

    def isFullArgument(self):
        return True

    def setConfig(self,config):
        """Set the configuration object"""
        self.__config = config

    def preRunAction(self):
        if not self: return
        JobOptionsArg.preRunAction(self)
        # execute it, so that any changes to the TransformConfig are processed
        env = {}
        if self.__config:
            # add config object with generic name
            env['runConfig'] = self.__config
        JobOptionsArg.exeJobOptions(self,env)

    def runArgsTemplate(self,objName):
        plate = [ JobOptionsArg.runArgsTemplate(self,objName) ]
        conf = self.__config
        if conf:
            plate += [ '# Make run configuration object available with generic name' ,
                       'from %s import %s' % (conf.__module__,conf.name()),
                       'runConfig = %s' % (conf.name(),) ]
        return os.linesep.join(plate)


class RandomSeedArg(IntegerArg):
    """Randomseed for AtGenRndmSvc"""
    def __init__(self,help,name='default'):
        IntegerArg.__init__(self,help,name)

    def isFullArgument(self):
        return True

    def jobOrTask(self):
        return 'job'


class RunNumberArg(IntegerArg):
    """Run number of data file"""
    def __init__(self,help='default',name='default'):
        IntegerArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True


class FirstEventArg(IntegerArg):
    """First event number to use"""
    def __init__(self,help='default',name='default'):
        IntegerArg.__init__(self,help,name)

    def isFullArgument(self):
        return True

    def jobOrTask(self):
        return 'job'


class SkipEventsArg(IntegerArg):
    """Number of events to skip"""
    def __init__(self,help='default',name='default'):
        IntegerArg.__init__(self,help,name)

    def isFullArgument(self):
        return True

    def jobOrTask(self):
        return 'job'

    def metaData(self):
        """No metadata for skipevents"""
        return {}

        
class MaxEventsArg(IntegerArg):
    """Maximum number of events to process"""
    def __init__(self,help='default',name='default'):
        IntegerArg.__init__(self,help,name)
        self.setDefault( -1 )
        self.setValue( -1 )

    def isFullArgument(self):
        return True

 
class GeometryVersionArg(StringArg):
    """Geometry Version"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True


class ConditionsTagArg(StringArg):
    """IOVDbSvc global tag in case it is needed to change the DEFAULT value"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True


class TriggerConfigArg(StringArg):
    """Configuration string to use for TrigT1 and HLT. Set to 'NONE' to switch off trigger,
    and set to 'DEFAULT' to use the default of the used release."""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)

    def isFullArgument(self):
        return True
    
    def setValue(self,value):
        valueUpper = value.upper()
        if valueUpper == 'OFF': # for backwards compatibility
            value = 'NONE'
        elif valueUpper == 'NONE':  # get consistent NONE value
            value = 'NONE'
        elif valueUpper == 'DEFAULT': # or consistent DEFAULT value
            value = 'DEFAULT'
        Argument.setValue(self,value)


class OptionArg(Argument):
    """Placeholder Argument that is treated as an option. Option arguments are always
    optional. If a default is not given when added to a JobTransform, then the
    default is taken from the associated option.
    <name>: name of the option it represents. Only names of existing options are accepted.
    (long name without the leading --)"""
    def __init__(self,name,runOptions=None):
        self.__runOptions = runOptions
        Argument.__init__(self,'',name)
        if runOptions:
            value = getattr(runOptions,self.name())
            default = runOptions.getProperty(self.name()).default()
            if not default: default = value
            if not self.hasDefault(): Argument.setDefault(self,default)
            if not self.hasValue()  : Argument.setValue(self,value)
    # Implement required functions of Argument
    #
    def isFullArgument(self):
        return True

    def basicType(self):
        return "string"

    def type(self):
        return "string"

    def metaType(self):
        return "option"

    def toPython(self,val):
        # any type is accepted
        return val

    # Override functions of Argument
    #
    def setValue(self,val):
        """Pass value on to the option"""
        Argument.setValue( self, val )
        setattr( self.__runOptions, self.name(), Argument.value( self ) )

    def value(self):
        """Return the option value"""
        return getattr(self.__runOptions,self.name())

    def help(self):
        return self.__runOptions.getProperty(self.name()).help()
    

class DBReleaseArg( InputTarFileAndSetupArg ):
    """Tarball containing the DBRelease to use"""
    def __init__( self, name = 'DBRelease', destdir = os.curdir ):
        InputTarFileAndSetupArg.__init__( self, help = 'default', name = name,
                                          setupfile = 'setup.py', envVars = None, destdir = destdir, temporary = True )

    def isFullArgument(self):
        return True

    def untarAndSetup(self):
        """Install the DBRelease"""
        try:
            InputTarFileAndSetupArg.untarAndSetup(self)
        except TransformArgumentError as e:
            e.setError('TRF_DBREL_TARFILE')
            raise
        # unset DBRELEASE_REQUESTED to avoid needless crash in check in JobTransform
        try:
            del os.environ['DBRELEASE_REQUESTED']
        except KeyError:
            pass

    def install(self):
        """Install the DBRelease if a filename is given."""
        if not self: return
        self.untarAndSetup()

    def metaData(self):
        """No metadata"""
        return {}

    def metaType(self):
        return 'dbrelease'


class EventSelectorQueryArg(StringArg):
    """String with an event selector query in it"""
    def __init__(self,help='default',name='EventSelectorQuery'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True
    
