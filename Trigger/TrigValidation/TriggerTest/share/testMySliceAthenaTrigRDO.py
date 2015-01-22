#
# test my slice / chains 
#
#  flags for command-line input
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
### usually ATN tests runs with following RDO input: defined in xmlcatalog_file:PoolFileCatalog.xml
# currently /afs/cern.ch/atlas/offline/ReleaseData/v18/testfile/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.digit.RDO.e1513_s1499_s1504_d700_10evt.pool.root
# set/add/change to your fav inputfiles here: 
#acf.FilesInput=["/afs/cern.ch/atlas/offline/ReleaseData/v19/testfile/valid1.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s488_d151_tid039414_RDO.039414._00001_extract_10evt.pool.root"]
#acf.FilesInput+=["/afs/cern.ch/atlas/offline/ReleaseData/v19/testfile/ mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.digit.RDO.e1513_s1499_s1504_d700_10evt.pool.root"]

if not acf.EvtMax.is_locked():
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO
#scan for RTT files (only if dsName and fileRange set)
include("TriggerTest/TrigScanFiles.py")

# flags for RecExCommon
doTrigger=True
TriggerModernConfig=True
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False 
rec.doESD.set_Value_and_Lock(False) 
doTAG=False
rec.doCBNT=False 
#rec.doTruth=True
rec.doTruth.set_Value_and_Lock(False)


#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

# set up trigger monitoring
TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]

# set trigger menu
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False
#(should be set correctly for default) TriggerFlags.useL1CaloCalibration = False
#TriggerFlags.triggerMenuSetup = 'default' 
TriggerFlags.triggerMenuSetup = 'Physics_pp_v5' 
#TriggerFlags.triggerMenuSetup = 'MC_pp_v5' 
TriggerFlags.doHLT=True
#TriggerFlags.doL1Topo=True 

# enable my slice only
def mineOnly():
    # first switch all slices OFF
    TriggerFlags.Slices_all_setOff()
    
    # you DON'T have to do the following: TriggerFlags.MySlice.setAll() 
    # this will add all chains from your slice defined in TriggerFlags.triggerMenuSetup = 'Physics_pp_v5' / 'MC_pp_v5' 
    # adding one chain by hand is suficient - see below
    
    # enable Electron/Photon slice
    #TriggerFlags.EgammaSlice.setAll()

    #TriggerFlags.MuonSlice.setAll()

    #TriggerFlags.JetSlice.setAll()
    
    #bphys
    #TriggerFlags.MuonSlice.setAll()
    #TriggerFlags.BphysicsSlice.setAll()

    #TriggerFlags.METSlice.setAll()

    #TriggerFlags.TauSlice.setAll() 


    # for now physics pp v5 has no taus included, so temporarily we add one signature
    #TriggerFlags.TauSlice.signatures = [['tau20_medium1',  'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'],1]]
    
    # for now physics pp v5 has no egammas included, so temporarily we add here one signature
    # example for adding a chain
    #TriggerFlags.EgammaSlice.signatures = [['e24_medium_iloose',   'L1_EM18VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'],1]] 
                    
    # for now physics pp v5 does not have any muons in, so we add some here
    # example for adding several chains
    #TriggerFlags.MuonSlice.signatures = [
    #                                     ['mu24',                 'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1],
    #                                     ['mu24_imedium',          'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1],
    #                                     ['mu50',                'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1],
    #                                     ['2mu6',            'L1_2MU6', [], ['Muon'], ['RATE:MultiMuon' ,'BW:Muon'],   1]
    #                                     ]



# Override list of signatures in e/gamma slice
try:
    from TriggerMenu import useNewTriggerMenu
    useNewTM = useNewTriggerMenu()
    log.info("Using new TriggerMenu: %r" % useNewTM)
except:
    useNewTM = False
    log.info("Using old TriggerMenuPython since TriggerMenu.useNewTriggerMenu can't be imported")

if useNewTM:
    from TriggerMenu.menu.GenerateMenu import GenerateMenu
else:
    from TriggerMenuPython.GenerateMenu import GenerateMenu
GenerateMenu.overwriteSignaturesWith(mineOnly)



#------------ This is a temporary fix ---------------
#DetFlags.simulateLVL1.Calo_setOn()
#-------------end of temporary fix-------------------

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

#-----------------------------------------------------------
include("TriggerTest/TriggerTestCommon.py")
#-----------------------------------------------------------

#from TrigSteerMonitor.TrigSteerMonitorConf import TrigChainMoni
#chainMoni = TrigChainMoni()
#chainMoni.OutputLevel=VERBOSE
#topSequence.TrigSteer_HLT.MonTools += [chainMoni]
#
#import AthenaCommon.Configurable as Configurable
#Configurable.log.setLevel( INFO )
#print topSequence.getChildren()
