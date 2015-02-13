# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys
import Code.JobUtils as JobUtils 

def main():

  ##########################
  ###  Setting Up Job  ###
  ######################
  
  #TestJob1 = JobUtils.Job( 'TestJob1' )
  #TestJob1.Level = 'VERBOSE'
  #TestJob1.Detector = 'ID'
  #TestJob1.FitType = 'Chi2'
  #TestJob1.Splittings = 1000
  #TestJob1.NumRandomValues = 10
  #TestJob1.RunMode = 'Simplex:20'
  #TestJob1.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter0Regions.txt' )
  #TestJob1.Files = JobUtils.Files( '$TestArea/MuonCalibrationFit/data/One', 'Data.root', 'MonteCarlo.root', 'Background.root' ) 
  #TestJob1.Stat = 1
  #TestJob1.JpsiMassPts = '6 50'
  #TestJob1.ZMassPts = '6 1000'
  #TestJob1.Run() 

  TestJob2 = JobUtils.Job( 'TestJob2' )
  TestJob2.StartFrom( 'TestJob1' )
  TestJob2.RunMode = 'Simplex:20+Migrad:1'
  TestJob2.NumRandomValues = 1
  TestJob2.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter3Regions.txt' )
  TestJob2.JpsiMassPts = '6 10 50'
  TestJob2.ZMassPts = '6 45 1000'
  TestJob2.Run() 
  '''
  #FunctionsJob2 = JobUtils.Job( 'FunctionsJob2' )
  #FunctionsJob2.StartFrom( 'FunctionsJob1' )
  #FunctionsJob2.RunMode = 'Simplex:20+Migrad:1'
  #FunctionsJob2.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter4Regions.txt' )
  #FunctionsJob2.JpsiMassPts = '6 10 50'
  #FunctionsJob2.ZMassPts = '6 45 1000'
  #FunctionsJob2.Run() 

  #FunctionsJob3 = JobUtils.Job( 'FunctionsJob3retry' )
  #FunctionsJob3.StartFrom( 'FunctionsJob2' )
  #FunctionsJob3.NumRandomValues = 10
  #FunctionsJob3.RunMode = 'Simplex:20+Migrad:1+SimpleScan:20'
  #FunctionsJob3.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter4Regions.txt' )
  #FunctionsJob3.JpsiMassPts = '6 9 12 50'
  #FunctionsJob3.ZMassPts = '6 25 40 50 1000'
  #FunctionsJob3.Run() 

  FunctionsJobEnd = JobUtils.Job( 'FunctionsJobEnd' )
  FunctionsJobEnd.StartFrom( 'FunctionsJobTest' )
  FunctionsJobEnd.RunMode = 'Simplex:20+Migrad:1+SimpleScan:30'
  FunctionsJobEnd.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter5Regions.txt' )
  FunctionsJobEnd.NumRandomValues = 100
  FunctionsJobEnd.Run() 

  #FunctionsJob4 = JobUtils.Job( 'FunctionsJob4' )
  #FunctionsJob4.StartFrom( 'FunctionsJob3' )
  #FunctionsJob4.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter4Regions.txt' )
  #FunctionsJob4.Run() 

  #FunctionsJob5 = JobUtils.Job( 'FunctionsJob5' )
  #FunctionsJob5.StartFrom( 'FunctionsJob4' )
  #FunctionsJob5.NumRandomValues = 50
  #FunctionsJob5.RunMode = 'Simplex:10+Migrad:1+SimpleScan:30'
  #FunctionsJob5.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter4Regions.txt' )
  #FunctionsJob5.Run() 

  #FunctionsJob7 = JobUtils.Job( 'FunctionsJob7' )
  #FunctionsJob7.StartFrom( 'FunctionsJob5' )
  #FunctionsJob7.NumRandomValues = 10
  #FunctionsJob7.RunMode = 'Simplex:10+Migrad:1+SimpleScan:20'
  #FunctionsJob7.Regions = os.path.expandvars( '$TestArea/MuonCalibrationFit/share/Iter5Regions.txt' )
  #FunctionsJob7.Run() 
  '''

if __name__ == '__main__':
  sys.exit( main() )
