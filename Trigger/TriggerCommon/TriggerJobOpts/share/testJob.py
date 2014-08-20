# jobO to run Electron slice with athena 

# Set flags for the ntuple Content
doCBNT=False
useROOTNtuple=True # False : hbook ntuple
doTruth=True     # needs to be true if creating Fake RoI 

if not ('EvtMax' in dir()):
    EvtMax=10
if not ('OutputLevel' in dir()):
    OutputLevel=DEBUG

#AllAlgs=False
doWriteAOD=False
doWriteESD=False
doWriteTAG=False
doAOD=False # make AOD objects
doESD=False # make ESD objects
doTAG=False

TriggerModernConfig=True
doTrigger=True

from TriggerJobOpts.TriggerFlags import TriggerFlags
#TriggerFlags.readHLTconfigFromXML=False
#TriggerFlags.readLVL1configFromXML=False
#TriggerFlags.inputHLTconfigFile='outputHLTconfig.xml'
#TriggerFlags.inputLVL1configFile='outputLVL1config.xml'



include("RecExCommon/RecExCommon_flags.py")




TriggerFlags.doLVL1=True
TriggerFlags.doEF=True
TriggerFlags.doID=True
TriggerFlags.doMuon=True



TriggerFlags.Slices_all_setOn()
# use what is below to leave only particular slice
# TriggerFlags.Slices_all_setOff()
# TriggerFlags.EgammaSlice.setAll()
# TriggerFlags.TauSlice.setAll()
# TriggerFlags.MuonSlice.setAll()
# TriggerFlags.BphysicsSlice.setAll()
TriggerFlags.METSlice.signatures = []
# TriggerFlags.JetSlice.setAll()

#TriggerFlags.EgammaSlice.signatures = ['e10', 'e25i', 'g10', 'e60', '2e15i', '3e15i', 'Zee']

#TriggerFlags.triggerMenuSetup = 'lumi0.01_no_prescale' # this woay one can modifying full menu depending on lumi switches
#TriggerFlags.triggerMenuSetup = 'undefined_no_prescale' # use this sot set all prescales to 1 (LVL1 and HLT)


TriggerFlags.doNtuple=True
doTrigEgammaBremFit=True
#doTrigEgammaBremFit=False


TriggerFlags.abortOnConfigurationError=True
TriggerFlags.enableMonitoring = [ 'Validation', 'Time' ]


TriggerFlags.print_JobProperties('tree&value')




#-------------------------------------------------------------
# End of setting flags
#-------------------------------------------------------------


if 'GlobalFlags' not in dir():
    from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.Luminosity.set_zero()



include( "RecExCommon/RecExCommon_topOptions.py" )



# uncomment to swicth on the special configuration for the Lvl1 muon barrel trigger
# i.e. mu4,mu6,mu10,mu11,mu20,mu40
ServiceMgr.RPCcablingSvc.HackFor1031 = True



MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence.TrigSteer_L2.OutputLevel = DEBUG

if TriggerFlags.doLVL2():
    for algo in topSequence.TrigSteer_L2.getChildren():
        algo.OutputLevel = DEBUG

if TriggerFlags.doEF():
    for algo in topSequence.TrigSteer_EF.getChildren():
        algo.OutputLevel = DEBUG

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job.CTPSimulation.OutputLevel=VERBOSE
job.RoIBuilder.OutputLevel=VERBOSE


#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False


print AlgSequence()


print ServiceMgr



#raise Exception("OK") # you can unsomment this like. Job will stop then but you would be able to see henerated oututputHLconfig.xml and check if it contains wahat you want. It is faster then to work. 




