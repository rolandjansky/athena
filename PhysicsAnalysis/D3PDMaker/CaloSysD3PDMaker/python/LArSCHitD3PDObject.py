# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import CaloSysD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from   D3PDMakerCoreComps.D3PDObject import (make_SG_D3PDObject, make_SGDataVector_D3PDObject)
from math import pi

def _hookForLArSCHitD3PDObject_(c, *arg, **kw ):

    basFiller = getattr(c, c.name() + '_Basic', None)
    print("getattr(c, c.name()) / Type= ", type(basFiller))
    if "CaloEtaCut" in list(kw.keys()):
        basFiller.CaloEtaCut = kw["CaloEtaCut"]
    if "CaloPhiCut" in list(kw.keys()):
        basFiller.CaloPhiCut = kw["CaloPhiCut"]
    if "CaloLayers" in list(kw.keys()):
        basFiller.CaloLayers = kw["CaloLayers"]
    if "CaloDetectors" in list(kw.keys()):
        basFiller.CaloDetectors = kw["CaloDetectors"]

    print("%s - CaloEtaCut = " % (basFiller.name()), basFiller.CaloEtaCut)
    print("%s - CaloPhiCut = " % (basFiller.name()), basFiller.CaloPhiCut)
    print("%s - CaloLayersCut = " % (basFiller.name()), basFiller.CaloLayers)
    print("%s - CaloDetectors = " % (basFiller.name()), basFiller.CaloDetectors)

    return 

def _makeLArSCHit_obj_(name, prefix, object_name,
                       getter=None,
                       sgKey=None,
                       typeName=None,
                       ):
    if not typeName:
        typeName = "LArHitContainer"
    if not sgKey:
        sgKey="LArHitEMB"
    if not getter:
        from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
        getter = D3PDMakerCoreComps.SGObjGetterTool(
            name + '_Getter',
            TypeName = typeName,
            SGKey = sgKey)

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.ObjFillerTool( name,
                                             Prefix = prefix,
                                             Getter = getter,
                                             ObjectName = object_name,
                                             SaveMetadata = \
                                             D3PDMakerFlags.SaveObjectMetadata()
                                             )
    
def make_LArSCHitD3PDObject( typeName="LArHitContainer",
                             sgKey="LArHitEMB",
                             prefix="sch_",
                             object_name="LArSCHitD3PDObject",
                             *args, **kw ):
        
    obj = D3PDObject(_makeLArSCHit_obj_, prefix, object_name,
                     allow_args=["CaloEtaCut","CaloPhiCut",
                                 "CaloLayers","CaloDetectors"],
                     sgkey=sgKey,
                     typename=typeName)
    obj.defineBlock( 0, 'Basic', CaloSysD3PDMaker.LArSCHitFillerTool)
    obj.defineHook( _hookForLArSCHitD3PDObject_ )
    return obj

LArSCHitD3PDObject = make_LArSCHitD3PDObject( typeName="LArHitContainer",
                                             sgKey = "LArHitEMB",
                                             prefix = "hsc_",
                                             CaloEtaCut=[],
                                             CaloPhiCut=[],
                                             CaloLayers=[],
                                             CaloDetectors=[],)
#
#      The arguments to the hook function are the object filler tool,
#      followed by the arguments that were passed to __call__.
#


"""
THIS WORKS BUT NO SELECTION DYNAMICALLY....
LArSCHitD3PDObject = make_SG_D3PDObject( "LArHitContainer",
                                         "LArHitEMB",
                                         "sch_","LArSCHitD3PDObject",
                                         allow_args=["CaloEtaCut" "CaloPhiCut","CaloLayers","CaloDetectors"]                                         )
LArSCHitD3PDObject.defineBlock( 0, 'Basic', CaloSysD3PDMaker.LArSCHitFillerTool,
                                CaloEtaCut=[0.4,0.425],
                                CaloPhiCut=[-pi,pi],
                                CaloLayers=[0,1,2,3],
                                CaloDetectors=[1]
                                )


"""




