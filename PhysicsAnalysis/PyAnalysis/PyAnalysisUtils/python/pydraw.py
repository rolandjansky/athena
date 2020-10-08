# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: pydraw.py
# Created: sss, a while ago.  Added to ATLAS repository, Dec 2008
# Purpose: Interactive python commands for plotting from a tuple-like object.
#

from __future__ import print_function

"""Interactive python commands for plotting from a tuple-like object.
This module provides in python functionality similar to TTree::Draw
and TTree::Scan.  The major conceptual differences are:

 - All expressions are written in python (and not the specialized
   TTree::Draw language).
 - This can be applied to any type of object providing a simple loop
   interface (explained below), not just to TTree's.  (A TTree will work;
   there is also a wrapper for the Athena event loop.)

In addition, this style of query may be somewhat
easier to type interactively.


Quick start
===========

Here are some examples of plotting commands to give you the flavor:

  d tt.metx
  d tt.ele_pt$i; 50 0 100*gev
  d tt.metx:mety
  d tt[100:1000].muo$i.pt() if abs(muo.eta)<2
  d tt.(ele$i+ele$j).m() if $i<$j; same
  scan tt.metx:mety

To use these commands, you can pass them as strings to pydraw.cmd:

  from pydraw import cmd
  cmd('d tt.metx')

Alternatively, if you execute the cmdhook function, you can type them
directly:

  from pydraw import cmdhook
  cmdhook()
  d tt.metx

(Note that this second form doesn't work in general for code that's read
from a file.)


Draw syntax
===========

The general syntax for drawing a histogram looks like this:

  d TUPLESPEC.[STMT@ ...]EXPR[:EXPR] [if EXPR] [; HISTSPEC]

These pieces are explained in more detail below.


Tuple specifications
--------------------

The object on which the draw operation is to be performed is given
by TUPLESPEC.  This should be the name of some object in the python
global dictionary.  Note that if the expression for the tuple
itself contains a period or brackets, you'll need to enclose it in parentheses:

  d (trees.tt[1])[:100].ele_pt$i

Optionally, this can include a slice-like notation
to restrict the rows to which the draw applies:

  tuple[i:j]  --- process rows i <= n < j
  tuple[:j]   --- process rows n < j
  tuple[i:]   --- process rows n >= i
  tuple[i]    --- process row i

i and j can be expressions as well.

The tuple object should contain a method like this:

  def loop (self, func, looplo, loophi)

This function should loop over rows [looplo, loophi) and
call func on each.  func will take two arguments, the first
being the row number and the second being the event object
(which may just be the tuple itself).

If a method named loop is not available, but GetEntry and GetEntries
methods are available, then those will be used instead.
Similarly, if size and seekEvent are available, a wrapper
appropriate for the Athena event loop will be used.

Thus, you can make plots from within Athena like this:
    
   d theApp.ElectronAODCollection$i.eta()


Expressions
-----------

Once a tuple is specified, one then needs to specify what to plot.
This is done with an arbitrary python expression, with some extensions.

To plot a simple numeric variable from the tuple, just give its name.
For example:

  d tt.foo

will plot the variable `foo' from each row of the tuple.  You can of course
also use more complicated expressions:

  d tt.r*cos(phi)

In the common case, though, the members of the tuple will be vectors
of numbers of objects over which you want to iterate.  You can iterate
plotting over such a vector by specifying a dummy index.  This is an
identifier starting with a dollar sign:

  d tt.pt_e$i

(Making the dummy indices explicit was a deliberate change from the
implicit iteration of root/paw.  Such implicit iteration sometimes
caused confusion about just what was being iterated over, and also
when people needed to distinguish between accessing attributes
of a container and attributes of the contained objects.)

This will automatically step $i over the contents of the vector pt_e,
plotting each one.  The vector being iterated over may contain objects
as well as numbers:

  d tt.ele$i.pt()

Note that the limits of the iteration are handled automatically.
The index $i can also be used in contexts other than that of an index.
For example, to limit the iteration in the above example to the first
four items, one can use:

  d tt.ele$i.pt() if $i<4

(This uses a selection clause.  This is to be described later, but the meaning
should be obvious.)

If a dummy index is used for more than one vector, the iteration limit
will be taken from whichever vector is smaller.  For example:

  d tt.ele$i+muo$i

will iterate min(len(ele),len(muo)) times.

Multiple dummy indices may be used.  In that case, all possible index
combinations are used (a simple nested loop).  If there is a symmetry
involved, a selection clause may be used to eliminate duplicates.
For example (given an appropriate ele definition), this will plot the
invariant masses of all electron pairs:

  d tt.(ele$i+ele$j).m() if $i>$j

One can also reference individual elements of a vector using the
notation $N.  Here, the indexing is 1-based (so $1 is the first element).
Example: to plot the pt of the leading electron

  d tt.ele$1.pt()

Note that one can do something similar just using standard python
indexing directly:

  d tt.ele[0].pt()

This, however, will crash if ele is empty; if the $N form is used,
an implicit selection is added to ensure that the index is in range.
Thus, the proper equivalent of the $N-form example is:

  d tt.ele[0].pt() if len(ele)>0

You can also use $nCONT as an abbreviation for len(CONT):

  d tt.ele[0].pt() if $nele>0

Names used in expressions will be looked up first in the tuple object,
then in the global dictionary.  Note that the lookup in the tuple happens
before looping starts, so all needed variables must already be present
in the tuple.  If you want to refer to the tuple object itself,
you can use the special variable `_ev'.  (This can be changed by assigning
a string to pydraw._evtvar.)

Multiple expressions may be given, separated by `:'.  For drawing histograms,
you can give two expressions, for the x and y axis expressions.  For scanning
tuples (see below), you give the list of expressions to be scanned.


Selections
----------

You can select items to be filled using a boolean expression, set off
with an `if' keyword:

  d tt.ele$i.pt() if abs(ele$i.eta())<2

All dummy variables are common between the variable expressions
and the selection.


Statements
----------

You can specify arbitrary Python expressions to be executed before looping
starts.  These come before the variables expressions, and are delimited
with `@'.  This is useful mostly for defining short aliases for tuple
variables.  For example:

  d tt.ele=ElectronAODCollection@ele$i.eta() if ele$i.pt()>100*gev


Histogram specifications
------------------------

Histogram specifications follow a semicolon.  They can have the forms:

  ! OPTIONS...
  >>NAME OPTIONS...
  NX XLO XHI OPTIONS...
  NX XLO XHI NY YLO YHI OPTIONS...
  OPTIONS...

For the first form, specifying ! reuses the same histogram that was used
for the last plot.

For the second form, specifying >>NAME looks in the global dictionary
for a histogram NAME and uses that.

Otherwise, a new histogram is created.  The binning for this histogram
may be specified by NX XLO XHI and NY YLO YHI.

Plotting options may also be given.  The special option `prof' means
to create a profile histogram.  Otherwise, root drawing options may
be used.  This package uses PyAnalysisUtils.draw_obj to draw the
histograms, so the extra options supported by that function may
also be used.  See draw_obj.py for details.

The last histogram to have been drawn is available is pydraw.last_hist.


Scan syntax
===========

The scan command is very similar to draw:

  scan TUPLESPEC.[STMT@ ...]EXPR[:EXPR] [if EXPR]

Instead of drawing a histogram, scan prints out a table of the expression
values.

The formatting of the data printed by scan is currently pretty rudimentary.
This should probably be improved.


Loop syntax
===========

There is also a loop command:

  loop TUPLESPEC.[STMT@ ...]EXPR[:EXPR] [if EXPR]

Loop will evaluate the given expressions in the same manner as draw and scan,
but the results of this are ignored.  So it only makes sense to use loop
to evaluate expressions for their side effects.  This can be used,
for example, to call a function that fills some large set of histograms.


Running commands
================

The general interface to execute one of these commands is the `cmd' function,
which takes the command as a string:

  from pydraw import cmd
  cmd ('d tt.foo')

Each command is also implemented by a single function, which may be called
directly.  The command name should not be included in this case:

  from pydraw import draw
  draw ('tt.foo')

Finally, if you call the function cmdhook(), then you can give the commands
directly on the python command line:

  from pydraw import cmdhook
  cmdhook()
  d tt.foo


Bugs/stuff missing
==================
 - No way to specify an event weight when filling a histogram using
   the draw command.

 - Hoist selection code out of the dummy index loops, when they don't
   depend on the index?  For example,

     d tt.foo$i if ht>100

   gets implemented like:

     for _it_foo in foo:
       if _ev.ht>100:
         Fill(_it_foo)

   but it would be more efficient to pull the selection out of the loop.

 - In an expr like d em.foo$i if em.bar$i>1
   then foo always gets evaluated even if the condition is false.

 - Scan formatting.
     
 
"""


