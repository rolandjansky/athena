# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys, fnmatch, getopt
from time import strftime, localtime

from findMostRecentLogDir import findMostRecentLogDir

def dumpLastLine(root, packages):
    try:
        recentLogDir =  findMostRecentLogDir(root)
    except:
        print 'cannot find log files below ', root
        sys.exit(0)


    print 'latest log directory',recentLogDir
    dirs = os.listdir(recentLogDir)
    logdirs = []
    [logdirs.extend(fnmatch.filter(dirs,p)) for p in packages]
    logdirs = [os.path.join(recentLogDir, l) for l in logdirs]
    logdirs = [l for l in logdirs if os.path.isdir(l)]

    nlines = []

    for logdir in logdirs:
        print 'examining ',logdir

        logs = os.listdir(logdir)
        for f in logs :
            fn = os.path.join(logdir,f)
            ifile = open(fn, 'r')
            lines = ifile.readlines()
            ifile.close()
            nlines.append( (os.stat(fn).st_atime, lines[-1], fn) )
        
    print 'found %d lines' % len(nlines)

    nlines.sort()
    for l in nlines:
        print '\n%s\n%s: %s\n' % (l[2], strftime("%d/%m %H:%M:%S", localtime(l[0])), l[1])

                                  

def usage():
    print
    print 'dumpLastLine -- dumps the time ordered last line for all log files in an RTT run.'
    print
    print 'usage: python dumpLastLine [options]'
    print
    print 'Options'
    print '   -p colon separated list of packages to examine defaults to "*"'
    print '   -l use the most recent version of the log files. must supply arg slc3 or slc4'
    print '      if ommitted, the path to the log files will be taken from the config file.'
    print '   -f triggers writing to a file. Filename is written to stdout'
    print '   -u set the user, eg  brinick, eric, peter, prod, Default prod'
    print '   -h print this message and exit'
    return

if __name__ == '__main__':


    try:
        opts, args = getopt.getopt(sys.argv[1:], 'p:l:fu:h')
        print opts
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    
    useLatest =  False
    packages  =  '*'
    slc       =  None
    output    =  None
    user      =  'prod'
    
    for o, a in opts:
        if o == '-l':
            if a == 'slc3':
                slc = 'SLC3'
            elif a == 'slc4':
                slc = 'SLC4_32'
        elif o == '-u':
            user = a
        elif o == '-p':
            packages = a.split(':')
        elif o == '-h':
            usage()
            sys.exit(0)
        if o == '-f':
            output  = '/afs/cern.ch/user/r/rtt/Scratch'

    frag = ''
    if user == 'prod':
        frag = os.path.join(user, slc, 'logs')
    else:
        frag = os.path.join('data', user, 'logs', slc, 'logs')
    
    root = '/afs/cern.ch/atlas/project/RTT/%s' %frag
    # packages = ['TestAtlfast', 'BPhysValidation']
    #packages = ['*']

    dumpLastLine(root, packages)
