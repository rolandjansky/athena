##
## # $Id: LArG4FastSimulation_VPTimer_EHist_jobOptions.py 708108 2015-11-16 12:34:05Z jchapman $
##
## # jobOptions file for preparing detailed program timer and histogramming of 
## # MC-level energy by particle and volume

## # must be imported after the command:
## theApp.initialize()

from G4AtlasApps import PyG4Atlas,AtlasG4Eng

actionProperties={
## EHist-only options:
'ROOTFileName':'top_ke_test.root',	# name of .root file output
'MaxHists':'1000',			# maximum number of histograms to store ( <~10000 for < 2MB of RAM)

## Depths: level in geometry tree under Atlas::Atlas volume at which to truncate output;
## counted Atlas::Atlas level = 0, CALO::CALO level = 1, etc.
'CaloDepth':'4',	# hadron calorimeter, CALO::CALO
'BeamPipeDepth':'2',	# beampipe, BeamPipe::BeamPipe
'InDetDepth':'2',	# inner detector, IDET::IDET
'MuonDepth':'2',	# muon detector, MUONQ02::MUONQ02

## Detail depth: output data for a specific volume and its parents; format /Atlas::Atlas/[lvl1Vol]/.../[Volume]
'DetailDepth':'/Atlas::Atlas/CALO::CALO/LArMgr::LArMgr/LArBarrel/Total LAR Volume/LAr::Barrel::Cryostat::HalfLAr::PosPhysical/LArMgr::LAr::EMB::ECAM'
}

## # volume/particle timing - prints cpu time spent per particle, per volume to outfile
## # DO NOT USE SIMULTANEOUSLY WITH EHistAction!
try:
    # Post UserAction Migration (ATLASSIM-1752)
    from G4AtlasServices.G4AtlasUserActionConfig import UAStore
    from AthenaCommon.CfgGetter import getPublicTool
    UAStore.addAction(getPublicTool('TestActionVPTimer'),['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
except:
    # Pre UserAction Migration
    TimerAction = PyG4Atlas.UserAction('G4ProfilingTools','TestActionVPTimer',
                                       ['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
    TimerAction.set_Properties(actionProperties)
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(TimerAction)
print "volume/particle timing ON (see stdout)"

## # energy histogramming - creates ROOT file with histograms of kinetic energy by particle, by volume
#try:
#    # Post UserAction Migration (ATLASSIM-1752)
#    from G4AtlasServices.G4AtlasUserActionConfig import UAStore
#    from AthenaCommon.CfgGetter import getPublicTool
#    UAStore.addAction(getPublicTool('TestActionEHist'),['BeginOfRun','EndOfRun','Step'])
#except:
#    # Pre UserAction Migration
#    EHistAction = PyG4Atlas.UserAction('G4ProfilingTools','TestActionEHist',
#                                       ['BeginOfRun','EndOfRun','Step'])
#    EHistAction.set_Properties(actionProperties)
#    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(EHistAction)
#print "energy histogramming ON (outfile:",actionProperties['ROOTFileName'],")"

## # must be imported prior to the commands:
## AtlasG4Eng.G4Eng.init_Simulation(3)
## theApp.nextEvent( theApp.EvtMax )
## theApp.finalize()
