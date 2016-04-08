# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: pydraw_t.py,v 1.3 2009-02-24 03:22:49 ssnyder Exp $
# File: pydraw_t.py
# Created: Dec, 2008
# Purpose: Regression tests for pydraw.py
#


import sys


def maxrep(x):
    if x == sys.maxint:
        return 'maxint'
    return x


class Tree_Test:
    def GetEntries (self):
        return 10
    def GetEntry (self, i):
        print 'GetEntry', i
        return


class Loop_Test:
    def  __init__ (self):
        self.foo = []
        self.bar = []
        self.fee = 0
        return
    def loop (self, f, lo, hi):
        if lo < 0: lo = 0
        if hi > 10: hi = 10
        for i in range(lo, hi):
            self.foo = [i*10+j for j in range(5)]
            self.bar = [i*10+j for j in range(5,10)]
            self.fee = i+0.5
            f(i, self)
        return


xsum=[0]
def sum(x):
    xsum[0] += x
    return


def _regr_parserange():
    """Regression tests for Draw_Cmd._parserange
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> from PyAnalysisUtils.pydraw import Draw_Cmd
    >>> tt = Loop_Test()
    >>> c = Draw_Cmd ("tt.foo")
    >>> print c.errstr, c.tuple, c.lo, maxrep(c.hi)
    None tt 0 maxint
    >>> c = Draw_Cmd ("tt[10].foo")
    >>> print c.errstr, c.tuple, c.lo, c.hi
    None tt 10 11
    >>> c = Draw_Cmd ("tt[10:].foo")
    >>> print c.errstr, c.tuple, c.lo, maxrep(c.hi)
    None tt 10 maxint
    >>> c = Draw_Cmd ("tt[:10].foo")
    >>> print c.errstr, c.tuple, c.lo, c.hi
    None tt 0 10
    >>> c = Draw_Cmd ("tt[10:20].foo")
    >>> print c.errstr, c.tuple, c.lo, c.hi
    None tt 10 20
    >>> ten = 10
    >>> twenty = 20
    >>> c = Draw_Cmd ("tt[ten:twenty].foo")
    >>> print c.errstr, c.tuple, c.lo, c.hi
    None tt 10 20
    >>> c = Draw_Cmd ("(tt)[ten:twenty].foo")
    >>> print c.errstr, c.tuple, c.lo, c.hi
    None tt 10 20
    """


