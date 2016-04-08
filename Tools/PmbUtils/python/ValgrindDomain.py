# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Helper module for using the callgrind with the Valkyrie service to just profile the algorithms in a given domain (from AlgDomains.py)
#
# Typical use-case, run Reco_trf.py under valgrind, and use the present module in a postExec to enable calgrind profiling of all jet algorithms:
#
#  $> valgrind --tool=callgrind --trace-children=yes --num-callers=8 --collect-jumps=yes --instr-atstart=no `which Reco_trf.py` [whatever other options you want] postExec="import PmbUtils.ValgrindDomain as vd;vd.setup(domain='jet')"
#
# (or append_postExec when appropriate)
#
# Run this command to see which domains are available in addition to 'jet':
#
# $> python -c 'import PmbUtils.AlgDomains as ad;print ad.map_pmbdomain2plotname.keys()'

def setup( domains=None,algs=None,ignore_first_n_events=2, verbose=True, domain=None ):
    from Valkyrie.JobOptCfg import ValgrindSvc
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from GaudiKernel.Constants import VERBOSE
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AlgSequence import iter_algseq
    import PmbUtils.AlgDomains

    if not domains: domains=[]
    if domain:
        domains += [domain]
    if not algs: algs=[]
    if not isinstance(domains,list): domains = [domains]
    if not isinstance(algs,list): algs = [algs]
    domains=set(domains)
    profalgs=set(algs)

    if domains:
        for a in iter_algseq(AlgSequence()):
            if a.name() in profalgs: continue
            if PmbUtils.AlgDomains.filter_alg_to_pmbdomains_rawtoesd(a.name()) in domains:
                profalgs.add(a.name())

    profalgs=list(profalgs)
    if not profalgs:
        print "ValgrindDomain WARNING: Not profiling any algorithms."
    if verbose:
        svcMgr += ValgrindSvc( OutputLevel=VERBOSE,
                               ProfiledAlgs=profalgs,
                               IgnoreFirstNEvents=ignore_first_n_events )
    else:
        svcMgr += ValgrindSvc( ProfiledAlgs=profalgs,
                               IgnoreFirstNEvents=ignore_first_n_events )
