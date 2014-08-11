#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__doc__ = """The SCT 24h Calibration Loop.
This transformation will run the SCT 24 hours calibration loop.
Mandatory values as to be given for the input ntuples 
as well as the output HitMaps and output stream file."""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *

dsDict={'input': [] , 'output' : []}
RunNumber=-1
SvcClass=''

def getDsFileName(self,file,input=False):
    """Returns dataset and logical file name for files of type ds##file or ds#dsfile."""
    global RunNumber
    global SvcClass

    if isinstance(file,dict):
        name=file['pfn']
        ds=file['dsn']
        SvcClass=file['svcclass']
    else:
        if file.find('##')!=-1:
            ds=file.split('##')[0]
            name=file.replace('##','')
        elif file.find('#')!=-1:
            ds=file.split('#')[0]
            name=file.split('#')[1]
        else:
            if file.find('/') is not -1:
                fileparts=file.split('/')
                ds=fileparts[len(fileparts)-1]
            else:
                ds=file
            name=file

    dsDict['input'].append({'file' : name, 'dataset' : ds})
    if RunNumber == -1:
        nb = ds.count('.')
        if nb >= 5:
            RunNumber=int(ds.split('.')[1])
  
    return name

def checkFileList(self,filelist):
    """Converts list of files of type ds#filename into a list of filenames, meanwhile
    setting ds value. If check is true it also checks the existence of the files."""
    # First check if type is list
    if not isinstance(filelist,list):
        filelist=[filelist]

    for i,ifile in enumerate(filelist):
        # extract ds,runnumber and svcclass
        filename=getDsFileName(self,ifile)
        # pass file check if file is on castor
        if filename.find('/castor',0,8) != -1:
            pass
        elif not fileutil.exists(filename):
            found = fileutil.exists_suffix_number(filename + '.')
            if not found:
                code = AtlasErrorCodes.getCode('TRF_INFILE_NOTFOUND')
                raise InputFileError(filename,'not found', code)
            if found != filename:
                self.logger().warning('replacing %s with %s' % (filename,found) )
                filename = found
        # correct filename in list
        filelist[i]=filename
    return filelist

class ListOfNtupleFilesArg(InputDataFileArg):
    """Input ntuple file"""
    def __init__(self,help='default',name='default'):
        InputDataFileArg.__init__(self,help,NtupleFile(),name)

    def isFullArgument(self):
        return True

    def toPython(self,valIn):
        if valIn is None: return []
        valList=InputDataFileArg.toPython(self,valIn)
        fileList=checkFileList(self,valList)
        return fileList

    def checkFile(self):
        castorFiles=False
        for val in self.value():
            if val.find('/castor',0,8) is not -1:
                castorFiles = True
        if not castorFiles:
            InputDataFileArg.checkFile(self)
            

class OutputFile:
    """ Dummy outputfile"""
    def __init__(self):
        self._value = None
        self._fileInfo = None
        self._type = 'any'
    def setValue(self,val):
        self._value=val
        
    def value(self):
        return self._value
    
    def setType(self,type):
        self._type=type
        
    def fileInfo(self):
        return  FileInfo( self._value , FileType(self._type).getGUID(self._value))

class RunSelectorArg(BoolArg):
    """Execute runSelector.py in transformation"""
    def __init__(self,help='default',name='default', **kwargs ):
        BoolArg.__init__(self,help,name, **kwargs )
        
    def isFullArgument(self):
        return True

class RunInfoArg(BoolArg):
    """Execute runInfo.py in transformation"""
    def __init__(self,help='default',name='default', **kwargs ):
        BoolArg.__init__(self,help,name, **kwargs )
        
    def isFullArgument(self):
        return True

class SplitNoisyStripArg(IntegerArg):
    """Split task or not"""
    def __init__(self,help='default',name='default', **kwargs ):
        IntegerArg.__init__(self,help,name, **kwargs )
        
    def isFullArgument(self):
        return True


