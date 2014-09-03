# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#Job options needed to enable LAr alignment

from IOVDbSvc.CondDB import conddb

if  conddb.isMC:
  #Monte Carlo case:
  conddb.addFolder("LAR_OFL","/LAR/Align")
  conddb.addFolder("LAR_OFL","/LAR/LArCellPositionShift")
else:
  #Regular offline data processing
  conddb.addFolder("LAR_ONL","/LAR/Align")
  conddb.addFolder("LAR_ONL","/LAR/LArCellPositionShift")



