#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from TrigMultiVarHypo.TrigL2CaloRingerCutDefs import TrigL2CaloRingerCutDefs
from TrigMultiVarHypo.TrigMultiVarHypoConf import TrigL2CaloRingerFexMT

def init_ringer():
    
    ringer = TrigL2CaloRingerFexMT()
    
    threshold = 15
    IDinfo = 'loose'
    trigType = 'e'
    theRingerConfig = TrigL2CaloRingerCutDefs(threshold,IDinfo,trigType)
    
    #Prepoc configuration
    ringer.NRings             = theRingerConfig.NRings
    ringer.SectionRings       = theRingerConfig.SectionRings
    ringer.NormalisationRings = theRingerConfig.NormalisationRings

    #MultiLayerPerceptron configuration
    ringer.Nodes       = theRingerConfig.Nodes
    ringer.Threshold  = theRingerConfig.Thresholds
    ringer.Bias        = theRingerConfig.Bias
    ringer.Weights     = theRingerConfig.Weights
    ringer.EtaBins     = theRingerConfig.EtaBins
    ringer.EtBins      = theRingerConfig.EtBins
    
    return ringer
