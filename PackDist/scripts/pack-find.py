#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""File: pack-find.py

Usage: pack-find.py [OPTION]... PATH...
Search directory tree rooted at each filename PATH for ${CMTCONFIG} directories,
pathnames matching any of shell patterns, supplied in files 'src.ptn', 'doc.ptn'
in directory ${scriptdir}, excluding any other directories listed in file
'platforms' or pathnames matching any of shell patterns in file 'junk.ptn' and
writing pathnames into files specified with relevant options or on standard output,
followed by a newline.

      --bin-file=BINFILE       file for CMTCONFIG value binary directories pathnames
      --src-file=SRCFILE       file for source code pathnames matching list of patterns in src.ptn
      --doc-file=DOCFILE       file for documentation pathnames matching list of patterns in doc.ptn
      --noarch-file=NOARCHFILE file for any other pathnames except for directories
      --dirs-file=DIRSFILE     file for any other directories
  -d, --debug                  print lots of debugging information
  -h, --help                   display this help and exit
      --version                output version information and exit

Report bugs to <Grigori.Rybkine@cern.ch>.
"""

__version__ = '0.1.0'
__date__ = 'Sat Feb 06 2010'
__author__ = 'Grigory Rybkin'

import sys, os
import getopt
from os.path import join, islink, basename
from fnmatch import fnmatch
import logging

class ProjectFiles(object):
    def __init__(self):
        self.cmtconfig = os.environ['CMTCONFIG']
        self.scriptdir = os.environ['scriptdir']
        self.types = ['bin', 'src', 'doc', 'noarch', 'dirs']
        for t in self.types:
            setattr(self, t, list())
            setattr(self, 'f' + t, None)

    def _init(self):
        platforms = open(join(self.scriptdir, 'platforms'))
        # platforms to skip - all but ${CMTCONFIG}
        self.platforms = [p for p in [p.strip() for p in platforms.readlines()]
                          if p != self.cmtconfig]
        platforms.close()
        logger.debug(self.platforms)

        def gen_expr(s):
            ptn = []
            for l in s.splitlines():
                ptn.append(l)
                if not l.startswith('*'):
                    ptn.append(join('*', l))
            logger.debug(ptn)
            return ptn
        def init_ptn(obj, name):
            f = open(join(obj.scriptdir, name + '.ptn'))
            setattr(obj, name + '_ptn', gen_expr(f.read()))
            f.close()

        for name in ['junk', 'src', 'doc']:
            init_ptn(self, name)
                
    def walk(self, top):
        self._init()

        def onerror(e):
            logger.error(e)
            Errno.set(e.errno)

        for root, dirs, files in os.walk(top, onerror = onerror):
            for n in dirs + files:
                p = join(root, n)
                # platforms to skip - all but ${CMTCONFIG}
                if n in self.platforms:
                    if n in dirs and not islink(p): dirs.remove(n)
                    continue
                # platform to pack - ${CMTCONFIG}
                if n == self.cmtconfig:
                    self.bin.append(p)
                    if n in dirs and not islink(p): dirs.remove(n)
#                    if n in dirs: dirs.remove(n)
                    continue
                # exclude junk (print nothing)
                m = False
                for ptn in self.junk_ptn:
                    if fnmatch(p, ptn):
                        m = True
                        break
                if m: continue
                if n in files or islink(p):
                    # source code
                    for ptn in self.src_ptn:
                        if fnmatch(p, ptn):
                            m = True
                            self.src.append(p)
                            break
                    if m: continue
                    # documentation files
                    for ptn in self.doc_ptn:
                        if fnmatch(p, ptn):
                            m = True
                            self.doc.append(p)
                            break
                    if m: continue
                    # anything else - noarch files
                    self.noarch.append(p)
                else:
                    # ALL directories
                    self.dirs.append(p)
        return Errno.errno
    
    def fprint(self):
        def write(obj, name):
            try:
                if getattr(obj, 'f' + name):
                    f = open(getattr(obj, 'f' + name), 'w')
                    for line in getattr(obj, name):
                        f.write(line + '\n')
                    f.close()
                else:
                    for line in getattr(obj, name):
                        sys.stdout.write(line + '\n')
            except IOError, e:
                logger.error(e)
                Errno.set(e.errno)

        for t in self.types:
            write(self, t)

class Errno:
    def set(code):
        Errno.errno = code
    set = staticmethod(set)
    errno = 0

def help():
    print sys.modules[__name__].__doc__

def main(argv=[__name__]):
    self = basename(argv[0])
    try:
        opts, args = getopt.getopt(argv[1:],
                                   "hd",
                                   ["bin-file=", "src-file=", "doc-file=",
                                    "noarch-file=", "dirs-file=",
                                    "help", "version", "debug"])
    except getopt.error, e:
        print >>sys.stderr, '%s: %s' % (self, str(e))
        print >>sys.stderr, "Try '%s --help' for more information." % self
        return 1

    global logger
    logging.basicConfig()
    logger = logging.getLogger(self)
    logger.setLevel(logging.INFO)

    pfs = ProjectFiles()

    for o, v in opts:
        if o in ("-h", "--help"):
            print sys.modules[__name__].__doc__
            return 0
        elif o in ("--version",):
            print '%s %s (%s)' % (self, __version__, __date__)
            print '%sWritten by %s.' % (os.linesep, __author__)
            return 0
        elif o in ("-d", "--debug"):
            logger.setLevel(logging.DEBUG)
        else:
            for type in pfs.types:
                if o == '--' + type + '-file':
                    setattr(pfs, 'f' + type, v)
                    logger.debug('set %r to %r' % (o, v))

    if not args:
        args = [os.curdir]
#         print >>sys.stderr, '%s: missing path argument' % self
#         print >>sys.stderr, "Try '%s --help' for more information." % self
#         return 1

    for arg in args:
        pfs.walk(arg)
#    if 0 != Errno.errno: return 1
    pfs.fprint()
    if 0 != Errno.errno: return 1
    else: return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
