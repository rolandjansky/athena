# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
dump minder information
"""



import os.path, string, time, xml.dom.minidom, sys, getopt
sys.path.append('../src')
from Evaluate import Evaluate


def dumpTag(p, de):
    print 

def dumpMinderTags(fn, paths):

    dom = xml.dom.minidom.parse(fn)
    de = dom.documentElement
    pnodes = de.getElementsByTagName('package')

    for p in pnodes:
        print '\n------- %s --------' % Evaluate('packageName/text()', p)
        mnodes = p.getElementsByTagName('minder')
        for m in mnodes:
            print '   %s' % Evaluate('identifiedName', m)
            for p in paths:
                print '      tag: %s   value %s' %  (p, Evaluate(p+'/text()', m))


if __name__ == '__main__':

    def usage():
        print
        print 'RTTSummaryTagDumper -- dumps minder tags'
        print
        print 'usage: python RTTSummaryTagDumper [options]'
        print
        print 'Options'
        print '   -l use the most recent version of the log files. must supply arg branch or release:branch:os'
        print '      posible release: rel_<n> n in [0,6], possible branch = ("bugfix", "dev", "val")'
        print '      possible os = ("slc3", "slc4") example: -l rel_0:dev:slc3'
        print '   -f triggers writing to a file. Filename is written to stdout'
        print '   -u set the user, eg  brinick, eric, peter, prod, Default prod'
        print '   -t colon seprated list of tags'
        print '   -h print this message and exit'
        return
    
    try:
        opts, args = getopt.getopt(sys.argv[1:], 't:b:l:fu:h')
        print opts
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    useLatest = False
    tags    = []
    slc       = None
    output    = None
    branch    = None
    user      = 'prod'
    for o, a in opts:
        if o == '-l':
            tokens = a.split(':')
            if len(tokens)!= 3:
                print 'bad -l'
                usage()
                sys.exit(2)
            
            release = tokens[0]
            if release not in ['rel_0', 'rel_1', 'rel_2', 'rel_3', 'rel_4', 'rel_5', 'rel_6']:

                print 'bad release'
                usage()
                sys.exit(2)
                
            branch = tokens[1]
            if branch not in ['bugfix', 'dev', 'val']:

                print 'bad branch'
                usage()
                sys.exit(2)
                
            opsys = tokens[2]
            if opsys not in ['slc3', 'slc4']:
                usage()
                sys.exit(2)
                
            if opsys == 'slc3':
                opsys = 'i686-slc3-gcc323-opt'
            elif opsys == 'slc4':
                opsys = 'i686-slc4-gcc34-opt'


        elif o == '-u':
            user = a
            if user != 'prod': user = 'data/'+user
        elif o == '-h':
            usage()
            sys.exit(0)
        if o == '-t':
            tags = a.split(':')
        if o == '-f':
            output = '/afs/cern.ch/user/r/rtt/Scratch'

    if not tags:
        print '-t option required'
        usage()
        sys.exit(2)


    if not tags:
        print '-b option required'
        usage()
        sys.exit(2)



    if user == 'prod':
        root = '/afs/cern.ch/atlas/project/RTT/%s/Results/%s/%s/build/%s' % (user, release, branch, opsys)
    else:
        root = '/afs/cern.ch/atlas/project/RTT/%s/Results/%s/%s/build/%s' % (user, release, branch, opsys)

    fn = os.path.join(root, 'RTTSummary.xml')
    print 'looking for summaryFile %s' %  fn


    if not os.path.exists(fn):
        print 'Balking as RTTSummary file does not exist:\n%s' % root
        sys.exit(0)


    if output:
        cname = 'RTTSummaryTagDump'
        cname += '_'+os.path.basename(root)+'.log'
        output = os.path.join(output, cname)
        of = open(output,'w')
        print 'writing results to ', output
    else:
        of = sys.stdout


    of.write(dumpMinderTags(fn, tags))
