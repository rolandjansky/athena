# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """FilePreStager for the castor tape system"""

__all__ = []

from FilePreStager import *
import os,re,time,commands
import rfio
from envutil import *

# mapping of Castor stager_qry output to FilePreStager stati.
_stager_qry_status_words = { 'STAGED'         : FilePreStager.STAGED ,
                             'CANBEMIGR'      : FilePreStager.STAGED ,
                             'STAGEIN'        : FilePreStager.STAGEIN ,
                             'INVALID_STATUS' : FilePreStager.INVALID ,
                             'not in stager'  : FilePreStager.ONTAPE }

_stager_qry_statusRE = '|'.join( _stager_qry_status_words.keys() )

_stager_qry_output_patterns = [ \
    r'^%%s\s+\S+\s+(?P<status>%s)$' % (_stager_qry_statusRE) ,
    r'^Error [0-9]+.*?\(File\s+%%s\s+(?P<status>%s)\s*\)$' % (_stager_qry_statusRE) ]

class CastorPreStager(FilePreStager):
    def __init__(self,name=None,filenamePattern=r'^/castor/|^rfio:/castor/|^castor:', maxWaitingTime=120*60, pollingInterval=5*60):
        FilePreStager.__init__(self,filenamePattern,maxWaitingTime, pollingInterval, name)

    def _writeOutFileList(self, listOfFiles, filename='castorFiles.list'):
        '''Write list of files for castor to operate on to a file - prevents problems with excessive command line length'''
        try:
            f = open(filename, 'w')
            for file in listOfFiles:
                print >>f, self.removePrefix(file)
            f.close()
        except IOError, (errno, errMsg):
            raise FilePreStageError('Got IOError writing out list of files to stage as %s: %s' % (filename, errMsg))
        return filename
        

    def getFilesStatus(self,listOfFiles):
        """Return a dictionary containing the status of each file in <listOfFiles>.
        The key is the filename and the value is the status."""
        if not listOfFiles: return {}
        if isinstance(listOfFiles, str):
            listOfFiles = [ listOfFiles ]
        fileList = self._writeOutFileList(listOfFiles)
        # compatibility with single filename
        # Write list of files to a file, so prevent 
        castor_cmd = 'stager_qry'
        if not find_executable(castor_cmd): 
            raise FilePreStageError( "%s not found in PATH" % (castor_cmd) )
        cmd = '%s -f %s' % (castor_cmd, fileList)
        # execute command
        self.printDebug(cmd)
        stat,output = commands.getstatusoutput( cmd )
        self.printVerbose(output)
        if stat:
            raise FilePreStageError( "Error executing %s (output=%s)" % (cmd,output) )
        # analyse output
        statusDict = {}
        for f in listOfFiles:
            for pat in _stager_qry_output_patterns:
                patF = pat % self.removePrefix(f)
                match = re.search( patF, output, re.MULTILINE )
                if match:
                    status = _stager_qry_status_words[match.group('status')]
                    break
            else:
                status = FilePreStager.UNKNOWN
            statusDict[f] = status

        return statusDict
        

    def preStageFiles(self,listOfFiles):
        if not listOfFiles: return
        if isinstance(listOfFiles, str):
            listOfFiles = [ listOfFiles ]
        # compatibility with single filename
        fileList = self._writeOutFileList(listOfFiles)
        castor_cmd = 'stager_get'
        if not find_executable(castor_cmd): 
            raise FilePreStageError( "%s not found in PATH" % (castor_cmd) )
        cmd = '%s -f %s' % (castor_cmd, fileList)
        # execute command
        self.printDebug(cmd)
        stat,output = commands.getstatusoutput( cmd )
        self.printVerbose(output)
        if stat:
            raise FilePreStageError( "Error executing %s (output=%s)" % (cmd,output) )

#
# setup for castor2 for atlas  using the env values for STAGE_HOST and STAGE_SVCCLASS
#
print "Setting up castor 2 for ATLAS ..."
castorEnv = {}
castorEnv['RFIO_USE_CASTOR_V2']='YES'
if os.getenv('STAGE_HOST')!= None:
    castorEnv['STAGE_HOST']=os.getenv('STAGE_HOST')
else:
    castorEnv['STAGE_HOST']='castoratlas'
if os.getenv('STAGE_SVCCLASS')!= None:
    castorEnv['STAGE_SVCCLASS']=os.getenv('STAGE_SVCCLASS')
else:
    castorEnv['STAGE_SVCCLASS']='default'
for n,v in castorEnv.items():
    print "%s=%s" % (n,v)

os.environ.update(castorEnv)

theCastorPreStager = CastorPreStager()