import sys
import string
import tokenize
import token
import copy
import ROOT
import cppyy # noqa: F401
import six
if six.PY2:
    from StringIO import StringIO #pragma: NO COVER
else:
    from io import StringIO #pragma: NO COVER
from PyAnalysisUtils.draw_obj import draw_obj, get_canvas


try:
    ScatterH2 = ROOT.RootUtils.ScatterH2
except AttributeError: #pragma: NO COVER
    ScatterH2 = ROOT.TH2F #pragma: NO COVER
    print ("WARNING: RootUtils::ScatterH2 not available; using TH2F instead") #pragma: NO COVER


try:
    ROOT.TH1.kCanRebin
    def _setCanRebin (h):                     #pragma: NO COVER
        h.SetBit (ROOT.TH1.kCanRebin)         #pragma: NO COVER
    def _hasCanRebin (h):                     #pragma: NO COVER
        return h.TestBit (ROOT.TH1.kCanRebin) #pragma: NO COVER
except AttributeError:                        #pragma: NO COVER
    def _setCanRebin (h):                     #pragma: NO COVER
        h.GetXaxis().SetCanExtend(True)       #pragma: NO COVER
    def _hasCanRebin (h):                     #pragma: NO COVER
        return h.GetXaxis().CanExtend()       #pragma: NO COVER


# The last histogram we made.
last_hist = None


# Dictionary in which to find global names.
_globals = sys.modules['__main__'].__dict__


# Characters that are legal in identifiers.
_idchars = string.ascii_letters + string.digits + '_'

