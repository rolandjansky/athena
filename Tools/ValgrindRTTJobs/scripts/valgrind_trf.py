#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import math, os, time
from PyJobTransformsCore.trf import *
from PyJobTransformsCore.trfutil import *
from PyJobTransformsCore.full_trfarg import *

__doc__ ="""Generate events for physics process defined in jobOptions file given in jobConfig argument, run Atlfast"""

import thread, os

def TopObserver(ToolName):
    print 'Spying the valgrind memory consumption in thread with top.'
    # os.system( ' while [ ! -e done ]; do top -b -n 1 | grep memcheck >> top.log 2>&1 ; sleep 5 ; done ' )
    os.system( ' while [ ! -e done ]; do top -b -n 1 | grep -e ' + ToolName + ' -e athena.py >> top.log 2>&1 ; sleep 5 ; done ' )
    print 'we are done !'
    
class ValgrindVersionArg(StringArg):
    """Valgrind Version"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

class ValgrindToolArg(StringArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

class ValgrindToolOptArg(StringArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

class ValgrindGenSuppressionArg(StringArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

class StacktraceDepthArg(IntegerArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        IntegerArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

class JobOptionsArg(StringArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

class NeedInputFileArg(StringArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True
    
class CheckFilesArg(StringArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

class AthenaOptArg(StringArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

class RecoTrfArg(StringArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

class ReqCmtConfigArg(StringArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        StringArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

# some tool options
class TrackOriginsArg(BoolArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        BoolArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

class DumpInstrArg(BoolArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        BoolArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

class InstrAtStartArg(BoolArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        BoolArg.__init__(self,help,name)
    
    def isFullArgument(self):
        return True

class RecExCommonLinksArg(BoolArg):
    """JobOptions"""
    def __init__(self,help='default',name='default'):
        BoolArg.__init__(self,help,name)
        
    def defaultHelp(self):
         help = Argument.defaultHelp(self)
         choicesHelp = self.choicesHelp()
         if choicesHelp: help += '. ' + choicesHelp
         return help

    def isFullArgument(self):
        return True

class ValgrindJobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__( self,
                               authors=[ Author('Rolf Seuster', 'seuster AT cern.ch') ] ,
                               help=__doc__ )
        # add arguments
        # self.add( MaxEventsArg() )
        self.add( JobOptionsArg(), 'HelloWorldOptions.py' )
        self.add( NeedInputFileArg(), 'None' )
        self.add( RecExCommonLinksArg(), False )
        self.add( TrackOriginsArg(), True )
        self.add( DumpInstrArg(), False )
        self.add( InstrAtStartArg(), True )
        self.add( CheckFilesArg(), 'Nothing' )
        self.add( ValgrindToolArg(), 'None' )
        self.add( ValgrindToolOptArg(), 'None' )
        self.add( ValgrindGenSuppressionArg(), 'None' )
        self.add( RecoTrfArg(), 'None' )
        self.add( AthenaOptArg(), 'None' )
        self.add( ReqCmtConfigArg(), 'None' )
        self.add( ValgrindVersionArg(), '3.8.1' )
        self.add( StacktraceDepthArg(), 5 )
        
    def runJob(self):
        """Run the athena job. Returns JobReport with the result of the run.
        Can be overridden in derived class, typically for a composite transform,
        running several athena jobs."""
        
        self.logger().info( 'Using %s' % ( trfenv.trfPath) )
        
        #
        # start with the pre-run actions
        #
        self.doPreRunActions()
        # gather environment only after preRunActions, as they may change the environment
        self.gatherEnvironmentInfo()
        
        # Prepare for running athena job
        #
        # open the logfile
        logFile = fileutil.Tee(self._logFilename,'a')
        
        #
        # get maximum event number, valgrind version and jobOptions to process
        #
        # maxEventsArg = self.getArgumentOfType("MaxEvents")
        # maxEvents = maxEventsArg and maxEventsArg.value()
        # self.logger().info( 'In runJob() %d' % ( maxEvents ) )
        
        stacktraceDepthArg = self.getArgumentOfType("StacktraceDepth")
        stacktraceDepth = stacktraceDepthArg and stacktraceDepthArg.value()
        self.logger().info( ' valgrind monitors up to stacktraceDepth: %s' % ( stacktraceDepth ) )
        
        valgrindToolArg = self.getArgumentOfType("ValgrindTool")
        valgrindTool = valgrindToolArg and valgrindToolArg.value()
        self.logger().info( ' will use valgrind tool %s' % ( valgrindTool ) )
        
        valgrindToolOptArg = self.getArgumentOfType("ValgrindToolOpt")
        valgrindToolOpt = valgrindToolOptArg and valgrindToolOptArg.value()
        self.logger().info( ' will use these valgrind tool options %s' % ( valgrindToolOpt ) )
        
        valgrindGenSuppressionArg = self.getArgumentOfType("ValgrindGenSuppression")
        valgrindGenSuppression = valgrindGenSuppressionArg and valgrindGenSuppressionArg.value()
        self.logger().info( ' will produce suppressions file %s' % ( valgrindGenSuppression ) )
        
        valVersionArg = self.getArgumentOfType("ValgrindVersion")
        valVersion = valVersionArg and valVersionArg.value()
        self.logger().info( ' will use valgrind version %s' % ( valVersion ) )
        
        jobOptionsArg = self.getArgumentOfType("JobOptions")
        jobOptions = jobOptionsArg and jobOptionsArg.value()
        self.logger().info( ' processing these jobOptions: %s' % ( jobOptions ) )
        
        needInputFileArg = self.getArgumentOfType("NeedInputFile")
        needInputFile = needInputFileArg and needInputFileArg.value()
        self.logger().info( ' need some input file: %s' % ( needInputFile ) )
        
        checkFilesArg = self.getArgumentOfType("CheckFiles")
        checkFiles = checkFilesArg and checkFilesArg.value()
        self.logger().info( ' doing checks on produced files: %s' % ( checkFiles ) )
        
        recoTrfArg = self.getArgumentOfType("RecoTrf")
        recoTrf    = recoTrfArg and recoTrfArg.value()
        self.logger().info( ' calling jobTransform in this way %s' % ( recoTrf.replace('+', ' ').replace('#', '+') ) )
        
        athenaOptArg = self.getArgumentOfType("AthenaOpt")
        athenaOpt    = athenaOptArg and athenaOptArg.value()
        self.logger().info( ' calling athena with these optional arguments %s' % ( athenaOpt ) )

        trackOriginsArg = self.getArgumentOfType("TrackOrigins")
        trackOrigins    = trackOriginsArg and trackOriginsArg.value()
        self.logger().info( ' track origins of uninitialized variables (memory hungry !): %s' % ( trackOrigins ) )
        
        dumpInstrArg = self.getArgumentOfType("DumpInstr")
        dumpInstr    = dumpInstrArg and dumpInstrArg.value()
        self.logger().info( ' callgrind: collect at instruction level: %s' % ( dumpInstr ) )

        instrAtStartArg = self.getArgumentOfType("InstrAtStart")
        instrAtStart    = instrAtStartArg and instrAtStartArg.value()
        self.logger().info( ' callgrind: start collecting at start of program: %s'% ( instrAtStart ) )
        
        recExCommonLinksArg = self.getArgumentOfType("RecExCommonLinks")
        recExCommonLinks = recExCommonLinksArg and recExCommonLinksArg.value()
        self.logger().info( ' run RecExCommon_links.sh before: %s' % ( recExCommonLinks ) )
        
        reqCmtConfigArg = self.getArgumentOfType("ReqCmtConfig")
        reqCmtConfig    = reqCmtConfigArg and reqCmtConfigArg.value()
        self.logger().info( ' requiring CMTCONFIG for this job to be %s' % ( reqCmtConfig ) )
        
        self.logger().info( 'Doing some tests before executing valgrind' )
        tmp = os.system( ' which athena.py > /dev/null ' )
        self.logger().info( ' Do we have athena setup ? %s' % ( tmp ) )
        
        # determine the platform from uname and modify for valgrind usage
        platform = os.uname()[4]
        valgrindMachPath='ia32'
        self.logger().info( ' will use as platform ? %s' % ( platform ) )
        if platform == "x86_64":
            valgrindMachPath='amd64'
            
        gzip_logfile=False
        tmp = os.environ['CMTCONFIG']
        mach = tmp.split('-')
        if len(mach) != 4:
            self.logger().error( 'Cannot determine linux versions and compiler from $CMTCONFIG !! It has %d instead of 4 elements !' % ( len(mach) ) )
            exit(0)
            
        self.logger().info( ' will also use : %s %s' % ( mach[1], mach[2] ) )
        # valgrindBasePath='/afs/cern.ch/sw/lcg/external/valgrind/3.4.1/' + mach[1] + '_' + valgrindMachPath + '_' + mach[2]
        valgrindBasePath='/afs/cern.ch/user/r/rig/sw/vg-3.8.1'
        if valVersion == "3.8.0":
            valgrindBasePath='/afs/cern.ch/user/r/rig/sw/vg-3.8.0'
            gzip_logfile=True
        if valVersion == "3.6.1":
            valgrindBasePath='/afs/cern.ch/user/r/rig/sw/vg-3.6.1'
            gzip_logfile=True
        if valVersion == "trunk":
            valgrindBasePath='/afs/cern.ch/user/r/rig/sw/vg-trunk'
            gzip_logfile=True
            
        self.logger().info( 'Setting up valgrind from located at %s' % ( valgrindBasePath ) )
        
        proName = os.environ['AtlasProject']
        tmp = os.path.split(os.getcwd())[0]
        jobName = os.path.split(tmp)[1]
        relName = os.environ['AtlasVersion']
        
        vetoed = False
        
        #fll=os.environ["FRONTIER_LOG_LEVEL"]
        #fsv=os.environ["FRONTIER_SERVER"]
        #os.environ.pop("FRONTIER_LOG_LEVEL")
        #os.environ.pop("FRONTIER_SERVER")
        
        if ( reqCmtConfig != 'None' ):
            self.logger().info( ' specific CMTCONFIG required : %s ' % reqCmtConfig )
            vetoed = True
            tmp = os.environ['CMTCONFIG']
            if ( tmp.find(reqCmtConfig) > -1 ):
                vetoed = False
                
            # if ( tmp.find("x86_64") > -1 ):
            #     vetoed = True
                
            if vetoed:
                self.logger().info( ' Vetoed: Yes, we will NOT run valgrind !' )
                os.system('touch vetoed')
                self.logger().info( ' Exiting !' )
                sys.exit(2)
            else:
                self.logger().info( ' Not vetoed, so we will run valgrind !' )
                
        if os.access(valgrindBasePath, os.F_OK):
            self.logger().info( ' Everything looks OK, get now the jobOptions %s ' % jobOptions )
            get_files ( jobOptions, fromWhere='JOBOPTSEARCHPATH', depth=1, sep=',' )
            
            self.logger().info( ' Everything looks OK, get now the suppression file(s)' )
            get_files ( 'valgrindRTT.supp' )
            get_files ( 'Gaudi.supp/Gaudi.supp' )
            get_files ( 'root.supp/root.supp' )
            get_files ( 'newSuppressions.supp' )
            get_files ( 'oracleDB.supp' )
            # get_files ( 'valgrind-python.supp/valgrind-python.supp' )
            
            # add valgrind to PATH and LD_LIBRARY_PATH
            os.environ['PATH'] = valgrindBasePath + '/bin' + ':' + os.environ['PATH']
            os.environ['LD_LIBRARY_PATH'] = valgrindBasePath + '/lib' + ':' + os.environ['LD_LIBRARY_PATH']
            
            tmp = os.system('which valgrind > /dev/null')
            self.logger().info( ' do we have valgrind setup correctly ? %s' % ( tmp ) )
            
            # somebody compiles in tmp directory, as valgrind also follows this compilation, we
            # need to supply absolute paths to all suppression files
            currentWorkArea = os.getcwd()
            
            if ( recExCommonLinks ):
                self.logger().info( ' executing RecExCommon_links.sh' )
                tmp = os.system('RecExCommon_links.sh > /dev/null')
                
            # tmp = os.system('setupLocalDBReplica_CERN.sh COMP200')
            # self.logger().info( ' executing RecExCommon_links.sh' )
            
            # toolOptions     = ' --leak-check=yes --show-reachable=yes --log-file=valgrind.out.process.\%p '
            toolOptions    = ' --leak-check=yes --log-file=valgrind.out.process.\%p '
            
            valgrindOptions = ' --trace-children=yes --track-fds=yes '
            # valgrindOptions = ' --trace-children=yes --track-fds=yes '
            # only in 3.5.0: 
            if valVersion == "3.6.1":
                valgrindOptions = valgrindOptions + ' --read-var-info=yes'
            
            valgrindOptions = valgrindOptions + ' --num-callers=' + str(stacktraceDepth)
            valgrindOptions = valgrindOptions + ' --suppressions=' + currentWorkArea + '/valgrindRTT.supp '
            valgrindOptions = valgrindOptions + ' --suppressions=' + currentWorkArea + '/newSuppressions.supp '
            valgrindOptions = valgrindOptions + ' --suppressions=' + currentWorkArea + '/oracleDB.supp '
            
            # add other suppressions files:
            valgrindOptions+= '--suppressions=' + currentWorkArea + '/Gaudi.supp/Gaudi.supp '
            valgrindOptions+= '--suppressions=' + currentWorkArea + '/root.supp/root.supp '
            
            # use suppression file shipped with root, if it exists
            # probably 99.99% overlap with ATLAS own's but might be a bit newer
            root_supp_path = os.path.expandvars('$ROOTSYS/etc/valgrind-root.supp')
            if os.path.exists( root_supp_path ):
                valgrindOptions+= '--suppressions=$ROOTSYS/etc/valgrind-root.supp '
            # --trace-children-skip=/bin/sh,cmt.exe
                
            athenaOptions   = ' `which python` `which athena.py` '
            
            pre=' -c "'
            post=' '
            if ( athenaOpt != 'None' ):
                athenaOptions += pre + athenaOpt
                pre =';'
                post='" '
                
            if ( needInputFile == "EVGEN" ):
                self.logger().info( ' EVGEN file requested, now copying' )
                tmp = os.system('cp /afs/cern.ch/atlas/maxidisk/d33/referencefiles/Sim/EVGEN.pool.root EVGEN.pool.root')
                
            if ( recoTrf != "None" ):
                
                if ( needInputFile != 'None' ):
                    self.logger().info( ' optionally using input file of type: %s ' % needInputFile )
                    if ( needInputFile == "ESD" ):
                        # tmp = os.system('cp /afs/cern.ch/atlas/maxidisk/d33/releases/latest.ESD.pool.root ESD.pool.root')
                        tmp = os.system('cp /afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_ESD.AOD_50Events/data10_7TeV.00167607.physics_JetTauEtmiss.recon.ESD.f298._lb0087._SFO-4._0001.1_50Events_rel.16.0.3.8_rereco ESD.pool.root')
                        
                    if ( needInputFile == "AOD" ):
                        tmp = os.system('cp /afs/cern.ch/atlas/maxidisk/d33/releases/latest.AOD.pool.root AOD.pool.root')
                
                from subprocess import Popen, PIPE
                cmd = recoTrf.replace('+', ' ').replace('#', '+')
                
                # first check, that all arguments are allowed (outputfiles often change ...)
                output = Popen([cmd.split()[0] + ' -h'],stdout=PIPE, shell=True).communicate()[0]
                # self.logger().info( 'Here is the output : ' )
                # for outline in output.split('\n'):
                #  self.logger().info( 'PIPE >' + outline + '<' )
                allfound=True
                newcmd=""
                sep=""
                for args in cmd.split():
                    # self.logger().info( 'CMD >' + args + '<' )
                    thisargstr=args
                    if ( args.find("=")>=0 ):
                        thisargstr=args.split("=")[0] + '< with value >' + args.split("=")[1]
                        
                    if ( output.find(args.split("=")[0]) > 0 or args.split("=")[0].startswith("append_pre") ):
                        self.logger().info( 'FOUND ARG >' + thisargstr + '<' )
                        newcmd += sep + args
                        sep = " "
                    else:
                        self.logger().info( ' NOT FOUND ARG >' + thisargstr + '<' )
                        # an argument for the job transform wasn;t found !
                        # remove it
                        allfound=False
                        
                self.logger().info( 'ARG CMP old>' + cmd + '<' )
                self.logger().info( 'ARG CMP new>' + newcmd + '<' )
                if ( cmd != newcmd ):
                    cmd = newcmd
                
                self.logger().info( 'running dummy jobTransform to create pickle file' )
                cmd += ' --athenaopts="--config-only=rec.pkg --keep-configuration" --omitvalidation=ALL '
                self.logger().info( ' cmd : ' + cmd )
                output = Popen([cmd],stdout=PIPE, shell=True).communicate()[0]
                self.logger().info( 'Here is the output :' )
                for outline in output.split('\n'):
                    self.logger().info( 'TRF >' + outline + '<' )
                    
                os.system('ls -la >& dir.log')
                
            else:
                
                if ( needInputFile != 'None' ):
                    self.logger().info( ' reco_trf will use input file of type: %s ' % needInputFile )
                    if ( needInputFile == "ESD" ):
                        athenaOptions += pre + 'from AthenaCommon.AthenaCommonFlags import athenaCommonFlags;athenaCommonFlags.FilesInput=[\'/afs/cern.ch/atlas/maxidisk/d33/releases/latest.ESD.pool.root\']'
                        pre =';'
                        post='" '
                        
                    if ( needInputFile == "AOD" ):
                        athenaOptions += pre + 'from AthenaCommon.AthenaCommonFlags import athenaCommonFlags;athenaCommonFlags.FilesInput=[\'/afs/cern.ch/atlas/maxidisk/d33/releases/latest.AOD.pool.root\']'
                        pre =';'
                        post='" '
                        
                cmd = athenaOptions + post + jobOptions
                cmd += ' --config-only=rec.pkg --keep-configuration'
                
                from subprocess import Popen, PIPE
                self.logger().info( 'running dummy athena job to create pickle file' )
                self.logger().info( ' cmd : ' + cmd )
                output = Popen([cmd],stdout=PIPE, shell=True).communicate()[0]
                self.logger().info( 'Here is the output :' )
                for outline in output.split('\n'):
                    self.logger().info( 'TRF >' + outline + '<' )
                    
                os.system('ls -la >& dir.log')

            athenaOptions += post

            athenaOptions += ' --stdcmalloc rec.pkg.pkl > out.valgrind.log 2>&1 '
            
            toolName = 'memcheck'
            
            trackOriginsStr = ''
            if ( trackOrigins ):
                trackOriginsStr = '--track-origins=yes'
                
            # toolOptions += " --malloc-fill=0xF0 --free-fill=0xF --error-limit=no --main-stacksize=268435456 --max-stackframe=33554432 " + trackOriginsStr
            toolOptions += " --malloc-fill=0xF0 --free-fill=0xF --error-limit=no " + trackOriginsStr
            
            if ( valgrindTool == "callgrind" ):
                toolOptions = ' --tool=callgrind --callgrind-out-file=callgrind.out.process.\%p '
                if dumpInstr:
                    toolOptions += ' --dump-instr=yes '
                if not instrAtStart:
                    toolOptions += ' --instr-atstart=no '
                toolName = valgrindTool
                
            if ( valgrindTool == "massif" ):
                toolOptions = ' --tool=massif --massif-out-file=massif.out.process.\%p --detailed-freq=1 --max-snapshots=300'
                toolName = valgrindTool
                
            if ( valgrindTool == "dhat" ):
                toolOptions = ' --tool=exp-dhat --show-top-n=2500 '
                toolName = valgrindTool
                
            if ( valgrindTool == "ptrcheck" ):
                toolOptions = ' --tool=exp-ptrcheck --error-limit=no '
                # to speed up ptrcheck, one can add --enable-sg-checks=no
                toolName = valgrindTool
                
            if ( valgrindGenSuppression == "yes" ):
                toolOptions += ' --gen-suppressions=all'

            if ( valgrindToolOpt != 'None' ):
                toolOptions += valgrindToolOpt
            
            self.logger().info( ' now starting TopObserver !' )
            thread.start_new_thread( TopObserver, ( toolName, ) )
            
            if not vetoed:
                self.logger().info( ' now starting valgrind !' )
                string = ' /usr/bin/time valgrind ' + toolOptions + valgrindOptions + athenaOptions
                self.logger().info( ' now calling valgrind with : <%s>' % string )
                tmp = os.system( string )
                if gzip_logfile:
                    tmp = os.system( "gzip out.valgrind.log" )
                # tmp = os.system( athenaOptions )
            else:
                self.logger().info( ' job was vetoed ! based on %s %s' % ( proName, jobName ))
                tmp = os.system( 'touch veto.log' )
                
            # testing, no valgrind ...
            # tmp = os.system( ' athena.py --stdcmalloc ' + jobOptions + ' > out.valgrind.log 2>&1 ' )
            
            self.logger().info( ' valgrind returned with code %s' % ( tmp ) )
            tmp = os.system('touch done')
            time.sleep(5)
            
            
            if ( valgrindTool == "callgrind" ):
                tmp = os.system( "mkdir t; cp callgrind.out.process.`grep 'ValgrindHelperAlg\ *INFO ValgrindHelperAlg:' out.valgrind.log | gawk '{ print $NF }'` t; gzip callgrind.out.process.*" )
                
            # clean up the local copies of the input files
            if ( recoTrf != "None" ):
                if ( needInputFile != 'None' ):
                    self.logger().info( ' cleaning up local copies of the input file of type: %s ' % needInputFile )
                    if ( needInputFile == "ESD" ):
                        tmp = os.system('rm -f ESD.pool.root')
                        
                    if ( needInputFile == "AOD" ):
                        tmp = os.system('rm -f AOD.pool.root')
                        
            if ( needInputFile == "EVGEN" ):
                self.logger().info( ' removing local copy of EVGEN file' )
                tmp = os.system('rm -f EVGEN.pool.root')
            
            #if ( checkFiles == 'diff' and needInputFile != 'None' ):
            #    tmp = os.system('diffPoolFiles.py -r ' + needInputFile + '.pool.root -f copy_' + needInputFile + '.pool.root > diffFiles.log 2>&1')
            #    
            #if ( checkFiles.beginsWith('check:') ):
            #    tmp = os.system('checkFile.py ' + checkFiles.rpartition(':') + ' > checkFile.log 2>&1')
                
            athenaReport = JobReport()

            thread.exit()

        else:
            self.logger().error( ' PATH does not point to a valid valgrind installation ! Cannot run !' )
            self.logger().error( ' PATH: %s' % valgrindBasePath )
            
        # overwrite producer for new errors that are added
        athenaReport.setProducer(self.name(),self.version())
        
        #os.environ["FRONTIER_LOG_LEVEL"]=fll
        #os.environ["FRONTIER_SERVER"]=fsv
        
        return athenaReport
    
# execute it if not imported
if __name__ == '__main__':
    trf = ValgrindJobTransform()
    sys.exit(trf.exeSysArgs().exitCode())
