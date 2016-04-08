
#needs to happen at end of joboptions
svcMgr.MessageSvc.OutputLevel=WARNING
if hasattr(svcMgr,"PoolSvc") : svcMgr.PoolSvc.OutputLevel=ERROR #even stricter for PoolSvc! Fixme: maybe just add to setError below?

svcMgr.MessageSvc.setInfo = Configurable.allConfigurables.keys()
svcMgr.MessageSvc.setError = ["HistogramPersistencySvc"] #even stricter for HistogramPersistencySvc too .. gives a silly/harmless warning otherwise

excludeList = ['CoreDumpSvc','TopAlg', 'DataModelCompatSvc', 'EventSelector', 'MetaDataSvc', 'ChronoStatSvc', 'AthDictLoaderSvc', 'AuditorSvc', 'ClassIDSvc', 'AthOutSeq', 'RndmGenSvc', 'InputMetaDataStore', 'StatusCodeSvc', 'AthMasterSeq', 'AthenaRootStreamerSvc', 'AthRegSeq', 'IOVDbMetaDataTool', 'Streams', 'ProxyProviderSvc', 'AlgContextAuditor', 'NTupleSvc', 'AthenaPoolCnvSvc', 'EventPersistencySvc', 'AthenaSealSvc', 'IncidentSvc', 'PoolSvc', 'ApplicationMgr', 'MetaDataStore', 'ServiceManager', 'AthenaPoolAddressProviderSvc', 'HistogramDataSvc', 'HistoryStore']
for x in excludeList : 
    while x in svcMgr.MessageSvc.setInfo : svcMgr.MessageSvc.setInfo.remove(x)
