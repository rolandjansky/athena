# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger( __name__ )

from TriggerMenuMT.HLT.Menu.MenuComponents import EmptyMenuSequence
from TriggerMenuMT.HLT.Config.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLT.MinBias.MinBiasMenuSequences import MinBiasSPSequence, MinBiasTrkSequence, MinBiasMbtsSequence, MinBiasZVertexFinderSequenceCfg
from TriggerMenuMT.HLT.MinBias.ALFAMenuSequences import ALFAPerfSequence
from TriggerMenuMT.HLT.MinBias.AFPMenuSequence import AFPTrkRecoSequence, AFPTrkRecoHypoSequence
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------
def MinBiasSPSequenceCfg(flags):
    return MinBiasSPSequence()

def MinBiasTrkSequenceCfg(flags):
    return MinBiasTrkSequence()


def MinBiasMbtsSequenceCfg(flags):
    return MinBiasMbtsSequence()

def MinBiasMbtsEmptySequenceCfg(flags):
    return EmptyMenuSequence("EmptyMbts")

def MinBiasZFindEmptySequenceCfg(flags):
    return EmptyMenuSequence("EmptyZFind")

def AFPrecoSequenceCfg(flags):
    return AFPTrkRecoSequence()

def AFPrecoHypoSequenceCfg(flags):
    return AFPTrkRecoHypoSequence()

def TrigAFPDijetComboHypoToolCfg(chainDict):
    from TrigAFPHypo.TrigAFPHypoConf import TrigAFPDijetComboHypoTool
    name = chainDict['chainName']
    tool = TrigAFPDijetComboHypoTool(name)

    monTool = GenericMonitoringTool("MonTool_"+name)
    monTool.Histograms = [defineHistogram('DijetMass', type='TH1F', path='EXPERT', title="Dijet mass", xbins=100, xmin=0, xmax=2000)]
    monTool.defineHistogram('DijetRapidity', type='TH1F', path='EXPERT', title="Dijet rapidity", xbins=100, xmin=-5, xmax=5)

    monTool.defineHistogram('XiJet1', type='TH1F', path='EXPERT', title="Jet 1 xi", xbins=100, xmin=0, xmax=1)
    monTool.defineHistogram('XiJet2', type='TH1F', path='EXPERT', title="Jet 2 x1", xbins=100, xmin=0, xmax=1)

    monTool.defineHistogram('PredictProtonAEnergy', type='TH1F', path='EXPERT', title="Predicted proton energy A", xbins=100, xmin=0, xmax=10000)
    monTool.defineHistogram('PredictProtonCEnergy', type='TH1F', path='EXPERT', title="Predicted proton energy C", xbins=100, xmin=0, xmax=10000)

    monTool.defineHistogram('SideA_predictX', type='TH1F', path='EXPERT', title="Predicted X side A", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideA_predictY', type='TH1F', path='EXPERT', title="Predicted Y side A", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideC_predictX', type='TH1F', path='EXPERT', title="Predicted X side C", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideC_predictY', type='TH1F', path='EXPERT', title="Predicted Y side C", xbins=100, xmin=-100, xmax=100)

    monTool.defineHistogram('XDiff', type='TH1F', path='EXPERT', title="X difference", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('YDiff', type='TH1F', path='EXPERT', title="Y difference", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('distance', type='TH1F', path='EXPERT', title="distance", xbins=100, xmin=0, xmax=50)

    monTool.defineHistogram('SideA_trackX', type='TH1F', path='EXPERT', title="Track X side A", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideA_trackY', type='TH1F', path='EXPERT', title="Track Y side A", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideA_diffX', type='TH1F', path='EXPERT', title="Track X diff side A", xbins=100, xmin=-50, xmax=50)
    monTool.defineHistogram('SideA_diffY', type='TH1F', path='EXPERT', title="Track Y diff side A", xbins=100, xmin=-50, xmax=50)

    monTool.defineHistogram('SideC_trackX', type='TH1F', path='EXPERT', title="Track X side C", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideC_trackY', type='TH1F', path='EXPERT', title="Track Y side C", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideC_diffX', type='TH1F', path='EXPERT', title="Track X diff side C", xbins=100, xmin=-50, xmax=50)
    monTool.defineHistogram('SideC_diffY', type='TH1F', path='EXPERT', title="Track Y diff side C", xbins=100, xmin=-50, xmax=50)

    monTool.HistPath = 'AFPComboHypo/'+tool.getName()
    tool.MonTool = monTool
    return tool

def ALFAPerfSequenceCfg(flags):
    return ALFAPerfSequence()

def MinBiasZVertexFinderCfg(flags):
    #TODO we can do that inside of the getStep ... next interation
    from ..Menu.MenuComponents import menuSequenceCAToGlobalWrapper
    return menuSequenceCAToGlobalWrapper(MinBiasZVertexFinderSequenceCfg, flags)

class MinBiasChainConfig(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChainImpl(self):
        log.debug("Assembling chain for %s", self.chainName)
        steps = []

        if "mbts" == self.chainPart['recoAlg'][0] or "mbts" in self.chainName:
            steps.append(self.getMinBiasMbtsStep())
        elif "afprec" == self.chainPart['recoAlg'][0]:
            steps.append(self.getAFPRecoStep())
        else:
            steps.append(self.getMinBiasEmptyMbtsStep())

        if self.chainPart['recoAlg'][0] in ['sp', 'sptrk', 'hmt', 'excl']:
            steps.append(self.getMinBiasSpStep())

        if self.chainPart['recoAlg'][0] in ['sptrk', 'hmt', 'excl']:
            if self.chainPart['pileupInfo']:
                steps.append(self.getMinBiasZFindStep())
            else:
                steps.append(self.getMinBiasEmptyZFindStep())

            steps.append(self.getMinBiasTrkStep())

        if "_alfaperf" in self.chainName:
            steps.append(self.getALFAPerfStep())

        return self.buildChain(steps)

    def getMinBiasMbtsStep(self):
        return self.getStep(1, 'Mbts',[MinBiasMbtsSequenceCfg])

    def getMinBiasEmptyMbtsStep(self):
        return self.getStep(1,'EmptyMbts',[MinBiasMbtsEmptySequenceCfg])

    def getMinBiasSpStep(self):
        return self.getStep(2,'SPCount',[MinBiasSPSequenceCfg])

    def getMinBiasZFindStep(self):
        return self.getStep(3,'ZFind',[MinBiasZVertexFinderCfg])

    def getMinBiasEmptyZFindStep(self):
        return self.getStep(3,'EmptyZFind',[MinBiasZFindEmptySequenceCfg])

    def getMinBiasTrkStep(self):
        return self.getStep(4,'TrkCount',[MinBiasTrkSequenceCfg])

    def getAFPRecoStep(self):
         return self.getStep(1,'AFPReco',[AFPrecoSequenceCfg])

    def getAFPRecoHypoStep(self):
         return self.getStep(1,'AFPRecoHypo',[AFPrecoHypoSequenceCfg])

    def getALFAPerfStep(self):
        return self.getStep(1,'ALFAPerf',[ALFAPerfSequenceCfg])
