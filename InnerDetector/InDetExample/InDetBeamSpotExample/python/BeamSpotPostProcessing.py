#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Beam spot postprocessing library.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id $'


import os

from InDetBeamSpotExample.PostProcessing import PostProcessingStep, PostProcessingError, PostponeProcessing, runPostProcStep
from InDetBeamSpotExample.TaskManager import TaskManager
from InDetBeamSpotExample import COOLUtils

from future import standard_library
standard_library.install_aliases()
import subprocess

linkTemplates = {
    'PlotBeamSpot.gif': '<a href="../files?u=%s/%s/%s">Summary</a>',
    'PlotBeamSpot.pdf': '(<a href="/jobfiles/%s/%s/%s">pdf</a>)',
    'PlotBeamSpotMon.gif': '<a href="../files?u=%s/%s/%s">Monitoring</a>',
    'PlotBeamSpotMon.pdf': '(<a href="/jobfiles/%s/%s/%s">pdf</a>)',
    'PlotGlobalMon.gif': '<a href="../files?u=%s/%s/%s">Globalmon</a>',
    'PlotGlobalMon.pdf': '(<a href="/jobfiles/%s/%s/%s">pdf</a>)',
    'PlotBeamSpotCompareReproc.gif': '<a href="../files?u=%s/%s/%s">Compare</a>',
    'PlotBeamSpotCompareReproc.pdf': '(<a href="/jobfiles/%s/%s/%s">pdf</a>)',
    'PlotBeamSpotCompareReproc.html': '(<a href="/jobfiles/%s/%s/%s">html</a>)',
    'PlotBCID-data.txt': '<a href="../files?u=%s/%s/%s">BCID data</a>',
    'PlotBCID-vsLb.gif': '<a href="../files?u=%s/%s/%s">vsLb</a>',
    'PlotBCID-vsLb.pdf': None,   # suppress linking until we get better quality PDF
    #'PlotBCID-vsLb.pdf': '(<a href="/jobfiles/%s/%s/%s">pdf</a>)',
    'PlotBCID-vsTime.gif': '<a href="../files?u=%s/%s/%s">vsTime</a>',
    'PlotBCID-vsTime.pdf': None,   # suppress linking until we get better quality PDF
    #'PlotBCID-vsTime.pdf': '(<a href="/jobfiles/%s/%s/%s">pdf</a>)',
    'PlotBCID-vsBCID.gif': '<a href="../files?u=%s/%s/%s">vsBCID</a>',
    'PlotBCID-vsBCID.pdf': None,   # suppress linking until we get better quality PDF
    'PlotBCID-vsBunchPos.gif': '<a href="../files?u=%s/%s/%s">vsBunchPos</a>',
    'PlotBCID-vsBunchPos.pdf': None,
    #'PlotBCID-vsBCID.pdf': '(<a href="/jobfiles/%s/%s/%s">pdf</a>)',
    'MergeNt-nt.root': None,            # suppress linking
    'BeamSpotNt-nt.root': None,         # suppress linking
    'AveBeamSpot-beamspot.db': None,    # suppress linking
    #'AveBeamSpot-dqflags.db' : None,   # suppress linking
    #'AveBeamSpot-dqflags.txt' : None,  # suppress linking     
    'DQBeamSpot-dqflags.db' : None,     # suppress linking
    'DQBeamSpot-dqflags.txt' : None,    # suppress linking
    'DQBeamSpotReproc-dqflags.db' : None, # suppress linking
    'PlotOnlineOfflineCompare.gif': None,
    'PlotOnlineOfflineCompare.pdf': None
}

# Beamspot tag is now taken from "current" BLK tag alias
#beamspottag = 'IndetBeampos-ES1-UPD2'
dqtag       = 'HEAD'
beamspottagonline = 'IndetBeamposOnl-LiveMon-001-00'

