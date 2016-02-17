# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkCore.DerivationFrameworkMaster import *
# Set up custom MET reconstruction algorithms
from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, RefConfig, METConfig, getMETRecoAlg

METConfigs_JetMet = {}

######################################## CONFIG 1 ########################################
# MET_RefFinal with likelihood electrons
# Define the tools to build each term
met_key = 'EleLHMed' # Container suffix
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
METConfigs_JetMet[met_key] = METConfig(met_key,
                                       met_builders,
                                       met_refiners,
                                       doSum=True,
                                       doTracks=True,
                                       duplicateWarning=False
                                       )

# Access the tool configurations here
METConfigs_JetMet[met_key].builders['Ele'].PIDSel = 'Electrons.DFCommonElectronsLHMedium'

######################################## CONFIG 2 ########################################
# MET_Topo: EM topoclusters
# Define the tools to build each term
met_key = 'Topo' # Container suffix
met_builders = [BuildConfig('SoftClus','Topo')]
met_refiners = []

# Define the details of the full configuration
METConfigs_JetMet[met_key] = METConfig(met_key,
                                       met_builders,
                                       met_refiners,
                                       doSum=False,
                                       doTracks=True,
                                       duplicateWarning=False
                                       )

######################################## MET FLAGS #######################################
# Update the common METFlags with the defined configurations
print "DFMissingET -- Defined custom MET configurations for JETMET:"
for key,cfg in METConfigs_JetMet.iteritems():
    print "DFMissingET --    ",key
    metFlags.METConfigs()[key] = cfg
    metFlags.METOutputList().append(key)
import METCommon
METCommon.METLists['JetMet'] = METConfigs_JetMet.keys()

DerivationFrameworkMetAlg_JetMet = getMETRecoAlg('METReco_DF_JetMet', METConfigs_JetMet)
DerivationFrameworkJob += DerivationFrameworkMetAlg_JetMet
