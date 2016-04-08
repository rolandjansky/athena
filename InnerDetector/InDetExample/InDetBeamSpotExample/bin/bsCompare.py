#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Compare beamspot DQ defects between various databases and tags
"""
__author__  = 'Carl Gwilliam'
__version__ = ''
__usage__   = '''%prog [options] [database1] [database2]

Database can be either connection string or db file:

  e.g COOLOFL_INDET/CONDBR2 (default)
      beamspot.db/BEAMSPOT

Example:

  dqCompare.py --rl 190236 ~atlidbs/jobs/data11_7TeV.00190236.express_express/DB_BEAMSPOT.x155_c779/data11_7TeV.00190236.express_express-DB_BEAMSPOT.x155_c779.AveBeamSpot-beamspot.db/BEAMSPOT

'''

from DQUtils import fetch_iovs, process_iovs
from DQUtils.sugar import IOVSet,RANGEIOV_VAL,RunLumiType

from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-f', '--folder', dest='folder', default='/Indet/Beampos', help='Folder name (default: /Indet/Beampos)')
parser.add_option('', '--tag1', dest='tag1', default='nominal', help='Tag to compare (default: nominal)')
parser.add_option('', '--tag2', dest='tag2', default='IndetBeampos-ES1-UPD2-03', help='Tag to compare to (default: IndetBeampos-ES1-UPD2-03)')
parser.add_option('', '--rl', dest='runMin', type='int', default=None, help='Start run number (default: None)')
parser.add_option('', '--ru', dest='runMax', type='int', default=None, help='Start run number (default: None)')
parser.add_option('', '--statusonly', dest='statusonly', action='store_true', default=False, help='only show runs with differences in status (default: False)')
parser.add_option('-s', '--summary', dest='summary', action='store_true', default=False, help='only show summary of runs with differences (default: False)')
parser.add_option('', '--span', dest='span', action='store_true', default=False, help='Allows IoVs spanning run boundaries (default: False)')
(options,args) = parser.parse_args()

db1 = args[0] if len(args)==1 else 'COOLOFL_INDET/CONDBR2'
db2 = args[1] if len(args)==2 else 'COOLOFL_INDET/CONDBR2'

def main():
    f1 = "%s::%s" % (db1, options.folder)
    f2 = "%s::%s" % (db2, options.folder)

    print "="*100
    print "Comparing: "
    print "  * ", f1, options.tag1
    print "  * ", f2, options.tag2
    print "="*100

    if options.runMin is not None:
        runs = options.runMin
        if options.runMax is not None:
            runs = options.runMin, options.runMax

    tag1iovs = fetch_iovs(f1, runs=runs, tag=options.tag1)
    tag2iovs = fetch_iovs(f2, runs=runs, tag=options.tag2)
        
    affected_runs = set()
    ar2 = set()

    #runiovs = IOVSet()
    #runiovs.add(RunLumiType((runs << 32)+0), RunLumiType((runs << 32)+(1 << 32)-1))

    # Loop over iovs in the two tags
    # for since, until, (t1, t2, r) in process_iovs(tag1iovs, tag2iovs, runiovs.solidify(RANGEIOV_VAL)):
    for since, until, (t1, t2) in process_iovs(tag1iovs, tag2iovs):

        # Ignore IoVs spanning runs
        if not options.span and since.run != until.run: continue

        # Make sure run exists in both
        #if not t1.since or not t2.since: continue

        # Only consider runs in the range interested in
        # if not r: continue

        # Check for IOVs with different status (one of which is non-zero)
        if (t1.status == 0 or t2.status == 0) and (t1.status != t2.status):
            if options.statusonly and not options.summary:
                print since, until, t1.status, t2.status
            ar2.add(since.run)

        # Check for differences in all fields 
        if not options.statusonly: 
            for i, field in enumerate(t1._fields):
                if t1[i] != t2[i]:
                #if field in ['posX', 'posY'] and abs(t1[i]-t2[i]) > 0.1e-3:
                #if field not in ['status'] and t1[i] != 0 and abs(t1[i]-t2[i])/t1[i] > 0.001:
                #if field in ['posZ'] and t1[i] != 0 and abs(t1[i]-t2[i])/t1[i] > 0.05:
                #if field in ['sigmaX', 'sigmaY'] and abs(t1[i]-t2[i]) > 0.2e-3:
                #if field in ['sigmaZ'] and abs(t1[i]-t2[i])/t1[i] > 0.002:
                    if not options.summary:
                        print since, until, field, t1[i], t2[i]
                    affected_runs.add(since.run)

    # Print results
    if not options.statusonly:
        print "Runs with differences (%s):" % len(affected_runs), ", ".join(map(str, sorted(affected_runs)))

    print "Runs with different status (and one non-zero):", ", ".join(map(str, sorted(ar2)))

if __name__ == "__main__":
    main()
