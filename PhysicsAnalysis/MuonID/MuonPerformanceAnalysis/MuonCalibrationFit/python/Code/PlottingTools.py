# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import math, re, ROOT

def DoGeneralPlot( Objects, Detector, Stat, IsJustCheck ):
  if not Objects:
    return
  ObjEx = Objects[ 0 ]
  Can = ROOT.TCanvas( '%s___%s_%s___%s' % ( ObjEx.Region, ObjEx.Variable, ObjEx.PtBin, ObjEx.FitLevel ), '', 800, 800 ) 
  Top = ROOT.TPad( 'Top', '', 0.0, 0.3, 1.0, 1.0 )
  Top.SetTopMargin( 0.05 )
  Top.SetBottomMargin( 0.03 )
  Top.Draw()
  Bottom = ROOT.TPad( 'Bottom', '', 0.0, 0.0, 1.0, 0.3 )
  Bottom.SetTopMargin( 0.00 )
  Bottom.SetBottomMargin( 0.35 )
  Bottom.Draw()
  Data = None
  Stack = ROOT.THStack( 'Stack', '' )
  for Obj in Objects:
    if 'Bkg' in Obj.DataType:
      Stack.Add( Obj.Hist, 'hist' )
  for Obj in Objects:
    if 'MC' in Obj.DataType:
      Stack.Add( Obj.Hist, 'hist' )
  for Obj in Objects:
    if Obj.DataType == 'Data':
      Data = Obj.Hist
  ### Top Pad
  Top.cd()
  Stack.Draw()
  Stack.SetMaximum( Data.GetMaximum() * 1.2 )
  Stack.GetXaxis().SetLabelColor( ROOT.kWhite )
  Stack.GetXaxis().SetTitleColor( ROOT.kWhite )
  Stack.GetYaxis().SetTitle( 'Events / %d MeV' % int( round( Data.GetBinWidth( 1 ) * 1000. ) ) )
  Stack.GetYaxis().SetTitleOffset( 2.2 )
  Stack.Draw()
  Error = Stack.GetStack().Last().Clone( 'Error' )
  Error.SetFillStyle( 3245 )
  Error.SetFillColor( ROOT.kBlack )
  Error.SetMarkerSize( 0 )
  Error.Draw( 'E2 same' )
  Data.Draw( 'esame' )
  Lat = ROOT.TLatex()
  Lat.SetNDC()
  Lat.SetTextFont( 42 )
  Lat.SetTextSize( 0.05 )
  Lat.SetTextColor( 1 )
  Lat.SetTextAlign( 12 )
  Lat.DrawLatex( 0.18, 0.9, '#bf{#it{ATLAS}} Internal' ) 
  Lat.SetTextSize( 0.045 )
  Lat.DrawLatex( 0.18, 0.83, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % Stat ) 
  Lat.SetTextSize( 0.035 )
  Lat.DrawLatex( 0.18, 0.76, ObjEx.FitLevel ) 
  for Index, Word in enumerate( ObjEx.RegionExpl.split( ':::' ) ):
    Lat.DrawLatex( 0.18, 0.71 - 0.05 * Index, Word ) 
  Lat.DrawLatex( 0.18, 0.71 - 0.05 * len( ObjEx.RegionExpl.split( ':::' ) ), ConvertPt( ObjEx.PtBin ) ) 
  Index = 0
  for Var in [ 's0', 's1', 'p0', 'p1', 'p2' ]: 
    if getattr( ObjEx, Var ) != 0:
      Lat.SetTextAlign( 31 )
      Lat.DrawLatex( 0.78, 0.65 - 0.05 * Index, '%s = ' % Convert( Var ) ) 
      Lat.SetTextAlign( 11 )
      Lat.DrawLatex( 0.78, 0.65 - 0.05 * Index, '%.8f' % getattr( ObjEx, Var ) ) 
      Index += 1
  if not IsJustCheck:
    chi2 = GetChi2( Data, Error ) 
    ndof = Data.GetNbinsX()
    norm_chi2 = chi2 / float( ndof )
    #Lat.DrawLatex( 0.7, 0.42, '#chi^{2}/ndof = %.1f/%d = %.3f' % ( chi2, ndof, norm_chi2 ) ) 
    Lat.DrawLatex( 0.7, 0.42, '#chi^{2}/ndof = %.3f' % ( norm_chi2 ) ) 
    '''
    Old times: printing LL or chi2 depending on type of fit
    if ObjEx.nll > ObjEx.chi2:
      Lat.DrawLatex( 0.7, 0.42, '-LL = %.1f' % ObjEx.nll ) 
    else:
      Lat.DrawLatex( 0.7, 0.42, '#chi^{2}/ndof = %.1f' % ( ObjEx.chi2 / float( ObjEx.NdoF ) ) ) 
    '''
  Leg = ROOT.TLegend( 0.7, 0.9, 0.85, 0.75 )
  Leg.SetFillStyle( 0 )
  Leg.SetBorderSize( 0 )
  Leg.SetTextFont( 42 )
  Leg.SetTextSize( 0.04 )
  Leg.AddEntry( Data, 'Data #scale[0.9]{(%d)}' % int( Data.Integral() ), 'pe' )
  for Obj in Objects:
    if Obj.DataType == 'Data':
      continue
    Title = ''
    if ObjEx.Variable == 'MassZ' or ObjEx.Variable == 'RhoZ':
      if 'Bkg' in Obj.DataType:
        Leg.AddEntry( Obj.Hist, 'Background', 'f' )
      else:
        Leg.AddEntry( Obj.Hist, 'Z #rightarrow #mu#mu', 'f' )
    if ObjEx.Variable == 'MassJpsi':
      if 'Bkg' in Obj.DataType:
        Leg.AddEntry( Obj.Hist, 'Background', 'f' )
      else:
        Leg.AddEntry( Obj.Hist, 'J/#psi #rightarrow #mu#mu', 'f' )
  Leg.Draw()
  #Top.SetLogy()
  ### Bottom Pad
  Bottom.cd()
  Ratio = GetRatio( Data, Stack, IsJustCheck ) 
  if ObjEx.Variable == 'MassJpsi' or ObjEx.Variable == 'MassZ':
    Ratio.GetXaxis().SetTitle( 'm_{#mu#mu}^{%s} [GeV]' % Detector )
  if ObjEx.Variable == 'RhoZ':
    Ratio.GetXaxis().SetTitle( '#rho = p_{T}^{MS}/p_{T}^{ID} - 1' )
  Ratio.GetXaxis().SetTitleOffset( 4 )
  Ratio.GetYaxis().SetTitleOffset( 2.2 )
  Ratio.Draw()
  Bottom.Update()
  Line = ROOT.TLine( Bottom.GetUxmin(), 1, Bottom.GetUxmax(), 1 )
  Line.SetLineWidth( 3 )
  Line.SetLineStyle( 2 )
  Line.Draw()
  Ratio.Draw( 'same' )
  Can.SaveAs( '.png' )
  Can.SaveAs( '.eps' )
  return Can.GetName() + '.png'