class TaskPostProcessing(PostProcessingStep):
    def run(self):
        postProcSteps = self.taskDict['TASKPOSTPROCSTEPS'].split()
        for step in postProcSteps:
            self.log('Running postprocessing step:  %s' % step)
            self.executedSteps = runPostProcStep(self.taskman,self.taskDict,self.oldStatus,self.executedSteps,step,self.postprocLib,self.jobName)


class JobPostProcessing(PostProcessingStep):
    def run(self):
        ## JobPostProcessing runs only on completed tasks in status POSTPROCESSING
        #if self.oldStatus==TaskManager.StatusCodes['POSTPROCESSING']:
        try:
            postProcSteps = self.getJobConfig(self.jobName)['jobpostprocsteps'].split()
        except Exception:
            raise PostProcessingError('ERROR: No config file or jobpostprocsteps parameter found for %s/%s' % (self.dsName,self.taskName),self.executedSteps)
        for step in postProcSteps:
            self.log('Running postprocessing step:  %s' % step)
            self.executedSteps  = runPostProcStep(self.taskman,self.taskDict,self.oldStatus,self.executedSteps,step,self.postprocLib,self.jobName)


class T0VertexDefaultProcessing(PostProcessingStep):
    def run(self):
        postProcSteps = 'PlotBeamSpotMon MergeNt BeamSpotNt PlotBeamSpot LinkResults PlotOnlineOfflineCompare AveBeamSpot CheckT0Status UploadBeamSpot DQBeamSpot UploadDataQuality BeamSpotGlobalNt '
        for step in postProcSteps.split():
            self.log('Running postprocessing step:  %s' % step)
            self.executedSteps  = runPostProcStep(self.taskman,self.taskDict,self.oldStatus,self.executedSteps,step,self.postprocLib,self.jobName)

class T0VertexTestProcessing(PostProcessingStep):
    def run(self):
        postProcSteps = 'PlotBeamSpotMon MergeNt BeamSpotNt PlotBeamSpot LinkResults PlotOnlineOfflineCompare AveBeamSpot CheckT0Status DQBeamSpot BeamSpotGlobalNt '
        for step in postProcSteps.split():
            self.log('Running postprocessing step:  %s' % step)
            self.executedSteps  = runPostProcStep(self.taskman,self.taskDict,self.oldStatus,self.executedSteps,step,self.postprocLib,self.jobName)

class ReprocVertexDefaultProcessing(PostProcessingStep):
    def run(self):
        postProcSteps = ' MergeNt BeamSpotNt PlotBeamSpot PlotBeamSpotCompareReproc LinkResults AveBeamSpot DQBeamSpotReproc'
        for step in postProcSteps.split():
            self.log('Running postprocessing step:  %s' % step)
            self.executedSteps  = runPostProcStep(self.taskman,self.taskDict,self.oldStatus,self.executedSteps,step,self.postprocLib,self.jobName)


class BCIDDefaultProcessing(PostProcessingStep):
    def run(self):
        postProcSteps = 'MergeNt BeamSpotNtNoAve PlotBCID LinkResults'
        for step in postProcSteps.split():
            self.log('Running postprocessing step:  %s' % step)
            self.executedSteps  = runPostProcStep(self.taskman,self.taskDict,self.oldStatus,self.executedSteps,step,self.postprocLib,self.jobName)


class PlotBeamSpotMon(PostProcessingStep):
    def run(self):
        outFileNameGIF = self.getFileName('.gif')
        outFileNamePDF = self.getFileName('.pdf')
        histFileName = self.getFileName('-beamspotmonitoring.root')
        if os.path.exists('/'.join([self.taskDir,histFileName])):
            self.logExec('cd %s; rm -f %s %s %s' % (self.taskDir,histFileName,outFileNameGIF,outFileNamePDF))
        postProcFiles = self.taskFileList('*-beamspotmonitoring.root','POSTPROCESSING',self.jobName)
        self.log(text='Found %i monitoring files from POSTPROCESSING jobs ...' % len(postProcFiles))
        completedFiles = self.taskFileList('*-beamspotmonitoring.root','COMPLETED',self.jobName)
        self.log(text='Found %i monitoring files from COMPLETED jobs ...' % len(completedFiles))
        histFiles = sorted(postProcFiles+completedFiles)
        if len(histFiles)>0:
            #self.logExec('cd %s; hadd -f %s %s' % (self.taskDir,histFileName,' '.join(histFiles)))
            self.hadd(self.taskDir,histFileName,histFiles)
            self.logExec('cd %s; plotBeamSpotMon.py -b -o %s,%s %s' % (self.taskDir,outFileNameGIF,outFileNamePDF,histFileName))
            self.addResult(outFileNameGIF)
            self.addResult(outFileNamePDF)
        else:
            self.log(text='No monitoring files yet ...')


