# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

f = open('input.txt', 'r')
inputFiles = f.readlines()
f.close()
inputFiles = [item.strip() for item in inputFiles]

from AthenaCommon.AthenaCommonFlags import jobproperties as jp
jp.AthenaCommonFlags.FilesInput = inputFiles

include('AthenaPython/read_file.py')

from AthenaCommon.AppMgr import theApp, ServiceMgr
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

import AthenaPoolCnvSvc.ReadAthenaPool
theApp.EvtMax = -1

if True:  # run only TauEnergyCalibration
    from TauDiscriminant.TauDiscriminantConf import TauEnergyCalibration
    TauEnergyCalibration.OutputLevel = DEBUG
    tauEnergyCalibration = TauEnergyCalibration('TauEnergyCalibration')
    tauEnergyCalibration.tauContainerKey = 'TauRecContainer'
    tauEnergyCalibration.calibrationFile = 'EnergyCalibration.root'
    tauEnergyCalibration.vertexContainerKey = 'VxPrimaryCandidate'
    job += tauEnergyCalibration
else:
    from TauDiscriminant.TauDiscriGetter import TauDiscriGetter
    TauDiscriGetter(do_upstream_algs = True, msglevel = DEBUG)









