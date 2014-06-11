#===========================================================================
#===========================================================================
# Description:
#  File is loaded from CompareAll.py and deal with the "special" plots
#
# Author:
#  John Alison <johnda@hep.upenn.edu>
#===========================================================================

# List of the histograms were we want ot do more that the standard procProfile or procHistogram
# This is a dictionarry of the histogram names and a list
#  The list has as its first elemet the function object to be called instead of procProfile or procHistogram
#         the second object is an argument to pass the function object. (This can be extended if needed)
isAnException = {"trt_b_residualR":[processAndFitTRTResiduals,"#mum"]
                 ,"trt_b_residualRSide_A":[processAndFitTRTResiduals,"#mum"]
                 ,"trt_b_residualRSide_C":[processAndFitTRTResiduals,"#mum"]
                 ,"trt_b_residualRSide_A":[processAndFitTRTResiduals,"#mum"]
                 ,"trt_b_residualR_Endcap_A":[processAndFitTRTResiduals,"#mum"]
                 ,"trt_b_residualR_Endcap_C":[processAndFitTRTResiduals,"#mum"]
                 }

trackParamNames = ["delta_d0","delta_z0","delta_eta0","delta_phi0","delta_qOverPt"]
trackParamUnits = ["mm","mm","eta","rad","GeV^{-1}"]
trackParamMiddles = ["","_Pull"]
trackParamEndings = ["","_n","_p"]

for tp in range(len(trackParamNames)):
    for tm in range(len(trackParamMiddles)):
        for te in range(len(trackParamEndings)):
            isAnException[trackParamNames[tp]+trackParamMiddles[tm]+trackParamEndings[te]] = [processAndQuoteHistogram,trackParamUnits[tp]]