def _regr_tupleparse():
    """Regression tests for Draw_Cmd._tupleparse
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> from PyAnalysisUtils.pydraw import Draw_Cmd
    >>> tt = Loop_Test()
    >>> c = Draw_Cmd ("tt.foo")
    >>> print c.errstr, c.tuple, c.lo, maxrep(c.hi), c.histspec, c.exprs, c.sel, c.stmts
    None tt 0 maxint [] ['_e_foo'] None []
    >>> c = Draw_Cmd ("tt.foo:fee; bar")
    >>> print c.errstr, c.tuple, c.lo, maxrep(c.hi), c.histspec, c.exprs, c.sel, c.stmts
    None tt 0 maxint ['bar'] ['_e_foo', '_e_fee'] None []
    >>> c = Draw_Cmd ("")
    >>> print c.errstr
    Empty draw string.
    >>> c = Draw_Cmd ("tt")
    >>> print c.errstr
    Missing period in tuple specification.
    >>> c = Draw_Cmd ("tt.a=b@d=c@foo; bar fee")
    >>> print c.errstr, c.tuple, c.lo, maxrep(c.hi), c.histspec, c.exprs, c.sel, c.stmts
    None tt 0 maxint ['bar', 'fee'] ['_e_foo'] None ['a=b', 'd=c']
    >>> c = Draw_Cmd ("tt.foo if bee==1")
    >>> print c.errstr, c.tuple, c.lo, maxrep(c.hi), c.histspec, c.exprs, c.sel, c.stmts
    None tt 0 maxint [] ['_e_foo'] bee==1 []
    >>> c = Draw_Cmd ("tt.[f in foo if f.x()] if bee==1")
    >>> print c.errstr, c.tuple, c.lo, maxrep(c.hi), c.histspec, c.exprs, c.sel, c.stmts
    None tt 0 maxint [] ['[f in _e_foo if f.x()]'] bee==1 []
    >>> c = Draw_Cmd ("xx.bar")
    >>> print c.errstr
    Can't find sample xx
    >>> c = Draw_Cmd ("tt[1/0].foo")
    >>> print c.errstr
    integer division or modulo by zero
    >>> yy = 1
    >>> c = Draw_Cmd ("yy.bar")
    >>> print c.errstr
    Sample yy doesn't have a correct interface.
    >>> zz = Tree_Test()
    >>> c = Draw_Cmd ("zz.bar")
    >>> print c.errstr, c.tuple_o.__class__
    None PyAnalysisUtils.pydraw.TreeLoopWrapper
    >>> c = Draw_Cmd ("tt.foo$i:bar$i if bar$i>0")
    >>> print c.sel, c.sel_orig, c.exprs, c.expr_orig
    _it_i_bar>0 bar$i>0 ['_it_i_foo', '_it_i_bar'] foo$i:bar$i
    >>> c = Draw_Cmd ("tt.foo; bar   baz")
    >>> print c.errstr, c.tuple, c.lo, maxrep(c.hi), c.histspec, c.exprs, c.sel, c.stmts
    None tt 0 maxint ['bar', 'baz'] ['_e_foo'] None []
    """


def _regr_mung_id():
    """Regression tests for Draw_Cmd._mung_id
    >>> from PyAnalysisUtils import pydraw
    >>> tt = Loop_Test()
    >>> from PyAnalysisUtils.pydraw import Draw_Cmd
    >>> c = Draw_Cmd ("tt.foo")
    >>> print c._mung_id ("foo")
    _e_foo
    >>> print c._mung_id ("bar")
    _e_bar
    >>> print c._mung_id ("foo")
    _e_foo
    >>> from test.test_support import sortdict
    >>> print sortdict (c._iddict)
    {'bar': '_e_bar', 'foo': '_e_foo'}
    """


def _regr_mung_index():
    """Regression tests for Draw_Cmd._mung_index
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> tt = Loop_Test()
    >>> from PyAnalysisUtils.pydraw import Draw_Cmd
    >>> c = Draw_Cmd ("tt.foo")
    >>> print c._mung_index ("foo", "2"), c.errstr
    _e_foo[1] None
    >>> print c._mung_index ("foo", "x"), c.errstr
     Bad index
    >>> c.errstr = None
    >>> print c._mung_index ("foo", "0"), c.errstr
     Bad index
    >>> c.errstr = None
    >>> print c._mung_index ("...", "2"), c.errstr
     Bad index
    >>> c.errstr = None
    >>> from test.test_support import sortdict
    >>> print sortdict (c._limdict)
    {'foo': 2}
    >>> print c._mung_index ("foo", "3"), c.errstr
    _e_foo[2] None
    >>> print sortdict (c._limdict)
    {'foo': 3}
    >>> print c._mung_index ("foo", "1"), c.errstr
    _e_foo[0] None
    >>> print sortdict (c._limdict)
    {'foo': 3}
    """


def _regr_mung_n():
    """Regression tests for Draw_Cmd._mung_n
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> tt = Loop_Test()
    >>> from PyAnalysisUtils.pydraw import Draw_Cmd
    >>> c = Draw_Cmd ("tt.foo")
    >>> print c._mung_n ("abc*", "nfoo+2"), c.errstr
    abc* len(_e_foo)+2 None
    """


