#
# $Id: coverage.py,v 1.1 2008-12-16 05:46:38 ssnyder Exp $
#
# File: coverage.py
# Purpose: A coverage utility for component tests.
# Created: Dec 2008, sss from D0 code circa Jan, 2000.
#
# Derived from trace.py by Andrew Dalke and Skip Montanaro, mainly by
# cutting out a whole bunch of stuff.
#
# To do coverage testing for some component 'Foo', add the following
# to the start of Foo's test module:
#
#   import python_util.coverage
#   python_util.coverage.Coverage ('Foo')
#
# If all executable lines in Foo are covered by the test, there will be
# no output.  Otherwise, a summary of the number of uncovered lines
# will be printed to stdout, and a file Foo.cover will be created
# with an annotated source listing for Foo, showing coverage counts
# and uncovered lines.
#
# You can also run coverage testing in conjunction with doctest-based
# regression tests.  In general, doctests can be both in the source file
# itself, or in a separate test file.  If you have a separate test file,
# you can run all the tests for a module DIR.MOD like this:
#
#   from PyUtils import coverage
#   c = coverage.Coverage ('DIR.MOD')
#   c.doctest_cover ()
#
# This will run any doctests in the DIR.MOD source file, plus any
# additional doctests in the test file.  If any lines in the DIR.MOD
# source file are uncovered, a coverage report will be generated.
#
# The coverage testing may be inhibited by setting the environment
# variable NOCOVER.
#
# Coverage testing for an individual line may be suppressed by
# adding '#pragma: NO COVER' to the end of the line.
#
# Original permission notice:
# Copyright 1999, Bioreason, Inc., all rights reserved.
# Author: Andrew Dalke
#
# Copyright 1995-1997, Automatrix, Inc., all rights reserved.
# Author: Skip Montanaro
#
# Copyright 1991-1995, Stichting Mathematisch Centrum, all rights reserved.
#
# Permission to use, copy, modify, and distribute this Python software
# and its associated documentation for any purpose without fee is
# hereby granted, provided that the above copyright notice appears in
# all copies, and that both that copyright notice and this permission
# notice appear in supporting documentation, and that the name of
# neither Automatrix nor Bioreason be used in advertising or publicity
# pertaining to distribution of the software without specific, written
# prior permission.
#

import sys
import string

## The completely brain-damaged fnorb setup overrides the builtin
## parser module, which we need!  Cudgel it out of the way.
#sys.path = [x for x in sys.path if string.find (x, '/fnorb/') < 0]

import re
import os
import __builtin__
import dis
import sys

running_coverage = 0


# Given a code string, return the SET_LINENO information
# This works up to python 2.2
def _find_LINENO_from_code_22(code):
    """return all of the SET_LINENO information from a code block"""
    co_code = code.co_code
    linenos = {}

    # This code was filched from the `dis' module then modified
    n = len(co_code)
    i = 0
    prev_op = None
    prev_lineno = 0
    while i < n:
        c = co_code[i]
        op = ord(c)
        if op == dis.SET_LINENO:
            if prev_op == op:
                # two SET_LINENO in a row, so the previous didn't
                # indicate anything.  This occurs with triple
                # quoted strings (?).  Remove the old one.
                del linenos[prev_lineno]
            prev_lineno = ord(co_code[i+1]) + ord(co_code[i+2])*256
            linenos[prev_lineno] = 1
        if op >= dis.HAVE_ARGUMENT:
            i = i + 3
        else:
            i = i + 1
        prev_op = op
    return linenos


# This works from python 2.3 on.
def _find_LINENO_from_code_23 (code):
    linenos = {}
    for (o, l) in dis.findlinestarts (code):
        linenos[l] = 1
    return linenos

# Choose the appropriate version.
if dis.__dict__.has_key ('SET_LINENO'):
    _find_LINENO_from_code = _find_LINENO_from_code_22
else:
    _find_LINENO_from_code = _find_LINENO_from_code_23

def _find_LINENO(code):
    """return all of the SET_LINENO information from a code object"""
    import types

    # get all of the lineno information from the code of this scope level
    #linenos = _find_LINENO_from_string(code.co_code)
    linenos = _find_LINENO_from_code(code)

    # and check the constants for references to other code objects
    for c in code.co_consts:
        if type(c) == types.CodeType:
            # find another code object, so recurse into it
            linenos.update(_find_LINENO(c))
    return linenos

def find_executable_linenos(filename):
    """return a dict of the line numbers from executable statements in a file

    Works by finding all of the code-like objects in the module then searching
    the byte code for 'SET_LINENO' terms (so this won't work one -O files).

    """
    import parser

    prog = open(filename).read()
    ast = parser.suite(prog)
    code = parser.compileast(ast, filename)

    # The only way I know to find line numbers is to look for the
    # SET_LINENO instructions.  Isn't there some way to get it from
    # the AST?

    return _find_LINENO(code)

