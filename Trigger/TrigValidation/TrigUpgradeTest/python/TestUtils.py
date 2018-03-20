#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

def writeEmulationFiles(data):
    """ Writes emulation files. key in the dict is a file name (+.dat), list which is value of each dict el is enetered into the file, one el. per line"""
    for name, d in data.iteritems():
        with open(name+".dat", "w") as f:
            for event in d:
                f.write(event)
                f.write("\n")

# Testing menu used in the L1 decoders
class MenuTest:
    CTPToChainMapping = ["0:HLT_e3_etcut",
                         "0:HLT_e5_etcut",
                         "0:HLT_g5_etcut",
                         "1:HLT_e7_etcut",
                         "23:HLT_2e3_etcut",
                         "23:HLT_e3e5_etcut",
                         "15:HLT_mu6",
                         "33:HLT_2mu6",
                         "15:HLT_mu6idperf",
                         "42:HLT_e15mu4",
                         "68:HLT_xe10", # the item is L1_XE10
                         "152:HLT_xs20",
                         "50:HLT_te15", # the seed is L1_TE15.0ETA24
                         ]

    EMThresholdToChainMapping = ["EM3 : HLT_e3_etcut",
                                 "EM3 : HLT_e5_etcut",
                                 "EM3 : HLT_2e3_etcut",
                                 "EM3 : HLT_e3e5_etcut",
                                 "EM5 : HLT_e3e5_etcut",
                                 "EM3 : HLT_g5_etcut",
                                 "EM7 : HLT_e7_etcut",
                                 "EM15 : HLT_e15mu4_etcut"]

    MUThresholdToChainMapping = ["MU6 : HLT_mu6",
                                 "MU6 : HLT_mu6idperf",
                                 "MU4 : HLT_e15mu4"]


    METThresholdToChainMapping = ["UNUSED : HLT_xe10",
                                  "UNUSED : HLT_xs20",
                                  "UNUSED : HLT_te15"]
def applyMenu(l1decoder ):
    l1decoder.ctpUnpacker.CTPToChainMapping = MenuTest.CTPToChainMapping
    if "MURoIsUnpackingTool" in  l1decoder.roiUnpackers:
        l1decoder.roiUnpackers["MURoIsUnpackingTool"].ThresholdToChainMapping = MenuTest.MUThresholdToChainMapping
        
    l1decoder.roiUnpackers["EMRoIsUnpackingTool"].ThresholdToChainMapping = MenuTest.EMThresholdToChainMapping
    l1decoder.roiUnpackers["METRoIsUnpackingTool"].ThresholdToChainMapping = MenuTest.METThresholdToChainMapping
        
    
# L1Decoder for bytestream
from L1Decoder.L1DecoderConf import L1Decoder
class L1DecoderTest(L1Decoder) :
    def __init__(self, name='L1DecoderTest', *args, **kwargs):
        super(L1DecoderTest, self).__init__(name, *args, **kwargs)

        from TriggerJobOpts.TriggerFlags import TriggerFlags
        from L1Decoder.L1DecoderMonitoring import CTPUnpackingMonitoring, RoIsUnpackingMonitoring
        from L1Decoder.L1DecoderConf import CTPUnpackingTool, EMRoIsUnpackingTool, MURoIsUnpackingTool, METRoIsUnpackingTool

        # CTP unpacker

        ctpUnpacker = CTPUnpackingTool(OutputLevel = self.OutputLevel,
                                       ForceEnableAllChains = True)
        ctpUnpacker.MonTool = CTPUnpackingMonitoring(512, 200)
        # Hard-coded CTP IDs from v7 menu
        ctpUnpacker.CTPToChainMapping = MenuTest.CTPToChainMapping
        self.ctpUnpacker = ctpUnpacker

        # EM unpacker
        if TriggerFlags.doID() or TriggerFlags.doCalo():
            emUnpacker = EMRoIsUnpackingTool(OutputLevel = self.OutputLevel,
                                             Decisions = "EMRoIDecisions",
                                             OutputTrigRoIs = "EMRoIs")
            emUnpacker.ThresholdToChainMapping = MenuTest.EMThresholdToChainMapping
            emUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 )
            self.roiUnpackers += [emUnpacker]

            metUnpacker = METRoIsUnpackingTool(OutputLevel = self.OutputLevel,
                                               Decisions = "METRoIDecisions",
                                               OutputTrigRoI = "METRoI")
            metUnpacker.ThresholdToChainMapping = MenuTest.METThresholdToChainMapping            
            self.roiUnpackers += [metUnpacker]

        # MU unpacker
        if TriggerFlags.doMuon():
            muUnpacker = MURoIsUnpackingTool(OutputLevel = self.OutputLevel,
                                             Decisions = "MURoIDecisions",
                                             OutputTrigRoIs = "MURoIs")
            muUnpacker.ThresholdToChainMapping = MenuTest.MUThresholdToChainMapping
            muUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="MU", maxCount=20 )
            self.roiUnpackers += [muUnpacker]

        self.Chains = "HLTChainsResult"


# L1 emulation for RDO
class L1EmulationTest(L1Decoder) :
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

        from TrigUpgradeTest.TestUtils import writeEmulationFiles
        writeEmulationFiles(data)
        ctpUnpacker = CTPUnpackingEmulationTool(OutputLevel = self.OutputLevel,
                                                ForceEnableAllChains = True)
        self.ctpUnpacker = ctpUnpacker
        self += ctpUnpacker
        # Hard-coded CTP IDs from v7 menu
        self.ctpUnpacker.CTPToChainMapping = MenuTest.CTPToChainMapping

        # EM unpacker
        if TriggerFlags.doID() or TriggerFlags.doCalo():
            emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool",
                                                    Decisions = "EMRoIDecisions",
                                                    OutputTrigRoIs = "EMRoIs",
                                                    OutputLevel = self.OutputLevel,
                                                    ThresholdToChainMapping = MenuTest.EMThresholdToChainMapping)
            self.roiUnpackers += [emUnpacker]
            print emUnpacker


        # MU unpacker
        if TriggerFlags.doMuon():
            muUnpacker = RoIsUnpackingEmulationTool("MURoIsUnpackingTool",
                                                    Decisions = "MURoIDecisions",
                                                    OutputTrigRoIs = "MURoIs",
                                                    OutputLevel=self.OutputLevel,
                                                    ThresholdToChainMapping = MenuTest.MUThresholdToChainMapping)
            self.roiUnpackers += [muUnpacker]

        self.Chains="HLTChainsResult"

if __name__ == "__main__":
    from AthenaCommon.Constants import DEBUG
    real = L1DecoderTest(OutputLevel=DEBUG)
    
