# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: ZdcDigitsD3PDObject.py 470583 2011-11-25 10:33:45Z krasznaa $

from   D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import ForwardDetectorsD3PDMaker


ZdcDigitsD3PDObject = make_SGDataVector_D3PDObject( "ZdcDigitsCollection",
                                                    "ZdcDigitsCollection",
                                                    "ZdcDigits_",
                                                    "ZdcDigitsD3PDObject" )

ZdcDigitsD3PDObject.defineBlock( 0,'ZdcDigits',
                                 ForwardDetectorsD3PDMaker.ZdcDigitsFillerTool )