def _regr_Loopvar():
    """Regression tests for Loopvar
    >>> from PyAnalysisUtils import pydraw
    >>> from PyAnalysisUtils.pydraw import _Loopvar
    >>> l = _Loopvar ('a')
    >>> print l.name
    a
    >>> print l.explicit
    0
    >>> l.explicit = 1
    >>> print l.explicit
    1
    >>> print l.get_ids()
    []
    >>> print l.itname ('foo')
    _it_a_foo
    >>> print l.dumname()
    _dum_a
    >>> print l.add_id ('foo')
    _it_a_foo
    >>> print l.add_id ('bar')
    _it_a_bar
    >>> print l.get_ids()
    ['foo', 'bar']
    """


def _regr_mung_loop():
    """Regression tests for Draw_Cmd._mung_loop
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> tt = Loop_Test()
    >>> from PyAnalysisUtils.pydraw import Draw_Cmd
    >>> c = Draw_Cmd ("tt.foo")
    >>> print c._mung_loop ('1+foo', 'i+2'), c.errstr
    1+_it_i_foo+2 None
    >>> print c._mung_loop ('1+foo', ''), c.errstr
     Bad loop var
    >>> c.errstr = None
    >>> print c._mung_loop ('1+', 'j+2'), c.errstr
    1+_dum_j+2 None
    >>> print c._mung_loop ('1+bar', 'i+2'), c.errstr
    1+_it_i_bar+2 None
    >>> print c._mung_loop ('1+bar', 'j+2'), c.errstr
    1+_it_j_bar+2 None
    >>> print c._mung_loop ('', 'j'), c.errstr
    _dum_j None
    >>> print c._mung_loop ('l', 'j'), c.errstr
    _it_j_l None
    >>> ll = c._loopdict.keys()
    >>> ll.sort()
    >>> print ll
    ['i', 'j']
    >>> print c._loopdict['i'].get_ids(), c._loopdict['i'].explicit
    ['foo', 'bar'] 0
    >>> print c._loopdict['j'].get_ids(), c._loopdict['j'].explicit
    ['bar', 'l'] 1
    """


def _regr_mung_expr_dollar():
    """Regression tests for Draw_Cmd._mung_expr_dollar
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> tt = Loop_Test()
    >>> from PyAnalysisUtils.pydraw import Draw_Cmd
    >>> c = Draw_Cmd ("tt.foo")
    >>> print c._mung_expr_dollar ("1+foo+2"), c.errstr
    1+foo+2 None
    >>> print c._mung_expr_dollar ("1+foo$i+len('$foo')+bar $1+$i+2"), c.errstr
    1+_it_i_foo+len('$foo')+_e_bar[0]+_dum_i+2 None
    >>> print c._mung_expr_dollar ("foo$i+($i+2)"), c.errstr
    _it_i_foo+(_dum_i+2) None
    >>> print c._mung_expr_dollar ("1+$nfoo+bar$n+2"), c.errstr
    1+ len(_e_foo)+_it_n_bar+2 None
    >>> print c._mung_expr_dollar ("1+$+2"), c.errstr
    1+$+2 None
    >>> print c._mung_expr_dollar (None), c.errstr
    None None
    >>> print c._mung_expr_dollar ("cond and $nfoo>0"), c.errstr
    cond and len(_e_foo)>0 None
    """


def _regr_mung_expr_ids():
    """Regression tests for Draw_Cmd._mung_expr_ids
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> tt = Loop_Test()
    >>> tt.foo=[1,2,3,4]
    >>> from PyAnalysisUtils.pydraw import Draw_Cmd
    >>> c = Draw_Cmd ("tt.foo")
    >>> print c._mung_expr_ids ("1+xyz+2"), c.errstr
    1+xyz+2 None
    >>> print c._mung_expr_ids ("1+foo+2"), c.errstr
    1+_e_foo+2 None
    >>> print c._mung_expr_ids (None), c.errstr
    None None
    """