class PlotBeamSpotCompareReproc(PostProcessingStep):
    def run(self):
        outFileNameGIF = self.getFileName('.gif')
        outFileNamePDF = self.getFileName('.pdf')
        ntFileName = self.getFileName('-nt.root','BeamSpotNt')
        
        # Resolve beamspot tag.  Since reprocessing results will be uploaded to the "next" tag, the "current"
        # tag will be the one containing the best results to date and hence we compare to that
        try: 
            beamspottag = COOLUtils.resolveCurrentBeamSpotFolder()
        except Exception:
            raise PostProcessingError('ERROR: Unable to resolve beamspot folder tag',self.executedSteps,TaskManager.StatusCodes['POSTPROCFAILED'])

        if os.path.exists('/'.join([self.taskDir,ntFileName])):
            self.logExec('cd %s; plotBeamSpotCompare.py -b -o %s,%s --config="Reproc" %s %s' % (self.taskDir, outFileNameGIF,outFileNamePDF,beamspottag,ntFileName))
            self.addResult(outFileNameGIF)
            self.addResult(outFileNamePDF)
            self.logExec('cd %s; mkdir -p %s' % (self.taskDir, self.getFileName('-compplots')))
            self.logExec('cd %s; plotBeamSpotCompare.py -b --outtag="%s/repro_" --output=".pdf,.png" --config="Reproc" --multicanv --html %s %s' % (self.taskDir, self.getFileName('-compplots'), beamspottag,ntFileName))
            self.addResult(self.getFileName('.html'))
            
class PlotOnlineOfflineCompare(PostProcessingStep):
    def run(self):
        outFileNameGIF = self.getFileName('.gif')
        outFileNamePDF = self.getFileName('.pdf')
        ntFileName = self.getFileName('-nt.root','BeamSpotNt')
        if os.path.exists('/'.join([self.taskDir,ntFileName])):
            cmd = 'cd %s; plotBeamSpotCompare.py -b -o %s,%s --online2 --config="OnlineOffline" %s %s' % (self.taskDir, outFileNameGIF, outFileNamePDF, ntFileName, beamspottagonline)
            status=self.logExec(cmd,doPrint=True,abortOnError=False)
            resultLinks = self.taskDict['RESULTLINKS']
            if status==0: 
                self.addResult(outFileNameGIF)
                self.addResult(outFileNamePDF)
                link = '<a class="ok" href="../files/?u=%s/%s/%s.gif">vsOnline</a>' % (self.dsName,self.taskName,self.baseNameStep)
                if resultLinks.find(link)==-1:
                    resultLinks = ' '.join([resultLinks,link])
                link = '(<a class="ok" href="/jobfiles/%s/%s/%s.pdf">pdf</a>)' % (self.dsName,self.taskName,self.baseNameStep)
                if resultLinks.find(link)==-1:
                    resultLinks = ' '.join([resultLinks,link])
            else:
                link = '<a class="warn" href="../files/?u=%s/%s/%s.log">No vsOnline</a>' % (self.dsName,self.taskName,self.baseNameStep)
                if resultLinks.find(link)==-1:
                    resultLinks = ' '.join([resultLinks,link])
            self.taskman.setValue(self.dsName,self.taskName,'RESULTLINKS',resultLinks)

