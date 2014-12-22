# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file D3PDMakerConfig/python/D3PDMakerFlags.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Common flags for D3PD making.
#


"""Common flags for D3PD making.
"""


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.SystemOfUnits import GeV


class D3PDMakerFlags (JobPropertyContainer):
    """Common flags for D3PD making.
"""
    pass


jobproperties.add_Container (D3PDMakerFlags)


# Helper to add a simple string property.
def _string_prop (name, val):
    prop = type (name, (JobProperty,), {})
    prop.statusOn = True
    prop.allowedTypes = ['str']
    prop.StoredValue = val
    jobproperties.D3PDMakerFlags.add_JobProperty(prop)
    return


##############################################################################
# SG key properties.
# 

_string_prop ('EventInfoSGKey',              'EventInfo')
_string_prop ('ElectronSGKey',               'AllElectrons')
_string_prop ('GSFTrackAssocSGKey',          'GSFTrackAssociation')
_string_prop ('PhotonSGKey',                 'Photons,PhotonCollection')
_string_prop ('MuonSGKey',                   'StacoMuonCollection,Muons')
_string_prop ('MuonSegmentSGKey',            'ConvertedMBoySegments')
_string_prop ('JetSGKey',                    'AntiKt4TopoEMJets,' +
                                             'AntiKt4LCTopoJets,' +
                                             'AntiKt4TopoAODJets,' +
                                             'AntiKt4H1TopoJets,' +
                                             'AntiKt4H1TopoAODJets,' +
                                             'Cone4H1TopoJets,' +
                                             'Cone4H1TopoAODJets')
_string_prop ('TruthJetSGKey',               'AntiKt4H1TruthJets,' +
                                             #'Cone4TruthJets,' +
                                             'AntiKt4TruthJets')
_string_prop ('MissingETSGKey',              'MET_Reference_AntiKt4EMTopo,MET_RefFinal')
_string_prop ('TauSGKey',                    'TauRecContainer')
_string_prop ('CellsSGKey',                  'AllCalo')
_string_prop ('ClusterSGKey',                'CaloCalTopoClusters,CaloCalTopoCluster')
_string_prop ('EMTopoClusterSGKey',          'EMTopoCluster430,' +
                                             'EMTopoSW35' )
_string_prop ('TrackSGKey',                  'TrackParticleCandidate')
_string_prop ('VertexSGKey',                 'PrimaryVertices')
_string_prop ('MBTSSGKey',                   'MBTSContainer')
_string_prop ('TruthSGKey',                  'TruthParticles')
_string_prop ('TruthParticlesSGKey',         'TruthParticles')
_string_prop ('TruthEventSGKey',             'TruthEvents')
_string_prop ('RawClustersSGKeySuffix',      '_D3PDRawClusters')
_string_prop ('RawClustersAssocSGKeySuffix', '_D3PDRawClustersAssoc')
_string_prop ('LArCollisionTimeSGKey',       'LArCollisionTime')


##############################################################################
# Trigger name pattern properties.
#

_string_prop ('egammaL1TrigPattern',   'L1_2?EM.*')
_string_prop ('ElectronL2TrigPattern', 'L2_2?e.*|L2_L1ItemStreamer_L1_2?EM.*|L2_SeededStreamerL1Calo|L2_SeededStreamerL1CaloEM')
_string_prop ('PhotonL2TrigPattern',   'L2_2?g.*|L2_L1ItemStreamer_L1_2?EM.*|L2_SeededStreamerL1Calo|L2_SeededStreamerL1CaloEM')
_string_prop ('ElectronEFTrigPattern', 'EF_2?e.*|EF_L1ItemStreamer_L1_EM.*|EF_SeededStreamerL1Calo|EF_SeededStreamerL1CaloEM')
_string_prop ('PhotonEFTrigPattern',   'EF_2?g.*|EF_L1ItemStreamer_L1_EM.*|EF_SeededStreamerL1Calo|EF_SeededStreamerL1CaloEM')

