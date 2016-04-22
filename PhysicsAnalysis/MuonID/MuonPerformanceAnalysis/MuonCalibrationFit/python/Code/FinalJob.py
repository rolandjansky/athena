# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import array, math, os, re, pickle, subprocess, sys
import ROOT
import FitResult, PlottingTools, Tools

class Tables:

  # --- # --- # --- # --- #
  def __init__ ( self, ID = '', MS = '' ):
    self.CV_ID_Job = GetJobInfo( ID )
    self.CV_MS_Job = GetJobInfo( MS )
    self.Syst_ID_Jobs = {}
    self.Syst_MS_Jobs = {}
    self.Checked = False
    self.ID_CentralValues = {}
    self.ID_SystematicsUp = {}
    self.ID_SystematicsDown = {}
    self.MS_CentralValues = {}
    self.MS_SystematicsUp = {}
    self.MS_SystematicsDown = {}
    for Type in [ 'Small', 'Large' ]:
      self.MS_CentralValues[ Type ] = {}
      self.MS_SystematicsUp[ Type ] = {}
      self.MS_SystematicsDown[ Type ] = {}

  # --- # --- # --- # --- #
  def Add ( self, title, name ):
    job = GetJobInfo( name )
    if job.Detector == 'ID':
      self.Syst_ID_Jobs[ title ] = job
    else:
      self.Syst_MS_Jobs[ title ] = job

  # --- # --- # --- # --- #
  def PrintTables ( self ):
    for var in [ 's1', 'p1', 'p2' ]:
      print
      print ' ----- Print table values for %s (%s) -----' % ( var, str( GetConversion( 'ID', var ) ) )
      print
      self.PrintIDLine( '$|\eta|\!<\!1.05$          ', 'b', self.CV_ID_Job, self.Syst_ID_Jobs, var )
      self.PrintIDLine( '$1.05\!\le\!|\eta|\!<\!2.0$', 't', self.CV_ID_Job, self.Syst_ID_Jobs, var )
      self.PrintIDLine( '$|\eta|\!\ge\!2.0$         ', 'e', self.CV_ID_Job, self.Syst_ID_Jobs, var )
    for sector in [ 'Large', 'Small' ]:
      for var in [ 's0', 's1', 'p0', 'p1', 'p2' ]:
        print
        print ' ----- Print table values for %s %s (%s) -----' % ( var, sector, str( GetConversion( 'MS', var ) ) )
        print
        self.PrintMSLine( '$|\eta|\!<\!1.05$          ', 'b', self.CV_MS_Job, self.Syst_MS_Jobs, var, sector )
        self.PrintMSLine( '$1.05\!\le\!|\eta|\!<\!2.0$', 't', self.CV_MS_Job, self.Syst_MS_Jobs, var, sector )
        self.PrintMSLine( '$|\eta|\!\ge\!2.0$         ', 'e', self.CV_MS_Job, self.Syst_MS_Jobs, var, sector )

  # --- # --- # --- # --- #
  def PrintIDLine ( self, title, region, nom, syst, var ):
    nominal   = self.Nominal( region, nom, var )
    stat      = self.Stat( region, nom, var )
    BkgSysDW  = self.Syst( region, syst[ 'Bkg/2' ], nom, var )
    BkgSysUP  = self.Syst( region, syst[ 'Bkg*2' ], nom, var )
    JpsiBkg   = self.Syst( region, syst[ 'Linear' ], nom, var )
    JpsiMinPt = self.Syst( region, syst[ 'Min Pt' ], nom, var )
    MCutSysDW = self.Syst( region, syst[ 'm_{Z} Window(#downarrow)' ], nom, var )
    MCutSysUP = self.Syst( region, syst[ 'm_{Z} Window(#uparrow)' ], nom, var )
    OnlyZ     = self.Syst( region, syst[ 'Only Z' ], nom, var )
    OnlyPsi   = self.Syst( region, syst[ 'Only J/#psi' ], nom, var )
    TotUncUp = math.sqrt( BkgSysDW[ 0 ] * BkgSysDW[ 0 ] + BkgSysUP[ 0 ] * BkgSysUP[ 0 ] + JpsiBkg[ 0 ] * JpsiBkg[ 0 ] + JpsiMinPt[ 0 ] * JpsiMinPt[ 0 ] + MCutSysDW[ 0 ] * MCutSysDW[ 0 ] + MCutSysUP[ 0 ] * MCutSysUP[ 0 ] + OnlyZ[ 0 ] * OnlyZ[ 0 ] + OnlyPsi[ 0 ] * OnlyPsi[ 0 ] + stat * stat ) 
    TotUncDw = math.sqrt( BkgSysDW[ 1 ] * BkgSysDW[ 1 ] + BkgSysUP[ 1 ] * BkgSysUP[ 1 ] + JpsiBkg[ 1 ] * JpsiBkg[ 1 ] + JpsiMinPt[ 1 ] * JpsiMinPt[ 1 ] + MCutSysDW[ 1 ] * MCutSysDW[ 1 ] + MCutSysUP[ 1 ] * MCutSysUP[ 1 ] + OnlyZ[ 1 ] * OnlyZ[ 1 ] + OnlyPsi[ 1 ] * OnlyPsi[ 1 ] + stat * stat ) 
    Tuple = ( title, nominal, TotUncUp, TotUncDw, stat, BkgSysDW[ 0 ], BkgSysDW[ 1 ], BkgSysUP[ 0 ], BkgSysUP[ 1 ], JpsiBkg[ 0 ], JpsiBkg[ 1 ], JpsiMinPt[ 0 ], JpsiMinPt[ 1 ], MCutSysDW[ 0 ], MCutSysDW[ 1 ], MCutSysUP[ 0 ], MCutSysUP[ 1 ], OnlyZ[ 0 ], OnlyZ[ 1 ], OnlyPsi[ 0 ], OnlyPsi[ 1 ] ) 
    print '%s  & %.5f & $^{+%.5f}_{-%.5f}$ & %.5f & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ \\\\ [2mm]' % Tuple

  # --- # --- # --- # --- #
  def PrintMSLine ( self, title, region, nom, syst, var, sec ):
    nominal   = self.Nominal( region, nom, var, sec )
    stat      = self.Stat( region, nom, var, sec )
    BkgSysDW  = self.Syst( region, syst[ 'Bkg/2' ], nom, var, sec )
    BkgSysUP  = self.Syst( region, syst[ 'Bkg*2' ], nom, var, sec )
    JpsiBkg   = self.Syst( region, syst[ 'Linear' ], nom, var, sec )
    JpsiMinPt = self.Syst( region, syst[ 'Min Pt' ], nom, var, sec )
    MCutSysDW = self.Syst( region, syst[ 'm_{Z} Window(#downarrow)' ], nom, var, sec )
    MCutSysUP = self.Syst( region, syst[ 'm_{Z} Window(#uparrow)' ], nom, var, sec )
    P0     = self.Syst( region, syst[ '#Delta r_{0}' ], nom, var, sec )
    P2Up   = self.Syst( region, syst[ '#Delta r_{2}(#uparrow)' ], nom, var, sec )
    P2Dw   = self.Syst( region, syst[ '#Delta r_{2}(#downarrow)' ], nom, var, sec )
    TotUncUp = math.sqrt( BkgSysDW[ 0 ] * BkgSysDW[ 0 ] + BkgSysUP[ 0 ] * BkgSysUP[ 0 ] + JpsiBkg[ 0 ] * JpsiBkg[ 0 ] + JpsiMinPt[ 0 ] * JpsiMinPt[ 0 ] + MCutSysDW[ 0 ] * MCutSysDW[ 0 ] + MCutSysUP[ 0 ] * MCutSysUP[ 0 ] + P0[ 0 ] * P0[ 0 ] + P2Up[ 0 ] * P2Up[ 0 ] + P2Dw[ 0 ] * P2Dw[ 0 ] + stat * stat ) 
    TotUncDw = math.sqrt( BkgSysDW[ 1 ] * BkgSysDW[ 1 ] + BkgSysUP[ 1 ] * BkgSysUP[ 1 ] + JpsiBkg[ 1 ] * JpsiBkg[ 1 ] + JpsiMinPt[ 1 ] * JpsiMinPt[ 1 ] + MCutSysDW[ 1 ] * MCutSysDW[ 1 ] + MCutSysUP[ 1 ] * MCutSysUP[ 1 ] + P0[ 1 ] * P0[ 1 ] + P2Up[ 1 ] * P2Up[ 1 ] + P2Dw[ 1 ] * P2Dw[ 1 ] + stat * stat ) 
    Tuple = ( title, nominal, TotUncUp, TotUncDw, stat, BkgSysDW[ 0 ], BkgSysDW[ 1 ], BkgSysUP[ 0 ], BkgSysUP[ 1 ], JpsiBkg[ 0 ], JpsiBkg[ 1 ], JpsiMinPt[ 0 ], JpsiMinPt[ 1 ], MCutSysDW[ 0 ], MCutSysDW[ 1 ], MCutSysUP[ 0 ], MCutSysUP[ 1 ], P0[ 0 ], P0[ 1 ], P2Up[ 0 ], P2Up[ 1 ], P2Dw[ 0 ], P2Dw[ 1 ] ) 
    print '%s  & %.5f & $^{+%.5f}_{-%.5f}$ & %.5f & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ & $^{+%.5f}_{-%.5f}$ \\\\ [2mm]' % Tuple

  # --- # --- # --- # --- #
  def Nominal( self, region, job, var, sec = None ):
    RegWeights = self.GetWeights( job.Detector )
    Values = self.GetValues( job, GetVariablePosition( var ), sec )
    Total = 0.
    Conversion = GetConversion( job.Detector, var )
    for Key in Values:
      Total += Values[ Key ] * RegWeights[ region ][ Key ]
    return Total * Conversion

  # --- # --- # --- # --- #
  def Stat( self, region, job, var, sec = None ):
    RegWeights = self.GetWeights( job.Detector )
    Values1 = self.GetValues( job, GetVariablePosition( var ) + 1, sec )
    Values2 = self.GetValues( job, GetVariablePosition( var ) + 2, sec )
    Total = 0.
    Conversion = GetConversion( job.Detector, var )
    for Key in Values1:
      Total += abs( 0.5 * Values1[ Key ] * RegWeights[ region ][ Key ] )
      Total += abs( 0.5 * Values2[ Key ] * RegWeights[ region ][ Key ] )
    if Total > 100:
      return 0
    return Total * Conversion
    
  # --- # --- # --- # --- #
  def Syst( self, region, job, nom, var, sec = None ):
    RegWeights = self.GetWeights( job.Detector )
    Values = self.GetValues( nom, GetVariablePosition( var ), sec )
    Systs = self.GetValues( job, GetVariablePosition( var ), sec )
    TotalUp = 0.
    TotalDown = 0.
    Conversion = GetConversion( job.Detector, var )
    for Key in Values:
      if Systs[ Key ] - Values[ Key ] > 0:
        TotalUp += abs( Systs[ Key ] - Values[ Key ] ) * RegWeights[ region ][ Key ]
      else:
        TotalDown += abs( Systs[ Key ] - Values[ Key ] ) * RegWeights[ region ][ Key ]
    return ( TotalUp * Conversion, TotalDown * Conversion )

  # --- # --- # --- # --- #
  def Test ( self ):
    print self.GetValues( self.CV_ID_Job, GetVariablePosition( 's1' ) )
    print self.Nominal( 'b', self.CV_ID_Job, 's1' )

  # --- # --- # --- # --- #
  def GetValues ( self, job, pos, sec = None ):
    InputFile = ROOT.TFile( job.Output )  
    Keys = ROOT.gDirectory.GetListOfKeys() 
    Reg = re.compile( 'FinalResults___%s___(\w+)' % job.Name )
    Results = {}
    for Key in Keys:
      TheName = Key.GetName()
      Match = Reg.match( TheName )
      if 'TVectorT<double>' in Key.GetClassName() and Match:
        if not sec or sec in Match.group( 1 ):
          Results[ Match.group( 1 ) ] = InputFile.Get( Key.GetName() )[ pos ]
    return Results

  # --- # --- # --- # --- #
  def GetWeights( self, det ):
    RegWeights = None 
    if det == 'ID':
      RegWeights = { 
        'b': { 'Region1': 0.4 / 2.1, 'Region2': 0.4 / 2.1, 'Region3': 0.4 / 2.1, 'Region4': 0.4 / 2.1, 'Region5': 0.25 / 2.1, 'Region6': 0.25 / 2.1, 'Region7': 0., 'Region8': 0., 'Region9': 0., 'Region10': 0., 'Region11': 0., 'Region12': 0., 'Region13': 0., 'Region14': 0., 'Region15': 0., 'Region16': 0., 'Region17': 0., 'Region18': 0., }, 
        't': { 'Region1': 0., 'Region2': 0., 'Region3': 0., 'Region4': 0., 'Region5': 0., 'Region6': 0., 'Region7' : 0.2 / 1.9, 'Region8' : 0.2 / 1.9, 'Region9' : 0.25 / 1.9, 'Region10': 0.25 / 1.9, 'Region11': 0.2 / 1.9, 'Region12': 0.2 / 1.9, 'Region13': 0.3 / 1.9, 'Region14': 0.3 / 1.9, 'Region15': 0., 'Region16': 0., 'Region17': 0., 'Region18': 0., }, 
        'e': { 'Region1': 0., 'Region2': 0., 'Region3': 0., 'Region4': 0., 'Region5': 0., 'Region6': 0., 'Region7' : 0., 'Region8' : 0., 'Region9' : 0., 'Region10': 0., 'Region11': 0., 'Region12': 0., 'Region13': 0., 'Region14': 0., 'Region15': 0.3 / 1.2, 'Region16': 0.3 / 1.2, 'Region17': 0.3 / 1.2, 'Region18': 0.3 / 1.2, } } 
    else:
      RegWeights = { 
        'b': { 'RegNegLarge1': 0.4 / 2.1, 'RegNegLarge2': 0.4 / 2.1, 'RegNegLarge3': 0.25 / 2.1, 'RegNegLarge4': 0., 'RegNegLarge5': 0., 'RegNegLarge6': 0., 'RegNegLarge7': 0., 'RegNegLarge8': 0., 'RegNegLarge9': 0., 'RegPosLarge1': 0.4 / 2.1, 'RegPosLarge2': 0.4 / 2.1, 'RegPosLarge3': 0.25 / 2.1, 'RegPosLarge4': 0., 'RegPosLarge5': 0., 'RegPosLarge6': 0., 'RegPosLarge7': 0., 'RegPosLarge8': 0., 'RegPosLarge9': 0., 'RegNegSmall1': 0.4 / 2.1, 'RegNegSmall2': 0.4 / 2.1, 'RegNegSmall3': 0.25 / 2.1, 'RegNegSmall4': 0., 'RegNegSmall5': 0., 'RegNegSmall6': 0., 'RegNegSmall7': 0., 'RegNegSmall8': 0., 'RegNegSmall9': 0., 'RegPosSmall1': 0.4 / 2.1, 'RegPosSmall2': 0.4 / 2.1, 'RegPosSmall3': 0.25 / 2.1, 'RegPosSmall4': 0., 'RegPosSmall5': 0., 'RegPosSmall6': 0., 'RegPosSmall7': 0., 'RegPosSmall8': 0., 'RegPosSmall9': 0., },
        't': { 'RegNegLarge1': 0., 'RegNegLarge2': 0., 'RegNegLarge3': 0., 'RegNegLarge4': 0.2 / 1.9, 'RegNegLarge5': 0.25 / 1.9, 'RegNegLarge6': 0.2 / 1.9, 'RegNegLarge7': 0.3 / 1.9, 'RegNegLarge8': 0., 'RegNegLarge9': 0., 'RegPosLarge1': 0., 'RegPosLarge2': 0., 'RegPosLarge3': 0., 'RegPosLarge4': 0.2 / 1.9, 'RegPosLarge5': 0.25 / 1.9, 'RegPosLarge6': 0.2 / 1.9, 'RegPosLarge7': 0.3 / 1.9, 'RegPosLarge8': 0., 'RegPosLarge9': 0., 'RegNegSmall1': 0., 'RegNegSmall2': 0., 'RegNegSmall3': 0., 'RegNegSmall4': 0.2 / 1.9, 'RegNegSmall5': 0.25 / 1.9, 'RegNegSmall6': 0.2 / 1.9, 'RegNegSmall7': 0.3 / 1.9, 'RegNegSmall8': 0., 'RegNegSmall9': 0., 'RegPosSmall1': 0., 'RegPosSmall2': 0., 'RegPosSmall3': 0., 'RegPosSmall4': 0.2 / 1.9, 'RegPosSmall5': 0.25 / 1.9, 'RegPosSmall6': 0.2 / 1.9, 'RegPosSmall7': 0.3 / 1.9, 'RegPosSmall8': 0., 'RegPosSmall9': 0., },
        'e': { 'RegNegLarge1': 0., 'RegNegLarge2': 0., 'RegNegLarge3': 0., 'RegNegLarge4': 0., 'RegNegLarge5': 0., 'RegNegLarge6': 0., 'RegNegLarge7': 0., 'RegNegLarge8': 0.3 / 1.2, 'RegNegLarge9': 0.3 / 1.2, 'RegPosLarge1': 0., 'RegPosLarge2': 0., 'RegPosLarge3': 0., 'RegPosLarge4': 0., 'RegPosLarge5': 0., 'RegPosLarge6': 0., 'RegPosLarge7': 0., 'RegPosLarge8': 0.3 / 1.2, 'RegPosLarge9': 0.3 / 1.2, 'RegNegSmall1': 0., 'RegNegSmall2': 0., 'RegNegSmall3': 0., 'RegNegSmall4': 0., 'RegNegSmall5': 0., 'RegNegSmall6': 0., 'RegNegSmall7': 0., 'RegNegSmall8': 0.3 / 1.2, 'RegNegSmall9': 0.3 / 1.2, 'RegPosSmall1': 0., 'RegPosSmall2': 0., 'RegPosSmall3': 0., 'RegPosSmall4': 0., 'RegPosSmall5': 0., 'RegPosSmall6': 0., 'RegPosSmall7': 0., 'RegPosSmall8': 0.3 / 1.2, 'RegPosSmall9': 0.3 / 1.2, } }
    return RegWeights