def DoDistrPlot( Objects, Detector, Stat ):
  if not Objects:
    return
  ObjEx = Objects[ 0 ]
  Can = ROOT.TCanvas( '%s___%s_%s___%s' % ( ObjEx.Region, ObjEx.Variable, ObjEx.PtBin, ObjEx.FitLevel ), '', 800, 800 ) 
  Top = ROOT.TPad( 'Top', '', 0.0, 0.3, 1.0, 1.0 )
  Top.SetTopMargin( 0.05 )
  Top.SetBottomMargin( 0.03 )
  Top.Draw()
  Bottom = ROOT.TPad( 'Bottom', '', 0.0, 0.0, 1.0, 0.3 )
  Bottom.SetTopMargin( 0.00 )
  Bottom.SetBottomMargin( 0.35 )
  Bottom.Draw()
  Data = None
  Stack = ROOT.THStack( 'Stack', '' )
  for Obj in Objects:
    if Obj.DataType == 'Data':
      Data = Obj.Hist
  IntegralForMC = Data.Integral()
  for Obj in Objects:
    if 'Bkg' in Obj.DataType:
      Stack.Add( Obj.Hist, 'hist' )
      IntegralForMC -= Obj.Hist.Integral()
  for Obj in Objects:
    if 'MC' in Obj.DataType:
      if Obj.Hist.Integral() > 0:
        Obj.Hist.Scale( IntegralForMC / Obj.Hist.Integral() )
      Stack.Add( Obj.Hist, 'hist' )
  ### Top Pad
  Top.cd()
  Stack.Draw()
  Stack.SetMaximum( Data.GetMaximum() * 1.2 )
  Stack.GetXaxis().SetLabelColor( ROOT.kWhite )
  Stack.GetXaxis().SetTitleColor( ROOT.kWhite )
  Stack.GetYaxis().SetTitle( 'Events' )
  Stack.GetYaxis().SetTitleOffset( 2.2 )
  Stack.Draw()
  Error = Stack.GetStack().Last().Clone( 'Error' )
  Error.SetFillStyle( 3245 )
  Error.SetFillColor( ROOT.kBlack )
  Error.SetMarkerSize( 0 )
  Error.Draw( 'E2 same' )
  Data.Draw( 'esame' )
  Lat = ROOT.TLatex()
  Lat.SetNDC()
  Lat.SetTextFont( 42 )
  Lat.SetTextSize( 0.05 )
  Lat.SetTextColor( 1 )
  Lat.SetTextAlign( 12 )
  Lat.DrawLatex( 0.18, 0.9, '#bf{#it{ATLAS}} Internal' ) 
  Lat.SetTextSize( 0.045 )
  Lat.DrawLatex( 0.18, 0.83, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % Stat ) 
  Lat.SetTextSize( 0.035 )
  Lat.DrawLatex( 0.18, 0.76, ObjEx.FitLevel ) 
  for Index, Word in enumerate( ObjEx.RegionExpl.split( ':::' ) ):
    Lat.DrawLatex( 0.18, 0.71 - 0.05 * Index, Word ) 
  Lat.DrawLatex( 0.18, 0.71 - 0.05 * len( ObjEx.RegionExpl.split( ':::' ) ), ConvertPt( ObjEx.PtBin ) ) 
  Index = 0
  Leg = ROOT.TLegend( 0.7, 0.8, 0.85, 0.65 )
  Leg.SetFillStyle( 0 )
  Leg.SetBorderSize( 0 )
  Leg.SetTextFont( 42 )
  Leg.SetTextSize( 0.04 )
  Leg.AddEntry( Data, 'Data', 'pe' )
  for Obj in Objects:
    if Obj.DataType == 'Data':
      continue
    Title = ''
    if 'Z' in ObjEx.Variable and not 'Jpsi' in ObjEx.Variable: 
      if 'Bkg' in Obj.DataType:
        Leg.AddEntry( Obj.Hist, 'Background', 'f' )
      else:
        Leg.AddEntry( Obj.Hist, 'Z #rightarrow #mu#mu', 'f' )
    if 'Jpsi' in ObjEx.Variable:
      if 'Bkg' in Obj.DataType:
        Leg.AddEntry( Obj.Hist, 'Background', 'f' )
      else:
        Leg.AddEntry( Obj.Hist, 'J/#psi #rightarrow #mu#mu', 'f' )
  Leg.Draw()
  ### Bottom Pad
  Bottom.cd()
  Ratio = GetRatio( Data, Stack ) 
  Ratio.GetXaxis().SetTitleOffset( 4 )
  Ratio.GetYaxis().SetTitleOffset( 2.2 )
  Ratio.Draw()
  Bottom.Update()
  Line = ROOT.TLine( Bottom.GetUxmin(), 1, Bottom.GetUxmax(), 1 )
  Line.SetLineWidth( 3 )
  Line.SetLineStyle( 2 )
  Line.Draw()
  Ratio.Draw( 'same' )
  Can.SaveAs( '.png' )
  Can.SaveAs( '.eps' )
  return Can.GetName() + '.png'

