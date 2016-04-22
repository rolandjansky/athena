# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import array, math, os, re
import ROOT
import Tools
import PlottingTools
import AtlasStyle

def GetFunctionIndex( function_name ):
  if function_name == 'Chi2':
    return 15
  if function_name == 'Likelihood':
    return 16

class FitResult:
  
  def __init__ ( self, name, the_file ):

    the_vector = the_file.Get( name )

    self.Val_s0   = the_vector[ 0  ]
    self.eUp_s0   = the_vector[ 1  ]
    self.eLo_s0   = the_vector[ 2  ]
    self.Val_s1   = the_vector[ 3  ]
    self.eUp_s1   = the_vector[ 4  ]
    self.eLo_s1   = the_vector[ 5  ]
    self.Val_p0   = the_vector[ 6  ]
    self.eUp_p0   = the_vector[ 7  ]
    self.eLo_p0   = the_vector[ 8  ]
    self.Val_p1   = the_vector[ 9  ]
    self.eUp_p1   = the_vector[ 10 ]
    self.eLo_p1   = the_vector[ 11 ]
    self.Val_p2   = the_vector[ 12 ]
    self.eUp_p2   = the_vector[ 13 ]
    self.eLo_p2   = the_vector[ 14 ]
    self.Val_chi2 = the_vector[ 15 ]
    self.eUp_chi2 = 0 
    self.eLo_chi2 = 0 
    self.Val_nll  = the_vector[ 16 ]
    self.eUp_nll  = 0 
    self.eLo_nll  = 0 
    self.NdoF     = the_vector[ 17 ]
    self.Name = name
    self.Dir = os.path.dirname( os.path.abspath( the_file.GetName() ) ).rstrip( '/' ) 
     
    r = re.compile( '(\w+)___(\w+)___(\w+)' )
    m = r.match( self.Name )
    self.Type = m.group( 1 )
    self.Iter = m.group( 2 )
    self.Reg = m.group( 3 )
    self.RegFile = '' 

    self.Header = ''
    self.Definition = ''
    self.FindRegionDefinition()
 
  def __str__( self ):

    Res = '|  %35s  |' % self.Name
    for var in [ 's0', 's1', 'p0', 'p1', 'p2' ]:
      Res += '  s0: %+.5f +%.5f -%.5f  |' % ( getattr( self, 'Val_%s' % var ), getattr( self, 'eUp_%s' % var ), getattr( self, 'eLo_%s' % var ) )
    return Res 

  def FindRegionDefinition ( self ):

    for File in os.listdir( self.Dir ): 
      if File.endswith( '.txt' ) and not 'eventLoopHeartBeat' in File and 'Regions' in File:
        with open( '%s/%s' % ( self.Dir, File ) ) as F:
          content = F.readlines()
          for line in content:
            if 'Var:' in line:
              self.Header = line.rstrip( '\n' )
            if '%s___%s' % ( self.Iter, self.Reg ) in line:
              self.Definition = line.rstrip( '\n' )
        self.RegFile = '%s/%s' % ( self.Dir, File ) 

