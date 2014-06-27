# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetRec.JetMomentGetter               import make_JetMomentGetter
from JetD3PDMaker.JetD3PDObject           import JetD3PDObject
import JetD3PDMaker
from JetTagD3PDMaker.JetTagD3PDMakerKeys  import JetTagD3PDKeys

from PhotonD3PDMaker.PhotonD3PDMakerFlags import PhotonD3PDMakerFlags
from PhotonD3PDMaker.PhotonD3PD           import _args

def AddJetsToD3PD(alg, **kw):
    for i in PhotonD3PDMakerFlags.JetCollections():
        
        collname=i
        if PhotonD3PDMakerFlags.EnableBTagging():
            collname=i+PhotonD3PDMakerFlags.BTaggingSuffix()

        # This will change the following variables:
        # _jvtx_x, _jvtx_y, _jvtx_z, _sumPtTrk, _nTrk, _OriginIndex, _isBad*, 
        if PhotonD3PDMakerFlags.ReCalculateJetMoments():
            make_JetMomentGetter(collname, [jvatool])
    
        collection_name = "jet_"+i+"_"
        jetd3pd = JetD3PDObject              (**_args ( 3, i,  kw,
                                                        prefix=collection_name,
                                                        sgkey=collname,
                                                        include=[JetTagD3PDKeys.TruthInfoBlockName(),
                                                                 JetTagD3PDKeys.BTagWeightsBlockName(),
                                                                 JetTagD3PDKeys.IP2DInfoBaseBlockName(),
                                                                 JetTagD3PDKeys.IP2DIPInfoBaseBlockName(),
                                                                 JetTagD3PDKeys.IP3DInfoBaseBlockName(),
                                                                 JetTagD3PDKeys.IP3DIPInfoBaseBlockName(),
                                                                 JetTagD3PDKeys.SV1InfoBaseBlockName(),
                                                                 JetTagD3PDKeys.SV2InfoBaseBlockName(),
                                                                 JetTagD3PDKeys.JetFitterInfoBaseBlockName(),
                                                                 JetTagD3PDKeys.JetFitterCombInfoBaseBlockName(),
                                                                 JetTagD3PDKeys.JetFitterTagInfoBlockName(),
                                                                 JetTagD3PDKeys.SVInfoPlusBlockName(),
                                                                 JetTagD3PDKeys.SV0InfoPlusBlockName(),
                                                                 'ConstituentScale',
                                                                 'Samplings',
                                                                 'TracksMoments'],
                                                        JetVertexFraction_FromUD=PhotonD3PDMakerFlags.FillFullJVF(),
                                                        JetVertexFraction_FillFullJVF=PhotonD3PDMakerFlags.FillFullJVF(),
                                                        allowMissing=True))

        alg += jetd3pd
