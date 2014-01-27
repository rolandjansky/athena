# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Run arguments file auto-generated on Fri Mar 12 16:42:54 2010 by:
# JobTransform: ESDtoAOD
# Version: PATJobTransforms-00-01-99

# Authors: David Cote <david.cote@cern.ch>

#################################################################################
from PyJobTransformsCore.runargs import RunArguments

runArgs = RunArguments()

from AthenaMonitoring.DQMonFlags import DQMonFlags

DQMonFlags.doGlobalMon=False
DQMonFlags.doLVL1CaloMon=False
DQMonFlags.doCTPMon=False
DQMonFlags.doHLTMon=False
DQMonFlags.doPixelMon=False
DQMonFlags.doSCTMon=False
DQMonFlags.doTRTMon=False
DQMonFlags.doTRTElectronMon=False
DQMonFlags.doInDetGlobalMon=False
DQMonFlags.doInDetAlignMon=False
DQMonFlags.doInDetPerfMon=False
DQMonFlags.doLArMon=False
DQMonFlags.doCaloMon=False
DQMonFlags.doTileMon=False
DQMonFlags.doEgammaMon=False
DQMonFlags.doMissingEtMon=False
DQMonFlags.doJetMon=False
DQMonFlags.doTauMon=False
DQMonFlags.doJetTagMon=False                    
DQMonFlags.doMuonRawMon.set_Value_and_Lock(True)
DQMonFlags.doMuonSegmentMon=False
DQMonFlags.doMuonTrackMon=False
DQMonFlags.doMuonAlignMon=False
DQMonFlags.doMuonPhysicsMon=False
DQMonFlags.doMuonCombinedMon=False
DQMonFlags.doLucidMon=False        

from MuonRecExample.MuonRecFlags import muonRecFlags

muonRecFlags.doCSCs=False
muonRecFlags.doTGCs=False
muonRecFlags.doRPCs=False
muonRecFlags.doMDTs=True


# Maximum number of events to process
runArgs.maxEvents = 100


# Derived class for monitoring HIST.
runArgs.outputDQMonitorFile = 'MonitorESD_lb2.root'

runArgs.inputESDFile = ['/data3/sampleESDs/data10_7TeV.00161379.physics_Muons.recon.ESD.f282/data10_7TeV.00161379.physics_Muons.recon.ESD.f282._lb0239._0006.1']
runArgs.inputESDFile += ['/data3/sampleESDs/data10_7TeV.00161379.physics_Muons.recon.ESD.f282/data10_7TeV.00161379.physics_Muons.recon.ESD.f282._lb0241._0002.1']
runArgs.inputESDFile += ['/data3/sampleESDs/data10_7TeV.00161379.physics_Muons.recon.ESD.f282/data10_7TeV.00161379.physics_Muons.recon.ESD.f282._lb0243._0008.1']
runArgs.inputESDFile += ['/data3/sampleESDs/data10_7TeV.00161379.physics_Muons.recon.ESD.f282/data10_7TeV.00161379.physics_Muons.recon.ESD.f282._lb0245._0006.1']
runArgs.inputESDFile += ['/data3/sampleESDs/data10_7TeV.00161379.physics_Muons.recon.ESD.f282/data10_7TeV.00161379.physics_Muons.recon.ESD.f282._lb0247._0008.1']
runArgs.inputESDFile += ['/data3/sampleESDs/data10_7TeV.00161379.physics_Muons.recon.ESD.f282/data10_7TeV.00161379.physics_Muons.recon.ESD.f282._lb0249._0008.1']
runArgs.inputESDFile += ['/data3/sampleESDs/data10_7TeV.00161379.physics_Muons.recon.ESD.f282/data10_7TeV.00161379.physics_Muons.recon.ESD.f282._lb0251._0006.1']
runArgs.inputESDFile += ['/data3/sampleESDs/data10_7TeV.00161379.physics_Muons.recon.ESD.f282/data10_7TeV.00161379.physics_Muons.recon.ESD.f282._lb0253._0002.1']


# Basic string
runArgs.beamType = 'collisions'


# List of strings separated by commas
runArgs.autoConfiguration = ['everything']


