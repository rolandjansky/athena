include.block("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

# our own skimming algs
from HSG2DPDUtils import H4lDPDMaker

from HSG2DPDUtils.CaloMuonOverlapFilter import CaloMuonOverlapFilter

import HSG2DPDUtils.HSG2DPDFlags

# shortcuts

eQual=jobproperties.HSG2.eQual()
muQual=jobproperties.HSG2.muQual()

eCollToUse='ElectronAODCollection'

# schedule the calo muon overlap removal
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

theJob += CaloMuonOverlapFilter(inCaloColl='CaloMuonCollection',
                                outCaloColl='UniqueCaloMuonCollection',
                                combCollections=['StacoMuonCollection','MuidMuonCollection'])

