#!/usr/bin/env python

import sys, os.path, re, glob, logging, commands, time, tempfile
import cPickle as pickle
from optparse import OptionParser, Option, OptionValueError
from copy import copy
from PyJobTransformsCore import fileutil, trfutil

# ListOfDefaultPositionalKeys needs to be maintained by hand, to support 'grep' from ProdSys
ListOfDefaultPositionalKeys = [ 'mergemethod', 'inputfilelist', 'outputfile', 'additionalmergedictfile', 'extraopts' ]

USAGE = "Arguments for %prog can be provided in one of the following ways:\n\
1) %prog -m MERGEMETHOD -i INPUTFILELIST -o OUTPUTFILE [-a ADDITIONALMERGEDICTFILE] [--extraopts=EXTRAOPTIONS]\n\
2) %prog mergemethod=MERGEMETHOD inputfilelist=INPUTFILELIST outputfile=OUTPUTFILE [additionalmergedictfile=ADDITIONALMERGEDICTFILE] [extraopts=EXTRAOPTIONS]\n\
3) %prog MERGEMETHOD INPUTFILELIST OUTPUTFILE [ADDITIONALMERGEDICTFILE] [EXTRAOPTIONS]\n"
VERSION = "%prog v0.3.0"

def _getLoggerName():
    frame = sys._getframe(1)
    try:
        name = os.path.splitext( os.path.basename( frame.f_globals[ '__file__' ] ) )[0]
    except Exception, msg:
        # no file associated with the frame (e.g. interactive prompt, exec statement)
        logger.debug( "_getLoggerName(): %s", msg )
        name = 'merge_trf.py'
    del frame
    return name

loggerName = _getLoggerName()
loggerFilename = '%s.log' % loggerName
logging.basicConfig( level = logging.DEBUG,
                     format = '%(levelname)-8s %(message)s',
                     datefmt = '%a, %d %b %Y, %H:%M:%S',
                     stream = sys.stdout )
mergeLogFH = logging.FileHandler( loggerFilename, 'w' )
mergeLogFH.setLevel( logging.DEBUG )
mergeLogFH.setFormatter( logging.Formatter( '%(asctime)s %(levelname)-8s %(message)s' ) )
logging.getLogger('').addHandler( mergeLogFH )
logger = logging.getLogger( loggerName )

# Returns a dictionary key/value pairs used to populate a job report dictionary.
def verifyNTUPLEmerge( mainscript_output_pair, post_retcode, post_stdout, *args, **kwargs ):
    # For Ntuple merging, pre_retcode and pre_stdout are ignored.
    if post_retcode == 256: #no errors found in postscript grep execution stdout.
        retcode = 0
        acronym = 'OK'
        stdout = 'trf finished OK'  
    else: #errors found in logfile
        retcode = 1
        acronym = 'TRF_ROOT_EXE'
        stdout = 'ROOT execution problem: \n' + mainscript_output_pair[1][ -1000: ]
    return { 'retcode' : retcode, 'acronym' : acronym, 'stdout' : stdout }