def _regr_mung_expr():
    """Regression tests for Draw_Cmd._mung_expr
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> tt = Loop_Test()
    >>> tt.foo=[1,2,3,4]
    >>> from PyAnalysisUtils.pydraw import Draw_Cmd
    >>> c = Draw_Cmd ("tt.foo")
    >>> print c._mung_expr ("1+foo+bar$i+2"), c.errstr
    1+_e_foo+_it_i_bar+2 None
    >>> print c._mung_expr (None), c.errstr
    None None
    """


def _regr_make_func():
    """Regression tests for Draw_Cmd._make_func
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> tt = Loop_Test()
    >>> from PyAnalysisUtils.pydraw import Draw_Cmd
    >>> pydraw._debug = True
    >>> c = Draw_Cmd ("tt.asd")
    >>> print c._make_func ("foo()"), c.errstr
    def _loopfunc(_i, _ev):
      foo()
    <BLANKLINE>
    def _loopfunc(_i, _ev):
      foo()
    None
    >>> pydraw._debug = False
    >>> c = Draw_Cmd ("tt.bar$i+bar$1+$i+fee$j if fee$j<10")
    >>> print c._make_func ("foo()"), c.errstr
    def _loopfunc(_i, _ev):
      _e_fee = _ev.fee
      _e_bar = _ev.bar
      for (_dum_i,_it_i_bar) in enumerate(_e_bar):
        for _it_j_fee in _e_fee:
          if (len(_e_bar)>=1 and (_it_j_fee<10)):
            foo()
    None
    >>> c = Draw_Cmd ("tt.ele=_ev.xx@ele[0]")
    >>> print c._make_func ("foo()"), c.errstr
    def _loopfunc(_i, _ev):
      ele=_ev.xx
      foo()
    None
    >>> c = Draw_Cmd ("tt.foo$10")
    >>> print c._make_func ("foo()"), c.errstr
    def _loopfunc(_i, _ev):
      _e_foo = _ev.foo
      if (len(_e_foo)>=10):
        foo()
    None
    >>> c = Draw_Cmd ("tt.foo$i+bar$i")
    >>> print c._make_func ("foo()"), c.errstr
    def _loopfunc(_i, _ev):
      _e_foo = _ev.foo
      _e_bar = _ev.bar
      for (_it_i_foo,_it_i_bar) in zip(_e_foo,_e_bar):
        foo()
    None
    >>> c = Draw_Cmd ("tt.foo$i if ($i%2)==0 and max(bar)<10")
    >>> print c._make_func ("foo()"), c.errstr
    def _loopfunc(_i, _ev):
      _e_foo = _ev.foo
      _e_bar = _ev.bar
      for (_dum_i,_it_i_foo) in enumerate(_e_foo):
        if ((_dum_i%2)==0 and max(_e_bar)<10):
          foo()
    None
    """


def floop(i, e):
    print 'floop', i, e.__class__.__name__
    return

def _regr_tree_loop_wrapper():
    """Regression test for TreeLoopWrapper.
    >>> from PyAnalysisUtils import pydraw
    >>> w = pydraw.TreeLoopWrapper (Tree_Test())
    >>> w.loop(floop)
    GetEntry 0
    floop 0 Tree_Test
    GetEntry 1
    floop 1 Tree_Test
    GetEntry 2
    floop 2 Tree_Test
    GetEntry 3
    floop 3 Tree_Test
    GetEntry 4
    floop 4 Tree_Test
    GetEntry 5
    floop 5 Tree_Test
    GetEntry 6
    floop 6 Tree_Test
    GetEntry 7
    floop 7 Tree_Test
    GetEntry 8
    floop 8 Tree_Test
    GetEntry 9
    floop 9 Tree_Test
    >>> w.loop(floop, 2, 4)
    GetEntry 2
    floop 2 Tree_Test
    GetEntry 3
    floop 3 Tree_Test
    """

