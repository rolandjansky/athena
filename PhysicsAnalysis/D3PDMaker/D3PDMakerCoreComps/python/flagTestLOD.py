# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#
# @file D3PDMakerCoreComps/python/flagTestLOD.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2012
# @brief Helper for making LOD functions to evaluate flags.
#


from AthenaCommon.JobProperties import JobProperty
from .D3PDObject                import DeferArg


def _make_fdict (flags):
    fdict = {}
    if not isinstance(flags, list):
        flags = [flags]
    for f in flags:
        for k, v in f.__dict__.items():
            if isinstance (v, JobProperty):
               fdict[k] = v()
    return fdict


def _eval_deferred (expr, fdict, hookargs, **kw):
    dd = fdict.copy()
    dd.update (hookargs)
    dd.update (kw)
    return eval (expr, globals(), dd)



def flagTestLOD (expr, flags, hook = None):
    """Helper for making LOD functions to evaluate flags.

    Numerous parts of the D3PD configuration use arbitrary global flags
    to control which blocks are included.  This is bad because there
    can be interference when multiple tuples are configured in the
    same job.

    Better is to use the level-of-detail mechanism to do this,
    passing in flags as extra arguments to the D3PDObject.

    This helper is to ease this conversion, while still allowing
    backwards compatibility with flag-based configuration.

    flagTestLOD will return a level-of-detail function that evaluates
    the provided expression string EXP.  This will be evaluated
    in an environment that includes names for all flags in the flags
    class(es) FLAGS, merged with the value of the hookargs.
    The requested level of detail is also available as `reqlev'.
    Thus, if you have something like:

        myobj = make_SGDataVector_D3PDObject ('mycont', 'mykey', 'my_', 'myob',
                                              allow_args = ['myLevel'])
        myobj.defineBlock (flagTestLOD('myLevel>2', myFlags),
                           'myblock', filler_tool)

    then myblock will be included in the tuple if the property
    myFlags.myLevel() is greater than two.  However, this can be overridden
    by passing an argument to the D3PDObject:

        alg += myobj (level, myLevel=4)

    Explicit inclusion / exclusion also overrides this determination.

    Flags are evaluated at the point where flagTestLOD is called.

    If HOOK is supplied, then it is called if the LOD test passes.
    It is passed as arguments two dictionaries: the block filler tool
    arguments and the hook arguments.

    Examples:
    >>> from AthenaCommon.JobProperties import JobPropertyContainer, JobProperty
    >>> f1=JobPropertyContainer ('f1')
    >>> f2=JobPropertyContainer ('f2')
    >>> class flag1(JobProperty):
    ...   statusOn = True
    ...   allowedTypes=['bool']
    ...   StoredValue = True
    >>> class flag2(JobProperty):
    ...   statusOn = True
    ...   allowedTypes=['bool']
    ...   StoredValue = False
    >>> f1.add_JobProperty(flag1)
    >>> f2.add_JobProperty(flag2)
    >>> lod = flagTestLOD ('flag1', f1)
    >>> lod(2, {}, {})
    True
    >>> lod(2, {}, {'flag1': False})
    False
    >>> lod(-999, {}, {})
    False
    >>> lod(999, {}, {'flag1': False})
    True
    >>> lod = flagTestLOD ('flag2 or reqlev>4', [f1,f2])
    >>> lod(2, {}, {})
    False
    >>> lod(10, {}, {})
    True
    >>> def hook(*args):
    ...    print(args)
    >>> lod = flagTestLOD ('flag2 or reqlev>4', [f1,f2], hook)
    >>> lod(2, {}, {})
    False
    >>> lod(10, {'a':1}, {'b':2})
    ({'a': 1}, {'b': 2})
    True
"""

    fdict = _make_fdict (flags)
    def flagTestLODFunc (reqlev, args, hookargs):
        if reqlev < 0:   return False # explicit exclusion
        if reqlev > 900:
            ret = True  # explicit inclusion
        else:
            ret = _eval_deferred (expr, fdict, hookargs, reqlev=reqlev)
        if ret and hook:
            hook (args, hookargs)
        return ret
    return flagTestLODFunc


def deferFlag (expr, flags, gdict={}):
    """Helper for making deferred arguments that evaluate flags.

    Numerous parts of the D3PD configuration use arbitrary global flags
    to control how tuples are configured.  This is bad because there
    can be interference when multiple tuples are configured in the
    same job.

    Better is to pass the flags in as arguments to the D3PDObject and pass
    them to blocks using the DeferArg mechanism.

    This helper is to ease this conversion, while still allowing
    backwards compatibility with flag-based configuration.

    deferFlag will return a DeferArg object that evaluates
    the provided expression string EXP.  This will be evaluated
    in an environment that includes names for all flags in the flags
    class(es) FLAGS, merged with the value of the hookargs.

    So if you do something like

      myobj = make_SGDataVector_D3PDObject ('mycont', 'mykey', 'my_', 'myob',
                                            allow_args = ['doExtra'])
      myobj.defineBlock (0, 'myblock', filler_tool,
                         DoExtra = deferFlag ('doExtra', myFlags))

    then the DoExtra property will be set from the property myFlags.doExtra()
    unless it's overridden by passing an argument to the D3PDObject:

      alg += myobj (level, doExtra = True)

    Flags are evaluated at the point where deferFlag is called.

    GDICT gives extra names to be made available when evaluating EXPR.


    Examples:
    >>> from AthenaCommon.JobProperties import JobPropertyContainer, JobProperty
    >>> f3=JobPropertyContainer ('f3')
    >>> class flag3(JobProperty):
    ...   statusOn = True
    ...   allowedTypes=['bool']
    ...   StoredValue = True
    >>> f3.add_JobProperty(flag3)
    >>> arg = deferFlag ('flag3', f3)
    >>> arg()
    True
    >>> arg({'flag3':False})
    False
    >>> arg = deferFlag ('foo', f3, {'foo' : 2})
    >>> arg()
    2
"""
    fdict = _make_fdict (flags)
    return DeferArg ('_eval_deferred(expr, fdict, locals(), **_gdict)',
                     expr = expr,
                     fdict = fdict,
                     _gdict = gdict,
                     _eval_deferred = _eval_deferred)