def verifyTAGmerge( mainscript_output_pair, post_retcode, post_stdout, _segmentLogName, *args, **kwargs ):
    if mainscript_output_pair[0] != 0:
        mainscript_output_pair[0] = 62600
    nevts = 0
    try:
        cDict = kwargs[ 'counterDict' ]
    except KeyError:
        raise MergeTrfInternalError( "counterDict reference not passed to verfyTAGmerge()." )
    if post_retcode == 0:
        post_retcode = 62601 # "error" string found.
    else:
        post_retcode = 0 # no "error" string found
        rc,os = commands.getstatusoutput( 'grep \"total events appended\" %s' % _segmentLogName )
        if rc == 0: # some lines containing the required text was found
            evtsRE = re.compile(r'(?P<nevts>\d+) total events appended')
            evtsList = evtsRE.findall( os )
            for evts in evtsList:
                nevts += int( evts )
            logger.info( "%s events appended." % nevts )
            # Check if the dictionary of counters has been provided.
            # This is used to provide support for arbitrary number of counters that
            # may be used by the verify*merge() methods since each invocation of 
            # the method is done on a single segment.
            try:
                cDict[ 'nevts' ] += nevts
                nevts = cDict[ 'nevts' ] # reset nevts to total counted.
            except:
                cDict[ 'nevts' ] = nevts
            logger.info( "Total events appended: %s" % nevts )

    retcode = 0
    acronym = 'OK'
    stdout = 'trf finished OK'
    if mainscript_output_pair[0] != 0:
        logger.error( "Problem in TAG merging!" )
        retcode = mainscript_output_pair[0]
        acronym = 'TAGMERGE_EXEPROBLEM'
        stdout = 'TAG merging problem: ' + mainscript_output_pair[1]
    elif post_retcode != 0 : 
        logger.error( "TAG merging error found in logfile!" )
        retcode = post_retcode
        acronym = 'TAGMERGE_ERROR'
        stdout = 'TAG merging error: ' + post_stdout
    return { 'retcode' : retcode, 'acronym' : acronym, 'stdout' : stdout, 'nevts' : nevts }

MERGEDICT = { 'ntuple': { 'scriptpath' : 'hadd',
                          'usage' : '%(scriptpath)s %(extraopts)s %(outputfile)s %(inputfilelist)s',
                          'description' : 'Merge NTUPLE files using hadd',
                          'splitinputfilelist' : False,
                          'extraopts' : '',
                          'inputfilelistseparator' : ' ',
                          'inputfileprefix' : '',
                          'inputfilepostfix' : '',
                          'outputfileprefix' : '',
                          'outputfilepostfix' : '',
                          'prescript' : '',
                          'postscript' : 'grep -i error %(mainscript_stdout)s',
                          'verifymerge' : verifyNTUPLEmerge,
                          'posttrfaction' : ''
                        },
              'tag':    { 'scriptpath' : 'export ROOTENV_NO_HOME=1; CollAppend',
                          'usage' : '%(scriptpath)s -src %(inputfilelist)s -dst %(outputfile)s %(extraopts)s',
                          'description' : 'Merge TAG files with CollAppend (AtlasPOOL)',
                          'splitinputfilelist' : True,
                          'extraopts' : '-nevtcached 5000 -merge',
                          'inputfilelistseparator' : ' ',
                          'inputfileprefix' : 'PFN:',
                          'inputfilepostfix' : ' RootCollection',
                          'outputfileprefix' : '',
                          'outputfilepostfix' : ' RootCollection',
                          'prescript' : '',
                          'postscript' : 'grep -i error %(mainscript_stdout)s | grep -v \"does not exist\" | grep -v hlterror | grep -v segmentation',
                          'verifymerge' : verifyTAGmerge,
                          'posttrfaction' : 'mv %(outputfile)s.root %(outputfile)s'
                        },
              'tag1':   { 'scriptpath' : 'export ROOTENV_NO_HOME=1; AthCollAppend',
                          'usage' : '%(scriptpath)s -src %(inputfilelist)s -dst %(outputfile)s %(extraopts)s',
                          'description' : 'Merge TAG files with AthCollAppend (Atlas)',
                          'splitinputfilelist' : True,
                          'extraopts' : '-nevtcached 5000 -merge',
                          'inputfilelistseparator' : ' ',
                          'inputfileprefix' : 'PFN:',
                          'inputfilepostfix' : ' RootCollection',
                          'outputfileprefix' : '',
                          'outputfilepostfix' : ' RootCollection',
                          'prescript' : '',
                          'postscript' : 'grep -i error %(mainscript_stdout)s | grep -v \"does not exist\" | grep -v hlterror | grep -v segmentation',
                          'verifymerge' : verifyTAGmerge,
                          'posttrfaction' : 'mv %(outputfile)s.root %(outputfile)s'
                        }
            }


