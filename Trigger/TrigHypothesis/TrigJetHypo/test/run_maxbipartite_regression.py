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

def run():
    version = open('../cmt/version.cmt').readlines()[0].strip()
    cwd = os.getcwd()
    fns = sorted([f for f in os.listdir('.') if f.startswith('fn_')])
    fns = [f.strip() for f in fns]
    tlines = ['max-bipartite version %s' % version]
    print fns
    
    for fn in fns:
        print 'reading ', fn
        try:
            lines =  subprocess.check_output(["TrigHLTJetHypoTests.exe", fn]).split(
                '\n')
        except Exception, e:
            raise RuntimeError('Error reading file %s: %s' % (fn, str(e)))
        
        tlines.append('%s %s' % (fn, lines[-2]))
    return tlines


def write_reference():
    print 'will write reference file ', ref_file_name
    lines = run()
        
    text = '\n'.join(lines)
    open(ref_file_name, 'w').write(text)


def cmp():
    print 'will compare with reference file ', ref_file_name
    n_lines = run()
    r_lines = open(ref_file_name).readlines()
    r_lines = [l.strip() for l in r_lines]

    if len(n_lines) != len(r_lines):
        print 'files to compare have different lengths'
        return

    z = zip(n_lines, r_lines)
    print 'comparing %s %s' % z[0]
    n = 0
    for zz in z[1:]:
        if zz[0] != zz[1]:
            print 'difference:'
            print '  ' + zz[0]
            print '  ' + zz[1]
        n += 1
        
    print 'done'


def usage():
    print 'python run_maxbipartite_regression.py'
    print 'options: '
    print '-w  read inputs and write reference file: '
    print '  default: read inputs and compare to the reference file'

if __name__ == '__main__':

    try:
        opts, args = getopt.getopt(sys.argv[1:], "hw", [])
    except getopt.GetoptError as err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)
    write = False
    for o, a in opts:
        if o == "-w":
            write = True
        elif o in ("-h", "--help"):
            usage()
            sys.exit()
        else:
            assert False, "unhandled option"
    # ...


    if write:
        write_reference()
    else:
        cmp()
