# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
These minimal job options are for testing only!!!
"""

import AthenaPoolCnvSvc.ReadAthenaPool
#ServiceMgr.EventSelector.InputCollections = ['/tmp/simonyan/mc11_7TeV.106052.PythiaZtautau.merge.AOD.e825_s1324_s1300_r2731_r2700_tid518036_00/AOD.518036._000301.pool.root.1']
ServiceMgr.EventSelector.InputCollections = ['inputAODFile=/afs/cern.ch/work/p/pmalecki/mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.merge.AOD.e1176_s1479_s1470_r3553_r3549_tid779134_00/AOD.779134._000258.pool.root.1']



from RecExConfig.RecFlags import rec

rec.readRDO=False
rec.readESD=False # set true for ESD reading
rec.readAOD=True # set false for ESD reading
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doCBNT=False
rec.doHist=False
rec.doTruth=False
rec.AutoConfiguration=['everything']
#rec.doFloatingPointException=True

include ("RecExCommon/RecExCommon_topOptions.py")

from AthenaCommon.AppMgr import theApp
from TauDiscriminant.TauDiscriGetter import TauDiscriGetter
TauDiscriGetter(msglevel = VERBOSE)

theApp.EvtMax = 500