def _regr_get_hist():
    """Regression tests for _get_hist
    >>> import ROOT
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> from PyAnalysisUtils.pydraw import _get_hist
    >>> (h, opt) = _get_hist (1, ['50','0','10'], "tmp1", "tmp2")
    >>> print h.__class__.__name__, h.GetName(), h.GetTitle()
    TH1F tmp1 tmp2
    >>> print h.GetXaxis().GetNbins(), h.GetXaxis().GetXmin(), \
        h.GetXaxis().GetXmax(), bool (pydraw._hasCanRebin (h))
    50 0.0 10.0 False
    >>> print opt
    <BLANKLINE>
    >>> (h, opt) = _get_hist (1, [], "tmp1", "tmp2")
    >>> print h.__class__.__name__, h.GetName(), h.GetTitle()
    TH1F tmp1 tmp2
    >>> print h.GetXaxis().GetNbins(), h.GetXaxis().GetXmin(), \
        h.GetXaxis().GetXmax(), bool (pydraw._hasCanRebin (h))
    50 0.0 1.0 True
    >>> print opt
    <BLANKLINE>
    >>> (h, opt) = _get_hist (2, ['50','0','10','10','2','5','fooopt'], "tmp1", "tmp2")
    >>> print h.__class__.__name__, h.GetName(), h.GetTitle()
    RootUtils::ScatterH2 tmp1 tmp2
    >>> print h.GetXaxis().GetNbins(), h.GetXaxis().GetXmin(), \
        h.GetXaxis().GetXmax(), bool (pydraw._hasCanRebin (h))
    50 0.0 10.0 False
    >>> print h.GetYaxis().GetNbins(), h.GetYaxis().GetXmin(), \
        h.GetYaxis().GetXmax(), bool (h.scatter())
    10 2.0 5.0 True
    >>> print opt
    fooopt
    >>> del h
    >>> (h, opt) = _get_hist (2, ['50','0','10','10','2','5','fooopt','prof'], "tmp1", "tmp2")
    >>> print h.__class__.__name__, h.GetName(), h.GetTitle()
    TProfile tmp1 tmp2
    >>> print h.GetXaxis().GetNbins(), h.GetXaxis().GetXmin(), \
        h.GetXaxis().GetXmax(), bool (pydraw._hasCanRebin (h))
    50 0.0 10.0 False
    >>> print h.GetYaxis().GetNbins(), h.GetYaxis().GetXmin(), \
        h.GetYaxis().GetXmax(), h.GetMinimum(), h.GetMaximum()
    1 0.0 1.0 2.0 5.0
    >>> print opt
    fooopt 
    """