_string_prop ('MuonL1TrigPattern',     'L1_2?MU.*')
_string_prop ('MuonL2TrigPattern',     'L2_2?mu.*|L2_L1ItemStreamer_L1_2?MU.*')
_string_prop ('MuonEFTrigPattern',     'EF_2?mu.*|EF_L1ItemStreamer_L1_2?MU.*')


##############################################################################


class AutoFlush (JobProperty):
    """Value to set for ROOT's AutoFlush parameter.
(For ROOT trees only; tells how often the tree baskets will be flushed.)
0 disables flushing.
-1 (default) makes no changes to what THistSvc did.
Any other negative number gives the number of bytes after which to flush.
A positive number gives the number of entries after which to flush."""
    statusOn     = True
    allowedTypes = ['int', 'long']
    StoredValue  = -30000000
jobproperties.D3PDMakerFlags.add_JobProperty(AutoFlush)


class DoTrigger (JobProperty):
    """If true, put trigger information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.D3PDMakerFlags.add_JobProperty(DoTrigger)


class AllowTrigExtend (JobProperty):
    """If true, we can extend the lists of trigger bits if the menu changes."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.D3PDMakerFlags.add_JobProperty(AllowTrigExtend)


class IsEMVersion (JobProperty):
    """Control which IsEM version is used by the data being analyzed.

    1 : < 15.2.0
    2 : >= 15.2.0
    None: try to determine automatically from the version in the file metadata.
"""
    statusOn     = True
    allowedTypes = ['int', None]
    StoredValue  = None
jobproperties.D3PDMakerFlags.add_JobProperty(IsEMVersion)


class HaveEgammaUserData (JobProperty):
    """If true, access results of egamma analysis stored in UserData."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(HaveEgammaUserData)


class MakeEgammaUserData (JobProperty):
    """If true, run egamma analysis to make UserData if not already done."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.D3PDMakerFlags.add_JobProperty(MakeEgammaUserData)


class EgammaUserDataPrefix (JobProperty):
    """Prefix to use for UserData labels for egamma."""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'egammaD3PDAnalysis::'
jobproperties.D3PDMakerFlags.add_JobProperty(EgammaUserDataPrefix)


class HaveJetUserData (JobProperty):
    """If true, access results of jet analysis stored in UserData."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(HaveJetUserData)


class MakeJetUserData (JobProperty):
    """If true, run jet analysis to make UserData if not already done."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.D3PDMakerFlags.add_JobProperty(MakeJetUserData)


class JetUserDataPrefix (JobProperty):
    """Prefix to use for UserData labels for jet."""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'JetD3PDAnalysis::'
jobproperties.D3PDMakerFlags.add_JobProperty(JetUserDataPrefix)


class EgammaShowerDepthTag (JobProperty):
    """If not None, this overrides the setting used for the COOL tag
to get the weights for the calibHitsShowerDepth variable."""
    statusOn = True
    allowedTypes = ['str', None]
    StoredValue = None
jobproperties.D3PDMakerFlags.add_JobProperty(EgammaShowerDepthTag)


class TruthDoPileup (JobProperty):
    """Set to true to include pileup in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(TruthDoPileup)


class TruthWritePartons (JobProperty):
    """Set to true to include partons in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWritePartons)


class TruthPartonPtThresh (JobProperty):
    """Only write partons with Pt above this threshold."""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = -1.*GeV
jobproperties.D3PDMakerFlags.add_JobProperty(TruthPartonPtThresh)


class TruthWriteHadrons (JobProperty):
    """Set to true to include hadrons in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteHadrons)


class TruthWriteGeant (JobProperty):
    """Set to true to include Geant particles in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteGeant)


class TruthGeantPhotonPtThresh (JobProperty):
    """Write Geant photons with Pt above this threshold
Set to < 0 to not write any."""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.5*GeV
jobproperties.D3PDMakerFlags.add_JobProperty(TruthGeantPhotonPtThresh)


