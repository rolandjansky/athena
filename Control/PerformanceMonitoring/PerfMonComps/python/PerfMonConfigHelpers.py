# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def setPerfmonFlagsFromRunArgs(ConfigFlags, runArgs):
    """ A helper function to set perfmon flags from runArgs."""

    if hasattr(runArgs, 'perfmon') and runArgs.perfmon != 'none':
        if runArgs.perfmon == 'fastmonmt':
            ConfigFlags.PerfMon.doFastMonMT = True
        elif runArgs.perfmon == 'fullmonmt':
            ConfigFlags.PerfMon.doFullMonMT = True
        else:
            raise RuntimeError(f"Unknown perfmon type: {runArgs.perfmon}")
        ConfigFlags.PerfMon.OutputJSON = f"perfmonmt_{runArgs.trfSubstepName}.json"
