# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import MissingETD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from MissingETD3PDMaker.MissingETD3PDMakerFlags import MissingETD3PDMakerFlags

MissingETGoodnessD3PDObject = \
                      make_SG_D3PDObject ('MissingET',
                                          MissingETD3PDMakerFlags.METRefFinalSGKey(),
                                          'MET_Goodness_',
                                          'MissingETGoodnessD3PDObject')
def _goodnessHook (c, **kw):
    # MissingETGoodness creates a JetBadChanCorrTool, but doesn't use the
    # standard configuration code, so the require THistSvc entry
    # doesn't get made.  Work around.
    from JetMomentTools.SetupJetMomentTools import getJetBadChanCorrTool
    getJetBadChanCorrTool ('Kt', 0.4, 'Topo')

    from AthenaCommon.AlgSequence import AlgSequence
    from MissingETGoodness.METGoodnessGetter import METGoodnessGetter
    METGoodnessGetter (AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName()))
    return
MissingETGoodnessD3PDObject.defineHook (_goodnessHook)

MissingETGoodnessD3PDObject.defineBlock ( 2, 'Goodness', MissingETD3PDMaker.MissingETGoodnessFillerTool )


#### Jet Collection Info Object
JetsInfoMETD3PDObject = \
                      make_SG_D3PDObject ('JetCollection',
                                                    MissingETD3PDMakerFlags.METDefaultJetCollectionSGKey(),
                                                    'METJetsInfo_')
JetsInfoMETD3PDObject.defineBlock (4, 'JetsInfo',
                                   MissingETD3PDMaker.MissingETJetsInfoFillerTool)