# This is what's used as the event formal argument in the generated functions.
_evtvar = '_ev'

# Set this to true to dump out the generated function bodies.
_debug = False

def _sanitize_hname(s):
    """Name a string safe to use as a histogram name.

    Root does bad things if you put / in a histogram name, so we remove them.
    Examples:
    >>> print (_sanitize_hname('foo'))
    foo
    >>> print (_sanitize_hname('foo/bar'))
    foo DIV bar
    """
    return s.replace ('/', ' DIV ')


def _untokenize (tokens):
    """Transform tokens back into Python source code.

    Each element returned by the iterable must be a token sequence
    with at least two elements, a token number and token value.

    Unlike tokenize.untokenize(), this does not handle multiple lines.
    It also tries not to add unneeded spaces.

    Examples:
    >>> from tokenize import generate_tokens, untokenize
    >>> import six
    >>> if six.PY2:
    ...     from StringIO import StringIO
    ... else:
    ...     from io import StringIO
    >>> def untokenize1(tt):
    ...   tt=list(tt)
    ...   if tt[-1][0]==0: tt=tt[:-1]
    ...   return untokenize(tt)
    >>> untokenize1(generate_tokens(StringIO('1+1').readline))
    '1+1'
    >>> _untokenize(generate_tokens(StringIO('1+1').readline))
    '1+1'
    >>> untokenize1(generate_tokens(StringIO('foo$i>2*h').readline))
    'foo$i>2*h'
    >>> _untokenize(generate_tokens(StringIO('foo$i>2*h').readline))
    'foo$i>2*h'
    """
    lastname = False
    toks = []
    toks_append = toks.append
    for tok in tokens:
        toknum, tokval = tok[:2]
        tokval = tokval.strip()
        if toknum in (token.NAME, token.NUMBER):
            if lastname:
                tokval = ' ' + tokval
            lastname = True
        else:
            lastname = False
        toks_append (tokval)
    return ''.join(toks)


def _find_outer (haystack, needle, ignore_delim = False):
    """Look for NEEDLE in HAYSTACK (token-based.  Return pair (HEAD, TAIL).

    HAYSTACK and NEEDLE are both strings.  Look for a token in HAYSTACK with
    a value matching NEEDLE that is outside of any paired delimiters.
    Also ignores things in strings.
    If IGNORE_DELIM is True, then we do find things inside delimiters
    (strings are still ignored).

    Returns a pair (HEAD, TAIL) of the pieces of the string before and
    after NEEDLE.  If there is no match, returns (HAYSTACK, None).
    Note that whitespace and formatting in HEAD and TAIL may differ
    from the original string.

    Examples:
    >>> _find_outer ("head.tail1.tail2", ".")
    ('head', 'tail1.tail2')
    >>> _find_outer ("(head.tail1).tail2", ".")
    ('(head.tail1)', 'tail2')
    >>> _find_outer ("[a for a in foo if good(a)] if bar", "if")
    ('[a for a in foo if good(a)]', 'bar')
    >>> _find_outer ("(a [b {c . d } ] ) . e", ".")
    ('(a [b {c . d } ] )', 'e')
    >>> _find_outer ("a.b", ";")
    ('a.b', None)
    >>> _find_outer ("a '$' b", '$')
    ("a '$' b", None)
    >>> _find_outer ("a $ b", '$')
    ('a', 'b')
    >>> _find_outer ("(head.tail1).tail2", ".", True)
    ('(head', 'tail1).tail2')
    >>> _find_outer ('a; 1 -1 1', ';')
    ('a', '1 -1 1')
"""
    tlist = tokenize.generate_tokens (StringIO(haystack).readline)
    pend = []
    head = []
    for (i, (tnum, val, a, b, c)) in enumerate (tlist):
        if tnum != token.STRING and not pend and val == needle:
            col1 = a[1]
            col2 = b[1]
            return (haystack[:col1].strip(),
                    haystack[col2:].strip())
        if not ignore_delim:
            if val == '(':
                pend.append (')')
            elif val == '[':
                pend.append (']')
            elif val == '{':
                pend.append ('}')
            elif pend and val == pend[-1]:
                pend.pop()
        head.append ((tnum, val))
    return (haystack, None)


def _split_outer (haystack, needle):
    """Split HAYSTACK at the delimiters NEEDLE, as in _find_outer.

    Examples:
    >>> _split_outer ("a,(b,c),d", ",")
    ['a', '(b,c)', 'd']
    >>> _split_outer ("a,,b", ",")
    ['a', '', 'b']
    >>> _split_outer ("a", ",")
    ['a']
    >>> #_split_outer ("", ",")
    []
"""
    out = []
    while True:
        (head, tail) = _find_outer (haystack, needle)
        head = head.strip()
        out.append (head)
        if tail is None:
            break
        else:
            haystack = tail
    return out


class TreeLoopWrapper(object):
    """Wrapper for TTree, supplying a loop method.

    This class wraps a TTree class and provides a loop method
    that will work with pydraw.
"""

    def __init__ (self, tree):
        """Make a wrapper for a tree."""
        self._tree = tree
        return

    def loop (self, f, looplo=0, loophi=sys.maxsize):
        """Call f(i,tree) on rows [looplo, loophi)"""
        tree = self._tree
        loophi = min (loophi, tree.GetEntries())
        getentry = tree.GetEntry
        for i in range(looplo, loophi):
            getentry(i)
            f(i, tree)
        return
    

