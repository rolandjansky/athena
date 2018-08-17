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
    from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

    monTool = GenericMonitoringTool("RingerFexMon")
    monTool.Histograms = [ defineHistogram( "TIME_total",      title="Total Time;time[ms]",         xbins=50, xmin=0, xmax=100 ),
                           defineHistogram( "TIME_preprocess", title="Preprocessing Time;time[ms]", xbins=50, xmin=0, xmax=50 ),
                           defineHistogram( "TIME_decision",   title="Decision Time;time[ms]",      xbins=50, xmin=0, xmax=50 )]
    ringer.MonTool = monTool
    return ringer