class GenericError( Exception ):
    """
    Simple wrapper for errors.
    """
    def __init__( self, msg=None ):
        if msg is None:
            self.errMsg = ''
        else:
            self.errMsg = msg

    def __repr__( self ):
        return self.errMsg

    def __str__( self ):
        return self.errMsg

class MergeTrfInternalError( GenericError ):
    def __init__( self, msg ):
        self.errMsg = "Contact merge_trf.py developers: %s" % msg

class MissingArgument( GenericError ):
    pass

class ArgumentError( GenericError ):
    pass

class MergeFailure( GenericError ):
    pass

class UnknownError( GenericError ):
    pass

class PythonVersionNotSupported( GenericError ):
    pass

# Check Python version e.g. versionTuple=(2,3,4)
def _pythonVersionCheck( *versionTuple ):
    defVersion = ( 2, 4, 0 )
    if versionTuple is ():
        versionTuple = defVersion
    if sys.version_info[ :len(versionTuple) ] < versionTuple:
        raise PythonVersionNotSupported( "Python version %s and above required." % versionTuple.__repr__() )

def _check_filemerge_list( option, opt, value ):
    mergedList = []
    for f in trfutil.expandStringToList( value ):
        if not fileutil.exists( f ):
            found = fileutil.exists_suffix_number( f + '.' )
            if not found:
                raise OptionValueError( "Option %s: %s does not exist." % ( opt, f ) )
            f = found
        mergedList.append( f )
    return mergedList

def _mergeDictDescription():
       return ', '.join( [ '"%s":%s' % ( mMethod, mDict['description'] ) for mMethod, mDict in MERGEDICT.items() ] )

# Extending the Option class to allow it to recognise the list of numbers used.
class FileListOption( Option ):
    TYPES = Option.TYPES + ("filemerge_list",)
    TYPE_CHECKER = copy( Option.TYPE_CHECKER )
    TYPE_CHECKER[ "filemerge_list" ] = _check_filemerge_list

oParser = OptionParser( option_class = FileListOption, usage = USAGE, version = VERSION ) 

oParser.add_option( "-m", "--mergemethod",
                     type = "string",
                     dest = "mergemethod",
                     help = "Choose between %s merge methods. %s." % ( MERGEDICT.keys(), _mergeDictDescription() ) )
oParser.add_option( "-i", "--inputfilelist", 
                     type = "filemerge_list", 
                     dest = "inputfilelist", 
                     help = "List of files e.g. abc.def._[0001,0002,0003].uvw.xyz" )
oParser.add_option( "-o", "--outputfile", 
                     type = "string", 
                     dest = "outputfile", 
                     help = "Output file containing the merged content." )
oParser.add_option( "-a", "--additionalmergedictfile",
                     type = "string",
                     dest = "additionalmergedictfile",
                     help = "additionalmergedictfile argument is of the form 'package.modulepath'. It refers to an additional Python module containing a MERGEDICT to provide support for additional merge methods." )
oParser.add_option( "", "--extraopts", 
                     type = "string", 
                     dest = "extraopts", 
                     help = "Extra options to be passed to the merge application." )

# Retrieval of key/value arguments.
def retrieveKVArgs( kwArgs ):
    kwDict = {}
    for kwArg in kwArgs:
        try:
            k,v = kwArg.split( '=' )
        except:
            raise ArgumentError( "Expecting argument in the form of key=value but got %s instead." % kwArg )
        option = oParser.get_option( "--%s" % k )
        if option and option.type == "filemerge_list":
            v = _check_filemerge_list( None, "--%s" % k, v )
        kwDict[ k ] = v
    return kwDict

# Retrieval of positional arguments.
def retrievePArgs( pArgs ):
    kwDict={}
    pos = 2 # start with the third option as -v/--version and -h/--help is the first two entries.
    for pArg in pArgs:
        option = oParser.option_list[ pos ]
        k = option.dest
        if option.type == "filemerge_list":
            pArg = _check_filemerge_list( None, "--%s" % k, pArg )
        kwDict[ k ] = pArg
        pos += 1
    return kwDict

