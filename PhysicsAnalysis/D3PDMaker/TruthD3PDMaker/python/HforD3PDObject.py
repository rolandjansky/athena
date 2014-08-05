# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
import TruthD3PDMaker
#import TopInputsD3PDMaker

HforD3PDObject = make_Void_D3PDObject( "top_", "HforD3PDObject",
                                       default_name = "HforFiller" )

HforD3PDObject.defineBlock( 0, "HforBits",
                            TruthD3PDMaker.HforFillerTool)

