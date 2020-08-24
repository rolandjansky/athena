# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def MonitoredCfg():
    result = ComponentAccumulator()
    monAlg = CompFactory.MonitoredAlg('MonAlg')

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')

    #monTool.HistPath = 'MyGroup/MySubDir'  # default is the parent name of MonTool
    monTool.defineHistogram( 'nTracks', path='EXPERT', type='TH1F', title='Counts',
                             xbins=10, xmin=0, xmax=10 )
    monTool.defineHistogram( 'eta', path='EXPERT', type='TH1F', title='#eta;;Entries',
                             xbins=30, xmin=-3, xmax=3 )
    monTool.defineHistogram( 'AbsPhi', path='EXPERT', type='TH1F', title='|#phi|;;Entries',
                             xbins=10, xmin=0, xmax=3.15 )
    monTool.defineHistogram( 'eta,AbsPhi', path='EXPERT', type='TH2F', title='#eta vs #phi',
                             xbins=15, xmin=-3, xmax=3, ybins=15, ymin=0, ymax=3.15 )
    monTool.defineHistogram( 'TIME_execute', path='EXPERT', type='TH1F', title='Time for execute',
                             xbins=100, xmin=0, xmax=100 )

    monAlg.MonTool = monTool

    histSvc = CompFactory.THistSvc(Output = ["EXPERT DATAFILE='expert-monitoring.root', OPT='RECREATE'"])

    result.addEventAlgo(monAlg)
    result.addService(histSvc)
    return result


if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Exec.MaxEvents=10
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(MonitoredCfg())
    cfg.run()
