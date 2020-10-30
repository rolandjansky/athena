"""Define functions for event bookkeeping configuration using ComponentAccumulator

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg


def BookkeeperToolCfg(flags, base_name='CutBookkeepers'):
    """BookkeeperTool config"""
    tool = CompFactory.BookkeeperTool(base_name + 'Tool',
                                      InputCollName = base_name,
                                      OutputCollName = base_name)
    return MetaDataSvcCfg(flags, tools=[tool])


def BookkeeperDumperToolCfg(flags):
    """BookkeeperDumperTool configuration"""
    return MetaDataSvcCfg(flags, toolNames=['BookkeeperDumperTool'])


def CutFlowSvcCfg(flags):
    """CutFlowSvc configuration"""
    acc = ComponentAccumulator()

    # Determine current input stream name
    # inputStreamName = GetCurrentStreamName( msg=msg )

    acc.addService(CompFactory.CutFlowSvc(Configured=True))
    # svcMgr.CutFlowSvc.InputStream   = inputStreamName

    acc.merge(BookkeeperToolCfg(flags))
    # TODO: different sequence?
    acc.addEventAlgo(CompFactory.AllExecutedEventsCounterAlg())

    return acc


def CutFlowOutputList(flags, base_name='CutBookkeepers'):
    """CutFlow output metadata list"""
    return [
        'xAOD::CutBookkeeperContainer#' + base_name,
        'xAOD::CutBookkeeperAuxContainer#' + base_name + 'Aux.*',
        'xAOD::CutBookkeeperContainer#Incomplete' + base_name,
        'xAOD::CutBookkeeperAuxContainer#Incomplete' + base_name + 'Aux.*'
    ]