# Retrieve the arguments at the command line
def retrieveArguments():
    # Parse arguments to retrieve 'option'-styled arguments
    # If other styles are used, the arguments will be stored in args.
    (options, args) = oParser.parse_args()
    
    # option arguments have priority over positional and key/value arguments.
    if options.inputfilelist is not None or options.outputfile is not None and options.mergemethod is not None:
        if args:
            logger.warning( "All positional and key/value arguments will be ignored!" )
        return { 'mergemethod' : options.mergemethod.lower(), 'inputfilelist' : options.inputfilelist, 'outputfile' : options.outputfile, 'additionalmergedictfile' : options.additionalmergedictfile, 'extraopts' : options.extraopts }
    elif not args:
        raise MissingArgument( "No arguments provided!" )
    else: # decide if user is using positional or key/value arguments. Positional arguments have the lowest priority.
        pArgs = []
        kwArgs = []
        tempDict = { True : kwArgs, False : pArgs }
        for arg in args:
            tempDict[ '=' in arg ].append( arg )
        if kwArgs: # key/value arguments exist. They will take precedence over positional arguments
            if pArgs:
                logger.warning( "All positional arguments will be ignored!" )
            return retrieveKVArgs( kwArgs )
        elif pArgs:
            return retrievePArgs( pArgs )
        else:
            raise UnknownError( "Arguments \n%s\nshould be present but not captured!" % args )

