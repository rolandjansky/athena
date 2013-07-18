
from AthenaCommon.AthenaCommonFlags import jobproperties

jobproperties.AthenaCommonFlags.PoolHitsInput = ["/tmp/asbrizzi/HITS.118273._000287.pool.root"]
jobproperties.AthenaCommonFlags.PoolRDOOutput =  "/tmp/asbrizzi/digi.pool.root"
jobproperties.AthenaCommonFlags.EvtMax = 100000

#from Digitization.DigitizationFlags import jobproperties

#jobproperties.Digitization.doXingByXingPileUp=True # do PileUpTool style overlay
#jobproperties.Digitization.doMinimumBias        = True
#jobproperties.Digitization.numberOfCollisions   = 5.3
#jobproperties.Digitization.initialBunchCrossing = -30
#jobproperties.Digitization.finalBunchCrossing   = +30
#jobproperties.Digitization.minBiasInputCols     = ["hits.new.pool.root"]

from AthenaCommon.GlobalFlags import jobproperties

jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'

from AthenaCommon.DetFlags import DetFlags

DetFlags.Lucid_setOn()
#DetFlags.LVL1_setOn()
DetFlags.Truth_setOn()

from Digitization.DigitizationFlags import jobproperties
jobproperties.Digitization.overrideMetadata=['SimulatedDetectors'] # temporary work around to run over old SimHits

include("LUCID_EventAthenaPool/LUCID_EventAthenaPool_joboptions.py")
include("Digitization/Digitization.py")

Service("StoreGateSvc").ActivateHistory = False
Service("GeoModelSvc").IgnoreTagDifference = True

lucid.numTubes          = 40
lucid.qdcChannelsPerPE  = 15.0
lucid.qdcPedestal       = 150.0
lucid.qdcFedNoiseFactor = 3.0
lucid.tdcPmtNoiseFactor = 2.8
lucid.tdcFedNoiseFactor = 0.8

lucid.TotalPMTgain      = 1.25e6
lucid.AmpFactor         = 7.5
lucid.Q1bin             = 0.1e-12
lucid.NoiseCharge       = 541666
lucid.numDyinodes       = 10
lucid.dynodeGammaFactor = 1.356

lucid.fillRootTree      = True

lucid.pmtSmearing  = [0.317, 0.000, 0.292, 0.316, 0.208, 0.178, 0.204, 0.281, 0.233, 0.261, 0.223, 0.250, 0.254, 0.239, 0.202, 0.224,  1,  1,  1,  1,
                      0.268, 0.277, 0.297, 0.310, 0.203, 0.347, 0.269, 0.241, 0.234, 0.234, 0.277, 0.297, 0.225, 0.297, 0.238, 0.000,  1,  1,  1,  1]
lucid.pmtScaling   = [1.010, 0.000, 0.991, 0.948, 1.152, 1.221, 1.160, 0.988, 1.092, 1.063, 1.143, 1.091, 1.109, 1.117, 1.185, 1.142,  1,  1,  1,  1,
                      1.023, 1.127, 1.043, 0.986, 1.148, 0.899, 0.898, 1.098, 1.115, 1.109, 1.127, 1.043, 1.085, 1.043, 1.063, 0.000,  1,  1,  1,  1]
lucid.gasScaling   = [1.176, 0.000, 1.217, 1.101, 1.143, 1.105, 1.103, 1.144, 1.075, 1.069, 1.100, 1.208, 1.212, 1.125, 1.026, 1.037,  1,  1,  1,  1,
                      1.064, 0.956, 0.975, 0.938, 1.205, 1.095, 1.137, 1.222, 1.262, 1.160, 0.923, 0.969, 1.132, 0.969, 1.174, 0.000,  1,  1,  1,  1]
lucid.npeThreshold = [   17,    15,    16,    16,    18,    16,    16,    18,    17,    16,    16,    17,    19,    16,    16,    17, 15, 15, 15, 15,
                         17,    16,    16,    17,    17,    15,    16,    16,    17,    16,    15,    17,    17,    15,    16,    15, 15, 15, 15, 15]

from GaudiSvc.GaudiSvcConf import THistSvc

if (lucid.fillRootTree):
    THistSvc = THistSvc()
    THistSvc.Output = ["AANT DATAFILE='cali.root' OPT='RECREATE'"]
    ServiceMgr += THistSvc

if DetFlags.LVL1_on():
    include("LUCID_Digitization/LUCID_LVL1Digitization_JobOptions.py")
