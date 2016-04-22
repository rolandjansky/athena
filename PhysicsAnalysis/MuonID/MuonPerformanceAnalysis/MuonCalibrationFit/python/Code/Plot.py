# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, re, sys, ROOT
import AtlasStyle
import Objects
import PlottingTools
import FitResult

def OverallPlot( Input, Detector, Stat ):

  ##### General commands #####
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = 3000

  ##### Opening file and read all contents #####
  InputFile = ROOT.TFile( Input )
  Keys = ROOT.gDirectory.GetListOfKeys() 

  ToBeUsed = []
  Iterations = set()
  GlobIters = []

  for Key in Keys:
    TheName = Key.GetName()
    if 'TVectorT<double>' in Key.GetClassName() and not 'NoCorrections' in TheName and not 'InitialCorrections' in TheName:
      FitRes = FitResult.FitResult( TheName, InputFile )
      Iterations.add( ( FitRes.Iter, FitRes.RegFile ) )
      ToBeUsed.append( FitRes )

  for Iter, File in Iterations:
    GlobIters.append( FitResult.IterOverview( Iter, File ) )

  AllPlotsDone = []
  for GlobIter in GlobIters:
    for OvPlot, NoErrPlot in GlobIter.DoPlot( ToBeUsed, Detector, Stat ):
      AllPlotsDone.append( os.path.abspath( OvPlot ) )
      AllPlotsDone.append( os.path.abspath( NoErrPlot ) )
  return AllPlotsDone


def ScanPlot( Input, Detector, Stat ):

  ##### General commands #####
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = 3000

  ##### Opening file and read all contents #####
  MyObjects = []
  AllTypes = { 'Regions': set() }
  InputFile = ROOT.TFile( Input )
  Keys = ROOT.gDirectory.GetListOfKeys() 
  Reg = re.compile( 'tree_(\w+)' ) 
  for Key in Keys:
    Match = Reg.search( Key.GetName() )
    if not Match:
      continue
    Region = Match.group( 1 )
    AllTypes[ 'Regions' ].add( Region )
    FittedPars = InputFile.Get( 'PostScanFit___%s' % Region )
    if FittedPars:
      MyObjects.append( Objects.ScanObject( InputFile.Get( 'PostScanFit___%s' % Region ), InputFile.Get( Key.GetName() ), Region, InputFile.Get( Region ).GetTitle() ) )

  AllPlotsDone = []
  for MyObject in MyObjects:
    for Scan in PlottingTools.DoScanPlot( MyObject, Detector, Stat ):
      AllPlotsDone.append( os.path.abspath( Scan ) )
  return AllPlotsDone


def BkgPlot( Input, Detector, Stat ):

  ##### General commands #####
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = 3000

  ##### Opening file and read all contents #####
  MyObjects = []
  AllTypes = { 'Regions': set(), 'VarPtBins': set() }
  InputFile = ROOT.TFile( Input )
  Keys = ROOT.gDirectory.GetListOfKeys() 
  Reg = re.compile( '(\w+)~BkgFit~(\w+)~(\w+)' ) 
  for Key in Keys:
    Match = Reg.search( Key.GetName() )
    if not Match:
      continue
    #print Key.GetName()
    Region = Match.group( 1 )
    Variable = Match.group( 2 )
    PtBin = Match.group( 3 )
    AllTypes[ 'Regions' ].add( Region )
    AllTypes[ 'VarPtBins' ].add( ( Variable, PtBin ) )
    MyObjects.append( Objects.BkgObject( InputFile.Get( Key.GetName() ), Region, InputFile.Get( Region ).GetTitle(), Variable, PtBin ) )

  AllPlotsDone = []
  for MyObject in MyObjects:
    for path in PlottingTools.DoBackgroundPlot( MyObject, Detector, Stat ):
      AllPlotsDone.append( os.path.abspath( path ) )
  return AllPlotsDone