class IterOverview:

  def __init__ ( self, itername, thefile ):

    self.IterName = itername
    self.File = thefile
    self.Lines = open( self.File ).readlines()
    self.Lines = Tools.Strip( '\n', self.Lines )
    tempLines = []
    for Index, Line in enumerate( self.Lines ):
      if Index == 0 and 'Var:' in Line: 
        self.Header = Line
      else:
        tempLines.append( Line )
    self.Lines = tempLines
  
    self.Dict = {}
    self.Vars = {}
    if self.InterpretHeader():
      self.InterpretLines()

    self.BinObjects = {}
    for Key in self.Vars:
      Var = self.Vars[ Key ]
      self.BinObjects[ Var ] = BinObject()
      for DictKey in self.Dict:
        self.BinObjects[ Var ].Add( self.Dict[ DictKey ][ Var ] )

    for Key in self.BinObjects:
      self.BinObjects[ Key ].Check()
      
  def DoPlot ( self, AllFitRes, Detector, Stat ):
    AllDone = []
    Colors = [ ROOT.kAzure + 2, ROOT.kRed + 1 ]
    Markers = [ 21, 25 ] 
    for Key in self.BinObjects:
      if 'eta' in Key:
        TheList = [ 's1', 'p1', 'p2', 'chi2' ]
        if Detector == 'MS':
          TheList = [ 's0', 's1', 'p0', 'p1', 'p2', 'chi2' ]
        for var in TheList: 
          if Detector == 'MS':
            theGraphs = self.GetGraphs( self.BinObjects[ Key ], var, AllFitRes, True, [ ( 0, 'Small' ), ( 1, 'Large' ) ] )
          else:
            theGraph = self.GetGraph( self.BinObjects[ Key ], var, AllFitRes, True )
          varTitle = PlottingTools.Convert( var, Detector )

          plotOpt = 'pa'
          if var == 'chi2':
            if Detector == 'MS':
              theGraphs[ 0 ].SetMinimum( 0 )
            else:
              theGraph.SetMinimum( 0 )
          Can = ROOT.TCanvas( '%s___%s___OverviewPlot' % ( self.IterName, var ), '', 800, 800 ) 
          if Detector == 'MS':
            for index, graph in enumerate( theGraphs ):
              graph.SetLineWidth( 2 )
              graph.SetLineColor( Colors[ index ] )
              graph.SetMarkerColor( Colors[ index ] )
              graph.SetMarkerStyle( Markers[ index ] )
              graph.SetLineStyle( 1 )
              if index == 0:
                graph.Draw( plotOpt )
                graph.GetXaxis().SetLimits( self.BinObjects[ Key ].Mins[ 0 ], self.BinObjects[ Key ].Maxs[ -1 ] )
                graph.GetXaxis().SetTitle( Tools.GetAxisQuality( Key ) )
                graph.GetYaxis().SetTitle( varTitle ) 
                graph.Draw( plotOpt )
              else:
                graph.Draw( plotOpt.replace( 'a', 'same' ) )
            Leg = ROOT.TLegend( 0.7, 0.9, 0.85, 0.8 )
            Leg.SetFillStyle( 0 )
            Leg.SetBorderSize( 0 )
            Leg.SetTextFont( 42 )
            Leg.SetTextSize( 0.03 )
            Leg.AddEntry( theGraphs[ 0 ], 'Small Sectors', 'pe' )
            Leg.AddEntry( theGraphs[ 1 ], 'Large Sectors', 'pe' )
            Leg.Draw()
          else:
            theGraph.SetLineWidth( 2 )
            theGraph.SetLineColor( ROOT.kBlack )
            theGraph.SetMarkerColor( ROOT.kBlack )
            theGraph.SetMarkerStyle( 20 )
            theGraph.SetLineStyle( 1 )
            theGraph.Draw( plotOpt )
            theGraph.GetXaxis().SetLimits( self.BinObjects[ Key ].Mins[ 0 ], self.BinObjects[ Key ].Maxs[ -1 ] )
            theGraph.GetXaxis().SetTitle( Tools.GetAxisQuality( Key ) )
            theGraph.GetYaxis().SetTitle( varTitle ) 
            theGraph.Draw( plotOpt )
          Lat = ROOT.TLatex()
          Lat.SetNDC()
          Lat.SetTextFont( 42 )
          Lat.SetTextSize( 0.035 )
          Lat.SetTextColor( 1 )
          Lat.SetTextAlign( 12 )
          Lat.DrawLatex( 0.2, 0.9, '#bf{#it{ATLAS}} Internal' ) 
          Lat.SetTextSize( 0.03 )
          Lat.DrawLatex( 0.2, 0.85, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % Stat ) 
          Lat.SetTextSize( 0.025 )
          Can.SaveAs( '.png' )
          Can.SaveAs( '.eps' )
          
          Can2 = ROOT.TCanvas( '%s___%s___NoErrorsPlot' % ( self.IterName, var ), '', 800, 800 ) 
          if Detector == 'MS':
            theNoErrGraphs = self.GetGraphs( self.BinObjects[ Key ], var, AllFitRes, False, [ ( 0, 'Small' ), ( 1, 'Large' ) ] )
            for index, graph in enumerate( theNoErrGraphs ):
              graph.SetLineWidth( 2 )
              graph.SetLineColor( Colors[ index ] )
              graph.SetMarkerColor( Colors[ index ] )
              graph.SetMarkerStyle( Markers[ index ] )
              graph.SetLineStyle( 1 )
              if index == 0:
                graph.Draw( plotOpt )
                graph.GetXaxis().SetLimits( self.BinObjects[ Key ].Mins[ 0 ], self.BinObjects[ Key ].Maxs[ -1 ] )
                graph.GetXaxis().SetTitle( Tools.GetAxisQuality( Key ) )
                graph.GetYaxis().SetTitle( varTitle ) 
                graph.Draw( plotOpt )
              else:
                graph.Draw( plotOpt.replace( 'a', 'same' ) )
            Leg = ROOT.TLegend( 0.7, 0.9, 0.85, 0.75 )
            Leg.SetFillStyle( 0 )
            Leg.SetBorderSize( 0 )
            Leg.SetTextFont( 42 )
            Leg.SetTextSize( 0.04 )
            Leg.AddEntry( theNoErrGraphs[ 0 ], 'Small Sectors', 'pe' )
            Leg.AddEntry( theNoErrGraphs[ 1 ], 'Large Sectors', 'pe' )
          else:
            theNoErrGraph = self.GetGraph( self.BinObjects[ Key ], var, AllFitRes, False )
            theNoErrGraph.GetXaxis().SetLimits( self.BinObjects[ Key ].Mins[ 0 ], self.BinObjects[ Key ].Maxs[ -1 ] )
            theNoErrGraph.GetXaxis().SetTitle( Tools.GetAxisQuality( Key ) )
            theNoErrGraph.GetYaxis().SetTitle( varTitle ) 
            theNoErrGraph.Draw( 'pa' )
          Lat = ROOT.TLatex()
          Lat.SetNDC()
          Lat.SetTextFont( 42 )
          Lat.SetTextSize( 0.035 )
          Lat.SetTextColor( 1 )
          Lat.SetTextAlign( 12 )
          Lat.DrawLatex( 0.2, 0.9, '#bf{#it{ATLAS}} Internal' ) 
          Lat.SetTextSize( 0.03 )
          Lat.DrawLatex( 0.2, 0.85, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % Stat ) 
          Lat.SetTextSize( 0.025 )
          Can2.SaveAs( '.png' )
          Can2.SaveAs( '.eps' )

          AllDone.append( ( Can.GetName() + '.png', Can2.GetName() + '.png' ) )

    return AllDone      

  def GetGraph ( self, bo, var, allfitres, do_err_y ):
    x, y = array.array( 'd' ), array.array( 'd' )
    xerr, yerr_up, yerr_low = array.array( 'd' ), array.array( 'd' ), array.array( 'd' )
    for Index, Value in enumerate( bo.BinExtremes ):
      if Index == len( bo.BinExtremes ) - 1:
        continue
      x.append( ( bo.BinExtremes[ Index + 1 ] + bo.BinExtremes[ Index ] ) * 0.5 )
      xerr.append( math.fabs( bo.BinExtremes[ Index + 1 ] - bo.BinExtremes[ Index ] ) * 0.5 )
      for fr in allfitres:
        if fr.Type == 'FinalResults':
          if RmLS( '%s___%s' % ( fr.Iter, fr.Reg ) ) == RmLS( bo.Names[ Index ] ):
            theVal = getattr( fr, 'Val_%s' % var )
            if var == 'chi2':
              theVal = theVal / float( fr.NdoF )
            y.append( theVal )
            if do_err_y:
              up_err = getattr( fr, 'eUp_%s' % var )
              if up_err > 0.9:
                up_err = 0
              yerr_up.append( up_err )
              # Careful when handling these, for p0, p1 and p2 
              low_err = getattr( fr, 'eLo_%s' % var )
              if var == 'p0' or var == 'p1' or var == 'p2':
                if theVal - low_err < 0:
                  low_err = theVal
              yerr_low.append( low_err )
            else:
              yerr_up.append( 0 ) 
              yerr_low.append( 0 )
    return ROOT.TGraphAsymmErrors( len( bo.BinExtremes ) - 1, x, y, xerr, xerr, yerr_low, yerr_up )

  def GetGraphs ( self, bo, var, allfitres, do_err_y, the_list ):
    x = array.array( 'd' )
    xerr = array.array( 'd' )
    y = {}
    yerr_up = {}
    yerr_low = {}
    for ( index, name ) in the_list:
      y[ name ] = array.array( 'd' )
      yerr_up[ name ] = array.array( 'd' )
      yerr_low[ name ] = array.array( 'd' )
    Graphs = []

    for ( index, name ) in the_list:
      for Index, Value in enumerate( bo.BinExtremes ):
        if Index == len( bo.BinExtremes ) - 1:
          continue
        x.append( ( bo.BinExtremes[ Index + 1 ] + bo.BinExtremes[ Index ] ) * 0.5 )
        xerr.append( math.fabs( bo.BinExtremes[ Index + 1 ] - bo.BinExtremes[ Index ] ) * 0.5 )
        for fr in allfitres:
          if fr.Type == 'FinalResults':
            if '%s___%s' % ( fr.Iter, fr.Reg ) == bo.Names[ Index ].replace( 'Small', name ):
              theVal = getattr( fr, 'Val_%s' % var )
              if var == 'chi2':
                theVal = theVal / float( fr.NdoF )
              y[ name ].append( theVal )
              if do_err_y:
                yerr_up[ name ].append( getattr( fr, 'eUp_%s' % var ) )
                # Careful when handling these, for p0, p1 and p2 
                low_err = getattr( fr, 'eLo_%s' % var )
                if var == 'p0' or var == 'p1' or var == 'p2':
                  if theVal - low_err < 0:
                    low_err = theVal
                yerr_low[ name ].append( low_err )
              else:
                yerr_up[ name ].append( 0 ) 
                yerr_low[ name ].append( 0 )
      Graphs.append( ROOT.TGraphAsymmErrors( len( bo.BinExtremes ) - 1, x, y[ name ], xerr, xerr, yerr_low[ name ], yerr_up[ name ] ) )
    return Graphs 
      
  def __str__ ( self ):

    return '%s - %s' % ( self.IterName, self.File )

  def InterpretHeader( self ):
    Split = self.Header.split()
    if Split.pop() != 'Name':
      print 'ooops'
      return False
    r = re.compile( 'Var:(\w+)\[\w{3}\]' )
    for Index, Token in enumerate( Split ):
      m = r.search( Token )
      NewIndex = int( math.floor( Index / 2 ) )
      self.Vars[ NewIndex ] = m.group( 1 )
    return True

  def InterpretLines( self ):
    for Line in self.Lines:
      Split = Line.split()
      tempName = Split.pop()
      self.Dict[ tempName ] = {}
      if len( Split ) % 2 != 0:
        print 'Error ---> ', Split
        return False
      Grouped = [ Split[ k:k+2 ] for k in range( 0, len( Split ), 2 ) ]
      for Index, Group in enumerate( Grouped ):
        self.Dict[ tempName ][ self.Vars[ Index ] ] = RegVar( self.Vars[ Index ], float( Group[ 0 ] ), float( Group[ 1 ] ), tempName )

