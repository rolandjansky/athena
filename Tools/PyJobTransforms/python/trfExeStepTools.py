# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


executorStepSuffix = 'ExecutorStep'


def commonExecutorStepName(name):
    """Remove executor step suffix from executor name."""
    if executorStepSuffix in name:
        name, _sep, _tail = name.rpartition(executorStepSuffix)
    return name


def loadSplitConfig(config):
    """Load splitting configuration from file."""
    parts = config.split('.')
    if len(parts) < 2:
        raise ValueError('Splitting config should be of the form Package.Module.Function or Package.Function if defined in __init__.py')

    function = parts[-1]
    module = '.'.join(parts[:-1])

    from importlib import import_module
    loaded_module = import_module(module)
    function_def = getattr(loaded_module, function)
    return function_def()


def getTotalExecutorSteps(executor, argdict=None):
    """Get total executor steps from executor."""
    if not argdict:
        argdict = executor.conf.argdict
    if 'splitConfig' not in argdict:
        return 0

    splitConfig = argdict['splitConfig'].returnMyValue(exe=executor)
    if not splitConfig:
        return 0

    steps, fractions = loadSplitConfig(splitConfig)
    return steps


def getExecutorStepEventCounts(executor, argdict=None):
    """Get executor step event counts from executor config."""
    if not argdict:
        argdict = executor.conf.argdict
    if 'splitConfig' not in argdict:
        return []

    maxEvents = argdict['maxEvents'].returnMyValue(exe=executor)
    skipEvents = argdict['skipEvents'].returnMyValue(exe=executor)
    splitConfig = argdict['splitConfig'].returnMyValue(exe=executor)
    if not splitConfig:
        return []

    steps, fractions = loadSplitConfig(splitConfig)

    if sum(fractions) != 1:
        raise ValueError('Event fractions should total to 1!')

    counts = []
    for i in range(len(fractions) - 1):
        counts.append(round(maxEvents * fractions[i]))
    counts.append(maxEvents - sum(counts))

    sums = []
    for i in range(len(fractions)):
        sums.append(skipEvents + sum(counts[:i]))

    return counts, sums