def execMerge( mergemethod, inputfilelist, outputfile, additionalmergedictfile = None, extraopts = None ):
    # load MERGEDICT from specified file and add new MERGEDICT entries.
    if additionalmergedictfile is not None:
        try:
            module = __import__( additionalmergedictfile, globals(), locals(), [ 'MERGEDICT' ] )
        except ImportError, msg:
            logger.warning( "Could not open additional merge dictionary file: %s" % msg )
        else:
            try:
                MERGEDICT.update( module.MERGEDICT )
            except Exception, msg:
                logger.warning( "Could not update MERGEDICT." % msg )
    if extraopts is None:
        extraopts = ''

    # pick the required MERGEDICT entry
    try:
        md = MERGEDICT[ mergemethod ]
    except KeyError, msg:
        logger.debug( "execMerge(): %s" % msg )
        raise MergeFailure( "Merge method %s not available." % mergemethod )

    # Add extra options
    md[ 'extraopts' ] += ' %s' % extraopts

    # finalise outputfile
    if md[ 'outputfileprefix' ] or md[ 'outputfilepostfix' ]:
        outputfile1 = "%s%s%s" % ( md[ 'outputfileprefix' ], outputfile, md[ 'outputfilepostfix' ] )
    else:
        outputfile1 = outputfile

    # finalise inputfilelist. Depending on 'splitinputfilelist', the merge may
    # have to be executed for individual input files. This is achieved 
    # easily by looping over inputfilelistoflists which may be a list of lists 
    # e.g. [[a],[b],[c],...] or a [[a,b,c,...]].
    if md[ 'splitinputfilelist' ]:
        inputfilelistoflists = [ [ "%s%s%s" % ( md[ 'inputfileprefix' ], x, md[ 'inputfilepostfix' ] ) ] for x in inputfilelist ]
    else:
        inputfilelistoflists = [ [ "%s%s%s" % ( md[ 'inputfileprefix' ], x, md[ 'inputfilepostfix' ] ) for x in inputfilelist ] ]

    logger.info( "############  ATLAS %s-File Merging  ############" % mergemethod.upper() )
    mainscript_output_pair = [] # Using a list as a mutable tuple containing the  return code and stdout from the execution of the mainscript.
    c = 0
    n_segments = len( inputfilelistoflists )
    outDict = { 'retcode' : 1 } # dummy values
    t_startmerge = time.time()
    counterDict = {}
    
    for inputfilelist1 in inputfilelistoflists:
        c+=1
        inputfilelist_str = md[ 'inputfilelistseparator' ].join( inputfilelist1 ) # single long string

        # finalise command
        tempDict = md.copy()
        tempDict[ 'inputfilelist' ] = inputfilelist_str
        tempDict[ 'outputfile' ] = outputfile1
        command = md[ 'usage' ] % tempDict
        del tempDict
        logger.info( "##---------  Attempting to merge segment %d of %d.  ---------##" % (c,n_segments) )
        logger.info( "Input file(s): " )
        logger.info( inputfilelist_str )

        # execute Prescript
        t_prescript_start = time.time()

        if md['prescript']:
            logger.info( "Prescript: Executing '%s'" % md['prescript'] )
            prescript_returncode, prescript_stdout = commands.getstatusoutput( md['prescript'] )
            logger.info( prescript_stdout )
            logger.info( "Prescript: Return code = %s" % prescript_returncode )
            logger.info( "Prescript: Completed in %d seconds." % int( time.time() - t_prescript_start ) )
        else:
            logger.info( "Prescript: None" )
            prescript_returncode, prescript_stdout = 0, ""

        # execute main merge script
        logger.info( "%s merge: Executing '%s'" % ( mergemethod.upper(), command ) )
        t_mainscript_start = time.time()
        mainscript_returncode, mainscript_stdout = commands.getstatusoutput( command )
        t_mainscript_end = time.time()
        mainscript_output_pair = [ mainscript_returncode, mainscript_stdout ]
        logger.info( "\n" + mainscript_stdout )
        logger.info( "%s merge: Return code = %s" % ( mergemethod.upper(), mainscript_returncode ) )
        logger.info( "%s merge: Completed in %d seconds." % ( mergemethod.upper(), int( t_mainscript_end - t_mainscript_start ) ) )
        # execute Postscript
        if md['postscript']:
            try:
                _segmentLog = file( 'tempLog_TOBEREMOVED_', 'w' )
            except:
                logger.error( "Postscript: Could not create temporary file to facilitate postscript execution." )
            else:
                _segmentLog.write( mainscript_stdout )
                _segmentLog.flush()
                postscript_cmd = md[ 'postscript' ] % { 'mainscript_stdout' : _segmentLog.name }
                logger.info( "Postscript: Executing '%s'" % postscript_cmd )
                postscript_returncode, postscript_stdout = commands.getstatusoutput( postscript_cmd )
                t_postcript_stop = time.time()
                logger.info( "\n"+postscript_stdout )
                logger.info( "Postscript: Return code = %s" % postscript_returncode )
                logger.info( "Postscript: Completed in %d seconds." % int( t_postcript_stop - t_mainscript_end ) )
                # provide the verification method with the return codes, stdout, 
                # the temporary segment specific log file, and a dictionary to allow
                # counters to work across segments.
                outDict.update( md[ 'verifymerge' ]( mainscript_output_pair, postscript_returncode, postscript_stdout, _segmentLog.name, counterDict = counterDict ) )
                _segmentLog.close()
                os.remove( _segmentLog.name )
        else:
            logger.info( "Postscript: None" )
            postscript_returncode, postscript_stdout = 0, ""

        logger.info( "The merging of segment %d of %d took a total of %d seconds." % ( c, n_segments, int( t_mainscript_end - t_mainscript_start ) ) )
        logger.info( "##-------  End of merge attempt of segment %d of %d.  -------##" % (c, n_segments) )

        # Check if the merge was successful. Stop processing if an error is detected.
        if outDict[ 'retcode' ] != 0:
            break
    
    t_endmerge = time.time()
    
    logger.info( "######### End of ATLAS %s-File Merging  #########" % mergemethod.upper() )
    
    # execute Post TRF action
    posttrfaction_cmd = md['posttrfaction'] % { 'outputfile' : outputfile }
    if posttrfaction_cmd:
        logger.info( "PostTRFaction: Executing '%s'" % posttrfaction_cmd )
        posttrfaction_returncode, posttrfaction_stdout = commands.getstatusoutput( posttrfaction_cmd )
        logger.info( "\n"+posttrfaction_stdout )
        logger.info( "PostTRFaction: Return code = %s" % posttrfaction_returncode )
    else:
        logger.info( "PostTRFaction: None" )

    # Generate job report
    logger.info( "Generating job report in jobReport.gpickle" )
    fCount = 0
    # cannot use content in fList directly due to inputfile prefixes and postfixes. 
    # Use original inputfilelist instead.
    for fList in inputfilelistoflists[:c]: 
        fCount += len( fList )
    
    outDict.update( { 'timing': int( t_endmerge-t_startmerge ), 'infiles': inputfilelist[:fCount], 'outfiles' : [ outputfile ]  } )
    # create jobReport.gpickle
    try:
        f = open( 'jobReport.gpickle', 'w' )
    except Exception, msg:
        logger.error( "Could not create jobReport.gpickle file" % msg )
    else:
        pickle.dump( createJobReportDict( **outDict ), f )
        f.close()
    
    # generate metadata.xml
    # Instantiating a JobReport to facilitate the creation of metadata.xml
    logger.info( "Generating metadata.xml" )
    from PyJobTransformsCore.JobReport import JobReport, FileInfo 
    from PyJobTransformsCore.trfutil import RootTTreeFile, FileType
    guid = FileType().getGUID( outputfile )
    outFileInfo = FileInfo( outputfile, guid )
    
    # Modification of key(s) in outDict (so as not to complicate matters in AMI)
    try:
        outDict[ 'events' ] = outDict.pop( 'nevts' )
    except KeyError:
        pass
    
    outFileInfo.addMetaData( outDict )
    jobReport = JobReport()
    #jobReport.setProducer( "%s_merge" % mergemethod )
    jobReport.addOutputFile( outFileInfo )
    jobReport.writeMetaDataXML( False )


