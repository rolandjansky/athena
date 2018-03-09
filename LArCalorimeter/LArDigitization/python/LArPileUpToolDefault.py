# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# function to configure LArPileUpTool

from AthenaCommon.Logging import logging

def LArPileUpToolDefault(useLArFloat=True,isOverlay=False,outputKey='LArDigitContainer_MC',outputKey_DigiHSTruth='LArDigitContainer_DigiHSTruth',name='LArPileUpToolDefault'):

    try:
        from AthenaCommon import CfgGetter
        theTool = CfgGetter.getPublicTool("LArPileUpTool")
        theTool.DigitContainer = outputKey
        theTool.DigitContainer_DigiHSTruth = outputKey_DigiHSTruth
        from Digitization.DigitizationFlags import digitizationFlags
        theTool.DoDigiTruthReconstruction = digitizationFlags.doDigiTruth()
    except Exception as configException:
        print configException
        print "ERROR Problem with configuration"

    return theTool
