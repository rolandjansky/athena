#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file avn.py
# @purpose an atlas oriented SVN helper script
# @date October 2009

# credits: Scott Snyder provided the first version.

__doc__ = """
an atlas oriented SVN helper script
-----------------------------------

examples of usage:
$ cd Control/AthenaServices
$ avn ci -m 'some interesting comment'
$ avn tag AthenaServices-99-99-99 -m 'some interesting comment'
$ avn diff @AthenaServices-66-66-66 @AthenaServices-99-99-99
$ avn diff @AthenaServices-66-66-66/src @AthenaServices-99-99-99/src
$ avn diff @AthenaServices-66-66-66/src/f.cxx @AthenaServices-99-99-99/src/f.cxx
$ avn lstags
$ avn switch @TRUNK
$ avn switch @AthenaServices-01-33-06-branch
$ avn up
$ avn -n some_command # => dryrun (does not execute the command)

"""
__version__ = "$Revision: 508489 $"
__author__ = "Sebastien Binet"

import sys
import os
import commands
import re
import string
import subprocess

tagchars = string.ascii_letters + string.digits + '-' + '_'

# patterns taken from atlas svn-hooks...
_is_tag_name_valid_tc = re.compile("(^[A-Za-z_]+-[A-Za-z_]+-[0-9]{2}-[0-9]{2}-[0-9]{2}$)|(^[A-Za-z_]+-[A-Za-z_]+-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2}$)").match
_is_tag_name_valid =    re.compile("(^[A-Za-z0-9_]+-[0-9]{2}-[0-9]{2}-[0-9]{2}$)|(^[A-Za-z0-9_]+-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2}$)").match
        
class AvnObject(object):
    """dummy object to hold options and variables
    """

    @property
    def url(self):
        url = None
        try:
            url = self._url
        except AttributeError:
            lines = commands.getoutput('svn info').split('\n')
            for l in lines:
                if l.startswith('URL: '):
                    url = self._url = l[5:]
                    break
        if url is None:
            err = "Can't find svn URL."
            print >> sys.stderr, err
            raise RuntimeError(err)
        return url

    @property
    def root(self):
        root = None
        try:
            root = self._root
        except AttributeError:
            lines = commands.getoutput('svn info').split('\n')
            for l in lines:
                if l.startswith('Repository Root: '):
                    root = self._root = l[17:]
                    break
        if root is None:
            err = "Can't find svn repository root. will assume to be $SVNROOT"
            print >> sys.stderr, err
            root = self._root = os.environ['SVNROOT']
        return root

    @property
    def testarea(self):
        try:
            return self._testarea
        except AttributeError:
            if not 'TestArea' in os.environ:
                err = 'TestArea is not defined.'
                print sys.stderr, err
                raise RuntimeError(err)
            self._testarea = os.path.realpath(os.environ['TestArea'])
        return self._testarea

    @property
    def cwd(self):
        cwd = os.getcwd()
        if not cwd.startswith(self.testarea):
            err = 'Current directory is not within TestArea.'
            print >> sys.stderr, err
            raise RuntimeError(err)
        return cwd
    
    @property
    def packpath(self):
        try:
            return self._packpath
        except AttributeError:
            testarea = self.testarea
            packpath = self.cwd[len(testarea):]
            if packpath.startswith('/'):
                packpath = packpath[1:]

            path = None
            while not os.path.exists (os.path.join (testarea, packpath, 'cmt')):
                comp = os.path.basename (packpath)
                packpath = os.path.dirname (packpath)
                if path is None:
                    path = comp
                else:
                    path = os.path.join (comp, path)
                if packpath == '':
                    err = "Can't find cmt directory."
                    print >> sys.stderr, err
                    raise RuntimeError(err)
            self._path = path
            self._packpath = packpath
        return self._packpath

    @property
    def path(self):
        packpath = self.packpath # side-effect will compute self._path
        return self._path
        
    pass # class AvnObject