class FinalJob:

  # --- # --- # --- # --- #
  def __init__ ( self, ID = '', MS = '' ):
    self.CV_ID_Job = GetJobInfo( ID )
    self.CV_MS_Job = GetJobInfo( MS )
    self.Syst_ID_Jobs = {}
    self.Syst_MS_Jobs = {}
    self.Checked = False
    self.IDRanges = { 's1': ( -0.004, 0.010 ), 'p1': ( 0., 0.035 ), 'p2': ( 0., 0.0007 ) }
    self.ID_CentralValues = {}
    self.ID_SystematicsUp = {}
    self.ID_SystematicsDown = {}
    self.MSRanges = { 's0': ( -0.13, 0.08 ), 's1': ( -0.008, 0.006 ), 'p0': ( -0.005, 0.08 ), 'p1': ( 0, 0.050 ), 'p2': ( 0., 0.0005 ) }
    self.MS_CentralValues = {}
    self.MS_SystematicsUp = {}
    self.MS_SystematicsDown = {}
    for Type in [ 'Small', 'Large' ]:
      self.MS_CentralValues[ Type ] = {}
      self.MS_SystematicsUp[ Type ] = {}
      self.MS_SystematicsDown[ Type ] = {}

  # --- # --- # --- # --- #
  def PrintFiles ( self, name ):
    File = open( 'Scale_%s.dat' % name, 'w' )
    File.write( 'Scale_ID s0_MS Scale_MS Scale_ID_SUp s0_MS_SUp Scale_MS_SUp Scale_ID_SDw s0_MS_SDw Scale_MS_SDw Scale_CB Scale_CB_err\n' )
    if self.ID_CentralValues:
      for Index in self.ID_CentralValues[ 's1' ]:
        # Central Value
        s1_ID = self.ID_CentralValues[ 's1' ][ Index ]
        # Max systematic variation - central value
        s1_ID_Up = self.ID_SystematicsUp[ 's1' ][ Index ] - s1_ID
        # Central value - min systematic variation
        s1_ID_Down = s1_ID - self.ID_SystematicsDown[ 's1' ][ Index ]
        for Type in [ 'Large', 'Small' ]:
          if self.MS_CentralValues[ Type ]:
            # Central Values
            s0_MS = self.MS_CentralValues[ Type ][ 's0' ][ Index ]
            s1_MS = self.MS_CentralValues[ Type ][ 's1' ][ Index ]
            # Max systematic variations - central values
            s0_MS_Up = self.MS_SystematicsUp[ Type ][ 's0' ][ Index ] - s0_MS
            s1_MS_Up = self.MS_SystematicsUp[ Type ][ 's1' ][ Index ] - s1_MS
            # Central values - min systematic variations
            s0_MS_Down = s0_MS - self.MS_SystematicsDown[ Type ][ 's0' ][ Index ]
            s1_MS_Down = s1_MS - self.MS_SystematicsDown[ Type ][ 's1' ][ Index ]
          else:
            s0_MS = 0 
            s1_MS = 0
            s0_MS_Up = 0
            s1_MS_Up = 0
            s0_MS_Down = 0
            s1_MS_Down = 0
          File.write( '%f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f\n' % ( s1_ID, s0_MS, s1_MS, s1_ID_Up, s0_MS_Up, s1_MS_Up, s1_ID_Down, s0_MS_Down, s1_MS_Down, -1, -1 ) )
    File.close()
    FormatFile( 'Scale_%s.dat' % name )

    File = open( 'Smearing_%s.dat' % name, 'w' )
    File.write(  'p1_ID  p2_ID  p2_ID_TG2  p0_MS  p1_MS  p2_MS  SUp_p1_ID  SUp_p2_ID  SUp_p2_ID_TG2  SUp_p0_MS  SUp_p1_MS  SUp_p2_MS  SDw_p1_ID  SDw_p2_ID  SDw_p2_ID_TG2  SDw_p0_MS  SDw_p1_MS  SDw_p2_MS\n'  )
    if self.ID_CentralValues:
      for Index in self.ID_CentralValues[ 'p1' ]:
        # Central Values
        p1_ID = self.ID_CentralValues[ 'p1' ][ Index ]
        p2_ID = self.ID_CentralValues[ 'p2' ][ Index ]
        # Max systematic variations - central values
        p1_ID_Up = self.ID_SystematicsUp[ 'p1' ][ Index ] - p1_ID
        p2_ID_Up = self.ID_SystematicsUp[ 'p2' ][ Index ] - p2_ID
        # Central values - min systematic variations
        p1_ID_Down = p1_ID - self.ID_SystematicsDown[ 'p1' ][ Index ]
        p2_ID_Down = p2_ID - self.ID_SystematicsDown[ 'p2' ][ Index ]
        for Type in [ 'Large', 'Small' ]:
          if self.MS_CentralValues[ Type ]:
            # Central Values
            p0_MS = self.MS_CentralValues[ Type ][ 'p0' ][ Index ]
            p1_MS = self.MS_CentralValues[ Type ][ 'p1' ][ Index ]
            p2_MS = self.MS_CentralValues[ Type ][ 'p2' ][ Index ]
            # Max systematic variations - central values
            p0_MS_Up = self.MS_SystematicsUp[ Type ][ 'p0' ][ Index ] - p0_MS 
            p1_MS_Up = self.MS_SystematicsUp[ Type ][ 'p1' ][ Index ] - p1_MS 
            p2_MS_Up = self.MS_SystematicsUp[ Type ][ 'p2' ][ Index ] - p2_MS 
            # Central values - min systematic variations
            p0_MS_Down = p0_MS - self.MS_SystematicsDown[ Type ][ 'p0' ][ Index ]
            p1_MS_Down = p1_MS - self.MS_SystematicsDown[ Type ][ 'p1' ][ Index ]
            p2_MS_Down = p2_MS - self.MS_SystematicsDown[ Type ][ 'p2' ][ Index ]
          else:
            p0_MS = 0 
            p1_MS = 0
            p2_MS = 0
            p0_MS_Up = 0
            p1_MS_Up = 0
            p2_MS_Up = 0
            p0_MS_Down = 0
            p1_MS_Down = 0
            p2_MS_Down = 0
          File.write( '%f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f\n' % ( p1_ID, p2_ID, 0, p0_MS, p1_MS, p2_MS, p1_ID_Up, p2_ID_Up, 0, p0_MS_Up, p1_MS_Up, p2_MS_Up, p1_ID_Down, p2_ID_Down, 0, p0_MS_Down, p1_MS_Down, p2_MS_Down ) )
    File.close()
    FormatFile( 'Smearing_%s.dat' % name )

    #File = open( 'Regions_%s.dat' % name, 'w' )
    #File.write( 'eta_min eta_max sec_min sec_max name\n' )
    #for Index, Line in enumerate( open( self.CV_MS_Job.Regions ).readlines() ):
    #  if Index == 0:
    #    continue
    #  File.write( Line )
    #File.close()


  # --- # --- # --- # --- #
  def Add ( self, title, name ):
    job = GetJobInfo( name )
    if job.Detector == 'ID':
      self.Syst_ID_Jobs[ title ] = job
    else:
      self.Syst_MS_Jobs[ title ] = job

  # --- # --- # --- # --- #
  def PrintPlots ( self ):
    self.Check()
    # ID
    for Var in [ 's1', 'p1', 'p2' ]:
      SystsForPlot = {}
      for Key in self.Syst_ID_Jobs:
        SystsForPlot[ Key ] = self.GetGraphs( self.Syst_ID_Jobs[ Key ], Var, False )
      self.SplitPlot( self.GetGraphs( self.CV_ID_Job, Var, True ), SystsForPlot, Var )
      self.MergedPlot( self.GetGraphs( self.CV_ID_Job, Var, True ), SystsForPlot, Var )
    # MS
    for Var in [ 's0', 's1', 'p0', 'p1', 'p2' ]:
      for Type in [ 'Small', 'Large' ]:
        SystsForPlot = {}
        for Key in self.Syst_MS_Jobs:
          SystsForPlot[ Key ] = self.GetGraphs( self.Syst_MS_Jobs[ Key ], Var, False, Type )
        self.SplitPlot( self.GetGraphs( self.CV_MS_Job, Var, True, Type ), SystsForPlot, Var, Type )
        self.MergedPlot( self.GetGraphs( self.CV_MS_Job, Var, True, Type ), SystsForPlot, Var, Type )

  def SplitPlot ( self, Nominal, Systematics, Var, Type = '' ):
    Min = GetMin( Nominal )
    Max = GetMax( Nominal )
    for Key in Systematics:
      Min = min( Min, GetMin( Systematics[ Key ] ) )
      Max = max( Max, GetMax( Systematics[ Key ] ) )
    sfMin = 0.8
    if Min < 0:
      sfMin = 1.2
    sfMax = 1.2
    if Max < 0:
      sfMax = 0.8
    Nominal.SetMinimum( Min * sfMin )
    Nominal.SetMaximum( Max * sfMax )
    if Type == '':
      Nominal.SetMinimum( self.IDRanges[ Var ][ 0 ] )
      Nominal.SetMaximum( self.IDRanges[ Var ][ 1 ] )
    else:
      Nominal.SetMinimum( self.MSRanges[ Var ][ 0 ] )
      Nominal.SetMaximum( self.MSRanges[ Var ][ 1 ] )
    Colors = [ ROOT.kRed + 1, ROOT.kAzure + 2, ROOT.kOrange + 7, ROOT.kSpring + 5, ROOT.kViolet, ROOT.kPink - 5, ROOT.kGray + 1, ROOT.kSpring - 5, ROOT.kAzure + 3 ]
    theJob = self.CV_ID_Job
    String = 'ID'
    if Type != '':
      theJob = self.CV_MS_Job
      String = 'MS'
    Can = ROOT.TCanvas( '%s___%s%s___SplitPlot' % ( theJob.Name, Var, Type ), '', 800, 800 ) 
    Nominal.SetLineWidth( 2 )
    Nominal.SetLineColor( ROOT.kBlack )
    Nominal.SetMarkerColor( ROOT.kBlack )
    Nominal.SetMarkerStyle( 20 )
    Nominal.SetLineStyle( 1 )
    Nominal.Draw( 'ap' )
    #Nominal.GetXaxis().SetLimits( self.BinObjects[ Key ].Mins[ 0 ], self.BinObjects[ Key ].Maxs[ -1 ] )
    Nominal.GetXaxis().SetTitle( '#eta' )
    Nominal.GetYaxis().SetTitle( PlottingTools.Convert( Var, String ) ) 
    Nominal.Draw( 'p' )
    Leg = ROOT.TLegend( 0.55, 0.9, 0.9, 0.75 )
    Leg.SetFillStyle( 0 )
    Leg.SetBorderSize( 0 )
    Leg.SetTextFont( 42 )
    Leg.SetTextSize( 0.025 )
    Leg.SetNColumns( 2 )
    Leg.AddEntry( Nominal, 'Central Value', 'pe' )
    #Leg.AddEntry( None, '', '' )
    for Index, Key in enumerate( Systematics ):
      Systematics[ Key ].SetLineWidth( 2 )
      Systematics[ Key ].SetLineColor( Colors[ Index ] )
      Systematics[ Key ].SetMarkerColor( Colors[ Index ] )
      Systematics[ Key ].SetMarkerStyle( 20 )
      Systematics[ Key ].SetLineStyle( 1 )
      Systematics[ Key ].Draw( 'p' )
      Leg.AddEntry( Systematics[ Key ], Key, 'pe' )
    Leg.Draw()
    Lat = ROOT.TLatex()
    Lat.SetNDC()
    Lat.SetTextFont( 42 )
    Lat.SetTextSize( 0.035 )
    Lat.SetTextColor( 1 )
    Lat.SetTextAlign( 12 )
    Lat.DrawLatex( 0.2, 0.9, '#bf{#it{ATLAS}} Internal' ) 
    Lat.SetTextSize( 0.03 )
    if Type != '':
      #Lat.DrawLatex( 0.2, 0.85, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % self.CV_MS_Job.Stat ) 
      Lat.DrawLatex( 0.2, 0.85, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % 2.7 ) 
    else:
      Lat.DrawLatex( 0.2, 0.85, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % self.CV_ID_Job.Stat ) 
    Lat.SetTextSize( 0.025 )
    Can.SaveAs( '.pdf' )

  def MergedPlot ( self, Nominal, Systematics, Var, Type = '' ):
    #############
    illustrate = False 
    #############
    x = array.array('d', [-2.45, -2.15, -1.85, -1.6, -1.375, -1.15, -0.925, -0.6, -0.2, 0.2, 0.6, 0.925, 1.15, 1.375, 1.6, 1.85, 2.15, 2.45])
    xerr = array.array('d', [0.15, 0.15, 0.15, 0.1, 0.125, 0.1, 0.125, 0.2, 0.2, 0.2, 0.2, 0.125, 0.1, 0.125, 0.1, 0.15, 0.15, 0.15])
    y = None
    yerrup = None
    yerrlow = None
    if Type != '' and Var == 'p2':
      if Type == 'Small':
        y = array.array('d',[ 0.000085, 0.000085, 0.000215, 0.000215, 0.000187, 0.000187, 0.000074, 0.000074, 0.000074, 0.000074, 0.000074, 0.000074, 0.000187, 0.000187, 0.000215, 0.000215, 0.000085, 0.000085 ]) 
        yerrup = array.array('d',[ 0.000010, 0.000010, 0.000018, 0.000018, 0.000044, 0.000044, 0.000006, 0.000006, 0.000006, 0.000006, 0.000006, 0.000006, 0.000044, 0.000044, 0.000018, 0.000018, 0.000010, 0.000010 ]) 
        yerrlow = array.array('d',[ 0.000008, 0.000008, 0.000018, 0.000018, 0.000033, 0.000033, 0.000006, 0.000006, 0.000006, 0.000006, 0.000006, 0.000006, 0.000033, 0.000033, 0.000018, 0.000018, 0.000008, 0.000008 ]) 
      elif Type == 'Large':
        y = array.array('d',[ 0.000103, 0.000103, 0.000114, 0.000114, 0.000196, 0.000196, 0.000118, 0.000118, 0.000118, 0.000118, 0.000118, 0.000118, 0.000196, 0.000196, 0.000114, 0.000114, 0.000103, 0.000103 ]) 
        yerrup = array.array('d',[ 0.000009, 0.000009, 0.000009, 0.000009, 0.000021, 0.000021, 0.000005, 0.000005, 0.000005, 0.000005, 0.000005, 0.000005, 0.000021, 0.000021, 0.000009, 0.000009, 0.000009, 0.000009 ]) 
        yerrlow = array.array('d',[ 0.000009, 0.000009, 0.000009, 0.000009, 0.000021, 0.000021, 0.000005, 0.000005, 0.000005, 0.000005, 0.000005, 0.000005, 0.000021, 0.000021, 0.000009, 0.000009, 0.000009, 0.000009 ]) 
    #############
    Band = GetBand( Nominal, Systematics )
    theJob = self.CV_ID_Job
    String = 'ID'
    if Type != '':
      theJob = self.CV_MS_Job
      String = 'MS'
    Can = ROOT.TCanvas( '%s___%s%s___MergedPlot' % ( theJob.Name, Var, Type ), '', 800, 800 ) 
    Nominal.SetLineWidth( 3 )
    Nominal.SetLineColor( ROOT.kBlack )
    Nominal.SetMarkerColor( ROOT.kBlack )
    Nominal.SetMarkerStyle( 21 )
    Nominal.SetLineStyle( 1 )
    if Type == '':
      Nominal.SetMinimum( self.IDRanges[ Var ][ 0 ] )
      Nominal.SetMaximum( self.IDRanges[ Var ][ 1 ] )
    else:
      Nominal.SetMinimum( self.MSRanges[ Var ][ 0 ] )
      Nominal.SetMaximum( self.MSRanges[ Var ][ 1 ] )
    Nominal.Draw( 'ap' )
    Nominal.GetXaxis().SetTitle( '#eta' )
    Nominal.GetYaxis().SetTitle( PlottingTools.Convert( Var, String ) ) 
    Band.SetLineWidth( 1 )
    color = ROOT.kSpring - 1 
    if Type == 'Large':
      color = ROOT.kRed + 1
    if Type == 'Small':
      color = ROOT.kAzure + 2 
    Band.SetFillColor( color )
    Band.SetLineColor( color )
    Band.SetFillStyle( 3244 )
    #if y:
    #  if illustrate:
    #    PF = ROOT.TGraphAsymmErrors( 18, x, y, xerr, xerr, yerrlow, yerrup )
    #    PF.SetMarkerColor( ROOT.kBlack )
    #    PF.SetMarkerStyle( 23 )
    #    PF.SetFillStyle( 3244 )
    #    PF.Draw( 'p2' )
    #  else: 
    #    PF = ROOT.TGraphAsymmErrors( 18, x, y, xerr, xerr, yerrlow, yerrup )
    #    ExpandBand( Band, PF )
    Band.Draw( '2same' )
    Nominal.Draw( 'p' )
    Lat = ROOT.TLatex()
    Lat.SetNDC()
    Lat.SetTextFont( 42 )
    Lat.SetTextSize( 0.035 )
    Lat.SetTextColor( 1 )
    Lat.SetTextAlign( 12 )
    Lat.DrawLatex( 0.2, 0.9, '#bf{#it{ATLAS}} Internal' ) 
    Lat.SetTextSize( 0.03 )
    if Type != '':
      #Lat.DrawLatex( 0.2, 0.85, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % self.CV_MS_Job.Stat ) 
      Lat.DrawLatex( 0.2, 0.85, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % 2.7 ) 
    else:
      Lat.DrawLatex( 0.2, 0.85, '#sqrt{s} = 13 TeV  #lower[-0.15]{#scale[0.6]{#int}}Ldt = %.1f fb^{-1}' % self.CV_ID_Job.Stat ) 
    Lat.SetTextSize( 0.035 )
    if Type != '':
      Lat.SetTextColor( color )
      Lat.DrawLatex( 0.7, 0.9, '%s Sectors' % Type ) 
    Can.SaveAs( '.pdf' )
    if Type == '':
      self.ID_CentralValues[ Var ] = {} 
      self.ID_SystematicsUp[ Var ] = {} 
      self.ID_SystematicsDown[ Var ] = {} 
    else: 
      self.MS_CentralValues[ Type ][ Var ] = {} 
      self.MS_SystematicsUp[ Type ][ Var ] = {} 
      self.MS_SystematicsDown[ Type ][ Var ] = {} 
    for Bin in range( Band.GetN() ):
      X, Y = ROOT.Double( 0. ), ROOT.Double( 0. )
      Band.GetPoint( Bin, X, Y )
      if Type == '':
        self.ID_CentralValues[ Var ][ Bin ] = Y
        self.ID_SystematicsUp[ Var ][ Bin ] = Y + Band.GetErrorYhigh( Bin )
        self.ID_SystematicsDown[ Var ][ Bin ] = Y - Band.GetErrorYlow( Bin )
      else:
        self.MS_CentralValues[ Type ][ Var ][ Bin ] = Y
        self.MS_SystematicsUp[ Type ][ Var ][ Bin ] = Y + Band.GetErrorYhigh( Bin )
        self.MS_SystematicsDown[ Type ][ Var ][ Bin ] = Y - Band.GetErrorYlow( Bin )

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

  # --- # --- # --- # --- #
  def Check ( self ):
    if self.Checked:
      return
    else:
      # Central Value Job Consistency (ID)
      if self.CV_ID_Job.Detector != 'ID':
        sys.exit( 0 )
      # Central Value Job Consistency (MS)
      if self.CV_MS_Job:
        if self.CV_MS_Job.Detector != 'MS':
          sys.exit( 0 )
      # Regions Consistency (ID)
      for SystKey in self.Syst_ID_Jobs:
        Syst = self.Syst_ID_Jobs[ SystKey ] 
        if Syst.Regions != self.CV_ID_Job.Regions:
          print ' ... ERROR ... Regions used in cv job (%s) and syst variation (%s) don\'t match' % ( self.CV_ID_Job.Name, Syst.Name )
          sys.exit( 0 )
      # Regions Consistency (MS)
      if self.CV_MS_Job:
        for SystKey in self.Syst_MS_Jobs:
          Syst = self.Syst_MS_Jobs[ SystKey ] 
          if Syst.Regions != self.CV_MS_Job.Regions:
            print ' ... ERROR ... Regions used in cv job (%s) and syst variation (%s) don\'t match' % ( self.CV_MS_Job.Name, Syst.Name )
            sys.exit( 0 )
      # Regions Consistency (ID/MS)
      '''
      if self.CV_MS_Job:
        if ( self.CV_ID_Job.GetEtaSet() != self.CV_MS_Job.GetEtaSet() ):
          print ' ... ERROR ... Regions used in ID (%s) and MS (%s) don\'t match' % ( self.CV_ID_Job.Name, self.CV_MS_Job.Name )
          sys.exit( 0 )
      '''
    self.Checked = True
    
