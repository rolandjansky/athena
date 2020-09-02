# switch off algorithms that are not needed for minimal reconstruction
if rec.doMinimalRec():
    from InDetRecExample.InDetJobProperties import InDetFlags
    from tauRec.tauRecFlags import jobproperties
    jobproperties.tauRecFlags.Enabled=False
    recAlgs.doMuonIDStandAlone=False
    recAlgs.doMuonIDCombined=False
    recAlgs.doMuidLowPt=False      
    recAlgs.doMuGirl=False
    recAlgs.doStaco=False
    recAlgs.doMuTag=False
    recAlgs.doTileMuID=False
    recAlgs.doCaloTrkMuId=False
    recAlgs.doMissingET=False
    recAlgs.doObjMissingET=False
    recAlgs.doMissingETSig=False
    recAlgs.doEFlow=False
    recAlgs.doEFlowJet=False
    recAlgs.doAtlfast=False

