# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""read FlowNetwork string reprenstations whihc have been extracted
from a log file (with, eg, getFlowNetworks.py) and either write
a reference file or perform a regerssion test.
"""

import subprocess
import os
import getopt
import sys

ref_file_name = './maxbipartite_ref.txt'


def read_ref(f):
    with open(ref_file_name) as ifile:
        for l in ifile:
            if l.startswith(f): return l.strip()
    print 'Error: %s not in ref file %s' % (f, ref_file_name)
    sys.exit(0)


def is_exe_found(exe):
    paths = os.environ['PATH'].split(':')
    for p in paths:
        if os.path.exists(os.path.join(p, exe)): return True
    return False


def run(exe, f):

    sys.stdout.write('%s\r' % 'reading %s' % f)
    sys.stdout.flush()
    
    try:
        lines =  subprocess.check_output([exe, f]).split('\n')
    except Exception, e:
        print 'Error processing file %s: %s' % (f, str(e))
        sys.exit(0)
        
    return lines[-2]


def write_reference(exe, td):
    print 'will write reference file ', ref_file_name
    lines = run(exe, td)
        
    text = '\n'.join(lines)
    open(ref_file_name, 'w').write(text)


def cmp(exe, testdir):

    ref_file_fn  = os.path.join(testdir)
    print 'will compare with reference file ', ref_file_fn
    version = open('../cmt/version.cmt').readlines()[0].strip()
    print 'max-bipartite version %s' % version

    fns = sorted([f for f in os.listdir(testdir) if f.startswith('fn_')])

    n = 0
    for f in fns:
        result = run(exe, os.path.join(testdir, f.strip()))
        result = '%s %s' % (f, result)
        expected = read_ref(f)
        if result != expected:
            print 'failure:'
            print 'obtained %s' % result
            print 'expected %s' % expected
            print 'bailing out'
            sys.exit(0)
        n += 1

    print '%d comparison tests passed' % n


def usage():
    print 'python run_maxbipartite_regression.py'
    print 'options: '
    print '-w  read inputs and write reference file: '
    print '-t  directory with input and reression files [default: cwd]: '
    print '  default: read inputs and compare to the reference file'

if __name__ == '__main__':

    try:
        opts, args = getopt.getopt(sys.argv[1:], "hwt:", [])
    except getopt.GetoptError as err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)
    write = False
    testdir = os.getcwd()
    for o, a in opts:
        if o == "-w":
            write = True
        if o == "-t":
            testdir  = a
        elif o in ("-h", "--help"):
            usage()
            sys.exit()
        else:
            assert False, "unhandled option"
    # ...

    if not os.path.exists(testdir):
        'test directory %s does not exists' % testdit
        sys.exit(0)

    print 'test directory: ', testdir

    exe = 'TrigHLTJetHypoTests.exe'
    if not is_exe_found(exe):
        print 'Executable %s is not found on $PATH' % exe
        sys.exit(0)

    if write:
        write_reference(exe, testdir)
    else:
        cmp(exe, testdir)
