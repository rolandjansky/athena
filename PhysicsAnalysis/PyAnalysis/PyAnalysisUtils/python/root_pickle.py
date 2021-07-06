# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#
# File: root_pickle.py
# Created: sss, 2004.
# Purpose: Pickle python data into a root file, preserving references
#          to root objects.
#

from __future__ import print_function

"""Pickle python data into a root file, preserving references to root objects.

This module allows pickling python
objects into a root file.  The python objects may contain
references to named root objects.  If one has set up a structure
of python objects to hold root histograms, this provides a
convenient way of saving and restoring your histograms.
The pickled python data are stored in an additional string
object in the root file; any root objects are stored as usual.
(Thus, root files written by root_pickle can be
read just like any other root file if you don't care about the
python data.)

Here's an example of writing a pickle:

   import ROOT
   from PyAnalysisUtils.root_pickle import dump_root
   hlist = []
   for i in range (10):
       name = 'h%d' % i
       hlist.append (ROOT.TH1F (name, name, 10, 0, 10))
   dump_root (hlist, 'test.root')

This writes a list of histograms to test.root.
The histograms may be read back like this:

   import ROOT
   from PyAnalysisUtils.root_pickle import load_root
   hlist = load_root ('test.root')


The following additional notes apply.

 - In addition to dump_root and
   load_root, the module also provides
   dump and load functions, which
   take root file objects instead of file names.

 - The present version of root_pickle will
   not work correctly for the case of python objects deriving
   from root objects.  It will probably also not work for the
   case of root objects which do not derive from
   TObject.

 - When the pickled data are being read, if a class
   doesn't exist, root_pickle will create a
   dummy class with no methods and use that.  This is
   different from the standard pickle behavior (where it
   would be an error), but it simplifies usage in the common
   case where the class is being used to hold histograms, and
   its methods are entirely concerned with filling the
   histograms.

 - When restoring a reference to a root object, the default behavior
   is to not read the root object itself, but instead to create a proxy.
   The root object will then be read the first time the proxy is accessed.
   This can help significantly with time and memory usage if you're
   only accessing a small fraction of the root objects, but it does
   mean that you need to keep the root file open.  Pass use_proxy=0
   to disable this behavior.
"""

import pickle
import ROOT
import sys
import six
from six.moves import intern

def _getdir():
    if hasattr (ROOT.TDirectory, 'CurrentDirectory'):
        d = ROOT.TDirectory.CurrentDirectory()
        if hasattr (d, 'load'):
            # Handle case of CurrentDirectory() returning an atomic.
            d = d.load()
    return ROOT.gDirectory


def _setdir (d):
    ROOT.TDirectory.cd (d)


#
# This stuff was originally written in terms of an stringIO stream.
# But with py3, i couldn't find a better way of getting bytes objects
# into and out of a TString.
#
# Argh!  We can't store NULs in TObjStrings.
# But pickle protocols > 0 are binary protocols, and will get corrupted
# if we truncate at a NUL.
# So, when we save the pickle data, make the mappings:
#  0x00 -> 0xff 0x01
#  0xff -> 0xff 0xfe
# ... This may actually be obsolete --- looks like we can have NULs
# in TObjString now, if we access the TString direectly.  But retain
# for compatibility with existing pickles.
# 


if six.PY2:
    from StringIO import StringIO
    def _protect (s):
        return s.replace ('\377', '\377\376').replace ('\000', '\377\001')
    def _restore (s):
        return s.replace ('\377\001', '\000').replace ('\377\376', '\377')


    class Write_Wrapper:
        def __init__ (self):
            return self.reopen()

        def write (self, s):
            return self.__s.write (_protect (s))

        def getvalue (self):
            return ROOT.TObjString (self.__s.getvalue())

        def reopen (self):
            self.__s = StringIO()
            return


    class Read_Wrapper:
        def __init__ (self):
            return self.reopen()

        def read (self, i):
            return self.__s.read (i)

        def readline (self):
            return self.__s.readline ()

        def setvalue (self, s):
            self.__s = StringIO (_restore (s.GetName()))
            return

        def reopen (self):
            self.__s = StringIO()
            return


