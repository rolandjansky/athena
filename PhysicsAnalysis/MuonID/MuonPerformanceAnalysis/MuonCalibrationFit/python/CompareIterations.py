# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import ROOT
import Code.Compare as cp

def main():

  ##### General commands #####
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = 3000

  #JobList = [ 'FullStat_Par_ID_Job02', 'FullStat_Par_ID_Job03', 'FullStat_Par_ID_Job04' ]
  #JobList = [ 'RewStat_ID_Job02', 'RewStat_ID_Job03', 'RewStat_ID_Job04', 'RewStat_ID_Job05', 'RewStat_ID_Job06' ] 
  #JobList = [ 'RewStat_ID_Job06', 'RewStat_ID_Job07', 'RewStat_ID_Job08', 'RewStat_ID_Job09', 'RewStat_ID_Job10',  ] 
  #JobList = [ 'RewStat_ID_Job11', 'RewStat_ID_Job12' ] 
  #JobList = [ 'Parallel_ID_Job01', 'Parallel_ID_Job02', 'Parallel_ID_Job03', 'Parallel_ID_Job04', 'Parallel_ID_Job05', 'Parallel_ID_Job06' ]
  #JobList = [ 'Parallel_ID_Job05', 'Parallel_ID_Job06', 'Parallel_ID_Job07', 'Parallel_ID_Job08', 'Parallel_ID_Job09', 'Parallel_ID_Job010' ]
  #JobList = [ 'Again_ID_Job01', 'Again_ID_Job02', 'Again_ID_Job03', 'Again_ID_Job04' ]
  #JobList = [ 'Again_ID_Job02', 'Zero_Job02', 'Zero_Job03', 'Zero_Job04' ]
  JobList = [ ( 'Jan.3^{rd}', 'FinalMS_Job011' ), ( 'Jan.19^{th}', 'FinalMS_Job017' ) ]
  #JobList = [ ( 'Jan.1^{rd}', 'FinalMS_Job011' ),( 'Jan.2^{rd}', 'FinalMS_Job012' ),( 'Jan.3^{rd}', 'FinalMS_Job013' ),( 'Jan.4^{rd}', 'FinalMS_Job014' ),( 'Jan.5^{rd}', 'FinalMS_Job015' ), ( 'Jan.19^{th}', 'FinalMS_Job017' ) ]


  job = cp.Compare( JobList )

  job.PrintPlots()

if __name__ == '__main__':
  sys.exit( main() )
