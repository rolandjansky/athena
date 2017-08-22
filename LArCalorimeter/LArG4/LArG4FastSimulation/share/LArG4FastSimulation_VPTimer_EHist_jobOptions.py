##
## # $Id: LArG4FastSimulation_VPTimer_EHist_jobOptions.py 708108 2015-11-16 12:34:05Z jchapman $
##
## # jobOptions file for preparing detailed program timer and histogramming of
## # MC-level energy by particle and volume

actionProperties={
## Depths: level in geometry tree under Atlas::Atlas volume at which to truncate output;
## counted Atlas::Atlas level = 0, CALO::CALO level = 1, etc.
'CaloDepth':'4',     # hadron calorimeter, CALO::CALO
'BeamPipeDepth':'2', # beampipe, BeamPipe::BeamPipe
'InDetDepth':'2',    # inner detector, IDET::IDET
'MuonDepth':'2',     # muon detector, MUONQ02::MUONQ02

## Detail depth: output data for a specific volume and its parents; format /Atlas::Atlas/[lvl1Vol]/.../[Volume]
'DetailDepth':'/Atlas::Atlas/CALO::CALO/LArMgr::LArMgr/LArBarrel/Total LAR Volume/LAr::Barrel::Cryostat::HalfLAr::PosPhysical/LArMgr::LAr::EMB::ECAM'
}

## # volume/particle timing - prints cpu time spent per particle, per volume to outfile
## # DO NOT USE SIMULTANEOUSLY WITH EHistAction!
from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::TestActionVPTimerTool',['Run','Event','Step'])
for key, value in actionProperties.iteritems():
    simFlags.UserActionConfig.addConfig('G4UA::TestActionVPTimerTool',key,value)
print "volume/particle timing ON (see stdout)"

## # energy histogramming - creates ROOT file with histograms of kinetic energy by particle, by volume

## EHist-only options:
#simFlags.OptionalUserActionList.addAction('G4UA::TestActionEHistTool',['Run','Tracking','Step'])
#for key, value in actionProperties.iteritems():
#    simFlags.UserActionConfig.addConfig('G4UA::TestActionEHistTool',key,value)
#simFlags.UserActionConfig.addConfig('G4UA::TestActionEHistTool','ROOTFileName','top_ke_test.root') # name of .root file output
#simFlags.UserActionConfig.addConfig('G4UA::TestActionEHistTool','MaxHists','1000') # maximum number of histograms to store ( <~10000 for < 2MB of RAM)
#print "energy histogramming ON (outfile:",actionProperties['ROOTFileName'],")"

## # must be imported prior to the commands:
## AtlasG4Eng.G4Eng.init_Simulation(3)
## theApp.nextEvent( theApp.EvtMax )
## theApp.finalize()
