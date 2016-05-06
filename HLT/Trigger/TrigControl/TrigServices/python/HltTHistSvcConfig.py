# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigServicesConf import HltTHistSvc

# just a helper function
# def defineTHistSvcOutputFile(dir, name, service, opt='NEW'):
#     if globals().has_key('numberOfThreads'):
#         threads = globals()['numberOfThreads']
#     else:
#         threads=0
        
#     if threads.value() == 0:
#         THistSvc = Service(service)
#         THistSvc.Output += [dir+" DATAFILE=\'"+name+"' OPT=\'"+opt+"\'" ]
#         return
#     for n in range(0,threads.value()):
#         threadID = '__%(#)d' % { '#': n}
#         THistSvc = Service ( service+threadID )
#         THistSvc.Output += [dir+" DATAFILE=\'"+name+threadID+"' OPT=\'"+opt+"\'" ]


class HltTHistSvcConfig(HltTHistSvc):
    def __init__(self, name='THistSvc'):
        super(HltTHistSvcConfig, self).__init__(name)
        self.Output += ["SHIFT   DATAFILE='shift-monitoring.root'   OPT='RECREATE'"]
        self.Output += ["EXPERT  DATAFILE='expert-monitoring.root'  OPT='RECREATE'"]
        self.Output += ["DEBUG   DATAFILE='debug-monitoring.root'   OPT='RECREATE'"]
        self.Output += ["RUNSTAT DATAFILE='runstat-monitoring.root' OPT='RECREATE'"]        