def Plot( Input, Detector, Stat, IsCheck ):

  ##### General commands #####
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = 3000

  ##### Opening file and read all contents #####
  MyObjects = []
  AllTypes = { 'FitLevels': set(), 'Regions': set(), 'VarPtBins': set(), 'DataTypes': set() }
  InputFile = ROOT.TFile( Input )
  Keys = ROOT.gDirectory.GetListOfKeys() 
  Reg = re.compile( '(\w+)~(\w+)~(\w+)~(\w+)~(.+)' ) 
  for Key in Keys:
    Match = Reg.search( Key.GetName() )
    if not Match:
      continue
    FitLevel = Match.group( 1 )
    Region = Match.group( 2 )
    Variable = Match.group( 3 )
    DataType = Match.group( 4 )
    PtBin = Match.group( 5 )
    if 'TempFit' in FitLevel:
      continue
    if 'RoF' in Variable:
      continue
    AllTypes[ 'FitLevels' ].add( FitLevel )
    AllTypes[ 'Regions' ].add( Region )
    AllTypes[ 'VarPtBins' ].add( ( Variable, PtBin ) )
    AllTypes[ 'DataTypes' ].add( DataType )
    MyObjects.append( Objects.PlotObject( InputFile.Get( Key.GetName() ), FitLevel, Region, InputFile.Get( Region ).GetTitle(), Variable, DataType, PtBin, InputFile.Get( FitLevel + '___' + Region ) ) )

  AllPlotsDone = []

  ##### Dumping single sets #####
  for FitLevel in AllTypes[ 'FitLevels' ]:
    for Region in AllTypes[ 'Regions' ]:
      for VarPtBin in AllTypes[ 'VarPtBins' ]:
        NeededObjects = []
        for MyObject in MyObjects:
          if not MyObject.FitLevel == FitLevel:
            continue
          if not MyObject.Region == Region:
            continue
          if not ( MyObject.Variable, MyObject.PtBin ) == VarPtBin:
            continue
          NeededObjects.append( MyObject )
        if NeededObjects:
          AllPlotsDone.append( os.path.abspath( PlottingTools.DoGeneralPlot( NeededObjects, Detector, Stat, IsCheck ) ) )
  return AllPlotsDone

def PlotDistr( Input, Detector, Stat ):

  ##### General commands #####
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = 3000

  ##### Opening file and read all contents #####
  MyObjects = []
  AllTypes = { 'FitLevels': set(), 'Regions': set(), 'VarPtBins': set(), 'DataTypes': set() }
  InputFile = ROOT.TFile( Input )
  Keys = ROOT.gDirectory.GetListOfKeys() 
  Reg = re.compile( '(\w+)~(\w+)~(\w+)~(\w+)~(.+)' ) 
  for Key in Keys:
    Match = Reg.search( Key.GetName() )
    if not Match:
      continue
    FitLevel = Match.group( 1 )
    Region = Match.group( 2 )
    Variable = Match.group( 3 )
    DataType = Match.group( 4 )
    PtBin = Match.group( 5 )
    if 'RoF' in Variable and FitLevel == 'NoCorrections':
      AllTypes[ 'FitLevels' ].add( FitLevel )
      AllTypes[ 'Regions' ].add( Region )
      AllTypes[ 'VarPtBins' ].add( ( Variable, PtBin ) )
      AllTypes[ 'DataTypes' ].add( DataType )
      NormalizationHistogram = None
      if not 'Data' in DataType:
        if 'Jpsi' in Variable: 
          NormalizationHistogram = InputFile.Get( '%s~%s~%s~%s~%s' % ( FitLevel, Region, 'MassJpsi', DataType, PtBin ) )
        else:
          NormalizationHistogram = InputFile.Get( '%s~%s~%s~%s~%s' % ( FitLevel, Region, 'MassZ', DataType, PtBin ) )
      MyObjects.append( Objects.PlotObject( InputFile.Get( Key.GetName() ), FitLevel, Region, InputFile.Get( Region ).GetTitle(), Variable, DataType, PtBin, InputFile.Get( FitLevel + '___' + Region ), NormalizationHistogram ) ) 

  AllPlotsDone = []

  ##### Dumping single sets #####
  for FitLevel in AllTypes[ 'FitLevels' ]:
    for Region in AllTypes[ 'Regions' ]:
      for VarPtBin in AllTypes[ 'VarPtBins' ]:
        NeededObjects = []
        for MyObject in MyObjects:
          if not MyObject.FitLevel == FitLevel:
            continue
          if not MyObject.Region == Region:
            continue
          if not ( MyObject.Variable, MyObject.PtBin ) == VarPtBin:
            continue
          NeededObjects.append( MyObject )
        if NeededObjects:
          AllPlotsDone.append( os.path.abspath( PlottingTools.DoDistrPlot( NeededObjects, Detector, Stat ) ) )
  return AllPlotsDone
