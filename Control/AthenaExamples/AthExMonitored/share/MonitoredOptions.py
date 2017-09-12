from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

from AthExMonitored.AthExMonitoredConf import MonitoredAlg
job += MonitoredAlg('MonAlg')

from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

monTool = GenericMonitoringTool('MonTool')

#monTool.HistPath = 'MyGroup/MySubDir'  # default is the parent name of MonTool
monTool.Histograms = [defineHistogram( 'nTracks', path='EXPERT', type='TH1F', title='Counts',
                                       xbins=10, xmin=0, xmax=10 ),
                      defineHistogram( 'eta', path='EXPERT', type='TH1F', title='#eta;;Entries',
                                       xbins=30, xmin=-3, xmax=3 ),
                      defineHistogram( 'AbsPhi', path='EXPERT', type='TH1F', title='|#phi|;;Entries',
                                       xbins=10, xmin=0, xmax=3.15 ),
                      defineHistogram( 'TIME_execute', path='EXPERT', type='TH1F', title='Time for execute',
                                       xbins=100, xmin=0, xmax=100 ) ]

job.MonAlg.MonTool = monTool

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc(Output = ["EXPERT DATAFILE='expert-monitoring.root', OPT='RECREATE'"])

theApp.EvtMax = 10
