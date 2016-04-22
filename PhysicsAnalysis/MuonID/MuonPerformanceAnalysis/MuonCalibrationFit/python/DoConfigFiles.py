# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import ROOT
import Code.FinalJob as FJ

def main():

  ##### General commands #####
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = 3000

  ###-----------------v Final ID Job
  ###----------------------------v Final MS Job
  #job = FJ.FinalJob( 'TrackSelPtBins_It04', 'MS_NoRho_Job11' )
  #job = FJ.FinalJob( 'Parallel_ID_Job01', 'MS_NoRho_Job12' )
  #job = FJ.FinalJob( 'Zero_Job02', 'MS_Early_Job07' )
  #job = FJ.FinalJob( 'Again_ID_Job09', 'MS_Early_Job07' )
  job = FJ.FinalJob( 'FinalID_Job04', 'FinalMS_Job017' )

  ###-------v Systematic Name 
  ###--------------------v Systematic Job Name 
  job.Add( 'Min Pt',                   'FinalID_Syst_JpsiPt' )
  job.Add( 'Only J/#psi',              'FinalID_Syst_JpsiOnly' )
  job.Add( 'Only Z',                   'FinalID_Syst_ZOnly' )
  job.Add( 'm_{Z} Window(#uparrow)',   'FinalID_Syst_MassWinUp' )
  job.Add( 'm_{Z} Window(#downarrow)', 'FinalID_Syst_MassWinDown' )
  job.Add( 'Linear',                   'FinalID_Syst_JpsiLinear' )
  job.Add( 'Bkg/2',                    'FinalID_Syst_Half' )
  job.Add( 'Bkg*2',                    'FinalID_Syst_Double' )

  ###
  job.Add( 'Min Pt',                   'FinalMS_Syst_JpsiPt' )
  job.Add( 'm_{Z} Window(#uparrow)',   'FinalMS_Syst_MassWinUp' )
  job.Add( 'm_{Z} Window(#downarrow)', 'FinalMS_Syst_MassWinDown' )
  job.Add( 'Linear',                   'FinalMS_Syst_JpsiLinear' )
  job.Add( 'Bkg/2',                    'FinalMS_Syst_Half' )
  job.Add( 'Bkg*2',                    'FinalMS_Syst_Double' )
  #job.Add( '#Delta r_{2} free',        'FinalMS_Syst_p2Free' )
  job.Add( '#Delta r_{2}(#uparrow)',   'FinalMS_Syst_p2Up' )
  job.Add( '#Delta r_{2}(#downarrow)', 'FinalMS_Syst_p2Dw' )
  job.Add( '#Delta r_{0}',             'FinalMS_Syst_p0' )

  ###
  job.PrintPlots()
  job.PrintFiles( 'muons_Data15_Recs2016_01_19' )

if __name__ == '__main__':
  sys.exit( main() )