def DoBackgroundPlot( Object, Detector, Stat ):
  if not Object:
    return
  #####
  Can = ROOT.TCanvas( '%s___%s_%s___BkgFitData' % ( Object.Region, Object.Variable, Object.PtBin ), '', 800, 800 ) 
  Model = Object.Ws.pdf( 'tempModel' )
  DataSet = Object.Ws.data( 'tempDataSet_Data' ) 
  X = Object.Ws.var( 'x' ) 
  X.setUnit( '' )
  X.SetTitle( 'm_{#mu#mu}^{%s} [GeV]' % Detector ) 
  Frame = X.frame()
  DataSet.plotOn( Frame )
  Model.plotOn( Frame, ROOT.RooFit.LineColor( ROOT.kRed + 1 ), ROOT.RooFit.Name( 'TOTAL' ) )
  Model.plotOn( Frame, ROOT.RooFit.Components( 'tempModel_MC' ), ROOT.RooFit.LineColor( ROOT.kOrange + 7 ), ROOT.RooFit.LineStyle( ROOT.kDashed ), ROOT.RooFit.Name( 'SIGNAL' ) )
  #Model.plotOn( Frame, ROOT.RooFit.Components( 'tempCB' ), ROOT.RooFit.LineColor( ROOT.kOrange + 7 ), ROOT.RooFit.LineStyle( ROOT.kDashed ), ROOT.RooFit.Name( 'SIGNAL' ) )
  Model.plotOn( Frame, ROOT.RooFit.Components( 'tempExpo' ), ROOT.RooFit.LineColor( ROOT.kAzure + 1 ), ROOT.RooFit.LineStyle( ROOT.kDotted ), ROOT.RooFit.Name( 'BACKGROUND' ) )
  DataSet.plotOn( Frame, ROOT.RooFit.Name( 'DATA' ) )
  Frame.Draw()
  Lat = ROOT.TLatex()
  Lat.SetNDC()
  Lat.SetTextFont( 42 )
  Lat.SetTextSize( 0.035 )
  Lat.SetTextColor( 1 )
  Lat.SetTextAlign( 12 )
  Lat.DrawLatex( 0.18, 0.9, '#bf{#it{ATLAS}} Internal' ) 
  Lat.SetTextSize( 0.03 )
  Lat.DrawLatex( 0.18, 0.85, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % Stat ) 
  Lat.SetTextSize( 0.025 )
  for Index, Word in enumerate( Object.RegionExpl.split( ':::' ) ):
    Lat.DrawLatex( 0.18, 0.78 - 0.04 * Index, Word ) 
  Lat.DrawLatex( 0.18, 0.78 - 0.04 * len( Object.RegionExpl.split( ':::' ) ), ConvertPt( Object.PtBin ) ) 
  Leg = ROOT.TLegend( 0.6, 0.9, 0.85, 0.65 )
  Leg.SetFillStyle( 0 )
  Leg.SetBorderSize( 0 )
  Leg.SetTextFont( 42 )
  Leg.SetTextSize( 0.025 )
  Leg.AddEntry( Frame.findObject( 'DATA' ), 'Data', 'pe' )
  Leg.AddEntry( Frame.findObject( 'TOTAL' ), 'Signal + Background Fit', 'l' )
  Leg.AddEntry( Frame.findObject( 'SIGNAL' ), 'Signal', 'l' )
  Leg.AddEntry( None, '(Crystal Ball + Gaussian)', '' )
  Leg.AddEntry( Frame.findObject( 'BACKGROUND' ), 'Background', 'l' )
  Leg.AddEntry( None, '(Exponential)', '' )
  Leg.Draw()
  Can.SaveAs( '.png' )
  Can.SaveAs( '.eps' )
  #####
  Can2 = ROOT.TCanvas( '%s___%s_%s___BkgFitMonteCarlo' % ( Object.Region, Object.Variable, Object.PtBin ), '', 800, 800 ) 
  Model = Object.Ws.pdf( 'tempModel_MC' )
  DataSet = Object.Ws.data( 'tempDataSet_MC' ) 
  X = Object.Ws.var( 'x' ) 
  X.setUnit( '' )
  X.SetTitle( 'm_{#mu#mu}^{%s} [GeV]' % Detector ) 
  Frame = X.frame()
  DataSet.plotOn( Frame, ROOT.RooFit.MarkerStyle( 25 ) )
  Model.plotOn( Frame, ROOT.RooFit.LineColor( ROOT.kOrange + 7 ), ROOT.RooFit.LineStyle( ROOT.kDashed ), ROOT.RooFit.Name( 'SIGNAL' ) )
  Model.plotOn( Frame, ROOT.RooFit.Components( 'tempCB_MC' ), ROOT.RooFit.LineColor( ROOT.kPink - 7 ), ROOT.RooFit.Name( 'CB' ) )
  Model.plotOn( Frame, ROOT.RooFit.Components( 'tempGauss_MC' ), ROOT.RooFit.LineColor( ROOT.kPink - 2 ), ROOT.RooFit.Name( 'GAUSS' ) )
  DataSet.plotOn( Frame, ROOT.RooFit.Name( 'DATASET' ) )
  Frame.Draw()
  Lat = ROOT.TLatex()
  Lat.SetNDC()
  Lat.SetTextFont( 42 )
  Lat.SetTextSize( 0.035 )
  Lat.SetTextColor( 1 )
  Lat.SetTextAlign( 12 )
  Lat.DrawLatex( 0.18, 0.9, '#bf{#it{ATLAS}} Internal' ) 
  Lat.SetTextSize( 0.03 )
  Lat.DrawLatex( 0.18, 0.85, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % Stat ) 
  Lat.SetTextSize( 0.025 )
  for Index, Word in enumerate( Object.RegionExpl.split( ':::' ) ):
    Lat.DrawLatex( 0.18, 0.78 - 0.04 * Index, Word ) 
  Lat.DrawLatex( 0.18, 0.78 - 0.04 * len( Object.RegionExpl.split( ':::' ) ), ConvertPt( Object.PtBin ) ) 
  Leg = ROOT.TLegend( 0.6, 0.9, 0.85, 0.72 )
  Leg.SetFillStyle( 0 )
  Leg.SetBorderSize( 0 )
  Leg.SetTextFont( 42 )
  Leg.SetTextSize( 0.025 )
  Leg.AddEntry( Frame.findObject( 'DATASET' ), 'MC Signal Template', 'pe' )
  Leg.AddEntry( Frame.findObject( 'SIGNAL' ), 'Crystal Ball + Gaussian', 'l' )
  Leg.AddEntry( Frame.findObject( 'CB' ), 'Crystal Ball', 'l' )
  Leg.AddEntry( Frame.findObject( 'GAUSS' ), 'Gaussian', 'l' )
  Leg.Draw()
  Can2.SaveAs( '.png' )
  Can2.SaveAs( '.eps' )
  return [ Can.GetName() + '.png', Can2.GetName() + '.png' ]
  #return [ Can.GetName() + '.png' ]

