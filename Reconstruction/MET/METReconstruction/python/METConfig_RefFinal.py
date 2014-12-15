# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, RefConfig, METConfig,clusterSigStates

from RecExConfig.RecFlags import rec
doInDet = rec.doInDet()
from JetRec.JetRecFlags import jetFlags
jetsUseTracks = jetFlags.useTracks()

############################################################################
# MET_RefFinal

rf_builders = [BuildConfig('Ele'),
               BuildConfig('Gamma'),
               BuildConfig('Tau'),
               BuildConfig('Jet'),
               BuildConfig('Muon'),
               BuildConfig('SoftClus'),
               BuildConfig('SoftTrk')]
rf_refiners = [RefConfig('TrackFilter','PVSoftTrk')]
cfg_mrf = METConfig('RefFinal',
                    rf_builders,
                    rf_refiners,
                    doSum=True,
                    doTracks=(doInDet and jetsUseTracks),
                    doRegions=True
                    )

# Configure the tools explicitly with defaults
# These are mostly already set in the constructor declareProperty calls
cfg_mrf.builders['Ele'].MinPt  = 10e3
cfg_mrf.builders['Ele'].MaxEta = 2.47
cfg_mrf.builders['Ele'].PIDSel = 'Medium'
cfg_mrf.builders['Ele'].AuthorSel = 17 # AuthorElectron | AuthorAmbiguous
#
cfg_mrf.builders['Gamma'].MinPt  = 10e3
cfg_mrf.builders['Gamma'].MaxEta = 2.47
cfg_mrf.builders['Gamma'].PIDSel = 'Tight'
cfg_mrf.builders['Gamma'].AuthorSel = 20 # AuthorPhoton | AuthorAmbiguous
#
cfg_mrf.builders['Tau'].MinPt     = 20e3
cfg_mrf.builders['Tau'].MaxEta    = 2.5
cfg_mrf.builders['Tau'].IsTauFlag = 20   # JetBDTSigMedium
cfg_mrf.builders['Tau'].ElVeto    = 23   # EleBDTMedium
cfg_mrf.builders['Tau'].MuVeto    = 4    # MuonVeto
cfg_mrf.builders['Tau'].MinWet    = 0.5
#
cfg_mrf.builders['Jet'].MinPt  = 0     # Perform OR on all jets, but don't select now
cfg_mrf.builders['Jet'].MaxEta = 5     # Will rerun RefJet and SoftTerms with calibration
cfg_mrf.builders['Jet'].MinWet = 0.5
cfg_mrf.builders['Jet'].DoMinWetPtCut = False
#
cfg_mrf.builders['Muon'].MinP             = 4e3
cfg_mrf.builders['Muon'].MinPt            = 2.5e3
cfg_mrf.builders['Muon'].MaxEta           = 2.5
cfg_mrf.builders['Muon'].QualCut          = 2     # 0:Tight, 1:Medium, 2:Loose
cfg_mrf.builders['Muon'].MinNpixel        = 3
cfg_mrf.builders['Muon'].MinNsilicon      = 5
cfg_mrf.builders['Muon'].MinEtaStandAlone = 2.5
cfg_mrf.builders['Muon'].MinNsegments     = 0
cfg_mrf.builders['Muon'].MinNprecision    = 3
#
cfg_mrf.builders['SoftClus'].VetoNegEClus = True
cfg_mrf.builders['SoftClus'].SignalState = clusterSigStates['LocHad']

metFlags.METConfigs()[cfg_mrf.suffix] = cfg_mrf
metFlags.METOutputList().append(cfg_mrf.suffix)
metFlags.METOutputList().append(cfg_mrf.suffix+"Regions")