# Still to be tested
class PlotGlobalMon(PostProcessingStep):
    def run(self):
        outFileNameGIF = self.getFileName('.gif')
        outFileNamePDF = self.getFileName('.pdf')
        # Get monitoring file name from jobConfig?
        self.logExec('cd %s; hadd -f %s-globalmonitoring.root */*-globalmonitoring.root' % (self.taskDir,self.baseName))
        self.logExec('cd %s; plotBeamSpotMon.py -b -o %s,%s %s-globalmonitoring.root' % (self.taskDir,outFileNameGIF,outFileNamePDF,self.baseName))
        self.addResult(outFileNameGIF)
        self.addResult(outFileNamePDF)


class MergeNt(PostProcessingStep):
    def run(self):
        ntFileName = self.getFileName('-nt.root')
        if os.path.exists('/'.join([self.taskDir,ntFileName])):
            self.logExec('cd %s; rm -f %s' % (self.taskDir,ntFileName))
        postProcFiles = self.taskFileList('*-nt.root','POSTPROCESSING',self.jobName)
        self.log(text='Found %i ntuple files from POSTPROCESSING jobs ...' % len(postProcFiles))
        completedFiles = self.taskFileList('*-nt.root','COMPLETED',self.jobName)
        self.log(text='Found %i ntuple files from COMPLETED jobs ...' % len(completedFiles))
        ntFiles = sorted(postProcFiles+completedFiles)
        if len(ntFiles)>0:
            #self.logExec('cd %s; hadd -f %s %s' % (self.taskDir,ntFileName,' '.join(ntFiles)))
            self.hadd(self.taskDir,ntFileName,ntFiles)
            self.addResult(ntFileName)
        else:
            raise PostProcessingError('WARNING: No beam spot ntuple files to merge',self.executedSteps)


class PlotBeamSpot(PostProcessingStep):
    def run(self):
        ntFileName = self.getFileName('-nt.root','BeamSpotNt')
        outFileNameGIF = self.getFileName('.gif')
        outFileNamePDF = self.getFileName('.pdf')
        if os.path.exists('/'.join([self.taskDir,ntFileName])):
            self.logExec('cd %s; beamspotnt.py -b -o %s,%s -f %s summary' % (self.taskDir,outFileNameGIF,outFileNamePDF,ntFileName))
            self.addResult(outFileNameGIF)
            self.addResult(outFileNamePDF)
        else:
            raise PostProcessingError('ERROR: No ntuple file %s - did BeamSpotNt step run?\n' % ntFileName,self.executedSteps)


class LinkResults(PostProcessingStep):
    def run(self):
        resultFiles = self.taskDict['RESULTFILES']
        resultLinks = ''
        if 'AveBeamSpot' in self.executedSteps:
            self.log(text='WARNING: You should always run AveBeamSpot after LinkResults to avoid overwriting results\n',doPrint=True)
        if not resultFiles:
            return
        for r in resultFiles.split():
            what = '.'.join(r.split('.')[-2:])
            if what in linkTemplates:
                if linkTemplates[what]:
                    resultLinks = ' '.join([resultLinks, linkTemplates[what] %  (self.dsName,self.taskName,r)])
            else:
                resultLinks = ' '.join([resultLinks,'<a href="../files?u=%s/%s/%s">%s</a>' % (self.dsName,self.taskName,r,what)])
        self.taskman.setValue(self.dsName,self.taskName,'RESULTLINKS',resultLinks)


