#!/usr/bin/env python

import PmbUtils.MsgSvcUtils
import PmbUtils.LogFileParsingUtils
import sys,os

def badusage():
    print "Usage:"
    print
    print "%s logfile1.txt <logfile2.txt>"%os.path.basename(sys.argv[0])
    print
    print "Run on 1 or 2 logfiles from jobs which were run in dbg builds and with"
    print "svcMgr.MessageSvc.countInactive=True to print the top sources of MsgSvc abuses."
    print ""
    print "If run on just logfile, a simple sorted list of abusers will be printed."
    print ""
    print "If two logfiles are specified, it is assumed that they come from similar jobs (same"
    print "release, config, inputfile, etc.) whose only difference is that they were run with"
    print "a different value of maxEvents. This is used to estimate the number of message abuses"
    print "within the event loop."
    sys.exit(1)

logs=[os.path.abspath(os.path.expanduser(f)) for f in sys.argv[1:]]
for l in logs:
    if not os.path.exists(l):
        badusage()
if len(logs)!=len(set(logs)) or not len(logs) in [1,2]:
    badusage()

infos = [(PmbUtils.LogFileParsingUtils.parseall(l),l) for l in logs]
for i,file in infos:
    if not i.get('misc',{}).get('statusok',False):
        print "ERROR: Job not succesful:",file
        sys.exit(1)
    if not i['misc'].get('msgsvc_abuses',None):
        print "ERROR: Job was not run with svcMgr.MessageSvc.countInactive=True or it was not run in a dbg build with the required Gaudi release."
        sys.exit(1)

if len(infos)==2:
    jobshort=infos[0][0]['misc']
    joblong=infos[1][0]['misc']
    if jobshort['inputevts']>joblong['inputevts']:
        jobshort,joblong=joblong,jobshort
    nshort,nlong=jobshort['inputevts'],joblong['inputevts']
    if nshort==nlong:
        print "ERROR: Jobs had same number of input events."
        sys.exit(1)
    for k in 'build_date','platform','infile','build_branch','atlas_release':
        v1,v2=jobshort.get(k,None),joblong.get(k,None)
        if v1!=v2:
            print "ERROR: The two logs have incompatible value of %s (%s vs %s)"%(k,v1,v2)
            sys.exit(1)
    evt,overhead=PmbUtils.MsgSvcUtils.split_in_evtloop_and_overhead(nshort,jobshort['msgsvc_abuses'],
                                                                    nlong,joblong['msgsvc_abuses'],ignore_negative_counts=True)

    out_evtloop='msgsvcabuse_evtloop.txt'
    out_overhead='msgsvcabuse_overhead.txt'
    title='Estimated absolute and relative number of MsgSvc abuses in job %s.'
    open(out_evtloop,'w').write('\n'.join(PmbUtils.MsgSvcUtils.list_to_string(evt,title%'evtloop each event',False))+'\n')
    print "Wrote",out_evtloop
    open(out_overhead,'w').write('\n'.join(PmbUtils.MsgSvcUtils.list_to_string(overhead,title%'one-time overhead',False))+'\n')
    print "Wrote",out_overhead
else:
    out='msgsvcabuse_jobtotal.txt'
    open(out,'w').write('\n'.join(PmbUtils.MsgSvcUtils.list_to_string(infos[0][0]['misc']['msgsvc_abuses'],'Unprotected MsgSvc calls',True))+'\n')
    print "Wrote",out
