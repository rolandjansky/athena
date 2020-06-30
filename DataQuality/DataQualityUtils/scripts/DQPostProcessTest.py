#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import sys

from ROOT import gROOT
gROOT.SetBatch(True)



from ROOT import gSystem
gSystem.Load('libDataQualityUtils')
from ROOT import dqutils


mf = dqutils.MonitoringFile()

def rpc_create(dummy):
  from DataQualityUtils.createRpcFolders import (createRPCDQMFDB,
                                                 createRPCConditionDB)
  createRPCDQMFDB()
  createRPCConditionDB()

def mdt_create(dummy):
  from DataQualityUtils.createMdtFolders import (createMDTConditionDBDead,
                                                 createMDTConditionDBNoisy)
  createMDTConditionDBDead()
  createMDTConditionDBNoisy()

##########################################
##Comment Out Those You do not wish to run
##########################################  

outFile = sys.argv[1]

isIncremental=False
if len(sys.argv) == 3:
  if sys.argv[2] == "True" or sys.argv[2] == "1":
    isIncremental = True

print('==================== Starting first round of checks ====================\n\n')

mf.fitMergedFile_IDPerfMonManager(outFile, isIncremental)
mf.fitMergedFile_DiMuMonManager(outFile, isIncremental)
mf.fitMergedFile_IDAlignMonManager(outFile, isIncremental)
rpc_create(outFile)
mf.RPCPostProcess(outFile, isIncremental)
mdt_create(outFile)
mf.CSCPostProcess(outFile, isIncremental)
mf.MDTPostProcess(outFile, isIncremental)
mf.TGCPostProcess(outFile, isIncremental)
mf.MDTvsTGCPostProcess(outFile, isIncremental)
mf.HLTEgammaPostProcess(outFile, isIncremental)
mf.HLTMuonPostProcess(outFile, isIncremental)
mf.HLTTauPostProcess(outFile, isIncremental)
mf.HLTMETPostProcess(outFile, isIncremental)
mf.HLTJetPostProcess(outFile, isIncremental)
mf.MuonTrackPostProcess(outFile, isIncremental)
mf.BJetTaggingPostProcess(outFile, isIncremental)
mf.L1CaloPostProcess(outFile, isIncremental)
mf.SCTPostProcess(outFile, isIncremental)
mf.VxMon_move(outFile, isIncremental) # may print a harmless error message about write access to EOS

print('\n\n====================== First round of checks are completed=============')
print('==================== Starting second round ====================\n\n')

mf.fitMergedFile_IDPerfMonManager(outFile, isIncremental)
mf.fitMergedFile_DiMuMonManager(outFile, isIncremental)
mf.fitMergedFile_IDAlignMonManager(outFile, isIncremental)
rpc_create(outFile)
mf.RPCPostProcess(outFile, isIncremental)
mdt_create(outFile)
mf.CSCPostProcess(outFile, isIncremental)
mf.MDTPostProcess(outFile, isIncremental)
mf.TGCPostProcess(outFile, isIncremental)
mf.MDTvsTGCPostProcess(outFile, isIncremental)
mf.HLTEgammaPostProcess(outFile, isIncremental)
mf.HLTMuonPostProcess(outFile, isIncremental)
mf.HLTTauPostProcess(outFile, isIncremental)
mf.HLTMETPostProcess(outFile, isIncremental)
mf.HLTJetPostProcess(outFile, isIncremental)
mf.MuonTrackPostProcess(outFile, isIncremental)
mf.BJetTaggingPostProcess(outFile, isIncremental)
mf.L1CaloPostProcess(outFile, isIncremental)
mf.SCTPostProcess(outFile, isIncremental)
mf.VxMon_move(outFile, isIncremental) # may print a harmless error message about write access to EOS

print('\n\n====================== Second round of checks are completed=============')
print('Postprocessing code should run in T0 without crashes ')