class AthenaLoopWrapper(object):
    """Wrapper for the Athena event loop, supplying a loop method.

    This class wraps an application manager object and provides a loop method
    that will work with pydraw.
"""
    def __init__ (self, app=None):
        from AthenaPython import PyAthena                  #pragma: NO COVER
        if app is None:                                    #pragma: NO COVER
            from AthenaCommon.AppMgr import theApp         #pragma: NO COVER
            app = theApp                                   #pragma: NO COVER
        self._app = app                                    #pragma: NO COVER
        self._sg = PyAthena.py_svc('StoreGateSvc')         #pragma: NO COVER
        return                                             #pragma: NO COVER

    
    def loop (self, f, looplo=0, loophi=sys.maxsize):
        """Call f(i,tree) on rows [looplo, loophi)"""
        loophi = min (loophi, self._app.size())            #pragma: NO COVER
        getentry = self._app.seekEvent                     #pragma: NO COVER
        for i in range(looplo, loophi):                    #pragma: NO COVER
            getentry(i)                                    #pragma: NO COVER
            f(i, self)                                     #pragma: NO COVER
        return                                             #pragma: NO COVER


    def __getattr__ (self, v):
        if not v.startswith('_'):                          #pragma: NO COVER
                return self._sg[v]                         #pragma: NO COVER
        raise AttributeError()                             #pragma: NO COVER


class _Loopvar(object):
    """Holds information about a dummy loop variable.

    Attributes:
      name - The name of the dummy variable.
      ids  - Set of loop identifiers (`foo' in `foo$i') with which
             this dummy has been used.
      explicit - Set to true if this variable is ever used on its own
             (just $i)
    """

    def __init__ (self, name):
        """Initialize given the name."""
        self.name = name
        self.ids = set()
        self.explicit = 0
        return

    def itname (self, id):
        """Return the iterator variable for this dummy and loop identifier ID.
        """
        return "_it_%s_%s" % (self.name, id)

    def dumname (self):
        """Return the dummy variable name for this dummy."""
        return "_dum_" + self.name

    def add_id (self, id):
        """Notice this this dummy is used with loop identifier ID.

        Return the iterator variable.
        """
        self.ids.add (id)
        return self.itname (id)

    def get_ids (self):
        """Return the list of loop identifiers with which we've been used."""
        return list (self.ids)


