# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import copy, multiprocessing, os, pickle, random, re, shutil, subprocess, sys, time
from multiprocessing.pool import ThreadPool

import ROOT

import FitResult, Tools

# --- # --- # --- # --- # 

class Job:

  def __init__ ( self, Name, Detector = 'ID' ):

    # ---> Job Info: General
    self.InitDir = os.path.abspath( '.' )
    self.BatchMode = ( os.fstat( 0 ) != os.fstat( 1 ) ) 
    self.Name = Name
    self.Ancestors = []
    self.LinkTo = ''
    self.LinkToID = ''
    self.Host = os.path.expandvars( '$HOSTNAME' ) 
    self.Stat = 1
    self.Recipients = 'giacomo.artoni@cern.ch'
    self.Delay = 100
    self.DoMonitoring = False
    self.RunningSystem = 'Local' # 'Local', 'PBS'
    self.RegionHandling = 'Parallel' # 'Serial', 'Parallel'
    self.JobsPerRegion = 1 
    self.DoCheck = True
    self.StartFromOldValues = False
    # ---> Job Info: Inputs
    self.Input = ''
    self.InputRegions = ''
    self.InputForID = ''
    self.InputRegionsForID = ''
    self.Template = os.path.expandvars( '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/templates/TemplateJob.py' )
    self.TemplatePBS = os.path.expandvars( '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/templates/TemplatePBS.sh' )
    self.TemplateMerger = os.path.expandvars( '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/templates/TemplateMerger.sh' )
    self.TemplateSimpleMerger = os.path.expandvars( '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/templates/TemplateSimpleMerger.sh' )
    self.Files = Files( '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/data', 'Data.root', 'MonteCarlo.root', 'Background.root' ) 
    self.JpsiRewFile = ''
    self.ZRewFile = ''
    # ---> Job Info: Outputs
    self.SubJobsRootFiles = None 
    self.Output = 'Corrections-%s.root' % self.Name
    self.TrimmedOutput = ''
    self.PreviousJobRegions = '' 
    self.Regions = os.path.expandvars( '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/share/Iter1Regions.txt' )
    self.OutputRegions = '' 
    self.CheckRegions = os.path.expandvars( '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/share/SingleRegion.txt' ) 
    self.Svn = '???'#self.GetSvnRevision()
    self.Start = None
    self.End = None
    self.Links = []
    # ---> Fit Info
    self.FitType = 'Chi2'
    self.Detector = Detector 
    self.Level = 'INFO'
    self.BkgParametrization = 'Exponential'
    self.Splittings = 1
    self.NumRandomValues = 1
    self.CPUTime = '02'
    self.RunMode = 'Simplex:1'
    self.GRL = os.path.expandvars( '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/grl/' )
    self.MaxEvents = -1
    self.MaxRetrials = 1
    self.ProbThreshold = 0.1
    self.RequireErrors = False
    self.MinMuonsPtForJpsi = 5
    self.MaxMuonsPtForJpsi = 15
    self.MinLeadingMuonPtForZ = 22
    self.MinSubLeadingMuonPtForZ = 15 
    self.JpsiMassBins = 85
    self.JpsiMassMin = 2.65 
    self.JpsiMassMax = 3.5 
    self.JpsiMassPts = ''
    self.ZMassBins = 100
    self.ZMassMin = 76
    self.ZMassMax = 106
    self.ZMassPts = ''
    self.ZRhoBins = 100
    self.ZRhoMin = -0.3
    self.ZRhoMax = 0.3
    self.ZRhoPts = ''
    self.Fit_s0 = True
    self.Fit_s1 = True
    self.Fit_p0 = True
    self.Fit_p1 = True
    self.Fit_p2 = True
    self.Init_s0 = 0
    self.Init_s1 = 0
    self.Init_p0 = 0
    self.Init_p1 = 0
    self.Init_p2 = 0
    self.Overwrite_s0 = False
    self.Overwrite_s1 = False
    self.Overwrite_p0 = False
    self.Overwrite_p1 = False
    self.Overwrite_p2 = False
    self.Step_s0 = 0.01
    self.Step_s1 = 0.001
    self.Step_p0 = 0.01
    self.Step_p1 = 0.001
    self.Step_p2 = 0.00005
    # ---> Resources Info
    self.Cores = 8

  def StartFrom ( self, JobName ):
    
    with open( '%s/AutoSave-%s.pkl' % ( JobName, JobName ), 'rb' ) as Input:
      PreviousJob = pickle.load( Input )
      if PreviousJob.Detector == 'ID' and self.Detector == 'MS':
        self.CopyInfoFrom( PreviousJob )
        self.PreviousJobRegions = PreviousJob.Regions 
        if hasattr( PreviousJob, 'TrimmedOutput' ):
          self.InputForID = PreviousJob.TrimmedOutput
        else:
          self.InputForID = PreviousJob.Output
        self.InputRegionsForID = PreviousJob.OutputRegions
        self.Input = '' 
        self.InputRegions = '' 
        self.Output = 'Corrections-%s.root' % self.Name
        self.LinkToID = PreviousJob.LinkTo
        self.Ancestors.append( PreviousJob.LinkTo )
        self.Links = []
        self.LinkTo = ''
      else:
        self.CopyInfoFrom( PreviousJob )
        # ---> Careful when copying these members!
        self.PreviousJobRegions = PreviousJob.Regions 
        if hasattr( PreviousJob, 'TrimmedOutput' ):
          self.Input = PreviousJob.TrimmedOutput
        else:
          self.Input = PreviousJob.Output
        self.PreviousJobRegions = PreviousJob.Regions 
        self.InputRegions = PreviousJob.OutputRegions
        self.Output = 'Corrections-%s.root' % self.Name
        self.Ancestors.append( PreviousJob.LinkTo )
        self.Links = []
        self.LinkTo = ''

  def CopyInfoFrom ( self, theJob ):
    
    for Element in dir( self ):
      if '__' in Element or callable( getattr( self, Element ) ) or Element == 'Name' or Element == 'Detector' or Element == 'Host' or not hasattr( theJob, Element ):
        pass
      else:
        if isinstance( getattr( theJob, Element ), list ):
          setattr( self, Element, copy.deepcopy( getattr( theJob, Element ) ) )
        setattr( self, Element, getattr( theJob, Element ) )

  def Run ( self ):
    
    Tools.CreateAndMoveIn( self.Name, 'Force' ) 
    #self.Commit()
    Begin = '\033[94m MuonCalibrationFit ----> '
    End = ' \033[0m'
    print Begin + ' Executing Job ' + self.Name + End
    self.Execute()
    print Begin + ' Saving Job ' + self.Name + End
    self.Save()
    print Begin + ' Producing Plots for Job ' + self.Name + End
    self.PublishPlots()
    print Begin + ' Sending Notification Mail for Job ' + self.Name + End
    self.SendMail()
    print Begin + ' (Re)Saving Job ' + self.Name + End
    self.Save()
    #===#
    if not 'Check' in self.Name and self.DoCheck:
      self.RunCheck()

  def RunCheck ( self ):

      checkJobSingle = Job( Name = self.Name + 'Check', Detector = self.Detector ) 
      checkJobSingle.StartFrom( self.Name )
      checkJobSingle.RunMode = ''
      checkJobSingle.Regions = self.CheckRegions
      if checkJobSingle.JpsiMassPts != '':
        checkJobSingle.JpsiMassPts = '6 20'
      if checkJobSingle.ZMassPts != '':
        checkJobSingle.ZMassPts = '25 300'
      if checkJobSingle.ZRhoPts != '':
        checkJobSingle.ZRhoPts = '25 300'
      checkJobSingle.DoMonitoring = True 
      checkJobSingle.Init_s0 = 0
      checkJobSingle.Init_s1 = 0
      checkJobSingle.Init_p0 = 0
      checkJobSingle.Init_p1 = 0
      checkJobSingle.Init_p2 = 0
      checkJobSingle.JobsPerRegion = 1
      checkJobSingle.Delay = 5
      checkJobSingle.Run()

      #checkJobPtBins = Job( Name = self.Name + '_CheckPtBins', Detector = self.Detector ) 
      #checkJobPtBins.StartFrom( self.Name )
      #checkJobPtBins.RunMode = ''
      #checkJobPtBins.Regions = self.CheckRegions
      #checkJobPtBins.DoMonitoring = True 
      #checkJobPtBins.Init_s0 = 0
      #checkJobPtBins.Init_s1 = 0
      #checkJobPtBins.Init_p0 = 0
      #checkJobPtBins.Init_p1 = 0
      #checkJobPtBins.Init_p2 = 0
      #checkJobPtBins.JobsPerRegion = 1
      #checkJobPtBins.Delay = 5
      #checkJobPtBins.Run()

      splitJobSingle = Job( Name = self.Name + 'SplitCheck', Detector = self.Detector ) 
      splitJobSingle.StartFrom( self.Name )
      splitJobSingle.RunMode = ''
      splitJobSingle.Regions = self.Regions
      if splitJobSingle.JpsiMassPts != '':
        splitJobSingle.JpsiMassPts = '6 20'
      if splitJobSingle.ZMassPts != '':
        splitJobSingle.ZMassPts = '25 300'
      if splitJobSingle.ZRhoPts != '':
        splitJobSingle.ZRhoPts = '25 300'
      splitJobSingle.DoMonitoring = False
      splitJobSingle.Init_s0 = 0
      splitJobSingle.Init_s1 = 0
      splitJobSingle.Init_p0 = 0
      splitJobSingle.Init_p1 = 0
      splitJobSingle.Init_p2 = 0
      splitJobSingle.JobsPerRegion = 1
      splitJobSingle.Delay = 5
      splitJobSingle.Run()

      #splitJobPtBins = Job( Name = self.Name + '_SplitCheckPtBins', Detector = self.Detector ) 
      #splitJobPtBins.StartFrom( self.Name )
      #splitJobPtBins.RunMode = ''
      #splitJobPtBins.Regions = self.Regions
      #splitJobPtBins.DoMonitoring = False
      #splitJobPtBins.Init_s0 = 0
      #splitJobPtBins.Init_s1 = 0
      #splitJobPtBins.Init_p0 = 0
      #splitJobPtBins.Init_p1 = 0
      #splitJobPtBins.Init_p2 = 0
      #splitJobPtBins.JobsPerRegion = 1
      #splitJobPtBins.Delay = 5
      #splitJobPtBins.Run()

  def __str__ ( self ):

    res = ''
    for Element in dir( self ):
      if '__' in Element or callable( getattr( self, Element ) ):
        pass
      else:
        res += '<h4>%s: %s</h4>' % ( Element, str( getattr( self, Element ) ) )
    return res

  def GetSvnRevision ( self ):

    return subprocess.check_output( "svn info svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit | grep Revision | awk '{print $2}'", shell = True ).rstrip()

  def Commit ( self ):

    subprocess.call( 'svn ci $TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit -m "before sending job named: %s"' % self.Name, shell = True )
    self.Svn = self.GetSvnRevision() 

  def Execute ( self ):

    if self.RegionHandling == 'Parallel' or 'Check' in self.Name:
      self.ParallelExecute()
    elif self.RegionHandling == 'Serial':
      self.SerialExecute()

  def SerialExecute ( self ):

    self.Start = time.time()
    TempRegions = self.SplitRegions()
    PreviousRegionMergeJob = None 
    if self.Input == '':
      PreviousRegionOutput = 'NONE' 
      PreviousRegionOutputs = []
    else:
      PreviousRegionOutput = self.Input 
      PreviousRegionOutputs = [ self.Input ]
    if self.InputRegions == '':
      PreviousRegionFile = 'NONE' 
    else:
      PreviousRegionFile = self.InputRegions
    for ( Region, File ) in TempRegions:
      os.chdir( Region )
      SingleRegionJob = self.CreateRegionJob( Region, File )
      Inputs = []
      SingleRegionSubJobs = []
      for Index in range( self.JobsPerRegion ):
        IndexName = 'Trial%d' % Index
        SingleRegionSubJob = self.CreateRegionJob( Region, File, IndexName, PreviousRegionOutput, PreviousRegionFile )
        pbsTemplate = open( self.TemplatePBS )
        job_sub_name = 'Submit_%s_%s.sh' % ( Region, IndexName )
        jobFile = open( job_sub_name, 'w' )
        while 1:
          line = pbsTemplate.readline()
          if not line: break
          line = line.replace( '__JOBOPTIONS_NAME__', SingleRegionSubJob.Script )
          line = line.replace( '__CURRENT_DIR__', os.getcwd() )
          line = line.replace( '__TIME__', self.CPUTime )
          line = line.replace( '__OUT_FILE__', SingleRegionSubJob.Out )
          line = line.replace( '__ERR_FILE__', SingleRegionSubJob.Err )
          jobFile.write( line )
        jobFile.close()
        os.system( 'chmod +x %s' % job_sub_name )
        Dep = ''
        if PreviousRegionMergeJob:
          Dep = '-W depend=afterok:%s' % PreviousRegionMergeJob
        SingleRegionSubJobs.append( ( Tools.SendCommand( 'qsub %s %s' % ( Dep, job_sub_name ) )[ 0 ].rstrip( '\n' ), SingleRegionSubJob ) )
        Inputs.append( SingleRegionSubJob.RootFile )
      # Merger Job
      pbs_mergerTemplate = open( self.TemplateMerger )
      job_name = 'Submit_Merge_%s.sh' % Region
      jobFile = open( job_name, 'w' )
      CheckScan = ''
      if 'SimpleScan' in self.RunMode:
        CheckScan = '--check-scan' 
      while 1:
        line = pbs_mergerTemplate.readline()
        if not line: break
        line = line.replace( '__INPUTS__', ','.join( Inputs ) ) 
        line = line.replace( '__FUNCTION__', str( FitResult.GetFunctionIndex( self.FitType ) ) ) 
        line = line.replace( '__JOBNAME__', self.Name ) 
        line = line.replace( '__REGIONNAME__', Region ) 
        line = line.replace( '__CURRENT_DIR__', os.getcwd() )
        line = line.replace( '__OUT_FILE__', SingleRegionJob.Out )
        line = line.replace( '__ERR_FILE__', SingleRegionJob.Err )
        line = line.replace( '__OUTPUT__', SingleRegionJob.RootFile ) 
        line = line.replace( '__INPUTS_SPACE__', ' '.join( PreviousRegionOutputs ) ) 
        line = line.replace( '__FIRST_REG_FILE__', File ) 
        line = line.replace( '__SECOND_REG_FILE__', PreviousRegionFile ) 
        RegionsSoFar = File.replace( '.txt', 'AllSoFar.txt' )
        line = line.replace( '__OUT_REG_FILE__', RegionsSoFar ) 
        line = line.replace( '__CHECK_SCAN__', CheckScan ) 
        jobFile.write( line )
      jobFile.close()
      JobsJustSent = [ Pair[ 0 ] for Pair in SingleRegionSubJobs ]
      PreviousRegionMergeJob = Tools.SendCommand( 'qsub -W depend=afterany:%s %s' % ( ':'.join( JobsJustSent ), job_name ) )[ 0 ].rstrip( '\n' ) 
      PreviousRegionOutput = SingleRegionJob.RootFile 
      PreviousRegionOutputs.append( os.path.abspath( 'temp_output_file.root' ) )
      PreviousRegionFile = RegionsSoFar
      #####
      os.chdir( '..' )
    JobsFinished = False
    while not JobsFinished:
      time.sleep( self.Delay )
      JobsFinished = Tools.PBSJobEnded( PreviousRegionMergeJob )
    self.Output = Tools.MergeRootFiles( self.Output, [ PreviousRegionOutput ] ) 
    self.TrimmedOutput = Tools.TrimRootFile( self.Output, self.Output.replace( 'Corrections', 'OnlyCorrections' ) ) 
    shutil.copyfile( PreviousRegionFile, '%s/Regions-%s.txt' % ( os.getcwd(), self.Name ) )
    self.OutputRegions = '%s/Regions-%s.txt' % ( os.getcwd(), self.Name ) 
    os.chdir( '..' )
    self.End = time.time()

  def ParallelExecute ( self ):

    self.Start = time.time() 
    Pool = None
    if self.RunningSystem == 'Local':
      Pool = ThreadPool( self.Cores )
    TempRegions = self.SplitRegions()
    SubJobs = []
    for ( Region, File ) in TempRegions:
      os.chdir( Region )
      this_SubJob = self.CreateRegionJob( Region, File )
      # Running each region in parallel
      if self.RunningSystem == 'Local':
        SubJobs.append( ( Pool.apply_async( Tools.SendCommand, ( 'athena.py %s' % this_SubJob.Script, ) ), this_SubJob ) )
      elif self.RunningSystem == 'PBS':
        Inputs = []
        SingleRegionSubJobs = []
        AllJobOutputs = []
        AllJobOutOutputs = []
        for Index in range( self.JobsPerRegion ):
          IndexName = 'Trial%d' % Index
          SingleRegionSubJob = self.CreateRegionJob( Region, File, IndexName )
          pbsTemplate = open( self.TemplatePBS )
          job_sub_name = 'Submit_%s_%s.sh' % ( Region, IndexName )
          jobFile = open( job_sub_name, 'w' )
          while 1:
            line = pbsTemplate.readline()
            if not line: break
            line = line.replace( '__JOBOPTIONS_NAME__', SingleRegionSubJob.Script )
            line = line.replace( '__CURRENT_DIR__', os.getcwd() )
            line = line.replace( '__TIME__', self.CPUTime )
            line = line.replace( '__OUT_FILE__', SingleRegionSubJob.Out )
            line = line.replace( '__ERR_FILE__', SingleRegionSubJob.Err )
            jobFile.write( line )
          jobFile.close()
          os.system( 'chmod +x %s' % job_sub_name )
          AllJobOutputs.append( SingleRegionSubJob.RootFile )
          AllJobOutOutputs.append( SingleRegionSubJob.Out )
          SingleRegionSubJobs.append( ( Tools.SendCommand( 'qsub %s' % ( job_sub_name ) )[ 0 ].rstrip( '\n' ), SingleRegionSubJob ) )
          time.sleep( 1 )
        # Merger Job
        pbs_mergerTemplate = open( self.TemplateSimpleMerger )
        job_name = 'Submit_Merge_%s.sh' % Region
        jobFile = open( job_name, 'w' )
        CheckScan = ''
        if 'SimpleScan' in self.RunMode:
          CheckScan = '--check-scan' 
        while 1:
          line = pbs_mergerTemplate.readline()
          if not line: break
          line = line.replace( '__FUNCTION__', str( FitResult.GetFunctionIndex( self.FitType ) ) ) 
          line = line.replace( '__CURRENT_DIR__', os.getcwd() )
          line = line.replace( '__JOBNAME__', self.Name ) 
          line = line.replace( '__REGIONNAME__', Region ) 
          line = line.replace( '__OUT_INPUTS__', ','.join( AllJobOutOutputs ) ) 
          line = line.replace( '__INPUTS__', ','.join( AllJobOutputs ) ) 
          line = line.replace( '__OUT_FILE__', this_SubJob.Out )
          line = line.replace( '__ERR_FILE__', this_SubJob.Err )
          line = line.replace( '__OUTPUT__', this_SubJob.RootFile ) 
          line = line.replace( '__CHECK_SCAN__', CheckScan ) 
          jobFile.write( line )
        jobFile.close()
        os.system( 'chmod +x %s' % job_name )
        JobsJustSent = [ Pair[ 0 ] for Pair in SingleRegionSubJobs ]
        #SubJobs.append( ( Tools.SendCommand( 'qsub -W depend=afterany:%s %s' % ( ':'.join( JobsJustSent ), job_name ) )[ 0 ].rstrip( '\n' ), this_SubJob ) )
        SubJobs.append( ( Tools.SendCommand( 'qsub %s' % ( job_name ) )[ 0 ].rstrip( '\n' ), this_SubJob ) )
      os.chdir( '..' )
      print SubJobs
    ## Checking job status
    if self.RunningSystem == 'Local':
      UnfinishedSubJobs = [ True ]
      while any( UnfinishedSubJobs ):
        time.sleep( self.Delay )
        UnfinishedSubJobs = []
        for ( Res, SubJob ) in SubJobs:
          UnfinishedSubJobs.append( not Res.ready() )
          if Res.ready():
            Out, Err = Res.get()
            FileOut = open( SubJob.Out, 'w' )
            FileOut.write( Out )
            FileOut.close()
            FileErr = open( SubJob.Err, 'w' )
            FileErr.write( Err )
            FileErr.close()
      Pool.close()
      Pool.join()
    elif self.RunningSystem == 'PBS':
      print 'Waiting for jobs to finish...'
      UnfinishedSubJobs = [ True ]
      while any( UnfinishedSubJobs ):
        time.sleep( self.Delay )
        UnfinishedSubJobs = []
        for ( JobNum, SubJob ) in SubJobs:
          UnfinishedSubJobs.append( not Tools.PBSJobEnded( JobNum, SubJob.Out ) )
        print UnfinishedSubJobs
      print 'Jobs have finished!!'
    self.SubJobsRootFiles = [ SubJob.RootFile for ( Res, SubJob ) in SubJobs ]
    self.Output = Tools.MergeRootFiles( self.Output, self.SubJobsRootFiles ) 
    self.TrimmedOutput = Tools.TrimRootFile( self.Output, self.Output.replace( 'Corrections', 'OnlyCorrections' ) ) 
    self.OutputRegions = Tools.MergeTxtFiles( 'Regions-%s.txt' % self.Name, [ SubJob.Reg for ( Res, SubJob ) in SubJobs ] )
    os.chdir( '..' )
    self.End = time.time()

  def UpdateRootFile ( self ):

    print ' ... Removing %s' % self.Output
    os.remove( self.Output )
    print ' ... Re-creating %s' % self.Output
    Tools.MergeRootFiles( self.Output, self.SubJobsRootFiles )

  def GetEtaSet ( self ):

    temp_list = [ line.strip() for line in open( self.Regions ).readlines() ]
    res = set()
    for index, line in enumerate( temp_list ):
      if index != 0:
        res.add( ( line.split()[ 0 ], line.split()[ 1 ] ) )
    return res

  def PublishPlots ( self ):

    import DumpAllPlots
    self.Links = DumpAllPlots.Run( self, 'Force' )

  def SendMail ( self ):
    
    MailContent  = 'Subject: Results available for MuonCalibrationFit - %s\n' % self.Name
    MailContent += 'FROM: MuonCalibrationFit\n'
    MailContent += 'TO: %s\n' % self.Recipients
    MailContent += 'Content-Type: text/html; charset=ISO-8859-1\n'
    MailContent += '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"><html>\n'
    MailContent += '<br>MuonCalibrationFit: <b>%s</b><br><br>' % self.Name
    MailContent += 'Start date: %s<br>' % time.asctime( time.localtime( self.Start ) )
    MailContent += 'End date: %s<br>' % time.asctime( time.localtime( self.End ) )
    MailContent += 'Total time taken: <font color="#CC0000">%s</font><br><br>' % Tools.GetHumanTimeFormat( self.End - self.Start ) 
    MailContent += 'SVN Info: Revision n.%s (<a href="https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit?rev=%s">Link to code</a>)<br><br>' % ( self.Svn, self.Svn )
    MailContent += 'Job Info: Links ('
    if self.Links:
      MyLinks = []
      for Index, Link in enumerate( self.Links ):
        MyLinks.append( Tools.GetInfoOnLink( Link ) )
      MyLinks.sort( key = lambda x: x[ 0 ] )
      for Index, MyLink in enumerate( MyLinks ):
        if Index > 0:
          MailContent += ', '
        MailContent += MyLink[ 1 ]
    MailContent += ')<br><br>'
    MailContent += 'Job History: %s' % self.LinkTo
    for thisLink in reversed( self.Ancestors ):
      MailContent += ' &nbsp; &#8592; &nbsp; %s' % thisLink
    MailContent += '<br><br>'
    if self.Detector == 'MS':
      MailContent += 'ID Corrections From: %s<br><br>' % self.LinkToID
    OutFile = open( 'Mail', 'w' )
    OutFile.write( MailContent )
    OutFile.close()
    #subprocess.call( 'ssh gartoni@lxplus.cern.ch -f \'echo "%s" | /usr/sbin/sendmail %s\'' % ( MailContent, self.Recipients ), shell = True )
    subprocess.call( 'mv Mail /afs/cern.ch/user/g/gartoni/www/Work/Muons/Calibration/mails', shell = True )

  def Save ( self ):
    with open( '%s/AutoSave-%s.pkl' % ( self.Name, self.Name ), 'wb' ) as Output:
      pickle.dump( self, Output, pickle.HIGHEST_PROTOCOL )

  def CreateRegionJob ( self, Region, RegionFile, IndexName = None, ExtraInputRootFile = None, ExtraInputRegionFile = None ):

    Res = RegionJob()
    UpdatedReg = Region
    if IndexName:
      UpdatedReg += IndexName
    Res.Script = os.path.abspath( 'JobOptions_%s.py' % UpdatedReg )
    Res.RootFile = os.path.abspath( 'Output%s.root' % UpdatedReg )
    Res.Out = os.path.abspath( 'Output%s.out' % UpdatedReg )
    Res.Err = os.path.abspath( 'Output%s.err' % UpdatedReg )
    Res.Reg = RegionFile
    myTemplate = open( self.Template )
    jobFileName = Res.Script
    jobFile = open( jobFileName, 'w' )
    while 1:
      line = myTemplate.readline()
      if not line: break
      line = line.replace( '_BATCHMODE_', str( self.BatchMode ) )
      line = line.replace( '_DETECTOR_', self.Detector )
      line = line.replace( '_FITTYPE_', self.FitType )
      line = line.replace( '_BKGPARAM_', self.BkgParametrization )
      line = line.replace( '_LEVEL_', self.Level )
      line = line.replace( '_SPLITTING_', str( self.Splittings ) )
      line = line.replace( '_NUMRANDOM_', str( self.NumRandomValues ) )
      line = line.replace( '_PROBTHRESHOLD_', str( self.ProbThreshold ) )
      line = line.replace( '_RUNMODE_', self.RunMode )
      line = line.replace( '_GRL_', self.GRL )
      line = line.replace( '_MONITORING_', str( self.DoMonitoring ) )
      line = line.replace( '_MAX_EVENTS_', str( self.MaxEvents ) )
      line = line.replace( '_MAXRETRIALS_', str( self.MaxRetrials ) )
      line = line.replace( '_REQUIRE_ERRORS_', str( self.RequireErrors ) )
      line = line.replace( '_MIN_JPSI_PT_CUT_', str( self.MinMuonsPtForJpsi ) )
      line = line.replace( '_MAX_JPSI_PT_CUT_', str( self.MaxMuonsPtForJpsi ) )
      line = line.replace( '_MIN_Z_LEAD_PT_CUT_', str( self.MinLeadingMuonPtForZ ) )
      line = line.replace( '_MIN_Z_SUB_PT_CUT_', str( self.MinSubLeadingMuonPtForZ ) ) 
      #####
      if ExtraInputRootFile and ExtraInputRegionFile:
        line = line.replace( '_INPUT_', ExtraInputRootFile )
        line = line.replace( '_INPUTREGION_', ExtraInputRegionFile )
      else:
        line = line.replace( '_INPUT_', self.Input )
        line = line.replace( '_INPUTREGION_', self.InputRegions )
      #####
      line = line.replace( '_INPUTFORID_', self.InputForID )
      line = line.replace( '_INPUTREGIONFORID_', self.InputRegionsForID )
      line = line.replace( '_OUTPUT_', Res.RootFile )
      line = line.replace( '_OUTPUTREGION_', RegionFile )
      #line = line.replace( '_CHECKREGION_', self.CheckRegion )
      line = line.replace( '_DATA_', self.Files.Data )
      line = line.replace( '_MONTECARLO_', self.Files.MonteCarlo )
      line = line.replace( '_BACKGROUND_', self.Files.Background )
      line = line.replace( '_JPSI_REW_', self.JpsiRewFile )
      line = line.replace( '_Z_REW_', self.ZRewFile )
      line = line.replace( '_JPSI_MASS_BINS_', str( self.JpsiMassBins ) )
      line = line.replace( '_JPSI_MASS_MIN_', str( self.JpsiMassMin ) )
      line = line.replace( '_JPSI_MASS_MAX_', str( self.JpsiMassMax ) )
      line = line.replace( '_JPSI_MASS_PTS_', self.JpsiMassPts )
      line = line.replace( '_Z_MASS_BINS_', str( self.ZMassBins ) )
      line = line.replace( '_Z_MASS_MIN_', str( self.ZMassMin ) )
      line = line.replace( '_Z_MASS_MAX_', str( self.ZMassMax ) )
      line = line.replace( '_Z_MASS_PTS_', self.ZMassPts )
      line = line.replace( '_Z_RHO_BINS_', str( self.ZRhoBins ) )
      line = line.replace( '_Z_RHO_MIN_', str( self.ZRhoMin ) )
      line = line.replace( '_Z_RHO_MAX_', str( self.ZRhoMax ) )
      line = line.replace( '_Z_RHO_PTS_', self.ZRhoPts )
      line = line.replace( '_USE_S0_', str( self.Fit_s0 ) )
      line = line.replace( '_USE_S1_', str( self.Fit_s1 ) )
      line = line.replace( '_USE_P0_', str( self.Fit_p0 ) )
      line = line.replace( '_USE_P1_', str( self.Fit_p1 ) )
      line = line.replace( '_USE_P2_', str( self.Fit_p2 ) )
      ###
      local_Init_s0 = self.Init_s0
      local_Init_s1 = self.Init_s1
      local_Init_p0 = self.Init_p0
      local_Init_p1 = self.Init_p1
      local_Init_p2 = self.Init_p2
      if isinstance( self.Init_s0, dict ):
        local_Init_s0 = self.Init_s0[ Region ] 
      if isinstance( self.Init_s1, dict ):
        local_Init_s1 = self.Init_s1[ Region ] 
      if isinstance( self.Init_p0, dict ):
        local_Init_p0 = self.Init_p0[ Region ] 
      if isinstance( self.Init_p1, dict ):
        local_Init_p1 = self.Init_p1[ Region ] 
      if isinstance( self.Init_p2, dict ):
        local_Init_p2 = self.Init_p2[ Region ] 
      # Check if previously used region is the same as current
      # in this case we can use as initial values the ones derived in the previous iteration!
      if ( self.Regions == self.PreviousJobRegions ) and ( self.Regions != '' ):
        if self.StartFromOldValues:
          S0_to_be_used = self.GetParameter( self.Input, Region, 0 )
          if self.Overwrite_s0:
            S0_to_be_used = local_Init_s0
          line = line.replace( '_INIT_S0_', str( S0_to_be_used ) ) 
          S1_to_be_used = self.GetParameter( self.Input, Region, 3 )
          if self.Overwrite_s1:
            S1_to_be_used = local_Init_s1
          line = line.replace( '_INIT_S1_', str( S1_to_be_used ) )
          P0_to_be_used = self.GetParameter( self.Input, Region, 6 )
          if self.Overwrite_p0:
            P0_to_be_used = local_Init_p0
          line = line.replace( '_INIT_P0_', str( P0_to_be_used ) )
          P1_to_be_used = self.GetParameter( self.Input, Region, 9 )
          if self.Overwrite_p1:
            P1_to_be_used = local_Init_p1
          line = line.replace( '_INIT_P1_', str( P1_to_be_used ) )
          P2_to_be_used = self.GetParameter( self.Input, Region, 12 )
          if self.Overwrite_p2:
            P2_to_be_used = local_Init_p2
          line = line.replace( '_INIT_P2_', str( P2_to_be_used ) ) 
        elif self.JobsPerRegion > 1:
          if self.Fit_s0:
            line = line.replace( '_INIT_S0_', str( float( self.GetParameter( self.Input, Region, 0  ) ) + 5. * random.uniform( -self.Step_s0, self.Step_s0 ) ) ) 
          else:
            line = line.replace( '_INIT_S0_', self.GetParameter( self.Input, Region, 0 ) ) 
          if self.Fit_s1:
            line = line.replace( '_INIT_S1_', str( float( self.GetParameter( self.Input, Region, 3  ) ) + 5. * random.uniform( -self.Step_s1, self.Step_s1 ) ) ) 
          else:
            line = line.replace( '_INIT_S1_', self.GetParameter( self.Input, Region, 3 ) ) 
          if self.Fit_p0:
            line = line.replace( '_INIT_P0_', str( float( self.GetParameter( self.Input, Region, 6  ) ) + 5. * random.uniform( -self.Step_p0, self.Step_p0 ) ) ) 
          else:
            line = line.replace( '_INIT_P0_', self.GetParameter( self.Input, Region, 6 ) ) 
          if self.Fit_p1:
            line = line.replace( '_INIT_P1_', str( float( self.GetParameter( self.Input, Region, 9  ) ) + 5. * random.uniform( -self.Step_p1, self.Step_p1 ) ) ) 
          else:
            line = line.replace( '_INIT_P1_', self.GetParameter( self.Input, Region, 9 ) ) 
          if self.Fit_p2:
            line = line.replace( '_INIT_P2_', str( float( self.GetParameter( self.Input, Region, 12  ) ) + 5. * random.uniform( -self.Step_p2, self.Step_p2 ) ) ) 
          elif isinstance( self.Init_p2, dict ):
            line = line.replace( '_INIT_P2_', str( local_Init_p2 ) ) 
          else:
            line = line.replace( '_INIT_P2_', self.GetParameter( self.Input, Region, 12 ) ) 
        else: 
          #line = line.replace( '_INIT_S0_', self.GetParameter( self.Input, Region, 0  ) ) 
          #line = line.replace( '_INIT_S1_', self.GetParameter( self.Input, Region, 3  ) ) 
          #line = line.replace( '_INIT_P0_', self.GetParameter( self.Input, Region, 6  ) ) 
          #line = line.replace( '_INIT_P1_', self.GetParameter( self.Input, Region, 9  ) ) 
          #line = line.replace( '_INIT_P2_', self.GetParameter( self.Input, Region, 12 ) ) 
          line = line.replace( '_INIT_S0_', str( local_Init_s0 ) ) 
          line = line.replace( '_INIT_S1_', str( local_Init_s1 ) ) 
          line = line.replace( '_INIT_P0_', str( local_Init_p0 ) ) 
          line = line.replace( '_INIT_P1_', str( local_Init_p1 ) ) 
          line = line.replace( '_INIT_P2_', str( local_Init_p2 ) ) 
      elif self.JobsPerRegion > 1:
        if self.Fit_s0:
          line = line.replace( '_INIT_S0_', str( local_Init_s0 + random.uniform( -2 * self.Step_s0, 2 * self.Step_s0 ) ) ) 
        else:
          line = line.replace( '_INIT_S0_', str( local_Init_s0 ) ) 
        if self.Fit_s1:
          line = line.replace( '_INIT_S1_', str( local_Init_s1 + random.uniform( -2 * self.Step_s1, 2 * self.Step_s1 ) ) ) 
        else:
          line = line.replace( '_INIT_S1_', str( local_Init_s1 ) ) 
        if self.Fit_p0:
          line = line.replace( '_INIT_P0_', str( local_Init_p0 + random.uniform( -2 * self.Step_p0, 2 * self.Step_p0 ) ) ) 
        else:
          line = line.replace( '_INIT_P0_', str( local_Init_p0 ) ) 
        if self.Fit_p1:
          line = line.replace( '_INIT_P1_', str( local_Init_p1 + random.uniform( -2 * self.Step_p1, 2 * self.Step_p1 ) ) ) 
        else:
          line = line.replace( '_INIT_P1_', str( local_Init_p1 ) ) 
        if self.Fit_p2:
          line = line.replace( '_INIT_P2_', str( local_Init_p2 + random.uniform( -2 * self.Step_p2, 2 * self.Step_p2 ) ) ) 
        else:
          line = line.replace( '_INIT_P2_', str( local_Init_p2 ) ) 
      else:
        line = line.replace( '_INIT_S0_', str( local_Init_s0 ) ) 
        line = line.replace( '_INIT_S1_', str( local_Init_s1 ) ) 
        line = line.replace( '_INIT_P0_', str( local_Init_p0 ) ) 
        line = line.replace( '_INIT_P1_', str( local_Init_p1 ) ) 
        line = line.replace( '_INIT_P2_', str( local_Init_p2 ) ) 
      jobFile.write( line )
    jobFile.close()
    os.system( 'chmod +x %s' % jobFileName )
    return Res

  def GetParameter( self, theInput, theRegion, Index ):
  
    tempFile = ROOT.TFile( theInput )
    theRegex = re.compile( '.*Corrections-(\w+).root' )
    theMatch = theRegex.match( theInput )
    theVector = tempFile.Get( 'FinalResults___%s___%s' % ( theMatch.group( 1 ), theRegion ) )
    return str( theVector[ Index ] )

  def SplitRegions( self ):

    returnList = []
    fileContent = open( self.Regions ).readlines()
    r = re.compile( '\s*(\S*)\s*(\S*)\s*(\S*)\s*(\S*)\s*(\S*)\s*' )
    for line in fileContent[ 1: ]:
      m = r.search( line )
      Tools.CreateAndMoveIn( m.group( 5 ), 'Stop' ) 
      tempFile = open( 'File_%s.txt' % m.group( 5 ), 'w' )
      tempFile.write( fileContent[ 0 ] )
      tempFile.write( line.replace( m.group( 5 ), '%s___%s' % ( self.Name, m.group( 5 ) ) ) )
      tempFile.close()
      returnList.append( ( m.group( 5 ), os.path.abspath( 'File_%s.txt' % m.group( 5 ) ) ) )
      os.chdir( '..' )
    '''
    if self.RegionHandling == 'Serial':
      GlobRegName = 'RegionAll'
      Tools.CreateAndMoveIn( GlobRegName, 'Stop' )
      tempFile = open( 'File_%s.txt' % GlobRegName, 'w' )
      tempFile.write( fileContent[ 0 ] )
      for line in fileContent[ 1: ]:
        m = r.search( line )
        tempFile.write( line.replace( m.group( 5 ), '%s___%s' % ( self.Name, m.group( 5 ) ) ) )
      tempFile.close()
      returnList.append( ( GlobRegName, os.path.abspath( 'File_%s.txt' % GlobRegName ) ) )
      os.chdir( '..' )
    elif self.RegionHandling == 'Parallel':
    '''
    return returnList

  def DumpFiles( self, Name ):

    ### Print regions used in this fit
    InitReg = open( self.Regions )
    OutReg = open( 'Regions_%s.dat' % Name, 'w' )
    while 1:
      line = InitReg.readline()
      if not line: break
      OutReg.write( line )
    OutReg.close()

    theFile = ROOT.TFile( self.Output )
    Keys = ROOT.gDirectory.GetListOfKeys() 

    OutScale = open( 'Scales_%s.dat' % Name, 'w' )
    OutScale.write( 'Scale_ID     s0_MS        Scale_MS     Scale_ID_SUp s0_MS_SUp    Scale_MS_SUp Scale_ID_SDw s0_MS_SDw    Scale_MS_SDw Scale_CB     Scale_CB_err\n' )
    for Key in Keys:
      theName = Key.GetName()
      if 'TVectorT<double>' in Key.GetClassName() and 'FinalResults' in theName:
        the_vector = theFile.Get( theName )
        OutScale.write( '%f   0    0    0     0     0     0     0     0    -1     -1\n' % the_vector[ 3 ] ) 
    OutScale.close()
        
    OutSmearing = open( 'Smearings_%s.dat' % Name, 'w' )
    OutSmearing.write( 'p1_ID        p2_ID        p2_ID_TG2    p0_MS        p1_MS        p2_MS        SUp_p1_ID    SUp_p2_ID    SUp_p2_ID_TG2 SUp_p0_MS   SUp_p1_MS    SUp_p2_MS    SDw_p1_ID    SDw_p2_ID    SDw_p2_ID_TG2 SDw_p0_MS   SDw_p1_MS    SDw_p2_MS\n' )
    for Key in Keys:
      theName = Key.GetName()
      if 'TVectorT<double>' in Key.GetClassName() and 'FinalResults' in theName:
        the_vector = theFile.Get( theName )
        OutSmearing.write( '%f   %f    0    0     0     0     0     0     0    0    0    0     0     0     0    0    0     0\n' % ( the_vector[ 9 ], the_vector[ 12 ] ) )
    OutSmearing.close()

    theFile.Close()

# --- # --- # --- # --- # 

class Files:

  def __init__ ( self, Dir, Data, MonteCarlo, Background ):

    myDir = os.path.expandvars( Dir )
    if myDir == '' or myDir[ -1 ] == '/':
      pass
    else:
      myDir += '/'
    self.Data = myDir + Data 
    self.MonteCarlo = myDir + MonteCarlo 
    self.Background = myDir + Background 

  def __str__ ( self ):

    res = ''
    for Element in dir( self ):
      if '__' in Element:
        pass
      else:
        res += '<h5>%s: %s</h5>' % ( Element, str( getattr( self, Element ) ) )
    return res

# --- # --- # --- # --- # 

class RegionJob:
  
  def __init__ ( self ):

    self.Script = ''
    self.RootFile = ''
    self.Out = ''
    self.Err = ''
    self.Reg = ''

  def __str__ ( self ):

    res = ''
    for Element in dir( self ):
      if '__' in Element:
        pass
      else:
        res += '      -> self.%s: %s\n' % ( Element, str( getattr( self, Element ) ) )
    return res