class RegVar:

  def __init__ ( self, name, min, max, regname ):

    self.Name = name
    self.Min = min
    self.Max = max
    self.RegName = regname

  def __str__ ( self ):
    return ' :::  %20s  :::  %+.5f  :::  %+.5f  ::: ' % ( self.Name, self.Min, self.Max )

class BinObject:

  def __init__ ( self ):

    self.Mins = []
    self.Maxs = []
    self.AllRegVars = []
    self.BinExtremes = []
    self.Names = []

  def Add ( self, r ):

    self.AllRegVars.append( r )
    self.Mins.append( r.Min )
    self.Maxs.append( r.Max )

  def Check ( self ):

    if len( set( self.Mins ) ) == 1 and len( set( self.Maxs ) ) == 1:
      self.Type = False
      return
      
    self.Mins = sorted( set( self.Mins ) )
    self.Maxs = sorted( set( self.Maxs ) )
    for Index, Value in enumerate( self.Mins ):
      if Index == 0:
        continue
      elif Index == ( len( self.Mins ) - 1 ):
        break
      elif self.Mins[ Index + 1 ] != self.Maxs[ Index ]:
        print 'Error'
      else:
        self.BinExtremes = self.Mins[:]
        self.BinExtremes.append( self.Maxs[ -1 ] )
    for Index, BinExt in enumerate( self.BinExtremes ):
      if Index == len( self.BinExtremes ) - 1:
        continue
      for theRegVar in self.AllRegVars:
        #print theRegVar.Min, self.BinExtremes[ Index ]
        #print theRegVar.Max, self.BinExtremes[ Index + 1 ]
        #print theRegVar.RegName
        #print
        if theRegVar.Min == self.BinExtremes[ Index ] and theRegVar.Max == self.BinExtremes[ Index + 1 ] and not 'Large' in theRegVar.RegName:
          self.Names.append( theRegVar.RegName )
    self.Type = True

  def __str__ ( self ):
    return 'Regions -> %s\nMins -> %s\nMaxs -> %s\nNames -> %s' % ( self.AllRegVars, self.Mins, self.Maxs, ', '.join( self.Names ) )

