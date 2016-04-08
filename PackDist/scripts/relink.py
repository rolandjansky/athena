#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""File: relink.py

Developed against Python 2.3
"""

__version__ = '0.9.0'
__date__ = 'Tue Mar 15 2016'
__author__  = 'Grigori Rybkine <Grigori.Rybkine@cern.ch>'

import os, sys
import getopt
import shutil

def cwd():
    _cwd = os.getenv('PWD')
    _gcwd = os.getcwd()
    if _cwd and os.path.samefile(_cwd, _gcwd):
        return _cwd
    else:
        return _gcwd

def cd(path):
    new = os.path.normpath(os.path.join(cwd(), path))
    os.chdir(path)
    _gcwd = os.getcwd()
    if os.path.samefile(new, _gcwd):
        os.environ['PWD'] = new
    else:
        os.environ['PWD'] = _gcwd

def commonhead(list1, list2):
    c = -1
    for i in xrange(min(len(list1), len(list2))):
        if list1[i] == list2[i]: c = i
        else: break
    #if c > -1: print >> sys.stderr, 'commonhead:', c, list1, list2
    return list1[:c + 1]

#def relink(path, v = False):
def relink(path, Arg = (None, True, False)):
    """Convert absolute link `path' (within current directory, to file in PROJ_SRC) to relative link.
    
    Current directory is a copy of (a part of) PROJ_SRC.
    Remove `path' if it is a dangling link, or points to a file outside PROJ_SRC.
    If the conversion results in a dangling link, replace `path' with a copy
    in case it originally pointed to a regular file in PROJ_SRC,
    create the empty target directory in case it originally pointed to a directory.
    """
    if not os.path.islink(path): return

    exps = Arg[0]
    debuginfo = Arg[1]
    v = Arg[2]

    path = os.path.normpath(path)
    s = os.readlink(path)
    snorm = os.path.normpath(s)
    p = os.path.join(PROJ_SRC, path)
    hatpath = os.path.join(PROJ_HAT, path)

    if snorm.startswith(PROJ_SRC + os.sep):
        srcpath = snorm[len(PROJ_SRC + os.sep):]

        pathl = path.split(os.sep)
        srcpathl = srcpath.split(os.sep)
        head = commonhead(pathl, srcpathl)

        if len(pathl) > len(head) + 1 or \
            len(pathl) == len(head) + 1 and len(srcpathl) > len(head):
            # pathl:    o o o a b            # pathl:    o o o a
            # srcpathl: o o o c d e          # srcpathl: o o o c d e
            # head:     o o o          or    # head:     o o o
            # ---------------------          # ---------------------
            # src:      ../c/d/e             # src:      c/d/e
            srcl = [os.pardir for i in xrange(len(pathl) - 1 - len(head))] + srcpathl[len(head):]
            src = os.path.join(*srcl)
        elif len(pathl) == len(head) + 1 and len(srcpathl) == len(head):
            # pathl:    o o o a
            # srcpathl: o o o
            # head:     o o o
            # ---------------------
            # src:      .
            src = os.curdir
            if v: print >> sys.stderr, 'detected symlink to current directory', `hatpath`, '->', `src`
        elif len(pathl) == len(head):
            src = os.path.join(*srcpathl[len(head) - 1:])
            if len(srcpathl) == len(head):
                # pathl:    o o a
                # srcpathl: o o a
                # ---------------------
                # src:      a
                if v: print >> sys.stderr, 'detected symlink to itself', `hatpath`, '->', `src`
            else:
                # pathl:    o o a
                # srcpathl: o o a c
                # ---------------------
                # src:      a/c
                if v: print >> sys.stderr, 'detected too many levels of symlinks', `hatpath`, '->', `src`
        else:
            print >> sys.stderr, 'detected UNFORESEEN', `path`, '->', `srcpath`
            return

        _srcpath = os.path.normpath(os.path.join(os.path.dirname(path), src))
        assert srcpath == _srcpath, '%s:\n%s not equal to %s' % (path, `srcpath`, `_srcpath`)

        os.remove(path)
        if os.path.isfile(srcpath) or os.path.isdir(srcpath):
            try:
                os.symlink(src, path)
            except (IOError, os.error), why:
                print >> sys.stderr, 'Cannot symlink %s -> %s: %s' % (`hatpath`, `src`, str(why))
            else:
                if v: print 'symlinked', `hatpath`, '->', `src`
        else:
            if os.path.isfile(s):
                print >> sys.stderr, 'missing:', hatpath, '->', src
                try:
                    shutil.copy2(s, path)
                except (IOError, os.error), why:
                    print >> sys.stderr, 'Cannot copy %s -> %s: %s' % (`s`, `hatpath`, str(why))
                else:
                    if v: print >> sys.stderr, 'copied', `s`, '->', `hatpath`
            elif os.path.isdir(s):
                print >> sys.stderr, 'missing:', hatpath, '->', src
                try:
                    os.makedirs(srcpath)
                except (IOError, os.error), why:
                    print >> sys.stderr, 'Cannot create directory %s: %s' % (`os.path.join(PROJ_HAT, srcpath)`, str(why))
                else:
                    if v: print >> sys.stderr, 'created directory', `os.path.join(PROJ_HAT, srcpath)`
                try:
                    os.symlink(src, path)
                except (IOError, os.error), why:
                    print >> sys.stderr, 'Cannot symlink %s -> %s: %s' % (`hatpath`, `src`, str(why))
                else:
                    if v: print 'symlinked', `hatpath`, '->', `src`
            else:
                print >> sys.stderr, 'dangling:', p, '->', s
                if v: print >> sys.stderr, 'removed', `hatpath`
#    elif os.path.normpath(os.path.join(os.path.dirname(p), s)).startswith(PROJ_SRC + os.sep):
    else:
        srcpath = os.path.normpath(os.path.join(os.path.dirname(p), s))
#        os.path.normpath(os.path.join(os.path.dirname(p), s)).startswith(PROJ_SRC + os.sep):
        if srcpath.startswith(PROJ_SRC + os.sep):
            if os.path.isfile(path) or os.path.isdir(path):
                if v: print 'relative:', hatpath, '->', s
            else:
                if os.path.isfile(p) or os.path.isdir(p):
                    print >> sys.stderr, 'missing:', hatpath, '->', s
                else:
                    print >> sys.stderr, 'dangling:', p, '->', s
                os.remove(path);
                if v: print >> sys.stderr, 'removed', `hatpath`
        else:
            if os.path.isfile(p) or os.path.isdir(p):
                if exps:
                    dst = exps.destination(srcpath)
                    if dst:
                        os.remove(path)
                        if not dst[1] or debuginfo:
                        # if not dst[1] or DEBUGINFO == 'yes' or MODE == 'dbg':
                            upl = [os.pardir for i in xrange(len(hatpath.split(os.sep)) - 1)]
                            src = os.path.join(os.path.join(*upl), dst[0])
                            try:
                                os.symlink(src, path)
                            except (IOError, os.error), why:
                                print >> sys.stderr, 'Cannot symlink %s -> %s: %s' % (`hatpath`, `src`, str(why))
                            else:
                                if v: print 'symlinked', `hatpath`, '->', `src`
                        else:
                            print 'debuginfo:', hatpath, '->', s
                            if v: print 'removed', `hatpath`
                    else:
                        print >> sys.stderr, 'not_exported:', srcpath
                        os.remove(path);
                        if v: print >> sys.stderr, 'removed', `hatpath`, '->', `s`
                else:
                    print >> sys.stderr, 'external:', hatpath, '->', s
                    os.remove(path);
                    if v: print >> sys.stderr, 'removed', `hatpath`
            else:
                print >> sys.stderr, 'dangling:', p, '->', s
                os.remove(path);
                if v: print >> sys.stderr, 'removed', `hatpath`

def doTree(top = cwd(), v = (None, True, True)):
    global PROJ_SRC, PROJ_HAT
    PROJ_SRC = os.path.normpath(os.path.join(cwd(), os.environ['projsrc']))
    PROJ_HAT = os.path.normpath(os.environ['projhat'])

    assert os.path.normpath(os.path.join(cwd(), top)).endswith(PROJ_HAT), '%s: Parent directory not %s' % (top, `PROJ_HAT`)

    def doList(v, dirpath, filenames):
        #print  >> sys.stderr, dirpath, filenames
        for filename in filenames:
            path = os.path.join(dirpath, filename)
            #print >> sys.stderr, path
            if os.path.islink(path):
                relink(path, v)

    curdir = cwd()
    cd(top)

    os.path.walk(os.curdir, doList, v)

    cd(curdir)

#if __name__ == '__main__':
    #fout = file(os.path.basename(sys.argv[0]) + '.out', 'w')
    #sys.stdout = fout
    #ferr = file(os.path.basename(sys.argv[0]) + '.err', 'w')
    #sys.stderr = ferr
#    if len(sys.argv) > 1:
#        for arg in sys.argv[1:]: doTree(arg)
#    else: doTree()

class ExportPaths(object):
    def __init__(self, path):
        self.paths = {}
        f = open(path, 'U')
        try:
            for l in f:
	        # expected line format: <src>\0<dst>\0<debuginfo_suffix>\n
                i = l[:-1].split('\0')
                assert len(i) == 3, '%s: %s: Wrong line format' % (path, `l`)
                r = os.path.realpath(i[0])
                i[0], i[1] = [os.path.normpath(p) for p in (i[0], i[1])]
                assert os.path.realpath(i[0]) == r, '%s (%s): Ambiguous export path' % (i[0], i[1])
                if i[2] == '': i[2] = '\0' # null character '\0' does not appear in file paths

                for (_r, s), (d, suf) in self.paths.iteritems():
                    if r == _r:
                        if i[0] == s and i[1] == d:
                            print >>sys.stderr, '%s (%s): Duplicate export path' % (i[0], i[1])
                        else:
                            print >>sys.stderr, '%s (%s), %s (%s): Redundant export paths' % \
                              (i[0], i[1], s, d)

                self.paths[(r, i[0])] = (i[1], i[2])
        finally:
            f.close()

    def destination(self, path):
        rpathl = os.path.realpath(path).lstrip(os.sep).split(os.sep)
        pathl = os.path.normpath(path).lstrip(os.sep).split(os.sep)
        for (r, s), (d, suf) in self.paths.iteritems():
            # first try with real paths
            rl = r.lstrip(os.sep).split(os.sep)
            head = commonhead(rl, rpathl)
#            print >>sys.stderr, 'destination: %s %s: %s' % \
#                  (str(rl), str(rpathl), str(head))
            if len(rl) == len(head):
                return (os.path.join(*(d.split(os.sep) + rpathl[len(rl):])), rpathl[-1].endswith(suf))
            elif len(rpathl) == len(head):
                dl = d.lstrip(os.sep).split(os.sep)
                if len(dl) + len(rpathl) - len(rl) > 0:
                    return (os.path.join(*dl[:len(dl) + len(rpathl) - len(rl)]), False)
            # then try with normalized paths
            sl = s.lstrip(os.sep).split(os.sep)
            head = commonhead(sl, pathl)
#            print >>sys.stderr, 'destination: %s %s: %s' % \
#                  (str(sl), str(pathl), str(head))
            if len(sl) == len(head):
                return (os.path.join(*(d.split(os.sep) + pathl[len(sl):])), pathl[-1].endswith(suf))
            elif len(pathl) == len(head):
                dl = d.lstrip(os.sep).split(os.sep)
                if len(dl) + len(pathl) - len(sl) > 0:
                    return (os.path.join(*dl[:len(dl) + len(pathl) - len(sl)]), False)
        return None

def main(argv=[__name__]):
    self = os.path.basename(argv[0])
    try:
        opts, args = getopt.getopt(argv[1:],
                                   "hIS:H:X:",
                                   ["help", "version", "debuginfo", "project-src=", "project-hat=",
                                    "export-paths-from="])
    except getopt.error, e:
        print >>sys.stderr, '%s: %s' % (self, str(e))
        print >>sys.stderr, "Try '%s --help' for more information." % self
        return 1

    debuginfo = False
    for o, v in opts:
        if o in ("-h", "--help"):
            print sys.modules[__name__].__doc__
            return 0
        elif o in ("--version",):
            print '%s %s (%s)' % (self, __version__, __date__)
            print '%sWritten by %s.' % (os.linesep, __author__)
            return 0
        elif o in ("-I", "--debuginfo"):
            debuginfo = True
        elif o in ("-S", "--project-src"):
            os.environ['projsrc'] = v.rstrip(os.sep)
        elif o in ("-H", "--project-hat"):
            os.environ['projhat'] = v.strip(os.sep)
        elif o in ("-X", "--export-paths-from"):
            export_paths = ExportPaths(v)

#    if not args:
#        print >>sys.stderr, '%s: missing path argument' % self
#        print >>sys.stderr, "Try '%s --help' for more information." % self
#        return 1

    try:
        Arg = (export_paths, debuginfo, True)
    except NameError:
        Arg = (None, debuginfo, True)

#    print >> sys.stderr, Arg
#    return 17

    if args:
        for arg in args: doTree(arg, Arg)
    else: doTree(cwd(), Arg)

if __name__ == '__main__':
    sys.exit(main(sys.argv))