def _regr_draw():
    """Regression tests for draw
    >>> import ROOT
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> tt = Loop_Test()
    >>> from PyAnalysisUtils.pydraw import draw
    >>> pydraw._debug = True
    >>> draw("tt.foo$i; 30 0 150")
    def _loopfunc(_i, _ev, _hfill = _hfill):
      _e_foo = _ev.foo
      for _it_i_foo in _e_foo:
        _hfill (_it_i_foo)
    <BLANKLINE>
    True
    >>> print pydraw.last_hist.GetName(), pydraw.last_hist.GetTitle()
    foo$i tt.foo$i
    >>> print pydraw.last_hist.GetXaxis().GetNbins()
    30
    >>> print [pydraw.last_hist.GetBinContent(i) for i in range(1,31)]
    [5.0, 0.0, 5.0, 0.0, 5.0, 0.0, 5.0, 0.0, 5.0, 0.0, 5.0, 0.0, 5.0, 0.0, 5.0, 0.0, 5.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    >>> draw("tt.foo$i if ($i%2)==0; 30 0 150")
    def _loopfunc(_i, _ev, _hfill = _hfill):
      _e_foo = _ev.foo
      for (_dum_i,_it_i_foo) in enumerate(_e_foo):
        if ((_dum_i%2)==0):
          _hfill (_it_i_foo)
    <BLANKLINE>
    True
    >>> print pydraw.last_hist.GetName(), pydraw.last_hist.GetTitle()
    foo$i tt.foo$i{($i%2)==0}
    >>> print pydraw.last_hist.GetXaxis().GetNbins()
    30
    >>> print [pydraw.last_hist.GetBinContent(i) for i in range(1,31)]
    [3.0, 0.0, 3.0, 0.0, 3.0, 0.0, 3.0, 0.0, 3.0, 0.0, 3.0, 0.0, 3.0, 0.0, 3.0, 0.0, 3.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    >>> h1 = ROOT.TH1F ('h1', 'h1', 40, 0, 120)
    >>> draw("tt.foo$i; >>h1")
    def _loopfunc(_i, _ev, _hfill = _hfill):
      _e_foo = _ev.foo
      for _it_i_foo in _e_foo:
        _hfill (_it_i_foo)
    <BLANKLINE>
    True
    >>> print id(h1) == id(pydraw.last_hist)
    True
    >>> print pydraw.last_hist.GetXaxis().GetNbins()
    40
    >>> print [pydraw.last_hist.GetBinContent(i) for i in range(1,31)]
    [3.0, 2.0, 0.0, 2.0, 3.0, 0.0, 1.0, 3.0, 1.0, 0.0, 3.0, 2.0, 0.0, 2.0, 3.0, 0.0, 1.0, 3.0, 1.0, 0.0, 3.0, 2.0, 0.0, 2.0, 3.0, 0.0, 1.0, 3.0, 1.0, 0.0]
    >>> draw("tt.foo$i; !")
    def _loopfunc(_i, _ev, _hfill = _hfill):
      _e_foo = _ev.foo
      for _it_i_foo in _e_foo:
        _hfill (_it_i_foo)
    <BLANKLINE>
    True
    >>> print id(h1) == id(pydraw.last_hist)
    True
    >>> print pydraw.last_hist.GetXaxis().GetNbins()
    40
    >>> print [pydraw.last_hist.GetBinContent(i) for i in range(1,31)]
    [6.0, 4.0, 0.0, 4.0, 6.0, 0.0, 2.0, 6.0, 2.0, 0.0, 6.0, 4.0, 0.0, 4.0, 6.0, 0.0, 2.0, 6.0, 2.0, 0.0, 6.0, 4.0, 0.0, 4.0, 6.0, 0.0, 2.0, 6.0, 2.0, 0.0]
    >>> draw("alkskjd")
    Missing period in tuple specification.
    False
    >>> draw("tt.foo$i:bar$i")
    def _loopfunc(_i, _ev, _hfill = _hfill):
      _e_foo = _ev.foo
      _e_bar = _ev.bar
      for (_it_i_foo,_it_i_bar) in zip(_e_foo,_e_bar):
        _hfill ((_it_i_foo),(_it_i_bar))
    <BLANKLINE>
    True
    >>> print pydraw.last_hist.GetName(), pydraw.last_hist.GetTitle()
    foo$i:bar$i tt.foo$i:bar$i
    >>> print pydraw.last_hist.GetXaxis().GetNbins()
    50
    >>> print pydraw.last_hist.GetYaxis().GetNbins()
    50
    >>> draw("tt.foo$i:bar$i; 5 0 150 5 0 150")
    def _loopfunc(_i, _ev, _hfill = _hfill):
      _e_foo = _ev.foo
      _e_bar = _ev.bar
      for (_it_i_foo,_it_i_bar) in zip(_e_foo,_e_bar):
        _hfill ((_it_i_foo),(_it_i_bar))
    <BLANKLINE>
    True
    >>> print pydraw.last_hist.GetXaxis().GetNbins()
    5
    >>> print pydraw.last_hist.GetYaxis().GetNbins()
    5
    >>> print [pydraw.last_hist.GetBinContent(i) for i in range(1,26)]
    [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 15.0, 0.0]
    >>> draw("tt.fee; 10 0 10")
    def _loopfunc(_i, _ev, _hfill = _hfill):
      _e_fee = _ev.fee
      _hfill (_e_fee)
    <BLANKLINE>
    True
    >>> print [pydraw.last_hist.GetBinContent(i) for i in range(0,11)]
    [0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]
    """


