# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagPixModCondD3PDObject.py
## @brief D3PD object for pixel module conditions
## @author Georges Aad
## @date Jun, 2011
##

import JetTagD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject

from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags


def _jetTagPixModCondAssocLevel (reqlev, args):

    if not args.has_key ('PixelSummarySvc') : 
        return False
    if args['PixelSummarySvc'] == None:
        return False

    return True


def makePixModCondD3PDObject (name, prefix, object_name):

    from JetTagD3PDMaker import JetTagPixelModuleGetterTool
    getter =  JetTagPixelModuleGetterTool( name + '_Getter' )

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.ObjFillerTool (name,
                                             Prefix = prefix,
                                             Getter = getter,
                                             ObjectName = object_name,
                                             SaveMetadata = \
                                             D3PDMakerFlags.SaveObjectMetadata())
    
JetTagPixModCondD3PDObject = D3PDObject (makePixModCondD3PDObject, 'deadPixMod_', 'JetTagPixModCondD3PDObject')


JetTagPixModCondD3PDObject.defineBlock (_jetTagPixModCondAssocLevel, 'DeadPixMod',
                                        JetTagD3PDMaker.JetTagPixelModCondFillerTool)


def getJetTagPixModCondD3PDObject(PixSummarySvc=None, **kw):
              
    return  JetTagPixModCondD3PDObject(0, DeadPixMod_PixelSummarySvc=PixSummarySvc, **kw) 

 




