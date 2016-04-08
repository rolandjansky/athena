# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ScriptWriter write is a base class for lineswriting shell scripts to run the jobs.
The shell scripts are typically written to a run directory, and are submitted to a
queue from there.

The base class provides a default implementation.
"""

from Factory_CmtLinesForScripts   import CmtLinesForScriptsFactory
from os.path                      import join
from releaseToNum                 import releaseToNum
from Factory_SiteSpecifics        import SiteSpecificsFactory
from Tools                        import changePerm
from exc2string2                  import exc2string2

import CMTtools
import os, sys, string

def libLineCalculator(siteSpecifics, mode, gcc_version):
    """
    This function calculates the LD_LIBRARY_PATH line.
    This line will be placed in a shell script that will be run using
    Popen4.
    
    The explicit value of LD_LIBRARY_PATH is written out so the file
    can be run by hand under the same conditions that the p4open object
    sees.
    """

    llp = os.environ.get('LD_LIBRARY_PATH','')
    if siteSpecifics.addToLibs():
        libLine  = 'LD_LIBRARY_PATH='
        libLine += siteSpecifics.addToLibs() + siteSpecifics.pathToOracleInstantClient(gcc_version) + ':$LD_LIBRARY_PATH'
    else:
        libLine = ''

    return libLine
    
def writeToDir(script, logger):    

    ofile = open("runScript.sh","w")
    ofile = ofile.write(script)
    ofile.close()
    
    changePerm("runScript.sh", 02775)
    logger.debug("wrote runscript to = "+os.getcwd())
    logger.debug("current dir: " + os.getcwd())

# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------

class ScriptWriter:
    
    def __init__(self, argsForScripts, logger):
        'Common stuff for all Scriptwriters - use Descriptor base path'

        self.logger         = logger
        self.logger.debug( 'initialising %s' % self.__class__.__name__)
        self.RTTSrcDir      = argsForScripts.RTTSrcDir
        self.RTTLibDir      = argsForScripts.RTTLibDir
        self.siteSpecifics  = argsForScripts.siteSpecifics
        self.workPath       = argsForScripts.workDirs['base']
        self.cmtLines       = argsForScripts.cmtLines
        self.mode           = argsForScripts.mode
        self.runPath        = argsForScripts.runPath
        self.castorSvcClass = '' # might be filled in from argsForScripts
        
        self.athenaCommand = getattr(argsForScripts,'athenaCommand', None)
        
        labels = ['jobGroup', 'runPath', 'castorSvcClass', 'jobTransformCmd', 'errorMessages']
        [setattr(self, l, getattr(argsForScripts, l, None)) for l in labels]

        compiler = CMTtools.decodeCMTCONFIG(argsForScripts.cmtConfig)['compiler']
        self.libLine = libLineCalculator(self.siteSpecifics,
                                         self.mode,
                                         compiler)

        self.logger.debug('libLine: %s' % self.libLine)
        self.pythonPath = string.join(sys.path,':')
        
        self.useSQLite = True

    def makeScript(self):
        raise NotImplementedError
    
    def runScript(self):
        script = self.makeScript()
        writeToDir(script, self.logger)

    def dumpEnvVars(self):
        script  = 'echo "-----------Dumping environment variables-------------"\n'
        script += 'env>env.log\n'
        script += 'echo "-----------------------------------------------------"\n'
        return script

    def toJobRunDir(self):
        return 'cd %s\n' % self.runPath

    def setupLocalSQLiteReplica(self):
        return 'setupLocalDBReplica_CERN.sh\n'

    def setupCASTOR(self):
        if self.castorSvcClass:
            return 'export STAGE_SVCCLASS=%s\n' % self.castorSvcClass
        return 'export STAGE_SVCCLASS=atlasgroupdisk\n'

    def indicateThisIsRTT(self):
        return 'export RTT=True\n'
    
    def postProcessorScript(self):
        rpath = self.runPath
        
        script = 'echo "----- RTT Post Processing Starts ---------"\n'
        script = '#! /bin/bash\n'
        script += 'set +e\n'
        script += 'cd %s\n' % rpath
        for line in self.cmtLines: script += line+'\n'
        script += 'which python\n'
        script += 'PYTHONPATH=%s:%s:$PYTHONPATH\n' % (self.RTTSrcDir, self.RTTLibDir)
        script += 'cd %s\n' % rpath
        script += 'python  %s\n' % os.path.join(rpath, 'testRunner.py')
        script += 'echo "----- RTT Post Processing Ends---------"\n'
        
        return script
    
        
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------

class NullScriptWriter(ScriptWriter):
    def __init__(self, argsForScripts, logger):
        ScriptWriter.__init__(self, argsForScripts, logger)
    
    def runScript(self):
        pass

# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------        

class MonitoringScriptWriter(ScriptWriter):
    def __init__(self, argsForScripts, logger):
        ScriptWriter.__init__(self, argsForScripts, logger)
        
    def makeScript(self):

         script = '#! /bin/bash\n'
         script += 'set +e\n',
         script += 'cd '
         script += self.runPath+'\n'
         for line in self.cmtLines: script += line+'\n'
         script += 'JOBOPTSEARCHPATH=,%s:$JOBOPTSEARCHPATH\n' % self.runPath
         script += 'cd '+self.runPath+'\n'
         script += self.libLine+'\n'
         script += self.dumpEnvVars()
         script += 'echo "one: 1  \n two:2 \n three:3 " > monitoringData.dat\n'
         script += 'echo "four: 4  \n five:5 \n six:6 " > someData.dat\n'
         script += 'echo " ">JobCompleteMarker.txt\n'

         return script
     
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------        

class AthenaScriptWriter(ScriptWriter):
    
    def __init__(self, argsForScripts, logger):
        "Provides a run script for Athena jobs"
        ScriptWriter.__init__(self,  argsForScripts, logger)
        
        self.releasePath            = argsForScripts.releasePath
        self.packageInstalledCMTdir = argsForScripts.cmtDir
        self.package                = argsForScripts.package

    def grabDBfile(self):
        return ''

    def cmtShowUses(self):
        script  = 'cd '+self.packageInstalledCMTdir+'\n'
        script += 'cmt show uses > '+join(self.runPath,'cmtShowUses.log')+'\n'
        return script            
        
    def makeScript(self):
        """
        Default shell script for running an Athena job.
        """
        
        script = '#! /bin/bash\n'
        script += 'set +e\n'
        script += self.indicateThisIsRTT()
        script += 'cd '+self.runPath+'\n'

        script += 'export SITEROOT=%s\n' % self.siteSpecifics.site()
        script += 'echo "Going to launch CMT lines"\n'
        for line in self.cmtLines: script += line+'\n'
        script += 'echo "Finished launching CMT lines"\n'
        script += 'JOBOPTSEARCHPATH=,%s:$JOBOPTSEARCHPATH\n' % self.runPath
        script += 'cd '+self.runPath+'\n'
        script += self.grabDBfile() + '\n'
        script += self.libLine+'\n'
        script += self.siteSpecifics.preAthena()+'\n'
        script += self.cmtShowUses() + '\n'
        script += self.toJobRunDir()
        if self.useSQLite: script += self.setupLocalSQLiteReplica()
        script += self.lsatlF('before')
        script += self.setupCASTOR()
        script += self.dumpEnvVars()
        script += self.athenaCommand+'\n'
        script += self.lsatlF('after')
        script += 'echo " ">JobCompleteMarker.txt\n'
           
        return script

    # ---------------------------------------------------------------------
    
    def lsatlF(self,when):
        fileName = join(self.runPath,'lsaltF.txt')+'\n'
        message = ''
        if when=='before':
            message = 'echo "Issuing command ls -altF on run directory, just BEFORE invoking athena..." > ' + fileName
        elif when=='after':
            message = '\n echo "Issuing command ls -altF on run directory, just AFTER invoking athena..." >> ' + fileName
            
        message = message + 'echo "------------------------------------------------" >> ' + fileName
        message = message + 'ls -altF >> ' + fileName
        return message

    # ---------------------------------------------------------------------
    
    def checkExtension(self, option):
        ext =option[-3:]
        if ext =='.py':
            pass
        else:
            option=option+'.py'
        return option

# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------        

class HLTScriptWriter(AthenaScriptWriter):
    def __init__(self, argsForScripts, logger):
        AthenaScriptWriter.__init__(self, argsForScripts, logger)
        self.runPath = argsForScripts.runPath
        self.package = argsForScripts.package
        
    def makeScript(self):
        """Default shell script for running a job."""
        
        script  = '#! /bin/bash\n'
        script += 'shopt -s expand_aliases\n'
        script += 'set +e\n'
        script += self.indicateThisIsRTT()
        script += 'cd '+self.runPath+'\n'

        script += 'export SITEROOT=%s\n' % self.siteSpecifics.site()
        for line in self.cmtLines: script += line+'\n'
        script += 'JOBOPTSEARCHPATH=,%s:$JOBOPTSEARCHPATH\n' % self.runPath
        script += 'cd '+self.runPath+'\n'
        script += self.grabDBfile() + '\n'
        script += self.libLine+'\n'
        script += self.siteSpecifics.preAthena()+'\n'
        script += self.cmtShowUses() + '\n'
        script += self.lsatlF('before')
        script += self.toJobRunDir()
        script += self.setupCASTOR()
        script += self.dumpEnvVars()
        script += self.athenaCommand+'\n'
        script += self.lsatlF('after')
        script += 'echo " ">JobCompleteMarker.txt\n'

        return script

# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------

class HLTTrigConfScriptWriter(AthenaScriptWriter):
    def __init__(self, argsForScripts, logger):
        AthenaScriptWriter.__init__(self, argsForScripts, logger)
    
    def makeScript(self):
        """
        Default shell script for running a job.
        """
        
        script = '#! /bin/bash\n'
        script += 'shopt -s expand_aliases\n'
        script += self.indicateThisIsRTT()
        script += 'set +e\n'
        script += 'cd '+self.runPath+'\n'

        script += 'export SITEROOT=%s\n' % self.siteSpecifics.site()
        for line in self.cmtLines: script += line+'\n'
        script += 'JOBOPTSEARCHPATH=,%s:$JOBOPTSEARCHPATH\n' % os.path.join(self.runPath, self.package)
        script += 'cd '+self.runPath+'\n'
        script += self.grabDBfile() + '\n'
        script += self.libLine+'\n'
        script += self.siteSpecifics.preAthena()+'\n'
        script += self.cmtShowUses() + '\n'
        script += self.lsatlF('before')
        script += self.toJobRunDir()
        # script += 'LD_LIBRARY_PATH=/afs/cern.ch/atlas/maxidisk/d90/13.1.0/InstallArea/i686-slc4-gcc34-dbg/lib:$LD_LIBRARY_PATH\n'
        # script += 'GAUDISVCROOT=/afs/cern.ch/atlas/maxidisk/d90/13.1.0/GaudiSvc\n'
        script += self.setupCASTOR()
        script += self.dumpEnvVars()
        script += self.athenaCommand+'\n'
        script += self.lsatlF('after')
        script += 'echo " ">JobCompleteMarker.txt\n'

        return script
    
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------

class JunkScriptWriter(AthenaScriptWriter):
    def __init__(self, argsForScripts, logger):
        AthenaScriptWriter.__init__(self, argsForScripts, logger)
        self.jobOptions = argsForScripts.pop('jobOptions', 'jobOptionsUnspecified')
        
    def makeScript(self):
        script = '#! /bin/bash\n'
        script += 'set +e\n'
        script += 'cd '+ self.runPath + '\n'
        script += 'echo AthenaCommon.Include.IncludeError ' + self.jobOptions + '\n'

        return script

# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------

class AthenaChildScriptWriter(AthenaScriptWriter):
    def __init__(self, argsForScripts, logger):
        AthenaScriptWriter.__init__(self, argsForScripts, logger)
        
    def scriptToSource(self):
        pass

    def makeScript(self):
        script = '#! /bin/bash\n'
        script += 'set +e\n'
        script += self.indicateThisIsRTT()
        script += 'cd '+self.runPath+'\n'
        script += 'export SITEROOT="CERN"\n' #should be CERN - used by CMT
        for line in self.cmtLines: script += line+'\n'
        script += 'JOBOPTSEARCHPATH=,%s:$JOBOPTSEARCHPATH\n' % self.runPath
        script += 'cd '+self.runPath+'\n'
        script += self.scriptToSource()
        script += self.libLine+'\n'
        script += self.siteSpecifics.preAthena()+'\n'
        script += self.cmtShowUses() + '\n'
        script += self.toJobRunDir()

        if self.useSQLite: script += self.setupLocalSQLiteReplica()
        script += self.lsatlF('before')
        script += self.setupCASTOR()
        script += self.dumpEnvVars()
        script += self.athenaCommand+'\n'
        script += self.lsatlF('after')
        script += 'echo " ">JobCompleteMarker.txt\n'
           
        return script

# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------

class RecExCommonScriptWriter(AthenaChildScriptWriter):
    """
    Script to run RecExCommon.
    This needs to perform various RecExCommon tasks.
    """
    def __init__(self, argsForScripts, logger):
        AthenaChildScriptWriter.__init__(self, argsForScripts, logger)

    def scriptToSource(self):
        return 'source RecExCommon_links.sh\n'
    
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------

class RecExCommissionScriptWriter(AthenaChildScriptWriter):
    """Script to run RecExCommission."""
    
    def __init__(self, argsForScripts, logger):
        AthenaChildScriptWriter.__init__(self, argsForScripts, logger)
        self.useSQLite = False

    def scriptToSource(self):
        return 'source RecExCommissionData_links.sh\n'

    # ---------------------------------------------------------------------

class CaloRecExAANTRealScriptWriter(AthenaChildScriptWriter):
    """
    Script to run RecExCommon.
    This needs to perform various RecExCommon tasks.
    """
    def __init__(self, argsForScripts, logger):
        AthenaChildScriptWriter.__init__(self, argsForScripts, logger)
        self.useSQLite = False
        
    def scriptToSource(self):
        return '\n' # i.e. none
    
    # ---------------------------------------------------------------------
    
class RecExCommissionSimScriptWriter(AthenaChildScriptWriter):
    """Script to run RecExCommissionSim"""
    
    def __init__(self, argsForScripts, logger):
        AthenaChildScriptWriter.__init__(self, argsForScripts, logger)
        self.useSQLite = False

    def scriptToSource(self):
        return 'source RecExCommission_links.sh\n'

    # ---------------------------------------------------------------------

class TileRecExScriptWriter(AthenaChildScriptWriter):
    """Script to run RecExCommission."""
    
    def __init__(self, argsForScripts, logger):
        AthenaChildScriptWriter.__init__(self, argsForScripts, logger)

    def scriptToSource(self):
        return 'source TileRecEx_links.sh\n'

    # ---------------------------------------------------------------------

class TileSimExScriptWriter(AthenaChildScriptWriter):
    """Script to run TileSimEx."""
    
    def __init__(self, argsForScripts, logger):
        AthenaChildScriptWriter.__init__(self, argsForScripts, logger)

    def scriptToSource(self):
        return 'source TileSimEx_links.sh\n'

# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------

class JiveXMLScriptWriter(RecExCommonScriptWriter):
    def __init__(self, argsForScripts, logger):
        RecExCommonScriptWriter.__init__(self, argsForScripts, logger)


# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------

class TrigReleaseIDScriptWriter(AthenaScriptWriter):
    def __init__(self, argsForScripts, logger):
        AthenaScriptWriter.__init__(self, argsForScripts, logger)

    def makeScript(self):
        script = '#! /bin/bash\n'
        script += 'set +e\n'
        script += 'cd '+self.runPath+'\n'
        script += self.indicateThisIsRTT()                 
        script += 'export SITEROOT="CERN"\n'
        for line in self.cmtLines: script += line+'\n'
        script += 'JOBOPTSEARCHPATH=,%s:$JOBOPTSEARCHPATH\n' % self.runPath
        script += 'cd '+self.runPath+'\n'
        script += self.grabDBfile() + '\n'
        script += 'source TriggerRelease_links.sh\n'
        script += self.libLine+'\n'
        script += self.siteSpecifics.preAthena()+'\n'
        script += self.cmtShowUses() + '\n'
        script += self.toJobRunDir()
        script += self.setupLocalSQLiteReplica()
        script += self.lsatlF('before')
        script += self.setupCASTOR()
        script += self.dumpEnvVars()
        script += self.athenaCommand+'\n'
        script += self.lsatlF('after')
        script += 'echo " ">JobCompleteMarker.txt\n'      
        
        return script

# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------

class TriggerReleaseScriptWriter(AthenaScriptWriter):
    def __init__(self, argsForScripts, logger):
        AthenaScriptWriter.__init__(self, argsForScripts, logger)

    def makeScript(self):
         
        script = '#! /bin/bash\n'
        script += 'set +e\n'
        script += 'export SITEROOT="CERN"\n'
        script += self.indicateThisIsRTT()                 
        script += 'cd '+self.runPath+'\n'
        for line in self.cmtLines: script += line+'\n'
        script += 'JOBOPTSEARCHPATH=,%s:$JOBOPTSEARCHPATH\n' % self.runPath
        script += 'cd '+self.runPath+'\n'
        script += self.grabDBfile() + '\n'
        #script += 'source TriggerRelease_links.sh\n'
        script += 'source RecExCommon_links.sh\n'
        script += self.libLine+'\n'
        script += self.siteSpecifics.preAthena()+'\n'
        script += self.cmtShowUses() + '\n'
        script += self.toJobRunDir()
        script += self.setupLocalSQLiteReplica()
        script += self.lsatlF('before')
        script += self.setupCASTOR()
        script += self.dumpEnvVars()
        script += self.athenaCommand+'\n'
        script += self.lsatlF('after')
        script += 'echo " ">JobCompleteMarker.txt\n'
        
        return script

# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------

class TrigReleaseCaloScriptWriter(TrigReleaseIDScriptWriter):
    def __init__(self, argsForScripts, logger):
        TrigReleaseIDScriptWriter.__init__(self, argsForScripts, logger)

# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------

class JobTransformScriptWriter(ScriptWriter):
    
    def __init__(self, argsForScripts, logger):
        "Provides a run script for Athena jobs"
        ScriptWriter.__init__(self, argsForScripts, logger)

        self.releasePath            = argsForScripts.releasePath
        self.packageInstalledCMTdir = argsForScripts.cmtDir
        self.mode                   = argsForScripts.mode
        
    def makeScript(self):
        """
        Shell script for running job trnsform python script.
        """
                
        script  = 'echo "----- JobTransform shell script starts  ---------"\n'
        script += '#! /bin/bash\n'
        script += 'set +e\n'
        script += self.indicateThisIsRTT()                 
        script += 'cd %s\n' % self.runPath
        for line in self.cmtLines: script += line+'\n'
        script += 'cd %s\n' % self.runPath
        script += self.setupCASTOR()
        script += self.dumpEnvVars()
        script += '%s\n' % self.jobTransformCmd
        script += 'echo "----- JobTransform shell script end  ---------"\n'
        
        return script

    
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------        

class PseudoScriptWriter(ScriptWriter):
    def __init__(self, argsForScripts, logger):
        ScriptWriter.__init__(self, argsForScripts, logger)
        
    def makeScript(self):

         script = '#! /bin/bash\n'
         script += 'set +e\n'
         script += self.indicateThisIsRTT()                 
         script += 'cd '
         script += self.runPath+'\n'
         for line in self.cmtLines: script += line+'\n'
         script += 'JOBOPTSEARCHPATH=,%s:$JOBOPTSEARCHPATH\n' % self.runPath
         script += 'cd '+self.runPath+'\n'
         script += self.libLine+'\n'
         script += self.dumpEnvVars() + '\n'        
         script += 'echo "one: 1  \n two:2 \n three:3 " > monitoringData.dat\n'
         script += 'echo "four: 4  \n five:5 \n six:6 " > someData.dat\n'
         script += 'sleep 60\n'
         script += 'echo " ">JobCompleteMarker.txt\n'

         return script
     
