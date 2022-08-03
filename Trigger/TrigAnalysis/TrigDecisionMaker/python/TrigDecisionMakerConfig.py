# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format
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
    if "EventInfo#EventInfo" not in flags.Input.TypedCollections:
        decCnv.EventInfoKey=""

    acc.addEventAlgo(decCnv)

    # TrigNavigationCnvAlg runs on the original HLTResult in the bytestream as it used
    # to be in serial athena, i.e. it does not run on the modified HLTResult created by
    # TrigBSExtraction. See also ATLASRECTS-6453.
    from SGComps.AddressRemappingConfig import InputRenameCfg

    # Even for Run-1, we only convert the EF result:
    aodKey = "HLTResult_HLT" if doHLT else "HLTResult_EF"

    acc.merge(InputRenameCfg("HLT::HLTResult", aodKey, aodKey+"_BS"))
    acc.addEventAlgo( CompFactory.xAODMaker.TrigNavigationCnvAlg('TrigNavigationCnvAlg',
                                                                 AODKey = aodKey,
                                                                 xAODKey = "TrigNavigation") )

    return acc

def Run3DecisionMakerCfg(flags):
    acc = ComponentAccumulator()
    tdm = CompFactory.TrigDec.TrigDecisionMakerMT()
    if flags.Input.Format is not Format.BS:
        # Construct trigger bits from HLTNav_summary instead of reading from BS
        tdm.BitsMakerTool = CompFactory.TriggerBitsMakerTool()
    acc.addEventAlgo( tdm )

    # Validate that the output of the TrigDecisionMakerMT is sensible.
    tdmv = CompFactory.TrigDec.TrigDecisionMakerValidator()
    tdmv.doL1 = flags.Trigger.L1.doCTP
    tdmv.doHLT = True
    tdmv.samplingFrequency = 1
    tdmv.errorOnFailure = False
    tdmv.EDMVersion = flags.Trigger.EDMVersion
    from TrigDecisionTool.TrigDecisionToolConfig import TrigDecisionToolCfg, getRun3NavigationContainerFromInput
    tdmv.TrigDecisionTool = acc.getPrimaryAndMerge(TrigDecisionToolCfg(flags))
    tdmv.NavigationKey = getRun3NavigationContainerFromInput(flags)
    acc.addEventAlgo( tdmv )

    return acc
