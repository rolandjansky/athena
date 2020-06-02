#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
import math

def CTPUnpackingMonitoring( maxItems, maxChains ):
    tool = GenericMonitoringTool( 'CTPUnpackingMonitoring' )
    tool.HistPath="HLTFramework/L1Decoder"
    tool.Histograms = [defineHistogram( 'TAVItems', path='EXPERT', type='TH1F', title='Number of active L1 TAV items;N Items;N Events',
                                       xbins=maxItems, xmin=0, xmax=maxItems ),
                       defineHistogram( 'Chains', path='EXPERT', type='TH1F', title='Number of activated Chains;N Chains;N Events',
                                       xbins=maxChains, xmin=0, xmax=maxChains ) ]
    return tool


def RoIsUnpackingMonitoring( prefix, maxCount, maxEta=3. ):    
    tool = GenericMonitoringTool( prefix+'RoIsUnpackingMonitoring' )
    tool.HistPath="HLTFramework/L1Decoder/RoIs"+prefix
    tool.Histograms = [ defineHistogram( 'count', path='EXPERT', type='TH1F', title='Number of RoIs;N RoIs;N Events',
                                        xbins=maxCount, xmin=0, xmax=maxCount ),
                        defineHistogram( 'eta', path='EXPERT', type='TH1F', title='Eta distribution;eta;N RoIs',
                                        xbins=60, xmin=-maxEta, xmax=maxEta ),
                        defineHistogram( 'phi', path='EXPERT', type='TH1F', title='Phi distribution;phi;N RoIs',
                                        xbins=64, xmin=-math.pi, xmax=math.pi ),
                        defineHistogram( 'eta, phi', path='EXPERT', type='TH2F', title='RoIs map;eta;phi',
                                        xbins=30, xmin=-maxEta, xmax=maxEta,
                                        ybins=32, ymin=-math.pi, ymax=math.pi ) ]
    return tool

def PrescalingMonitoring():
    tool = GenericMonitoringTool('PrescalingMonitoring')
    tool.HistPath="HLTFramework/L1Decoder"
    tool.defineHistogram('LB,HLTPSK', path='EXPERT', type='TH2I', opt='kAddBinsDynamically',
                         title='HLT prescale key used in LB;Lumiblock;HLTPSK',
                         xbins=100, xmin=0, xmax=100, ybins=5, ymin=0, ymax=5)
    return tool
