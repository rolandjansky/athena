# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkCore.DerivationFrameworkMaster import *
# Set up custom MET reconstruction algorithms
from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, RefConfig, METConfig, getMETRecoAlg

METConfigs_Top = {}

######################################## CONFIG 1 ########################################
# Ignore tau and photon ID/calibration, medium electrons
# Define the tools to build each term
met_key = 'EleMedNoGammaNoTau' # Container suffix
met_builders = [BuildConfig('Ele'),
                BuildConfig('Jet'),
                BuildConfig('Muon'),
                BuildConfig('SoftClus'),
                BuildConfig('SoftTrk')]
met_refiners = [RefConfig('JetFilter','RefJet_JVFCut'),
                RefConfig('TrackFilter','PVSoftTrk')]

# Define the details of the full configuration
METConfigs_Top[met_key] = METConfig(met_key,
                                    met_builders,
                                    met_refiners,
                                    doSum=True,
                                    doTracks=True,
                                    duplicateWarning=False
                                    )

# Access the tool configurations here
# Switch electron selection to Tight
METConfigs_Top[met_key].builders['Ele'].PIDSel = 'Medium'

######################################## CONFIG 2 ########################################
# Ignore tau and photon ID/calibration, tight electrons
# Define the tools to build each term
met_key = 'EleTightNoGammaNoTau' # Container suffix
met_builders = [BuildConfig('Ele'),
                BuildConfig('Jet'),
                BuildConfig('Muon'),
                BuildConfig('SoftClus'),
                BuildConfig('SoftTrk')]
met_refiners = [RefConfig('JetFilter','RefJet_JVFCut'),
                RefConfig('TrackFilter','PVSoftTrk')]

# Define the details of the full configuration
METConfigs_Top[met_key] = METConfig('EleTightNoGammaNoTau',
                                    met_builders,
                                    met_refiners,
                                    doSum=True,
                                    doTracks=True,
                                    duplicateWarning=False
                                    )

# Access the tool configurations here
# Switch electron selection to Tight
METConfigs_Top[met_key].builders['Ele'].PIDSel = 'Tight'

######################################## CONFIG 3 ########################################
# Ignore tau and photon ID/calibration, medium likelihood electrons
# Define the tools to build each term
met_key = 'EleLHMedNoGammaNoTau' # Container suffix
met_builders = [BuildConfig('Ele'),
                BuildConfig('Jet'),
                BuildConfig('Muon'),
                BuildConfig('SoftClus'),
                BuildConfig('SoftTrk')]
met_refiners = [RefConfig('JetFilter','RefJet_JVFCut'),
                RefConfig('TrackFilter','PVSoftTrk')]

# Define the details of the full configuration
METConfigs_Top[met_key] = METConfig('EleLHMedNoGammaNoTau',
                                    met_builders,
                                    met_refiners,
                                    doSum=True,
                                    doTracks=True,
                                    duplicateWarning=False
                                    )

# Access the tool configurations here
# Switch electron selection to Tight
METConfigs_Top[met_key].builders['Ele'].PIDSel = 'Electrons.DFCommonElectronsLHMedium'

######################################## CONFIG 4 ########################################
# Ignore tau and photon ID/calibration, tight likelihood electrons
# Define the tools to build each term
met_key = 'EleLHTightNoGammaNoTau' # Container suffix
met_builders = [BuildConfig('Ele'),
                BuildConfig('Jet'),
                BuildConfig('Muon'),
                BuildConfig('SoftClus'),
                BuildConfig('SoftTrk')]
met_refiners = [RefConfig('JetFilter','RefJet_JVFCut'),
                RefConfig('TrackFilter','PVSoftTrk')]

# Define the details of the full configuration
METConfigs_Top[met_key] = METConfig('EleLHTightNoGammaNoTau', # Container suffix
                                    met_builders,
                                    met_refiners,
                                    doSum=True,
                                    doTracks=True,
                                    duplicateWarning=False
                                    )

# Access the tool configurations here
# Switch electron selection to Tight
METConfigs_Top[met_key].builders['Ele'].PIDSel = 'Electrons.DFCommonElectronsLHTight'

######################################## MET FLAGS #######################################
# Update the common METFlags with the defined configurations
print "DFMissingET -- Defined custom MET configurations for TOP:"
for key,cfg in METConfigs_Top.iteritems():
    print "DFMissingET --    ",key
    metFlags.METConfigs()[key] = cfg
    metFlags.METOutputList().append(key)
import METCommon
METCommon.METLists['Top'] = METConfigs_Top.keys()

DerivationFrameworkMetAlg_Top = getMETRecoAlg('METReco_DF_Top', METConfigs_Top)
DerivationFrameworkJob += DerivationFrameworkMetAlg_Top