class TruthWriteBHadrons (JobProperty):
    """Set to true to include heavy flavor hadrons in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteBHadrons)


class TruthWriteBosons (JobProperty):
    """Set to true to include bosons in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteBosons)


class TruthWriteBSM (JobProperty):
    """Set to true to include BSM particles in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteBSM)


class TruthWriteBosonProducts (JobProperty):
    """Set to true to include bosons in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteBosonProducts)


class TruthWriteBSMProducts (JobProperty):
    """Set to true to include BSM particles in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteBSMProducts)


class TruthWriteTauHad (JobProperty):
    """Set to true to include hadronic tau decays in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteTauHad)


class TruthWriteTopAndDecays (JobProperty):
    """Set to true to include hadronic tau decays in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteTopAndDecays)


class TruthWriteEverything (JobProperty):
    """Set to true to include *everything* in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteEverything)


class TruthWriteAllLeptons (JobProperty):
    """Set to true to include all leptons in truth information."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteAllLeptons)


class TruthWriteFirstN (JobProperty):
    """Set to true to include first N particles in truth information."""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = -1
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteFirstN)


class TruthWriteStatus3 (JobProperty):
    """Set to true to write status code 3 particles into the truth record."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteStatus3)


class TruthWriteExtraJets (JobProperty):
    """Set to true to include additional truth jet collections."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(TruthWriteExtraJets)

class PreD3PDAlgSeqName (JobProperty):
    """Sequence of algorithms to run before the D3PD maker."""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PreD3PDAlgorithms'
jobproperties.D3PDMakerFlags.add_JobProperty(PreD3PDAlgSeqName)


class FilterAlgSeqSuffix (JobProperty):
    """Suffix for a sequence of algorithms to filter D3PD making.

The sequence name is formed by adding this string to the name of the
D3PD making algorithm.  If any of the algorithms in this sequence fail
their filter decision, then no D3PD entry will be made for this event.

The filter sequence may also be referenced by the filterSeq property
of the D3PD algorithm.
"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = '_FilterAlgorithms'
jobproperties.D3PDMakerFlags.add_JobProperty(FilterAlgSeqSuffix)


class FilterCollCand (JobProperty):
    """If true, filter D3PDs on the collision candidate flag.."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(FilterCollCand)

class DoEventSkimming (JobProperty):
    """Turn ON/OFF event selection (skimming) during D3PD making.

Exactly what this does depends on the particular D3PD flavor being made."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.D3PDMakerFlags.add_JobProperty(DoEventSkimming)


class DoPAU (JobProperty):
    """Control whether to run PhotonAnalysisUtils and fill the variables
it calculates."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.D3PDMakerFlags.add_JobProperty(DoPAU)


class RedoEgammaOQFlags (JobProperty):
    """If true, recalculate Object Quality flags for egamma objects."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(RedoEgammaOQFlags)


class RerunTauID (JobProperty):
    """Control whether tau ID should be rerun."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.D3PDMakerFlags.add_JobProperty(RerunTauID)

class RerunTauRec (JobProperty):
        """Controls the upstream algorithms in tauRec."""
        statusOn     = True
        allowedTypes = ['bool']
        StoredValue  = False
jobproperties.D3PDMakerFlags.add_JobProperty(RerunTauRec)


class DoTJVA (JobProperty):
    """Controls doing the TJVA tracks for taus"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.D3PDMakerFlags.add_JobProperty(DoTJVA)


class SaveObjectMetadata (JobProperty):
    """Control whether metadata about the D3PDObjects should be saved."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.D3PDMakerFlags.add_JobProperty(SaveObjectMetadata)


class CompressionLevel (JobProperty):
    """Controls the compression level of the ROOT file produced."""
    statusOn     = True
    allowedTypes = ['int', 'long']
    StoredValue  = 6
jobproperties.D3PDMakerFlags.add_JobProperty(CompressionLevel)


D3PDMakerFlags = jobproperties.D3PDMakerFlags

                 
