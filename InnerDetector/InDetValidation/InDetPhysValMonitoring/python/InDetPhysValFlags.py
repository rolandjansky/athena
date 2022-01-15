# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.AllConfigFlags import _addFlagsCategory

def createIDPVMConfigFlags():
    from AthenaConfiguration.AthConfigFlags import AthConfigFlags
    icf = AthConfigFlags()

    icf.addFlag("IDPVM.doValidateDBMTracks", False )
    icf.addFlag("IDPVM.doValidateGSFTracks", False )
    icf.addFlag("IDPVM.doValidateLooseTracks", False )
    icf.addFlag("IDPVM.doValidateTightPrimaryTracks", False )
    icf.addFlag("IDPVM.doValidateTracksInJets", False )
    icf.addFlag("IDPVM.doValidateTracksInBJets", False )
    icf.addFlag("IDPVM.doValidateTruthToRecoNtuple", False )
    icf.addFlag("IDPVM.doValidateMuonMatchedTracks", False )
    icf.addFlag("IDPVM.doValidateElectronMatchedTracks", False )
    icf.addFlag("IDPVM.doValidateLargeD0Tracks", False )
    icf.addFlag("IDPVM.doPhysValOutput", False )
    icf.addFlag("IDPVM.doExpertOutput", False )
    icf.addFlag("IDPVM.doTruthOriginPlots", False )
    icf.addFlag("IDPVM.doPerAuthorPlots", False )
    icf.addFlag("IDPVM.doHitLevelPlots", False )
    icf.addFlag("IDPVM.setTruthStrategy", "HardScatter" )
    icf.addFlag("IDPVM.validateExtraTrackCollections", [] ) # List of extra track collection names to be validated in addition to Tracks.
    icf.addFlag("IDPVM.ancestorIDs", [] )
    icf.addFlag("IDPVM.requiredSiHits", 1)
    icf.addFlag("IDPVM.hardScatterStrategy", 0 ) # The hard-scatter vertex selection strategy to use when running hard-scatter efficiency / performance plots in IDPVM. 0 corresponds to sumPt^2, 1 corresponds to sumPt
    
    return icf

_addFlagsCategory(ConfigFlags, "IDPVM", createIDPVMConfigFlags , "AthenaMonitoring")
