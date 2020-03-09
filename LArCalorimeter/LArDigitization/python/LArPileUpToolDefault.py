# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# function to configure LArPileUpTool

from AthenaCommon.Logging import logging

def LArPileUpToolDefault(useLArFloat=True,isOverlay=False,outputKey='LArDigitContainer_MC',outputKey_DigiHSTruth='LArDigitContainer_DigiHSTruth',name='LArPileUpToolDefault'):

    if isOverlay:
        #For all other cases, this is already done by LArConditionsCommon_MC_jobOptions.py
        from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArfSamplMC_LArfSamplSym_ as LArfSamplSymAlg
        condSeq = AthSequencer("AthCondSeq")
        condSeq+=LArfSamplSymAlg(ReadKey="LArfSampl",WriteKey="LArfSamplSym")

    try:
        from AthenaCommon import CfgGetter
        theTool = CfgGetter.getPublicTool("LArPileUpTool")
        theTool.DigitContainer = outputKey
        theTool.DigitContainer_DigiHSTruth = outputKey_DigiHSTruth
        from Digitization.DigitizationFlags import digitizationFlags
        theTool.DoDigiTruthReconstruction = digitizationFlags.doDigiTruth()


    except Exception as configException:
        import traceback
        traceback.print_exc()

    return theTool
