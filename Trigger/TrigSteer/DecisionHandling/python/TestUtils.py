#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from __future__ import print_function
import six

def writeEmulationFiles(data):
    """ Writes emulation files. key in the dict is a file name (+.dat), list which is value of each dict el is enetered into the file, one el. per line"""
    for name, d in six.iteritems (data):
        with open(name+".dat", "w") as f:
            for event in d:
                f.write(event)
                f.write("\n")

# Testing menu used in the L1 decoders
class MenuTest(object):
    pass



from L1Decoder.L1DecoderConf import L1Decoder
# L1 emulation for RDO
class L1EmulationTest(L1Decoder):
    def __init__(self, name='L1EmulationTest', *args, **kwargs):
        super(L1EmulationTest, self).__init__(name, *args, **kwargs)

        from TriggerJobOpts.TriggerFlags import TriggerFlags
        from L1Decoder.L1DecoderConf import CTPUnpackingEmulationTool, RoIsUnpackingEmulationTool

        self.RoIBResult = ""

        data = {}
        data['CTPEmulation'] = ['HLT_e3 HLT_g5 HLT_e7 HLT_2e3 HLT_mu6 HLT_2mu6 HLT_mu6idperf HLT_e15mu4',
                                'HLT_e3 HLT_g5 HLT_mu6 HLT_2mu6 HLT_mu6idperf HLT_e15mu4',
                                'HLT_e3 HLT_g5 HLT_e7 HLT_2e3 HLT_mu6 HLT_2mu6',
                                'HLT_mu6 HLT_2mu6 HLT_mu6idperf HLT_e15mu4', ]   # just to see some change
        data['RoIEmulation'] = ['1.3,2.9,2704088841,EM3,EM7; 1.2,3.1,2972524297,EM3,EM7,EM10,EM15; -3.2,-2.0,3103727387,MU0,MU4,MU6,MU8',
                                '1.2,1.9,2733969453,MU0,MU4,MU6,MU8,MU10;2.2,1.0,2733969453,MU0,MU4,MU6',
                                '-3.2,3.0,2704088841,MU0,MU4,MU6,MU8;3.0,1.6,2972524297,MU0,MU4',
                                '1.3,1.9,3103727387,MU0,MU10;1.2,2.6,2733969453,MU6;-1.1,2.6,2972524297,MU6; -1.2,2.6,2704088842,MU20']


        writeEmulationFiles(data)
        ctpUnpacker = CTPUnpackingEmulationTool(OutputLevel = self.getDefaultProperty("OutputLevel"),
                                                ForceEnableAllChains = True)
        self.ctpUnpacker = ctpUnpacker
        self += ctpUnpacker

        from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection

        # EM unpacker
        if TriggerFlags.doID() or TriggerFlags.doCalo():
            emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool",
                                                    Decisions = "EMRoIDecisions",
                                                    OutputTrigRoIs = mapThresholdToL1RoICollection("EM"),
                                                    OutputLevel = self.getDefaultProperty("OutputLevel"))
            self.roiUnpackers += [emUnpacker]
            print (emUnpacker)


        # MU unpacker
        if TriggerFlags.doMuon():
            muUnpacker = RoIsUnpackingEmulationTool("MURoIsUnpackingTool",
                                                    Decisions = "MURoIDecisions",
                                                    OutputTrigRoIs = mapThresholdToL1RoICollection("MU"),
                                                    OutputLevel=self.getDefaultProperty("OutputLevel"))
            self.roiUnpackers += [muUnpacker]

        self.L1DecoderSummaryKey = "L1DecoderSummary"




chainsCounter = 0

def makeChain( name, L1Thresholds, ChainSteps, Streams="physics:Main", Groups=["RATE:TestRateGroup", "BW:TestBW"] ):
    """
    In addition to making the chain object fills the flags that are used to generate MnuCOnfig JSON file
    """

    from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp
    prop = ChainProp( name=name,  l1SeedThresholds=L1Thresholds, groups=Groups )

    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT

    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    chainDict = dictFromChainName( prop )
    global chainsCounter
    chainDict["chainCounter"] = chainsCounter
    chainsCounter += 1

    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain
    chainConfig = Chain( name=name, L1Thresholds=L1Thresholds, ChainSteps=ChainSteps )

    TriggerConfigHLT.registerChain( chainDict, chainConfig )

    return chainConfig

if __name__ == "__main__":
    from AthenaCommon.Constants import DEBUG
    real = L1EmulationTest(OutputLevel=DEBUG)
