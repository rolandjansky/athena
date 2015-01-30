# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# $Id: transBranchRemap.py,v 1.1 2008-10-06 15:35:51 ssnyder Exp $
#
# @file AthenaROOTAccess/python/transBranchRemap.py
# @author sss
# @date Oct 2008
# @brief Table of transient tree key remappings.
#


# We have to remap names of some branches of the transient tree
# because we have entries with the same SG key but different types.
# These are listed in this table.
# Keys are tuples of (SG key, type), and values
# are the new branch name.
transBranchRemap = {
    ('AODConfig-0', 'TrigConf::HLTAODConfigData') : 'HLTAODConfigData',
    ('AODConfig-0', 'TrigConf::Lvl1AODConfigData') : 'Lvl1AODConfigData',
    ('AODConfig-0', 'TrigConf::Lvl1AODPrescaleConfigData') : 'Lvl1AODPrescaleConfigData',
    ('HLT_TrigIDSCAN_Jet', 'DataVector<TrigVertex>') : 'HLT_TrigIDSCAN_Jet_Vx',
    ('HLT_TrigIDSCAN_Tau', 'DataVector<TrigVertex>') : 'HLT_TrigIDSCAN_Tau_Vx',
    ('HLT_TrigIDSCAN_eGamma', 'DataVector<TrigVertex>') : 'HLT_TrigIDSCAN_eGamma_Vx',
    ('HLT',        'Analysis::TauDetailsContainer') :'HLT_TauDetails',
    ('HLT_MuonEF', 'TrigMuonEFInfoContainer') : 'HLT_MuonEFInfo',

    ("Kt6H1TopoParticleJets"   , "JetCollection"): "Kt6H1TopoJets",
    ("Kt4H1TopoParticleJets"   , "JetCollection"): "Kt4H1TopoJets",
    ("Cone7H1TopoParticleJets" , "JetCollection"): "Cone7H1TopoJets",
    ("Cone4H1TopoParticleJets" , "JetCollection"): "Cone4H1TopoJets",
    ("Kt6H1TowerParticleJets"  , "JetCollection"): "Kt6H1TowerJets",
    ("Kt4H1TowerParticleJets"  , "JetCollection"): "Kt4H1TowerJets",
    ("Cone7H1TowerParticleJets", "JetCollection"): "Cone7H1TowerJets",
    ("Cone4H1TowerParticleJets", "JetCollection"): "Cone4H1TowerJets",
    ("Kt6TruthParticleJets"    , "JetCollection"): "Kt6TruthJets",
    ("Kt4TruthParticleJets"    , "JetCollection"): "Kt4TruthJets",
    ("Cone7TruthParticleJets"  , "JetCollection"): "Cone7TruthJets",
    ("Cone4TruthParticleJets"  , "JetCollection"): "Cone4TruthJets",
    ("AtlfastParticleJetContainer","JetCollection"): "AtlfastJetContainer",
    ("TruthEvent"              , "McEventCollection"):    "McTruthEvent",
    }

# Pick up extra trigger remappings.
try:
    # Be tolerant of import failures, since we're in AtlasEvent
    # and TrigEDMConfig is in AtlasTrigger.
    from TrigEDMConfig.TriggerEDM import getARATypesRenaming
    transBranchRemap.update (getARATypesRenaming())
except ImportError:
    print 'WARNING: Cannot import TrigEDMConfig.TriggerEDM.getARATypesRenaming'
