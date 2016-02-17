# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkCore.DerivationFrameworkMaster import *
# Set up custom MET reconstruction algorithms
from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, RefConfig, METConfig, getMETRecoAlg

METConfigs_SUSY = {}

######################################## CONFIG 1 ########################################
# Ignore tau ID/calibration
# Define the tools to build each term
met_key = 'NoTau' # Container suffix
met_builders = [BuildConfig('Ele'),
                BuildConfig('Gamma'),
                BuildConfig('Jet'),
                BuildConfig('Muon'),
                BuildConfig('SoftClus'),
                BuildConfig('SoftTrk')]
met_refiners = [RefConfig('JetFilter','RefJet_JVFCut'),
                RefConfig('TrackFilter','PVSoftTrk')]

# Define the details of the full configuration
METConfigs_SUSY[met_key] = METConfig(met_key,
                                     met_builders,
                                     met_refiners,
                                     doSum=True,
                                     doTracks=True,
                                     duplicateWarning=False
                                     )

######################################## CONFIG 2 ########################################
# Ignore tau ID/calibration, use likelihood electrons
# Define the tools to build each term
met_key = 'EleLHMedNoTau' # Container suffix
met_builders = [BuildConfig('Ele'),
                BuildConfig('Gamma'),
                BuildConfig('Jet'),
                BuildConfig('Muon'),
                BuildConfig('SoftClus'),
                BuildConfig('SoftTrk')]
met_refiners = [RefConfig('JetFilter','RefJet_JVFCut'),
                RefConfig('TrackFilter','PVSoftTrk')]

# Define the details of the full configuration
METConfigs_SUSY[met_key] = METConfig(met_key,
                                     met_builders,
                                     met_refiners,
                                     doSum=True,
                                     doTracks=True,
                                     duplicateWarning=False
                                     )
# Access the tool configurations here
METConfigs_SUSY[met_key].builders['Ele'].PIDSel = 'Electrons.DFCommonElectronsLHMedium'

######################################## MET FLAGS #######################################
# Update the common METFlags with the defined configurations
print "DFMissingET -- Defined custom MET configurations for SUSY:"
for key,cfg in METConfigs_SUSY.iteritems():
    print "DFMissingET --    ",key
    metFlags.METConfigs()[key] = cfg
    metFlags.METOutputList().append(key)
import METCommon
METCommon.METLists['SUSY'] = METConfigs_SUSY.keys()

DerivationFrameworkMetAlg_SUSY = getMETRecoAlg('METReco_DF_SUSY', METConfigs_SUSY)
DerivationFrameworkJob += DerivationFrameworkMetAlg_SUSY