def _regr_scan():
    """Regression tests for scan
    >>> import ROOT
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> tt = Loop_Test()
    >>> from PyAnalysisUtils.pydraw import scan
    >>> pydraw._debug = True
    >>> scan("asdasd")
    Missing period in tuple specification.
    False
    >>> scan("tt.foo$i:bar$i")
    def _loopfunc(_i, _ev, _print = _print):
      _e_foo = _ev.foo
      _e_bar = _ev.bar
      for (_it_i_foo,_it_i_bar) in zip(_e_foo,_e_bar):
        _print (_i, (_it_i_foo),(_it_i_bar))
    <BLANKLINE>
         0        0        5
         0        1        6
         0        2        7
         0        3        8
         0        4        9
         1       10       15
         1       11       16
         1       12       17
         1       13       18
         1       14       19
         2       20       25
         2       21       26
         2       22       27
         2       23       28
         2       24       29
         3       30       35
         3       31       36
         3       32       37
         3       33       38
         3       34       39
         4       40       45
         4       41       46
         4       42       47
         4       43       48
         4       44       49
         5       50       55
         5       51       56
         5       52       57
         5       53       58
         5       54       59
         6       60       65
         6       61       66
         6       62       67
         6       63       68
         6       64       69
         7       70       75
         7       71       76
         7       72       77
         7       73       78
         7       74       79
         8       80       85
         8       81       86
         8       82       87
         8       83       88
         8       84       89
         9       90       95
         9       91       96
         9       92       97
         9       93       98
         9       94       99
    True
    >>> scan("tt[0:2].foo$i")
    def _loopfunc(_i, _ev, _print = _print):
      _e_foo = _ev.foo
      for _it_i_foo in _e_foo:
        _print (_i, (_it_i_foo))
    <BLANKLINE>
         0        0
         0        1
         0        2
         0        3
         0        4
         1       10
         1       11
         1       12
         1       13
         1       14
    True
    """

def _regr_loop():
    """Regression tests for loop
    >>> import ROOT
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> tt = Loop_Test()
    >>> from PyAnalysisUtils.pydraw import loop
    >>> pydraw._debug = True
    >>> loop("asdasd")
    Missing period in tuple specification.
    False
    >>> xsum[0]=0
    >>> loop("tt.sum(foo$i)")
    def _loopfunc(_i, _ev):
      _e_foo = _ev.foo
      for _it_i_foo in _e_foo:
        (sum(_it_i_foo),)
    <BLANKLINE>
    True
    >>> xsum
    [2350]
    >>> xsum[0]=0
    >>> loop("tt.sum(foo$i):sum(bar$i)")
    def _loopfunc(_i, _ev):
      _e_foo = _ev.foo
      _e_bar = _ev.bar
      for (_it_i_foo,_it_i_bar) in zip(_e_foo,_e_bar):
        (sum(_it_i_foo),sum(_it_i_bar),)
    <BLANKLINE>
    True
    >>> xsum
    [4950]
    """
    return