def GetJobInfo ( job_name ):
  tempJob = None
  if job_name != '':
    with open( '%s/AutoSave-%s.pkl' % ( job_name, job_name ), 'rb' ) as IN:
      tempJob = pickle.load( IN )
  return tempJob

def GetBand ( nom, systs ):
  band = nom.Clone( '%sband' % nom.GetName() )
  for bin in range( nom.GetN() ):
    X, Y = ROOT.Double( 0. ), ROOT.Double( 0. )
    nom.GetPoint( bin, X, Y )
    #print nom.GetErrorYhigh( bin ), nom.GetErrorYlow( bin )
    safety_high = nom.GetErrorYhigh( bin )
    safety_low = nom.GetErrorYlow( bin )
    if safety_high > 10000:
      safety_high = 0
    if safety_low > 10000:
      safety_low = 0
    tot_up = math.pow( safety_high, 2 )
    tot_low = math.pow( safety_low, 2 )
    for key in systs:
      syst = systs[ key ]
      Xp, Yp = ROOT.Double( 0. ), ROOT.Double( 0. )
      syst.GetPoint( bin, Xp, Yp )
      if Y < Yp:
        tot_up += math.pow( Yp - Y, 2 ) 
      else: 
        tot_low += math.pow( Yp - Y, 2 ) 
    band.SetPointEYlow( bin, math.sqrt( tot_low ) )
    band.SetPointEYhigh( bin, math.sqrt( tot_up ) )
  return band