class AveBeamSpot(PostProcessingStep):
    def run(self):
        ntFileName              = self.getFileName('-nt.root','BeamSpotNt')
        beamSpotDbFileName      = self.getFileName('-beamspot.db')
        #dataQualityDbFileName   = self.getFileName('-dqflags.db')

        if os.path.exists('/'.join([self.taskDir,beamSpotDbFileName])):
            self.logExec('cd %s; rm -f %s' % (self.taskDir,beamSpotDbFileName))
        #if os.path.exists('/'.join([self.taskDir,dataQualityDbFileName])):
        #   self.logExec('cd %s; rm -f %s' % (self.taskDir,dataQualityDbFileName))

        if os.path.exists('/'.join([self.taskDir,ntFileName])):
            cmd = 'cd %s; beamspotnt.py --tag=%s -f %s --newave ave' % (self.taskDir,beamSpotDbFileName[:-3],ntFileName)
            #cmd = 'cd %s; aveBeamSpot.py -b -o %s %s' % (self.taskDir,beamSpotDbFileName,ntFileName)
            #cmd = 'cd %s; aveBeamSpot.py -b -o %s -d %s %s' % (self.taskDir,beamSpotDbFileName,dataQualityDbFileName,ntFileName)            
            status = self.logExec(cmd,doPrint=True,abortOnError=False)
            if status and status!=2 and status!=3:
                # Some other error than simply no beam spot data
                raise PostProcessingError('ERROR in postprocessing step AveBeamSpot while executing:\n\n%s\n' % (cmd),self.executedSteps)
            resultLinks = self.taskDict['RESULTLINKS']
            if status==0:
                self.addResult(beamSpotDbFileName)
                link = '<a class="ok" href="../files/?u=%s/%s/%s.log">Beam spot</a>' % (self.dsName,self.taskName,self.baseNameStep)
                if resultLinks.find(link)==-1:
                    resultLinks = ' '.join([link,resultLinks])
            if status==2:
                link = '<a class="warn" href="../files/?u=%s/%s/%s.log">No beam spot</a>' % (self.dsName,self.taskName,self.baseNameStep)
                if resultLinks.find(link)==-1:
                    resultLinks = ' '.join([link,resultLinks])
            if status==3:
                link = '<a class="warn" href="../files/?u=%s/%s/%s.log">No vtx/beam spot</a>' % (self.dsName,self.taskName,self.baseNameStep)
                if resultLinks.find(link)==-1:
                    resultLinks = ' '.join([link,resultLinks])

#             # Add DQ flags to results and do linking
#             self.addResult(dataQualityDbFileName)
#             
#             dataQualityTxtFileName = dataQualityDbFileName.replace('.db','.txt')            
#             stat,out = subprocess.getstatusoutput('grep -c "ID_BS_NOBEAMSPOT" %s/%s' %  (self.taskDir, dataQualityTxtFileName) )
# 
#             # Catch errors (do it this way as the exit codes from grep are nasty)
#             try:
#                 nRed=int(out)                                                
#             except ValueError:
#                 raise PostProcessingError('ERROR in postprocessing step AveBeamSpot while executing:\n\n%s\n' % (cmd),self.executedSteps)
#         
#             if nRed==0:
#                 link='<a class="ok" href="../files?u=%s/%s/%s-dqflags.txt">DQFlags</a>' % (self.dsName,self.taskName,self.baseNameStep)
#             else:
#                 link='<a class="bad" href="../files?u=%s/%s/%s-dqflags.txt">DQFlags</a>' % (self.dsName,self.taskName,self.baseNameStep)                
# 
#             if resultLinks.find(link)==-1:
#                 resultLinks = ' '.join([resultLinks, link])

            # Set links by hand
            self.taskman.setValue(self.dsName,self.taskName,'RESULTLINKS',resultLinks)
        else:
            raise PostProcessingError('ERROR: No ntuple file %s - did BeamSpotNt step run?\n' % ntFileName,self.executedSteps)


class CheckT0Status(PostProcessingStep):
    """Check in T0 task DB if T0 processing is done, if not raise PostponeProcessing exception."""
    def run(self):
        filter = 'AOD'  # T0 uses AODs by default (at least for pp running)
        try:
            f = self.getJobConfig(self.jobName)['inputfiles'][0]
            if 'ESD' in f:
                filter = 'ESD'
        except Exception:
            self.log('WARNING: Unable to determine input file type - will assume default (%s)\n' % filter, doPrint=True)
        cmd = 'beamspotman.py -n -f %s queryT0 %s %s' % (filter,self.dsName,self.taskName)
        status = self.logExec(cmd,doPrint=True,abortOnError=False)
        if status:
            # The task is either not finished or there was a problem
            if status==2:
                # Task still running
                raise PostponeProcessing('Tier-0 processing still in progress - restart postprocessing later',self.executedSteps)
            else:
                # Unable to determine status
                raise PostProcessingError('ERROR: Unable to determine Tier-0 task status',self.executedSteps)


