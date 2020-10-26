# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
import os
from future import standard_library
standard_library.install_aliases()
import subprocess
from PyDumper.Dumpers import get_dumper_fct
from AthenaPython import PyAthena
from PyUtils.fprint import fprintln

if ('ATLAS_REFERENCE_TAG' not in globals() and
    'ATLAS_REFERENCE_TAG' in os.environ):
    ATLAS_REFERENCE_TAG = os.environ['ATLAS_REFERENCE_TAG']

def find_file (fname,refPaths):
    for p in refPaths:
        if p:
            path = os.path.join (p, fname)
            if os.path.exists (path):
                return path
    print ('ERROR: Cannot find file: ', fname)
    return None


#
# If a new xAOD variable appears, print a warning, but don't treat
# it as a failure.
#
def checknewvars (output):
    names = set()
    for l in output.split ('\n'):
        if not l: continue
        if l.startswith ('+++') or l.startswith ('---'): continue
        if l[0] == '-': return None
        if l[0] == '+':
            pos = l.find (':')
            if l.startswith ('+     ') and pos > 0:
                names.add (l[6:pos])
            else:
                return None
    l = list(names)
    l.sort()
    return l


class Dumper (PyAthena.Alg):
    def __init__ (self, name, inputFile, Keys, refpaths):
        PyAthena.Alg.__init__ (self, name)
        self.infile = inputFile
        self.keys = Keys
        self.refpaths = refpaths
        return

    def initialize (self):
        self.sg = PyAthena.py_svc ('StoreGateSvc')
        self.ofile_name = os.path.basename (self.infile) + '.dump'
        refbase = os.path.basename (self.infile) + '.ref'
        self.reffile_name = '../share/' + refbase
        if not os.path.exists (self.reffile_name):
            self.reffile_name = '../' + self.reffile_name

        if not os.path.exists (self.reffile_name) and 'ATLAS_REFERENCE_TAG' in globals():
            from AthenaCommon.Utils.unixtools import find_datafile
            r = find_datafile (ATLAS_REFERENCE_TAG)
            if r:
                self.reffile_name = os.path.join (r, ATLAS_REFERENCE_TAG,
                                                  refbase)

        if not os.path.exists (self.reffile_name) and 'ATLAS_REFERENCE_TAG' in globals():
            self.reffile_name = find_file (os.path.join (ATLAS_REFERENCE_TAG,
                                                         refbase), self.refpaths)

        self.ofile = open (self.ofile_name, 'w')
        self.icount = 0
        return 1

    def finalize (self):
        self.ofile.close()
        ret, output = subprocess.getstatusoutput ('diff -u %s %s' % (self.reffile_name, self.ofile_name))
        if ret != 0:
            newvars = checknewvars (output)
            if newvars:
                print ('WARNING: new xAOD variables ', newvars)
            else:
                print ('ERROR running diff with reference')
                print (output)
        return 1

    def execute (self):
        fprintln (self.ofile, 'Event index', self.icount)
        self.icount += 1
        for k in self.keys:
            nmax = None
            apos = k.find ('@')
            if apos >= 0:
                nmax = int (k[apos+1:])
                k = k[:apos]
            fprintln (self.ofile, '-->', k)

            store = self.sg
            spos = k.find ('/')
            if spos >= 0:
                store = PyAthena.py_svc (k[:spos])
                k = k[spos+1:]

            spos = k.find ('#')
            if spos >= 0:
                (typ, k) = k.split ('#')
                o = store.retrieve (typ, k)
            else:
                o = store[k]
            dumper = get_dumper_fct (type (o), self.ofile, nmax = nmax)
            dumper (o)
        fprintln (self.ofile, '\n')
        return 1