def ExpandBand ( band, add ):
  for bin in range( band.GetN() ):
    X, Y = ROOT.Double( 0. ), ROOT.Double( 0. )
    Xa, Ya = ROOT.Double( 0. ), ROOT.Double( 0. )
    band.GetPoint( bin, X, Y )
    add.GetPoint( bin, Xa, Ya )
    err_low = band.GetErrorYlow( bin )
    err_up = band.GetErrorYhigh( bin )
    if ( Y - Ya ) > err_low and ( Y - Ya ) > 0:
      band.SetPointEYlow( bin, Y - Ya )
    elif ( Ya - Y ) > err_up and ( Ya - Y ) > 0:
      band.SetPointEYhigh( bin, Ya - Y )
  return band

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

def FormatFile( filename ):
  Command = 'column -t %s > temp' % filename
  #process = subprocess.Popen( Command.split(), stdout = subprocess.PIPE )
  #output = process.communicate()[ 0 ]
  os.system( Command )
  os.rename( 'temp', filename )

def GetVariablePosition( var ):
  if var == 's0':
    return 0
  if var == 's1':
    return 3
  if var == 'p0':
    return 6
  if var == 'p1':
    return 9
  if var == 'p2':
    return 12

def GetConversion( det, var ):
  if det == 'ID':
    if var == 's1':
      return 1000.
    if var == 'p2':
      return 1000.
  else:
    if var == 's1':
      return 1000.
    if var == 'p2':
      return 1000.
  return 1.
