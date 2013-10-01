theApp.EvtMax = 2

svcMgr += CfgMgr.AthTPCnvSvc(OutputLevel=Lvl.VERBOSE)
theApp.CreateSvc += ["AthTPCnvSvc/AthTPCnvSvc"]
theApp.run(1)
if 0:
    theApp.Dlls += ["AthExThinningPoolCnv"]

svc = PyAthena.py_svc("AthTPCnvSvc", iface='ITPCnvSvc')
cnvs = []

ti_name = PyAthena.System.typeinfoName

for n in ['AthExDecayCnv_p1',
          'AthExElephantinoCnv_p1',
          'AthExParticlesCnv_p1',
          'AthExFatObjectCnv_p1',
          'TruthEtIsolationsCnv_p1',
          'TruthEtIsolationsContainerCnv_p1',
          'TruthParticleContainerCnv_p1',
          'TruthParticleContainerCnv_p6',
          'RootTruthParticleContainerCnv_p4',
          ]:
    print "::: loading T/P converter [%s]..." % (n,)
    cnv = svc.load_tpcnv(n)
    if cnv:
        print " =>", cnv
        print " -> (%s => %s)" % (ti_name(cnv.transientTInfo()),
                                  ti_name(cnv.persistentTInfo()))
        cnvs.append(cnv)
        ## trans_obj = getattr(PyAthena, ti_name(cnv.transientTInfo()))
        ## pers_obj = getattr(PyAthena, ti_name(cnv.persistentTInfo()))
        ## print " -> t:",trans_obj
        ## print " -> p:",pers_obj
        ## cnv.persToTransUntyped(pers_obj, trans_obj, PyAthena.MsgStream())
    print "::: loading T/P converter [%s]... [done]" % (n,)
    

