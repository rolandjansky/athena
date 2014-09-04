#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @file icython.py: a dead simple cython interpreter
# @author Sebastien Binet <binet@cern.ch>
# @date June 2009

__version__ = "$Revision$"
__doc__ = """A dead simple cython interpreter to try out cython statements"""
__author__= "Sebastien Binet <binet@cern.ch>"

### stdlib imports ------------------------------------------------------------
from code import InteractiveConsole

### cython
import pyximport
pyximport.install()

### classes -------------------------------------------------------------------
class CythonInteractiveConsole(InteractiveConsole):
    """A dead simple Cython interpreter
    """

    def __init__(self, *args, **kwds):
        # interactive console is an old-style class
        InteractiveConsole.__init__(self, *args, **kwds)
        import sys
        sys.ps1 = 'cython> '
        sys.ps2 = '.   ... '
        self._pyxbuild_dir = kwds.get('pyxbuild_dir', None)
        self.py_compile = self.compile
        
    def runcode(self, code):
        """%s""" % InteractiveConsole.runcode.__doc__
        # we need to fix-up that method as we are given a C-ext module
        # in case of cython (instead of a code-object)
        import types
        if isinstance(code, types.ModuleType):
            # slam the content of the module into our local namespace
            for k,v in code.__dict__.iteritems():
                if not k.startswith('__'):
                    self.locals[k] = v
        else:
            return InteractiveConsole.runcode(self, code)

    def runsource(self, source, filename="<input>", symbol="single"):
        kwds = dict(source=source, filename=filename, symbol=symbol)
        try:
            code = self.cython_compile(source, filename, symbol)
        except (OverflowError, SyntaxError, ValueError):
            # Case 1
            self.showsyntaxerror(filename)
            return False

        if code is None:
            # Case 2
            return True

        # Case 3
        self.runcode(code)
        return False

    def cython_compile(self, source, filename, symbol):
        try:
            # first try normal python...
            return self.compile(source, filename, symbol)
        except SyntaxError, py_err:
            # maybe a cython-construct...
            try:
                fname = _maybe_cython_compile(source=source, ctx=self)
                if fname is None:
                    # more input needed...
                    return
                from pyximport import load_module as cy_load_module
                mod = cy_load_module("_cython_gen_%s" % id(source), fname)
                import os
                os.remove(fname)
                return mod
            except Exception, cython_err:
                raise SyntaxError(cython_err)
    pass # class CythonInteractiveConsole

### utils ---------------------------------------------------------------------
def code_to_file(source):
    import tempfile
    fd, fname = tempfile.mkstemp(prefix="cython_source_",
                                 suffix='.pyx')
    import os
    os.close(fd)
    if os.path.exists(fname):
        os.remove(fname)
    f = open(fname, 'w')
    f.write(source)
    f.flush()
    f.close()
    return fname

def _maybe_cython_compile(source, filename="<input>", symbol="single", ctx=None):
    # Check for source consisting of only blank lines and comments
    for line in source.split("\n"):
        line = line.strip()
        if line and line[0] != '#':
            break               # Leave it alone
    else:
        if symbol != "eval":
            source = "pass"     # Replace it with a 'pass' statement

    err = err1 = err2 = None
    code = code1 = code2 = None

    lines = source.split("\n")
    if lines[-1].strip() != "":
        return #
    
    from pyximport import pyxbuild
    #pyxbuild.DEBUG = 1
    pyx_to_dll = pyxbuild.pyx_to_dll

    # the following is modeled after python:codeop._maybe_compile
    try:
        fname = code_to_file(source+"\n ")
        code = pyx_to_dll(filename=fname,
                          force_rebuild=1,
                          pyxbuild_dir=ctx._pyxbuild_dir)
    except (pyxbuild.DistutilsError, pyxbuild.CCompilerError), err:
        pass

    try:
        open(fname, "a").write('\n')
        code1 = pyx_to_dll(filename=fname,
                           force_rebuild=1,
                           pyxbuild_dir=ctx._pyxbuild_dir)
    except (pyxbuild.DistutilsError, pyxbuild.CCompilerError), err1:
        pass

    try:
        open(fname, "a").write('\n')
        code2 = pyx_to_dll(filename=fname,
                           force_rebuild=1,
                           pyxbuild_dir=ctx._pyxbuild_dir)
    except (pyxbuild.DistutilsError, pyxbuild.CCompilerError), err2:
        pass

    if code and fname:
        return fname
    if not code1 and repr(err1) == repr(err2):
        raise SyntaxError, err1

### ---------------------------------------------------------------------------
if __name__ == "__main__":
    import user # allow user to inject its own python customizations...
    banner = """
###################################
# Welcome to ICython,             
#  an interactive Cython console  
#  (version=%s)                   
###################################
""" % __version__
    import sys
    cprt = 'Type "help", "copyright", "credits" or "license" for more information.'
    banner += "\nPython %s on %s\n%s\n(%s)\n" % (
        sys.version, sys.platform, cprt,
        CythonInteractiveConsole.__name__)

    # fix-up distutils, see bug #51501
    import platform
    if platform.architecture()[0] == '32bit':
        import distutils.sysconfig as ds
        ds.get_config_vars()['CFLAGS'] += ' -m32'
        ds.get_config_vars()['LDSHARED'] += ' -m32'
    del platform
    
    ns = dict(locals())
    for k in ('_maybe_cython_compile',
              'code_to_file', 'cprt', 'banner', 'InteractiveConsole',
              'CythonInteractiveConsole'
              ):
        del ns[k]
    icython = CythonInteractiveConsole(locals=ns)
    icython.interact(banner=banner)
