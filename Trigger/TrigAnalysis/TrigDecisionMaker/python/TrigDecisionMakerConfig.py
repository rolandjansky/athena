# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging


class WritexAODTrigDecision ( object ) :
    def __init__(self):
        from AthenaCommon.AlgSequence import AlgSequence
        from xAODTriggerCnv.xAODTriggerCnvConf import (xAODMaker__TrigDecisionCnvAlg,
                                                       xAODMaker__TrigDecisionCnvTool,
                                                       xAODMaker__TrigNavigationCnvAlg)

        log = logging.getLogger( 'WritexAODTrigDecision' )
        TopAlg = AlgSequence()

        # In order for the conversion to work we need to setup the TrigDecisionTool such that it uses the old decision
        from AthenaCommon.AppMgr import ToolSvc, ServiceMgr as svcMgr
        if not hasattr(ToolSvc, 'TrigDecisionTool'):
            from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
            ToolSvc += Trig__TrigDecisionTool('TrigDecisionTool')

        ToolSvc.TrigDecisionTool.UseAODDecision = True
        ToolSvc.TrigDecisionTool.TrigDecisionKey = "TrigDecision"

        log.info('TrigDecisionTool setup to use old decision')

        # Setup the TrigDecision and Navigation converters:

        TopAlg += xAODMaker__TrigDecisionCnvAlg(
            xAODKey = "xTrigDecision",
            CnvTool = xAODMaker__TrigDecisionCnvTool(
                TrigConfigSvc = svcMgr.xAODConfigSvc)  # setup in TriggerConfigGetter
            )
        TopAlg += xAODMaker__TrigNavigationCnvAlg('TrigNavigationCnvAlg')

        log.info('TrigDecision writing to xAOD enabled')



class WriteTrigDecision ( object ) :
    def __init__ ( self, AODItemList = None, ESDItemList = None, doxAOD = True) :

        from AthenaCommon.AlgSequence import AlgSequence
        TopAlg = AlgSequence()

        self.TrigDecMaker    = CompFactory.TrigDecisionMaker('TrigDecMaker')

        TopAlg += self.TrigDecMaker

        if AODItemList is not None : self.addItemsToList(AODItemList)
        if ESDItemList is not None : self.addItemsToList(ESDItemList)

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( 'WriteTrigDecisionToAOD' )

        log.info('TrigDecision writing enabled')
        
        WritexAODTrigDecision()


    def addItemsToList(self, itemList) :
        itemList += [ "TrigDec::TrigDecision#*" ]


def Run1Run2DecisionMakerCfg(flags):
    """Configures HLTNavigation(tool) -> xAODNavigation and TrigDec::TrigDecision -> xAOD::TrigDecision """
    acc = ComponentAccumulator()
    doL1=True
    doL2=True
    doEF=True
    doHLT=True

    if 'HLT' not in flags.Trigger.availableRecoMetadata:
        doL2=False
        doEF=False
        doHLT=False
        
    if 'L1' not in flags.Trigger.availableRecoMetadata:
        doL1=False

    if flags.Trigger.EDMVersion == 1:  # Run-1 has L2 and EF result
        doHLT = False
    else:
        doL2 = False
        doEF = False

    decMaker = CompFactory.TrigDec.TrigDecisionMaker( 'TrigDecMaker', 
                                                      doL1 = doL1,
                                                      doL2 = doL2,
                                                      doEF = doEF,
                                                      doHLT = doHLT)
    acc.addEventAlgo(decMaker)


    from TrigDecisionTool.TrigDecisionToolConfig import TrigDecisionToolCfg
    acc.merge(TrigDecisionToolCfg(flags))

    from TrigConfxAOD.TrigConfxAODConfig import getxAODConfigSvc
    cnvTool = CompFactory.xAODMaker.TrigDecisionCnvTool('TrigDecisionCnvTool', 
                                                        TrigConfigSvc = acc.getPrimaryAndMerge( getxAODConfigSvc( flags )) )

    decCnv = CompFactory.xAODMaker.TrigDecisionCnvAlg(CnvTool = cnvTool)    

    acc.addEventAlgo(decCnv)

    acc.addEventAlgo( CompFactory.xAODMaker.TrigNavigationCnvAlg('TrigNavigationCnvAlg', 
                                                                 doL2 = doL2 and not doEF, 
                                                                 doEF = doEF,
                                                                 doHLT = doHLT))
    return acc

def Run3DecisionMakerCfg(flags):
    acc = ComponentAccumulator()
    tdm = CompFactory.TrigDec.TrigDecisionMakerMT()
    if not flags.Trigger.readBS:
        # Construct trigger bits from HLTNav_summary instead of reading from BS
        tdm.BitsMakerTool = CompFactory.TriggerBitsMakerTool()
    acc.addEventAlgo( tdm )
    return acc
