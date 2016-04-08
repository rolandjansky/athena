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
  TemplatesDir = '/home/gartoni/Work/MuonCalibrationFit/python/Templates/'
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
  PlotsDone.append( Plot.Plot( '%s/%s/Corrections-%s.root' % ( initDir, theInput, theInput ), Detector, Stat ) )
  HtmlFiles.append( CreateWebPage.CreateWebPage( PlotsDone[ -1 ], '%sPlotViewerTemplate.php' % TemplatesDir, 'TemplateFits.php' ) )
 
  ##### Dumping Distribution Plots #####
  PlotsDone.append( Plot.PlotDistr( '%s/%s/Corrections-%s.root' % ( initDir, theInput, theInput ), Detector, Stat ) )
  HtmlFiles.append( CreateWebPage.CreateWebPage( PlotsDone[ -1 ], '%sPlotViewerTemplate.php' % TemplatesDir, 'Distributions.php' ) )
 
  ##### Dumping Background Plots #####
  PlotsDone.append( Plot.BkgPlot( '%s/%s/Corrections-%s.root' % ( initDir, theInput, theInput ), Detector, Stat ) )
  HtmlFiles.append( CreateWebPage.CreateWebPage( PlotsDone[ -1 ], '%sPlotViewerTemplate.php' % TemplatesDir, 'BackgroundFits.php' ) )
 
  ##### Dumping Scan Plots #####
  PlotsDone.append( Plot.ScanPlot( '%s/%s/Corrections-%s.root' % ( initDir, theInput, theInput ), Detector, Stat ) )
  HtmlFiles.append( CreateWebPage.CreateWebPage( PlotsDone[ -1 ], '%sPlotViewerTemplate.php' % TemplatesDir, 'Scans.php' ) )
 
  ##### Dumping Overall Plots #####
  PlotsDone.append( Plot.OverallPlot( '%s/%s/Corrections-%s.root' % ( initDir, theInput, theInput ), Detector, Stat ) )
  HtmlFiles.append( CreateWebPage.CreateWebPage( PlotsDone[ -1 ], '%sPlotViewerTemplate.php' % TemplatesDir, 'Overview.php' ) )

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
      shutil.move( HtmlFile, '.' )
    Links.append( 'https://gartoni.web.cern.ch/gartoni/Work/Muons/Calibration/Results_%s/%s' % ( theInput, os.path.basename( HtmlFile ) ) )
  Tools.CreateAndMoveIn( 'plots' )
  for pd in PlotsDone:
    for plot in pd:
      shutil.move( plot, '.' )

  Links = [ Link.replace( 'index.php', '' ) for Link in Links ]
  for Link in Links:
    if not '.php' in Link:
      theJob.LinkTo = '<a href="%s">%s</a>' % ( Link, theInput ) 
  os.chdir( CurrentDirectory )

  return sorted( list( set( Links ) ) )

if __name__ == '__main__':
  sys.exit( main() )
