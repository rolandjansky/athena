# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, shutil, sys, tarfile
from optparse import OptionParser
import ROOT
import Tools
import Plot
import CreateWebPage

def main():

  ##### Parsing options #####
  parser = OptionParser()
  parser.add_option( '-i', '--input',  dest = 'input',  action = 'store', type = 'string', default = '', help = '' )
  ( options, args ) = parser.parse_args()

  Run( '.', options.input )

def Run( theJob, Perm = 'Ask' ):

  ##### Global Settings #####
  ##### Must be made more general!!! #####
  BasicDir = '/afs/cern.ch/user/g/gartoni/www/Work/Muons/Calibration/' 
  TemplatesDir = os.path.expandvars( '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/python/Code/Templates/' )
  ROOT.gSystem.Load( 'libRooFit' )
  ROOT.RooMsgService.instance().setGlobalKillBelow( ROOT.RooFit.WARNING )
  ROOT.RooMsgService.instance().setSilentMode( True )
  ROOT.gErrorIgnoreLevel = 3000

  PlotsDone = []
  HtmlFiles = []

  initDir = theJob.InitDir.rstrip( '/' )
  theInput = theJob.Name
  Stat = theJob.Stat
  Detector = theJob.Detector

  ##### Dumping Invariant Mass, Rho Plots #####
  PlotsDone.append( Plot.Plot( '%s/%s/Corrections-%s.root' % ( initDir, theInput, theInput ), Detector, Stat, theJob.RunMode == '' ) )
  HtmlFiles.append( CreateWebPage.CreateWebPage( theInput, PlotsDone[ -1 ], '%sPlotViewerTemplate.php' % TemplatesDir, 'TemplateFits.php' ) )

  ##### Dumping Background Plots #####
  PlotsDone.append( Plot.BkgPlot( '%s/%s/Corrections-%s.root' % ( initDir, theInput, theInput ), Detector, Stat ) )
  HtmlFiles.append( CreateWebPage.CreateWebPage( theInput, PlotsDone[ -1 ], '%sPlotViewerTemplate.php' % TemplatesDir, 'BackgroundFits.php' ) )
    
  if theJob.DoMonitoring:
    ##### Dumping Distribution Plots #####
    PlotsDone.append( Plot.PlotDistr( '%s/%s/Corrections-%s.root' % ( initDir, theInput, theInput ), Detector, Stat ) )
    HtmlFiles.append( CreateWebPage.CreateWebPage( theInput, PlotsDone[ -1 ], '%sPlotViewerTemplate.php' % TemplatesDir, 'Distributions.php' ) )
  
  if theJob.RunMode != '':
    
    ##### Dumping Scan Plots #####
    PlotsDone.append( Plot.ScanPlot( '%s/%s/Corrections-%s.root' % ( initDir, theInput, theInput ), Detector, Stat ) )
    HtmlFiles.append( CreateWebPage.CreateWebPage( theInput, PlotsDone[ -1 ], '%sPlotViewerTemplate.php' % TemplatesDir, 'Scans.php' ) )
    
    ##### Dumping Overall Plots #####
    if not 'SingleRegion.txt' in theJob.Regions:
      PlotsDone.append( Plot.OverallPlot( '%s/%s/Corrections-%s.root' % ( initDir, theInput, theInput ), Detector, Stat ) )
      HtmlFiles.append( CreateWebPage.CreateWebPage( theInput, PlotsDone[ -1 ], '%sPlotViewerTemplate.php' % TemplatesDir, 'Overview.php' ) )

  ##### Web Page for Browsing #####
  HtmlFiles.append( CreateWebPage.MakeCopy( '%sindex.php' % TemplatesDir ) )

  ##### Web Page for Job Configurations #####
  HtmlFiles.append( CreateWebPage.CreateJobInfoPage( '%sConfiguration.php' % TemplatesDir, theJob ) )

  ##### Create Results Directory And Put It On AFS
  Links = []
  CurrentDirectory = os.getcwd()
  Tools.CreateAndMoveIn( BasicDir + 'Results_%s' % theInput, Perm )
  for HtmlFile in HtmlFiles:
    if HtmlFile:
      shutil.move( HtmlFile, './' + os.path.basename( HtmlFile ) )
    Links.append( 'https://gartoni.web.cern.ch/gartoni/Work/Muons/Calibration/Results_%s/%s' % ( theInput, os.path.basename( HtmlFile ) ) )
  Tools.CreateAndMoveIn( 'plots' )
  CreateWebPage.MakeCopy( '%sindex.php' % TemplatesDir )
  for pd in PlotsDone:
    for plot in pd:
      #print plot
      if os.path.isfile( plot ):
        shutil.move( plot, './' + os.path.basename( plot ) )
  Tools.CreateAndMoveIn( 'eps' )
  CreateWebPage.MakeCopy( '%sindex.php' % TemplatesDir )
  for pd in PlotsDone:
    for plot in pd:
      eps_plot = plot.replace( '.png', '.eps' )
      if os.path.isfile( eps_plot ):
        shutil.move( eps_plot, './' + os.path.basename( eps_plot ) )

  Links = [ Link.replace( 'index.php', '' ) for Link in Links ]
  for Link in Links:
    if not '.php' in Link:
      theJob.LinkTo = '<a href="%s">%s</a>' % ( Link, theInput ) 
  os.chdir( CurrentDirectory )

  return sorted( list( set( Links ) ) )

if __name__ == '__main__':
  sys.exit( main() )
