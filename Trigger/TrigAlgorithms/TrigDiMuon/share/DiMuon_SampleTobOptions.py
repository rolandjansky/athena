###############################################################
#
# Set Flags for Running the Trigger in TransientBS Mode
# includes Trigger_topOptions.py to do the real work.
# Reads input from ZEBRA.P (readG3=true)
#
# use:
# athena.py -bs TrigTransientBS_Flags.py
# athena.py -bs -c onlyCalo=True -bs TrigTransientBS_Flags.py
# athena.py -bs -c onlyID=True TrigTransientBS_Flags.py
# athena.py -bs -c onlyMuon=True TrigTransientBS_Flags.py
# 
#==============================================================
#onlyMuon=True

AllAlgs=False
doCBNT=False
doTruth=True     # needs to be true if creating Fake RoI 
doTrigger=True
doWriteAOD=False
doWriteESD=False
doWriteTAG=False

include( "TriggerRelease/TriggerFlags.py" )

#File generated on LAyout Q
#PoolRDOInput = [ "LFN:mu100_layQ_extract.pool" ]
#PoolRDOInput = ["rfio:/castor/cern.ch/atlas/csc/valiprod/sampleA/mc11/004042.mu_pt10/digit/v11000401/mc11.004042.mu_pt10.digit.RDO.v11000401._00001.pool.root.1"]
PoolRDOInput = ["rfio:/castor/cern.ch/atlas/csc/valiprod/sampleA/mc11/017702.Bs_Jpsi_mu6mu3_phi_KplusKminus/digit/v11000302/mc11.017702.Bs_m6m3_phi_KK.digit.RDO.v11000302._00049.pool.root"]
#PoolRDOInput = ["/home/panikas/atlas/11.0.4/mc11.017702.Bs_m6m3_phi_KK.digit.RDO.v11000302._00049.pool.root"]

# Set detector description
#DetDescrVersion = "Rome-Initial"
DetDescrVersion = "ATLAS-DC3-02"

# to start from HITS and not from DIGITS
#readMuonHits=True
#readMuonDigit=False

include( "RecExCommon/RecExCommon_flags.py" )
TriggerFlags.mySlice.setAll()

DetFlags.detdescr.Muon_setOn()
DetFlags.makeRIO.Calo_setOff()
DetFlags.makeRIO.Muon_setOff()
DetFlags.readRDOPool.Muon_setOn()
DetFlags.readRIOPool.LVL1_setOn()
DetFlags.readRIOBS.Muon_setOn()
DetFlags.writeBS.Muon_setOn()
TriggerFlags.doLVL1 = False
	
if readMuonHits:
    DetFlags.digitize.Muon_setOn()
    DetFlags.readRIOBS.Muon_setOff()
    DetFlags.readRIOPool.LVL1_setOff()
    TriggerFlags.doLVL1=True

# Set flags for the ntuple Content
TriggerFlags.doNtuple=True
TriggerFlags.doCalo=False
TriggerFlags.doEF=False
readHLTconfigFromXML=False 

#TriggerFlags.MuonSlice.unsetAll()
#TriggerFlags.MuonSlice.setL2()
TriggerFlags.mySlice.setL2()
TriggerFlags.EgammaSlice.unsetAll()
TriggerFlags.TauSlice.unsetAll()

#-------------------------------------------------------------
# End of setting flags
#-------------------------------------------------------------

include( "RecExCommon/RecExCommon_topOptions.py" )

theApp.Dlls += ["TrigSteering"]    
MessageSvc.OutputLevel = INFO

Algorithm("IOVDbSvc").OutputLevel = INFO
Algorithm("HistorySvc").OutputLevel = INFO

MessageSvc.defaultLimit = 10000000

# Number of events to be processed 
theApp.EvtMax = 10

IdScan_Bphysics_L2 = Algorithm ( "IdScan_Bphysics_L2" )
IdScan_Bphysics_L2.etaHalfWidth = 0.6
IdScan_Bphysics_L2.phiHalfWidth = 0.6
IdScan_Bphysics_L2.IDScanZFinder.ROIPhiWidth = 2*IdScan_Bphysics_L2.phiHalfWidth
IdScan_Bphysics_L2.HitFilter.phiHalfWidth = IdScan_Bphysics_L2.phiHalfWidth
IdScan_Bphysics_L2.HitFilter.etaHalfWidth = IdScan_Bphysics_L2.etaHalfWidth

TrigDiMuonFast_1_L2 = Algorithm ( "TrigDiMuonFast_1_L2" )
TrigDiMuonFast_1_L2.OutputLevel = DEBUG
TrigDiMuonFast_1_L2.doTiming = False
TrigDiMuonFast_1_L2.doNtuple = True

NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='ntuple.root' OPT='NEW'" ]

if not 'DiMuonNtupleFile' in dir():
    DiMuonNtupleFile = "ntuple_dimuon.root"
NTupleSvc.Output += [ "DIMUON DATAFILE='"+DiMuonNtupleFile+"' TYP='ROOT' OPT='NEW'" ]     

Algorithm("StepController_L2").OutputLevel=DEBUG
Algorithm("Lvl1Conversion_L2").OutputLevel=DEBUG
Algorithm("ResultBuilder_L2").OutputLevel=DEBUG
Algorithm("StepDecision_L2").OutputLevel=DEBUG
Algorithm("StepHandler_L2").OutputLevel=DEBUG
Algorithm("IdScan_Bphysics_L2").OutputLevel=DEBUG
Algorithm("TrigDiMuonFast_1_L2").OutputLevel=DEBUG
Algorithm("JpsiHypo_6GeV_L2").OutputLevel=DEBUG
