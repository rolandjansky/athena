# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from TrackD3PDMaker import TrackD3PDMakerConf
from TrackD3PDMaker.VertexD3PDObject import DefineVertexD3PDObject

def BuildV0D3PDObject(_prefix='v0_',
                      _label='v0',
                      _sgkey='SimpleV0Candidates',
                      _object_name='V0D3PDObject'):


    def makeV0D3PDObject (name, prefix, object_name,
                          getter = None,
                          sgkey = _sgkey,
                          label = _label):

        if sgkey == None:
            sgkey = 'SimpleV0Candidates'
            
        if label == None:
            label = prefix
                
        if not getter:
            from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__V0ContainerGetterTool
            getter = D3PD__V0ContainerGetterTool\
                     (name + '_Getter',
                      TypeName = 'V0Container',
                      SGKey = sgkey,
                      Label = label)

        from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags            
        return D3PDMakerCoreComps.VectorFillerTool (name,
                                                    Prefix = prefix,
                                                    Getter = getter,
                                                    ObjectName = object_name,
                                                    AllowMissing = True,
                                                    SaveMetadata = \
                                                    D3PDMakerFlags.SaveObjectMetadata())

    object = D3PDObject(makeV0D3PDObject, _prefix, _object_name)

    object.defineBlock (0, 'V0Attributes',
                        TrackD3PDMaker.V0CandidateFillerTool)

    VxCandidateAssoc = SimpleAssociation\
                       (object,
                        TrackD3PDMaker.V0CandidateVxCandidateAssociationTool)
    
    DefineVertexD3PDObject(VxCandidateAssoc,
                           trackTarget='trk',
                           trackPrefix='trk_',
                           trackType='Rec::TrackParticleContainer')
    return object

V0D3PDObject = BuildV0D3PDObject ('v0_', 'v0',
                                  'SimpleV0Candidates',
                                  'V0D3PDObject')

DefaultV0D3PDObject = BuildV0D3PDObject ('v0_', 'v0',
                                         'V0Candidates',
                                         'DefaultV0D3PDObject')
