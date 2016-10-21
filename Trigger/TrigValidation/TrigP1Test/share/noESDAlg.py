from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled=False
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.Enabled=False
from MuonRecExample.MuonRecFlags import muonRecFlags
jobproperties.MuonRec.Enabled=False
rec.doTau=False
rec.doMuonCombined=False
rec.doEgamma=False
rec.doJetMissingETTag=False