def GetCorrespondingTruth( var, det ):
  if det == 'ID':
    if var == 'chi2':
      return ROOT.TF1( 'chi2_func', '-1', -2.5, 2.5 ) 
    if var == 's0':
      return ROOT.TF1( 's0_func', '0.', -2.5, 2.5 ) 
    if var == 's1':
      #return ROOT.TF1( 's1_func', '-0.001', -2.5, 2.5 )
      return ROOT.TF1( 's1_func', '0.01 * TMath::Sin( 2 * x ) / ( x + 5 )', -2.5, 2.5 )
    if var == 'p0':
      return ROOT.TF1( 'p0_func', '0.', -2.5, 2.5 ) 
    if var == 'p1':
      #return ROOT.TF1( 'p1_func', '0.01', -2.5, 2.5 ) 
      return ROOT.TF1( 'p1_func', '0.002 + 0.01 * TMath::Exp( - TMath::Power( 0.5 * x, 2 ) )', -2.5, 2.5 ) 
    if var == 'p2':
      #return ROOT.TF1( 'p2_func', '0.0005', -2.5, 2.5 ) 
      return ROOT.TF1( 'p2_func', '0.00016 * ( 2 + TMath::Erf( x ) )', -2.5, 2.5 )

def RmLS ( name ):
  res = name.replace( 'Large', '' )
  res = res.replace( 'Small', '' )
  return res