class Coverage:
    def __init__(self, modname, toplev_name = 'main'):
        global running_coverage
        running_coverage = 1
        self.modname = modname
        self.toplev_name = toplev_name
        self.counts = {}   # keys are linenumber
        self.mutex = None
        if not os.environ.has_key ('NOCOVER'):
            #self.save_import = __builtin__.__import__
            #__builtin__.__import__ = self.import_hook
            sys.settrace (self.trace)
        return


    #def import_hook (self, name, globals={}, locals={}, fromlist=[]):
    #    loaded = sys.modules.has_key (name)
    #    mod = self.save_import (name, globals, locals, fromlist)
    #    if not loaded:
    #        if name == 'thread_util':
    #            self.set_thread_hook (mod)
    #        elif name == 'itc' or name == 'd0me':
    #            self.set_itc_hook (mod)
    #    return mod


    #def set_thread_hook (self, thread_util):
    #    if not self.mutex:
    #        self.mutex = thread_util.ACE_Thread_Mutex ()
    #    thread_util.Pythread._save_run = thread_util.Pythread._run
    #    def _run (self, coverage=self):
    #        sys.settrace (coverage.trace)
    #        return self._save_run ()
    #    thread_util.Pythread._run = _run
    #    return

    #def set_itc_hook (self, itc):
    #    if not self.mutex:
    #        import thread_util
    #        assert self.mutex != None
    #    self.old_itc_hook = itc._callback_hook
    #    def itc_hook (self=self):
    #        sys.settrace (self.trace)
    #        if self.old_itc_hook: self.old_itc_hook ()
    #        return
    #    itc._callback_hook = itc_hook
    #    return


    def trace(self, frame, why, arg):
        # something is fishy about getting the file name
        modulename = frame.f_globals.get ("__name__")
        if why == 'call':
            # Don't bother tracing if we're not in one of these modules.
            if not (modulename == self.modname or
                    (modulename == '__main__' and
                     frame.f_code.co_name == self.toplev_name)):
                return None
        if why == 'line':
            if modulename == self.modname:
                lineno = frame.f_lineno

                # record the file name and line number of every trace
                if self.mutex: self.mutex.acquire ()
                self.counts[lineno] = self.counts.get(lineno, 0) + 1
                if self.mutex: self.mutex.release ()

        elif why == 'return':
            if frame.f_code.co_name == self.toplev_name:
                sys.settrace (None)
                if self.mutex: self.mutex.acquire ()
                self.analyze ()
                m = self.mutex
                self.mutex = None
                if m: m.release ()

        return self.trace

    def analyze (self):
        filename = sys.modules[self.modname].__file__
        if filename[-4:] == ".pyc" or filename[-4:] == ".pyo":
            orig_filename = filename[:-4] + '.py'
        else:
            orig_filename = filename

        # Get the original lines from the .py file
        try:
            lines = open(orig_filename, 'r').readlines()
        except IOError, err:
            sys.stderr.write(
                "%s: Could not open %s for reading because: %s - skipping\n" %
\
                ("trace", `filename`, err.strerror))
            return

        # there are many places where this is insufficient, like a blank
        # line embedded in a multiline string.
        blank = re.compile(r'^\s*(#.*)?$')

        executable_linenos = find_executable_linenos(orig_filename)

        lines_hit = self.counts
        uncovered = 0
        outlines = []
        for i in range(len(lines)):
            line = lines[i]

            # do the blank/comment match to try to mark more lines
            # (help the reader find stuff that hasn't been covered)
            if lines_hit.has_key(i+1):
                # count precedes the lines that we captured
                prefix = '%5d: ' % lines_hit[i+1]
            elif blank.match(line):
                # blank lines and comments are preceded by dots
                prefix = '    . '
            else:
                # lines preceded by no marks weren't hit
                # Highlight them if so indicated, unless the line contains
                # '#pragma: NO COVER' (it is possible to embed this into
                # the text as a non-comment; no easy fix)
                if executable_linenos.has_key(i+1) and \
                   string.find(lines[i], '#pragma: NO COVER') == -1:
                    prefix = '>>>>>> '
                    uncovered = uncovered + 1
                else:
                    prefix = ' '*7
            outlines.append (prefix + string.expandtabs(line, 8))

        if uncovered:
            print "*** There were %d uncovered lines." % uncovered
        else:
            return

        # build list file name by appending a ".cover" to the module name
        # and sticking it into the specified directory
        listfilename = self.modname + ".cover"
        try:
            outfile = open(listfilename, 'w')
        except IOError, err:
            sys.stderr.write(
                '%s: Could not open %s for writing because: %s - skipping\n' %
                ("trace", `listfilename`, err.strerror))
            return

        for l in outlines:
            outfile.write (l)

        outfile.close()
        
        return


    def doctest_cover (self, *args, **kw):
        import doctest
        m = __import__ (self.modname)

        # Note a peculiarity of __import__: if modname is like A.B,
        # then it returns A not B...
        mm = self.modname.split ('.')
        if len(mm) > 1:
            m = getattr (m, mm[-1])

        oldrun = doctest.DocTestRunner.run
        def xrun (xself, *args, **kw):
            sys.settrace (self.trace)
            return oldrun (xself, *args, **kw)
        doctest.DocTestRunner.run = xrun

        import bdb
        old_set_continue = bdb.Bdb.set_continue
        def xcontinue (xself):
            old_set_continue (xself)
            sys.settrace (self.trace)
            return
        bdb.Bdb.set_continue = xcontinue

        doctest.testmod (m, *args, **kw)

        main = sys.modules['__main__']
        if m != main:
            doctest.testmod (main, *args, **kw)

        self.analyze()
        return