def DoScanPlot( Object, Detector, Stat ):
  if not Object:
    print 'No valid object given!!!!'
    return
  Plots = []
  for Var in Object.Vars:
    Can = ROOT.TCanvas( '%s___%s___Scan' % ( Object.Region, Var ), '', 800, 800 ) 
    #print '%s___%s___Scan' % ( Object.Region, Var )
    myGraph = Object.GetGraph( Var )
    myGraph.SetLineWidth( 2 )
    myGraph.SetLineColor( ROOT.kBlack )
    myGraph.SetLineStyle( 1 )
    myGraph.Draw( 'alp' )
    myGraph.GetXaxis().SetLimits( Object.Min[ Var ], Object.Max[ Var ] )
    myGraph.GetXaxis().SetTitle( Convert( Var, Detector ) ) 
    myGraph.GetYaxis().SetTitle( Object.TitleY ) 
    myGraph.Draw( 'alp' )
    for Val in [ 1 * Object.ErrVal, 4 * Object.ErrVal ]:
      locals()[ 'line%d' % Val ] = ROOT.TLine( Object.Min[ Var ], Val, Object.Max[ Var ], Val )
      locals()[ 'line%d' % Val ].SetLineColor( ROOT.kRed + 1 )
      locals()[ 'line%d' % Val ].SetLineStyle( 2 )
      locals()[ 'line%d' % Val ].SetLineWidth( 2 )
      locals()[ 'line%d' % Val ].Draw()
    for Index, Error in enumerate( [ -Object.ErrorLow[ Var ], Object.ErrorUp[ Var ] ] ):
      X = Object.BestFitValue[ Var ] + Error
      locals()[ 'vert_line%d' % Index ] = ROOT.TLine( X, 0, X, 2.5 )
      locals()[ 'vert_line%d' % Index ].SetLineColor( ROOT.kBlack )
      locals()[ 'vert_line%d' % Index ].SetLineStyle( 2 )
      locals()[ 'vert_line%d' % Index ].SetLineWidth( 2 )
      locals()[ 'vert_line%d' % Index ].Draw()
    myGraph.Draw( 'lp' )
    Lat = ROOT.TLatex()
    Lat.SetNDC()
    Lat.SetTextFont( 42 )
    Lat.SetTextSize( 0.035 )
    Lat.SetTextColor( 1 )
    Lat.SetTextAlign( 12 )
    Lat.DrawLatex( 0.18, 0.9, '#bf{#it{ATLAS}} Internal' ) 
    Lat.SetTextSize( 0.03 )
    Lat.DrawLatex( 0.18, 0.85, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % Stat ) 
    Lat.SetTextSize( 0.025 )
    Lat.DrawLatex( 0.18, 0.8, '#color[633]{Fitted value: %+.5f +%.5f -%.5f}' % ( Object.BestFitValue[ Var ], Object.ErrorUp[ Var ], Object.ErrorLow[ Var ] ) )  
    for Index, Word in enumerate( Object.RegionExpl.split( ':::' ) ):
      Lat.DrawLatex( 0.65, 0.9 - 0.05 * Index, Word ) 
    Can.SaveAs( '.png' )
    Can.SaveAs( '.eps' )
    Plots.append( Can.GetName() + '.png' )
  return Plots

