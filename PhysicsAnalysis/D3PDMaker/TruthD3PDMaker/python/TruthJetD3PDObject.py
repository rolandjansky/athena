# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import EventCommonD3PDMaker
import JetD3PDMaker
from D3PDMakerCoreComps.D3PDObject      import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags     import D3PDMakerFlags
from RecExConfig.RecFlags               import rec

TruthJetD3PDObject = make_SGDataVector_D3PDObject ('JetCollection',
                                                   D3PDMakerFlags.JetSGKey(),
                                                   'truthjet_',
                                                   'TruthJetD3PDObject')

TruthJetD3PDObject.defineBlock(0, 'Kinematics',
                               EventCommonD3PDMaker.FourMomFillerTool,
                               WriteE  = True)

TruthJetD3PDObject.defineBlock(2, 'JetShape',
                               JetD3PDMaker.JetShapeFillerTool)

if rec.doTruth:
    TruthJetD3PDObject.defineBlock(2, 'TrueFlavorComponents',
                                   JetD3PDMaker.JetTrueTagFillerTool)