class Draw_Cmd(object):
    """Holds information used to implement a draw/scan/loop command.

    Pass the draw string to the constructor.  See the file-level comments
    for details on the syntax of this.  This will define the
    following attributes:

      errstr -   If set to a string, there was an error.
                 Should be None if everything's ok.
      tuple -    The name of the tuple object.
      tuple_o -  Tuple object.
      lo -       The lower bound for row iteration.
      hi -       The upper bound for row iteration.
      stmts -    List of additional statements.
      exprs -    List of draw expressions.
      sel -      Selection expression or None.
      sel_orig - Untransformed selection expression or None.
      expr_orig- Untransformed plotting expression.
      histspec - The text following `;', split into space-separated words.

    Other attributes:
      _iddict -  Map from loop identifiers (`foo' in `foo$i')
                 to temp variables used to reference
                 them in the loop function.
      _limdict - Map from loop identifiers (`foo' in `foo$2')
                 to the largest explicit index seen.
      _loopdict - Map of loop dummy variable names to _Loopvar instances.
"""

    def __init__ (self, s):
        """Initialize from a draw string.  See above for more details."""

        # Assume no error.
        self.errstr = None

        self._iddict = {}
        self._limdict = {}
        self._loopdict = {}

        try:
            self._tupleparse (s)
        except Exception as e:
            import traceback
            self.errstr = str(e)
            self.excstr = traceback.format_exc()
        return



    def _tupleparse (self, s):
        """Parse a draw string.  See above for more details."""

        # Split off the histspec.
        (s, self.histspec) = _find_outer (s, ';')

        # ??? Don't split at spaces in delimiters.
        # _find_outer doesn't really work for this since it operates
        # on the tokenized string, in which whitespace doesn't appear.
        if self.histspec is None:
            self.histspec = []
        else:
            self.histspec = self.histspec.split ()

        # Gotta have something.
        s = s.strip()
        if not s:
            self.errstr = "Empty draw string."
            return

        # Split off the tuple part --- before the first period.
        (tuple, s) = _find_outer (s, '.')
        if not s:
            self.errstr = "Missing period in tuple specification."
            return

        # Handle a range specification on the sample.
        self._parserange (tuple)

        # Try to find the sample.
        try:
            self.tuple_o = eval (self.tuple, _globals)
        except NameError:
            self.tuple_o = None
        if not self.tuple_o:
            self.errstr = "Can't find sample " + self.tuple
            return

        # Look for additional statements.
        self.stmts = _split_outer (s, '@')
        s = self.stmts[-1]
        del self.stmts[-1]
        self.stmts = [self._mung_expr(x) for x in self.stmts]

        # Split off the selection.
        (self.expr_orig, self.sel_orig) = _find_outer (s, "if")
        self.sel = self._mung_expr (self.sel_orig)

        self.exprs = [self._mung_expr(x) for x in
                      _split_outer (self.expr_orig, ':')]

        # Check the interface of the sample.  If it doesn't have
        # the loop interface but has the root tree interface,
        # use a wrapper.
        if hasattr (self.tuple_o, 'loop'):
            # Ok --- has the loop interface.
            pass
        elif (hasattr (self.tuple_o, 'GetEntry') and
              hasattr (self.tuple_o, 'GetEntries')):
            # Has the TTree interface.  Use a wrapper.
            self.tuple_o = TreeLoopWrapper (self.tuple_o)
        elif (hasattr (self.tuple_o, 'size') and
              hasattr (self.tuple_o, 'seekEvent')):           #pragma: NO COVER
            # Has the appmgr interface.  Use a wrapper.
            self.tuple_o = AthenaLoopWrapper (self.tuple_o)   #pragma: NO COVER
        else:
            # An error --- complain.
            self.errstr = ("Sample " + self.tuple +
                           " doesn't have a correct interface.")
            return

        return



    def _parserange (self, tuple):
        """Parse the range part of a draw string.

        See above for more details.
        Fills self.tuple, self.lo, self.hi.
        """
        lo = 0
        hi = sys.maxsize
        (tuple, tail) = _find_outer (tuple, '[')
        if tail:
            g = copy.copy (_globals)

            pos = tail.find (':')
            pos2 = tail.find (']')
            if pos2 < 0:
                    pos2 = len (tail) #pragma: NO COVER
            if pos < 0:
                slo = tail[:pos2].strip()
                if len (slo) > 0:
                    lo = int (eval (slo, g))
                hi = lo + 1
            else:
                slo = tail[:pos].strip()
                if len (slo) > 0:
                    lo = int (eval (slo, g))
                shi = tail[pos+1:pos2].strip()
                if len (shi) > 0:
                    hi = int (eval (shi, g))

        if tuple[0] == '(' and tuple[-1] == ')':
            tuple = tuple[1:-1].strip()
        self.tuple = tuple
        self.lo = lo
        self.hi = hi
        return


    def _mung_id (self, id):
        """Given a loop identifier (`foo' in `foo$i'), return the identifier
        used to reference it in loop functions.
        """
        out = self._iddict.get (id)
        if not out:
            out = '_e_' + id
            self._iddict[id] = out
        return out


    def _mung_index (self, s1, s2):
        """Handle an explicit index reference; i.e., `foo$2'.

        S1 and S2 are pieces of the string before and after the `$'.
        Returns the modified string.
        """
        pos2 = 0
        while pos2 < len(s2) and s2[pos2] in string.digits:
            pos2 += 1
        if pos2 == 0:
            self.errstr = "Bad index"
            return ''
        i = int (s2[:pos2])
        if i < 1:
            self.errstr = "Bad index"
            return ''
        s = ("[%d]" % (i-1)) + s2[pos2:]
        pos2 = len(s1)-1
        while pos2 >= 0 and s1[pos2] in _idchars:
            pos2 -= 1
        pos2 += 1
        if pos2 == len(s1):
            self.errstr = "Bad index"
            return ''
        id = s1[pos2:]
        s = s1[:pos2] + self._mung_id (id) + s
        self._limdict[id] = max (i, self._limdict.get(id, 0))
        return s


    def _mung_n (self, s1, s2):
        """Handle a length reference; i.e., `$nfoo'.

        S1 and S2 are pieces of the string before and after the `$'.
        Returns the modified string.
        """
        pos2 = 1
        while pos2 < len(s2) and s2[pos2] in _idchars:
            pos2 += 1
        id = s2[1:pos2]
        s = s1 + (" len(%s)" % self._mung_id(id)) + s2[pos2:]
        return s


    def _mung_loop (self, s1, s2):
        """Handle use of a dummy loop variable, such as foo$i.

        S1 and S2 are pieces of the string before and after the `$'.
        Returns the modified string.
        """

        # Scan after the $ to find the dummy variable.
        pos2 = 0
        while pos2 < len(s2) and s2[pos2] in _idchars:
            pos2 += 1
        if pos2 == 0:
            self.errstr = "Bad loop var"
            return ''
        loopvar =  s2[:pos2]

        # Look it up.  Make a new _Loopvar object if it's not in the map.
        ll = self._loopdict.get (loopvar)
        if not ll:
            ll = _Loopvar(loopvar)
            self._loopdict[loopvar] = ll

        # Is the $ after an identifier?
        if len(s1) > 0 and s1[-1] in _idchars:
            # Yes --- find the identifier.
            pos3 = len(s1)-1
            while pos3 >= 0 and s1[pos3] in _idchars:
                pos3 -= 1
            pos3 += 1
            assert (len(s1) - pos3 >= 1)
            id = s1[pos3:]

            # Replace with the iterator.
            s = s1[:pos3] + ll.add_id(id) + s2[pos2:]
            self._mung_id (id)
        else:
            # Explicit use of the dummy.
            # Replace with the dummy name and note that it was used explicitly.
            s = s1 + ("%s" % ll.dumname()) + s2[pos2:]
            ll.explicit = 1
        return s


    def _mung_expr_dollar (self, s):
        """Process $ constructions in string S.

        Returns the modified string.
        """
        if not s:
            return s
        pos = 0
        while 1:
            (s1, s2) = _find_outer (s[pos:], '$', True)
            if s2 is None:
                break
            snew = None
            if len(s2) > 0:
                if s2[0] in string.digits:
                    snew = self._mung_index (s1, s2)
                elif (s2[0] == 'n' and
                      (not (len(s1) > 0 and s1[-1] in _idchars) or
                       s1.endswith (' and') or
                       s1.endswith (' or') or
                       s1.endswith ('not'))):
                    snew = self._mung_n (s1, s2)
                elif s2[0] in string.ascii_letters:
                    snew = self._mung_loop (s1, s2)
            s = s[:pos]
            if snew is None:
                snew = s1 + '$' + s2
                pos = pos + len(s1)+1
            s = s + snew
        return s


    def _mung_expr_ids (self, s):
        """Perform id substitution in S.

        For identifiers in S that are attributes of our tuple,
        replace them with references to the tuple attribute
        (using _mung_id).

        Returns the modified string.
        """
        if not s:
            return s

        tlist = tokenize.generate_tokens (StringIO(s).readline)
        out = []
        for tnum, val, a, b, c in tlist:
            if tnum == token.NAME:
                if hasattr (self.tuple_o, val):
                    val = self._mung_id (val)
                    #val = _evtvar + '.' + val
            out.append ((tnum, val))
        return _untokenize (out)


    def _mung_expr (self, s):
        """Process $ constructions and id substitution in string S.

        Returns the modified string.
        """
        s = self._mung_expr_dollar (s)
        return self._mung_expr_ids (s)

    
    def _make_func (self, payload, extargs = ''):
        """Create the text for the function to process this query.

        PAYLOAD is the payload expression to plug in.
        EXTARGS is an additional string to add to the end of the
        function's argument list (to set default values, for example).
        Returns the function definition as a string.
        """

        sel = self.sel
        if self._limdict:
            limsel = ' and '.join (["len(%s)>=%d" % (self._iddict[p[0]], p[1])
                                    for p in self._limdict.items()])
            if not sel:
                sel = limsel
            else:
                sel = limsel + " and (" + sel + ")"

        ftext = "def _loopfunc(_i, %s%s):\n" % (_evtvar, extargs)
        for (id1, id2) in sorted(self._iddict.items()):
            ftext += "  %s = %s.%s\n" % (id2, _evtvar, id1)
        indent = 2

        for (i,l) in sorted(self._loopdict.items()):
            ids = sorted(l.get_ids())
            assert (not not ids)
            if len(ids) == 1:
                vars = l.itname (ids[0])
                lists = self._iddict[ids[0]]
            else:
                vars = "(" + ','.join([l.itname (id) for id in ids]) + ")"
                lists = ("zip(" + ','.join([self._iddict[id] for id in ids])
                         + ")")
            if l.explicit:
                vars = "(%s,%s)" % (l.dumname(), vars)
                lists = "enumerate(%s)" % lists
            ftext += ' '*indent + "for %s in %s:\n" % (vars, lists)
            indent += 2

        for s in self.stmts:
            ftext += ' '*indent + s + '\n'

        if sel and sel != '1':
            ftext += ' '*indent + "if (%s):\n" % sel
            indent += 2

        ftext += ' '*indent + "%s\n" % payload

        if _debug:
            print (ftext)
        
        return ftext