def mungtag(s, allow_use_cmt=False):
    if allow_use_cmt:
        try:
            return mungtag(s)
        except Exception:
            import PyCmt.Cmt as Cmt
            cmt = Cmt.CmtWrapper()
            pkg = cmt.find_pkg(s.split('-')[0])
            if pkg:
                avn._packpath = os.path.join(pkg.path, pkg.name)
                avn._path = None
            
    if s in ('HEAD','TRUNK'):
        thisurl = os.path.join (avn.root, avn.packpath, 'trunk')
    elif s == 'THIS':
        thisurl = avn.url
    elif s.endswith ('-branch'):
        thisurl = os.path.join (avn.root, avn.packpath, 'branches', s)
    else:
        thisurl = os.path.join (avn.root, avn.packpath, 'tags', s)
    if not (avn.path is None):
        thisurl = os.path.join (thisurl, avn.path)
    return thisurl

def mungarg(s,do_mung=True):
    if do_mung:
        beg = 0
        while True:
            beg = s.find ('@', beg)
            if beg < 0: break
            end = beg + 1
            while end < len(s) and s[end] in tagchars:
                end += 1
            if beg == end:
                beg += 1
                continue
            s = s[:beg] + mungtag(s[beg+1:end], allow_use_cmt=True) + s[end:]
    if s.find(' ') >= 0:
        s = "'" + s + "'"
    return s

avn = AvnObject()
"""the one and only one Atlas Svn helper object"""
del AvnObject

### ---------------------------------------------------------------------------
### command functions

def avn_help():
    print __doc__
    print "normal svn help output follows"
    print "-"*80
    print
    return

def avn_tag(args):
    cmd_args = args[:]
    for i,tag in enumerate(cmd_args):
        if tag[0] == '-':
            if tag.startswith(('-r', '--revision')):
                has_r_switch = True
            continue
        if tag.find(':') < 0:
            if tag[0] == '@':
                tag = tag[1:]
            
            if _is_tag_name_valid(tag):
                tag = mungtag(tag)
                cmd_args[i] = tag
                pass
            pass
        pass
    cmd_args = ['cp', avn.url,] + cmd_args[1:]
    return cmd_args

def avn_lstags(args):
    cmd_args = ['ls', os.path.join (avn.root, avn.packpath, 'tags'),
                os.path.join (avn.root, avn.packpath, 'branches')]
    return cmd_args

def avn_ls(args):
    cmd_args = args[:]
    if cmd_args[1].find('//') < 0:
        cmd_args[1] = os.path.join (avn.root, avn.packpath, cmd_args[1])
    return cmd_args

def avn_diff(args):
    cmd_args = args[:]
    ## if cmd_args[1][0] != '-' and cmd_args[2][0] != '-':
    ##     rev_ref = mungtag(cmd_args[1], allow_use_cmt=True)
    ##     rev_chk = mungtag(cmd_args[2], allow_use_cmt=True)
    ##     if len(cmd_args) == 4:
    ##         file_or_dir = cmd_args[3]
    ##         rev_ref = '/'.join([rev_ref, file_or_dir])
    ##         rev_chk = '/'.join([rev_chk, file_or_dir])
    ##     cmd_args[:] = ['diff', rev_ref, rev_chk]
    return cmd_args

def main(args):
    import os
    
    dryrun = False
    if len(args)>0 and args[0] == '-n':
        dryrun = True
        args = args[1:]

    if not os.environ.has_key ('TestArea'):
        print >> sys.stderr, 'TestArea is not defined.'
        return 1

    avn.dryrun = dryrun
    
    ###

    cmd_args = args[:]

    ## command dispatch...
    if '-h' in args or '--help' in args:
        avn_help()
        
    if len(args)>=2 and args[0] == 'tag':
        cmd_args = avn_tag(args)
        
    elif len(args)>=1 and args[0] == 'lstags':
        cmd_args = avn_lstags(args)
        pass

    if len(args) >= 2 and args[0] == 'ls':
        cmd_args = avn_ls(args)

    if len(args) >= 3 and args[0] == 'diff':
        cmd_args = avn_diff(args)

    args = ['svn']
    do_mung = True # remember if next arg has to munged or not
    for s in cmd_args:
        args.append(mungarg(s,do_mung=do_mung))
        do_mung = True
        if s in ('-m', '--message'):
            do_mung = False

    cmd = subprocess.list2cmdline(args)
    print cmd
    sc = 0
    if not avn.dryrun:
        sc = subprocess.call(args)

    return sc

if __name__ == "__main__":
    import sys
    args = sys.argv[1:]
    sys.exit(main(args))
    
