# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$

import CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject

""" level of details:
0: digits
1: digit+SCA index
2: OF Iteration results

Note: 1 and 2 are not available in ESD.   They are available when running recon on raw data. 
"""

LArDigitD3PDObject = make_SGDataVector_D3PDObject( "LArDigitContainer",
                                                   "LArDigitContainer_Thinned",
                                                   "lardigit_", "LArDigitD3PDObject" )

LArDigitD3PDObject.defineBlock( 0, 'Digits',
                                CaloD3PDMaker.LArDigitFillerTool,
                                SaveDigit= True,
                                SaveId = True,
                                SaveSCAAddress= False,
                                DumpIterResults= False )

LArDigitD3PDObject.defineBlock( 1, 'SCA',
                                CaloD3PDMaker.LArDigitFillerTool,
                                SaveDigit= False,
                                SaveId = False,
                                SaveSCAAddress= True,
                                DumpIterResults= False )

LArDigitD3PDObject.defineBlock( 2, 'ITER',
                                CaloD3PDMaker.LArDigitFillerTool,
                                SaveDigit= False,
                                SaveId = False,
                                SaveSCAAddress= False,
                                DumpIterResults= True )