class _Bins(object):
    """Holds the results of _get_bins.  Defined attributes:

       nbins
       lo
       hi
       rebin
       """


def _get_bins (args, ndim, axis):
    """Parse bin specifications from split list of arguments ARGS.
    NDIM is 1 or 2, and AXIS is 0 or 1, for the x or y axis.

    Examples:
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> import ROOT
    >>> ROOT.gPad.Range(0, 1,2,3)
    >>> b = _get_bins (["50", "10", "100"], 1, 0)
    >>> print (b.nbins, b.lo, b.hi, b.rebin)
    50 10.0 100.0 0
    >>> b = _get_bins ([], 1, 0)
    >>> print (b.nbins, b.lo, b.hi, b.rebin)
    50 0 1 1
    >>> b = _get_bins (["!", "10"], 1, 0)
    >>> print (b.nbins, b.lo, b.hi, b.rebin)
    50 10.0 11.0 1
    >>> b = _get_bins (["!", "!", "10"], 1, 0)
    >>> print (b.nbins, b.lo, b.hi, b.rebin)
    50 0 10.0 0
    >>> scale = 10
    >>> b = _get_bins (["50", "0", "2*scale"], 1, 0)
    >>> print (b.nbins, b.lo, b.hi, b.rebin)
    50 0.0 20.0 0
    >>> b = _get_bins ([], 2, 0)
    >>> print (b.nbins, b.lo, b.hi, b.rebin)
    50 0.0 2.0 1
    >>> b = _get_bins ([], 2, 1)
    >>> print (b.nbins, b.lo, b.hi, b.rebin)
    50 1.0 3.0 1
    >>> b = _get_bins ([], 2, 2)
    Traceback (most recent call last):
        ...
    AssertionError
    """
    
    g = copy.copy (_globals)

    bins = _Bins()

    bins.nbins = 0
    if len(args) >= 1 and args[0] != '!' and len(args[0]) > 0:
        bins.nbins = int (eval (args[0], g))
    if bins.nbins <= 0:
        bins.nbins = 50

    bins.lo = 0
    if len(args) >= 2 and args[1] != '!' and len(args[1]) > 0:
        bins.lo = float (eval (args[1], g))

    bins.hi = 0
    if len(args) >= 3 and args[2] != '!' and len(args[2]) > 0:
        bins.hi = float (eval (args[2], g))

    bins.rebin = 0
    if bins.hi <= bins.lo:
        bins.rebin = 1
        if ndim == 1:
            bins.hi = bins.lo + 1
        elif axis == 0:
            bins.lo = ROOT.gPad.GetUxmin()
            bins.hi = ROOT.gPad.GetUxmax()
        elif axis == 1:
            bins.lo = ROOT.gPad.GetUymin()
            bins.hi = ROOT.gPad.GetUymax()
        else:
            assert 0

    return bins