class SCTCalibTrf( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Peter L Rosendahl', 'Peter.Lundgaard.Rosendahl@cern.ch'), Author('Misha Lisovyi', 'Mykhailo.Lisovyi@cern.ch')] ,
                              name = 'sct_calib',
                              skeleton='SCT_CalibAlgs/skeleton.sct_calib.py' ,
                              help = __doc__ )

        #add arguments
        self.add( ListOfNtupleFilesArg(name='input', help='List of input files.'))
        self.add( BasicStringArg(name='prefix', help='Prefix for output files'), default='' )
        self.add( MaxEventsArg(), default='-1')
        self.add( ListOfStringsArg(name='part',help='List of calibration algorithms to be run.'),default=['doNoisyStrip'])
        self.add( JobOptionsArg(name='SCTCalibConfig',package='SCT_CalibAlgs',help='Config file for the SCT Calibration.'),default='SCTCalibConfig.py')
        self.add( RunSelectorArg(name='doRunSelector',help='Specifies if runSelector.py is executed'), default='False')
        self.add( RunInfoArg(name='doRunInfo',help='Specifies if runInfo.py is executed'), default='False')
        self.add( SplitNoisyStripArg(name='splitNoisyStrip',help='Split task or not.'), default='0')

    def addOutFile(self,name='',type='any'):
        if name is '': return
        file = OutputFile()
        file.setValue(name)
        file.setType(type)
        self._addOutputFile(file)
    
    def readArgsFromPickle(self,filename):
        """Read transform arguments from pickle file. It will only use the arguments
        that is supports, and ignore any others in the file"""
        import cPickle as pickle
        argFile = open(filename)
        argDict = pickle.load(argFile)
        argFile.close()
        for n,v in argDict.items():
            # Make exception for input files
            if n == "input":
                # Extract DS and RunNumber
                v=checkFileList(self,v)
            self.setArgument(n,v)

    def doPreRunActions(self):
        """ Execute runInfo, set environment and check inputtype"""
        # Execute runInfo.py
        if self._runArgs.doRunInfo:
            import SCT_CalibAlgs.runInfo as runInfo
            print "RunNumber for the runInfo = " + str(RunNumber)
            runInfo.main(RunNumber)
            
        # Set STAGE_SVCCLASS
        #os.environ['STAGE_SVCCLASS']='atlcal'
        if not SvcClass is '':
            os.environ['STAGE_SVCCLASS']=SvcClass

        # Check input type
        inputtype=dsDict['input'][0]['dataset'].split('.')[4]
        print "Input type = " + inputtype
        self.add( BasicStringArg(name='InputType', help='Type of input in case of noisystrips, e.g. RAW or NTUP.'),default=inputtype)

        # check which parts to be run
        part=self._runArgs.part
        for ipart in part:
            if not ipart in ['doNoisyStrip','doNoiseOccupancy','doDeadChip','doDeadStrip','doHV','doBSErrorDB','doRawOccupancy','doEfficiency']:
                self.logger().fatal('Argument part=%s does not match any of the possible candidates' % ipart)
                raise

        # get prefix
        prefix=self._runArgs.prefix

        # set job number
        jobnb=''

        # find seperator for jobnumber
        if prefix is not '' : 
            sep=prefix.find('._')
            if ( sep != -1 ) :
                jobnb=prefix[sep+1:]
            elif ( prefix.rfind('#_') != -1 ):
                sep=prefix.rfind('#_')
                jobnb=prefix[sep+1:]
            
            # find seperator for prefix
            sep=prefix.find('#')
            if (sep != -1) :
                prefix=prefix[:sep]
            elif (prefix.find('._') != -1):
                sep=prefix.rfind('._')
                prefix=prefix[:sep]

            # set prefix and jobnumber
            self.setArgument("prefix",prefix)
            self.logger().info('Setting prefix to %s' % prefix)

        if jobnb is not '':
            self.add(BasicStringArg(name='JobNumber',help=''),default=jobnb)
            self.logger().warning('Setting JobNumber to %s' % jobnb)


        # add outputfiles
        if prefix != '':
            prefix += '.'
            
        if 'doNoisyStrip' not in part: 
            self.addOutFile(prefix+'log')
        
        splitNoisyStrip=self._runArgs.splitNoisyStrip
        
        if not 'doHV' in part and splitNoisyStrip != 1:
            self.addOutFile(prefix+'mycool.db')
        if 'doNoisyStrip' in part and splitNoisyStrip == 1:
            #self.addOutFile(prefix+'SCTHitMaps.root','root')
            #self.addOutFile(prefix+'SCTLB.root')
            if jobnb is not '':
              prefixTmp = prefix + jobnb + "."
            else :
              prefixTmp = prefix
            self.addOutFile(prefixTmp+'SCTHitMaps.root','root')
            self.addOutFile(prefixTmp+'SCTLB.root')
            self.addOutFile(prefixTmp+'log')
        if 'doNoisyStrip' in part and splitNoisyStrip != 1:
            self.addOutFile(prefix+'SCTHitMaps.root','root')
            self.addOutFile(prefix+'SCTLB.root')
            self.addOutFile(prefix+'BadStripsAllFile.xml')
            self.addOutFile(prefix+'BadStripsNewFile.xml')
            self.addOutFile(prefix+'BadStripsSummaryFile.xml')
            self.addOutFile(prefix+'log')
        if 'doHV' in part:
            self.addOutFile(prefix+'BadModulesFile.xml')
        if 'doDeadChip' in part:
            self.addOutFile(prefix+'SCTHitMaps.root','root')
            self.addOutFile(prefix+'DeadChipsFile.xml')
            self.addOutFile(prefix+'DeadSummaryFile.xml')
            self.addOutFile(prefix+'SCTBSErrors.root','root')
        if 'doDeadStrip' in part:
            self.addOutFile(prefix+'SCTHitMaps.root','root')
            self.addOutFile(prefix+'DeadStripsFile.xml')
            self.addOutFile(prefix+'DeadSummaryFile.xml')
            self.addOutFile(prefix+'SCTBSErrors.root','root')
        if 'doNoiseOccupancy' in part:
            self.addOutFile(prefix+'NoiseOccupancyFile.xml')
            self.addOutFile(prefix+'NoiseOccupancySummaryFile.xml')
        if 'doRawOccupancy' in part:
            self.addOutFile(prefix+'RawOccupancySummaryFile.xml')
        if 'doEfficiency' in part:
            self.addOutFile(prefix+'EfficiencySummaryFile.xml')
        if 'doBSErrorDB' in part:
            self.addOutFile(prefix+'BSErrorSummaryFile.xml')

        # Do other prerun actions
        JobTransform.doPreRunActions(self)

        # get RunNumber from datasetName
        if not RunNumber == -1:
            self.add(RunNumberArg(name='RunNumber',help='RunNumber read from dataset name'),default=RunNumber)

    def runJob(self):
        # Check the run for criteria in runSelector
        if self._runArgs.doRunSelector:
            import SCT_CalibAlgs.runSelector as runSelector
            part=self._runArgs.part
            if self._runArgs.splitNoisyStrip == 1 :
              skipQueue = 1
            else:
              skipQueue = 0
            checkRun=runSelector.main(RunNumber,part,skipQueue)
            if not checkRun:
                AtlasErrorCodes.addCategory( 1, 'NEEDCHECK', 'Did not pass run selection criteria' )
                self.addError( acronym = 'NEEDCHECK', severity = AtlasErrorCodes.FATAL, message = 'Did not pass run selection criteria' )
                report=self._jobReport
                return report

        rootHitmapFiles = []
        rootLbFiles = []
        for inputFileName in self._runArgs.input :
          if inputFileName.find("SCTHitMaps") != -1:
            rootHitmapFiles.append(inputFileName)
          if inputFileName.find("SCTLB") != -1:
            rootLbFiles.append(inputFileName)
           

        if self._runArgs.splitNoisyStrip ==2 :
          #self.logger().info("Should try merging : N(LB) = " + str(len(rootLbFiles)) + ", N(Hitmaps) = " + str(len(rootHitmapFiles)))
          #print rootLbFiles
          #print rootHitmapFiles
          if len(rootLbFiles) == len(rootHitmapFiles) and len(rootHitmapFiles) > 0 :
            #self.logger().info("Start merging")
            #if fileutil.exists('SCTHitMaps.root'):
            fileutil.remove('SCTHitMaps.root')
            #if fileutil.exists('SCTLB.root'):
            fileutil.remove('SCTLB.root')
            cmd = "cp -v $ROOTSYS/bin/hadd . \n"
            cmd += "hadd SCTHitMaps.root "
            for inputFileName in rootHitmapFiles :
              cmd += "%s " %(inputFileName)
            cmd += " >> /dev/null 2>&1 \n"
            cmd += "hadd SCTLB.root "
            for inputFileName in rootLbFiles :
              cmd += "%s " %(inputFileName)
            cmd += " >> /dev/null 2>&1 \n"
            
            #print cmd + "\n"
            self.logger().info('Merging Hitmap and LB files!')
            retcode=1
            try:
              retcode = os.system(cmd)
              #retcode = call(cmd, shell=True)
            except OSError, e:
              retcode = 1
            if retcode == 0:
              self.logger().info('Root merge successful')
            else:
              self.logger().error("FAILED to merge root files")
        
        report=JobTransform.runJob(self)
        # check for minimum events
        if report.exitCode() is not 0:
            try:
                if report.getExitError().message.find('less than the required minimum number of events') is not -1:
                    AtlasErrorCodes.addCategory( 1, 'NEEDCHECK', 'Successful but warrants further investigation' )
                    self.addError( acronym = 'NEEDCHECK', severity = AtlasErrorCodes.FATAL, message = report.getExitError().message )
                    report=self._jobReport
            except:
                pass

        # change name of log file
        prefix=self._runArgs.prefix
        self.logger().debug('------------------------')


        self.logger().debug('------------------------')
        return report

        
    def doPostRunActions(self):
        # get prefix and jobnb
        self.logger().debug('+++++++++++++++++++++++++')
        prefix=self._runArgs.prefix
        if 'JobNumber' in self._runArgs.__dict__ and self._runArgs.splitNoisyStrip == 1:
            jobnb=self._runArgs.JobNumber
            try:
                #self.logger().info(prefix + '.SCTHitMaps.root ' + prefix + "." + jobnb + "." + 'SCTHitMaps.root')
                os.rename(prefix + '.SCTHitMaps.root',prefix + "." + jobnb + "." + 'SCTHitMaps.root')
                os.rename(prefix + '.SCTLB.root'     ,prefix + "." + jobnb + "." + 'SCTLB.root')
            except:
                self.logger().warning('failed to rename ROOT files.' )
        else:
            jobnb=''
        if prefix is not '':
            try:
                if self._runArgs.splitNoisyStrip !=1 :
                  os.rename('mycool.db',prefix+'.mycool.db')                
                #print self._runArgs.__dict__
                if jobnb is not '':
                    prefixTmp = prefix + "."+ jobnb
                else :
                    prefixTmp = prefix
                #self.logger().info(prefixTmp)
                os.rename(self._logFilename,prefixTmp+'.log')
                self._logFilename=prefixTmp+'.log'
                if self._runArgs.splitNoisyStrip == 2:
                  os.rename('SCTHitMaps.root',prefix+'.SCTHitMaps.root')
                  os.rename('SCTLB.root',prefix+'.SCTLB.root')
            except:
                self.logger().warning('failed to rename DB, ROOT or LOG file.' )
            
        self.logger().debug('+++++++++++++++++++++++++')
        JobTransform.doPostRunActions(self)
        
    def writeReports(self):
        import cPickle as pickle
        JobTransform.writeReports(self)
        # read in jobreport file
        file=open('jobReport.gpickle')
        dict=pickle.load(file)
        file.close()
        removelist=[]
        for f in dict['prodsys']['jobOutputs']:
            prefix=self._runArgs.prefix
            if prefix is not '':
                f['dataset']=prefix
            if self._runArgs.maxEvents > 0:
                f['events']=self._runArgs.maxEvents
            try:
                os.stat(f['lfn'])
            except:
                removelist.append(f)
                
        for i in removelist:
            dict['prodsys']['jobOutputs'].remove(i)

        # create new jobreport
        file=open('jobReport.gpickle','w')
        pickle.dump(dict,file,0)
        file.close()
        file2=open('jobReport_sct_calib.gpickle','w')
        pickle.dump(dict,file2,0)
        file2.close()
        
# execute it if not imported
if __name__ == '__main__':
    trf = SCTCalibTrf()
    sys.exit(trf.exeSysArgs().exitCode())
