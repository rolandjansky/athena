# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


def executeFromFragment(fragment_string, flags, cfg=None):
    """Execute a function from a pre/post include fragment."""
    parts = fragment_string.split('.')
    if len(parts) < 2:
        raise ValueError('Pre/post include should be of the form Package.Module.Function or Package.Function if defined in __init__.py')

    function = parts[-1]
    module = '.'.join(parts[:-1])

    from importlib import import_module
    loaded_module = import_module(module)
    function_def = getattr(loaded_module, function)

    if not cfg:
        function_def(flags)
        return

    from inspect import getfullargspec
    argspec = getfullargspec(function_def)
    print(argspec)
    argCount = len(argspec.args)
    defaultsCount = len(argspec.defaults) if argspec.defaults else 0
    if argCount - defaultsCount == 1:
        cfg.merge(function_def(flags))
    else:
        function_def(flags, cfg)


def processPreExec(runArgs, ConfigFlags):
    """Process preExec from runtime arguments."""
    if hasattr(runArgs, 'preExec') and runArgs.preExec and runArgs.preExec != 'NONE':
        for cmd in runArgs.preExec:
            exec(cmd)


def processPostExec(runArgs, ConfigFlags, cfg):
    """Process postExec from runtime arguments."""
    if not ConfigFlags.locked():
        raise RuntimeError('Running a postExec before locking ConfigFlags')

    if hasattr(runArgs, 'postExec') and runArgs.postExec and runArgs.postExec != 'NONE':
        for cmd in runArgs.postExec:
            exec(cmd)


def processPreInclude(runArgs, ConfigFlags):
    """Process preInclude from runtime arguments."""
    if hasattr(runArgs, 'preInclude') and runArgs.preInclude and runArgs.preInclude != 'NONE':
        for fragment in runArgs.preInclude:
            executeFromFragment(fragment, ConfigFlags)


def processPostInclude(runArgs, ConfigFlags, cfg):
    """Process postInclude from runtime arguments."""
    if not ConfigFlags.locked():
        raise RuntimeError('Running a postInclude before locking ConfigFlags')

    if hasattr(runArgs, 'postInclude') and runArgs.postInclude and runArgs.postInclude != 'NONE':
        for fragment in runArgs.postInclude:
            executeFromFragment(fragment, ConfigFlags, cfg)


def UseFrontier(flags):
    """Switch database to using FRONTIER, but with a fallback
    to DBRelease if FRONTIER_SERVER is undefined (e.g., on HPC)

    Move from RecJobTransforms to PyJobTransforms to enable use
    in simulation ATN and KV jobs
    """
    from os import environ
    from AthenaCommon.Logging import logging
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory

    log = logging.getLogger('UseFrontier')

    cfg = ComponentAccumulator()
    if environ.get('FRONTIER_SERVER'):
        log.info('Enabling FRONTIER DB access')
        cfg.addService(CompFactory.DBReplicaSvc(COOLSQLiteVetoPattern='DBRelease'))
    else:
        log.info('Using default DB access')

    return cfg


def DumpPickle(flags, cfg):
    """Dump the pickle file for the current configuration"""
    with open("Configuration.pkl", "wb") as f:
        cfg.store(f)
