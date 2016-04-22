# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
#=#=#                                                   #=#=#
#=#=#  Example.py : an example of python script to run  #=#=#
#=#=#               the MuonCalibrationFit package      #=#=#
#=#=#                                                   #=#=#
#=#=#  Author : Giacomo Artoni <giacomo.artoni@cern.ch  #=#=#
#=#=#                                                   #=#=#
#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#

import os, sys
import Code.JobUtils as JobUtils 

def main():

  #=#=#  Setting main working directory
  CurrentDir = '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit'


  #=#=#
  #=#=#=#=#  First Iteration: Job1

  #=#=#  Setup the job name + the type of fit to be performed
  #=#=#    Available options: ID, MS
  Job1 = JobUtils.Job( Name = 'Job1', Detector = 'ID' )

  #=#=#  Select the printout level
  #=#=#    Available options: ERROR, WARNING, *INFO*, DEBUG, VERBOSE
  Job1.Level = 'INFO'

  #=#=#  Select the type of fit to be performed
  #=#=#    Available options: *Chi2*, Likelihood
  Job1.FitType = 'Chi2'

  #=#=#  Select how many times bins should be split to smoothen distribution
  Job1.Splittings = 100

  #=#=#  Select the number of random values used to smoothen MC templates
  Job1.NumRandomValues = 1

  #=#=#  Select the steps to be performed in the fit
  #=#=#    Can choose a combination of algorithms with specified retrials
  #=#=#    ex. Alg1:10+Alg2:3+...
  #=#=#    Available options: *Simplex*, Migrad, SimpleScan
  Job1.RunMode = 'Simplex:20+SimpleScan:1'

  #=#=#  Select the parametrization for the J/psi background
  #=#=#    Available options: *Exponential*, Linear 
  Job1.BkgParametrization = 'Exponential'

  #=#=#  Select the file holding the region definition for this job 
  Job1.Regions = os.path.expandvars( '%s/share/PreTest/RegionsIteration0.txt' % CurrentDir )

  #=#=#  Select the datasets to be used in the fit
  Job1.Files = JobUtils.Files( '%s/data' % CurrentDir, 'MCPCalibNtuple_xAOD_Data.root', 'MCPCalibNtuple_xAOD_MonteCarlo.root', 'MCPCalibNtuple_xAOD_Background.root' ) 

  #=#=#  Specify how much statistics is used (for labelling plots) 
  Job1.Stat = 1 

  #=#=#  Select how many times the fit should be retried in case it does not converge
  Job1.MaxRetrials = 3

  #=#=#  Select the chi2 probability to be used when determining if a fit was successful (only for chi2 fits) 
  Job1.ProbThreshold = 0.3

  #=#=#  Select how many bins are needed in the J/psi mass templates 
  Job1.JpsiMassBins = 100

  #=#=#  Select pt binning for the J/psi mass templates 
  Job1.JpsiMassPts = '6 50'

  #=#=#  Select how many bins are needed in the Z mass templates 
  Job1.ZMassBins = 100

  #=#=#  Select pt binning for the Z mass templates 
  Job1.ZMassPts = '6 1000'

  #=#=#  Choose if s0 should be fit
  #=#=#    note: always False for ID fit
  Job1.Fit_s0 = True

  #=#=#  Choose if s1 should be fit
  Job1.Fit_s1 = True

  #=#=#  Choose if p0 should be fit
  #=#=#    note: always False for ID fit
  Job1.Fit_p0 = True

  #=#=#  Choose if p1 should be fit
  Job1.Fit_p1 = True

  #=#=#  Choose if p2 should be fit
  Job1.Fit_p2 = True

  #=#=#  Choose how many cores to use
  #=#=#    default: 8 
  Job1.Cores = 8

  #=#=#  Run the job 
  Job1.Run() 

  return


  #=#=#
  #=#=#=#=#  Second Iteration: Job2

  Job2 = JobUtils.Job( Name = 'Job2', Detector = 'ID' )

  #=#=#  Choose which job should be used as a starting point 
  #=#=#    copying all options from previous job!
  Job2.StartFrom( 'Job1' )
  Job2.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  Job2.Run() 




  #=#=#
  #=#=#=#=#  Third Iteration: Job3

  Job3 = JobUtils.Job( Name = 'Job3', Detector = 'ID' )
  Job3.StartFrom( 'Job2' )
  Job3.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  Job3.Run() 




  #=#=#
  #=#=#=#=#  Fourth Iteration: Job4

  Job4 = JobUtils.Job( Name = 'Job4', Detector = 'MS' )
  Job4.StartFrom( 'Job3' )
  Job4.Level = 'DEBUG'
  Job4.RunMode = 'Simplex:20+SimpleScan:1'

  #=#=#  Select how many bins are needed in the Z rho templates 
  Job4.ZRhoBins = 100

  #=#=#  Select pt binning for the Z rho templates 
  Job4.ZRhoPts = '6 1000'

  Job4.Run() 




  #=#=#
  #=#=#=#=#  Fifth Iteration: Job5

  Job5 = JobUtils.Job( 'Job5', Detector = 'MS' )
  Job5.StartFrom( 'Job4' )
  Job5.RunMode = 'Simplex:30+Migrad:1+SimpleScan:1'
  Job5.Run() 




if __name__ == '__main__':
  sys.exit( main() )