def _regr_cmd():
    """Regression tests for cmd
    >>> import ROOT
    >>> from PyAnalysisUtils import pydraw
    >>> pydraw._globals = globals()
    >>> tt = Loop_Test()
    >>> pydraw._debug = True
    >>> from PyAnalysisUtils.pydraw import cmd
    >>> cmd("d tt.foo$i")
    def _loopfunc(_i, _ev, _hfill = _hfill):
      _e_foo = _ev.foo
      for _it_i_foo in _e_foo:
        _hfill (_it_i_foo)
    <BLANKLINE>
    True
    >>> cmd("draw tt.foo$i")
    def _loopfunc(_i, _ev, _hfill = _hfill):
      _e_foo = _ev.foo
      for _it_i_foo in _e_foo:
        _hfill (_it_i_foo)
    <BLANKLINE>
    True
    >>> cmd("scan tt[:2].foo$i")
    def _loopfunc(_i, _ev, _print = _print):
      _e_foo = _ev.foo
      for _it_i_foo in _e_foo:
        _print (_i, (_it_i_foo))
    <BLANKLINE>
         0        0
         0        1
         0        2
         0        3
         0        4
         1       10
         1       11
         1       12
         1       13
         1       14
    True
    >>> cmd("loop tt.sum(foo$i)")
    def _loopfunc(_i, _ev):
      _e_foo = _ev.foo
      for _it_i_foo in _e_foo:
        (sum(_it_i_foo),)
    <BLANKLINE>
    True
    >>> cmd("x")
    False
    >>> cmd("d sodfj")
    Missing period in tuple specification.
    False
    >>> cmd("sdfkl sodfj")
    False
    """
    return


def _exc_rethrow(exc, value, traceb):
    raise value
    return


def _regr_cmdhook():
    """Regression tests for cmdhook
    >>> import ROOT
    >>> from PyAnalysisUtils import pydraw
    >>> import sys
    >>> pydraw._globals = globals()
    >>> tt = Loop_Test()
    >>> pydraw.cmdhook()
    >>> #Note that we can't really check the trapping of syntax errors.
    >>> #Everything we do here is executed in a try...except in doctest,
    >>> #so we'll never get to the outer exception handler.
    >>> #We'll add a quick test for trapping syntax errors later.
    >>> try:
    ...   eval('scan tt[:2].foo$i')
    ... except:
    ...   pydraw._excepthook (*sys.exc_info())
    def _loopfunc(_i, _ev, _print = _print):
      _e_foo = _ev.foo
      for _it_i_foo in _e_foo:
        _print (_i, (_it_i_foo))
    <BLANKLINE>
         0        0
         0        1
         0        2
         0        3
         0        4
         1       10
         1       11
         1       12
         1       13
         1       14
    >>> pydraw.cmdhook()
    >>> try:
    ...   eval('scan tt[:2].foo$i')
    ... except:
    ...   pydraw._excepthook (*sys.exc_info())
    def _loopfunc(_i, _ev, _print = _print):
      _e_foo = _ev.foo
      for _it_i_foo in _e_foo:
        _print (_i, (_it_i_foo))
    <BLANKLINE>
         0        0
         0        1
         0        2
         0        3
         0        4
         1       10
         1       11
         1       12
         1       13
         1       14
    >>> 1+1
    2
    >>> ehook_save = pydraw._orig_ehook
    >>> pydraw._orig_ehook =  _exc_rethrow
    >>> try:
    ...   eval('aldj lasjd d')
    ... except:
    ...   pydraw._excepthook (*sys.exc_info())
    Traceback (most recent call last):
        ...
    SyntaxError: invalid syntax
    >>> pydraw._orig_ehook = ehook_save
    """
    return


import ROOT
ROOT.gROOT.SetBatch (True)

h1=ROOT.TH1F('hx','hx', 10, 0, 10)
h1.Draw()
p=ROOT.gPad
p.SetPad('pad', 'pad', 0, 0.25, 0.5, 1)
p.Range(0, 1,2,3)

from PyUtils import coverage
c = coverage.Coverage ('PyAnalysisUtils.pydraw')
print ''
c.doctest_cover ()

#from PyAnalysisUtils import pydraw
#pydraw._globals = globals()
#tt = Loop_Test()
#pydraw.cmdhook()
#pydraw._debug = False
#exec 'd tt.foo$i'