else:
    class Write_Wrapper:
        def __init__ (self):
            return self.reopen()

        def write (self, s):
            ss = self._str
            log = []
            for c in s:
                code = c
                if code == 0xff:
                    ss.Append (0xff)
                    ss.Append (0xfe)
                    log.append (0xff)
                    log.append (0xfe)
                elif code == 0x00:
                    ss.Append (0xff)
                    ss.Append (0x01)
                    log.append (0xff)
                    log.append (0x01)
                else:
                    ss.Append (code)
                    log.append (code)
            return

        def getvalue (self):
            return self._s

        def reopen (self):
            self._s = ROOT.TObjString()
            self._str = self._s.String()
            return


    class Read_Wrapper:
        def __init__ (self):
            return self.reopen()


        def read (self, i):
            out = []
            slen = len(self._str)
            while i != 0 and self._pos < slen:
                c = ord(self._str[self._pos])
                if c == 0xff:
                    self._pos += 1
                    if self._pos >= slen:
                        break
                    c = ord(self._str[self._pos])
                    if c == 0x01:
                        c = 0x00
                    elif c == 0xfe:
                        c = 0xff
                out.append (c)
                self._pos += 1
                i -= 1
            return bytes(out)


        def readline (self):
            out = []
            slen = len(self._str)
            while self._pos < slen:
                c = ord(self._str[self._pos])
                if c == 0xff:
                    self._pos += 1
                    if self._pos >= slen:
                        break
                    c = ord(self._str[self._pos])
                    if c == 0x01:
                        c = 0x00
                    elif c == 0xfe:
                        c = 0xff
                out.append (c)
                self._pos += 1
                if c == 10:
                    break
            return bytes(out)


        def setvalue (self, s):
            self._s = s
            self._str = self._s.String()
            self._pos = 0
            return


        def reopen (self):
            self.setvalue (ROOT.TObjString())
            return


class Pickler(object):
    def __init__ (self, file, proto=0):
        """Create a root pickler.
FILE should be a Root TFile.  PROTO is the python pickle protocol
version to use.  The python part will be pickled to a Root
TObjString called _pickle; it will contain references to the
Root objects.
"""
        self.__file = file
        self.__keys = file.GetListOfKeys()
        self.__io = Write_Wrapper()
        self.__pickle = pickle.Pickler (self.__io, proto)
        self.__pickle.persistent_id = self._persistent_id
        self.__pmap = {}
        return


    def dump (self, o, key=None):
        """Write a pickled representation of o to the open TFile."""
        if key is None:
            key = '_pickle'
        dir = _getdir()
        try:
            self.__file.cd()
            self.__pickle.dump (o)
            s = self.__io.getvalue()
            self.__io.reopen()
            s.Write (key)
            self.__file.Flush()
            self.__pmap.clear()
        finally:
            _setdir (dir)
        return


    def clear_memo (self):
        """Clears the pickler's internal memo."""
        self.__pickle.memo.clear()
        return
    


    def _persistent_id (self, o):
        if hasattr (o, '_Root_Proxy__obj'):
            o = o._Root_Proxy__obj()
        if (isinstance (o, ROOT.TObject)):
            # Write the object, and return the resulting NAME;CYCLE.
            # We used to to this like this:
            #o.Write()
            #k = self.__file.GetKey (o.GetName())
            #pid = "%s;%d" % (k.GetName(), k.GetCycle())
            # It turns out, though, that destroying the python objects
            # referencing the TKeys is quite expensive (O(logN) where
            # N is the total number of pyroot objects?).  Although
            # we want to allow for the case of saving multiple objects
            # with the same name, the most common case is that the name
            # has not already been written to the file.  So we optimize
            # for that case, doing the key lookup before we write the
            # object, not after.  (Note further: GetKey() is very slow
            # if the key does not actually exist, as it does a linear
            # search of the key list.  We use FindObject instead for the
            # initial lookup, which is a hashed lookup, but it is not
            # guaranteed to find the highest cycle.  So if we do
            # find an existing key, we need to look up again using GetKey.
            nm = o.GetName()
            k = self.__keys.FindObject(nm)
            o.Write()
            if k:
                k = self.__file.GetKey (nm)
                pid = "%s;%d" % (nm, k.GetCycle())
            else:
                pid = nm + ";1"
            return pid
        return



class Saver(object):
    def __init__ (self):
        self.__chunksize = 65536
        self.__i = self.__chunksize
        self.__chunks = []
        return

    def add (self, o):
        if self.__i >= self.__chunksize:
            self.__chunks.append ([None] * self.__chunksize)
            self.__i = 0
        self.__chunks[-1][self.__i] = o
        self.__i += 1
        return


