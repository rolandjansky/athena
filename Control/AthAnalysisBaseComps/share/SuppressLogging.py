MessageSvc.setWarning += ["ClassIDSvc",
                          "PoolSvc",
                          "AthDictLoaderSvc",
                          "AthenaPoolAddressProviderSvc",
                          "ProxyProviderSvc",
                          "DBReplicaSvc",
                          "MetaDataSvc",
                          "MetaDataStore",
                          "AthenaPoolCnvSvc",
                          "TagMetaDataStore",
                          "EventSelector",
                          #"ApplicationMgr", can't silence because otherwise ATN tests fail, see ATLINFR-1235
                          "CoreDumpSvc",
                          "AthMasterSeq",
                          "EventPersistencySvc",
                          "ActiveStoreSvc",
                          "AthenaEventLoopMgr",
                          "AthOutSeq",
                          "AthRegSeq"]

#also silence storegates if not dumping
if not hasattr(StoreGateSvc,"Dump") or not StoreGateSvc.Dump: MessageSvc.setWarning += ["StoreGateSvc"]
if hasattr(svcMgr,"InputMetaDataStore"):
    if not hasattr(svcMgr.InputMetaDataStore,"Dump") or not svcMgr.InputMetaDataStore.Dump: MessageSvc.setWarning += ["InputMetaDataStore"]
else:
    MessageSvc.setWarning += ["InputMetaDataStore"]

MessageSvc.setError += ["HistogramPersistencySvc"] #even stricter for HistogramPersistencySvc too .. gives a silly/harmless warning otherwise

MessageSvc.setError += ["PoolSvc"] #even stricter for PoolSvc, to silence RootCollectionScehemeEditor warnings

#do this to suppress logging from a reinitialize of the eventselector
for svc in MessageSvc.setWarning:
    if hasattr(svcMgr,svc): getattr(svcMgr,svc).OutputLevel=WARNING

for svc in MessageSvc.setError:
    if hasattr(svcMgr,svc): getattr(svcMgr,svc).OutputLevel=ERROR


if not hasattr(svcMgr,"DBReplicaSvc"):
    svcMgr += CfgMgr.DBReplicaSvc()
svcMgr.DBReplicaSvc.OutputLevel=WARNING

