# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
rec.doInDet.set_Value_and_Lock(False)
rec.doCalo.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doForwardDet.set_Value_and_Lock(True)
rec.doTrigger.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
from JetRec.JetRecFlags import jetFlags
jetFlags.Enabled.set_Value_and_Lock(False)
rec.doAlfa.set_Value_and_Lock(False)
rec.doAFP.set_Value_and_Lock(False)
rec.doLucid.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
rec.doHeavyIon.set_Value_and_Lock(False)
