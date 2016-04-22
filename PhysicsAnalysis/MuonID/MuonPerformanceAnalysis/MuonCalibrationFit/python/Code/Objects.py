# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import array, ROOT

class PlotObject:
  
  def __init__( self, hist, fitlevel, region, region_expl, variable, datatype, ptbin, parameters, norm_histogram = None ):
    self.Hist = hist
    if norm_histogram and self.Hist.Integral() > 0:
      #print
      #print norm_histogram.Integral(), norm_histogram.Integral( 0, norm_histogram.GetNbinsX() + 1 )
      #print self.Hist.Integral(), self.Hist.Integral( 0, self.Hist.GetNbinsX() + 1 )
      #self.Hist.Scale( norm_histogram.Integral() / self.Hist.Integral( 0, self.Hist.GetNbinsX() + 1 ) )
      self.Hist.Scale( norm_histogram.Integral() / self.Hist.Integral() )
    self.FitLevel = fitlevel
    self.Region = region
    self.RegionExpl = region_expl
    self.Variable = variable
    self.DataType = datatype
    self.PtBin = ptbin
    self.s0      = parameters[ 0  ]
    self.s0errUp = parameters[ 1  ]
    self.s0errLo = parameters[ 2  ]
    self.s1      = parameters[ 3  ]
    self.s1errUp = parameters[ 4  ]
    self.s1errLo = parameters[ 5  ]
    self.p0      = parameters[ 6  ]
    self.p0errUp = parameters[ 7  ]
    self.p0errLo = parameters[ 8  ]
    self.p1      = parameters[ 9  ]
    self.p1errUp = parameters[ 10 ]
    self.p1errLo = parameters[ 11 ]
    self.p2      = parameters[ 12 ]
    self.p2errUp = parameters[ 13 ]
    self.p2errLo = parameters[ 14 ]
    self.nll     = parameters[ 15 ]
    self.chi2    = parameters[ 16 ]
    self.NdoF    = parameters[ 17 ]
    #=====#
    self.Hist.SetLineColor( ROOT.kBlack )
    self.Hist.SetLineStyle( 1 )
    self.Hist.SetLineWidth( 2 )
    if self.DataType == 'Data':
      pass
    else:
      if 'Jpsi' in self.Variable:
        if 'MC' in self.DataType:
          #self.Hist.SetFillColor( ROOT.TColor.GetColor( '#FFFFCC' ) ) 
          #self.Hist.SetFillColor( ROOT.TColor.GetColor( '#FFFFFF' ) ) 
          self.Hist.SetFillColor( ROOT.TColor.GetColor( '#ffffff' ) ) 
        elif 'Bkg' in self.DataType:
          #self.Hist.SetFillColor( ROOT.TColor.GetColor( '#FFAD33' ) )  #ROOT.TColor.GetColor( '#AFEEEE' ) ) #ROOT.TColor.GetColor( '#336699' ) ) 
          #self.Hist.SetFillColor( ROOT.TColor.GetColor( '#002147' ) )  #ROOT.TColor.GetColor( '#AFEEEE' ) ) #ROOT.TColor.GetColor( '#336699' ) ) 
          self.Hist.SetFillColor( ROOT.TColor.GetColor( '#b5d4e3' ) )   #ROOT.TColor.GetColor( '#69913B' ) )  #ROOT.TColor.GetColor( '#AFEEEE' ) ) #ROOT.TColor.GetColor( '#336699' ) ) 
      else:
        if 'MC' in self.DataType:
          #self.Hist.SetFillColor( ROOT.TColor.GetColor( '#FFE6FF' ) ) 
          self.Hist.SetFillColor( ROOT.TColor.GetColor( '#ffff94' ) ) #'#FF8566' ) ) #CEE6FF' ) ) 
        elif 'Bkg' in self.DataType:
          #self.Hist.SetFillColor( ROOT.TColor.GetColor( '#471947' ) ) 
          self.Hist.SetFillColor( ROOT.TColor.GetColor( '#eb8847' ) ) #'#B80000' ) ) #'#002147' ) ) 

  def __str__( self ):
    return '%s - %s - %s - %s - %s' % ( self.FitLevel, self.Region, self.Variable, self.PtBin, self.DataType )

class BkgObject:
  
  def __init__( self, ws, region, region_expl, variable, ptbin ):
    self.Ws = ws
    self.Region = region
    self.RegionExpl = region_expl
    self.Variable = variable
    self.PtBin = ptbin
    #=====#

  def __str__( self ):
    return '%s - %s - %s' % ( self.Region, self.Variable, self.PtBin )

class ScanObject:
  
  def __init__( self, fitres, tree, region, region_expl ):
    self.Tree = tree
    self.Region = region
    self.RegionExpl = region_expl
    self.Vars = { 's0': 0, 's1': 1, 'p0': 2, 'p1': 3, 'p2': 4 }
    #=====#
    self.Min = {}
    self.Max = {}
    self.Values = {}
    self.BestFitValue = {}
    self.ErrorUp = {}
    self.ErrorLow = {}
    self.ErrVal = 0
    self.TitleY = ''
    for Key in self.Vars:
      self.BestFitValue[ Key ] = fitres[ self.Vars[ Key ] * 3     ]
      self.ErrorUp     [ Key ] = fitres[ self.Vars[ Key ] * 3 + 1 ]
      self.ErrorLow    [ Key ] = fitres[ self.Vars[ Key ] * 3 + 2 ]
    for Var in self.Vars:
      self.Min[ Var ] = 1000000
      self.Max[ Var ] = -10000
      self.Values[ Var ] = []
    # Are scans available in this file?
    NewVars = set()
    for Entry in self.Tree:
      for Var in self.Vars: 
        if Entry.scan_par < 0:
          pass
        elif Entry.scan_par == self.Vars[ Var ]:
          NewVars.add( Var )
    TempVars = {}
    for newVar in NewVars:
      TempVars[ newVar ] = self.Vars[ newVar ]
    self.Vars = TempVars
    # Get last iteration index
    MaxIteration = 0 
    for Entry in self.Tree:
      MaxIteration = max( MaxIteration, Entry.scan_iter )
    for Entry in self.Tree:
      if not self.TitleY:
        if Entry.chi2 > Entry.nll:
          self.TitleY = '#Delta#chi^{2}' 
          self.ErrVal = 1
        else:
          self.TitleY = '#DeltaNLL' 
          self.ErrVal = 0.5
      for Var in self.Vars: 
        if Entry.scan_par == self.Vars[ Var ] and Entry.scan_iter == MaxIteration:
          self.Values[ Var ].append( ( getattr( Entry, Var ), max( Entry.chi2, Entry.nll ) ) )
          self.Min[ Var ] = min( self.Min[ Var ], getattr( Entry, Var ) )
          self.Max[ Var ] = max( self.Max[ Var ], getattr( Entry, Var ) )

  def GetGraphs( self ):
    Graphs = {}
    for Var in self.Vars:
      Graphs[ Var ] = GetGraph( Var )
    return Graphs

  def GetGraph( self, Var ):
    MyList = sorted( self.Values[ Var ], key = lambda Pair: Pair[ 0 ] )
    X = array.array( 'd', [ Pair[ 0 ] for Pair in MyList ] )
    Ytemp = array.array( 'd', [ Pair[ 1 ] for Pair in MyList ] )
    Y = array.array( 'd', [ Val - min( Ytemp ) for Val in Ytemp ] )
    Graph = ROOT.TGraph( len( X ), X, Y )
    return Graph

  def __str__( self ):
    return '%s' % ( self.Region )