def GetRatio( HistNum, HistDen, ShrinkRange = False ):
  if HistDen.ClassName() == 'THStack':
    HistDen = HistDen.GetStack().Last()
  Result = HistNum.Clone( 'RatioHistogramFor' + HistNum.GetName() )
  AverageRatio = 0
  NumberOfPoints = 0
  for i in range( 1, Result.GetNbinsX() ):
    if HistDen.GetBinContent( i ) > 0:
      Result.SetBinContent( i, HistNum.GetBinContent( i ) / HistDen.GetBinContent( i ) )
      Result.SetBinError( i, math.sqrt( math.pow( HistNum.GetBinError( i ) / HistDen.GetBinContent( i ), 2 ) + math.pow( HistNum.GetBinContent( i ) * HistDen.GetBinError( i ) / math.pow( HistDen.GetBinContent( i ), 2 ), 2 ) ) )
      AverageRatio += HistNum.GetBinContent( i ) / HistDen.GetBinContent( i )
      NumberOfPoints += 1.
  Result.SetLineStyle( 1 );
  Result.SetLineColor( ROOT.kBlack );
  Result.SetMarkerSize( 1 );
  Result.GetYaxis().SetNdivisions( 5, 3, 0 )
  Result.SetMaximum( 1.23 )
  Result.SetMinimum( 0.77 )
  '''
  if ShrinkRange:
    Result.SetMaximum( 1.09 )
    Result.SetMinimum( 0.91 )
  '''
  Result.GetYaxis().SetTitle( '#frac{Data}{MonteCarlo}' );
  Result.GetYaxis().CenterTitle( True )
  #print ' Average Ratio: %.2f' % ( AverageRatio / NumberOfPoints )
  return Result

