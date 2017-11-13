#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
The SCT 24h Calibration Loop.
This transformation will run the SCT 24 hours calibration loop.
Mandatory values as to be given for the input ntuples 
as well as the output HitMaps and output stream file.
"""


import os,sys,time,shutil

from PyJobTransforms.trfLogger import *
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.trfSignal import setTrfSignalHandlers, resetTrfSignalHandlers

from IOVDbSvc.CondDB import conddb

from ROOT import TFile, TH1I

import PyJobTransforms.trfArgClasses as trfArgClasses
import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfValidation as trfValidation

from PyJobTransforms.trfReports import *

#temporal
from PyJobTransformsCore.trfutil import *


dsDict={'input': [] , 'output' : []}
RunNumber=-1
SvcClass=''
Stream=''
NumberOfEvents=0


def getDsFileName(file,input=False):
    """Returns dataset and logical file name for files of type ds##file or ds#dsfile."""
    global RunNumber
    global SvcClass
    global Stream
    global NumberOfEvents
    if isinstance(file,dict):
#        name=file['pfn']
        name=file['lfn']
        ds=file['dsn']
#        SvcClass=file['svcclass']
        SvcClass=file.get('svcclass','')
        if 'events' in file:
            NumberOfEvents+=file['events']
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

    if '/eos/atlas/' in name:
        name='root://eosatlas.cern.ch/'+name

    dsDict['input'].append({'file' : name, 'dataset' : ds})
    if RunNumber == -1:
        nb = ds.count('.')
        if nb >= 4:
            RunNumber=int(ds.split('.')[1])
            longStream=ds.split('.')[0]
            #Stream is either cos or xxxeV energy
            Stream=longStream.split('_')[1]

    return name

def checkFileList(filelist):
    """Converts list of files of type ds#filename into a list of filenames, meanwhile
    setting ds value. If check is true it also checks the existence of the files."""
    # First check if type is list
    
    if not isinstance(filelist,list):
        filelist=[filelist]

    for i,ifile in enumerate(filelist):
        # extract ds,runnumber and svcclass
        filename=getDsFileName(ifile)
        # pass file check if file is on castor
        if filename.find('/castor',0,8) != -1:
            pass
        elif not fileutil.exists(filename):
            found = fileutil.exists_suffix_number(filename + '.')
            if not found:

                errMsg = filename+' not found'
                raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_INPUT_FILE_VALIDATION_FAIL'), errMsg)

            if found != filename:

                filename = found
        # correct filename in list
        filelist[i]=filename
    return filelist

def updateLastRun(RunNumber):

    if os.path.exists('/afs/cern.ch/user/s/sctcalib/scratch0/lastRun'):
        f = open('/afs/cern.ch/user/s/sctcalib/scratch0/lastRun','w')
        f.write(str(RunNumber)+' ')
        f.close()
        

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():

    msg.info("This is %s" % sys.argv[0])
    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    
    addOutputArgs(trf.parser, trf._argdict)
    trf.parseCmdLineArgs(sys.argv[1:])

#    trf.checkNoisyStrips()

    trf.execute()
    trf.generateReport()
    
    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)

def getTransform():

    exeSet = set()
#    exeSet.add(SCTCalibExecutor('/afs/cern.ch/work/c/csander/sct/testarea/AtlasProduction-20.7.9.3//InnerDetector/InDetCalibAlgs/SCT_CalibAlgs/share/skeleton.sct_calib.py'))
    exeSet.add(SCTCalibExecutor('/afs/cern.ch/user/s/sctcalib/testarea/latest/InnerDetector/InDetCalibAlgs/SCT_CalibAlgs/share/skeleton.sct_calib.py'))

    trf = transform(executor=exeSet) 

    addAthenaArguments(trf.parser) 
    addSCTCalibArgs(trf.parser)
    
    return trf


def addSCTCalibArgs(parser):


    parser.defineArgGroup('Calibration', 'Specific options related to the calibration configuration')

    parser.add_argument('--input',
                        help = 'List of CSV input files',group='Calibration')

    parser.add_argument('--prefix', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help = 'Prefix for output files',group='Calibration')

    parser.add_argument('--part', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                        help = 'List of calibration algorithms to be run',group='Calibration')

    parser.add_argument('--SCTCalibConfig', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                        help = 'Config file for the SCT Calibration',group='Calibration')
    
    parser.add_argument('--doRunSelector', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help = 'Specifies if runSelector.py is executed',group='Calibration')
    parser.add_argument('--doRunInfo', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help = 'Specifies if runInfo.py is executed',group='Calibration')
    parser.add_argument('--splitNoisyStrip', type=trfArgClasses.argFactory(trfArgClasses.argInt,runarg=True),      
                        help = 'Split task or not',group='Calibration')


