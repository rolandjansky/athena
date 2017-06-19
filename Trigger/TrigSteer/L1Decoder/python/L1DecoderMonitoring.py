from AthenaMonitoring.AthenaMonitoringConf import GenericMonitoringTool
from AthenaMonitoring.DefineHistogram import defineHistogram


def CTPUnpackingMonitoring(maxItems, maxChains):
    tool = GenericMonitoringTool('CTPUnpackingMonitoring')
    tool.HistogramsGroupName="L1Decoder";
    tool.Histograms = [defineHistogram('TAVItems', path='EXPERT', type='TH1F', title='Number of active L1 TAV items;N Items;N Events',
                                       xbins=maxItems, xmin=0, xmax=maxItems),
                       defineHistogram('Chains', path='EXPERT', type='TH1F', title='Number of activated Chains;N Chains;N Events',
                                       xbins=maxChains, xmin=0, xmax=maxChains) ]
    return tool
