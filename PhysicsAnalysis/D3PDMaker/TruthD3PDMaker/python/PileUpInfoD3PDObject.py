# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TruthD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import *
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from TruthD3PDMaker.TruthD3PDMakerConf import *
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation


PileUpInfoD3PDObject=make_SG_D3PDObject("PileUpEventInfo","OverlayEvent","pevt_","PileUpInfoD3PDObject")

PileUpObject=ContainedVectorMultiAssociation(PileUpInfoD3PDObject,TruthD3PDMaker.PileUpInfoAssociatorTool,"")

PileUpObject.defineBlock (0,'PileUpInfo',TruthD3PDMaker.PileUpInfoFillerTool)
