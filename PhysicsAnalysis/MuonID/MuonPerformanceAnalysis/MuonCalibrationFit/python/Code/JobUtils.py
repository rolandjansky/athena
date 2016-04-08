# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import copy, multiprocessing, os, pickle, re, subprocess, sys, time
from multiprocessing.pool import ThreadPool

import Tools

# --- # --- # --- # --- # 

class Job:

  def __init__ ( self, name ):

    # ---> Job Info: General
    self.InitDir = os.path.abspath( '.' )
    self.BatchMode = ( os.fstat( 0 ) != os.fstat( 1 ) ) 
    self.Name = name
    self.Ancestors = []
    self.LinkTo = ''
    self.LinkToID = ''
    self.Host = os.path.expandvars( '$HOSTNAME' ) 
    self.Stat = 1
    self.Recipients = 'giacomo.artoni@cern.ch' #,kevin.michael.loew@cern.ch'
    # ---> Job Info: Inputs
    self.Input = ''
    self.InputRegions = ''
    self.InputForID = ''
    self.InputRegionsForID = ''
    self.Template = os.path.expandvars( '$TestArea/MuonCalibrationFit/templates/TemplateJob.py' )
    self.Files = Files( '$TestArea/MuonCalibrationFit/data', 'Data.root', 'MonteCarlo.root', 'Background.root' ) 
    # ---> Job Info: Outputs
    self.Output = 'Corrections-%s.root' % self.Name
    self.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter1Regions.txt' )
    self.OutputRegions = '' 
    self.Svn = self.GetSvnRevision()
    self.Start = None
    self.End = None
    self.Links = []
    # ---> Fit Info
    self.FitType = 'Chi2'
    self.Detector = 'ID'
    self.Level = 'INFO'
    self.BkgParametrization = 'Exponential'
    self.Splittings = 1
    self.NumRandomValues = 1
    self.RunMode = 'Simplex:1'
    self.MaxRetrials = 1
    self.ProbThreshold = 0.1
    self.JpsiMassBins = 100
    self.JpsiMassPts = ''
    self.ZMassBins = 100
    self.ZMassPts = ''
    self.Fit_s0 = True
    self.Fit_s1 = True
    self.Fit_p0 = True
    self.Fit_p1 = True
    self.Fit_p2 = True
    # ---> Resources Info
    self.Cores = 8

  def StartFrom ( self, JobName ):
    
    with open( '%s/AutoSave-%s.pkl' % ( JobName, JobName ), 'rb' ) as Input:
      PreviousJob = pickle.load( Input )
      if PreviousJob.Detector == 'ID' and self.Detector == 'MS':
        self.InputForID = PreviousJob.Output
        self.InputRegionsForID = PreviousJob.OutputRegions
        self.LinkToID = PreviousJob.LinkTo
      else:
        self.CopyInfoFrom( PreviousJob )
        # ---> Careful when copying these members!
        self.Input = PreviousJob.Output
        self.InputRegions = PreviousJob.OutputRegions
        self.Output = 'Corrections-%s.root' % self.Name
        self.Ancestors.append( PreviousJob.LinkTo )
        self.Links = []
        self.LinkTo = ''

  def CopyInfoFrom ( self, theJob ):
    
    for Element in dir( self ):
      if '__' in Element or callable( getattr( self, Element ) ) or Element == 'Name' or not hasattr( theJob, Element ):
        pass
      else:
        if isinstance( getattr( theJob, Element ), list ):
          setattr( self, Element, copy.deepcopy( getattr( theJob, Element ) ) )
        setattr( self, Element, getattr( theJob, Element ) )

  def Run ( self ):

    Tools.CreateAndMoveIn( self.Name, 'Force' ) 
    #self.Commit()
    self.Execute()
    self.Save()
    self.PublishPlots()
    self.SendMail()
    self.Save()

  def __str__ ( self ):

    res = ''
    for Element in dir( self ):
      if '__' in Element or callable( getattr( self, Element ) ):
        pass
      else:
        res += '<h4>%s: %s</h4>' % ( Element, str( getattr( self, Element ) ) )
    return res

  def GetSvnRevision ( self ):

    return subprocess.check_output( "svn info svn+ssh://svn.cern.ch/reps/gartonisvn/Muons/Calibration/MuonCalibrationFit | grep Revision | awk '{print $2}'", shell = True ).rstrip()

  def Commit ( self ):

    subprocess.call( 'svn ci /home/gartoni/Work/MuonCalibrationFit -m "before sending job named: %s"' % self.Name, shell = True )
    self.Svn = self.GetSvnRevision() 

  def Execute ( self ):

    self.Start = time.time() 
    Pool = ThreadPool( self.Cores )
    TempRegions = self.SplitRegions()
    SubJobs = []
    for ( Region, File ) in TempRegions:
      os.chdir( Region )
      this_SubJob = self.CreateRegionJob( Region, File )
      SubJobs.append( ( Pool.apply_async( Tools.SendCommand, ( 'athena.py %s' % this_SubJob.Script, ) ), this_SubJob ) )
      os.chdir( '..' )
    UnfinishedSubJobs = [ True ]
    while any( UnfinishedSubJobs ):
      time.sleep( 100 )
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
    self.Output = Tools.MergeRootFiles( self.Output, [ SubJob.RootFile for ( Res, SubJob ) in SubJobs ] )
    self.OutputRegions = Tools.MergeTxtFiles( 'Regions-%s.txt' % self.Name, [ SubJob.Reg for ( Res, SubJob ) in SubJobs ] )
    os.chdir( '..' )
    self.End = time.time()

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
    MailContent += 'SVN Info: Revision n.%s (<a href="https://svnweb.cern.ch/trac/gartonisvn/browser/Muons/Calibration/MuonCalibrationFit?rev=%s">Link to code</a>)<br><br>' % ( self.Svn, self.Svn )
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
    subprocess.call( 'echo "%s" | sendmail %s' % ( MailContent, self.Recipients ), shell = True )

  def Save ( self ):
    with open( '%s/AutoSave-%s.pkl' % ( self.Name, self.Name ), 'wb' ) as Output:
      pickle.dump( self, Output, pickle.HIGHEST_PROTOCOL )

  def CreateRegionJob ( self, Region, RegionFile ):

    Res = RegionJob()
    Res.Script = os.path.abspath( 'JobOptions_%s.py' % Region )
    Res.RootFile = os.path.abspath( 'Output%s.root' % Region )
    Res.Out = os.path.abspath( 'Output%s.out' % Region )
    Res.Err = os.path.abspath( 'Output%s.err' % Region )
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
      line = line.replace( '_MAXRETRIALS_', str( self.MaxRetrials ) )
      line = line.replace( '_INPUT_', self.Input )
      line = line.replace( '_INPUTREGION_', self.InputRegions )
      line = line.replace( '_INPUTFORID_', self.InputForID )
      line = line.replace( '_INPUTREGIONFORID_', self.InputRegionsForID )
      line = line.replace( '_OUTPUT_', Res.RootFile )
      line = line.replace( '_OUTPUTREGION_', RegionFile )
      line = line.replace( '_DATA_', self.Files.Data )
      line = line.replace( '_MONTECARLO_', self.Files.MonteCarlo )
      line = line.replace( '_BACKGROUND_', self.Files.Background )
      line = line.replace( '_JPSI_MASS_BINS_', str( self.JpsiMassBins ) )
      line = line.replace( '_JPSI_MASS_PTS_', self.JpsiMassPts )
      line = line.replace( '_Z_MASS_BINS_', str( self.ZMassBins ) )
      line = line.replace( '_Z_MASS_PTS_', self.ZMassPts )
      line = line.replace( '_USE_S0_', str( self.Fit_s0 ) )
      line = line.replace( '_USE_S1_', str( self.Fit_s1 ) )
      line = line.replace( '_USE_P0_', str( self.Fit_p0 ) )
      line = line.replace( '_USE_P1_', str( self.Fit_p1 ) )
      line = line.replace( '_USE_P2_', str( self.Fit_p2 ) )
      jobFile.write( line )
    jobFile.close()
    os.system( 'chmod +x %s' % jobFileName )
    return Res

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
    return returnList

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