def addOutputArgs(parser,dict):
    
    if not 'part' in dict:
        checkPart = ['doNoisyStrip']
    else:
        checkPart = dict['part']._value
        
    if not 'splitNoisyStrip' in dict:
        checkSplit= 0
    else:
        checkSplit = dict['splitNoisyStrip']._value
        
    if not 'prefix' in dict:
        checkPrefix=''
    else:
        checkPrefix=dict['prefix']._value

    #     # # add outputfiles
    if checkPrefix != '':
        checkPrefix += '.'


    if not 'doHV' in checkPart and checkSplit != 1:
        parser.add_argument('--outputCOOL', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'COOL DB',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'mycool.db'],runarg=True))
    
    #NOISY STRIPS OUTPUT FILES
    if 'doNoisyStrip' in checkPart and checkSplit == 1:

        parser.add_argument('--outputHITMapFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True,io='output'),
                            help = 'HitMap output file',group='Calibration',default=trfArgClasses.argNTUPFile([checkPrefix+'SCTHitMaps.root'],runarg=True))
        parser.add_argument('--outputLBFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True,io='output'),
                            help = 'LB output file',group='Calibration',default=trfArgClasses.argNTUPFile([checkPrefix+'SCTLB.root'],runarg=True))
   
    if 'doNoisyStrip' in checkPart and checkSplit != 1:

        parser.add_argument('--outputHITMapFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True,io='output'),
                            help = 'HitMap output file',group='Calibration',default=trfArgClasses.argNTUPFile([checkPrefix+'SCTHitMaps.root'],runarg=True))
        parser.add_argument('--outputLBFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True,io='output'),
                            help = 'LB output file',group='Calibration',default=trfArgClasses.argNTUPFile([checkPrefix+'SCTLB.root'],runarg=True))

        parser.add_argument('--outputBSSummaryFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Bad Strips summary file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'BadStripsSummaryFile.xml'],runarg=True))
        parser.add_argument('--outputBSAllFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Bad Strips All file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'BadStripsAllFile.xml'],runarg=True))
        parser.add_argument('--outputBSNewFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Bad Strips New file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'BadStripsNewFile.xml'],runarg=True))

    if 'doHV' in checkPart:
        parser.add_argument('--outputBadModulesFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Bad Modules file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'BadModulesFile.xml'],runarg=True))

    #DEAD CHIP OUTPUT FILES
    if 'doDeadChip' in checkPart:
        parser.add_argument('--outputHITMapFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True,io='output'),
                            help = 'HitMap output file',group='Calibration',default=trfArgClasses.argNTUPFile([checkPrefix+'SCTHitMaps.root'],runarg=True))
        parser.add_argument('--outputDeadChipFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Dead Chip file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'DeadChipsFile.xml'],runarg=True))
        parser.add_argument('--outputDeadSummaryFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Dead Chip Summary file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'DeadSummaryFile.xml'],runarg=True))
        parser.add_argument('--outputBSErrorsFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True,io='output'),
                            help = 'BS Errors file',group='Calibration',default=trfArgClasses.argNTUPFile([checkPrefix+'SCTBSErrors.root'],runarg=True))
    #DEAD STRIP OUTPUT FILES
    if 'doDeadStrip' in checkPart:
        parser.add_argument('--outputHITMapFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True,io='output'),
                            help = 'HitMap output file',group='Calibration',default=trfArgClasses.argNTUPFile([checkPrefix+'SCTHitMaps.root'],runarg=True))
        parser.add_argument('--outputDeadStripFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Dead Strip file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'DeadStripsFile.xml'],runarg=True))
        parser.add_argument('--outputDeadSummaryFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Dead Strip Summary file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'DeadSummaryFile.xml'],runarg=True))
        parser.add_argument('--outputBSErrorsFile', type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True,io='output'),
                            help = 'BS Errors file',group='Calibration',default=trfArgClasses.argNTUPFile([checkPrefix+'SCTBSErrors.root'],runarg=True))
    #NOISE OCCUPANCY OUTPUT FILES
    if 'doNoiseOccupancy' in checkPart:
        parser.add_argument('--outputNOFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Noise Occupancy file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'NoiseOccupancyFile.xml'],runarg=True))
        parser.add_argument('--outputNOSummaryFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Noise Occupancy Summary file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'NoiseOccupancySummaryFile.xml'],runarg=True))
    #LORENTZ ANGLE OUTPUT FILES
    if 'doLorentzAngle' in checkPart:
        parser.add_argument('--outputLAFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Lorentz Angle file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'LorentzAngleFile.xml'],runarg=True))
        parser.add_argument('--outputLASummaryFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Lorentz Angle Summary file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'LorentzAngleSummaryFile.xml'],runarg=True))
    #RAW OCCUPANCY OUTPUT FILES
    if 'doRawOccupancy' in checkPart:
        parser.add_argument('--outputROSummaryFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Raw Occupancy Summary file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'RawOccupancySummaryFile.xml'],runarg=True))
    #EFFICIENCY OUTPUT FILES
    if 'doEfficiency' in checkPart:
        parser.add_argument('--outputEffModuleFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Efficiency file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'EfficiencyModuleSummary.xml'],runarg=True))
        parser.add_argument('--outputEffSummaryFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'Efficiency Summary file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'EfficiencySummaryFile.xml'],runarg=True))
    #BS ERRORS OUTPUT FILES
    if 'doBSErrorDB' in checkPart:
        parser.add_argument('--outputBSModuleFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'BS Errors Summary file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'BSErrorModuleSummary.xml'],runarg=True))
        parser.add_argument('--outputBSSummaryFile', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True,io='output'),
                            help = 'BS Errors Summary file',group='Calibration',default=trfArgClasses.argFile([checkPrefix+'BSErrorSummaryFile.xml'],runarg=True))





class SCTCalibExecutor( athenaExecutor ):
    def __init__(self, skeleton):
        athenaExecutor.__init__(self,
                                name = 'sctcalib',
                                skeletonFile='/afs/cern.ch/user/s/sctcalib/testarea/latest/InnerDetector/InDetCalibAlgs/SCT_CalibAlgs/share/skeleton.sct_calib.py')
#                                skeletonFile='/afs/cern.ch/work/c/csander/sct/testarea/AtlasProduction-20.7.9.3/InnerDetector/InDetCalibAlgs/SCT_CalibAlgs/share/skeleton.sct_calib.py')


    def preExecute(self, input=set(), output=set()):

        """ Execute runInfo, set environment and check inputtype"""
        # Execute runInfo.py
        runArgs=self.conf._argdict

        checkFileList(runArgs['input'])
        namelist=[]
        for i in range(0,len(dsDict['input'])):
            namelist.append(dsDict['input'][i]['file'])
            
        self.conf.addToArgdict('inputNames', trfArgClasses.argList(namelist))

        nName=namelist[0].count('/')
        fileName=namelist[0].split('/')[nName]
        projectName=str(fileName.split('.')[0])


        if not 'doRunInfo' in runArgs:
            self.conf.addToArgdict('doRunInfo', trfArgClasses.argBool(False))
        else:
            if runArgs['doRunInfo']._value:
                import SCT_CalibAlgs.runInfo as runInfo

                print "RunNumber for the runInfo = " + str(RunNumber) + " " + Stream
                runInfo.main(RunNumber, projectName)

        if not 'splitNoisyStrip' in runArgs:
            self.conf.addToArgdict('splitNoisyStrip', trfArgClasses.argInt(0))
        if not 'doRunSelector' in runArgs:
            self.conf.addToArgdict('doRunSelector', trfArgClasses.argBool(False))

            
            
        # Set STAGE_SVCCLASS
        if not SvcClass is '' and not SvcClass is None:
            os.environ['STAGE_SVCCLASS']=SvcClass

        # Check input type
        inputtype=dsDict['input'][0]['dataset'].split('.')[4]
        print "Input type = " + inputtype
        self.conf.addToArgdict('InputType', trfArgClasses.argString(inputtype))


        # check which parts to be run
        if not 'part' in runArgs:
            self.conf.addToArgdict('part', trfArgClasses.argString('doNoisyStrip'))

        part=runArgs['part']._value


        

        for ipart in part:
            if not ipart in ['doNoisyStrip','doNoiseOccupancy','doDeadChip','doDeadStrip','doHV','doBSErrorDB','doRawOccupancy','doEfficiency','doLorentzAngle','doNoisyLB']:
                self._errMsg = 'Argument part=%s does not match any of the possible candidates' % ipart
                raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_ARG_ERRO'), self._errMsg)


        # get prefix
        if not 'prefix' in runArgs:
            self.conf.addToArgdict('prefix', trfArgClasses.argString(''))
        

        prefix=runArgs['prefix']._value

            
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
            prefix+='.'+jobnb
            runArgs['prefix']._value = prefix


        # When ATLAS is NOT in standby the SCT is, the hitmap root files have 0 events,
        # even though the calibration_SCTNoise streams has 10k+ events.
        # If the noisy strips task is generated, the jobs will fail. A.N has implemented
        # a condition a t0 level so they won't be defined. However,
        # when runSelector uses AtlRunQuery to look for the runs that have 10k+ events
        # in the calibration_SCTNoise stream, those runs that failed or were skipped
        # will appear as waiting to be uploaded, making the rest keep on hold.

        # We include a protection against those cases: if the summed number of events
        # of hitmap files is <10k, we don't execute the noisy strips. Rather, we exit
        # with 'success' status, so the job won't fail at t0, and update the value
        # of the last run uploaded as if this run had been uploaded, to avoid the
        # next run being indefinitely on hold
        # print 'Number of events: ', NumberOfEvents
        if 'doNoisyStrip' in part and runArgs['splitNoisyStrip']._value==2 and NumberOfEvents<10000:
            self._isValidated = True
            self._trf._exitCode = 0
            self._trf._exitMsg = 'Noisy strips trying to read root files with 0 events. Gracefully exit and update lastRun counter to %s' %(RunNumber)

            updateLastRun(RunNumber)
            emptyDic = {}
            self._trf._dataDictionary = emptyDic

            resetTrfSignalHandlers()
            self._trf.generateReport(fast=True)
            sys.exit(0)

#                raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_SETUP_FAIL'), self._errMsg)


        if jobnb is not '':
            self.conf.addToArgdict('JobNumber', trfArgClasses.argString(jobnb))

        # get RunNumber from datasetName
        if not RunNumber == -1:
            self.conf.addToArgdict('RunNumber', trfArgClasses.argInt(RunNumber))
        if not Stream == '':
            self.conf.addToArgdict('Stream', trfArgClasses.argString(Stream))

        # Do other prerun actions
        super(SCTCalibExecutor, self).preExecute(input,output)
        




    def execute(self):

        runArgs=self.conf._argdict
        # Check the run for criteria in runSelector
        if runArgs['doRunSelector']._value:
            import SCT_CalibAlgs.runSelector as runSelector
            part=runArgs['part']._value
            if runArgs['splitNoisyStrip']._value == 1 :
              skipQueue = 1
            else:
              skipQueue = 0
            checkRun=runSelector.main(RunNumber,part,skipQueue,Stream)
            if not checkRun:

                print "Run %s didn't pass run selection criteria. It will not be processed and no output will be generated. Finish execution and exit gracefully" %(RunNumber)
                #No processing->no output
                #Need an empry dictionary so the job won't fail in t0
                #when trying to copy output files
                emptyDic = {}
                self._trf._dataDictionary = emptyDic

                    
                self._isValidated = True
                self._trf._exitMsg = 'Did not pass run selection criteria. Finish execution and exit gracefully.'
                self._trf._exitCode = 0
                resetTrfSignalHandlers()
                self._trf.generateReport(fast=True)
                sys.exit(0)


        rootHitmapFiles = []
        rootLbFiles = []
        for inputFileName in runArgs['input'] :
            if inputFileName.find("SCTHitMaps") != -1:
                rootHitmapFiles.append(inputFileName)
            if inputFileName.find("SCTLB") != -1:
                rootLbFiles.append(inputFileName)
           

        if runArgs['splitNoisyStrip']._value ==2 :
            if len(rootLbFiles) == len(rootHitmapFiles) and len(rootHitmapFiles) > 0 :

                fileutil.remove('SCTHitMaps.root')
                fileutil.remove('SCTLB.root')

                cmd = "cp -v $ROOTSYS/bin/hadd . \n"
                cmd += "hadd SCTHitMaps.root " 
                for inputFileName in rootHitmapFiles :
                    cmd += "%s " %(inputFileName)
                cmd += "\n"
    #            cmd += " >> /dev/null 2>&1 \n"
                cmd += "hadd SCTLB.root "
                for inputFileName in rootLbFiles :
                    cmd += "%s " %(inputFileName)
                cmd += "\n"
#            cmd += " >> /dev/null 2>&1 \n"
            
                print cmd
                self._echologger.info('Merging Hitmap and LB files!')
                retcode=1
                try:
                    retcode = os.system(cmd)
                except OSError, e:
                    retcode = 1
                if retcode == 0:
                    self._echologger.info('Root merge successful')
                else:
                    self._echologger.error("FAILED to merge root files")
        


        super(SCTCalibExecutor, self).execute()

        if self._rc is not 0:
            try:

                if 'less than the required minimum number of events' in open('log.sctcalib').read():
                    self._errMsg = 'Successful but warrants further investigation'
                    raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_UNKOWN'), self._errMsg)
            except:
                pass

        
    def postExecute(self):

        runArgs=self.conf._argdict
        prefix=runArgs['prefix']._value


        #After processing Hitmaps, change Metadata of SCTHitMaps and SCTLB files so
        #they contain the number of events. This value can be used when processing
        #noisy strips to avoid running over empty files
        if 'doNoisyStrip' in runArgs['part']._value and runArgs['splitNoisyStrip']._value == 1:
            outInstance0 = self.conf.dataDictionary[list(self._output)[0]]
            outTFile0 = TFile(outInstance0._value[0])
            print outTFile0.GetName()
            outNentries0 = int(outTFile0.Get('GENERAL/events').GetEntries())
            outInstance0._setMetadata(outInstance0._value,{'nentries': outNentries0})
            
            outInstance1 = self.conf.dataDictionary[list(self._output)[1]]
            outTFile1 = TFile(outInstance1._value[0])
            print outTFile1.GetName()
            outNentries1 = int(outTFile1.Get('GENERAL/events').GetEntries())
            outInstance1._setMetadata(outInstance1._value,{'nentries': outNentries1})


        if 'doDeadStrip' in runArgs['part']._value:
            pwd=os.getcwd()
            deadFile=pwd+'/'+prefix+'.DeadStripsFile.xml'
            deadSummary=pwd+'/'+prefix+'.DeadSummaryFile.xml'
            
            numLinesFile = 0
            numLinesSummary = 0
            if os.path.exists(deadFile):
                numLinesFile = sum(1 for line in open(deadFile))
            if os.path.exists(deadSummary):
                numLinesSummary = sum(1 for line in open(deadSummary))
                    

            #if the files exist, but there were no dead strips there won't be COOL file, making the job fail                         
             #remove the COOL file of the list of output files. Clunky, but temporal fix                                                   
                
            if ( numLinesFile == 2  and  numLinesSummary == 20 ):
                dataDic =  self._trf.dataDictionary
                listOfKeys = []

                for key in dataDic:
                    if key != 'COOL':
                        listOfKeys.append(key)

                redDict = {key:dataDic[key] for key in listOfKeys}
                self._trf._dataDictionary = redDict


        if 'doDeadChip' in runArgs['part']._value:
            pwd=os.getcwd()
            deadFile=pwd+'/'+prefix+'.DeadChipsFile.xml'
            deadSummary=pwd+'/'+prefix+'.DeadSummaryFile.xml'

            numLinesFile = 0
            numLinesSummary = 0
            if os.path.exists(deadFile):
                numLinesFile = sum(1 for line in open(deadFile))
            if os.path.exists(deadSummary):
                numLinesSummary = sum(1 for line in open(deadSummary))


            #if the files exist, but there were no dead strips there won't be COOL file, making the job fail              
           #remove the COOL file of the list of output files. Clunky, but temporal fix                                                      
            if ( numLinesFile == 2 and numLinesSummary == 20 ):
                dataDic =  self._trf.dataDictionary
                listOfKeys = []

                for key in dataDic:
                    if key != 'COOL':
                        listOfKeys.append(key)

                redDict = {key:dataDic[key] for key in listOfKeys}
                self._trf._dataDictionary = redDict



        if 'JobNumber' in runArgs and runArgs['splitNoisyStrip']._value == 1:
            jobnb=runArgs['JobNumber']._value
        else:
            jobnb=''
          


        if prefix is not '':
            try:
                if runArgs['splitNoisyStrip']._value !=1:
                    os.rename('mycool.db',prefix+'.mycool.db')                
                if jobnb is not '':
                    prefixTmp = prefix + "."+ jobnb
                else :
                    prefixTmp = prefix
                if runArgs['splitNoisyStrip']._value == 2:
                    os.rename('SCTHitMaps.root',prefix+'.SCTHitMaps.root')
                    os.rename('SCTLB.root',prefix+'.SCTLB.root')
            except:
                self._echologger.warning('failed to rename DB, ROOT or LOG file.' )
            

        super(SCTCalibExecutor, self).postExecute()

    
    def validate(self):
        self._hasValidated = True
        deferredException = None

       
        if 'ignorePatterns' in self.conf._argdict:
            igPat = self.conf.argdict['ignorePatterns'].value
        else:
            igPat = []
        if 'ignoreFiles' in self.conf._argdict:
            ignorePatterns = trfValidation.ignorePatterns(files = self.conf._argdict['ignoreFiles'].value, extraSearch=igPat)
        elif self._errorMaskFiles is not None:
            ignorePatterns = trfValidation.ignorePatterns(files = self._errorMaskFiles, extraSearch=igPat)
        else:
            ignorePatterns = trfValidation.ignorePatterns(files = athenaExecutor._defaultIgnorePatternFile, extraSearch=igPat)
       
        # Now actually scan my logfile
        msg.info('Scanning logfile {0} for errors'.format(self._logFileName))
        self._logScan = trfValidation.athenaLogFileReport(logfile = self._logFileName, ignoreList = ignorePatterns)
        worstError = self._logScan.worstError()

        

        # In general we add the error message to the exit message, but if it's too long then don't do
        # that and just say look in the jobReport
        if worstError['firstError']:
            if len(worstError['firstError']['message']) > athenaExecutor._exitMessageLimit:
                if 'CoreDumpSvc' in worstError['firstError']['message']:
                    exitErrorMessage = "Core dump at line {0} (see jobReport for further details)".format(worstError['firstError']['firstLine'])
                elif 'G4Exception' in worstError['firstError']['message']:
                    exitErrorMessage = "G4 exception at line {0} (see jobReport for further details)".format(worstError['firstError']['firstLine'])
                else:
                    exitErrorMessage = "Long {0} message at line {1} (see jobReport for further details)".format(worstError['level'], worstError['firstError']['firstLine'])
            else:
                exitErrorMessage = "Logfile error in {0}: \"{1}\"".format(self._logFileName, worstError['firstError']['message'])
        else:
            exitErrorMessage = "Error level {0} found (see athena logfile for details)".format(worstError['level'])

        # If we failed on the rc, then abort now
        if deferredException is not None:
            # Add any logfile information we have
            if worstError['nLevel'] >= stdLogLevels['ERROR']:
                deferredException.errMsg = deferredException.errMsg + "; {0}".format(exitErrorMessage)
            raise deferredException
       

        #ignore instances of "unknown offline id..."
        #less than ~10/event are admisible
        #if > 10/event, event is skipped in SCT_CalibEventInfo

        if worstError['firstError'] is not None:
            if 'ERROR Unknown offlineId for OnlineId' in worstError['firstError']['message']:
                worstError['nLevel'] = 30
                worstError['level'] = 'WARNING'
                

        #edit starting here
        # Very simple: if we get ERROR or worse, we're dead, except if ignoreErrors=True
        if worstError['nLevel'] == stdLogLevels['ERROR'] and ('ignoreErrors' in self.conf._argdict and self.conf._argdict['ignoreErrors'].value is True):
            msg.warning('Found ERRORs in the logfile, but ignoring this as ignoreErrors=True (see jobReport for details)')
        elif worstError['nLevel'] >= stdLogLevels['ERROR']:
            self._isValidated = False
            msg.error('Fatal error in athena logfile (level {0})'.format(worstError['level']))
            raise trfExceptions.TransformLogfileErrorException(trfExit.nameToCode('TRF_EXEC_LOGERROR'),
                                                               ' Fatal error in athena logfile: "{0}"'.format(exitErrorMessage))

        # Must be ok if we got here!
        msg.info('Executor {0} has validated successfully'.format(self.name))
        self._isValidated = True




        


if __name__ == "__main__":
    main()

        

