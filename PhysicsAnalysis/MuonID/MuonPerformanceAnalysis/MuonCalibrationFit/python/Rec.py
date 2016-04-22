# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, pickle

CurrentDir = '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit'

with open( 'Again_ID_Job04/AutoSave-Again_ID_Job04.pkl', 'rb' ) as IN:
  myJob = pickle.load( IN )
  #myJob.UpdateRootFile()
  #import Code.DumpAllPlots
  #Code.DumpAllPlots.Run( myJob )
  #myJob.PublishPlots()
  #myJob.SendMail()
  #myJob.Save()
  #myJob.CheckRegions = os.path.expandvars( '%s/share/PreTest/RegionsIteration0_withSectors.txt' % CurrentDir )
  myJob.RunCheck()

'''
for i in range( 4, 12 + 1 ):
  with open( 'Job_ID_%d/AutoSave-Job_ID_%d.pkl' % ( i, i ), 'rb' ) as IN:
    myJob = pickle.load( IN )
    #myJob.UpdateRootFile()
    #import Code.DumpAllPlots
    #Code.DumpAllPlots.Run( myJob )
    #myJob.PublishPlots()
    #myJob.SendMail()
    #myJob.Save()
    myJob.RunCheck()
'''
