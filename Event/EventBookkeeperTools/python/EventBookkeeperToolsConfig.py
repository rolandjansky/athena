"""Define functions for event bookkeeping configuration using ComponentAccumulator

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg


def BookkeeperToolCfg(flags, name='BookkeeperTool', output_name='CutBookkeepers'):
    """BookkeeperTool config"""
    tool = CompFactory.BookkeeperTool(name,
                                      InputCollName = output_name,
                                      OutputCollName = output_name)
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


def CutFlowOutputList(flags, output_name='CutBookkeepers'):
    """CutFlow output metadata list"""
    return [
        'xAOD::CutBookkeeperContainer#' + output_name + '*',
        'xAOD::CutBookkeeperAuxContainer#' + output_name + '*Aux.*',
        'xAOD::CutBookkeeperContainer#Incomplete' + output_name + '*',
        'xAOD::CutBookkeeperAuxContainer#Incomplete' + output_name + '*Aux.*'
    ]
