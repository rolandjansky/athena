# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$

import CaloD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

def makeD3PDObject (name, prefix, object_name, getter = None,
                           sgkey = None,
                           label = None):
    if sgkey == None: sgkey = "LArHits"
    if label == None: label = prefix

    if prefix=="hitemb_" :
        sgkey = "LArHitEMB"
    elif prefix=="hitemec_" :
        sgkey = "LArHitEMEC"
    elif prefix=="hithec_" :
        sgkey = "LArHitHEC"
    elif prefix=="hitfcal_" :
        sgkey = "LArHitFCAL"

    if not getter:
        getter = CaloD3PDMaker.LArHitContainerGetterTool \
                 (name + '_Getter',
                  TypeName = 'LArHitContainer',
                  SGKey = sgkey,
                  Label = label)

    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                SaveMetadata = \
                                                D3PDMakerFlags.SaveObjectMetadata())

LArHitEMBD3PDObject = D3PDObject (makeD3PDObject, 'hitemb_', 'LArHitEMBD3PDObject')

LArHitEMBD3PDObject.defineBlock (1, 'Hits',
                               CaloD3PDMaker.LArHitFillerTool)


LArHitEMECD3PDObject = D3PDObject (makeD3PDObject, 'hitemec_', 'LArHitEMECD3PDObject')

LArHitEMECD3PDObject.defineBlock (1, 'Hits',
                               CaloD3PDMaker.LArHitFillerTool)


LArHitHECD3PDObject = D3PDObject (makeD3PDObject, 'hithec_', 'LArHitHECD3PDObject')

LArHitHECD3PDObject.defineBlock (1, 'Hits',
                               CaloD3PDMaker.LArHitFillerTool)


LArHitFCALD3PDObject = D3PDObject (makeD3PDObject, 'hitfcal_', 'LArHitFCALD3PDObject')

LArHitFCALD3PDObject.defineBlock (1, 'Hits',
                               CaloD3PDMaker.LArHitFillerTool)

