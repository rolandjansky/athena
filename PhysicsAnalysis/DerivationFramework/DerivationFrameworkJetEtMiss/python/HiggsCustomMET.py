# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkCore.DerivationFrameworkMaster import *
# Set up custom MET reconstruction algorithms
from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, RefConfig, METConfig, getMETRecoAlg

METConfigs_Higgs = {}

######################################## CONFIG 1 ########################################
# MET with EM+JES jets
# Define the tools to build each term
met_key = 'EMJets' # Container suffix
met_builders = [BuildConfig('Ele'),
                BuildConfig('Gamma'),
                BuildConfig('Tau'),
                BuildConfig('Jet'),
                BuildConfig('Muon'),
                BuildConfig('SoftClus'),
                BuildConfig('SoftTrk')]
met_refiners = [RefConfig('JetFilter','RefJet_JVFCut'),
                RefConfig('TrackFilter','PVSoftTrk')]

# Define the details of the full configuration
METConfigs_Higgs[met_key] = METConfig(met_key,
                                       met_builders,
                                       met_refiners,
                                       doSum=True,
                                       doTracks=True,
                                       duplicateWarning=False
                                       )

# Access the tool configurations here
METConfigs_Higgs[met_key].builders['Jet'].InputCollection = 'AntiKt4EMTopoJets'

######################################## CONFIG 2 ########################################
# MET with EMJES jets and likelihood electrons
# Define the tools to build each term
met_key = 'EleLHMedEMJets' # Container suffix
met_builders = [BuildConfig('Ele'),
                BuildConfig('Gamma'),
                BuildConfig('Tau'),
                BuildConfig('Jet'),
                BuildConfig('Muon'),
                BuildConfig('SoftClus'),
                BuildConfig('SoftTrk')]
met_refiners = [RefConfig('JetFilter','RefJet_JVFCut'),
                RefConfig('TrackFilter','PVSoftTrk')]

# Define the details of the full configuration
METConfigs_Higgs[met_key] = METConfig(met_key,
                                       met_builders,
                                       met_refiners,
                                       doSum=True,
                                       doTracks=True,
                                       duplicateWarning=False
                                       )

# Access the tool configurations here
METConfigs_Higgs[met_key].builders['Jet'].InputCollection = 'AntiKt4EMTopoJets'
METConfigs_Higgs[met_key].builders['Ele'].PIDSel = 'Electrons.DFCommonElectronsLHMedium'

######################################## MET FLAGS #######################################
# Update the common METFlags with the defined configurations
print "DFMissingET -- Defined custom MET configurations for HIGG:"
for key,cfg in METConfigs_Higgs.iteritems():
    print "DFMissingET --    ",key
    metFlags.METConfigs()[key] = cfg
    metFlags.METOutputList().append(key)
import METCommon
METCommon.METLists['Higgs'] = METConfigs_Higgs.keys()

DerivationFrameworkMetAlg_Higgs = getMETRecoAlg('METReco_DF_Higgs', METConfigs_Higgs)
DerivationFrameworkJob += DerivationFrameworkMetAlg_Higgs
