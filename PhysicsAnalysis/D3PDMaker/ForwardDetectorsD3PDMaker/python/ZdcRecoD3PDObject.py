# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: ZdcRecoD3PDObject.py 538891 2013-02-25 04:56:06Z leite $

from   D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
import ForwardDetectorsD3PDMaker

ZdcRecoD3PDObject = make_SG_D3PDObject( "ZdcDigitsCollection",
                                        "ZdcDigitsCollection",
                                        "ZdcReco_",
                                        "ZdcRecoD3PDObject" )

ZdcRecoD3PDObject.defineBlock( 0, 'ZdcDigitsCollection',
                               ForwardDetectorsD3PDMaker.ZdcRecoFillerTool )