def createJobReportDict( **kwargs ):
    outDict = { 'retcode' : 1, 'acronym' : 'TRF_NOT_OK', 'outfiles' : [],
                'infiles' : [], 'nevts' : 0, 'timing' : 0, 'stdout' : "" }
    outDict.update( kwargs )
    jobReport = { 'prodsys' : { 'trfCode' : outDict[ 'retcode' ],
                                'trfAcronym' : outDict[ 'acronym' ],
                                'jobOutputs': outDict[ 'outfiles' ],
                                'jobInputs' : outDict[ 'infiles' ],
                                'more' : { 'num1' : outDict[ 'nevts' ], 
                                           'num2' : outDict[ 'timing' ], 
                                           'txt1' : outDict[ 'stdout' ]
                                         }
                              }
                }
    return jobReport
    

if __name__ == '__main__':
    try:
        _pythonVersionCheck()
    except PythonVersionNotSupported, msg:
        logger.critical( msg )
        sys.exit( -3 )
    except Exception, msg:
        logger.critical( msg )
        sys.exit( -3 )

    try:
        kwDict = retrieveArguments()
    except MissingArgument, msg:
        logger.critical( "In retrieveArguments(): %s" % msg )
        print oParser.print_help()
        sys.exit( -2 ) # use optparse's help facility
    except ArgumentError, msg:
        logger.critical( "In retrieveArguments(): %s" % msg )
        sys.exit( -2 )
    except UnknownError, msg:
        logger.critical( "In retrieveArguments(): %s" % msg )
        sys.exit( -2 )
    except Exception, msg:
        logger.critical( "In retrieveArguments(): %s" % msg )
        sys.exit( -2 )

    try:
        execMerge( **kwDict )
    except MergeFailure, msg:
        logger.critical( "Error executing merge: %s" % msg )
        sys.exit( -1 )
    except MergeTrfInternalError, msg:
        logger.critical( "Merge TRF internal error: %s" % msg )
        sys.exit( -1 )
    except KeyError, msg:
        logger.critical( "Missing MERGEDICT entrie(s): %s" % msg )
        sys.exit( -1 )
    except Exception, msg:
        logger.critical( "Unexpected error: %s" % msg )
        sys.exit( -1 )
    else:
        sys.exit( 0 )
