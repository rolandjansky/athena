include.block ("TrigTier0/TriggerNTupleProduction.py")

## 
## Job options to steer the inclusion of the ntuple production setup for all slices
## (this has to be included after the TriggerFlags.NtupleProductionFlags have been set)
##

## Inside the specific slice JO the TriggerFlags.NtupleProductionFlags
## can be used to access the name of the root file where each slice
## should go:
## example: NTAlg.DestinationFile = TriggerFlags.NtupleProductionFlags.FileNames.forSlice('MinBias')


## Root trees should not be written directly to file but registered in
## the THistSvc together with the file name (see example in
## Trigger/TrigAnalysis/TrigNtupleBase/NtupleAlgorithm.cxx)

from RecExConfig.RecFlags import rec

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter()

from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
from AthenaCommon.Logging import logging

logTriggerNTupleProduction = logging.getLogger("TriggerNTupleProduction.py")


logTriggerNTupleProduction.info("Including trigger ntuple setups according to the following specifications")
logTriggerNTupleProduction.info("Production Site: %r" % tf.NtupleProductionFlags.ProductionLocation() )
logTriggerNTupleProduction.info("Slices         : %r" % tf.NtupleProductionFlags.SliceTuples() )
logTriggerNTupleProduction.info("Files          : %r" % tf.NtupleProductionFlags.FileNames() )
logTriggerNTupleProduction.info("Disabled slices: %r" % tf.NtupleProductionFlags.DisabledSlices() )

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
  
for filename in tf.NtupleProductionFlags.FileNames():
    ServiceMgr.THistSvc.Output += [filename+" DATAFILE=\'"+filename+"\' OPT=\'RECREATE\'"]  

# MinBias
if tf.NtupleProductionFlags.SliceTuples.doSlice( "MinBias" ):
    include("TrigMinBiasNtuple/TrigMinBiasNtuple_jo.py")
    pass

# Muon
if tf.NtupleProductionFlags.SliceTuples.doSlice( "Muon" ) and rec.doMuon() :
    include("TrigMuonNtuple/TrigMuonNtuple_jo.py")
    pass

# Tau
if tf.NtupleProductionFlags.SliceTuples.doSlice( "Tau" ) and rec.doTau() :
    include("TrigTauPerformAthena/TrigTauNtuple_jo.py")
    pass

# Egamma
if tf.NtupleProductionFlags.SliceTuples.doSlice( "EGamma" ) and rec.doEgamma() :
    include("TrigEgammaNtuple/TrigEgammaNtuple_jo.py")
    pass

#L1Calo
if tf.NtupleProductionFlags.SliceTuples.doSlice( "L1Calo" ):
    include("TrigT1CaloCalibTools/L1Calo_D3PD_Tier1_jobOptions.py")
    pass

# TrigMenu
if tf.NtupleProductionFlags.SliceTuples.doSlice( "TrigMenu" ):
    include("TriggerMenuAnalysis/TriggerMenuAnalysis_Nt_Tier0.py")
    pass
