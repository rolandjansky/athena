"""Define functions for event bookkeeping configuration using ComponentAccumulator

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg


def BookkeeperDumperToolCfg(flags):
    """BookkeeperDumperTool configuration"""
    return MetaDataSvcCfg(flags, toolNames=['BookkeeperDumperTool'])
