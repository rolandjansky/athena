# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import TruthD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation


PileUpInfoD3PDObject=make_SG_D3PDObject("PileUpEventInfo","OverlayEvent","pevt_","PileUpInfoD3PDObject")

PileUpObject=ContainedVectorMultiAssociation(PileUpInfoD3PDObject,TruthD3PDMaker.PileUpInfoAssociatorTool,"")

PileUpObject.defineBlock (0,'PileUpInfo',TruthD3PDMaker.PileUpInfoFillerTool)
