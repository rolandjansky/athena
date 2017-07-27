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


from L1Decoder.L1DecoderConf import L1Decoder
class L1DecoderTest(L1Decoder) :
    def __init__(self, name='L1DecoderTest', *args, **kwargs):
        super(L1DecoderTest, self).__init__(name, *args, **kwargs)

        from TriggerJobOpts.TriggerFlags import TriggerFlags        
        from L1Decoder.L1DecoderMonitoring import CTPUnpackingMonitoring, RoIsUnpackingMonitoring
        from L1Decoder.L1DecoderConf import CTPUnpackingTool, EMRoIsUnpackingTool, MURoIsUnpackingTool

        # CTP unpacker
        self.ctpUnpacker = CTPUnpackingTool(OutputLevel = self.OutputLevel, 
                                            ForceEnableAllChains = True)
        self.ctpUnpacker.MonTool = CTPUnpackingMonitoring(512, 200)
        # Hard-coded CTP IDs from v7 menu
        self.ctpUnpacker.CTPToChainMapping = ["0:HLT_e3",  
                                              "0:HLT_g5", 
                                              "1:HLT_e7", 
                                              "23:HLT_2e3", 
                                              "15:HLT_mu6", 
                                              "33:HLT_2mu6", 
                                              "15:HLT_mu6idperf", 
                                              "42:HLT_e15mu4"]

        # EM unpacker
        if TriggerFlags.doID() or TriggerFlags.doCalo():
            emUnpacker = EMRoIsUnpackingTool(OutputLevel = self.OutputLevel)
            emUnpacker.ThresholdToChainMapping = ["EM3 : HLT_e3", "EM3 : HLT_g5",  "EM7 : HLT_e7", "EM15 : HLT_e15mu4" ]
            emUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 )
            self.roiUnpackers.append(emUnpacker)

        # MU unpacker
        if TriggerFlags.doMuon():
            muUnpacker = MURoIsUnpackingTool(OutputLevel = self.OutputLevel)
            muUnpacker.ThresholdToChainMapping = ["MU6 : HLT_mu6", "MU6 : HLT_mu6idperf", "MU4 : HLT_e15mu4"] 
            muUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="MU", maxCount=20 )
            self.roiUnpackers.append(muUnpacker)

        self.Chains = "HLTChainsResult"
