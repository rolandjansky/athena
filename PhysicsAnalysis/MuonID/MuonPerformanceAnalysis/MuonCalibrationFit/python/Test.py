# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys
import Code.JobUtils as JobUtils 

def main():

  ##########################
  ###  Setting Up Job  ###
  ######################
 
  ##########################
  ###   Begin 1 fb-1   ###
  ######################

  #rTest1InvFb_1 = JobUtils.Job( 'rTest1InvFb_1' )
  #rTest1InvFb_1.Level = 'INFO'
  #rTest1InvFb_1.Detector = 'ID'
  #rTest1InvFb_1.FitType = 'Chi2'
  #rTest1InvFb_1.Splittings = 1000
  #rTest1InvFb_1.NumRandomValues = 1
  #rTest1InvFb_1.RunMode = 'Simplex:20+SimpleScan:1'
  #rTest1InvFb_1.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter0Regions.txt' )
  #rTest1InvFb_1.Files = JobUtils.Files( '$TestArea/MuonCalibrationFit/data/Constant/InvFb1', 'Data.root', 'MonteCarlo.root', 'Background.root' ) 
  #rTest1InvFb_1.Stat = 1 
  #rTest1InvFb_1.MaxRetrials = 5
  #rTest1InvFb_1.ProbThreshold = 0.3
  #rTest1InvFb_1.JpsiMassPts = '6 50'
  #rTest1InvFb_1.ZMassPts = '6 1000'
  #rTest1InvFb_1.Run() 

  #rTest1InvFb_2 = JobUtils.Job( 'rTest1InvFb_2' )
  #rTest1InvFb_2.StartFrom( 'rTest1InvFb_1' )
  #rTest1InvFb_2.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter3Regions.txt' )
  #rTest1InvFb_2.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  #rTest1InvFb_2.NumRandomValues = 10
  #rTest1InvFb_2.JpsiMassPts = '6 8 9 11 50'
  #rTest1InvFb_2.ZMassPts = '6 30 40 50 1000'
  #rTest1InvFb_2.Run() 

  #rTest1InvFb_3 = JobUtils.Job( 'rTest1InvFb_3' )
  #rTest1InvFb_3.StartFrom( 'rTest1InvFb_2' )
  #rTest1InvFb_3.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter4Regions.txt' )
  #rTest1InvFb_3.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  #rTest1InvFb_3.NumRandomValues = 50
  #rTest1InvFb_3.JpsiMassPts = '6 8 9 11 50'
  #rTest1InvFb_3.ZMassPts = '6 30 40 50 1000'
  #rTest1InvFb_3.Run() 

  ######################
  ###   End 1 fb-1    ###
  ##########################

  ##########################
  ###   Begin 5 fb-1   ###
  ######################

  #rTest5InvFb_1 = JobUtils.Job( 'rTest5InvFb_1' )
  #rTest5InvFb_1.Level = 'INFO'
  #rTest5InvFb_1.Detector = 'ID'
  #rTest5InvFb_1.FitType = 'Chi2'
  #rTest5InvFb_1.Splittings = 1000
  #rTest5InvFb_1.NumRandomValues = 1
  #rTest5InvFb_1.RunMode = 'Simplex:20+SimpleScan:1'
  #rTest5InvFb_1.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter0Regions.txt' )
  #rTest5InvFb_1.Files = JobUtils.Files( '$TestArea/MuonCalibrationFit/data/Constant/InvFb5', 'Data.root', 'MonteCarlo.root', 'Background.root' ) 
  #rTest5InvFb_1.Stat = 5
  #rTest5InvFb_1.MaxRetrials = 5
  #rTest5InvFb_1.ProbThreshold = 0.3
  #rTest5InvFb_1.JpsiMassPts = '6 50'
  #rTest5InvFb_1.ZMassPts = '6 1000'
  #rTest5InvFb_1.Run() 

  #rTest5InvFb_2 = JobUtils.Job( 'rTest5InvFb_2' )
  #rTest5InvFb_2.StartFrom( 'rTest5InvFb_1' )
  #rTest5InvFb_2.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter3Regions.txt' )
  #rTest5InvFb_2.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  #rTest5InvFb_2.NumRandomValues = 10
  #rTest5InvFb_2.JpsiMassPts = '6 8 9 11 50'
  #rTest5InvFb_2.ZMassPts = '6 30 40 50 1000'
  #rTest5InvFb_2.Run() 

  #rTest5InvFb_3 = JobUtils.Job( 'rTest5InvFb_3' )
  #rTest5InvFb_3.StartFrom( 'rTest5InvFb_2' )
  #rTest5InvFb_3.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter4Regions.txt' )
  #rTest5InvFb_3.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  #rTest5InvFb_3.NumRandomValues = 50
  #rTest5InvFb_3.JpsiMassPts = '6 8 9 11 50'
  #rTest5InvFb_3.ZMassPts = '6 30 40 50 1000'
  #rTest5InvFb_3.Run() 

  ######################
  ###   End 5 fb-1    ###
  ##########################

  ##########################
  ###   Begin 10 fb-1  ###
  ######################

  #rTest10InvFb_1 = JobUtils.Job( 'rTest10InvFb_1' )
  #rTest10InvFb_1.Level = 'INFO'
  #rTest10InvFb_1.Detector = 'ID'
  #rTest10InvFb_1.FitType = 'Chi2'
  #rTest10InvFb_1.Splittings = 1000
  #rTest10InvFb_1.NumRandomValues = 1
  #rTest10InvFb_1.RunMode = 'Simplex:20+SimpleScan:1'
  #rTest10InvFb_1.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter0Regions.txt' )
  #rTest10InvFb_1.Files = JobUtils.Files( '$TestArea/MuonCalibrationFit/data/Constant/InvFb10', 'Data.root', 'MonteCarlo.root', 'Background.root' ) 
  #rTest10InvFb_1.Stat = 10 
  #rTest10InvFb_1.MaxRetrials = 5
  #rTest10InvFb_1.ProbThreshold = 0.3
  #rTest10InvFb_1.JpsiMassPts = '6 50'
  #rTest10InvFb_1.ZMassPts = '6 1000'
  #rTest10InvFb_1.Run() 

  #rTest10InvFb_2 = JobUtils.Job( 'rTest10InvFb_2' )
  #rTest10InvFb_2.StartFrom( 'rTest10InvFb_1' )
  #rTest10InvFb_2.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter3Regions.txt' )
  #rTest10InvFb_2.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  #rTest10InvFb_2.NumRandomValues = 10
  #rTest10InvFb_2.JpsiMassPts = '6 8 9 11 50'
  #rTest10InvFb_2.ZMassPts = '6 30 40 50 1000'
  #rTest10InvFb_2.Run() 

  #rTest10InvFb_3 = JobUtils.Job( 'rTest10InvFb_3' )
  #rTest10InvFb_3.StartFrom( 'rTest10InvFb_2' )
  #rTest10InvFb_3.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter4Regions.txt' )
  #rTest10InvFb_3.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  #rTest10InvFb_3.NumRandomValues = 50
  #rTest10InvFb_3.JpsiMassPts = '6 8 9 11 50'
  #rTest10InvFb_3.ZMassPts = '6 30 40 50 1000'
  #rTest10InvFb_3.Run() 

  ######################
  ###   End 10 fb-1    ###
  ##########################

  ##########################
  ###   Begin 50 fb-1   ###
  ######################

  #rTest50InvFb_1 = JobUtils.Job( 'rTest50InvFb_1' )
  #rTest50InvFb_1.Level = 'INFO'
  #rTest50InvFb_1.Detector = 'ID'
  #rTest50InvFb_1.FitType = 'Chi2'
  #rTest50InvFb_1.Splittings = 1000
  #rTest50InvFb_1.NumRandomValues = 1
  #rTest50InvFb_1.RunMode = 'Simplex:20+SimpleScan:1'
  #rTest50InvFb_1.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter0Regions.txt' )
  #rTest50InvFb_1.Files = JobUtils.Files( '$TestArea/MuonCalibrationFit/data/Constant/InvFb50', 'Data.root', 'MonteCarlo.root', 'Background.root' ) 
  #rTest50InvFb_1.Stat = 50
  #rTest50InvFb_1.MaxRetrials = 5
  #rTest50InvFb_1.ProbThreshold = 0.3
  #rTest50InvFb_1.JpsiMassPts = '6 50'
  #rTest50InvFb_1.ZMassPts = '6 1000'
  #rTest50InvFb_1.Run() 

  #rTest50InvFb_2 = JobUtils.Job( 'rTest50InvFb_2' )
  #rTest50InvFb_2.StartFrom( 'rTest50InvFb_1' )
  #rTest50InvFb_2.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter3Regions.txt' )
  #rTest50InvFb_2.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  #rTest50InvFb_2.NumRandomValues = 10
  #rTest50InvFb_2.JpsiMassPts = '6 8 9 11 50'
  #rTest50InvFb_2.ZMassPts = '6 30 40 50 1000'
  #rTest50InvFb_2.Run() 

  #rTest50InvFb_3 = JobUtils.Job( 'rTest50InvFb_3' )
  #rTest50InvFb_3.StartFrom( 'rTest50InvFb_2' )
  #rTest50InvFb_3.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter4Regions.txt' )
  #rTest50InvFb_3.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  #rTest50InvFb_3.NumRandomValues = 50
  #rTest50InvFb_3.JpsiMassPts = '6 8 9 11 50'
  #rTest50InvFb_3.ZMassPts = '6 30 40 50 1000'
  #rTest50InvFb_3.Run() 

  ######################
  ###   End 50 fb-1    ###
  ##########################

  ##########################
  ###   Begin 100 fb-1   ###
  ######################

  rTest100InvFb_1 = JobUtils.Job( 'rTest100InvFb_1' )
  rTest100InvFb_1.Level = 'INFO'
  rTest100InvFb_1.Detector = 'ID'
  rTest100InvFb_1.FitType = 'Chi2'
  rTest100InvFb_1.Splittings = 1000
  rTest100InvFb_1.NumRandomValues = 1
  rTest100InvFb_1.RunMode = 'Simplex:20+SimpleScan:1'
  rTest100InvFb_1.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter0Regions.txt' )
  rTest100InvFb_1.Files = JobUtils.Files( '$TestArea/MuonCalibrationFit/data/Constant/InvFb100', 'Data.root', 'MonteCarlo.root', 'Background.root' ) 
  rTest100InvFb_1.Stat = 100
  rTest100InvFb_1.MaxRetrials = 5
  rTest100InvFb_1.ProbThreshold = 0.3
  rTest100InvFb_1.JpsiMassPts = '6 50'
  rTest100InvFb_1.ZMassPts = '6 1000'
  rTest100InvFb_1.Run() 

  rTest100InvFb_2 = JobUtils.Job( 'rTest100InvFb_2' )
  rTest100InvFb_2.StartFrom( 'rTest100InvFb_1' )
  rTest100InvFb_2.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter3Regions.txt' )
  rTest100InvFb_2.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  rTest100InvFb_2.NumRandomValues = 10
  rTest100InvFb_2.JpsiMassPts = '6 8 9 11 50'
  rTest100InvFb_2.ZMassPts = '6 30 40 50 1000'
  rTest100InvFb_2.Run() 

  rTest100InvFb_3 = JobUtils.Job( 'rTest100InvFb_3' )
  rTest100InvFb_3.StartFrom( 'rTest100InvFb_2' )
  rTest100InvFb_3.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter4Regions.txt' )
  rTest100InvFb_3.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  rTest100InvFb_3.NumRandomValues = 50
  rTest100InvFb_3.JpsiMassPts = '6 8 9 11 50'
  rTest100InvFb_3.ZMassPts = '6 30 40 50 1000'
  rTest100InvFb_3.Run() 

  ######################
  ###   End 100 fb-1    ###
  ##########################

if __name__ == '__main__':
  sys.exit( main() )
