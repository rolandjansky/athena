from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.beamType.set_Value_and_Lock("singlebeam")

## calo settings from Guillaume
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
BTaggingFlags.Active=False

## muon settings from Rosy
#muonRecFlags.doMoore=False
#muonCombinedRecFlags.doMuidMuonCollection=False 
#muonRecFlags.doMuonboy=False
#muonCombinedRecFlags.doStacoMuonCollection=False
mooreFlags.doSegmentsOnly=True
muonboyFlags.doSegmentsOnly=True
muonRecFlags.doSegmentT0Fit = False
muonCombinedRecFlags.Enabled = False

## from Markus, Tony...
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doMinimalReco      = True
