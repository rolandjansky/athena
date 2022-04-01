# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from collections import defaultdict
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

_VERTEX_DECO         = 'TTVA_AMVFVertices_forReco'
_WEIGHT_DECO         = 'TTVA_AMVFWeights_forReco'


# Create a dictionnary which auto-generates a number for each unique entry it is given
# this is used to generate a unique alg name for different pairs of trk,vtx container names
_trkvtxPairCounter = defaultdict()
_trkvtxPairCounter.default_factory = lambda : str(len(_trkvtxPairCounter))

def idForTrackVtxContainers(trackCont, vtxCont, vtxdeco, wdeco):
    return "InDetUsedInFitDecorator_"+_trkvtxPairCounter[ (trackCont, vtxCont, vtxdeco, wdeco) ]


def getUsedInVertexFitTrackDecoratorAlg(trackCont = 'InDetTrackParticles', vtxCont = 'PrimaryVertices'):
    """ Create the alg  to decorate the used-in-fit information for AMVF """
    alg = CompFactory.getComp("InDet::InDetUsedInVertexFitTrackDecorator")(
        idForTrackVtxContainers(trackCont,vtxCont, _VERTEX_DECO, _WEIGHT_DECO)+"Alg" ,
        UsedInFitDecoratorTool=CompFactory.getComp("InDet::InDetUsedInFitTrackDecoratorTool")(
            "UsedInFitTrackDecoratorTool",
            AMVFVerticesDecoName= _VERTEX_DECO,
            AMVFWeightsDecoName=_WEIGHT_DECO,
            TrackContainer=trackCont,
            VertexContainer=vtxCont,
        ),
    )
    return alg

def UsedInVertexFitTrackDecoratorCfg(flags, trackCont = 'InDetTrackParticles', vtxCont = 'PrimaryVertices' ):
    """ Create the ComponentAccumulator  to decorate the used-in-fit information for AMVF """
    acc = ComponentAccumulator()
    acc.addEventAlgo( getUsedInVertexFitTrackDecoratorAlg( trackCont, vtxCont ) )
    return acc
