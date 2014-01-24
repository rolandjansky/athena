# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: ZdcD3PDObject.py 470583 2011-11-25 10:33:45Z krasznaa $

from   D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import ForwardDetectorsD3PDMaker

ZdcD3PDObject = make_SGDataVector_D3PDObject( "ZdcRawChannelCollection",
                                              "ZdcRawChannelCollection",
                                              "Zdc_",
                                              "ZdcD3PDObject" )

ZdcD3PDObject.defineBlock( 0, 'ZdcRawChannel',
                           ForwardDetectorsD3PDMaker.ZdcFillerTool )