def _get_hist (ndim, args, hname, htitle):
    """Create a new histogram from options.

    NDIM is the dimensionality of the histogram (1 or 2).
    ARGS is a list of the arguments given to specify the histogram.
    HNAME and HTITLE are the histogram name and title, respectively.
    """
    get_canvas()

    # Get the x-axis bin specifications.
    xbins = _get_bins (args, ndim, 0)
    rebin = xbins.rebin

    # Get the y-axis bin specifications.
    if ndim >= 2:
        ybins = _get_bins (args[3:], ndim, 1)
        rebin = rebin or ybins.rebin

    profile = 0
    # Look for drawing options.
    options = ''
    for i in range (0, len(args)):
        if args[i][0] in string.ascii_letters:
            for j in range (i, len(args)):
                if ndim == 2 and args[j].lower() == "prof":
                    profile = 1
                    args[j] = ''
            options = ' '.join (args[i:])
            break

    # Delete any old object of the same name.
    hold = ROOT.gROOT.FindObject (hname)
    if hold:
        # n.b. TObject::Delete() doesn't work reliably for objects
        # created via reflex.  (reflex doesn't handle custom
        # new/delete, so TStorage won't be called to allocate
        # the object, so IsOnHeap might return false for it.)
        # Force the issue by doing a C++ delete directly.
        ROOT.gROOT.ProcessLine ("delete (%s*)%d" %
                                (hold.__class__.__cpp_name__,
                                 ROOT.AddressOf(hold)[0]))

    # Create the histogram.
    if profile:
        hist = ROOT.TProfile (hname, htitle, xbins.nbins, xbins.lo, xbins.hi)
        if not ybins.rebin:
            hist.SetMinimum (ybins.lo)
            hist.SetMaximum (ybins.hi)
    elif ndim == 1:
        hist = ROOT.TH1F (hname, htitle, xbins.nbins, xbins.lo, xbins.hi)
    elif ndim == 2:
        hist = ScatterH2 (hname, htitle,
                          xbins.nbins, xbins.lo, xbins.hi,
                          ybins.nbins, ybins.lo, ybins.hi)
        if hasattr (hist, 'scatter'):
            hist.scatter (1)

    # Automatic rebinning?
    if rebin:
        _setCanRebin (hist)

    return (hist, options)


def draw (arg):
    """Process a draw command.

    ARG is the command arguments (without the command word itself).
    See the header comments for the command syntax.
    """

    global last_hist

    # Initial parsing of the arguments.
    c = Draw_Cmd (arg)
    if c.errstr:
        print (c.errstr)
        return False

    # Construct the expression to use to fill the histogram.
    if len (c.exprs) == 1:
        ndim = 1
        payload = "_hfill (%s)" % c.exprs[0]
    else:
        ndim = 2
        payload = "_hfill ((%s),(%s))" % (c.exprs[0], c.exprs[1])
        
    # Construct the histogram title.
    htitle = "%s.%s" % (c.tuple, c.expr_orig)
    if c.sel_orig:
        htitle = htitle + '{%s}' % c.sel_orig

    # Make the histogram.
    # If it's `!', then we just use the last one.
    if len(c.histspec) >= 1 and c.histspec[0] == "!" and last_hist is not None:
        hist = last_hist
        options = ' '.join (c.histspec[1:])
    elif len(c.histspec) >= 1 and c.histspec[0][:2] == '>>':
        hname = c.histspec[0][2:]
        hist = _globals.get (hname)
        options = ' '.join (c.histspec[1:])
    else:
        (hist, options) = _get_hist (ndim, c.histspec,
                                     _sanitize_hname(c.tuple+'.'+c.expr_orig), htitle)

    # Remember it.
    last_hist = hist

    # Generate the function.
    # It will be defined as _loopfunc in g.
    g = copy.copy (_globals)
    g['_hfill'] = hist.Fill
    ftext = c._make_func (payload, ', _hfill = _hfill')
    exec (ftext, g)

    # Execute the loop over the data.
    c.tuple_o.loop (g['_loopfunc'], c.lo, c.hi)

    # Adjust binning, if requested.
    if _hasCanRebin (hist):
        hist.LabelsDeflate ("X")
        if ndim > 1:
            hist.LabelsDeflate ("Y")

    # Draw the histogram.
    draw_obj (hist, options)
    return True


