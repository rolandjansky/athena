# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from   D3PDMakerCoreComps.D3PDObject import (make_SG_D3PDObject, make_SGDataVector_D3PDObject)
from math import pi

import CaloSysD3PDMaker

AllSCD3PDObject = make_SG_D3PDObject( "CaloCellContainer",
                                   "AllCalo",
                                   "sc_",
                                   "SCD3PDObject" )
AllSCD3PDObject.defineBlock( 0, 'Basic', CaloSysD3PDMaker.SCFillerTool )


def _hookForSCD3PDObject_(c, *arg, **kw ):

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
    if "TileDLayerOption" in list(kw.keys()):
        basFiller.TileDLayerOption = kw["TileDLayerOption"]
        
    print("%s - CaloEtaCut = " % (basFiller.name()), basFiller.CaloEtaCut)
    print("%s - CaloPhiCut = " % (basFiller.name()), basFiller.CaloPhiCut)
    print("%s - CaloLayersCut = " % (basFiller.name()), basFiller.CaloLayers)
    print("%s - CaloDetectors = " % (basFiller.name()), basFiller.CaloDetectors)
    #print "%s - TileDLayerOption = " % (basFiller.name()), basFiller.TileDLayerOption
    return 

def _makeSC_obj_(name, prefix, object_name,
                       getter=None,
                       sgKey=None,
                       typeName=None,
                       ):
    if not typeName:
        typeName = "CaloCellContainer"
    if not sgKey:
        sgKey="AllCalo"
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
    
def make_SCD3PDObject( typeName="CaloCellContainer",
                             sgKey="AllCalo",
                             prefix="sc_",
                             object_name="SCD3PDObject",
                             *args, **kw ):
        
    obj = D3PDObject(_makeSC_obj_, prefix, object_name,
                     allow_args=["CaloEtaCut","CaloPhiCut",
                                 "CaloLayers","CaloDetectors","TileDLayerOption"],
                     sgkey=sgKey,
                     typename=typeName)
    obj.defineBlock( 0, 'Basic', CaloSysD3PDMaker.SCFillerTool)
    obj.defineHook( _hookForSCD3PDObject_ )
    return obj

SCD3PDObject = make_SCD3PDObject( typeName="CaloCellContainer",
                                   sgKey = "AllCalo",
                                   prefix = "sc_",
                                   CaloEtaCut=[],
                                   CaloPhiCut=[],
                                   CaloLayers=[],
                                   CaloDetectors=[],
                                   TileDLayerOption=False )