class UploadBeamSpot(PostProcessingStep):
    def run(self):
        beamSpotDbFileName = self.getFileName('-beamspot.db','AveBeamSpot')
        cooltags = self.taskDict['COOLTAGS']
        if not cooltags: cooltags = ''

        # Resolve beamspot tag
        try: 
            beamspottag = COOLUtils.resolveCurrentBeamSpotFolder()
        except Exception:
            raise PostProcessingError('ERROR: Unable to resolve beamspot folder tag',self.executedSteps,TaskManager.StatusCodes['POSTPROCFAILED'])
        
        # Check that we haven't uploaded yet
        if beamspottag in cooltags.split():
            raise PostProcessingError('ERROR: Beam spot already uploaded to COOL - if this is intentional, please upload manually.',
                                      self.executedSteps,TaskManager.StatusCodes['POSTPROCFAILED'])
        # Check that this is the express stream
        if self.dsName.split('.')[-1] != 'express_express' and self.dsName.split('.')[-1] != 'calibration_BeamSpot':
            print (self.dsName.split('.')[-1] )
            self.log(text="WARNING: Not running on the express or BeamSpot  stream, so won't upload anything - if this is intentional, please upload manually\n",doPrint=True)
            return
        if os.path.exists('/'.join([self.taskDir,beamSpotDbFileName])):
            # Dumping of the beam spot SQLite file is done by beamspotman
            #self.logExec('cd %s; dumpBeamSpot.py %s' % (self.taskDir,beamSpotDbFileName))
            self.logExec('beamspotman.py -b -n -t %s upload %s %s' % (beamspottag,self.dsName,self.taskName))
        else:
            #raise PostProcessingError('ERROR: No beam spot COOL SQLite file %s - did AveBeamSpot succeed?\n' % beamSpotDbFileName,self.executedSteps)
            self.log(text='ERROR: No beam spot COOL SQLite file %s\n       Nothing to upload - was beam spot determination successful?\n' % beamSpotDbFileName,doPrint=True)

#         # Once uploaded the beamspot can upload the DQ flags 
#         dataQualityDbFileName = self.getFileName('-dqflags.db','AveBeamSpot')
#         if os.path.exists('/'.join([self.taskDir,dataQualityDbFileName])):
#             self.logExec('beamspotman.py -b -n --dqtag %s dqflag %s %s' % (dqtag,self.dsName,self.taskName))
#         else:
#             self.log(text='ERROR: No beam spot DQ flag SQLite file %s\n       Nothing to upload - was beam spot or DQ determination successful?\n' % dqDbFileName,doPrint=True)

