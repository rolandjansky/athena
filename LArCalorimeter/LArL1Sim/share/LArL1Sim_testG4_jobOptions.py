# +===========================================================================+
# +                                                                           +
# + Author ........: F. Ledroit                                               +
# + Institut ......: ISN Grenoble                                             +
# + Creation date .: 15/09/2004      Version 0.01                             +
# + Subject .......: Job Option file to test LArTTL1Maker on G4               +
# +===========================================================================+ 
# Top Level JobOptions to test LArTTl1Maker algorithm.
# Input is from LArHits (simulation)

DetDescrVersion = "ATLAS-GEO-03-00-00"
include( "RecExCond/RecExCommon_flags.py" )

#DetFlags.detdescr.ID_setOff()
#DetFlags.detdescr.Calo_setOff()
#DetFlags.detdescr.LAr_setOff()
#DetFlags.detdescr.Tile_setOff()
#DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()

# set up all detector description 
include ("RecExCond/AllDet_detDescr.py")
include( "IOVDbSvc/IOVRecExCommon.py" )

#globalflags.ConditionsTag = 'OFLCOND-SIM-00-00-03'
if len(globalflags.ConditionsTag())!=0:
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())

# Number of events to be processed (default is 10)
theApp.EvtMax = 10
##svcMgr.EventSelector.FirstEvent        = 1

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel               = 3
#svcMgr.MessageSvc.verboseLimit = 100000
#svcMgr.MessageSvc.debugLimit = 100000
#ServiceMgr.IOVSvc.OutputLevel = VERBOSE 

# to read pool Data
import AthenaPoolCnvSvc.ReadAthenaPool

# G4 Pool input 
#svcMgr.EventSelector.InputCollections = ["LFN:input.root"];
#svcMgr.EventSelector.InputCollections = "$INFN";

# csc datasets
#svcMgr.EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/f/fledroit/csc7051/calib0_mc12.007051.singlepart_gammaTT_emecIW.simul.HITS.v12003103_tid004119._00015.pool.root"]
#svcMgr.EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/f/fledroit/csc7003/calib0_csc11.007003.singlepart_e_Et25.simul.HITS.v12000301_tid003216._00001.pool.root"]
#svcMgr.EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/s/svahsen/digitization/RTT/calib1_csc11.005200.T1_McAtNlo_Jimmy.simul.HITS.v12003104_tid004131._00069.pool.root.10"]
#svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/f/fledroit/scratch0/test_ntuples/calib0_csc11.007003.singlepart_e_Et25.simul.HITS.v12000301_tid003216._00001.pool.root"]
#
#svcMgr.EventSelector.InputCollections = ["rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/HITS/valid1.007003.singlepart_e_Et25.simul.HITS.e322_s472_tid025400/HITS.025400._00002.pool.root.1"]
svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/k/ketevi/w0/data/singleMuon100.HITS.020162._00001.pool.root"]



include( "LArL1Sim/LArL1Sim_G4_jobOptions.py" )
theLArTTL1Maker.OutputLevel               = 2

# Configure LArTTL1Maker here
#theLArTTL1Maker.SubDetectors="LAr_All"
#theLArTTL1Maker.EmBarrelHitContainerName="LArHitEMB"
#theLArTTL1Maker.EmEndCapHitContainerName="LArHitEMEC"
#theLArTTL1Maker.HecHitContainerName="LArHitHEC"
#theLArTTL1Maker.ForWardHitContainerName="LArHitFCAL"

#theLArTTL1Maker.EmTTL1ContainerName="LArTTL1EM"
#theLArTTL1Maker.HadTTL1ContainerName="LArTTL1HAD"
        
#theLArTTL1Maker.NoiseOnOff=true

#theLArTTL1Maker.PileUp=false
#theLArTTL1Maker.UseTriggerTime=false
#theLArTTL1Maker.TriggerTimeToolName="CosmicTriggerTimeTool"

#theLArTTL1Maker.EmBarrelCalibrationCoeffs=[1.03,  1.024, 1.019, 1.02,  1.02,  1.024, 1.03,  1.046, 1.06, 1.053, 1.057, 1.062, 1.063, 1.076, 1.176]
#theLArTTL1Maker.EmEndCapCalibrationCoeffs=[1.176, 1.061, 1.087, 1.015, 1.019, 1.014, 1.014, 1.009, 1.01, 1.003, 1.016, 1.003, 0.993, 1.005, 0.963]
#theLArTTL1Maker.HECCalibrationCoeffs=[1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.]
#theLArTTL1Maker.EmFcalCalibrationCoeffs=[1., 1., 1., 1.]
#theLArTTL1Maker.HadFcalCalibrationCoeffs=[1., 1., 1., 1.]

#theLArTTL1Maker.NoEmCalibrationMode=false
#theLArTTL1Maker.NoHadCalibrationMode=false
#theLArTTL1Maker.ChronoTest=false
#theLArTTL1Maker.DebugThreshold=5000.

