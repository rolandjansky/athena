# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrigTauD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import TrigInDetD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject


#-----------------------------------------------------------------------------
# Trig L2 Tau D3PDObject Maker
#-----------------------------------------------------------------------------
TrigL2TauD3PDObject = make_SGDataVector_D3PDObject( "TrigTauContainer",
                                                    "HLT",
                                                    "trig_L2_tau_",
                                                    "TrigL2TauD3PDObject" )

#-----------------------------------------------------------------------------
# Blocks
#-----------------------------------------------------------------------------
TrigL2TauD3PDObject.defineBlock (0, 'Kinematics',
                                 EventCommonD3PDMaker.FourMomFillerTool,
                                 WriteM = False,
                                 WriteRect = True)

TrigL2TauD3PDObject.defineBlock (0, 'TriggerObjInfo',
                                 TrigTauD3PDMaker.TrigL2TauTrigTauFiller,
                                 DetailLvlInc = False)
TrigL2TauD3PDObject.defineBlock (1, 'TrigTauInfo',
                                 TrigTauD3PDMaker.TrigL2TauTrigTauFiller,
                                 DetailLevel = 1,
                                 DetailLvlInc = False)
TrigL2TauD3PDObject.defineBlock (1, 'Decision',
                                 TrigTauD3PDMaker.TrigL2TauObjectDecisionFillerTool,
                                 AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
                                 Chains = [ "L2_tau.*", "L2_2tau.*" ])

TrigL2TauClusterAssoc = SimpleAssociation \
                        (TrigL2TauD3PDObject,
                         TrigTauD3PDMaker.TrigL2TauClusterAssTool)
TrigL2TauClusterAssoc.defineBlock \
                        (1, 'TrigTauClusterInfo',
                         TrigTauD3PDMaker.TrigL2TauClusterFiller,
                         prefix='cluster_')

TrigL2TauTracksInfoAssoc = SimpleAssociation \
                           (TrigL2TauD3PDObject,
                            TrigTauD3PDMaker.TrigL2TauTracksInfoAssTool)
TrigL2TauTracksInfoAssoc.defineBlock \
                         (1, 'TrigTauTracksInfo',
                          TrigTauD3PDMaker.TrigL2TauTracksInfoFiller,
                          prefix='tracksinfo_')

TrigL2InDetTrackAssoc = SimpleAssociation \
                        (TrigL2TauD3PDObject,
                         TrigTauD3PDMaker.TrigL2InDetTrackAssTool)

TrigL2IDSCANTrackNavAssoc = IndexMultiAssociation\
                            (TrigL2TauD3PDObject,
                             TrigTauD3PDMaker.TrigL2InDetTracksAssTool,
                             target = 'trig_L2_trk_idscan_tau_',
                             prefix="idscan_trk_",
                             blockname = "TrigL2IDSCANTracksNavAssoc",
                             allowMissing = True)

TrigL2SITrackNavAssoc = IndexMultiAssociation\
                        (TrigL2TauD3PDObject,
                         TrigTauD3PDMaker.TrigL2InDetTracksAssTool,
                         target = 'trig_L2_trk_sitrack_tau_',
                         prefix="sitrack_trk_",
                         blockname = "TrigL2SITracksNavAssoc",
                         allowMissing = True)
                         
## EOF