class DQBeamSpot(PostProcessingStep):
    """Automatically determine DQ info (after uploading beamspot)"""
    def run(self, absent=False):
        ntFileName              = self.getFileName('-nt.root','BeamSpotNt')
        dataQualityDbFileName   = self.getFileName('-dqflags.db')

        if os.path.exists('/'.join([self.taskDir,dataQualityDbFileName])):
            self.logExec('cd %s; rm -f %s' % (self.taskDir,dataQualityDbFileName))

        if os.path.exists('/'.join([self.taskDir,ntFileName])):
            cmd = 'cd %s; dqBeamSpot.py -b %s -o %s %s' % (self.taskDir,'-a' if absent else '',dataQualityDbFileName,ntFileName)
            status = self.logExec(cmd,doPrint=True,abortOnError=False)

            if status:
                raise PostProcessingError('ERROR in postprocessing step DQBeamSpot while executing:\n\n%s\n' % (cmd),self.executedSteps)

            # Add DQ flags to results and do linking
            self.addResult(dataQualityDbFileName)
            
            dataQualityTxtFileName = dataQualityDbFileName.replace('.db','.txt')            
            cmd = 'grep -c "ID_BS_NOBEAMSPOT" %s/%s' %  (self.taskDir, dataQualityTxtFileName)
            status,out = subprocess.getstatusoutput(cmd)

            # Catch errors (do it this way as the exit codes from grep are nasty)
            try:
                nRed=int(out)                                                
            except ValueError:
                raise PostProcessingError('ERROR in postprocessing step DQBeamSpot while executing:\n\n%s\n' % (cmd),self.executedSteps)
        
            if nRed==0:
                link='<a class="ok" href="../files?u=%s/%s/%s-dqflags.txt">DQFlags</a>' % (self.dsName,self.taskName,self.baseNameStep)
            else:
                link='<a class="bad" href="../files?u=%s/%s/%s-dqflags.txt">DQFlags</a>' % (self.dsName,self.taskName,self.baseNameStep)                

            resultLinks = self.taskDict['RESULTLINKS']
            if resultLinks.find(link)==-1:
                resultLinks = ' '.join([resultLinks, link])

            # Set links by hand
            self.taskman.setValue(self.dsName,self.taskName,'RESULTLINKS',resultLinks)
        else:
            raise PostProcessingError('ERROR: No ntuple file %s - did BeamSpotNt step run?\n' % ntFileName,self.executedSteps)

class DQBeamSpotReproc(DQBeamSpot):
    """Automatically determine DQ info (after uploading beamspot).  For reprocessing also upload absent defects to remove previosuly set ones"""
    def run(self):
        return DQBeamSpot.run(self, absent=True)

class UploadDataQuality(PostProcessingStep):
    """Upload beampsot data quality info"""
    def run(self):
        dataQualityDbFileName   = self.getFileName('-dqflags.db', 'DQBeamSpot')
        
        # Upload the DQ flags 
        if os.path.exists('/'.join([self.taskDir,dataQualityDbFileName])):
            self.logExec('beamspotman.py -b -n --dqtag %s dqflag %s %s' % (dqtag,self.dsName,self.taskName))            
        else:
            self.log(text='ERROR: No beam spot DQ flag SQLite file %s\n       Nothing to upload - was DQ determination successful?\n' % dataQualityDbFileName,doPrint=True)


class BeamSpotNtNoAve(PostProcessingStep):
    def run(self):
        ntFileName = self.getFileName('-nt.root','MergeNt')
        bsNtFileName = self.getFileName('-nt.root','BeamSpotNt')
        if os.path.exists('/'.join([self.taskDir,ntFileName])):
            self.logExec("cd %s; rm -f %s; beamspotnt.py -f %s --status '' --fillCOOL merge %s" % (self.taskDir,bsNtFileName,bsNtFileName,ntFileName))
            self.addResult(bsNtFileName)
        else:
            raise PostProcessingError('ERROR: No merged ntuple file %s - did MergeNt step run?\n' % ntFileName,self.executedSteps)



class BeamSpotNt(PostProcessingStep):
    def run(self):
        ntFileName = self.getFileName('-nt.root','MergeNt')
        bsNtFileName = self.getFileName('-nt.root')
        if os.path.exists('/'.join([self.taskDir,ntFileName])):
            self.logExec("cd %s; rm -f %s; beamspotnt.py -f %s --status '' --fillCOOL --useAve merge %s" % (self.taskDir,bsNtFileName,bsNtFileName,ntFileName))
            self.addResult(bsNtFileName)
        else:
            raise PostProcessingError('ERROR: No merged ntuple file %s - did MergeNt step run?\n' % ntFileName,self.executedSteps)


