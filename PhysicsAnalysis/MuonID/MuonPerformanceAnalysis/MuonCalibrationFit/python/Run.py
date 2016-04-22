# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys
import Code.JobUtils as JobUtils 

def main():

  CurrentDir = '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit'
  
  #Parallel_ID_Job01 = JobUtils.Job( Name = 'Parallel_ID_Job01', Detector = 'ID' )
  #Parallel_ID_Job01.Level = 'INFO'
  #Parallel_ID_Job01.FitType = 'Likelihood'
  #Parallel_ID_Job01.Splittings = 1
  #Parallel_ID_Job01.NumRandomValues = 1
  #Parallel_ID_Job01.RunMode = 'InitScan:1+Simplex:1'
  #Parallel_ID_Job01.MaxRetrials = 1
  ##Parallel_ID_Job01.Regions = os.path.expandvars( '%s/share/OrderedRegionsIDAbsEtaEight.txt' % CurrentDir )
  #Parallel_ID_Job01.Regions = os.path.expandvars( '%s/share/OrderedRegionsID.txt' % CurrentDir )
  #Parallel_ID_Job01.Files = JobUtils.Files( '%s/data/25ns' % CurrentDir, 'Data2015.root', 'MonteCarlo.root', 'Background.root' ) 
  #Parallel_ID_Job01.GRL = os.path.expandvars( '%s/grl/data15_13TeV.periodAllYear_DetStatus-v73-pro19-08_DQDefects-00-01-02_PHYS_StandardGRL_All_Good_25ns.xml' % CurrentDir )
  #Parallel_ID_Job01.Stat = 3.2 
  #Parallel_ID_Job01.JpsiMassPts = '5 7 20'
  #Parallel_ID_Job01.ZMassPts = '22 47 300'
  #Parallel_ID_Job01.JpsiRewFile = os.path.expandvars( '%s/reweighting/JpsiRew.root' % CurrentDir )
  #Parallel_ID_Job01.ZRewFile = os.path.expandvars( '%s/reweighting/ZRew.root' % CurrentDir )
  #Parallel_ID_Job01.Fit_s0 = False
  #Parallel_ID_Job01.Fit_s1 = True
  #Parallel_ID_Job01.Fit_p0 = False
  #Parallel_ID_Job01.Fit_p1 = True
  #Parallel_ID_Job01.Fit_p2 = True
  #Parallel_ID_Job01.RunningSystem = 'PBS'
  #Parallel_ID_Job01.RegionHandling = 'Parallel'
  #Parallel_ID_Job01.JobsPerRegion = 10
  #Parallel_ID_Job01.Run() 

  for Index in [ 2, 3, 4 ]:
  #for Index in [ 2, 3, 4, 5, 6 ]:
  #for Index in [ 8, 9, 10 ]:
    locals()[ 'Parallel_ID_Job0%d' % Index ] = JobUtils.Job( Name = 'Parallel_ID_Job0%d' % Index, Detector = 'ID' )
    locals()[ 'Parallel_ID_Job0%d' % Index ].StartFrom( 'Parallel_ID_Job0%d' % ( Index - 1 ) )
    locals()[ 'Parallel_ID_Job0%d' % Index ].Splittings = 1000
    locals()[ 'Parallel_ID_Job0%d' % Index ].Files = JobUtils.Files( '%s/data/25ns' % CurrentDir, 'Data2015.root.new', 'MonteCarlo.root.new.2', 'Background.root.new' ) 
    locals()[ 'Parallel_ID_Job0%d' % Index ].JobsPerRegion = 15
    locals()[ 'Parallel_ID_Job0%d' % Index ].Run()

  #Parallel_ID_Job02 = JobUtils.Job( Name = 'Parallel_ID_Job02', Detector = 'ID' )
  #Parallel_ID_Job02.StartFrom( 'Parallel_ID_Job01' )
  #Parallel_ID_Job02.Regions = os.path.expandvars( '%s/share/OrderedRegionsID.txt' % CurrentDir )
  #Parallel_ID_Job02.Run()

  #Parallel_ID_Job03 = JobUtils.Job( Name = 'Parallel_ID_Job03', Detector = 'ID' )
  #Parallel_ID_Job03.StartFrom( 'Parallel_ID_Job02' )
  #Parallel_ID_Job03.Run()

  #Parallel_ID_Job04 = JobUtils.Job( Name = 'Parallel_ID_Job04', Detector = 'ID' )
  #Parallel_ID_Job04.StartFrom( 'Parallel_ID_Job03' )
  #Parallel_ID_Job04.Run()

if __name__ == '__main__':
  sys.exit( main() )
