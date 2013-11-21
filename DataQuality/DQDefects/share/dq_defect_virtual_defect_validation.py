#!/usr/bin/env python

def truncate_iovs_to_runs(iovs, runends):
    for iov in iovs:
        if iov.since.run not in runends:
            print 'Missing run', iov.since.run, 'from EOR_Params'
            
    rv = filter(None, [iov.intersect(runends[iov.since.run])
                   for iov in iovs if iov.since.run in runends])
    rv = filter(lambda x: not(x.until.lumi-x.since.lumi == 0),
                rv)
    return rv
   
def get_runends():
    from DQUtils.db import fetch_iovs
    from DQUtils.sugar import RunLumi
    eor_params = fetch_iovs('EOR_Params', 
                             with_channel=False, what=[])
    runends = {}
    for iov in eor_params:
        if iov.since.lumi == 0: iov = iov._replace(since=RunLumi(iov.since.run, 1))
        runends[iov.since.run] = iov
    return runends

def do_compact(iovs):
    iovs = sorted(iovs)
    rv = []
    pos = 0
    while pos < len(iovs):
        tiov = iovs[pos]
        pos += 1
        while pos < len(iovs):
            niov = iovs[pos]
            if niov.since != tiov.until:
                break
            tiov = tiov._replace(until=niov.until)
            pos += 1
        rv.append(tiov)
    return rv

if __name__ == '__main__':
    import optparse

    parser = optparse.OptionParser()
    parser.add_option('--defectdb', default='defectstest.db/COMP200',
                      help='Set defect database')
    parser.add_option('--vfdb', default='LBSUMM',
                      help='Set DQ flag database (connection string format as in DQUtils.db.Databases)')

    opts, args = parser.parse_args()

    # get virtual flag IOVs
    import VirtualFlags, DQUtils
    from DQDefects import DefectsDB
    print 'Getting virtual flag information...',
    vfdb = DQUtils.db.Databases.get_folder(opts.vfdb)
    vff = VirtualFlags.VirtualFlagFolder(vfdb)
    mapping = vff.get_flag_name_id_mapping()
    print 'done'

    print 'Getting virtual defect information...',
    ddb = DefectsDB(opts.defectdb, read_only=True)
    defectids, defectnames, defectdict = ddb.get_virtual_channels()
    print 'done'

    print 'Retrieving run ends...',
    runends = get_runends()
    print 'done'
 
    for key in mapping:
        if not isinstance(key, basestring): continue
        #if '_' in key: continue
        if key not in defectdict or key in ['IDPF', 'LCD', 'MET', 'IDBCM', 'TIGB']:
            print 'MISSING:', key
            continue
        print key,
        vfiovs = do_compact(vff.browseObjects((152166,0), (167845,0), [key], 'DetStatus-v03-repro05-01', selection=lambda x: x.Code < 3))
        #print vfiovs
        d1 = set([((x.since.run, x.since.lumi), (x.until.run, x.until.lumi)) for x in truncate_iovs_to_runs(vfiovs, runends) if x.since.run != 152220])
        diovs = do_compact(ddb.retrieve((152166,0), (167845,0), [key]))
        d2 = set([((x.since.run, x.since.lumi), (x.until.run, x.until.lumi)) for x in truncate_iovs_to_runs(diovs, runends) if x.since.run != 152220])
        print d1 == d2
        if d1 != d2:
            #print d1
            d1a = dict([(((x.since.run, x.since.lumi), (x.until.run, x.until.lumi)), x.Comment) for x in truncate_iovs_to_runs(vfiovs, runends)])
            d2a = dict([(((x.since.run, x.since.lumi), (x.until.run, x.until.lumi)), x.comment) for x in truncate_iovs_to_runs(diovs, runends)])

            print 'In VF but not defects:', [(x, d1a[x]) for x in d1-d2]
            print 'In defects but not VF:', [(x, d2a[x]) for x in d2-d1]

