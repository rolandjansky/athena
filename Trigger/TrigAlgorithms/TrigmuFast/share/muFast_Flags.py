###############################################################
#
# Options for running muFast in Transient Bytetsream mode
# to be used with the TriggerRelease fragments
# 
#==============================================================

onlyMuon=True
# SkipEvents = 519

# Set flags that determine the detector description
# Set only one flag True:
TDRdata=False
DC1data=False     
DC2data=False
RomeData=True
# This flag can be set in addition to the DC1data flag
InitialLayout=False


# Set flags that determine the input data source
# Set only one flag True:
readZEBRA=False
readPool=True
readBS=  False     
# These flags can be set in addition:
writeBS= False     
transientBS=True 

# uncomment for starting from Simulation HITS
#readMuonHits = True

include ("TriggerRelease/TriggerFlags.py")


# Set flags for the ntuple Content
doCBNT=False
TriggerFlags.doNtuple=True
useROOTNtuple=True # False : hbook ntuple

doTruth=True     # needs to be true if creating Fake RoI 

#-------------------------------------------------------------
# End of setting flags
#-------------------------------------------------------------

include( "TriggerRelease/Trigger_topOptions.py" )
    
MessageSvc.OutputLevel = ERROR

MessageSvc.defaultLimit = 10000000

# Number of events to be processed 
theApp.EvtMax = 50000

muFast = Algorithm ( "muFast_1_L2" )

muFast.OutputLevel = ERROR

muFast.MUlvl1INFO  = True
muFast.MUtrackINFO = True
muFast.MUroadsINFO = True
muFast.MUdecoINFO  = True
muFast.MUcontINFO  = True
muFast.MUfitINFO   = True
muFast.MUsagINFO   = True
muFast.MUptINFO    = True

muFast.UseCalibration = True

muFast.Timing = False
muFast.FEXntuple = True
muFast.LUTntuple = False

muRoadsSvc = Service("MuRoadsSvc")
muRoadsSvc.ScaleBarrelMiddleRoads = 3.5
muRoadsSvc.ScaleBarrelInnerRoads = 2.5
muRoadsSvc.ScaleBarrelOuterRoads = 2.5


IdScan = Algorithm ("IdScan_IdScanMain_1_L2")
IdScan.OutputLevel = DEBUG
IdScan.etaHalfWidth             = 0.4
IdScan.phiHalfWidth             = 0.4
IdScan.ZFinder.PhiWidth         = 0.8  # twice



NTupleSvc.Output = [ "FILE1 DATAFILE='trigger.root' OPT='NEW' TYP='ROOT'" ]
