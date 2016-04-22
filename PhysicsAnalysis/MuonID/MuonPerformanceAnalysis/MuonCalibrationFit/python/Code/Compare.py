# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import math, os, re, pickle, subprocess, sys
import ROOT
import FitResult, PlottingTools, Tools

class Compare:

  # --- # --- # --- # --- #
  def __init__ ( self, List ):
    self.List = List
    self.JobList = {}
    for jobname, job in self.List:
      self.JobList[ jobname ] = GetJobInfo( job )
    self.IDRanges = { 's1': ( -0.004, 0.010 ), 'p1': ( 0., 0.035 ), 'p2': ( 0., 0.0007 ) }
    self.MSRanges = { 's0': ( -0.13, 0.08 ), 's1': ( -0.008, 0.006 ), 'p0': ( -0.005, 0.08 ), 'p1': ( 0, 0.050 ), 'p2': ( 0., 0.0005 ) }

  # --- # --- # --- # --- #
  def PrintPlots ( self ):
    #for Var in [ 's1', 'p1', 'p2' ]:
    for Var in [ 's0', 's1', 'p0', 'p1', 'p2' ]:
      for Type in [ 'Small', 'Large' ]:
        PlotList = [ ( Key, self.GetGraphs( self.JobList[ Key ], Var, True, Type ) ) for Key in self.JobList ]
        self.Plot( PlotList, Var, Type )

  def Plot ( self, Graphs, Var, Type = '' ):
    Min = 10000000 
    Max = -1000000 
    for key, graph in Graphs:
      Min = min( Min, GetMin( graph ) )
      Max = max( Max, GetMax( graph ) )
    sfMin = 0.8
    if Min < 0:
      sfMin = 1.2
    sfMax = 1.2
    if Max < 0:
      sfMax = 0.8
    Graphs[ 0 ][ 1 ].SetMinimum( Min * sfMin )
    Graphs[ 0 ][ 1 ].SetMaximum( Max * sfMax )
    if Type == '':
      Graphs[ 0 ][ 1 ].SetMinimum( self.IDRanges[ Var ][ 0 ] )
      Graphs[ 0 ][ 1 ].SetMaximum( self.IDRanges[ Var ][ 1 ] )
    else:
      Graphs[ 0 ][ 1 ].SetMinimum( self.MSRanges[ Var ][ 0 ] )
      Graphs[ 0 ][ 1 ].SetMaximum( self.MSRanges[ Var ][ 1 ] )
    #Colors = [ ROOT.kRed + 1, ROOT.kAzure + 2, ROOT.kOrange + 7, ROOT.kSpring + 5, ROOT.kViolet, ROOT.kPink - 5, ROOT.kGray, ROOT.kBlack ]
    Colors = [ ROOT.kPink - 5, ROOT.kAzure + 3, ROOT.kOrange + 7, ROOT.kSpring + 5, ROOT.kViolet, ROOT.kRed + 1, ROOT.kGray, ROOT.kBlack ]
    Can = ROOT.TCanvas( 'Plot_%s%s' % ( Var, Type ), '', 800, 800 ) 
    Graphs[ 0 ][ 1 ].Draw( 'ap' )
    #Graphs[ 0 ].GetXaxis().SetLimits( self.BinObjects[ Key ].Mins[ 0 ], self.BinObjects[ Key ].Maxs[ -1 ] )
    Graphs[ 0 ][ 1 ].GetXaxis().SetTitle( '#eta' )
    String = 'ID'
    if Type != '':
      String = 'MS'
    Graphs[ 0 ][ 1 ].GetYaxis().SetTitle( PlottingTools.Convert( Var, String ) )
    Graphs[ 0 ][ 1 ].Draw( 'p' )
    Leg = ROOT.TLegend( 0.7, 0.92, 0.9, 0.8 )
    Leg.SetFillStyle( 0 )
    Leg.SetBorderSize( 0 )
    Leg.SetTextFont( 42 )
    Leg.SetTextSize( 0.025 )
    for Index, ( key, graph ) in enumerate( Graphs ):
      graph.SetLineWidth( 2 )
      graph.SetLineColor( Colors[ Index ] )
      graph.SetMarkerColor( Colors[ Index ] )
      graph.SetMarkerStyle( 20 )
      graph.SetLineStyle( 1 )
      graph.Draw( 'p' )
      Leg.AddEntry( graph, key, 'pe' )
    Leg.Draw()
    Lat = ROOT.TLatex()
    Lat.SetNDC()
    Lat.SetTextFont( 42 )
    Lat.SetTextSize( 0.035 )
    Lat.SetTextColor( 1 )
    Lat.SetTextAlign( 12 )
    Lat.DrawLatex( 0.2, 0.9, '#bf{#it{ATLAS}} Internal' ) 
    Lat.SetTextSize( 0.03 )
    Lat.DrawLatex( 0.2, 0.85, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % ( 2.7 ) ) 
    Lat.SetTextSize( 0.025 )
    Can.SaveAs( '.pdf' )

  # --- # --- # --- # --- #
  def GetGraphs ( self, job, var, with_errors = True, Type = '' ):
    InputFile = ROOT.TFile( job.Output )  
    Keys = ROOT.gDirectory.GetListOfKeys() 
    ToBeUsed = []
    Reg = re.compile( 'FinalResults___%s___(\w+)' % job.Name )
    for Key in Keys:
      TheName = Key.GetName()
      Match = Reg.match( TheName )
      if 'TVectorT<double>' in Key.GetClassName() and Match:
        if Type == '' or Type in Match.group( 1 ):
          FitRes = FitResult.FitResult( TheName, InputFile )
          ToBeUsed.append( FitRes )
    iter_o = FitResult.IterOverview( job.Name, job.OutputRegions )
    return iter_o.GetGraph( iter_o.BinObjects[ 'eta' ], var, ToBeUsed, with_errors ) 

def GetMin( graph ):
  myMin = 1000000000
  for Bin in range( graph.GetN() ):
    X, Y = ROOT.Double( 0. ), ROOT.Double( 0. )
    graph.GetPoint( Bin, X, Y ) 
    Err = graph.GetErrorYlow( Bin )
    myMin = min( [Y - Err, myMin ] )
  return myMin

def GetMax( graph ):
  myMax = -100000000000000
  for Bin in range( graph.GetN() ):
    X, Y = ROOT.Double( 0. ), ROOT.Double( 0. )
    graph.GetPoint( Bin, X, Y ) 
    Err = graph.GetErrorYhigh( Bin )
    myMax = max( [Y + Err, myMax ] )
  return myMax

def GetJobInfo ( job_name ):
  tempJob = None
  if job_name != '':
    with open( '%s/AutoSave-%s.pkl' % ( job_name, job_name ), 'rb' ) as IN:
      tempJob = pickle.load( IN )
  return tempJob

