# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

include.block("CaloMonitoring/CaloNewMonitoring_jobOptions.py")

#Create the set of flags
from AthenaMonitoring.DQMonFlags import DQMonFlags


if DQMonFlags.monManEnvironment() != 'AOD':
  from CaloMonitoring.LArCellMonAlg import LArCellMonConfigOld
  topSequence += LArCellMonConfigOld(DQMonFlags)

