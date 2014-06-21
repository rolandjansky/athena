# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TauD3PDMaker/python/TauSkimDecisionD3PDObject.py
# @author Dugan O'Neil 
# @date July, 2010
# @brief D3PD object for skim decision flags.
#


import TauD3PDMaker
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject         import make_SG_D3PDObject


TauSkimDecisionD3PDObject = \
           make_SG_D3PDObject ('SkimDecisionCollection',
                               'StreamDESD_MET_SkimDecisionsContainer',
                               'skimDecision_',
                               'TauSkimDecisionD3PDObject')


TauSkimDecisionD3PDObject.defineBlock \
              (0, 'TauSkimDecision',
               EventCommonD3PDMaker.SkimDecisionFillerTool,
               ['Other_LargeMetStream_LargeMetFilter:skim_LargeMETFilter:'
                'True if the Large MET DPD condition passed',
                
                'Other_LargeMetStream_MetFilter:skim_METFilter:'
                'True if the MET DPD condition passed',
                
                'Other_LargeMetStream_FirstMetFilter:skim_FirstMETFilter:'
                'True if the First MET DPD condition passed',
                
                'Other_LargeMetStream_TauFilter:skim_TauFilter:'
                'True if the Tau DPD condition passed',
                
                'Other_LargeMetStream_MediumTauFilter:skim_MediumTauFilter:'
                'True if the Tau DPD condition passed',

                'Other_LargeMetStream_DiTauFilter:skim_DiTauFilter:'
                'True if the DiTau DPD condition passed',
                   ])