_compat_hooks = None
xsave=Saver()
class Root_Proxy (object):
    __slots__ = ('__f', '__pid', '__o')
    def __init__ (self, f, pid):
        self.__f = f
        self.__pid = intern(pid)
        self.__o = None
        return
    def __getattr__ (self, a):
        if self.__o is None:
            self.__o = self.__f.Get (self.__pid)
            if self.__o.__class__.__module__ != 'ROOT':
                self.__o.__class__.__module__ = 'ROOT'
        return getattr (self.__o, a)
    def __obj (self):
        if self.__o is None:
            self.__o = self.__f.Get (self.__pid)
            if self.__o.__class__.__module__ != 'ROOT':
                self.__o.__class__.__module__ = 'ROOT'
        return self.__o
class Unpickler (pickle.Unpickler):
    def __init__ (self, file, use_proxy = True, use_hash = False):
        """Create a root unpickler.
FILE should be a Root TFile.
"""
        self.__use_proxy = use_proxy
        self.__file = file
        self.__io = Read_Wrapper()
        pickle.Unpickler.__init__ (self, self.__io)

        self.__n = 0
        xsave.add (file)

        if use_hash:
            htab = {}
            ctab = {}
            for k in file.GetListOfKeys():
                nm = k.GetName()
                cy = k.GetCycle()
                htab[(nm,cy)] = k
                if cy > ctab.get(nm,0):
                    ctab[nm] = cy
                    htab[(nm,9999)] = k
            file._htab = htab
            oget = file.Get
            def xget (nm0):
                nm = nm0
                ipos = nm.find (';')
                if ipos >= 0:
                    cy = nm[ipos+1]
                    if cy == '*':
                        cy = 10000
                    else:
                        cy = int(cy)
                    nm = nm[:ipos-1]
                else:
                    cy = 9999
                ret = htab.get ((nm,cy), None)
                if not ret:
                    print ("did't find", nm, cy, len(htab))
                    return oget (nm0)
                #ctx = ROOT.TDirectory.TContext (file)
                ret = ret.ReadObj()
                #del ctx
                return ret
            file.Get = xget
        return


    def load (self, key=None):
        """Read a pickled object representation from the open file."""
        if key is None:
            key = '_pickle'
        o = None
        if _compat_hooks:
            save = _compat_hooks[0]()
        try:
            self.__n += 1
            s = self.__file.Get (key + ';%d' % self.__n)
            self.__io.setvalue (s)
            o = pickle.Unpickler.load(self)
            self.__io.reopen ()
        finally:
            if _compat_hooks:
                save = _compat_hooks[1](save)
        return o
    
    def persistent_load (self, pid):
        if self.__use_proxy:
            o = Root_Proxy (self.__file, pid)
        else:
            o = self.__file.Get (pid)
        #print ('load ', pid, o)
        xsave.add(o)
        return o


    def find_class (self, module, name):
        try:
            try:
                __import__(module)
                mod = sys.modules[module]
            except ImportError:
                print ("Making dummy module %s" % (module))
                class DummyModule:
                    pass
                mod = DummyModule()
                sys.modules[module] = mod
            klass = getattr(mod, name)
            return klass
        except AttributeError:
            print ("Making dummy class %s.%s" % (module, name))
            mod = sys.modules[module]
            class Dummy(object):
                pass
            setattr (mod, name, Dummy)
            return Dummy
        return
        


def compat_hooks (hooks):
    """Set compatibility hooks.
If this is set, then hooks[0] is called before loading,
and hooks[1] is called after loading.  hooks[1] is called with
the return value of hooks[0] as an argument.  This is useful
for backwards compatibility in some situations."""
    global _compat_hooks
    _compat_hooks = hooks
    return


def dump (o, f, proto=0, key=None):
    """Dump object O to the Root TFile F."""
    return Pickler(f, proto).dump(o, key)

def load (f, use_proxy = 1, key=None):
    """Load an object from the Root TFile F."""
    return Unpickler(f, use_proxy).load(key)

def dump_root (o, fname, proto=0, key=None):
    """Dump object O to the Root file named FNAME."""
    f = ROOT.TFile (fname , "RECREATE")
    dump (o, f, proto, key)
    f.Close()
    return

def load_root (fname, use_proxy = 1, key=None):
    """Load an object from the Root file named FNAME."""
    return load (ROOT.TFile (fname), use_proxy, key)


