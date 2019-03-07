# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configure all flags (for RecExCommon) so that only Muon Standalone and Muon Combined reconstruction are run"""

from AthenaCommon.DetFlags import DetFlags 
from AthenaCommon.GlobalFlags import globalflags,GlobalFlags
from RecExConfig.RecFlags import rec as recFlags
from RecExConfig.RecAlgsFlags import recAlgs as recAlgsFlags
from RecExConfig.RecConfFlags import recConfFlags
from InDetRecExample.InDetJobProperties import InDetFlags
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags

# import standalone to avoid that it overwrites combined later on
import MuonRecExample.MuonRecStandaloneOnlySetup

# Output
recFlags.doWriteESD = False
recFlags.doWriteAOD = False

# reconstruction
recFlags.doESD = True
recFlags.doAOD = recFlags.doWriteAOD()

# enable inner detector
DetFlags.ID_setOn()
recFlags.doInDet = True
InDetFlags.Enabled = True
InDetFlags.disableInDetReco = False

# enable calorimeter
DetFlags.Calo_setOn()
recFlags.doCalo = True
recFlags.doLArg = True
recFlags.doTile = True
# do not run digitization
DetFlags.digitize.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.overlay.all_setOff()

recAlgsFlags.doEFlowJet = False

recFlags.doMuonCombined = True
muonCombinedRecFlags.Enabled = True
muonCombinedRecFlags.setDefaults()

# disable JetRec (no RecFlags flag for that)
try:
    from JetRec.JetRecFlags import jetFlags
    jetFlags.Enabled = False
except ImportError:
    pass

# bug in RecExCond does not pass recFlags.doTau()
try:
    from tauRec.tauRecFlags import jobproperties
    jobproperties.tauRecFlags.Enabled = recFlags.doTau()
except ImportError:
    pass

# disable MuonSpShower (needs JetRec)
recAlgsFlags.doMuonSpShower = False

# for AOD, only keep muons (with pass for P1HLT project)
try:
    from ParticleBuilderOptions.AODFlags import AODFlags
    AODFlags.allSetOff()
    AODFlags.Muon = True
    AODFlags.MuonTrackSlimmer = True
    AODFlags.SpclMC = True
except ImportError:
    pass
