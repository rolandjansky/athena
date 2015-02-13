# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import pickle
with open( 'Run_FiveFb_Again_3/AutoSave-Run_FiveFb_Again_3.pkl', 'rb' ) as IN:
  myJob = pickle.load( IN )
  #import Code.DumpAllPlots
  #Code.DumpAllPlots.Run( myJob )
  myJob.PublishPlots()
  myJob.SendMail()
  myJob.Save()
