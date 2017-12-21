from MuonRecExample.MuonRecFlags import muonRecFlags,mooreFlags,muonboyFlags
from MuonCalibAlgs.MuonCalibFlags import muonCalibFlags
from RecExConfig.RecFlags import rec as recFlags
from RecExConfig.RecAlgsFlags import recAlgs as recAlgsFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

from AthenaCommon.DetFlags import DetFlags
DetFlags.Muon_setOn()
DetFlags.Calo_setOff()
DetFlags.ID_setOff()

recAlgsFlags.doMuonIDStandAlone = False
recAlgsFlags.doMuonIDCombined = False

# in Standalone mode, don't allow any configuration errors
athenaCommonFlags.AllowIgnoreConfigError = False

# add some more flags available for standalone running
import MuonRecExample.MuonRecStandaloneFlags

# configure flags so that only Muon Standalone reco is run
import MuonRecExample.MuonRecStandaloneOnlySetup

muonRecFlags.useAlignmentCorrections = True
