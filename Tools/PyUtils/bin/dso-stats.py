#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: PyUtils/bin/dso-stats.py
# @purpose: compile statistics about shared libraries
# @author:  Scott Snyder

import re
import sys
import os
import subprocess

## monkey patch subprocess to be forward compatible with py-3k
def getstatusoutput(cmd):
    if isinstance(cmd, basestring):
        cmd = cmd.split()
    if not isinstance(cmd, (list, tuple)):
        raise TypeError('expects a list, a tuple or a space separated string')
    process = subprocess.Popen(cmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT)
    stdout, _ = process.communicate()
    return process.returncode, stdout
subprocess.getstatusoutput = getstatusoutput
del getstatusoutput

def getstatus(cmd):
    sc,_ = subprocess.getstatusoutput(cmd)
    return sc
subprocess.getstatus = getstatus
del getstatus

def getoutput(cmd):
    _,out = subprocess.getstatusoutput(cmd)
    return out
subprocess.getoutput = getoutput
del getoutput
## --------------------------------------------------------------

#lib = 'atlas-work4/InstallArea/i686-slc4-gcc34-dbg/lib/libAthenaROOTAccess.so'
def _getpagesz():
    import resource
    sz = resource.getpagesize()
    del resource
    return sz
PAGESIZE = int(_getpagesz())
del _getpagesz


pat = re.compile (' *[0-9]* ([^ ]+) *([0-9a-f]+)')

format = "%(name)-30s %(dso)5s %(code)5s %(puredata)5s %(cpp)5s %(initdata)5s %(bss)5s %(frag)5s %(total)6s"

def parse_lib (lib):
    out = subprocess.getoutput ("objdump -h " + lib)
    secs = []
    for l in out.split ('\n'):
        m = pat.match (l)
        if m:
            secs.append ((m.group(1), int(m.group(2), 16)))
    return secs


def _frag (sz):
    return ((sz + PAGESIZE-1) & (~(PAGESIZE-1))) - sz


def _cleanname (name):
    if name == 'None':
        return 'None'
    name = os.path.basename (name)
    name = os.path.splitext(name)[0]
    if name.startswith ('lib'):
        name = name[3:]
    return name


def _form (x):
    return int ((x+512) / 1024)

class Data:
    def __init__ (self, secs = None, name = None):
        self.name = name
        self.dso = 0
        self.code = 0
        self.puredata = 0
        self.cpp = 0
        self.java = 0
        self.initdata = 0
        self.bss = 0
        self.frag = 0

        self.ro = 0
        self.rw = 0
        
        if secs:
            self.add_secs (secs)
            self.est_frag()
        return


    def __iadd__ (self, other):
        self.dso += other.dso
        self.code += other.code
        self.puredata += other.puredata
        self.cpp += other.cpp
        self.java += other.java
        self.initdata += other.initdata
        self.bss += other.bss
        self.ro += other.ro
        self.rw += other.rw
        self.frag += other.frag
        return self


    def est_frag (self):
        self.frag += _frag (self.ro)
        self.frag += _frag (self.rw)
        self.frag += _frag (self.bss)
        return


    def total (self):
        return (self.dso + self.code + self.puredata + self.cpp +
                self.java + self.initdata + self.frag + self.bss)


    def add_secs (self, secs):
        for s, sz in secs:
            if s in ['.hash', '.dynsym', '.dynstr', '.gnu.version',
                     '.gnu.version_r', '.rel.dyn', '.rel.plt',
                     '.init', '.plt', '.fini']:
                self.dso += sz
                self.ro += sz

            elif s in ['.text']:
                self.code += sz
                self.ro += sz

            elif s in ['.rodata']:
                self.puredata += sz
                self.ro += sz

            elif s in ['.eh_frame_hdr', '.eh_frame', '.gcc_except_table']:
                self.cpp += sz
                self.ro += sz

            elif s in ['.ctors', '.dtors']:
                self.cpp += sz
                self.rw += sz

                
            elif s in ['.jcr']:
                self.java += sz
                self.rw += sz

            elif s in ['.dynamic', '.got', '.got.plt']:
                self.dso += sz
                self.rw += sz

            elif s in ['.data']:
                self.initdata += sz
                self.rw += sz

            elif s in ['.bss']:
                self.bss += sz

            elif s == '.comment' or s.startswith ('.debug'):
                pass

            else:
                print >> sys.stderr, '** Unknown section [%s] **' % s

        return



    def dump (self, f):
        kw = {}
        kw['name'] = _cleanname (self.name)
        kw['dso'] = _form (self.dso)
        kw['code'] = _form (self.code)
        kw['puredata'] = _form (self.puredata)
        kw['cpp'] = _form (self.cpp)
        kw['java'] = _form (self.java)
        kw['initdata'] = _form (self.initdata)
        kw['frag'] = _form (self.frag)
        kw['bss'] = _form (self.bss)
        kw['total'] = _form (self.total())
        print >> f, format % kw
        

# secs = parse_lib (lib)
# data = Data(secs, name = lib)
# print data.dso, data.code, data.puredata, data.cpp, data.java, data.initdata, data.bss
# print data.ro, data.rw, data.frag

# data2 = Data(secs)
# data += data2
# print data.dso, data.code, data.puredata, data.cpp, data.java, data.initdata, data.bss
# print data.ro, data.rw, data.frag



# data.dump (sys.stdout)


kw = {'name' : 'Name',
      'dso'  : 'DSO',
      'code' : 'Code',
      'puredata': 'Pure',
      'cpp'  : 'C++',
      'java' : 'Java',
      'initdata': 'data',
      'bss'  : 'BSS',
      'frag' : 'Frag',
      'total': 'Total'}
print >> sys.stdout, format % kw


total = Data(name = 'Total')
import fileinput
libs = []
for l in fileinput.input():
    if l[-1] == '\n':
        l = l[:-1]
    secs = parse_lib(l)
    data = Data (secs, name = l)
    libs.append (data)
    total += data

def fn (a, b):
    return b.total() - a.total()
libs.sort (fn)

for l in libs:
    l.dump (sys.stdout)
total.dump (sys.stdout)