def ConvertPt( Bin ):
  Reg = re.compile( '(\d+)_(\d)_pt_(\d+)_(\d)' )
  Match = Reg.search( Bin )
  if Match:
    return '%d.%d < p_{T} [GeV] < %d.%d' % ( int( Match.group( 1 ) ), int( Match.group( 2 ) ), int( Match.group( 3 ) ), int( Match.group( 4 ) ) )
  return ''

def Convert( Name, Det = '' ):
  if Name == 'chi2':
    return '#chi^{2}/ndof'
  if Name == 's0':
    return 's_{0}^{%s} [GeV]' % Det
  if Name == 's1':
    return 's_{1}^{%s}' % Det
  if Name == 'p0':
    return '#Deltar_{0}^{%s} [GeV]' % Det
  if Name == 'p1':
    return '#Deltar_{1}^{%s}' % Det
  if Name == 'p2':
    return '#Deltar_{2}^{%s} [GeV^{-1}]' % Det

def GetChi2( d, m ):
  result = 0
  for Index in range( 1, d.GetNbinsX() + 1 ):
    data = d.GetBinContent( Index )
    data_err_sq = d.GetBinContent( Index )
    mc = m.GetBinContent( Index )
    mc_err_sq = m.GetBinError( Index )
    diff = data - mc
    if ( data_err_sq + mc_err_sq ) > 0:
      result += diff * diff / ( data_err_sq + mc_err_sq )
  return result