class BeamSpotGlobalNt(PostProcessingStep):
    def run(self):
        # Don't merge into global ntuple if no beamspot uploaded to COOL
        cooltags = self.taskDict['COOLTAGS']
        if not cooltags: cooltags = ''

        # Check if the task has uploded to COOL
        # if not beamspottag in cooltags.split():
        if not cooltags:            
            self.log(text='WARNING: Beam spot not uploaded - result not merged to global ntuple\n',doPrint=True)
            return
        
        ntFileName = self.getFileName('-nt.root','MergeNt')
        globalNtDir = '/afs/cern.ch/user/a/atlidbs/nt/t0'
        beamspottag = 'IndetBeampos-RUN2-ES1-UPD2-13' # Always update to one global ntuple so don't resolve current
        globalNtFileName = '%s/beamspotnt-%s.root' % (globalNtDir,beamspottag)
        globalNtFileName2 = '%s/beamspotnt-%s.root' % (globalNtDir,'IndetBeampos-ES1-UPD2')
        if not os.path.exists(globalNtDir):
            raise PostProcessingError('ERROR: Cannot access directory with global beam spot ntuple: %s' % globalNtDir, self.executedSteps)
        if os.path.exists('/'.join([self.taskDir,ntFileName])):
            self.logExec("cd %s; beamspotnt.py -f %s --status '' --fillCOOL --useAve merge %s" % (self.taskDir,globalNtFileName,ntFileName))
            self.logExec("cd %s; beamspotnt.py -f %s --status '' --fillCOOL --useAve merge %s" % (self.taskDir,globalNtFileName2,ntFileName))
        else:
            raise PostProcessingError('ERROR: No merged ntuple file %s - did MergeNt step run?\n' % ntFileName, self.executedSteps)


class PlotBCID(PostProcessingStep):
    def run(self):
        bsNtFileName = self.getFileName('-nt.root','BeamSpotNt')
        dumpFileName = self.getFileName('-data.txt')
        outFileNameLbGIF = self.getFileName('-vsLb.gif')
        outFileNameLbPDF = self.getFileName('-vsLb.pdf')
        outFileNameTimeGIF = self.getFileName('-vsTime.gif')
        outFileNameTimePDF = self.getFileName('-vsTime.pdf')
        outFileNameBCIDGIF = self.getFileName('-vsBCID.gif')
        outFileNameBCIDPDF = self.getFileName('-vsBCID.pdf')
        outFileNameTrainGIF = self.getFileName('-vsBunchPos.gif')
        outFileNameTrainPDF = self.getFileName('-vsBunchPos.pdf') 
        if os.path.exists('/'.join([self.taskDir,bsNtFileName])):
            self.logExec("cd %s; rm -f %s; beamspotnt.py -v -f %s dump > %s" % (self.taskDir,dumpFileName,bsNtFileName,dumpFileName))
            self.addResult(dumpFileName)
            self.logExec("cd %s; beamspotnt.py --batch -o %s,%s -f %s summary" % (self.taskDir,outFileNameLbGIF,outFileNameLbPDF,bsNtFileName))
            self.addResult(outFileNameLbGIF)
            self.addResult(outFileNameLbPDF)
            self.logExec("cd %s; beamspotnt.py --batch --time -o %s,%s -f %s summary" % (self.taskDir,outFileNameTimeGIF,outFileNameTimePDF,bsNtFileName))
            self.addResult(outFileNameTimeGIF)
            self.addResult(outFileNameTimePDF)
            self.logExec("cd %s; beamspotnt.py --batch -m 0.2 --perbcid -o %s,%s -f %s summary" % (self.taskDir,outFileNameBCIDGIF,outFileNameBCIDPDF,bsNtFileName))
            self.addResult(outFileNameBCIDGIF)
            self.addResult(outFileNameBCIDPDF)
            self.logExec("cd %s; beamspotnt.py --batch --vsbunchpos -o %s,%s -f %s summary" % (self.taskDir,outFileNameTrainGIF,outFileNameTrainPDF,bsNtFileName))
            self.addResult(outFileNameTrainGIF)
            self.addResult(outFileNameTrainPDF)
        else:
            raise PostProcessingError('ERROR: No beam spot ntuple file %s - did BeamSpotNt step run?\n' % bsNtFileName,self.executedSteps)