def _scan_print (i, *args):
    """Helper to print out one row of a scan.

    I is the row number and ARGS is a tuple of the column values."""
    
    s = '%6d' % i
    for a in args:
        if isinstance(a, six.integer_types):
            s += ' %8d' % a
        else:
            s += ' %8g' % a
    print (s)
    return


def scan (arg):
    """Process a scan command.

    ARG is the command arguments (without the command word itself).
    See the header comments for the command syntax.
    """

    # Initial parsing of the arguments.
    c = Draw_Cmd (arg)
    if c.errstr:
        print (c.errstr)
        return False

    payload = "_print (_i, %s)" % \
              ','.join (['(%s)'%e for e in c.exprs])

    # Generate the function.
    # It will be defined as _loopfunc in g.
    g = copy.copy (_globals)
    g['_print'] = _scan_print
    ftext = c._make_func (payload, ', _print = _print')
    exec (ftext, g)

    # Execute the loop over the data.
    c.tuple_o.loop (g['_loopfunc'], c.lo, c.hi)

    return True


def loop (arg):
    """Process a loop command.

    ARG is the command arguments (without the command word itself).
    See the header comments for the command syntax.
    """

    # Initial parsing of the arguments.
    c = Draw_Cmd (arg)
    if c.errstr:
        print (c.errstr)
        return False

    payload = "(%s,)" % ','.join (c.exprs)

    # Generate the function.
    # It will be defined as _loopfunc in g.
    g = copy.copy (_globals)
    ftext = c._make_func (payload)
    exec (ftext, g)

    # Execute the loop over the data.
    c.tuple_o.loop (g['_loopfunc'], c.lo, c.hi)

    return True


# Dictionary of command handlers.
# Should return True if cmd was handled.
_cmddict = {'d': draw,
            'draw' : draw,
            'scan' : scan,
            'loop' : loop,
            }


def cmd (s):
    """Process a command S.

    Returns True if the command was handled, False otherwise.
    See the header comments for the command syntax.
    """

    ssplit = s.split (None, 1)

    if len(ssplit) < 2:
        return False

    cmd, args = ssplit

    func = _cmddict.get (cmd)
    if func:
        return func (args)
  
    return False



#############################################################################
# The stuff here sets things up so that after calling cmdhook(), the
# user can enter drawing commands directly at the python prompt.
# We do this by catching syntax errors; when we get a syntax error,
# we try to parse the input as one of our commands.
#

#
# Hook holding the original value of the exception hook.
# But be careful not to overwrite this if this file is reread.
#
if '_orig_ehook' not in globals():
    _orig_ehook = None


def _excepthook (exctype, value, traceb):
    """Exception hook used by pydraw to process drawing commands."""

    # If it's a syntax error, try interpreting as a drawing command.
    if isinstance (value, SyntaxError):
        val = value.text
        if val[-1] == '\n':
                val = val[:-1] #pragma: NO COVER
        if cmd (val):
            # Success --- update root stuff and return.
            # (This will swallow the original syntax error.)
            ROOT.gInterpreter.EndOfLineAction()
            return

    # No luck --- pass it on to the original exception handler.
    _orig_ehook (exctype, value, traceb)


def cmdhook():
    """Enable entering drawing commands directly at the python prompt."""

    # Store the old value of the exception hook (only if we haven't
    # done so already).
    global _orig_ehook
    if _orig_ehook is None:
        _orig_ehook = sys.excepthook

    # Install our handler.
    sys.excepthook = _excepthook
    return


#############################################################################

# import ROOT
# import string
# import sys
# import exceptions
# import copy
# from PyAnalysisUtils.draw_obj import draw_obj, get_canvas

# try:
#     ScatterH2 = ROOT.RootUtils.ScatterH2
# except AttributeError:
#     ScatterH2 = ROOT.TH2F
#     print ("WARNING: RootUtils::ScatterH2 not available; using TH2F instead")

# kCanRebin = ROOT.TH1.kCanRebin

# _last_hist = None
# #_marker_style = 3
# #_marker_size = 0.5

# _evtvar = 'e'






# from array import array
# class hist_filler:
#     def __init__ (self, hist, nbuf=100):
#         self.hist = hist
#         self.filln = hist.FillN
#         self.nbuf = nbuf
#         self.xarr = array ('d', 100*[0.])
#         self.warr = array ('d', 100*[1.])
#         self.i = [0]

#         self.xlist = 100*[0.]
#         return
#     def get_filler (self):
#         def fill (x,i=self.i,nbuf=self.nbuf,xarr=self.xarr,flush=self.flush):
#             ii = i[0]
#             if ii == nbuf:
#                 flush()
#                 ii = i[0]
#             xarr[ii] = x
#             i[0] = ii + 1
#             return
#         return fill
#     def flush (self):
#         #print (self.i, self.xarr)
#         self.filln (self.i[0], self.xarr, self.warr)
#         self.i[0] = 0
#         return
            
# ##############################################################################
# # Here are the command handlers.
