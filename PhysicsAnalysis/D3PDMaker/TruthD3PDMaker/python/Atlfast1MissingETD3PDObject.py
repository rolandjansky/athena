# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import MissingETD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from MissingETD3PDMaker.MissingETD3PDMakerFlags import MissingETD3PDMakerFlags

Atlfast1MissingETD3PDObject = \
                      make_SG_D3PDObject ('MissingET',
                                          D3PDMakerFlags.MissingETSGKey(),
                                          'MET_RefFinal_',
                                          'Atlfast1MissingETD3PDObject')
Atlfast1MissingETD3PDObject.defineBlock (0, 'MET_RefFinal_',
                                         MissingETD3PDMaker.MissingETFillerTool)

TruthMETD3PDObject = \
                      make_SG_D3PDObject ('MissingEtTruth',
                                          MissingETD3PDMakerFlags.METTruthSGKey(),
                                          'MET_Truth_',
                                          'TruthMETD3PDObject')
TruthMETD3PDObject.defineBlock (1, 'MET_Truth_NonInt_',
                                MissingETD3PDMaker.MissingETTruthNonIntFillerTool)
TruthMETD3PDObject.defineBlock (3, 'MET_Truth_Int_',
                                MissingETD3PDMaker.MissingETD3PDMakerConf.D3PD__MissingETTruthIntFillerTool)
